#include "Utils.hpp"
#include <iostream>

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
