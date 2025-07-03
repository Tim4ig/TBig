#include <iostream>
#include <bits/ostream.tcc>

#include "bigint.hpp"

int main()
{
    auto a = t::big::BigInt<8192>(16);
    const auto b = t::big::BigInt<8192>(16);

    for (int i = 0; i < 350; ++i)
    {
        std::cout << "i=" << i << ",val=" << a.to_string(t::big::BigIntBase::Hex) << '\n';
        a *= b;
    }

    std::cout << a.to_string() << '\n';

    return 0;
}
