#define _CRT_SECURE_NO_WARNINGS
#include "PasswordManager.hpp"
#include "CaesarCipher.hpp"

//lifecycle
PasswordManager::PasswordManager()
{
	CaesarCipher defaultCipher(5);
	this->dataVault = new DataVault(defaultCipher);
	this->fileName = nullptr;
	this->isOpen = false;
	this->isModified = false;
}

PasswordManager::~PasswordManager()
{
	delete this->dataVault;
	delete[] this->fileName;
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
			if (strcmp(command, "help") == 0) {
				help();
			}
			else if (strcmp(command, "exit") == 0) {

			}
			else if (strcmp(command, "save") == 0) {
				save();
			}
			else if (strcmp(command, "close") == 0) {
				close();
			}
			else if (strcmp(command, "print") == 0) {
				print();
			}
			else if (strcmp(command, "open") == 0) {
				char* fileName = strtok(nullptr, "");
				if (!fileName) {
					out << "Misssing file name!\n";
					continue;
				}

				open(fileName);
			}
			else if (strcmp(command, "create") == 0) {
				char* fileName = strtok(nullptr, "");

				if (!fileName) {
					out << "Missing file name!\n";
					continue;
				}

				create(fileName);
			}
			else if (strcmp(command, "saveas") == 0) {
				char* fileName = strtok(nullptr, "");

				if (!fileName) {
					out << "Missing file name!\n";
					continue;
				}

				saveAs(fileName);
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

void PasswordManager::create(const char* fileName)
{
}

void PasswordManager::open(const char* fileName)
{
}

void PasswordManager::close()
{
}

void PasswordManager::save()
{
}

void PasswordManager::saveAs(const char* fileName)
{
}

void PasswordManager::help() const
{
}

void PasswordManager::add()
{
}

void PasswordManager::remove(const char* website)
{
}

void PasswordManager::change(const char* website)
{
}

void PasswordManager::print() const
{
}
