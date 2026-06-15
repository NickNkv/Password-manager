#define _CRT_SECURE_NO_WARNINGS
#include "TextCodeCipher.hpp"
#include <iostream>
#include "Utils.hpp"

//lifecycle
TextCodeCipher::TextCodeCipher(const char* filePath)
{
	if (!filePath || strlen(filePath) == 0) {
		throw std::invalid_argument("File path can not be nullptr or empty!");
	}

	this->sourceText = utils::readFile(filePath);
	this->filePath = new (std::nothrow) char[strlen(filePath) + 1];
	if (!this->filePath) {
		delete[] this->sourceText;
		throw std::bad_alloc();
	}

	strcpy(this->filePath, filePath);

	for (size_t i = 0; i < LETTER_SET_LEN; i++) {
		this->firstOccurance[i] = -1;
	}
	buildDictionary(this->sourceText);
}

TextCodeCipher::~TextCodeCipher()
{
}

//overrides
char* TextCodeCipher::encrypt(const char* text) const
{
	return nullptr;
}

char* TextCodeCipher::decrypt(const char* text) const
{
	return nullptr;
}

void TextCodeCipher::serialize(std::ostream& out) const
{
}

TextCodeCipher* TextCodeCipher::clone() const
{
	return nullptr;
}

const char* TextCodeCipher::getType() const
{
	return nullptr;
}

//private helpers
void TextCodeCipher::buildDictionary(const char* text)
{
	size_t length = strlen(text);
	for (int i = 0; i < length; i++) {
		unsigned char symbol = (unsigned char)text[i];

		if (this->firstOccurance[symbol] == -1) {
			this->firstOccurance[symbol] = i;
		}
	}
}