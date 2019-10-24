#include <iostream>

#include "sigutils.hpp"

int main(){
    // secp256k1
    // ECDSA::BigInt a = -3;
    // ECDSA::BigInt b("2455155546008943817740293915197451784769108058161191238065");
    // ECDSA::BigInt m("6277101735386680763835789423207666416083908700390324961279");
    // ECDSA::BigInt x("602046282375688656758213480587526111916698976636884684818");
    // ECDSA::BigInt y("174050332293622031404857552280219410364023488927386650641");
    // Point<ECDSA::BigInt> G(x, y, a, b, m);
    // ECDSA::BigInt q("6277101735386680763835789423176059013767194773182842284081");
    // ECDSA::BigInt privkey("1998");

    // std::string message("Hello World!");

    // ECDSA::SignStruct sgn(privkey, q, G);
    // auto sign = ECDSA::sign(sgn, message);
    // ECDSA::VerifyStruct vgn(privkey * G, q, G);
    // std::cout << ECDSA::verify(vgn, message, sign) << std::endl;

try{
    // random curve from Web
    ECDSA::BigInt a = 6; // 6
    ECDSA::BigInt b = 3; // 3
    ECDSA::BigInt m = 11; //11
    ECDSA::BigInt x = 9; // 9
    ECDSA::BigInt y = 4; // 4
    Point<ECDSA::BigInt> G(x, y, a, b, m);
    ECDSA::BigInt q = 5; // 5
    ECDSA::BigInt privkey;
    Point<ECDSA::BigInt> pubkey;

    // {
    //     privkey = 4;
    //     pubkey = privkey * G;
    // }

    std::string message("Hello World!");

    ECDSA::SignStruct sgn;
    ECDSA::signature sign;
    {
        CATCHTHAT:
        try{
            privkey = ECDSA::get_secret(q, G);
            pubkey = privkey * G;
            sgn = ECDSA::SignStruct(privkey, q, G);
            sign = ECDSA::sign(sgn, message);
        } catch(const char* e){
            goto CATCHTHAT;
        }
    }

    ECDSA::VerifyStruct vgn(pubkey, q, G);
    auto is_valid = ECDSA::verify(vgn, message, sign);

    std::cout << "Pubkey: " << pubkey << std::endl;
    std::cout << "Privkey: " << privkey << std::endl;
    if(is_valid) {
        std::cout << "Signature valid" << std::endl;
    } else {
        std::cout << "Signature not valid" << std::endl;
    }

} catch (const char* e){
    std::cout << e << std::endl;
}
}