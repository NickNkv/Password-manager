#pragma once
#include <iostream>

class Matrix {
public:
	Matrix(size_t rows, size_t cols);
	Matrix(const Matrix& other);
	Matrix& operator= (const Matrix& other);
	~Matrix();

	//getters
	int getAt(size_t row, size_t col) const;
	size_t getRows() const;
	size_t getCols() const;

	//setters
	void setAt(size_t row, size_t col, int value);

	//helpers
	int determinant() const;
	bool isInvertableMod26() const;
	Matrix inverseMod26() const;
	void serialize(std::ostream& out) const;
	static Matrix deserialize(std::istream& in);

	//operators
	int* operator[](size_t row);
	const int* operator[](size_t row) const;
	Matrix operator*(const Matrix& other) const;

private:
	int** data;
	size_t rows;
	size_t cols;
};