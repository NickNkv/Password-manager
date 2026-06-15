#include "Matrix.hpp"
#include "Utils.hpp"

//lifecycle
Matrix::Matrix(size_t rows, size_t cols)
{
	if (rows == 0 || cols == 0) {
		throw std::invalid_argument("Matrix dimensions must be positive!");
	}

	this->rows = rows;
	this->cols = cols;

	this->data = new int* [this->rows];

	for (size_t i = 0; i < this->rows; i++) {
		this->data[i] = new (std::nothrow) int[this->cols];

		if (!this->data[i]) {
			for (size_t j = 0; j < i; j++) {
				delete[] this->data[j];
				this->data[j] = nullptr;
			}
			delete[] this->data;
			this->data = nullptr;
			throw std::bad_alloc();
		}
		else {
			for (size_t j = 0; j < this->cols; j++) {
				this->data[i][j] = 0;
			}
		}
	}
}

Matrix::Matrix(const Matrix& other)
{
	this->rows = other.rows;
	this->cols = other.cols;

	this->data = new int* [this->rows];

	for (size_t i = 0; i < this->rows; i++) {
		this->data[i] = new (std::nothrow) int[this->cols];

		if (!this->data[i]) {
			for (size_t j = 0; j < i; j++) {
				delete[] this->data[j];
				this->data[j] = nullptr;
			}
			delete[] this->data;
			this->data = nullptr;
			throw std::bad_alloc();
		}
		else {
			for (size_t j = 0; j < this->cols; j++) {
				this->data[i][j] = other.data[i][j];
			}
		}
	}
}

Matrix& Matrix::operator=(const Matrix& other)
{
	if (this != &other) {
		int** tempData = new int* [other.rows];

		for (size_t i = 0; i < other.rows; i++) {
			tempData[i] = new (std::nothrow) int[other.cols];

			if (!tempData[i]) {
				for (size_t j = 0; j < i; j++) {
					delete[] tempData[j];
					tempData[j] = nullptr;
				}
				delete[] tempData;
				tempData = nullptr;
				throw std::bad_alloc();
			}
			else {
				for (size_t j = 0; j < other.cols; j++) {
					tempData[i][j] = other.data[i][j];
				}
			}
		}

		//point of no return
		for (size_t i = 0; i < this->rows; i++) {
			delete[] this->data[i];
			this->data[i] = nullptr;
		}
		delete[] this->data;

		this->data = tempData;
		this->rows = other.rows;
		this->cols = other.cols;
	}

	return *this;
}

Matrix::~Matrix()
{
	for (size_t i = 0; i < this->rows; i++) {
		delete[] this->data[i];
		this->data[i] = nullptr;
	}

	delete[] this->data;
	this->data = nullptr;
}

//getters
int Matrix::getAt(size_t row, size_t col) const
{
	if (row < this->rows && col < this->cols) {
		return this->data[row][col];
	}
	else {
		throw std::invalid_argument("Rows or cols out of boundary!");
	}
}

size_t Matrix::getRows() const
{
	return this->rows;
}

size_t Matrix::getCols() const
{
	return this->cols;
}

int Matrix::determinant() const
{
	if (this->rows != this->cols) {
		throw std::invalid_argument("Matrix must be square!");
	}

	if (this->rows == 1) {
		return this->data[0][0];
	}

	if (this->rows == 2) {
		return this->data[0][0] * this->data[1][1] - this->data[0][1] * this->data[1][0];
	}

	int det = 0;
	for (size_t col = 0; col < this->cols; col++) {
		Matrix minor = getMinor(0, col);

		int sign = (col % 2 == 0) ? 1 : -1;
		det += sign * this->data[0][col] * minor.determinant();
	}

	return det;
}

bool Matrix::isInvertableMod26() const
{
	if (this->rows != this->cols) {
		return false;
	}

	int det = determinant();

	det %= 26;
	if (det < 0) {
		det += 26;
	}

	return utils::gcd(det, 26) == 1;
}

Matrix Matrix::transpose() const
{
	Matrix result(this->rows, this->cols);

	for (size_t i = 0; i < this->rows; i++) {
		for (size_t j = 0; j < this->cols; j++) {
			result[j][i] = this->data[i][j];
		}
	}

	return result;
}

Matrix Matrix::inverseMod26() const
{
	if (this->rows != this->cols) {
		throw std::invalid_argument("Matrix must be square!");
	}

	if (!isInvertableMod26()) {
		throw std::invalid_argument("Matrix is not invertable in modulo 26!");
	}

	int det = determinant();
	det = utils::mod(det, 26);
	int determinantInverse = utils::modInverse(det, 26);

	Matrix cofactors(this->rows, this->cols);

	for (size_t i = 0; i < this->rows; i++) {
		for (size_t j = 0; j < this->cols; j++) {
			Matrix minor = getMinor(i, j);

			int sign = ((i + j) % 2 == 0) ? 1 : -1;
			int value = sign * minor.determinant();

			cofactors[i][j] = value;
		}
	}

	Matrix adjugate = cofactors.transpose();
	Matrix inverse(this->rows, this->cols);

	for (size_t i = 0; i < this->rows; i++) {
		for (size_t j = 0; j < this->cols; j++) {
			inverse[i][j] = utils::mod(adjugate[i][j] * determinantInverse, 26);
		}
	}

	return inverse;
}

void Matrix::serialize(std::ostream& out) const
{
	if (!out) {
		throw std::invalid_argument("Invalid output stream!");
	}

	out << this->rows << ' ' << this->cols << '\n';

	for (size_t i = 0; i < this->rows; i++) {
		for (size_t j = 0; j < this->cols; j++) {
			out << this->data[i][j] << ' ';
		}
		out << '\n';
	}

	if (!out) {
		throw std::runtime_error("Error while writing matrix!");
	}
}

Matrix Matrix::deserialize(std::istream& in)
{
	if (!in) {
		throw std::invalid_argument("Invalid input stream!");
	}

	size_t rows = 0;
	size_t cols = 0;

	in >> rows >> cols;
	Matrix result(rows, cols);

	for (size_t i = 0; i < rows; i++) {
		for (size_t j = 0; j < cols; j++) {
			in >> result[i][j];
		}
	}

	if (!in) {
		throw std::runtime_error("Error while reading the matrix! Corrupted matrix!");
	}

	return result;
}

//operators
int* Matrix::operator[](size_t row)
{
	return this->data[row];
}

const int* Matrix::operator[](size_t row) const
{
	return this->data[row];
}

Matrix Matrix::operator*(const Matrix& other) const
{
	if (this->cols != other.rows) {
		throw std::invalid_argument("Invalid matrix dimensions!");
	}

	Matrix result(this->rows, other.cols);

	for (size_t i = 0; i < this->rows; i++) {
		for (size_t j = 0; j < other.cols; j++) {
			for (size_t k = 0; k < this->cols; k++) {
				result[i][j] += this->data[i][k] * other[k][j];
			}
		}
	}

	return result;
}

//private helpers
Matrix Matrix::getMinor(size_t exclRow, size_t exclCol) const
{
	if (this->rows != this->cols) {
		throw std::invalid_argument("Matrix must be square!");
	}
	if (exclCol >= this->rows) {
		throw std::invalid_argument("Excluded row is outside of the matrix boundaries!");
	}
	if (exclCol >= this->cols) {
		throw std::invalid_argument("Excluded col is outside of the matrix boundaries!");
	}

	Matrix minor(this->rows - 1, this->cols - 1);

	size_t minorRow = 0;
	for (size_t i = 0; i < this->rows; i++) {
		if (i == exclRow) {
			continue;
		}

		size_t minorCol = 0;
		for (size_t j = 0; j < this->cols; j++) {
			if (j == exclCol) {
				continue;
			}

			minor[minorRow][minorCol] = this->data[i][j];
			minorCol++;
		}

		minorRow++;
	}

	return minor;
}
