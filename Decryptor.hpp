#pragma once
#include <cstring>
#include "CryptoKey.hpp"


class Encryptor;

class Decryptor : public CryptoKey {
protected:
	std::string filename;
	std::ifstream fin;

	void check_file(std::ifstream& file) {
		if (!file.is_open()) {
			file.clear();
			// open default file
			filename = "encrypted.enc";
			file.open(filename, std::ios::in | std::ios::binary);
			if (!file.is_open()) throw fexception;
		}
	}
public:
	Decryptor() {};

	Decryptor(const char* encrypted_file, const unsigned char* key) : filename(encrypted_file) {
		for (int i = 0; i < 256; i++)
			this->key[key[i]] = i;
	}

	Decryptor(const char* encrypted_file, const char* key_filename) : filename(encrypted_file) {
		std::ifstream keyfile(key_filename, std::ios::in | std::ios::binary);

		unsigned char inv_key[256];
		keyfile.read((char*)inv_key, sizeof(inv_key));
		keyfile.close();
		for (int i = 0; i < 256; i++)
			key[inv_key[i]] = i;
	}
	
	Decryptor(const Decryptor& other) {
		memcpy_s(key, sizeof(key), other.key, sizeof(key));
		filename = other.filename;
	}
	
	Decryptor(Decryptor&& other) noexcept {
		filename = std::move(other.filename);
		fin = std::move(other.fin);
		memcpy_s(key, sizeof(key), other.key, sizeof(key));
	}

	~Decryptor() { if (fin.is_open()) fin.close(); }

	Decryptor& operator=(Decryptor&& other) noexcept{
		if (this != &other) {
			filename = std::move(other.filename);
			memcpy_s(key, sizeof(key), other.key, sizeof(key));
			if (fin.is_open()) {
				fin.close();
				fin.clear();
			}
		}
		return *this;
	}

	Decryptor& operator=(const Decryptor& other) {
		if (this != &other) {
			filename = other.filename;
			memcpy_s(key, sizeof(key), other.key, sizeof(key));
			if (fin.is_open()) {
				fin.close();
				fin.clear();
			}
		}
		return *this;
	}

	bool operator==(const Decryptor& other) const {
		return !memcmp(key, other.key, sizeof(key)) && filename == other.filename;
	}

	bool operator!=(const Decryptor& other) const {
		return !(*this == other);
	}

	template<typename T>
	Decryptor& operator>>(T& out) {
		if (!fin.is_open())
			fin.open(filename, std::ios::in | std::ios::binary);
		check_file(fin);
		unsigned char tmp[sizeof(out)];
		fin.read((char*)tmp, sizeof(out));
		unsigned char* p = reinterpret_cast<unsigned char*>(&out);
		for (int i = 0; i < sizeof(out); i++)
			tmp[i] = key[tmp[i]];
			
		memcpy_s(&out, sizeof(out), tmp, sizeof(out));
		return *this;
	}

	template<>
	Decryptor& operator>>(char*& out) {
		if (!fin.is_open())
			fin.open(filename, std::ios::in | std::ios::binary);
		check_file(fin);
		uint16_t len;
		fin.read((char*)&len, sizeof(len));
		unsigned char* tmp = new unsigned char[len];
		if (!out) delete[] out;
		out = new char [len + 1];
		fin.read((char*)tmp, len);
		for (uint16_t i = 0; i < len; i++)
			out[i] = key[tmp[i]];

		delete[] tmp;
		out[len] = 0;
		return *this;
	}


	template<>
	Decryptor& operator>>(std::string& out) {
		if (!fin.is_open())
			fin.open(filename, std::ios::in | std::ios::binary);
		check_file(fin);
		uint16_t len;
		fin.read((char*)&len, sizeof(len));
		out.resize(len);
		unsigned char* tmp = new unsigned char[len];
		fin.read((char*)tmp, len);
		for (uint16_t i = 0; i < len; i++)
			out[i] = key[tmp[i]];
		delete[] tmp;
		return *this;
	}

	Encryptor make_encryptor() const;
};


