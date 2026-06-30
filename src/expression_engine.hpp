//
// Created by mbero-akoko on 7/1/26.
//

#ifndef SYMBOLICSTOZCOMPILER_EXPRESSION_ENGINE_HPP
#define SYMBOLICSTOZCOMPILER_EXPRESSION_ENGINE_HPP
#include "poly.hpp"
#include <concepts>
#include <complex>
#include <type_traits>
#include <utility>

namespace dsp {
    // Forward declarations of our AST nodes
    template <typename T> struct VariableNode;
    template <typename T> struct ConstantNode;

    template <typename Expr> struct UnaryMinusNode;

    struct AddOp;
    struct SubOp;
    struct MulOp;
    struct DivOp;


    template <typename Op, typename Left, typename Right> struct BinaryNode;

    // Here we will use some expression templates
    template <typename T>
    struct is_expression : std::false_type {};

    template <typename T>
    concept ExpressionConcept = is_expression<std::decay_t<T>>::value;

    template <typename T>
    concept ExpressionOrScalar = ExpressionConcept<T> || Arithmetic<std::decay_t<T>>;


    /**
    * @brief Base class for all expression templates using C++23 Deducing This.
    * This class provides standard operator overloads for any derived expression node type.
    */
    template <typename Derived>
    struct Expression {
        // Register this class as a valid expression
        using is_expression_type = std::true_type;

        // Helper to access the derived node
        constexpr const Derived& derived(this const auto& self) {
            return static_cast<const Derived&>(self);
        }
    };

    // Specialize the trait to recognize our Expressions
    template <typename Derived>
    struct is_expression<Expression<Derived>> : std::true_type {};

    template <typename Derived> // NOTE: there is a bug here
    struct is_expression : std::bool_constant<std::is_base_of_v<Expression<Derived>, Derived>> {};

    template <typename T>
    constexpr auto as_node(T&& val) {
        if constexpr (ExpressionConcept<T>) {
            return std::forward<T>(val);
        } else {
            using ScalarType = std::decay_t<T>;
            return ConstantNode<ScalarType>{std::forward<T>(val)};
        }
    }

}

#endif //SYMBOLICSTOZCOMPILER_EXPRESSION_ENGINE_HPP
