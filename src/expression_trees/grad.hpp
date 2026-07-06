//
// Created by mbero-akoko on 7/2/26.
//

#ifndef SYMBOLICSTOZCOMPILER_GRAD_HPP
#define SYMBOLICSTOZCOMPILER_GRAD_HPP

#include <ostream>
#include <bits/stdc++.h>

namespace grad {
    struct Value;
    using ValueSharedPtr = std::shared_ptr<Value>;

    struct  Value: std::enable_shared_from_this<Value> {

        double data;
        double grad = 0;
        std::vector<Value> _prev_val;
        std::string _op;
        std::string label;
        std::function<void()> _backward = []{};

        Value(double data, std::vector<Value> children = {}, const std::string& op = "", const std::string& label = "")
            : data(data), _prev_val(std::move(children)), _op(op), label(label){

        }

        friend std::ostream & operator<<(std::ostream &os, const Value &obj) {
            return os
                    << "Value ("
                   << "data: " << obj.data
                   << " grad: " << obj.grad
                   << " _op: " << obj._op
                   << " label: " << obj.label
                    << " )";
        }
    };


}


#endif //SYMBOLICSTOZCOMPILER_GRAD_HPP
