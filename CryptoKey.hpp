#pragma once
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <ctime>
#include <exception>

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