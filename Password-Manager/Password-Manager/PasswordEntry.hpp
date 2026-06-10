#pragma once
#include <iostream>
#include "Cipher.hpp"

class PasswordEntry {
public:
	PasswordEntry() = delete;
	PasswordEntry(const char* website, const char* username, const char* password, const Cipher& cipher);
	PasswordEntry(const PasswordEntry& other);
	PasswordEntry& operator= (const PasswordEntry& other);
	~PasswordEntry();
	
	//getters
	const char* getWebsite() const;
	const char* getUsername() const;
	const char* getPassword() const;
	const Cipher* getCipher() const;

	//setters
	void updatePassword(const char* newPassword);

	//helpers
	const char* getEncryptedPassword() const;
	void serialize(std::ostream& out) const;
	bool isMatch(const char* website, const char* username) const;

private:
	char* website;
	char* username;
	char* encryptedPassword; //PasswordEntry не съхранява чистата парола, а само криптираната
	Cipher* cipher;
};