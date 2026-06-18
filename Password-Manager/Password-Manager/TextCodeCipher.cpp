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
	buildDictionary(sourceText);
}

TextCodeCipher::TextCodeCipher(const TextCodeCipher& other)
{
	this->sourceText = new char[strlen(other.sourceText) + 1];

	this->filePath = new (std::nothrow) char[strlen(other.filePath) + 1];
	if (!this->filePath) {
		delete[] this->sourceText;
		throw std::bad_alloc();
	}

	strcpy(this->sourceText, other.sourceText);
	strcpy(this->filePath, other.filePath);

	for (size_t i = 0; i < LETTER_SET_LEN; i++) {
		this->firstOccurance[i] = other.firstOccurance[i];
	}
}

TextCodeCipher& TextCodeCipher::operator=(const TextCodeCipher& other)
{
	if (this != &other) {
		char* tempSourceText = new char[strlen(other.sourceText) + 1];
		char* tempFilePath = new (std::nothrow) char[strlen(other.filePath) + 1];
		if (!tempFilePath) {
			delete[] tempSourceText;
			throw std::bad_alloc();
		}

		strcpy(tempSourceText, other.sourceText);
		strcpy(tempFilePath, other.filePath);

		//point of no return
		delete[] this->sourceText;
		this->sourceText = tempSourceText;
		tempSourceText = nullptr;

		delete[] this->filePath;
		this->filePath = tempFilePath;
		tempFilePath = nullptr;

		for (size_t i = 0; i < LETTER_SET_LEN; i++) {
			this->firstOccurance[i] = other.firstOccurance[i];
		}
	}

	return *this;
}

TextCodeCipher::~TextCodeCipher()
{
	delete[] this->sourceText;
	delete[] this->filePath;
}

//overrides
char* TextCodeCipher::encrypt(const char* text) const
{
	if (!text || strlen(text) == 0) {
		throw std::invalid_argument("Text can not be nullptr or empty!");
	}

	size_t textLength = strlen(text);
	size_t encryptedTextSize = 1; // '\0'

	//first we determine the size of the encrypted text
	for (size_t i = 0; i < textLength; i++) {
		unsigned char symbol = (unsigned char)text[i];

		if (this->firstOccurance[symbol] == -1) {
			throw std::invalid_argument("Unknown character! Unable to encrypt text!");
		}

		//could throw bad alloc
		char* tempIntToStr = utils::intToString(this->firstOccurance[symbol]);
		encryptedTextSize += strlen(tempIntToStr);

		if (i != textLength - 1) {
			encryptedTextSize++; // '|'
		}

		delete[] tempIntToStr;
	}

	//then we encrypt
	char* encryptedText = new char[encryptedTextSize];
	encryptedText[0] = '\0';
	for (size_t i = 0; i < textLength; i++) {
		unsigned char symbol = (unsigned char)text[i];

		char* tempIntToStr = nullptr;
		try {
			//could throw bad alloc
			tempIntToStr = utils::intToString(this->firstOccurance[symbol]);
		}
		catch (...) {
			delete[] encryptedText;
			throw;
		}

		strcat(encryptedText, tempIntToStr);
		if (i != textLength - 1) {
			strcat(encryptedText, "|");
		}

		delete[] tempIntToStr;
	}

	return encryptedText;
}

char* TextCodeCipher::decrypt(const char* text) const
{
	if (!text || strlen(text) == 0) {
		throw std::invalid_argument("Text can not be nullptr or empty!");
	}

	size_t textLength = strlen(text);
	char* decryptedText = new char[textLength + 1];
	size_t decryptedTextIndex = 0;
	int number = 0; //number form encrypted text 22|31|1|0

	for (size_t i = 0; ; i++) {
		if (text[i] == '|' || text[i] == '\0') {
			if (number < 0 || number >= (int)strlen(this->sourceText)) {
				delete[] decryptedText;
				throw std::invalid_argument("Invalid index in encrypted text!");
			}

			decryptedText[decryptedTextIndex] = sourceText[number];
			decryptedTextIndex++;
			number = 0;

			if (text[i] == '\0') {
				break;
			}
		}
		else if (text[i] >= '0' && text[i] <= '9') {
			number = number * 10 + (text[i] - '0');
		}
		else {
			delete[] decryptedText;
			throw std::invalid_argument("Invalid encrypted text!");
		}
	}

	decryptedText[decryptedTextIndex] = '\0';

	return decryptedText;
}

void TextCodeCipher::serialize(std::ostream& out) const
{
	if (!out) {
		throw std::invalid_argument("Invalid output stream!");
	}

	out << getType() << '\n';
	out << this->filePath << '\n';

	if (!out) {
		throw std::runtime_error("Error while writing cipher!");
	}
}

const char* TextCodeCipher::serializeToText() const
{
	return nullptr;
}

TextCodeCipher* TextCodeCipher::clone() const
{
	return new TextCodeCipher(*this);
}

const char* TextCodeCipher::getType() const
{
	return "TEXTCODE";
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