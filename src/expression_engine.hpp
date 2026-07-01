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



    /**
    * @brief Represents the independent variable 's' in the continuous domain.
    */
    template <typename T>
    struct VariableNode : public Expression<VariableNode<T>> {
        using value_type = T;

        constexpr VariableNode() = default;

        // Evaluation: VariableNode 's' simply evaluates to the input value x
        template <typename U>
        constexpr auto evaluate(const U& x) const {
            return x;
        }
    };

    /**
    * @brief Represents a scalar literal constant within the expression tree.
    */
    template <typename T>
    struct ConstantNode : public Expression<ConstantNode<T>> {
        using value_type = T;
        T value;

        constexpr explicit ConstantNode(T val) : value(val) {}

        // Evaluation: ConstantNode always evaluates to its stored constant value
        template <typename U>
        constexpr auto evaluate(const U&) const {
            return value;
        }
    };

    /**
     * @brief Represents symbolic negation: -Expr
     */
    template <typename Expr>
    struct UnaryMinusNode : public Expression<UnaryMinusNode<Expr>> {
        Expr expr;

        constexpr explicit UnaryMinusNode(Expr e) : expr(std::move(e)) {}

        template <typename U>
        constexpr auto evaluate(const U& x) const {
            return -expr.evaluate(x);
        }
    };


    /**
    * @brief Generic Binary Operator Node supporting addition, subtraction, multiplication, and division.
    */
    template <typename Op, typename Left, typename Right>
    struct BinaryNode : public Expression<BinaryNode<Op, Left, Right>> {
        Left lhs;
        Right rhs;

        constexpr BinaryNode(Left l, Right r) : lhs(std::move(l)), rhs(std::move(r)) {}

        template <typename U>
        constexpr auto evaluate(const U& x) const {
            auto left_val = lhs.evaluate(x);
            auto right_val = rhs.evaluate(x);

            if constexpr (std::is_same_v<Op, AddOp>) {
                return left_val + right_val;
            } else if constexpr (std::is_same_v<Op, SubOp>) {
                return left_val - right_val;
            } else if constexpr (std::is_same_v<Op, MulOp>) {
                return left_val * right_val;
            } else if constexpr (std::is_same_v<Op, DivOp>) {
                return left_val / right_val;
            }
        }
    };

}

#endif //SYMBOLICSTOZCOMPILER_EXPRESSION_ENGINE_HPP
