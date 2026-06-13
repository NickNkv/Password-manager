#include "Matrix.hpp"

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
					tempData[i][j] = 0;
				}
			}
		}

		//point of no return TO FINISHI IT
		for (size_t i = 0; i < this->rows; i++) {
			delete[] this->data[i];
			this->data[i] = nullptr;
		}

	}

	return *this;
}
