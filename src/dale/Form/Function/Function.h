#ifndef DALE_FORM_FUNCTION
#define DALE_FORM_FUNCTION

#include "../../Units/Units.h"

namespace dale
{
/*! Parse a function definition form.
 *  @param units The units context.
 *  @param node The node containing the definition.
 *  @param name The name of the function.
 *  @param new_fn A pointer for storing the newly-created function object.
 *  @param linkage The linkage for the new function (optional).
 *  @param is_anonymous Whether the function is anonymous.
 */
bool FormFunctionParse(Units *units, Node *node, const char *name,
                       Function **new_fn, int linkage, bool is_anonymous);
/*! Determine whether a given function can be validly defined.
 *  @param ctx The context.
 *  @param node The node containing the function definition.
 *  @param name The name of the function.
 *  @param linkage The linkage for the new function (optional).
 *  @param function The function to be defined.
 */
bool isValidDeclaration(Context *ctx, Node *node, const char *name,
                        int linkage, Function *fn);

}

#endif
