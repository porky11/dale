#include "Def.h"
#include "Config.h"
#include "../../../Generator/Generator.h"
#include "../../../BasicTypes/BasicTypes.h"
#include "../../../CoreForms/CoreForms.h"
#include "../../../Node/Node.h"
#include "../../Function/Function.h"
#include "../../Linkage/Linkage.h"
#include "../../Linkage/Enum/Enum.h"
#include "../../Type/Type.h"
#include "../../ProcBody/ProcBody.h"

#include "../GlobalVariable/GlobalVariable.h"
#include "../Function/Function.h"
#include "../Struct/Struct.h"
#include "../Macro/Macro.h"
#include "../Enum/Enum.h" 

namespace dale
{
namespace Form
{
namespace TopLevel
{
namespace Def
{
llvm::FunctionType *
getFunctionType(llvm::Type *t,
                std::vector<llvm::Type*> &v,
                bool b) {
    llvm::ArrayRef<llvm::Type*> temp(v);
    return llvm::FunctionType::get(t, temp, b);
}

bool parse(Generator *gen,
           Node *top)
{
    Context *ctx = gen->ctx;

    symlist *lst = top->list;

    if (lst->size() != 3) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectNumberOfArgs,
            top,
            "def", 2, (int) (lst->size() - 1)
        );
        ctx->er->addError(e);
        return false;
    }

    Node *n = (*lst)[1];

    n = gen->parseOptionalMacroCall(n);
    if (!n) {
        return false;
    }
    if (!n->is_token) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            n,
            "def", "an atom", "1", "a list"
        );
        ctx->er->addError(e);
        return false;
    }

    Token *name = n->token;

    if (name->type != TokenType::String) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            n,
            "def", "a symbol", "1", name->tokenType()
        );
        ctx->er->addError(e);
        return false;
    }

    n = (*lst)[2];

    if (!n->is_list) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            n,
            "def", "a list", "2", "an atom"
        );
        ctx->er->addError(e);
        return false;
    }

    n = gen->parseOptionalMacroCall(n);
    if (!n) {
        return false;
    }

    symlist *sublst = n->list;

    Node *subn = (*sublst)[0];

    if (!subn->is_token) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            subn,
            "def", "an atom", "2:1", "a list"
        );
        ctx->er->addError(e);
        return false;
    }

    Token *subt = subn->token;

    if (subt->type != TokenType::String) {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            subn,
            "def", "a symbol", "2:1", subt->tokenType()
        );
        ctx->er->addError(e);
        return false;
    }

    if (!subt->str_value.compare("fn")) {
        Form::TopLevel::Function::parse(gen, top);
    } else if (!subt->str_value.compare("var")) {
        Form::TopLevel::GlobalVariable::parse(gen, top);
    } else if (!subt->str_value.compare("struct")) {
        Form::TopLevel::Struct::parse(gen, top);
    } else if (!subt->str_value.compare("macro")) {
        Form::TopLevel::Macro::parse(gen, top);
    } else if (!subt->str_value.compare("enum")) {
        Form::TopLevel::Enum::parse(gen, top);
    } else {
        Error *e = new Error(
            ErrorInst::Generator::IncorrectArgType,
            subn,
            "def", "'fn'/'var'/'struct'/'macro'",
            "2:1"
        );
        std::string temp;
        temp.append("'");
        temp.append(subt->str_value);
        temp.append("'");
        e->addArgString(&temp);
        ctx->er->addError(e);
        return false;
    }

    return true;
}
}
}
}
}