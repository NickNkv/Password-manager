#include "Matrix.hpp"
#include "Utils.hpp"

//lifecycle
Matrix::Matrix(size_t rows, size_t cols)
{
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
	return 0;
}

bool Matrix::isInvertableMod26() const
{
	int det = determinant();

	det %= 26;
	if (det < 0) {
		det += 26;
	}

	return utils::gcd(det, 26) == 1;
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
