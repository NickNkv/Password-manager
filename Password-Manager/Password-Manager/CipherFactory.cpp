#include "CipherFactory.hpp"

#include "CaesarCipher.hpp"
#include "TextCodeCipher.hpp"
#include "HillCipher.hpp"
#include "VigenereCipher.hpp"

#define TYPE_LEN 256
#define FILEPATH_LEN 256 //for TextCode

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
    case 1: {
        int shift;
        out << "Shift: ";
        in >> shift;
        return new CaesarCipher(shift);
    }

    case 2: {
        char filePath[FILEPATH_LEN];
        out << "File path: ";
        in.ignore();
        in.getline(filePath, FILEPATH_LEN);
        return new TextCodeCipher(filePath);
    }

    case 3: {
        //I could use matrix.serialize() 
        //but I want to add UX with messages
        size_t n;
        out << "Matrix size (n x n): ";
        in >> n;

        Matrix key(n, n);
        out << "Enter matrix values:\n";
        for (size_t i = 0; i < n; i++) {
            for (size_t j = 0; j < n; j++) {
                in >> key[i][j];
            }
        }

        return new HillCipher(key);
    }

    case 4: {
        char keyword[FILEPATH_LEN];
        out << "Keyword: ";
        in.ignore();
        in.getline(keyword, FILEPATH_LEN);
        return new VigenereCipher(keyword);
    }

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
        char filePath[FILEPATH_LEN];
        in.getline(filePath, FILEPATH_LEN);
        
        return new TextCodeCipher(filePath);
    }
    else if (strcmp(type, "HILL") == 0) {
        Matrix key = Matrix::deserialize(in);
        
        return new HillCipher(key);
    }
    else if (strcmp(type, "VIGENERE") == 0) {
        char keyword[FILEPATH_LEN];
        in.getline(keyword, FILEPATH_LEN);

        return new VigenereCipher(keyword);
    }
    else {
        throw std::runtime_error("Unknown cipher!");
    }
}
