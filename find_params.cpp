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

/// new code
bool check_a_b(int64_t a, int64_t b, int64_t M) {
    return mod<int64_t>(4 * modpow<int64_t>(a, 3, M) + 27 * modpow<int64_t>(b, 2, M), M) == 0;
}

int calculate_Jacobi_symbol(int64_t a, int64_t n) {
    int64_t t = 1;
    while(a != 0) {
        while(! (a % 2) ) {
            a /= 2;
            int64_t r = n % 8;
            if(r == 3 || r == 5)
                t = -t;
        }
        std::swap(a, n);
        if(a % 4 == 3 && n % 4 == 3)
            t = -t;
        a %= n;
    }
    if (n == 1)
        return t;
    else
        return 0;
}

std::set< std::pair<int64_t, int64_t> > find_x_and_y(int64_t a, int64_t b, int64_t modulo) {
    std::set< std::pair<int64_t, int64_t> > result;

    int64_t m = (modulo - 3) / 4;
    for(int64_t x = 0; x < modulo; ++x){
        auto temp = modpow<int64_t>(x, 3, modulo) + a * x + b;
        if(calculate_Jacobi_symbol(temp, modulo) == 1) {
            auto y1 =  modpow<int64_t>(temp, m + 1, modulo);
            auto y2 = modulo - y1;
            result.insert(std::make_pair(x, y1));
            result.insert(std::make_pair(x, y2));
        }
        if(mod<int64_t>(temp, modulo) == 0){
            auto y = mod<int64_t>(modpow<int64_t>(x, 3, modulo) + a * x + b, modulo);
            result.insert(std::make_pair(x, y));
        }
    }

    return result;
}

template<typename Callback>
set_params find_points_abxy(int64_t modulo, bool is_positive, Callback find_xy_points) {
    counter_one = 0;
    counter_two = 0;

    set_params result(comp);

    for(int64_t a = is_positive ? 0 : -modulo + 1; a < modulo; ++a) {
        for(int64_t b = is_positive ? 0 : -modulo + 1; b < modulo; ++b) {
            counter_one++;
            if(check_a_b(a, b, modulo)) {
                auto points = find_xy_points(a, b, modulo);
                if(!points.size()) continue;
                result.insert(Params(a, b, points));
            } else {
                counter_two++;
            }
        }
    }

    return result;
}

int main(){
    const int64_t M = 47;

    std::ofstream output_my_way("results_in_my_way.txt");
    auto my_params = find_points_abxy(M, false, find_points);
    for(auto i: my_params){
        output_my_way << i << std::endl;
    }

    std::ofstream output("results.txt");
    auto params = find_points_abxy(M, true, find_x_and_y);
    for(auto i: params){
        output << i << std::endl;
    }

    std::cout << "All iterations = " << counter_one << std::endl;
    std::cout << "a & b feet = " << params.size() << std::endl;
    std::cout <<  "a & b not feet = " << counter_two << std::endl;

    std::set<std::pair<int64_t, int64_t>> xy_set;
    for(auto i: params){
        xy_set.insert(i.points.begin(), i.points.end());
    }

    std::cout << "Points in total = " << xy_set.size() << std::endl;

    {   // code for testing
        std::cout << "Testing params(1, 0, 23):" << std::endl;
        
        auto found = find_x_y(1, 0, 23);
        for(auto i: found){
            std::cout << "x = " << i.first << "\ty = " << i.second << std::endl;
        }

        std::cout << "Testing params(2, 6, 7):" << std::endl;
        for(auto i: find_x_and_y(2, 6, 7)){
            std::cout << "x = " << i.first << " y = " << i.second << std::endl;
        }
    }
}