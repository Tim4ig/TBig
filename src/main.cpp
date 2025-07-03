#include <iostream>
#include <bits/ostream.tcc>

#include "bigint.hpp"

int main()
{
    const auto a = t::big::BigInt<4096>(4);
    const auto b = t::big::BigInt<4096>(3);

    std::cout << a.to_string() << '\n';
    std::cout << b.to_string() << '\n';

    std::cout << (a + b).to_string() << '\n';
    std::cout << (a - b).to_string() << '\n';
    std::cout << (a * b).to_string() << '\n';

    return 0;
}
