#include "CaesarCipher.hpp"
#include "Utils.hpp"
#include <iostream>
#define _CRT_SECURE_NO_WARNINGS
#define BASE_LIMIT 32
#define CEIL_LIMIT 126  //ASCII in range of [BASE_LIMIT, CEIL_LIMIT]

CaeserCipher::CaeserCipher(int shift)
{
	this->shift = shift;
}

char* CaeserCipher::encrypt(const char* text) const
{
	if (!text || strlen(text) == 0) {
		throw std::invalid_argument("Text can not be nullptr or empty!");
	}

	size_t textLength = strlen(text);
	char* encryptedText = new char[textLength + 1];

	//we work with ASCII 32 to 126 => 95 symbols in total
	//we will shift the ASCII list "down" with 32 
	//then we can use a module function to traverse the ASCII list
	//this way we can have 95k + n options and thus we do not impose a limit on the shift param

	for (size_t i = 0; i < textLength; i++) {
		char current = text[i];

		if (current < BASE_LIMIT || current > CEIL_LIMIT) {
			delete[] encryptedText;
			throw std::invalid_argument("Invalid ASCII characters!");
		}

		int normalised = current - BASE_LIMIT;
		normalised = utils::mod(normalised + this->shift, CEIL_LIMIT - BASE_LIMIT + 1);
		encryptedText[i] = normalised + BASE_LIMIT;
	}

	encryptedText[textLength] = '\0';
	return encryptedText;
}

char* CaeserCipher::decrypt(const char* text) const
{
	return nullptr;
}

void CaeserCipher::serialize(std::ostream& out) const
{
}

CaeserCipher* CaeserCipher::clone() const
{
	return nullptr;
}

const char* CaeserCipher::getType() const
{
	return nullptr;
}

