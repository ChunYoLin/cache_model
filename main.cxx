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
    cache.write(0, 1, 1);
    cache.write(1, 1, 1);
    uint32_t data = cache.read(0, 4);

    std::cout << data << std::endl;
    std::cout << cache.stat.hit << "/" << cache.stat.access << std::endl;
}
