#pragma once
#include <cstring>
#include "CryptoKey.hpp"

// TODO документирующие комментарии
// TODO перегрузка операций присваивания
// TODO вывод на экран :( (хранить в файле и тип данных ?)
// TODO сравнение объектов
// TODO разобраться с move конструкторами и операторами

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

	// TODO Будет ли мешать не в vs 22 ?
	Decryptor(const Decryptor& other) {
		memcpy_s(key, sizeof(key), other.key, sizeof(key));
		filename = other.filename;
	}


	// TODO нужно ли все это ?
	Decryptor(Decryptor&& other) noexcept {
		//enc_file = std::move(other.enc_file);
		filename = std::move(other.filename);
		fin = std::move(other.fin);
		memcpy_s(key, sizeof(key), other.key, sizeof(key));
	}

	~Decryptor() { if (fin.is_open()) fin.close(); }

	Decryptor& operator=(Decryptor&& other) noexcept {
		if (this != &other) {
			//enc_file = std::move(other.enc_file);
			filename = std::move(other.filename);
			memcpy_s(key, sizeof(key), other.key, sizeof(key));
		}
		return *this;
	}

	template<typename T>
	Decryptor& operator>>(T& out) {
		//std::ifstream fin(filename, std::ios::in | std::ios::binary);
		if (!fin.is_open())
			fin.open(filename, std::ios::in | std::ios::binary);
		//if (!check_file(fin)) throw std::exception("file error");
		check_file(fin);
		unsigned char tmp[sizeof(out)];
		fin.read((char*)tmp, sizeof(out));
		unsigned char* p = reinterpret_cast<unsigned char*>(&out);
		for (int i = 0; i < sizeof(out); i++)
			tmp[i] = key[tmp[i]];
			
		memcpy_s(&out, sizeof(out), tmp, sizeof(out));
		//fin.close();
		return *this;
	}

	// out обязательно nullptr
	template<>
	Decryptor& operator>>(char*& out) {
		//std::ifstream fin(filename, std::ios::in | std::ios::binary);
		if (!fin.is_open())
			fin.open(filename, std::ios::in | std::ios::binary);
		//if (!check_file(fin)) throw std::exception("file error");
		check_file(fin);
		uint16_t len;
		fin.read((char*)&len, sizeof(len));
		//std::cout << len << std::endl;
		unsigned char* tmp = new unsigned char[len];
		if (!out) delete[] out;
		out = new char [len + 1];
		fin.read((char*)tmp, len);
		for (uint16_t i = 0; i < len; i++)
			out[i] = key[tmp[i]];

		delete[] tmp;
		out[len] = 0;
		//fin.close();
		return *this;
	}


	template<>
	Decryptor& operator>>(std::string& out) {
		//std::ifstream fin(filename, std::ios::in | std::ios::binary);
		if (!fin.is_open())
			fin.open(filename, std::ios::in | std::ios::binary);
		//if (!check_file(fin)) throw std::exception("file error");
		check_file(fin);
		uint16_t len;
		fin.read((char*)&len, sizeof(len));
		out.resize(len);
		unsigned char* tmp = new unsigned char[len];
		fin.read((char*)tmp, len);
		for (uint16_t i = 0; i < len; i++)
			out[i] = key[tmp[i]];
		delete[] tmp;
		//fin.close();
		return *this;
	}

	Encryptor make_encryptor() const;
};


