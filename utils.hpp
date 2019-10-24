#include <ostream>
#include <random> 

// math mod operation
template <typename T>
T mod(T a, T b) {
    return (a % b+ b) % b;
}

// pow number by modulus
template <typename T>
T modpow(T base, T exp, T modulus) {
  base = mod(base, modulus);
  T result = 1;
  T null = 0;
  while (exp > 0) {
    if ((exp & 1) != null) result = mod((result * base), modulus);
    base = mod((base * base), modulus);
    exp >>= 1;
  }
  return result;
}

// Euclidean algorithm
template <typename T>
T gcdExtended(T a, T b, T *x, T *y) { 
    // Base Case 
    if (a == 0) 
    { 
        *x = 0, *y = 1; 
        return b; 
    } 
  
    T x1, y1; // To store results of recursive call 
    T gcd = gcdExtended(mod(b, a), a, &x1, &y1); 
  
    // Update x and y using results of recursive 
    // call 
    *x = y1 - (b/a) * x1; 
    *y = x1; 
  
    return gcd; 
} 
// Function to find modulus inverse of a 
template <typename T>
T modInverse(T a, T m) { 
    T x, y; 
    T g = gcdExtended(a, m, &x, &y); 
    if (g != 1) 
        throw "Inverse doesn't exist"; 
    else
    { 
        // m is added to handle negative x 
        return mod((mod(x, m) + m), m);  
    } 
}

// Point on elliptic curve
// this class implements only idea of Point, not the best performance
template <typename T>
class Point {
public:
    T a = 0;
    T b = 0;
    T x = 0;
    T y = 0;
    T modulo = 0;

    Point(){}

    Point(T _x, T _y, T _a, T _b, T m):
          a(_a), b(_b), x(_x), y(_y), modulo(m)
    {}

    bool operator==(const Point& p) const {
        return x == p.x && y == p.y;
    }

    bool is_equal(const Point& p) const {
        return a == p.a && b == p.b &&
               x == p.x && y == p.y &&
               modulo == p.modulo;
    }

    bool operator!=(const Point& p) const {
        return !(*this == p);
    }

    Point operator+(const Point& p) const {
        auto lambda = get_lamda(p);
        auto new_x = mod<T>(modpow<T>(lambda, 2, modulo) - p.x - x, modulo);
        return Point(new_x,
                     mod<T>(lambda * (x - new_x) - y, modulo), a, b, modulo);
    }

    Point operator*(T G) const {
        Point p = *this;
        for(T i = 1; i < G; ++i){
            p = p + p;
        }

        return p;
    }

private:
    T get_lamda(const Point& p) const {
        if(*this != p) {
            if(p.x - x < 0)
                return mod<T>(-(p.y - y) * modInverse<T>(-(p.x - x), modulo), modulo);
            return mod<T>((p.y - y) * modInverse<T>(p.x - x, modulo), modulo);
        } else {
            return mod<T>( (3 * modpow<T>(x, 2, modulo) + a) *  modInverse<T>(2 * y, modulo), modulo);
        }
    }
};

Point<int64_t> operator*(uint64_t G, const Point<int64_t>& p) {
    return p * G;
}

template <typename T>
std::ostream& operator<<(std::ostream& os, const Point<T>& p) {
    os << "x = " << p.x << " y = " << p.y;
    return os;
}

// implemetation of Diffie–Hellman key exchange on elliptic curves
// not best usage and perfomance
class ExchangeKeys {
public:
    uint64_t privkey;
    Point<int64_t> pubkey;
    Point<int64_t> G;
    uint64_t modulo = 0;
    int64_t a = 0;
    int64_t b = 0;

    ExchangeKeys(int64_t _a, int64_t _b, uint64_t m, Point<int64_t> g):
                 G(g), modulo(m), a(_a), b(_b)
    {
        generate_privkey();
        generate_pubkey();
    }

    Point<int64_t> get_shared_secret(const Point<int64_t>& shared_pubkey) {
        return shared_pubkey * privkey;
    }

private:
    void generate_privkey() {
        std::random_device rd;
        std::mt19937 mt(rd());
        std::uniform_int_distribution<uint64_t> dist(1, modulo);
        privkey = dist(mt);
    }

    void generate_pubkey() {
        pubkey = G * privkey;
    }
};
