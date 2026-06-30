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
        constexpr auto operator-(this auto&& self){
            using Decayed = std::decay_t<decltype(self)>;
            Decayed result;
            for (std::size_t i = 0; i <= Deg; ++i) {
                result.coeffs_[i] = -self.coeffs_[i];
            }
            return result;
        }

        // Scalar Multiplication (P * scalar)
        template <typename Scalar>
            requires std::convertible_to<Scalar, T>
        constexpr auto operator*(this auto&& self, Scalar val) {
            using Decayed = std::decay_t<decltype(self)>;
            Decayed result;
            for (std::size_t i = 0; i <= Deg; ++i) {
                result.coeffs_[i] = self.coeffs_[i] * static_cast<T>(val);
            }
            return result;
        }

        // Scalar Division (P / scalar)
        template <typename Scalar>
            requires std::convertible_to<Scalar, T>
        constexpr auto operator/(this auto&& self, Scalar val) {
            using Decayed = std::decay_t<decltype(self)>;
            Decayed result;
            T inv = T{1.0} / static_cast<T>(val);
            for (std::size_t i = 0; i <= Deg; ++i) {
                result.coeffs_[i] = self.coeffs_[i] * inv;
            }
            return result;
        }


        void print() const {
            bool first = true;
            for (std::size_t i = 0; i <= Deg; ++i) {
                if (coeffs_[i] == T{0}) continue;

                if (!first && coeffs_[i] > T{0}) std::cout << " + ";
                else if (coeffs_[i] < T{0}) std::cout << " - ";

                T abs_val = coeffs_[i] < T{0} ? -coeffs_[i] : coeffs_[i];

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


    // Scalar * Polynomial
    template <Arithmetic T, std::size_t Deg, typename Scalar>
        requires std::convertible_to<Scalar, T>
    constexpr auto operator*(Scalar val, const Polynomial<T, Deg>& poly) {
        return poly * val;
    }

    // Polynomial Addition (P1 + P2)
    template <Arithmetic T, std::size_t Deg1, std::size_t Deg2>
    constexpr auto operator+(const Polynomial<T, Deg1>& lhs, const Polynomial<T, Deg2>& rhs) {
        constexpr std::size_t OutDeg = std::max(Deg1, Deg2);
        Polynomial<T, OutDeg> result{};

        for (std::size_t i = 0; i <= OutDeg; ++i) {
            T val1 = (i <= Deg1) ? lhs.coeffs_[i] : T{0};
            T val2 = (i <= Deg2) ? rhs.coeffs_[i] : T{0};
            result.coeffs_[i] = val1 + val2;
        }
        return result;
    }


    // Polynomial Subtraction (P1 - P2)
    template <Arithmetic T, std::size_t Deg1, std::size_t Deg2>
    constexpr auto operator-(const Polynomial<T, Deg1>& lhs, const Polynomial<T, Deg2>& rhs) {
        constexpr std::size_t OutDeg = std::max(Deg1, Deg2);
        Polynomial<T, OutDeg> result{};

        for (std::size_t i = 0; i <= OutDeg; ++i) {
            T val1 = (i <= Deg1) ? lhs.coeffs_[i] : T{0};
            T val2 = (i <= Deg2) ? rhs.coeffs_[i] : T{0};
            result.coeffs_[i] = val1 - val2;
        }
        return result;
    }


    /**
     * Polynomial Multiplication (P1 * P2 using algebraic convolution)
     * @tparam T Param Type
     * @tparam Deg1 degree of poly 1
     * @tparam Deg2 degree of poly 2
     * @param lhs poly_1
     * @param rhs poly_2
     * @return multiplicand of polynomial
     */
    template <Arithmetic T, std::size_t Deg1, std::size_t Deg2>
    constexpr auto operator*(const Polynomial<T, Deg1>& lhs, const Polynomial<T, Deg2>& rhs) {
        constexpr std::size_t OutDeg = Deg1 + Deg2;
        Polynomial<T, OutDeg> result{};

        for (std::size_t i = 0; i <= Deg1; ++i) {
            for (std::size_t j = 0; j <= Deg2; ++j) {
                result.coeffs[i + j] += lhs.coeffs_[i] * rhs.coeffs_[j];
            }
        }
        return result;
    }


}

#endif //SYMBOLICSTOZCOMPILER_POLY_HPP
