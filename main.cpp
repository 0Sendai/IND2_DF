#include <iostream>
#include <sstream>
#include "Encryptor.hpp"
#include "Decryptor.hpp"

using namespace std;
int main() {
	setlocale(LC_ALL, "RU");
	// 1.
	{
		cout << "1.\n";
		Encryptor enc("test.enc");
		try {
			enc << 152 << -24.5 << "hello" << false;
		}
		catch (FileException)
		{
			cout << "File access error\n";
			return 1;
		}
		Decryptor dec = enc.make_decryptor();
		int x1; double x2; char* x3; bool x4;
		try {
			dec >> x1 >> x2 >> x3 >> x4;
		}
		catch (FileException)
		{
			cout << "File access error\n";
			return 1;
		}
		
		cout << boolalpha << x1 << ' ' << x2 << ' ' << x3 << ' ' << x4 << endl;
		try {
			enc << -235;
			dec >> x1;
		}
		catch (FileException)
		{
			cout << "File access error\n";
			return 1;
		}
		
		cout << x1 << endl << endl;
		delete[] x3;

	}

	/*
	result:
	1.
	152 -24.5 hello false
	-235
	*/

	// 2.
	{
		cout << "2.\n";
		ifstream fin("text.txt");
		if (!fin.is_open()) {
			cout << "File access error\n";
			return 1;
		}
		stringstream str;
		str << fin.rdbuf();
		fin.close();

		Encryptor enc("test.enc");
		try {
			enc << str.str();
		}
		catch (FileException)
		{
			cout << "File access error\n";
			return 1;
		}
		enc.write_key("key.enc");
		cout << "File encrypted!\n";
	}
	{
		Decryptor dec("test.enc", "key.enc");
		string s; 
		try {
			dec >> s;
		}
		catch (FileException)
		{
			cout << "File access error\n";
			return 1;
		}
		cout << "Decrypted file:\n" << s << endl;
		Encryptor enc = dec.make_encryptor();
		try {
			enc << "yet another string";
		}
		catch (FileException)
		{
			cout << "File access error\n";
			return 1;
		}
		char* s2;
		try {
			dec >> s2;
		}
		catch (FileException)
		{
			cout << "File access error\n";
			return 1;
		}
		cout << '\n' << s2 << endl << endl;
		delete[] s2;
	}

	/*
	result:
	2.
	File encrypted!
	Decrypted file:
	Мороз и солнце; день чудесный!
	Еще ты дремлешь, друг прелестный -
	Пора, красавица, проснись:
	Открой сомкнуты негой взоры
	Навстречу северной Авроры,
	Звездою севера явись!

	yet another string
	*/

	// 3.
	{
		// исключение
		cout << "3.\n";
		Decryptor dec("ff.enc", "key.enc");
		int x;
		try {
			dec >> x;
		}
		catch (FileException)
		{
			cout << "File access error\n";
			return 1;
		}
	}

	/*
	result:
	3.
	File access error
	*/
}