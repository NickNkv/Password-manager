#include "Utils.hpp"
#include <iostream>
#include <fstream>

int utils::gcd(int a, int b)
{
    while (b != 0) {
        int temp = a % b;
        a = b;
        b = temp;
    }

    return a;
}

//mathematical module
int utils::mod(int value, int module)
{
    int result = value % module;
    if (result < 0) {
        result += module;
    }

    return result;
}

//finds the module inverse element
//a * x = 1 (mod module)
//3 * 9 = 1 mod 26
int utils::modInverse(int a, int module)
{
    a = mod(a, module);

    for (int x = 1; x < module; x++) {
        if ((a * x) % module == 1) {
            return x;
        }
    }

    throw std::invalid_argument("Modular inverse does not exist!");
}

char* utils::readFile(const char* filePath)
{
    if (!filePath || strlen(filePath) == 0) {
        throw std::invalid_argument("File path can not be nullptr or empty!");
    }

    std::ifstream in(filePath);

    if (!in.is_open()) {
        throw std::runtime_error("Cannot open file!");
    }

    in.seekg(0, std::ios::end);
    size_t size = in.tellg();
    in.seekg(0, std::ios::beg);

    char* content = new char[size + 1];
    in.read(content, size);
    content[size] = '\0';

    if (!in && !in.eof()) {
        delete[] content;
        throw std::runtime_error("Error while reading file!");
    }

    return content;
}
