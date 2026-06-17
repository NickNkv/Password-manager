#define _CRT_SECURE_NO_WARNINGS
#include "HillCipher.hpp"
#include "Utils.hpp"
#include <iostream>

//lifecycle
HillCipher::HillCipher(const Matrix& key) : key(key)
{
	//is the matrix square
	if (key.getRows() != key.getCols()) {
		throw std::invalid_argument("Key matrix must be square!");
	}

	if (!key.isInvertableMod26()) {
		throw std::invalid_argument("Matrix must be invertable modulo 26!");
	}
}

//overrides
char* HillCipher::encrypt(const char* text) const
{
	char* normalized = normalizeText(text);
	size_t length = strlen(normalized);
	size_t n = this->key.getRows();
	char* encryptedText = new (std::nothrow) char[length + 1];
	if (!encryptedText) {
		delete[] normalized;
		throw std::bad_alloc();
	}

	for (size_t block = 0; block < length; block += n) {
		Matrix vector(n, 1);

		//text block to vector
		for (size_t i = 0; i < n; i++) {
			vector[i][0] = normalized[block + i] - 'A';
		}

		//multiply by key
		Matrix encrypted(1, 1);
		try {
			encrypted = key * vector;
		}
		catch (...) {
			delete[] normalized;
			delete[] encryptedText;
			throw;
		}

		//normalize modulo 26
		for (size_t i = 0; i < n; i++) {
			int value = utils::mod(encrypted[i][0], 26);
			encryptedText[block + i] = value + 'A';
		}
	}

	encryptedText[length] = '\0';
	delete[] normalized;
	return encryptedText;
}

char* HillCipher::decrypt(const char* text) const
{
	if (!text || strlen(text) == 0) {
		throw std::invalid_argument("Text can not be nullptr or empty!");
	}

	size_t textLength = strlen(text);
	size_t n = this->key.getRows();

	if (textLength % n != 0) {
		throw std::invalid_argument("Invalid cipher text!");
	}

	Matrix inverseKey = this->key.inverseMod26();
	char* decryptedText = new char[textLength + 1];

	for (size_t block = 0; block < textLength; block += n) {
		Matrix vector(n, 1);

		for (size_t i = 0; i < n; i++) {
			char current = text[block + i];

			if (current < 'A' || current > 'Z') {
				delete[] decryptedText;
				throw std::invalid_argument("Only letters A-Z are allowed!");
			}

			vector[i][0] = current - 'A';
		}

		Matrix decrypted = inverseKey * vector;

		for (size_t i = 0; i < n; i++) {
			int value = utils::mod(decrypted[i][0], 26);
			decryptedText[block + i] = value + 'A';
		}
	}

	decryptedText[textLength] = '\0';
	return decryptedText;
}

void HillCipher::serialize(std::ostream& out) const
{
	if (!out) {
		throw std::invalid_argument("Invalid output stream!");
	}

	out << getType() << '\n';
	this->key.serialize(out);

	if (!out) {
		throw std::runtime_error("Error while writing cipher!");
	}
}

HillCipher* HillCipher::clone() const
{
	return new HillCipher(*this);
}

const char* HillCipher::getType() const
{
	return "HILL";
}

//private helpers
char* HillCipher::normalizeText(const char* text) const
{
	if (!text || strlen(text) == 0) {
		throw std::invalid_argument("Text can not be nullptr or empty!");
	}

	size_t textLength = strlen(text);
	size_t blockSize = this->key.getRows();
	size_t paddedLength = textLength;

	while (paddedLength % blockSize != 0) {
		paddedLength++;
	}

	char* result = new char[paddedLength + 1];
	for (size_t i = 0; i < textLength; i++) {
		char current = text[i];

		if (current >= 'a' && current <= 'z') {
			current = current - 'a' + 'A';
		}
		if (current < 'A' || current > 'Z') {
			delete[] result;
			throw std::invalid_argument("Only letters A-Z are allowed!");
		}

		result[i] = current;
	}

	for (size_t i = textLength; i < paddedLength; i++) {
		result[i] = 'X';
	}

	result[paddedLength] = '\0';
	return result;
}