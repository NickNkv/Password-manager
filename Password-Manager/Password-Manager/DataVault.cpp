#define _CRT_SECURE_NO_WARNINGS
#include "DataVault.hpp"
#include <iostream>
#define ALLOC_STEP 8

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
