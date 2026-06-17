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
	bool isOpen;
	bool isModified;

	//private helpers
	void create(const char* fileName);
	void open(const char* fileName);
	void close();
	void save();
	void saveAs(const char* fileName);
	void help() const; 
	void add();
	void remove(const char* website);
	void change(const char* website);
	void print() const;
};