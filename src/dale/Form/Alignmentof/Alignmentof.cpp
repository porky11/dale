#include "../../Generator/Generator.h"
#include "../../Node/Node.h"
#include "../../ParseResult/ParseResult.h"
#include "../../Element/Function/Function.h"
#include "llvm/Function.h"

namespace dale
{
namespace Form
{
namespace Alignmentof
{
bool execute(Generator *gen,
             Element::Function *fn,
             llvm::BasicBlock *block,
             Node *node,
             bool get_address,
             bool prefixed_with_core,
             ParseResult *pr)
{
    assert(node->list && "must receive a list!");

    Context *ctx = gen->ctx;

    if (!ctx->er->assertArgNums("alignmentof", node, 1, 1)) {
        return false;
    }

    symlist *lst = node->list;

    Node *mytype = (*lst)[1];
    mytype = gen->parseOptionalMacroCall(mytype);
    if (!mytype) {
        return false;
    }
    Element::Type *type = gen->parseType(mytype, false,
                                    false);
    if (!type) {
        return false;
    }

    bool res = gen->getAlignmentofType(block, type, pr);
    return res;
}
}
}
}