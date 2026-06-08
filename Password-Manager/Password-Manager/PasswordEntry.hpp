#pragma once
#include <iostream>
#include "Cipher.hpp"

class PasswordEntry {
public:
	PasswordEntry() = delete;
	PasswordEntry(const char* website, const char* user, const char* password, const Cipher& cipher);
	PasswordEntry(const PasswordEntry& other);
	PasswordEntry& operator= (const PasswordEntry& other);
	~PasswordEntry();

	//засега няма да пиша сетъри, не знам дали са ми нужни или дали не биха били опасни
	//за валидността на обектите
	
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
	char* password;
	Cipher* cipher;
};