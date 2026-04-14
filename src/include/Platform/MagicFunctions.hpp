#pragma once

#include "LSP/Utils.hpp"
#include "Luau/Ast.h"
#include "Luau/BuiltinDefinitions.h"
#include "Luau/ConstraintSolver.h"
#include "Luau/Type.h"
#include "Luau/TypeInfer.h"

#include <memory>
#include <string>
#include <vector>

/// A reusable magic function that looks up a string argument in a predefined list
/// and narrows the return type to the corresponding declared class type.
/// Used for patterns like `game:GetService("ServiceName")` or `Library:GetModule("ModuleName")`.
struct MagicTypeLookup final : Luau::MagicFunction
{
    std::vector<std::string> lookupList;
    std::string errorMessagePrefix;

    MagicTypeLookup(std::vector<std::string> lookupList, std::string errorMessagePrefix)
        : lookupList(std::move(lookupList))
        , errorMessagePrefix(std::move(errorMessagePrefix))
    {
    }

    std::optional<Luau::WithPredicate<Luau::TypePackId>> handleOldSolver(struct Luau::TypeChecker& typeChecker,
        const std::shared_ptr<struct Luau::Scope>& scope, const class Luau::AstExprCall& expr,
        Luau::WithPredicate<Luau::TypePackId> withPredicate) override;
    bool infer(const Luau::MagicFunctionCallContext&) override;
};
