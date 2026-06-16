#include <iostream>
#include <fstream>
#include "PasswordEntry.hpp"
#include "Matrix.hpp"
#include "CaesarCipher.hpp"
#include "Utils.hpp"
#include "TextCodeCipher.hpp"

int main()
{
    //std::cout << utils::intToString(707);
    TextCodeCipher t("Source.txt");
    std::cout << t.decrypt("38|2|18|0|59|1|6|34");
    //CaesarCipher a(100);
   // std::cout << a.decrypt("6789:;<=>5%yjxy%yj}y%");

    //Matrix m(5, 5);
    //std::ifstream file("Matrix.txt");

    //// Check if the file was opened successfully
    //if (!file.is_open()) {
    //    std::cerr << "Error: Could not open matrix.txt" << std::endl;
    //    return 1;
    //}

    //m = Matrix::deserialize(file);

    //Matrix inv = m.inverseMod26();
    //inv.serialize(std::cout);

    //file.close();
}

