#define _CRT_SECURE_NO_WARNINGS
#include "VigenereCipher.hpp"
#include <iostream>
#include "Utils.hpp"

#define BASE_LIMIT 32
#define CEIL_LIMIT 126  //ASCII in range of [BASE_LIMIT, CEIL_LIMIT]

VigenereCipher::VigenereCipher(const char* keyword)
{
	if (!keyword || strlen(keyword) == 0) {
		throw std::invalid_argument("Keyword can not be nullptr or empty!");
	}

	this->keyword = new char[strlen(keyword) + 1];
	strcpy(this->keyword, keyword);
}

VigenereCipher::VigenereCipher(const VigenereCipher& other)
{
	this->keyword = new char[strlen(other.keyword) + 1];
	strcpy(this->keyword, other.keyword);
}

VigenereCipher& VigenereCipher::operator=(const VigenereCipher& other)
{
	if (this != &other) {
		char* temp = new char[strlen(other.keyword) + 1];
		strcpy(temp, other.keyword);

		//point of no return
		delete[] this->keyword;
		this->keyword = temp;
		temp = nullptr;
	}

	return *this;
}

VigenereCipher::~VigenereCipher()
{
	delete[] this->keyword;
}

//overrides
char* VigenereCipher::encrypt(const char* text) const
{
	if (!text || strlen(text) == 0) {
		throw std::invalid_argument("Text can not be nullptr or empty!");
	}

	size_t textLength = strlen(text);
	size_t keyLength = strlen(this->keyword);
	char* encryptedText = new char[textLength + 1];

	for (size_t i = 0; i < textLength; i++) {
		char current = text[i];

		if (current < BASE_LIMIT || current > CEIL_LIMIT) {
			delete[] encryptedText;
			throw std::invalid_argument("Unknown ASCII character!");
		}

		//we work with ASCII 32 to 126 => 95 symbols in total
		//we will shift the ASCII list "down" with 32 
		//then we can use a module function to traverse the ASCII list
		int shift = this->keyword[i % keyLength] - BASE_LIMIT;
		int normalised = current - BASE_LIMIT;
		normalised = utils::mod(normalised + shift, CEIL_LIMIT - BASE_LIMIT + 1);
		encryptedText[i] = normalised + BASE_LIMIT;
	}

	encryptedText[textLength] = '\0';
	return encryptedText;
}

char* VigenereCipher::decrypt(const char* text) const
{
	if (!text || strlen(text) == 0) {
		throw std::invalid_argument("Text can not be nullptr or empty!");
	}

	size_t textLength = strlen(text);
	size_t keyLength = strlen(this->keyword);
	char* decryptedText = new char[textLength + 1];

	for (size_t i = 0; i < textLength; i++) {
		char current = text[i];

		if (current < BASE_LIMIT || current > CEIL_LIMIT) {
			delete[] decryptedText;
			throw std::invalid_argument("Unknown ASCII character!");
		}

		int shift = this->keyword[i % keyLength] - BASE_LIMIT;
		int normalised = current - BASE_LIMIT;
		normalised = utils::mod(normalised - shift, CEIL_LIMIT - BASE_LIMIT + 1);
		decryptedText[i] = normalised + BASE_LIMIT;
	}

	decryptedText[textLength] = '\0';
	return decryptedText;
}

void VigenereCipher::serialize(std::ostream& out) const
{
	if (!out) {
		throw std::invalid_argument("Invalid output stream!");
	}

	out << getType() << '\n';
	out << this->keyword << '\n';

	if (!out) {
		throw std::runtime_error("Error while writing cipher!");
	}
}

const char* VigenereCipher::serializeToText() const
{
	return nullptr;
}

VigenereCipher* VigenereCipher::clone() const
{
	return new VigenereCipher(*this);
}

const char* VigenereCipher::getType() const
{
	return "VIGENERE";
}


