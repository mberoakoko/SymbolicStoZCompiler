//
// Created by mbero-akoko on 6/30/26.
//

#ifndef SYMBOLICSTOZCOMPILER_POLY_HPP
#define SYMBOLICSTOZCOMPILER_POLY_HPP

#include <array>
#include <concepts>
#include <algorithm>
#include <iostream>
#include <complex>
#include <utility>

namespace dsp {
    /// Concept requiring a type to support standard real or complex floating-point arithmetic.
    template <typename T>
    concept Arithmetic = std::floating_point<T> || requires(T a, T b) {
        { a + b } -> std::convertible_to<T>;
        { a - b } -> std::convertible_to<T>;
        { a * b } -> std::convertible_to<T>;
        { a / b } -> std::convertible_to<T>;
    };

    template<Arithmetic T, std::size_t Deg>
    class Polynomial {
    public:
        static constexpr std::size_t Degree = Deg;

        // Fields
        std::array<T, Degree + 1> coeffs_ {};
        constexpr Polynomial() = default;
        constexpr Polynomial(const std::array<T, Degree + 1>& initial_coeffs): coeffs_(std::move(initial_coeffs)){}


        template <typename... Args>
        requires (sizeof...(Args) == Deg + 1) && (std::convertible_to<Args, T> && ...)
        constexpr Polynomial(Args& ... args): coeffs_(static_cast<T>(std::forward<Args>(args))...){}


        // --- Accessors ---

        constexpr T& operator[](std::size_t index) { return coeffs_[index]; }
        constexpr const T& operator[](std::size_t index) const { return coeffs_[index]; }

        /**
        * @brief Evaluates the polynomial at a given value (real or complex) using Horner's Method.
        * @tparam U The input type (can be T, std::complex<T>, etc.).
        * @param x The evaluation point.
        * @return The evaluated result.
        */
        template <typename U>
        constexpr auto evaluate(const U& x) const {
            using ResultType = decltype(std::declval<T>() * std::declval<U>());
            auto result = static_cast<ResultType>(coeffs_[Deg]);

            // Horner's Method: loop backwards from Deg-1 down to 0
            if constexpr (Deg > 0) {
                for (auto i = Deg; i > 0; --i) {
                    result = result * x + coeffs_[i - 1];
                }
            }
            return result;
        }


        // Unary Negation (-P)
        constexpr auto operator-(this auto&& self)->std::decay<Polynomial&&> {
            using Decayed = std::decay_t<decltype(self)>;
            Decayed result;
            for (std::size_t i = 0; i <= Deg; ++i) {
                result.coeffs[i] = -self.coeffs[i];
            }
            return result;
        }

        // Scalar Multiplication (P * scalar)
        template <typename Scalar>
            requires std::convertible_to<Scalar, T>
        constexpr auto operator*(this auto&& self, Scalar val)->std::decay<Polynomial&&> {
            using Decayed = std::decay_t<decltype(self)>;
            Decayed result;
            for (std::size_t i = 0; i <= Deg; ++i) {
                result.coeffs[i] = self.coeffs[i] * static_cast<T>(val);
            }
            return result;
        }

        // Scalar Division (P / scalar)
        template <typename Scalar>
            requires std::convertible_to<Scalar, T>
        constexpr auto operator/(this auto&& self, Scalar val)->std::decay<Polynomial&&> {
            using Decayed = std::decay_t<decltype(self)>;
            Decayed result;
            T inv = T{1.0} / static_cast<T>(val);
            for (std::size_t i = 0; i <= Deg; ++i) {
                result.coeffs[i] = self.coeffs[i] * inv;
            }
            return result;
        }


        void print() const {
            bool first = true;
            for (std::size_t i = 0; i <= Deg; ++i) {
                if (coeffs_[i] == T{0}) continue;

                if (!first && coeffs_[i] > T{0}) std::cout << " + ";
                else if (coeffs_[i] < T{0}) std::cout << " - ";

                T abs_val = coeffs[_i] < T{0} ? -coeffs_[i] : coeffs_[i];

                if (abs_val != T{1} || i == 0) {
                    std::cout << abs_val;
                }

                if (i > 0) {
                    std::cout << "s";
                    if (i > 1) std::cout << "^" << i;
                }
                first = false;
            }
            if (first) std::cout << "0";
            std::cout << "\n";
        }
    };
}

#endif //SYMBOLICSTOZCOMPILER_POLY_HPP
