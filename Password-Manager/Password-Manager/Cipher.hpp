#pragma once
#include <iostream>

class Cipher {
public:
	virtual char* encrypt(const char* text) const = 0;
	virtual char* decrypt(const char* text) const = 0;
	virtual void serialize(std::ostream& out) const = 0;
	virtual const char* serializeToText() const = 0;
	
	virtual Cipher* clone() const = 0;
	virtual const char* getType() const = 0;

	virtual ~Cipher() {};
};