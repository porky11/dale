#ifndef DALE_CONTEXT
#define DALE_CONTEXT

#include "../Namespace/Namespace.h"
#include "../Linkage/Linkage.h"
#include "../Function/Function.h"
#include "../Variable/Variable.h"
#include "../Struct/Struct.h"
#include "../Enum/Enum.h"
#include "../ErrorReporter/ErrorReporter.h"
#include "../Node/Node.h"
#include "../NativeTypes/NativeTypes.h"
#include "../TypeRegister/TypeRegister.h"
#include "../STL/STL.h"

#include <vector>
#include <string>
#include <map>

namespace dale
{
struct NSNode
{
    Namespace *ns;
    std::map<std::string, NSNode *> children;
};

/*! Context

    A class for containing the details of a tree of namespaces.
    Provides for activating/deactivating namespaces (named and
    anonymous), searching for bindings throughout the tree and
    constructing LLVM types from 'internal' types, among other things.

    Within this class, an 'active' namespace is one into which new
    bindings will be put, whereas a 'used' namespace is one that is
    only used for binding resolution.  Note that all 'active'
    namespaces are also 'used' namespaces, for so long as they are
    active.

    None of the retrieval functions transfer ownership to the caller.
    Merging a context C into a context D transfers ownership of all of
    C's namespaces and bindings to D.

*/
class Context
{
public:
    /*! The native types for the context. Used primarily for type
     *  conversion. */
    NativeTypes *nt;
    /*! The type register for the context. */
    TypeRegister *tr;
    /*! The error reporter for this context. */
    ErrorReporter *er;
    /*! The node for the root namespace. */
    NSNode *namespaces;
    /*! The list of the currently-active nodes, with the most
     *  recently-activated node being at the back. */
    std::vector<NSNode *> active_ns_nodes;
    /*! The list of the currently-used nodes, with the most
     *  recently-used node being at the back. */
    std::vector<NSNode *> used_ns_nodes;
    /*! The current label-variable index for the context. */
    int lv_index;

    /*! The void constructor, intended solely for use by the
     *  deserialisation procedures.
     */
    Context();
    /*! The normal constructor.
     *  @param er The error reporter.
     *  @param nt The native types.
     *  @param tr The type register.
     *
     *  This does not take ownership of any of its arguments.
     */
    Context(ErrorReporter *er,
            NativeTypes *nt,
            TypeRegister *tr);
    ~Context();

    /*! Get the currently-active namespace.
     */
    Namespace *ns();
    /*! Pop active/used namespaces until the given namespace has been reached.
     *  @param ns The namespace that needs to be reached.
     */
    bool popUntilNamespace(Namespace *ns);

    /*! Activate the namespace with the given name.
     *  @param name The name of the namespace.
     *
     *  If the namespace does not already exist, then a new namespace
     *  object will be created and added to the tree.
     */
    bool activateNamespace(const char *name);
    /*! Deactivate the namespace with the given name.
     *  @param name The name of the namespace.
     *
     *  Reports errors and returns false if the namespace is not the
     *  most recently activated and used namespace.
     */
    bool deactivateNamespace(const char *name);

    /*! Activate a new anonymous namespace. */
    bool activateAnonymousNamespace();
    /*! Deactivate an anonymous namespace.
     *
     *  The only difference between this method and
     *  deactivateNamespace is that the latter checks that the name of
     *  the namespace is 'correct'; i.e., deactivateNamespace may be
     *  used directly, as the pair to a corresponding
     *  activateAnonymousNamespace call, by fetching the namespace's
     *  name manually after activation.
     */
    bool deactivateAnonymousNamespace();

    /*! Retrieve a namespace node by name.
     *  @param name The name of the namespace.
     *  @param ignore_last Whether to ignore the last segment of the
     *  argument name.
     *  @param ns_parts An optional vector for storing the argument
     *  name's segments.
     */
    NSNode *getNSNode(const char *name,
                      bool ignore_last,
                      std::vector<std::string> *ns_parts);
    /*! Retrieve a namespace node by name.
     *  @param name The name of the namespace.
     *  @param ignore_last Whether to ignore the last segment of the
     *  argument name.
     */
    NSNode *getNSNode(const char *name,
                      bool ignore_last);
    /*! Retrieve a namescape by name.
     *  @param name The name of the namespace.
     *  @param ignore_last Whether to ignore the last segment of the
     *  argument name.
     */
    Namespace *getNamespace(const char *name,
                            bool ignore_last);

    /*! Use a namespace.
     *  @param name The name of the namespace.
     *
     *  See also the (using-namespace) core form.
     */
    bool useNamespace(const char *name);
    /*! Remove the last-used namespace from the used list.
     */
    bool unuseNamespace();

    /*! Erase LLVM macros from all namespaces.
     */
    void eraseLLVMMacros();
    /*! Erase LLVM macros and compile-time functions from all
     * namespaces.
     */
    void eraseLLVMMacrosAndCTOFunctions();

    /*! Check whether an extern-c function with the given name exists.
     *  @param name The name of the function.
     */
    bool existsExternCFunction(const char *name);
    /*! Check whether a non-extern-c function with the given name
     *  exists.
     *  @param name The name of the function.
     */
    bool existsNonExternCFunction(const char *name);
    /*! Check whether there are multiple instances of the function
     *  with the given name.
     *  @param name The name of the function.
     */
    bool isOverloadedFunction(const char *name);

    /*! Get the function with the given name and arguments.
     *
     *  See Namespace::getFunction, the parameters for which are the
     *  same.  This iterates over the used namespaces, calling that
     *  method.
     */
    Function *getFunction(const char *name,
                          std::vector<Type *> *types,
                          Function **closest_fn,
                          bool is_macro,
                          std::vector<bool> *lvalues = NULL);
    /*! Get the variable with the given name.
     *
     *  See Namespace::getVariable.  As per getFunction, this iterates
     *  over the used namespaces, calling that method.
     */
    Variable *getVariable(const char *name);
    /*! Get the struct with the given name.
     */
    Struct *getStruct(const char *name);
    /*! Get the struct with the given name.
     */
    Struct *getStruct(const char *name,
                      std::vector<std::string> *namespaces);
    /*! Get the struct for the given type.
     */
    Struct *getStruct(Type *type);
    /*! Get the enum with the given name.
     */
    Enum *getEnum(const char *name);

    /*! Get the function names from all namespaces.
     *
     *  See Namespace::getFunctionNames.
     */
    void getFunctionNames(std::set<std::string> *names,
                          std::string *prefix);

    /*! Stores the struct's namespace names into the vector.
     *  @param name The name of the struct.
     *  @param namespaces A vector for the struct's namespaces.
     *
     *  If the struct name is already qualified, then namespaces is
     *  set based on that.  Otherwise, this attempts to find the given
     *  struct in the used namespaces, and sets namespaces
     *  accordingly.
     */
    bool setNamespacesForStruct(const char *name,
                                std::vector<std::string> *namespaces);
    /*! Stores the enum's namespace names into the vector.
     *
     *  As per setNamespacesForStruct.
     */
    bool setNamespacesForEnum(const char *name,
                              std::vector<std::string> *namespaces);
    /*! Store the fully-qualified struct name into the argument
     *  string.
     *  @param name The name of the struct.
     *  @param fqsn The string for storing the fully-qualified name.
     */
    bool setFullyQualifiedStructName(const char *name,
                                     std::string *fqsn);

    /*! Merge another context into this one.
     *  @param other The other context.
     *
     *  The active/used namespaces in the other context are ignored:
     *  this is just about the bindings.
     */
    bool merge(Context *other);

    /*! Convert a Dale type into an LLVM type.
     *  @param type The Dale type.
     *  @param n A node to which errors should refer.
     *  @param allow_non_first_class Whether to allow a
     *  non-first-class type to be converted.
     *  @param externally_defined Whether the type was defined
     *  externally.
     *  @param refs_to_pointers Whether to convert Dale reference
     *  types into LLVM pointer types.
     *
     *  The other toLLVMType functions should not be used externally.
     */
    llvm::Type *toLLVMType(Type *type,
                           Node *n,
                           bool allow_non_first_class = true,
                           bool externally_defined = false,
                           bool refs_to_pointers = false);
    llvm::Type *toLLVMType_(Type *type,
                            Node *n,
                            bool refs_to_pointers);
    llvm::Type *toLLVMTypeStruct(Type *type,
                                 Node *n);
    llvm::Type *toLLVMTypeBase(Type *type,
                               Node *n);
    llvm::Type *toLLVMTypeFunction(Type *type,
                                   Node *n,
                                   bool refs_to_pointers);
    llvm::Type *toLLVMTypePointer(Type *type,
                                  Node *n,
                                  bool refs_to_pointers);
    llvm::Type *toLLVMTypeArray(Type *type,
                                Node *n);

    /*! Convert a Dale linkage into an LLVM linkage.
     *  @param linkage The Dale linkage.
     */
    llvm::GlobalValue::LinkageTypes toLLVMLinkage(int linkage);

    /*! Reget all LLVM types/values/functions from the module.
     *  @param mod The LLVM module.
     */
    bool regetPointers(llvm::Module *mod);
    /*! Reget all LLVM types/values/functions from the module.
     *  @param mod The LLVM module.
     *
     *  Should be used immediately after merging a new module's
     *  context.
     */
    bool regetPointersForNewModule(llvm::Module *mod);
    /*! Rebuilds a single LLVM function.
     *  @param fn The function to rebuild.
     *  @param name The function's name (unmangled).
     *  @param mod The LLVM module.
     */
    bool rebuildFunction(Function *fn, const char *name,
                         llvm::Module *mod);
    /*! Rebuilds LLVM functions, where required.
     *  @param mod The LLVM module.
     *  @param node The namespace node.
     *
     *  All functions that do not already have an LLVM function will
     *  have a new LLVM function retrieved or constructed as
     *  appropriate.
     */
    bool rebuildFunctions(llvm::Module *mod, NSNode *node);

    /*! Rebuilds a single LLVM variable.
     *  @param fn The function to rebuild.
     *  @param mod The LLVM module.
     */
    bool rebuildVariable(Variable *var, const char *name,
                         llvm::Module *mod);
    /*! Rebuilds LLVM variables, where required.
     *  @param mod The LLVM module.
     *  @param name The variable's name (unmangled).
     *  @param node The namespace node.
     *
     *  As per rebuildFunctions, this only retrieves/constructs
     *  LLVM variables for variables that do not already have one.
     */
    bool rebuildVariables(llvm::Module *mod, NSNode *node);

    /*! Remove bindings that were added by way of module import.
     *
     *  See Namespace::removeDeserialised.
     */
    bool removeDeserialised();
    /*! Remove bindings that aren't included in the set of forms.
     *
     * See Namespace::removeUnneeded.
     */
    bool removeUnneeded(std::set<std::string> *forms,
                        std::set<std::string> *found_forms);

    /*! Erase LLVM function bodies and variables for forms with a
     *  given 'once' tag.
     *  @param once_tags The current set of 'once' tags.
     *  @param mod The LLVM module to use for erasure.
     *
     *  See Namespace::eraseOnceFunctions and
     *  Namespace::eraseOnceVariables.
     */
    bool eraseOnceForms(std::set<std::string> *once_tags,
                        llvm::Module *mod);
    /*! Delete all anonymous namespaces from this context.
     */
    bool deleteAnonymousNamespaces();
    /*! Delete the given namespace and all namespaces under it.
     *  @param node The namespace node.
     */
    void deleteNamespaces(NSNode *node);

    /*! Relink all namespaces.
     *
     *  This entails iterating over the namespace nodes and setting
     *  the parent namespace of each child node to that of the
     *  namespace from the namespace node.
     */
    void relink();

    /*! Print the context's details to stderr. */
    void print();
};
}

#endif
