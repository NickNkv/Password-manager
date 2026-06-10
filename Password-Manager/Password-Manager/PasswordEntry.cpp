#define _CRT_SECURE_NO_WARNINGS
#include "PasswordEntry.hpp"

//lifecycle
PasswordEntry::PasswordEntry(const char* website, const char* username, const char* password, const Cipher& cipher)
{
	//data validation
	if (!website || strlen(website) == 0) {
		throw std::invalid_argument("Website can not be nullptr or empty!");
	}
	if (!username || strlen(username) == 0) {
		throw std::invalid_argument("Username can not be nullptr or empty!");
	}
	if (!password || strlen(password) == 0) {
		throw std::invalid_argument("Password can not be nullptr or empty!");
	}

	//saving data
	this->website = new char[strlen(website) + 1];

	this->username = new (std::nothrow) char[strlen(username) + 1];
	if (!this->username) {
		delete[] this->website;
		throw std::bad_alloc();
	}

	strcpy(this->website, website);
	strcpy(this->username, username);

	//clone() can throw an exception
	try {
		this->cipher = cipher.clone();
		
	}
	catch (...) {
		delete[] this->website;
		delete[] this->username;
		throw;
	}

	//encrypt() can throw an exception
	try {
		this->encryptedPassword = this->cipher->encrypt(password);
	}
	catch (...) {
		delete[] this->website;
		delete[] this->username;
		delete this->cipher;
		throw;
	}
}

PasswordEntry::PasswordEntry(const PasswordEntry& other)
{
	this->website = new char[strlen(other.website) + 1];

	this->username = new (std::nothrow) char[strlen(other.username) + 1];
	if (!this->username) {
		delete[] this->website;
		throw std::bad_alloc();
	}

	this->encryptedPassword = new (std::nothrow) char[strlen(other.encryptedPassword) + 1];
	if (!this->encryptedPassword) {
		delete[] this->website;
		delete[] this->username;
		throw std::bad_alloc();
	}

	//clone() can throw an exception
	try {
		this->cipher = other.cipher->clone();
	}
	catch (...) {
		delete[] this->website;
		delete[] this->username;
		delete[] this->encryptedPassword;
		throw;
	}

	strcpy(this->website, other.website);
	strcpy(this->username, other.username);
	strcpy(this->encryptedPassword, other.encryptedPassword);
}

PasswordEntry& PasswordEntry::operator=(const PasswordEntry& other)
{
	if (this != &other) {
		
	}

	return *this;
}
