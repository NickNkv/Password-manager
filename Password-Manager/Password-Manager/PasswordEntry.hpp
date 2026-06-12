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
	const char* getEncryptedPassword() const;
	const Cipher* getCipher() const;

	//setters
	void updatePassword(const char* newPassword);

	//helpers
	const char* getPassword() const;
	void serialize(std::ostream& out) const;
	bool isMatch(const char* website, const char* username) const;
	static PasswordEntry* createFromEncrypted(const char* website, const char* username, const char* encryptedPassword, Cipher* cipher);

private:
	char* website;
	char* username;
	char* encryptedPassword; //PasswordEntry keeps ONLY the ENCRYPTED password
	Cipher* cipher;

	//helper ctor
	PasswordEntry(const char* website, const char* username, const char* password, Cipher* cipher, bool alreadyEncrypted);
};