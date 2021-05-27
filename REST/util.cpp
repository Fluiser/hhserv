#pragma once
#ifdef DEBUG
#include <iostream>
template<class... Args>
void d_log(Args&&... args)
{
    auto t = {
        (std::cout << args, 0)...
    };

    (void)t;
    std::cout << "\n";
}
#else
#define d_log(x) /* x */
#endif