#pragma once
#include "PasswordEntry.hpp"
#include "Cipher.hpp"

class DataVault {
public:
	DataVault() = delete;
	DataVault(const Cipher& defaultCipher);
	DataVault(const DataVault& other);
	DataVault& operator= (const DataVault& other);
	~DataVault();

	//getters and setters
	//may not be needed

	//helpers
	void addEntry(const PasswordEntry& entry);
	PasswordEntry* find(const char* website, const char* username) const;
	void updateEntry(const char* website, const char* username, const char* newPassword);
	void removeEntry(const char* website, const char* name);
	void removeWebsite(const char* website);
	void list() const;
	void serialize(std::ostream& out) const;
	void deserialize(std::istream& in);


private:
	size_t size;
	size_t allocated;
	Cipher* defaultCipher;
	PasswordEntry** entries;

	//private helpers
	void resize();
};