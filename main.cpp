#include "crypto.hpp"

int main() {
	Encryptor enc("test.enc");

	char b[] = "test";
	std::string c("hey");
	enc << 152 << 14.2 << false << "ping" << std::string("wow") << c;
	int x1; double x2; bool x3; char* x4 = nullptr, *x5 = nullptr; std::string x6, x7;
	Decryptor dec = enc.make_decryptor();
	dec >> x1 >> x2 >>  x3 >> x4 >> x5 >> x6;
	std::cout << std::boolalpha << x1 << ' ' << x2 << ' ' << x3 << ' ' << x4
		<< ' ' << x5 << ' ' << x6 << ' ' << std::endl;// x7;//<< std::endl;
	enc << "biba";
	dec >> x4;
	enc << "boba";
	dec >> x5;
	std::cout << x4 <<  ' ' << x5 << std::endl;
	delete[] x4; delete[] x5;
	return 0;
}