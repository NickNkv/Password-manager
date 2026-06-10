#pragma once
#include <iostream>
#include "Cipher.hpp"

class CipherFactory {
public:
	static Cipher* createCipher();
	static Cipher* readCipher(std::istream& in);
};