#include "Encryptor.hpp"
#include "Decryptor.hpp"

 Decryptor Encryptor::make_decryptor() const{
	 return Decryptor(filename.c_str(), key);
}

 Encryptor Decryptor::make_encryptor() const {
	 return Encryptor(filename.c_str(), key);
 }