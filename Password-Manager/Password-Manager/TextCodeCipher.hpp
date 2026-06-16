#pragma once
#include "Cipher.hpp"
#define LETTER_SET_LEN 256 //max number of letters used by the cipher

class TextCodeCipher : public Cipher {
public:
	//lifecycle
	TextCodeCipher() = delete;
	TextCodeCipher(const char* filePath);
	TextCodeCipher(const TextCodeCipher& other);
	TextCodeCipher& operator= (const TextCodeCipher& other);
	~TextCodeCipher();

	//overrides
	char* encrypt(const char* text) const override;
	char* decrypt(const char* text) const override;
	void serialize(std::ostream& out) const override;
	TextCodeCipher* clone() const override;
	const char* getType() const override;

private:
	char* filePath;
	int firstOccurance[LETTER_SET_LEN];
	char* sourceText;

	//private helpers
	void buildDictionary(const char* text);
};