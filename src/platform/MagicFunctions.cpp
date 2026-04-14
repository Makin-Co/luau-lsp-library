#include "Platform/MagicFunctions.hpp"

std::optional<Luau::WithPredicate<Luau::TypePackId>> MagicTypeLookup::handleOldSolver(struct Luau::TypeChecker& typeChecker,
    const std::shared_ptr<struct Luau::Scope>&, const class Luau::AstExprCall& expr, Luau::WithPredicate<Luau::TypePackId>)
{
    if (expr.args.size < 1)
        return std::nullopt;

    if (auto str = expr.args.data[0]->as<Luau::AstExprConstantString>())
    {
        auto className = std::string(str->value.data, str->value.size);
        if (contains(lookupList, className))
        {
            std::optional<Luau::TypeFun> tfun = typeChecker.globalScope->lookupType(className);
            if (!tfun || !tfun->typeParams.empty() || !tfun->typePackParams.empty())
            {
                typeChecker.reportError(Luau::TypeError{expr.args.data[0]->location, Luau::UnknownSymbol{className, Luau::UnknownSymbol::Type}});
                return std::nullopt;
            }

            auto type = Luau::follow(tfun->type);

            Luau::TypeArena& arena = typeChecker.currentModule->internalTypes;
            Luau::TypePackId classTypePack = arena.addTypePack({type});
            return Luau::WithPredicate<Luau::TypePackId>{classTypePack};
        }
        else
        {
            typeChecker.reportError(Luau::TypeError{expr.args.data[0]->location, Luau::GenericError{errorMessagePrefix + " '" + className + "'"}});
        }
    }

    return std::nullopt;
}

bool MagicTypeLookup::infer(const Luau::MagicFunctionCallContext& context)
{
    if (context.callSite->args.size < 1)
        return false;

    if (auto str = context.callSite->args.data[0]->as<Luau::AstExprConstantString>())
    {
        auto className = std::string(str->value.data, str->value.size);
        if (contains(lookupList, className))
        {
            std::optional<Luau::TypeFun> tfun = context.solver->rootScope->lookupType(className);
            if (!tfun || !tfun->typeParams.empty() || !tfun->typePackParams.empty())
            {
                context.solver->reportError(
                    Luau::TypeError{context.callSite->args.data[0]->location, Luau::UnknownSymbol{className, Luau::UnknownSymbol::Type}});
                return false;
            }

            auto type = Luau::follow(tfun->type);
            Luau::TypePackId classTypePack = context.solver->arena->addTypePack({type});
            asMutable(context.result)->ty.emplace<Luau::BoundTypePack>(classTypePack);
            return true;
        }
        else
        {
            context.solver->reportError(
                Luau::TypeError{context.callSite->args.data[0]->location, Luau::GenericError{errorMessagePrefix + " '" + className + "'"}});
        }
    }

    return false;
}
