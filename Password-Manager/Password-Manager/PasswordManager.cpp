#define _CRT_SECURE_NO_WARNINGS
#include "PasswordManager.hpp"
#include "CipherFactory.hpp"
#include "VigenereCipher.hpp"
#include "PasswordEntry.hpp"
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
					out << "Misssing file name!\n\n";
					continue;
				}
				create(fileName, out, in);
			}
			else if (strcmp(command, "open") == 0) {
				char* fileName = strtok(nullptr, "");
				if (!fileName) {
					out << "Misssing file name!\n\n";
					continue;
				}

				open(fileName, out, in);
			}
			else if (strcmp(command, "close") == 0) {
				close(out, in);
			}
			else if (strcmp(command, "save") == 0) {
				char* website = strtok(nullptr, " ");
				if (!website) {
					out << "Missing website name!\n\n";
					continue;
				}

				char* user = strtok(nullptr, "");
				if (!user) {
					out << "Missing username!\n\n";
					continue;
				}

				save(website, user, out, in);
			}
			else if (strcmp(command, "load") == 0) {
				char* website = strtok(nullptr, " ");
				if (!website) {
					out << "Missing website name!\n\n";
					continue;
				}

				char* user = strtok(nullptr, " ");
				if (!user) {
					user = nullptr;
				}
				
				load(out, in, website, user);
			}
			else if (strcmp(command, "update") == 0) {
				char* website = strtok(nullptr, " ");
				if (!website) {
					out << "Missing website name!\n\n";
					continue;
				}

				char* user = strtok(nullptr, " ");
				if (!user) {
					out << "Missing user!\n\n";
					continue;
				}

				char* newPassword = strtok(nullptr, "");
				if (!newPassword) {
					out << "Missing new password!\n\n";
					continue;
				}

				update(website, user, newPassword);
			}
			else if (strcmp(command, "delete") == 0) {
				char* website = strtok(nullptr, " ");
				if (!website) {
					out << "Missing website name!\n\n";
					continue;
				}

				char* user = strtok(nullptr, " ");
				if (!user) {
					user = nullptr;
				}

				remove(out, in, website, user);
			}
			else if (strcmp(command, "list") == 0) {
				list(out, in);
			}
			else if (strcmp(command, "exit") == 0) {
				close(out, in);
				break;
			}
			else if (strcmp(command, "help") == 0) {
				help(out);
			}
			else {
				out << "Unknown command! Type [help] to see all commands!\n\n";
			}
		}
		catch (std::bad_alloc()) {
			out << "Memory allocation error! Please try again!\n\n";
		}
		catch (const std::exception& e) {
			out << "Error: " << e.what() << "\n\n";
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
	out << "Default cipher: \n";
	Cipher* defaultCipher = CipherFactory::createCipher(out, in);
	this->dataVault = new DataVault(*defaultCipher);

	char password[256];
	out << "File password: ";
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

	saveChanges();
	close(out, in);
}

void PasswordManager::open(const char* fileName, std::ostream& out, std::istream& in)
{
	if (!fileName) {
		throw std::invalid_argument("Missing file name!");
	}

	if (this->isOpen) {
		close(out, in);
	}

	this->dataVault = new DataVault(VigenereCipher("temp"));//dummy

	char password[256];
	out << "File password: ";
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

	try {
		this->dataVault->deserialize(tempIn);
	}
	catch (...) {
		tempIn.close();
		std::remove("_temp_.txt");
		throw;
	}

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
	if (!this->isOpen) {
		out << "There are no open files\n";
	}

	if (isModified) {
		char line[5];
		out << "Do you want to save the changes on your file (yes/no)?\n";
		out << "Answer: ";
		in.getline(line, 5);

		char* command = strtok(line, " ");

		if (strcmp(command, "yes") == 0) {
			saveChanges();
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
}

void PasswordManager::save(const char* website, const char* user, std::ostream& out, std::istream& in)
{
	if (!this->isOpen) {
		throw std::runtime_error("No open file!");
	}

	Cipher* cipher = CipherFactory::createCipher(out, in);

	char password[256];
	out << "Website password: ";
	in.getline(password, 256);
	PasswordEntry entry(website, user, password, *cipher);
	this->dataVault->addEntry(entry);
	delete cipher;
	this->isModified = true;

	saveChanges();
}

void PasswordManager::saveChanges()
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

void PasswordManager::load(std::ostream& out, std::istream& in, const char* website, const char* user)
{
	if (!this->isOpen) {
		throw std::runtime_error("No open file!\n");
	}

	//if we search by user
	if (user) {
		PasswordEntry* entry = this->dataVault->find(website, user);
		if (!entry) {
			throw std::runtime_error("Website and user not found!\n");
		}

		out << "Password: " << entry->getPassword() << '\n';
	}
	else { //if we search by website only  
		size_t count = this->dataVault->countByWebsite(website);
		if (count == 0) {
			throw std::runtime_error("No password entries for this website!\n");
		}

		PasswordEntry** entries = this->dataVault->findByWebsite(website);

		for (size_t i = 0; i < count; i++) {
			out << "User: " << entries[i]->getUsername() << '\n';
			out << "Password: " << entries[i]->getPassword() << '\n';
			out << '\n';
		}

		delete[] entries;
	}
}

void PasswordManager::update(const char* website, const char* user, const char* newPassword)
{
	if (!this->isOpen) {
		throw std::runtime_error("No open file!\n");
	}

	PasswordEntry* entry = this->dataVault->find(website, user);
	if (!entry) {
		throw std::runtime_error("There is no password for these website and user!\n");
	}

	if (strcmp(entry->getPassword(), newPassword) == 0) {
		throw std::runtime_error("New password is the same as the old one!\n");
	}

	entry->updatePassword(newPassword);
	this->isModified = true;
}

void PasswordManager::remove(std::ostream& out, std::istream& in, const char* website, const char* user)
{
	if (!this->isOpen) {
		throw std::runtime_error("No open file!\n");
	}

	//if we delete by user
	if (user) {
		this->dataVault->removeEntry(website, user);
		this->isModified = true;
		out << "Deletes: " << user << "'s data for " << website << "\n";
	}
	else { //if we delete by website
		size_t count = this->dataVault->countByWebsite(website);
		if (count == 0) {
			throw std::runtime_error("There are no passwords for this website!\n");
		}

		char answer[16];
		out << "Delete all " << count << " entries for " << website << "?\n(yes/no): ";
		in.getline(answer, 16);

		if (strcmp(answer, "yes") != 0) {
			return;
		}

		this->dataVault->removeWebsite(website);
		out << count << " entries deleted\n";
		this->isModified = true;
	}
}

void PasswordManager::list(std::ostream& out, std::istream& in)
{
	size_t pageLen = 5;
	this->dataVault->list(out, in, pageLen);
}

void PasswordManager::help(std::ostream& out) const
{
	out << "List of all commands:\n";
	out << "> create <filename> - creates an encrypted file with default cipher\n";
	out << "> open <filename> - opens a file and asks for its password\n";
	out << "> close - closes the curently opened file (asks whether to save changes)\n";
	out << "> save <website> <user> - saves user and website and encrypted password\n";
	out << "> load <website> [<user>] - shows the password for the website and user\n";
	out << "> update <website> <user> <new-password> - changes the password\n";
	out << "> delete <website> [<user>] - deletes user login in <website> or deletes all logins for <website>\n";
	out << "> list - list the website and user data (without the passwords)\n";
	out << "> exit - exits the program (asks whether to save changes)\n";
	out << "\n";
}
