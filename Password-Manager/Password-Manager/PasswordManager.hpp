#pragma once
#include "DataVault.hpp"
#include <iostream>

class PasswordManager {
public:
	PasswordManager();
	PasswordManager(const PasswordManager& other) = delete;
	PasswordManager& operator= (const PasswordManager& other) = delete;
	~PasswordManager();

	//helpers
	//in our case out and in will be std::cout and std::cin
	void run(std::ostream& out, std::istream& in);

private:
	DataVault* dataVault;
	char* fileName;
	char* masterPassword;
	bool isOpen;
	bool isModified;

	//private helpers
	void create(const char* fileName, std::ostream& out, std::istream& in);
	void open(const char* fileName, std::ostream& out, std::istream& in);
	void close(std::ostream& out, std::istream& in);
	void save(const char* website, const char* user, std::ostream& out, std::istream& in);
	void saveChanges();
	void saveAs(const char* fileName);
	void load(const char* website, const char* user = nullptr);
	void update(const char* website);
	void remove(const char* website); // > delete <website> [<user>]
	void list();
	void exit();
	void help(std::ostream& out) const;
};