#include "bigint.hpp"

#include <iostream>

int main()
{
    t::big::BigInt<2048> a{}, b{};

    a = 1;
    std::cout << a.to_string() << '\n';

    b = 254;
    std::cout << b.to_string() << '\n';

    auto c = a + b;
    std::cout << c.to_string() << '\n';

    std::cout << (a == b) << '\n';

    return 0;
}
