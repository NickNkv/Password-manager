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
		char* tempWebsite = new char[strlen(other.website) + 1];
		
		char* tempUsername = new (std::nothrow) char[strlen(other.username) + 1];
		if (!tempUsername) {
			delete[] tempWebsite;
			throw std::bad_alloc();
		}

		char* tempEncryptedPassword = new (std::nothrow) char[strlen(other.encryptedPassword) + 1];
		if (!tempEncryptedPassword) {
			delete[] tempWebsite;
			delete[] tempUsername;
			throw std::bad_alloc();
		}

		//clone() can throw an exception
		Cipher* tempCipher = nullptr;
		try {
			tempCipher = other.cipher->clone();
		}
		catch (...) {
			delete[] tempWebsite;
			delete[] tempUsername;
			delete[] tempEncryptedPassword;
			throw;
		}

		//copying data
		strcpy(tempWebsite, other.website);
		strcpy(tempUsername, other.username);
		strcpy(tempEncryptedPassword, other.encryptedPassword);

		//point of no return!
		delete[] this->website;
		delete[] this->username;;
		delete[] this->encryptedPassword;

		this->website = tempWebsite;
		tempWebsite = nullptr;
		this->username = tempUsername;
		tempUsername = nullptr;
		this->encryptedPassword = tempEncryptedPassword;
		tempEncryptedPassword = nullptr;
		this->cipher = tempCipher;
		tempCipher = nullptr;
	}

	return *this;
}

PasswordEntry::~PasswordEntry()
{
	delete[] this->website;
	delete[] this->username;
	delete[] this->encryptedPassword;
	delete this->cipher;
}

//getters
const char* PasswordEntry::getWebsite() const
{
	return this->website;
}

const char* PasswordEntry::getUsername() const
{
	return this->username;
}

const char* PasswordEntry::getEncryptedPassword() const
{
	return this->encryptedPassword;
}

const Cipher* PasswordEntry::getCipher() const
{
	return this->cipher;
}

//setters
void PasswordEntry::updatePassword(const char* newPassword)
{
	if (!newPassword || strlen(newPassword) == 0) {
		throw std::invalid_argument("Password can not be nullptr or empty!");
	}

	//encrypt() can throw an ecxeption!
	char* tempNewEncryptedPass = this->cipher->encrypt(newPassword);

	//point of no return!
	delete[] this->encryptedPassword;
	this->encryptedPassword = tempNewEncryptedPass;
	tempNewEncryptedPass = nullptr;
}

//helpers
const char* PasswordEntry::getPassword() const
{
	//decrypt() can throw an exception!
	return this->cipher->decrypt(this->encryptedPassword);
}

void PasswordEntry::serialize(std::ostream& out) const
{
	if (!out) {
		throw std::runtime_error("Invalid output stream!");
	}

	out << "ENTRY\n";
	out << this->website << '\n';
	out << this->username << '\n';

	this->cipher->serialize(out);
	out << this->encryptedPassword << '\n';

	if (!out) {
		throw std::runtime_error("Error while writing a password entry! Potentially broken entry!");
	}
}

bool PasswordEntry::isMatch(const char* website, const char* username) const
{
	if (!website || !username) {
		return false;
	}

	return strcmp(this->website, website) == 0 && strcmp(this->username, username) == 0;
}