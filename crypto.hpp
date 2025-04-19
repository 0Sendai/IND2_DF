#pragma once
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <exception>
#include <cstring>

// TODO документирующие комментарии
// TODO перегрузка операций присваивания
// TODO вывод на экран :( (хранить в файле и тип данных ?)
// TODO сравнение объектов

class FileException : public std::exception {
private:
	const static constexpr char* message = "File access error";
public:
	const char* what() { return message; }
};

class CryptoKey {
protected:
	unsigned char key[256]{};
	FileException fexception;
public:
	void make_key() {
		std::srand(std::time(0));
		for (int i = 0; i < 256; i++)
			key[i] = i;
		for (int i = 0; i < 5000; i++) {
			int a = rand() % 256;
			int b = rand() % 256;
			std::swap(key[a], key[b]);
		}
	}
	void write_key(const char* filename) const {
		std::ofstream fout(filename, std::ios::out | std::ios::binary);
		if (!fout.is_open()) throw fexception;
		fout.write((char*)key, sizeof(key));
		fout.close();
	}

	void read_key(const char* filename) {
		std::ifstream fin(filename, std::ios::in | std::ios::binary);
		if (!fin.is_open()) throw fexception;
		fin.read((char*)key, sizeof(key));
		fin.close();
	}
	
};

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
		// ?
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

};

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
	Encryptor(const char* filename): filename(filename) {
		// truncate if file exist
		std::ofstream file(filename);
		if (file.is_open()) file.close();
		make_key();
	}

	Encryptor(const char* filename, const char* keyfile): filename(filename) {
		this->read_key(keyfile);
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

	//template<> Не работает как специализация шаблона
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

	Decryptor make_decryptor() const {
		return Decryptor(filename.c_str(), key);
	}
};

