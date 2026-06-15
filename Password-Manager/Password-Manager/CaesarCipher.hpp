#pragma once
#include "Cipher.hpp"

class CaesarCipher : public Cipher {
public:
	CaesarCipher(int shift);

	//overrides
	char* encrypt(const char* text) const override;
	char* decrypt(const char* text) const override;
	void serialize(std::ostream& out) const override;
	CaesarCipher* clone() const override;
	const char* getType() const override;

private:
	int shift;
};