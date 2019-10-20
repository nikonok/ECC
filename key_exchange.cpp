#include <iostream>

#include "utils.hpp"

int main(){
    Point<int64_t> p(16, 5, 9, 17, 23);

    // should be (20, 20)
    std::cout << "Point 2 * p, p: " << p << std::endl;
    std::cout << (int64_t)2 * p << std::endl;

    int64_t a = 0;
    int64_t b = -4;
    uint64_t m = 211;
    Point<int64_t> G(2, 2, a, b, m);
    ExchangeKeys exch1(a, b, m, G);
    ExchangeKeys exch2(a, b, m, G);

    auto secret1 = exch1.get_shared_secret(exch2.pubkey);
    auto secret2 = exch2.get_shared_secret(exch1.pubkey);
    std::cout << "Shared secret one: " << secret1 << std::endl;
    std::cout << "Shared secret two: " << secret2 << std::endl;
    std::cout << "Are thay equal: " << (secret1 == secret2) << std::endl;
}