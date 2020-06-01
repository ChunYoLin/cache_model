#include "cache.h"

#include <iostream>


int main()
{
    auto cache = Cache(4096, 32, 1);

    cache.read(0, 1);
    cache.read(0, 1);
    cache.read(31, 1);
    cache.read(0x1 << 19, 1);
    cache.read(0, 1);
    cache.read(0, 1);

    std::cout << cache.stat.hit << "/" << cache.stat.access << std::endl;
}
