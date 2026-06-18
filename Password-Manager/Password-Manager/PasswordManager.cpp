#define _CRT_SECURE_NO_WARNINGS
#include "PasswordManager.hpp"
#include "CipherFactory.hpp"
#include "VigenereCipher.hpp"
#include <fstream>

//lifecycle
PasswordManager::PasswordManager()
{
	this->dataVault = nullptr;
	this->fileName = nullptr;
	this->masterPassword = nullptr;
	this->isOpen = false;
	this->isModified = false;
}

PasswordManager::~PasswordManager()
{
	delete this->dataVault;
	delete[] this->fileName;
	delete[] this->masterPassword;
}

//helpers
void PasswordManager::run(std::ostream& out, std::istream& in)
{
	char line[1024];

	while (true) {
		try {
			out << "> ";
			in.getline(line, 1024);

			if (!in) {
				break;
			}

			char* command = strtok(line, " ");

			if (!command) {
				continue;
			}

			//menu
			if (strcmp(command, "create") == 0) {
				char* fileName = strtok(nullptr, "");
				if (!fileName) {
					out << "Misssing file name!\n";
					continue;
				}
				create(fileName, out, in);
			}
			else if (strcmp(command, "open") == 0) {
				char* fileName = strtok(nullptr, "");
				if (!fileName) {
					out << "Misssing file name!\n";
					continue;
				}

				open(fileName, out, in);
			}
			else if (strcmp(command, "close") == 0) {
				close(out, in);
			}
			else if (strcmp(command, "save") == 0) {
				save();
			}
			else if (strcmp(command, "saveas") == 0) {
				char* fileName = strtok(nullptr, "");

				if (!fileName) {
					out << "Missing file name!\n";
					continue;
				}

				saveAs(fileName);
			}
			else if (strcmp(command, "load") == 0) {
				load(nullptr);
			}
			else if (strcmp(command, "update") == 0) {
				update(nullptr);
			}
			else if (strcmp(command, "delete") == 0) {
				remove(nullptr);
			}
			else if (strcmp(command, "list") == 0) {
				list();
			}
			else if (strcmp(command, "exit") == 0) {
				exit();
			}
			else if (strcmp(command, "help") == 0) {
				help(out);
			}
			else {
				out << "Unknown command!\n";
			}
		}
		catch (std::bad_alloc()) {
			out << "Memory allocation error! Please try again!\n";
		}
		catch (const std::exception& e) {
			out << "Error: " << e.what() << '\n';
		}
	}
}

void PasswordManager::create(const char* fileName, std::ostream& out, std::istream& in)
{
	if (!fileName) {
		throw std::invalid_argument("Missing file name!");
	}

	if (this->isOpen) {
		close(out, in);
	}
	Cipher* defaultCipher = CipherFactory::createCipher(out, in);
	this->dataVault = new DataVault(*defaultCipher);

	char password[256];
	out << "File password: ";
	in.ignore();
	in.getline(password, 256);

	this->fileName = new char[strlen(fileName) + 1];
	strcpy(this->fileName, fileName);

	this->masterPassword = new (std::nothrow) char[strlen(password) + 1];
	if (!this->masterPassword) {
		delete[] this->fileName;
		this->fileName = nullptr;
		throw std::bad_alloc();
	}
	strcpy(this->masterPassword, password);

	this->isOpen = true;
	this->isModified = false;

	save();
}

void PasswordManager::open(const char* fileName, std::ostream& out, std::istream& in)
{
	if (!fileName) {
		throw std::invalid_argument("Missing file name!");
	}

	if (this->isOpen) {
		close(out, in);
	}

	char password[256];
	out << "File password: ";
	in.ignore();
	in.getline(password, 256);

	char* tempFileName = new char[strlen(fileName) + 1];
	char* tempFilePassword = new (std::nothrow) char[strlen(password) + 1];
	if (!tempFilePassword) {
		delete[] tempFileName;
		throw std::bad_alloc();
	}

	strcpy(tempFileName, fileName);
	strcpy(tempFilePassword, password);

	std::ifstream inStr(fileName);
	if (!inStr.is_open()) {
		throw std::runtime_error("Cannot open file!");
	}

	std::ofstream outStr("_temp_.txt");
	if (!outStr.is_open()) {
		inStr.close();
		throw std::runtime_error("Can not open temp file!");
	}

	VigenereCipher cipher(password);
	char buffer[1024];

	while (inStr.getline(buffer, 1024)) {
		char* decrypted = cipher.decrypt(buffer);
		outStr << decrypted << '\n';
		delete[] decrypted;
	}

	inStr.close();
	outStr.close();

	std::ifstream tempIn("_temp_.txt");
	if (!tempIn.is_open()) {
		std::remove("_temp_.txt");
		throw std::runtime_error("Can not open temp file!");
	}

	this->dataVault->deserialize(tempIn);
	tempIn.close();
	std::remove("_temp_.txt");
	
	delete[] this->fileName;
	this->fileName = tempFileName;
	delete[] this->masterPassword;
	this->masterPassword = tempFilePassword;
	this->isOpen = true;
	this->isModified = false;
}

void PasswordManager::close(std::ostream& out, std::istream& in)
{
	if (this->isOpen) {
		out << "There are no open files\n";
	}

	if (isModified) {
		char line[5];
		out << "Do you want to save the changes on your file (yes/no)?\n";
		out << "Answer: ";
		in.ignore();
		in.getline(line, 5);

		char* command = strtok(line, " ");

		if (strcmp(command, "yes") == 0) {
			save();
			out << "Changes saved!";
		}
		else if (strcmp(command, "no") == 0) {
			//
		}
		else {
			throw std::invalid_argument("Invalid answer!");
		}
	}

	delete this->dataVault;
	this->dataVault = nullptr;
	delete[] this->fileName;
	this->fileName = nullptr;
	delete[] this->masterPassword;
	this->masterPassword = nullptr;
	this->isOpen = false;
	this->isModified = false;

	out << "File is closed\n";
}

void PasswordManager::save()
{
	if (!this->isOpen) {
		throw std::runtime_error("No open file!");
	}

	if (!this->fileName) {
		throw std::runtime_error("No file selected!");
	}

	{
		std::ofstream outStr(this->fileName);
		if (!outStr.is_open()) {
			throw std::runtime_error("Cannot open file!");
		}
		this->dataVault->serialize(outStr);
		outStr.close();
	}

	std::ifstream inStr(this->fileName);
	if (!inStr.is_open()) {
		std::remove(this->fileName);
		throw std::runtime_error("Internal error! Try again!");
	}

	std::ofstream outStr("_temp_.txt");
	if (!outStr.is_open()) {
		std::remove(this->fileName);
		inStr.close();
		throw std::runtime_error("Internal error! Try again!");
	}

	VigenereCipher cipher(this->masterPassword);
	char buffer[1024];

	while (inStr.getline(buffer, 1024)) {
		char* encrypted = cipher.encrypt(buffer);
		outStr << encrypted << '\n'; 
		delete[] encrypted;
	}

	inStr.close();
	outStr.close();

	std::remove(this->fileName);
	std::rename("_temp_.txt", this->fileName);
	this->isModified = false;
}

void PasswordManager::saveAs(const char* fileName)
{
}

void PasswordManager::load(const char* website, const char* user)
{
}

void PasswordManager::update(const char* website)
{
}

void PasswordManager::remove(const char* website)
{
}

void PasswordManager::list()
{
}

void PasswordManager::exit()
{
}

void PasswordManager::help(std::ostream& out) const
{
	out << "List of all commands:\n";
	out << "create <filename> <cipher> <password>\n";
}
