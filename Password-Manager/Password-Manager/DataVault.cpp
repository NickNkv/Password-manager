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
			this->entries[i] = new (std::nothrow) PasswordEntry(*other.entries[i]);
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
