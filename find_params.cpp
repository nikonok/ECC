#include <iostream>
#include <vector>
#include <fstream>
#include <set>

#include "utils.hpp"

struct Params {
    int64_t a = 0;
    int64_t b = 0;
    std::set< std::pair<int64_t, int64_t> > points;

    Params(){}
    Params(int64_t _a, int64_t _b, std::set< std::pair<int64_t, int64_t> > _points):
           a(_a), b(_b), points(_points.begin(), _points.end()){}
    Params(const Params& p): a(p.a), b(p.b), points(p.points.begin(), p.points.end()){}
};

std::ostream& operator<<(std::ostream& os, const Params& param){
    os << "a = " << param.a << "\tb = " << param.b << std::endl;
    for(auto item: param.points) {
        os << "\tx = " << item.first << "\ty = " << item.second << std::endl;
    }
    return os;
}

auto comp = [](const Params& left, const Params& right) {
    if(left.a < right.a) return true;
    if(left.a > right.a) return false;
    if(left.b < right.b) return true;
    return false;
};

using set_params = std::set<Params, decltype(comp)>;

int64_t find_y(int64_t yq, int64_t modulo) {
    auto L = modpow<int64_t>(yq, mod<int64_t>( ( (int64_t)modulo - 1) * modInverse<int64_t>(2, modulo), modulo ), modulo );
    if(L==1){
        for(int64_t y = 2; y <= modulo; ++y){
            if(modpow<int64_t>(y, 2, modulo) == yq){
                return y;
            }
        }
    }
    throw "Not found";
}

std::set< std::pair<int64_t, int64_t> > find_points(int64_t a, int64_t b, int64_t modulo) {
    std::set< std::pair<int64_t, int64_t> > result;

    for(int64_t x = 0; x <= modulo; ++x){
        // y ^ 2
        auto yq = mod<int64_t>(modpow<int64_t>(x, 3, modulo) + a * x + b, modulo);
        int64_t y = 0;
        try{
            y = find_y(yq, modulo);
        } catch(const char * e) {
            continue;
        }
        result.insert(std::make_pair(x, y));
        result.insert(std::make_pair(x, -y));
    }

    return result;
}

uint64_t counter_one = 0;
uint64_t counter_two = 0;

set_params find_params(int64_t modulo){
    set_params result(comp);

    for(int64_t a = -modulo + 1; a < modulo; ++a) {
        for(int64_t b = -modulo + 1; b < modulo; ++b) {
            counter_one++;
            if(mod<int64_t>(4 * modpow<int64_t>(a, 3, modulo) + 27 * modpow<int64_t>(b, 2, modulo), modulo) != 0) {
                auto points = find_points(a, b, modulo);
                if(!points.size()) continue;
                result.insert(Params(a, b, points));
            } else {
                counter_two++;
            }
        }
    }

    return result;
}

//// testing params
bool is_result(uint64_t x, uint64_t y, uint64_t a, uint64_t b, uint64_t M) {
    return modpow<uint64_t>(y, 2, M) == mod( modpow<uint64_t>(x, 3, M) + mod(a * x, M) + b, M);
}

std::set< std::pair<uint64_t, uint64_t> > find_x_y(uint64_t a, uint64_t b, uint64_t M){
    std::set< std::pair<uint64_t, uint64_t> > result;
    for(uint64_t x = 0; x < M; ++x){
        for(uint64_t y = 0; y < M; ++y){
            if(is_result(x, y, a, b, M)){
                result.insert(std::make_pair(x, y));
            }
        }
    }
    return result;
}

int main(){
    const int64_t M = 47;

    std::ofstream output("results.txt");
    auto params = find_params(M);
    for(auto i: params){
        output << i << std::endl;
    }

    std::cout << "All iterations = " << counter_one << std::endl;
    std::cout << "a & b feet = " << params.size() << std::endl;
    std::cout <<  "a & b not feet = " << counter_two << std::endl;

    { // code for testing
        std::cout << "Testing params(1, 0, 23):" << std::endl;
        
        auto found = find_x_y(1, 0, 23);
        for(auto i: found){
            std::cout << "x = " << i.first << "\ty = " << i.second << std::endl;
        }
    }
}