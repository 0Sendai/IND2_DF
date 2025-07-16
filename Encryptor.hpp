#pragma once
#include "CryptoKey.hpp"
class Decryptor;

/// <summary>
/// Шифрование строк только в кодировке ascii или windows-1251
/// </summary>
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
		// append to encrypted_file
		for (int i = 0; i < 256; i++)
			this->key[key[i]] = i;
	}

	Encryptor(const Encryptor& other) {
		filename = other.filename;
		memcpy_s(key, sizeof(key), other.key, sizeof(key));
	}

	Encryptor(Encryptor&& other) noexcept {
		filename = std::move(other.filename);
		memcpy_s(key, sizeof(key), other.key, sizeof(key));
	}

	Encryptor& operator=(const Encryptor& other) {
		if (this != &other) {
			filename = other.filename;
			memcpy_s(key, sizeof(key), other.key, sizeof(key));
		}
		return *this;
	}

	Encryptor& operator=(Encryptor&& other) noexcept{
		if (this != &other) {
			filename = std::move(other.filename);
			memcpy_s(key, sizeof(key), other.key, sizeof(key));
		}
		return *this;
	}

	bool operator==(const Encryptor& other) const {
		return !(memcmp(key, other.key, sizeof(key))) && filename == other.filename;
	}

	bool operator!=(const Encryptor& other) const {
		return !(*this == other);
	}

	template <typename T>
	Encryptor& operator<<(T data) {
		std::ofstream fout(filename, std::ios::out | std::ios::app | std::ios::binary);
		check_file(fout);
		unsigned char* p = reinterpret_cast<unsigned char*>(&data);
		for (int i = 0; i < sizeof(data); i++)
			fout.write((char*)&key[p[i]], sizeof(char));
		fout.close();
		return *this;
	}

	template<>
	Encryptor& operator<<(const char* data) {
		std::ofstream fout(filename, std::ios::out | std::ios::app | std::ios::binary);
		check_file(fout);
		uint16_t len = (uint16_t)strlen(data);
		fout.write((char*)&len, sizeof(len));
		for (uint16_t i = 0; i < len; i++)
			fout.write((char*)&key[(unsigned char)data[i]], sizeof(char));
		fout.close();
		return *this;
	}

	//template<>// Не работает как специализация шаблона
	Encryptor& operator<<(const std::string& data) {
		std::ofstream fout(filename, std::ios::out | std::ios::app | std::ios::binary);
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
