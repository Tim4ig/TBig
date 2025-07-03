#include <iostream>
#include <bits/ostream.tcc>

#include "bigint.hpp"

int main()
{
    auto a = t::big::BigInt<8192>(17);
    const auto b = t::big::BigInt<8192>(11);

    for (int i = 0; i < 350; ++i)
    {
        a *= b;
    }

    std::cout << a.to_string() << '\n';

    auto c = t::big::BigInt<8192>(432);
    const auto d = t::big::BigInt<8192>(15);

    for (int i = 0; i < 117; ++i)
    {
        c *= d;
    }

    std::cout << c.to_string() << '\n';

    std::cout << (a + c).to_string() << '\n';


    return 0;
}
