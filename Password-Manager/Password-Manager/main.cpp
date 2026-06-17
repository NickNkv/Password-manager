#include <iostream>
#include <fstream>
#include "PasswordEntry.hpp"
#include "Matrix.hpp"
#include "CaesarCipher.hpp"
#include "Utils.hpp"
#include "TextCodeCipher.hpp"
#include "VigenereCipher.hpp"
#include "HillCipher.hpp"
#include "PasswordManager.hpp"

int main()
{
    //VigenereCipher v("banana");
    //char* t = v.decrypt("YJ^F");
    //std::cout << t;
    //std::cout << utils::intToString(707);
    //TextCodeCipher t("Source.txt");
    //std::cout << t.decrypt("38|2|18|0|59|1|6|34");
    //CaesarCipher a(100);
   // std::cout << a.decrypt("6789:;<=>5%yjxy%yj}y%");

    //Matrix m(3, 3);
    //std::ifstream file("Matrix.txt");
    //m = Matrix::deserialize(file);

    //HillCipher h(m);
    //char* encr = h.decrypt("UXQDFXNQYXSA");
    //std::cout << encr;
    //delete[] encr;

    PasswordManager p;
    p.run(std::cout, std::cin);

    //// Check if the file was opened successfully
    //if (!file.is_open()) {
    //    std::cerr << "Error: Could not open matrix.txt" << std::endl;
    //    return 1;
    //}


    //file.close();
    return 0;
}

