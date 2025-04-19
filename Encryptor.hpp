#pragma once
#include "CryptoKey.hpp"

class Decryptor;

class Encryptor : public CryptoKey {
protected:
	std::string filename;

	void check_file(std::ofstream& file) {
		if (!file.is_open()) {
			file.clear();
			// open default file
			filename = "encrypted.enc";
			file.open(filename, std::ios::out | std::ios::app | std::ios::binary);
			if (!file.is_open()) throw fexception;
		}
	}

public:
	Encryptor(const char* filename) : filename(filename) {
		// truncate if file exist
		std::ofstream file(filename);
		if (file.is_open()) file.close();
		make_key();
	}

	Encryptor(const char* filename, const char* keyfile) : filename(filename) {
		this->read_key(keyfile);
	}

	Encryptor(const char* encrypted_file, const unsigned char* key) : filename(encrypted_file) {
		//std::ofstream file(filename);
		//if (file.is_open()) file.close();
		for (int i = 0; i < 256; i++)
			this->key[key[i]] = i;
	}

	Encryptor(const Encryptor& other) {
		filename = other.filename;
		memcpy_s(key, sizeof(key), other.key, sizeof(key));
	}

	template <typename T>
	Encryptor& operator<<(T data) {
		std::ofstream fout(filename, std::ios::out | std::ios::app | std::ios::binary);
		check_file(fout);
		unsigned char* p = reinterpret_cast<unsigned char*>(&data);
		for (int i = 0; i < sizeof(data); i++)
			fout.write(reinterpret_cast<char*>(&key[p[i]]), sizeof(char));
		fout.close();
		return *this;
	}

	template<>
	Encryptor& operator<<(const char* data) {
		std::ofstream fout(filename, std::ios::out | std::ios::app | std::ios::binary);
		check_file(fout);
		uint16_t len = (uint16_t)strlen(data);
		//if (len > 256) throw std::exception("long string");
		fout.write((char*)&len, sizeof(len));
		for (uint16_t i = 0; i < len; i++)
			fout.write((char*)&key[(unsigned char)data[i]], sizeof(char));
		fout.close();
		return *this;
	}

	//template<>// Ќе работает как специализаци€ шаблона
	Encryptor& operator<<(const std::string& data) {
		std::ofstream fout(filename, std::ios::out | std::ios::app | std::ios::binary);
		//if (!check_file(fout)) throw std::exception("file error");
		check_file(fout);
		uint16_t len = (uint16_t)data.length();
		fout.write((char*)&len, sizeof(len));
		for (uint16_t i = 0; i < len; i++)
			fout.write((char*)&key[(unsigned char)data[i]], sizeof(char));
		fout.close();
		return *this;
	}

	Decryptor make_decryptor() const;
};
