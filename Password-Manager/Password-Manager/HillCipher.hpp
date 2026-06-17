#pragma once
#include "Cipher.hpp"
#include "Matrix.hpp"

class HillCipher : public Cipher {
public:
	//lifrcycle
	HillCipher() = delete;
	HillCipher(const Matrix& key);

	//overrides
	char* encrypt(const char* text) const override;
	char* decrypt(const char* text) const override;
	void serialize(std::ostream& out) const override;
	HillCipher* clone() const override;
	const char* getType() const override;

private:
	Matrix key;

	//private helpers
	char* normalizeText(const char* text) const; //makes the text in UPPER CASE and adds padding 'X' if needed
};