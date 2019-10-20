#include <iostream>

#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>

#include "utils.hpp"

/// new operators for BigInt support
Point<CryptoPP::Integer> operator*(CryptoPP::Integer integer, const Point<CryptoPP::Integer>& p) {
    Point<CryptoPP::Integer> result = p;
    for(CryptoPP::Integer i = 1; i < integer; i += 1){
        result = result + result;
    }
    return result;
}

Point<CryptoPP::Integer> operator*(const Point<CryptoPP::Integer>& p, CryptoPP::Integer integer) {
    return integer * p;
}

//// SHA256 of string to BitInt
CryptoPP::Integer SHA256(std::string msg) {
    CryptoPP::SHA256 hash;
    std::string result;

	CryptoPP::StringSource(msg, true, 
		new CryptoPP::HashFilter(hash,
        new CryptoPP::HexEncoder(
			new CryptoPP::StringSink(result))
		)
	);
    
    return CryptoPP::Integer(result.c_str());
}

// don't provide the best perfomance!!
// only idea
namespace ECDSA {
    using signature = std::pair<CryptoPP::Integer, CryptoPP::Integer>;
    using BigInt = CryptoPP::Integer;

    BigInt get_secret(BigInt q, const ::Point<BigInt>& G) {
        CryptoPP::AutoSeededRandomPool rng;
        BigInt privkey;
        ::Point<BigInt> pubkey;
        {
            bool flag = true;
            while(flag){
                try{
                    privkey = BigInt(rng, 1, q - 1);
                    pubkey = privkey * G;
                    flag = false;
                } catch (const char*) {
                    continue;
                }
            }
        }
        return privkey;
    }

    struct SignStruct {
        BigInt privkey;
        BigInt q;
        Point<BigInt> G;

        SignStruct(BigInt pkey, BigInt _q, Point<BigInt> g):
                   privkey(pkey), q(_q), G(g)
        {}
    };

    struct VerifyStruct {
        Point<BigInt> pubkey;
        BigInt q;
        Point<BigInt> G;

        VerifyStruct(Point<BigInt> pkey, BigInt _q, Point<BigInt> g):
                   pubkey(pkey), q(_q), G(g)
        {}
    };
    bool verify(VerifyStruct vgn, std::string msg, signature msg_sgn);
    signature sign(SignStruct sgn, std::string msg) {
        bool flag = true;
        BigInt r = 0;
        BigInt s = 0;
        while(flag){
            try {
                // hash msg
                auto hashed = SHA256(msg);
                BigInt k = 0;
                Point<BigInt> kG;
                do {
                    do {
                        // generate k
                        {
                            CryptoPP::AutoSeededRandomPool rng;
                            k = BigInt(rng, 2, sgn.q - 1);
                        }
                        // k * G
                        kG = k * sgn.G;
                        r = mod<BigInt>(kG.x, sgn.q);
                    } while (r == 0);
                    // s = k ^ -1 * (hashed + privkey * r)
                    s = mod<BigInt>(modInverse<BigInt>(k, sgn.q) * (hashed + BigInt(sgn.privkey) * r), sgn.q);
                } while(s == 0);
                flag = false;
                {
                    ECDSA::VerifyStruct vgn(sgn.privkey * 
                    sgn.G, sgn.q, sgn.G);
                    flag = !ECDSA::verify(vgn, msg, std::make_pair(r, s));
                }
            } catch (const char*){
                continue;
            }
        }
        return std::make_pair(r, s);
    }

    bool verify(VerifyStruct vgn, std::string msg, signature msg_sgn) {
        bool flag = true;
        while(flag){
            try {
                auto hashed = SHA256(msg);
                if(!(msg_sgn.first > 1 && msg_sgn.first < vgn.q) ||
                   !(msg_sgn.first > 1 && msg_sgn.first < vgn.q) ){
                    return false;
                }
                BigInt w = modInverse<BigInt>(msg_sgn.second, vgn.q);
                BigInt u1 = mod<BigInt>(hashed * w, vgn.q);
                BigInt u2 = mod<BigInt>(msg_sgn.first * w, vgn.q);
                auto uG = u1 * vgn.G + u2 * vgn.pubkey;
                auto r = mod<BigInt>(uG.x, vgn.q);
                if(msg_sgn.first == r)
                    return true;
                flag = false;
            } catch (const char*){
                continue;
            }
        }
        return false;
    }
};