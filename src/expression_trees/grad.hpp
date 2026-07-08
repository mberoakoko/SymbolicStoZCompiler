//
// Created by mbero-akoko on 7/2/26.
//

#ifndef SYMBOLICSTOZCOMPILER_GRAD_HPP
#define SYMBOLICSTOZCOMPILER_GRAD_HPP

#include <ostream>
#include <utility>
#include <bits/stdc++.h>

namespace grad {
    class Value;
    using ValueSharedPtr = std::shared_ptr<Value>;

    // Forward declaration of the node implementation
    struct ValueImpl : public std::enable_shared_from_this<ValueImpl> {
        double data;
        double grad = 0.0;

        // Graph edges MUST hold shared pointers to prevent copying and share ownership
        std::vector<std::shared_ptr<ValueImpl>> _prev_val;
        std::string _op;
        std::string label;
        std::function<void()> _backward = []{};

        ValueImpl(double data, std::vector<std::shared_ptr<ValueImpl>> children = {}, std::string  op = "", std::string  label = "")
            : data(data), _prev_val(std::move(children)), _op(std::move(op)), label(std::move(label)) {}

        // A sample addition operation showing how to use shared_from_this() safely
        static auto add(const std::shared_ptr<ValueImpl>& lhs, const std::shared_ptr<ValueImpl>& rhs) -> std::shared_ptr<ValueImpl> {
            auto out = std::make_shared<ValueImpl>(lhs->data + rhs->data, std::vector{lhs, rhs}, "+");

            // Use weak pointers inside lambda to avoid reference cycles (Memory Leaks!)
            std::weak_ptr weak_out = out;
            out->_backward = [lhs, rhs, weak_out] {
                if (auto strong_out = weak_out.lock()) {
                    lhs->grad += strong_out->grad;
                    rhs->grad += strong_out->grad;
                }
            };
            return out;
        }


        static auto subtract(const std::shared_ptr<ValueImpl>& lhs, const std::shared_ptr<ValueImpl>& rhs) -> std::shared_ptr<ValueImpl> {
            auto out = std::make_shared<ValueImpl>(lhs->data - rhs->data, std::vector{lhs, rhs}, "-");

            std::weak_ptr weak_out = out;
            out->_backward = [lhs, rhs, weak_out] {
                if (auto strong_out = weak_out.lock()) {
                    lhs->grad -= strong_out->grad;
                    rhs->grad -= strong_out->grad;
                }
            };

            return out;
        }

        static auto multiply(const std::shared_ptr<ValueImpl>& lhs, const std::shared_ptr<ValueImpl>& rhs) -> std::shared_ptr<ValueImpl> {
            auto out = std::make_shared<ValueImpl>(lhs->data * rhs->data, std::vector{lhs, rhs}, "*" );

            std::weak_ptr weak_out = out;
            out->_backward = [lhs, rhs, weak_out] {
                if (auto strong_out = weak_out.lock()) {
                    lhs->grad += strong_out->grad;
                    rhs->grad += strong_out->grad;
                }

            };

            return out;
        }

        static auto divide(const std::shared_ptr<ValueImpl>& lhs, const std::shared_ptr<ValueImpl>& rhs)-> std::shared_ptr<ValueImpl> {
            // auto out = std::make_shared<ValueImpl>(lhs->data / )
        }
    };

    // Optional: A clean wrapper class so the user doesn't have to type std::shared_ptr everywhere
    class Value {
        std::shared_ptr<ValueImpl> impl;
    public:
        Value(double val) : impl(std::make_shared<ValueImpl>(val)) {}
        // Implement operator+, operator*, backward(), etc. here by forwarding to 'impl'
    };


}


#endif //SYMBOLICSTOZCOMPILER_GRAD_HPP
