#include <iostream>
#include "src/poly.hpp"

auto run_poly_functions() {
    auto poly_1 = dsp::Polynomial<float, 4>{std::array<float, 5>{1123, 3, 4, 5}};
    auto poly_2 = dsp::Polynomial<float, 4>{std::array<float, 5>{1, 3, 4, 5}};
    for (const auto item : {poly_1, poly_2}) {
        item.print();
    }
    std::cout << "Multiply operation\n";
    auto poly_3 = 2 * poly_2;
    poly_3.print();
    (poly_1 + poly_2).print();

}

int main() {
    std::cout << "Hello, World!" << std::endl;
    run_poly_functions();
    return 0;
}
