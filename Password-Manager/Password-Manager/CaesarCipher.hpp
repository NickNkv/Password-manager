#pragma once
#include "Cipher.hpp"

class CaeserCipher : public Cipher {
public:
	CaeserCipher(int shift);

	//overrides
	char* encrypt(const char* text) const override;
	char* decrypt(const char* text) const override;
	void serialize(std::ostream& out) const override;
	CaeserCipher* clone() const override;
	const char* getType() const override;

private:
	int shift;
};