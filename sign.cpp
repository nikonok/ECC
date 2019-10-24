#include <iostream>

#include <ecc/ecc.h>

#include <cryptopp/sha.h>
#include <cryptopp/filters.h>
#include <cryptopp/hex.h>
#include <cryptopp/osrng.h>

constexpr char hexmap[] = {'0', '1', '2', '3', '4', '5', '6', '7',
                           '8', '9', 'a', 'b', 'c', 'd', 'e', 'f'};

std::string hexStr(unsigned char *data, int len)
{
  std::string s(len * 2, ' ');
  for (int i = 0; i < len; ++i) {
    s[2 * i]     = hexmap[(data[i] & 0xF0) >> 4];
    s[2 * i + 1] = hexmap[data[i] & 0x0F];
  }
  return s;
}

//// SHA256 of string to BitInt
std::string SHA256(std::string msg) {
    CryptoPP::SHA256 hash;
    std::string result;

	CryptoPP::StringSource(msg, true, 
		new CryptoPP::HashFilter(hash,
        new CryptoPP::HexEncoder(
			new CryptoPP::StringSink(result))
		)
	);
    
    return result;
}

int main(){
    uint8_t publicKey[ECC_BYTES+1]; 
    uint8_t privateKey[ECC_BYTES];

    std::cout << "Is generated: " << ecc_make_key(publicKey, privateKey) << std::endl;

    std::cout << "Privkey: " << hexStr(privateKey, ECC_BYTES) << std::endl;
    std::cout << "Pubkey: " << hexStr(publicKey, ECC_BYTES+1) << std::endl;

    std::string msg("Hello, world");
    auto hashed_msg = SHA256(msg);

    uint8_t signature[ECC_BYTES*2];
    std::cout << "Is signed: " << ecdsa_sign(privateKey, reinterpret_cast<const uint8_t*>(hashed_msg.data()), signature) << std::endl;

    std::cout << "Is valid sign: " << ecdsa_verify(publicKey, reinterpret_cast<const uint8_t*>(hashed_msg.data()), signature) << std::endl;
}