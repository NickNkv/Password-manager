#define _CRT_SECURE_NO_WARNINGS
#include "DataVault.hpp"
#include "CipherFactory.hpp"
#include "Utils.hpp"
#include <iostream>
#define ALLOC_STEP 8
#define BUFFER_LEN 256

//lifecycle
DataVault::DataVault(const Cipher& defaultCipher)
{
	this->allocated = ALLOC_STEP;
	this->size = 0;

	//clone() can throw an exception!
	this->defaultCipher = defaultCipher.clone();

	this->entries = new (std::nothrow) PasswordEntry * [this->allocated];
	if (!this->entries) {
		delete this->defaultCipher;
		throw std::bad_alloc();
	}

	for (size_t i = 0; i < this->allocated; i++) {
		this->entries[i] = nullptr;
	}
}

DataVault::DataVault(const DataVault& other)
{
	this->size = other.size;
	this->allocated = other.allocated;

	//clone() can throw an exception!
	this->defaultCipher = other.defaultCipher->clone();

	this->entries = new (std::nothrow) PasswordEntry * [this->allocated];
	if (!this->entries) {
		delete this->defaultCipher;
		throw std::bad_alloc();
	}

	for (size_t i = 0; i < this->allocated; i++) {
		if (!other.entries[i]) {
			this->entries[i] = nullptr;
			continue;
		}

		try {
			this->entries[i] = new PasswordEntry(*other.entries[i]);
		}
		catch(...) {
			for (size_t j = 0; j < i; j++) {
				delete this->entries[j];
			}
			delete[] this->entries;
			delete this->defaultCipher;
			throw;
		}
	}
}

DataVault& DataVault::operator=(const DataVault& other)
{
	if (this != &other) {
		//clone() can throw an exception
		Cipher* tempCipher = other.defaultCipher->clone();

		PasswordEntry** tempEntries = new (std::nothrow) PasswordEntry * [other.allocated];
		if (!tempEntries) {
			delete tempCipher;
			throw std::bad_alloc();
		}

		for (size_t i = 0; i < other.allocated; i++) {
			if (!other.entries[i]) {
				tempEntries[i] = nullptr;
				continue;
			}

			try {
				tempEntries[i] = new PasswordEntry(*other.entries[i]);
			}
			catch (...) {
				for (size_t j = 0; j < i; j++) {
					delete tempEntries[j];
				}
				delete[] tempEntries;
				delete tempCipher;
				throw;
			}
		}

		//point of no return!
		for (size_t i = 0; i < this->allocated; i++) {
			delete this->entries[i];
		}
		delete[] this->entries;

		delete this->defaultCipher;

		this->size = other.size;
		this->allocated = other.allocated;
		this->defaultCipher = tempCipher;
		this->entries = tempEntries;

		tempEntries = nullptr;
		tempCipher = nullptr;
	}

	return *this;
}

DataVault::~DataVault()
{
	for (size_t i = 0; i < this->allocated; i++) {
		delete this->entries[i];
	}
	delete[] this->entries;

	delete this->defaultCipher;
}

//getters
size_t DataVault::getSize() const
{
	return this->size;
}

//helpers
void DataVault::addEntry(const PasswordEntry& entry)
{
	if (this->size == this->allocated) {
		resize();
	}

	if (find(entry.getWebsite(), entry.getUsername()) != nullptr) {
		throw std::invalid_argument("Entry already exists!");
	}

	this->entries[this->size] = new PasswordEntry(entry);
	this->size += 1;
}

PasswordEntry* DataVault::find(const char* website, const char* username) const
{
	if (!website || !username) {
		return nullptr;
	}

	for (size_t i = 0; i < this->size; i++) {
		if (this->entries[i]->isMatch(website, username)) {
			return this->entries[i];
		}
	}

	return nullptr;
}

void DataVault::updateEntry(const char* website, const char* username, const char* newPassword)
{
	PasswordEntry* entry = find(website, username);
	if (!entry) {
		throw std::invalid_argument("Entry not found!");
	}

	//updatePassword() can throw an exception!
	entry->updatePassword(newPassword);
}

void DataVault::removeEntry(const char* website, const char* name)
{
	size_t index = this->size;

	for (size_t i = 0; i < this->size; i++) {
		if (this->entries[i]->isMatch(website, name)) {
			index = i;
			break;
		}
	}

	if (index == this->size) {
		throw std::invalid_argument("Entry not found!");
	}

	delete this->entries[index];

	for (size_t i = index; i < this->size - 1; i++) {
		this->entries[i] = this->entries[i + 1];
	}

	this->entries[this->size - 1] = nullptr;
	this->size -= 1;
}

void DataVault::removeWebsite(const char* website)
{
	if (!website) {
		throw std::invalid_argument("Website can not be nullptr!");
	}

	size_t i = 0;

	while (i < this->size) {
		if (!this->entries[i]) {
			continue;
		}
		else if (strcmp(this->entries[i]->getWebsite(), website) == 0) {
			delete this->entries[i];

			for (size_t j = i; j < this->size - 1; j++) {
				this->entries[j] = this->entries[j + 1];
			}

			this->entries[this->size - 1] = nullptr;
			this->size -= 1;
		}
		else {
			i++;
		}
	}
}

size_t DataVault::countByWebsite(const char* website) const
{
	size_t count = 0;

	for (size_t i = 0; i < this->size; i++) {
		if (strcmp(this->entries[i]->getWebsite(), website) == 0) {
			count++;
		}
	}

	return count;
}

PasswordEntry** DataVault::findByWebsite(const char* website) const
{
	size_t count = countByWebsite(website);
	size_t index = 0;
	PasswordEntry** result = new PasswordEntry*[count];

	for (size_t i = 0; i < this->size; i++) {
		if (strcmp(this->entries[i]->getWebsite(), website) == 0) {
			result[index] = this->entries[i];
			index++;
		}
	}

	return result;
}

void DataVault::list(std::ostream& out, std::istream& in, size_t pageLen) const
{
	size_t index = 0;

	for (size_t i = 0; i < this->size; i++) {
		out << this->entries[i]->getWebsite()
		<< " | "
		<< this->entries[i]->getUsername()
		<< '\n';

		index++;
		if (index >= pageLen && i != this->size - 1) {
			char answer[16];
			out << "Do you want to see more entries? (yes/no): ";
			in.getline(answer, 16);

			if (strcmp(answer, "yes") != 0) {
				break;
			}

			index = 0;
		}
	}
}

void DataVault::serialize(std::ostream& out)
{
	if (!out) {
		throw std::runtime_error("Invalid output stream!");
	}

	out << "DATA VAULT\n";
	out << this->size << '\n';

	this->defaultCipher->serialize(out);

	for (size_t i = 0; i < this->size; i++) {
		if (!this->entries[i]) {
			continue;
		}
		this->entries[i]->serialize(out);
	}

	if (!out) {
		throw std::runtime_error("Error while writing data vault!!");
	}
}

void DataVault::deserialize(std::istream& in)
{
	if (!in) {
		throw std::runtime_error("Invalid input stream!");
	}

	for (size_t i = 0; i < this->size; i++) {
		delete this->entries[i];
		this->entries[i] = nullptr;
	}

	delete this->defaultCipher;
	this->defaultCipher = nullptr;

	this->size = 0;

	//reading the new data
	char buffer[BUFFER_LEN];
	in.getline(buffer, BUFFER_LEN);

	if (strcmp(buffer, "DATA VAULT") != 0) {
		throw std::runtime_error("Corrupted file!");
	}

	size_t entriesCount = 0;
	in >> entriesCount;
	in.ignore();

	this->defaultCipher = CipherFactory::readCipher(in);

	while (allocated < entriesCount) {
		resize();
	}

	for (size_t i = 0; i < entriesCount; i++) {
		in.getline(buffer, BUFFER_LEN);

		if (strcmp(buffer, "ENTRY") != 0) {
			for (size_t j = 0; j < i; j++) {
				delete this->entries[j];
				this->entries[j] = nullptr;
			}

			delete this->defaultCipher;
			this->defaultCipher = nullptr;

			throw std::runtime_error("Corrupted entry!");
		}

		char website[BUFFER_LEN];
		char username[BUFFER_LEN];
		char encryptedPassword[BUFFER_LEN];

		in.getline(website, BUFFER_LEN);
		in.getline(username, BUFFER_LEN);

		Cipher* cipher = nullptr;
		try {
			cipher = CipherFactory::readCipher(in);
		}
		catch (...) {
			for (size_t j = 0; j < i; j++) {
				delete this->entries[j];
				this->entries[j] = nullptr;
			}

			delete this->defaultCipher;
			this->defaultCipher = nullptr;

			throw;
		}

		in.getline(encryptedPassword, BUFFER_LEN);

		try {
			this->entries[this->size] = PasswordEntry::createFromEncrypted(website, username, encryptedPassword, cipher);
			this->size += 1;
		}
		catch (...) {
			for (size_t j = 0; j < i; j++) {
				delete this->entries[j];
				this->entries[j] = nullptr;
			}

			delete this->defaultCipher;
			this->defaultCipher = nullptr;

			delete cipher;
		}
	}

	if (!in) {
		throw std::runtime_error("Error while reading data vault!");
	}
}

const char* DataVault::serializeToText() const
{
	size_t strSize = 15; //DATA VAULT + '\0' + '\n'
	strSize += strlen(utils::intToString(this->size));
	const char* defCipherText = this->defaultCipher->serializeToText();
	strSize += strlen(defCipherText);

	for (size_t i = 0; i < this->size; i++) {
		if (!this->entries[i]) {
			continue;
		}
		
		try {
			const char* temp = this->entries[i]->serializeToText();
			strSize += strlen(temp);
			delete[] temp;
		}
		catch (...) {
			delete[] defCipherText;
			throw std::bad_alloc();
		}
	}

	char* vaultText = new (std::nothrow) char[strSize];
	if (!vaultText) {
		delete[] defCipherText;
		throw std::bad_alloc();
	}

	vaultText[0] = '\0';
	strcat(vaultText, "DATA VAULT\n");
	strcat(vaultText, utils::intToString(this->size));
	strcat(vaultText, "\n");
	strcat(vaultText, defCipherText);
	
	for (size_t i = 0; i < this->size; i++) {
		if (!this->entries[i]) {
			continue;
		}
		
		try {
			const char* temp = this->entries[i]->serializeToText();
			strcat(vaultText, temp);
			delete[] temp;
		}
		catch (...) {
			delete[] defCipherText;
			delete[] vaultText;
			throw std::bad_alloc();
		}
	}

	delete[] defCipherText;
	return vaultText;
}

//private helpers
void DataVault::resize()
{
	PasswordEntry** tempEntries = new (std::nothrow) PasswordEntry * [this->allocated * 2];
	if (!tempEntries) {
		throw std::runtime_error("Memory allocation error due to internal resize method, please try again!");
	}

	for (size_t i = 0; i < this->allocated; i++) {
		tempEntries[i] = this->entries[i];
	}
	for (size_t i = this->allocated; i < this->allocated * 2; i++) {
		tempEntries[i] = nullptr;
	}

	delete[] this->entries;
	this->entries = tempEntries;
	this->allocated *= 2;
}
