#include <iostream>
#include <bits/ostream.tcc>

#include "bigint.hpp"

int main()
{
    auto a = t::big::BigInt<8192>(0x10);

    for (int i = 1; i <= 1024; ++i)
    {
        std::cout << static_cast<std::string>(a) << '\n';
        a *= 0x10;
    }

    return 0;
}
