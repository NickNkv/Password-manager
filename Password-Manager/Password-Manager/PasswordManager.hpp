#pragma once
#include "DataVault.hpp"

class PasswordManager {
public:
	PasswordManager();
	PasswordManager(const PasswordManager& other) = delete;
	PasswordManager& operator= (const PasswordManager& other) = delete;
	~PasswordManager();

	//TODO methods

private:
	DataVault* dataVault;
	char* fileName;
	bool opened;
	bool modified;
};