#pragma once
#include "Cipher.hpp"

class VigenereCipher : public Cipher {
public:
	//lifecycle
	VigenereCipher() = delete;
	VigenereCipher(const char* keyword);
	VigenereCipher(const VigenereCipher& other);
	VigenereCipher& operator= (const VigenereCipher& other);
	~VigenereCipher();

	//overrides
	char* encrypt(const char* text) const override;
	char* decrypt(const char* text) const override;
	void serialize(std::ostream& out) const override;
	VigenereCipher* clone() const override;
	const char* getType() const override;

private:
	char* keyword;
};