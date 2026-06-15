#pragma once
#include <iostream>
#include "Cipher.hpp"

class CipherFactory {
public:
	//for createCipher(): in our case out will be std::cout and in -> std::cin
	static Cipher* createCipher(std::ostream& out, std::istream& in); 
	static Cipher* readCipher(std::istream& in);
};