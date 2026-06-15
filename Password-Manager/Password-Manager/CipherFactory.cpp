#include "CipherFactory.hpp"

#include "CaesarCipher.hpp"

#define TYPE_LEN 256

Cipher* CipherFactory::createCipher(std::ostream& out, std::istream& in)
{
    int choice;

    out << "Choose cipher:\n";
    out << "1. Caesar\n";
    out << "2. TextCode\n";
    out << "3. Hill\n";
    out << "4. Vigenere\n";
    out << "\nEnter the cipher code: ";
    in >> choice;

    switch (choice) {
    case 1:
        int shift;
        out << "Shift: ";
        in >> shift;

        return new CaesarCipher(shift);

    default:
        throw std::invalid_argument("Invalid cipher code!");
    }
}

Cipher* CipherFactory::readCipher(std::istream& in)
{
    if (!in) {
        throw std::invalid_argument("Invalid input stream!");
    }

    char type[TYPE_LEN];
    in.getline(type, TYPE_LEN);

    if (strcmp(type, "CAESAR") == 0) {
        int shift;
        in >> shift;
        in.ignore();

        return new CaesarCipher(shift);
    }
    else if (strcmp(type, "TEXTCODE") == 0) {
        //TODO
    }
    else if (strcmp(type, "HILL") == 0) {
        //TODO
    }
    else if (strcmp(type, "VIGENERE") == 0) {
        //TODO
    }
    else {
        throw std::runtime_error("Unknown cipher!");
    }
}
