#include "MatCpp.h"
namespace CppToolBox {
	void swap(double** input, double**output, int dimension, int first_row, int second_row)
	{
		double* temp_row1 = new double[dimension];
		double* temp_row2 = new double[dimension];
		int i;
		for (i = 0; i<dimension; i++)
		{
			temp_row1[i] = input[first_row][i];
			temp_row2[i] = output[first_row][i];
		}
		for (i = 0; i<dimension; i++)
		{
			input[first_row][i] = input[second_row][i];
			output[first_row][i] = output[second_row][i];
			input[second_row][i] = temp_row1[i];
			output[second_row][i] = temp_row2[i];
		}
		delete[] temp_row1;
		delete[] temp_row2;
	}
	void reorderOutput(double** output, int dimension)
	{
		double**temp = new double*[dimension];
		for (int i = 0; i<dimension; ++i)
			temp[i] = new double[dimension];

		for (int i = 1; i<dimension; ++i)
			memcpy(temp[i - 1], output[i], sizeof(double)*dimension);
		memcpy(temp[dimension - 1], output[0], sizeof(double)*dimension);

		for (int i = 0; i<dimension; ++i)
			memcpy(output[i], temp[i], sizeof(double)*dimension);

		for (int i = 0; i<dimension; ++i)
			delete[] temp[i];
		delete[] temp;
	}

	double * Vector::get_array() {
		return data;
	}
	Vector::Vector() :length(0), data(NULL), is_shared(false) {}
	Vector::Vector(const Vector & vec) :length(vec.length)
	{
		data = new double[length];
		is_shared = false;
		memcpy(data, vec.data, sizeof(double) * length);
	}
	Vector::Vector(int len) :length(len), is_shared(false) {
		data = new double[length];
		memset(data, 0, sizeof(double) * length);
	}
	Vector::Vector(double * value, int len) :length(len), is_shared(false)
	{
		data = new double[length];
		memcpy(data, value, sizeof(double) * length);
	}
	Vector Vector::operator+(Matrix & obj)const
	{
		Vector total = Vector(*this);
		for (int i = 0;i < length;i++)
		{
			total.data[i] += obj[i][0];
		}
		return total;
	}
	Vector::~Vector()
	{
		if (data && !is_shared)delete[] data;
	}
	double & Vector::operator[](int index)const
	{
		if (index < 0 || index >= length)throw "index out of bounds in vector";
		return data[index];
	}


	double ** Matrix::get_array() {
		return data;
	}
	//constructors
	Matrix::Matrix() :rows(0), cols(0), data(NULL), is_shared(false), current_row(NULL) { }
	Matrix::Matrix(int r, int c) :rows(r), cols(c), is_shared(false), current_row(NULL) {
		data = new double*[rows];
		for (int i = 0; i < rows; i++)
		{
			data[i] = new double[cols];
			memset(data[i], 0, sizeof(double) * cols);
		}
	};
	Matrix::Matrix(const Matrix & obj) :rows(obj.rows), cols(obj.cols), is_shared(false), current_row(NULL)
	{
		data = new double*[rows];
		for (int i = 0; i < rows; i++)
		{
			data[i] = new double[cols];
			memcpy(data[i], obj.data[i], sizeof(double) * cols);
		}
	}
	Matrix::Matrix(const double ** val, int r, int c) :rows(r), cols(c), is_shared(false), current_row(NULL)
	{
		data = new double*[rows];
		for (int i = 0; i < rows; i++)
		{
			data[i] = new double[cols];
			memcpy(data[i], val[i], sizeof(double) * cols);
		}
	}
	Matrix::Matrix(const double * val, int r, int c) :rows(r), cols(c), is_shared(false), current_row(NULL)
	{
		data = new double*[rows];
		for (int i = 0; i < rows; i++)
		{
			data[i] = new double[cols];
			memcpy(data[i], val + i * cols, sizeof(double) * cols);
		}
	}
	//deconstructors
	Matrix::~Matrix()
	{
		for (int i = 0; i < rows; i++)
			delete[] data[i];
		delete[] data;
	}
	//operators
	Matrix & Matrix::operator=(const Matrix & obj)
	{
		cols = obj.cols;
		rows = obj.rows;
		is_shared = false;
		data = new double*[rows];
		for (int i = 0; i < rows; i++)
		{
			data[i] = new double[cols];
			memcpy(data[i], obj.data[i], sizeof(double) * cols);
		}
		return *this;
	}
	Matrix Matrix::operator-(const Matrix & obj)const
	{
		Matrix total = obj;
		if (obj.cols == cols && obj.rows == rows)
			for (int i = 0; i < rows; i++)
				for (int j = 0; j < cols; j++)
					total.data[i][j] = data[i][j] - obj.data[i][j];
		else throw "operator- failed due to dimension mismatch";
		return total;
	}
	Matrix Matrix::operator-(const double obj)const
	{
		Matrix total = *this;
		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
				total.data[i][j] = data[i][j] - obj;
		return total;
	}
	Matrix Matrix::operator+(const Matrix & obj)const
	{
		Matrix total = obj;
		if (obj.cols == cols && obj.rows == rows)
			for (int i = 0; i < rows; i++)
				for (int j = 0; j < cols; j++)
					total.data[i][j] = data[i][j] + obj.data[i][j];
		else throw "operator+ failed due to dimension mismatch";
		return total;
	}
	Matrix Matrix::operator+(const double obj)const
	{
		Matrix total = *this;
		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
				total.data[i][j] = data[i][j] + obj;
		return total;
	}
	Matrix Matrix::operator*(const Matrix & obj)const
	{
		Matrix total = Matrix(rows,obj.cols);
		double num = 0.0;
		if (cols == obj.rows)
			for (int i = 0; i < rows; i++)
				for (int j = 0; j < obj.cols; j++)
					for (int k = 0; k < cols; k++)
					{
						num = data[i][k] * obj.data[k][j];
						total.data[i][j] += num;
					}
		else throw "operator* failed due to dimension mismatch";
		return total;
	}
	Matrix Matrix::operator*(const Vector & obj)const
	{
		Matrix total = Matrix(rows, 1);
		double num = 0.0;
		if (cols == obj.length)
			for (int i = 0; i < rows; i++)
				for (int j = 0; j < 1; j++)
					for (int k = 0; k < cols; k++)
					{
						num = data[i][k] * obj[k];
						total.data[i][j] += num;
					}
		else throw "operator* failed due to dimension mismatch";
		return total;
	}
	Matrix Matrix::operator*(const double obj)const
	{
		Matrix total = *this;
		double num = 0.0;
		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
				total.data[i][j] = data[i][j] * obj;
		return total;
	}
	Vector & Matrix::operator[](int row)
	{
		if (row < 0 || row >= rows)throw "index out of bounds in matrix";
		if (current_row)current_row->~Vector();
		current_row = Vector::reference_of_array(data[row], rows);
		return *current_row;
	}
	Matrix Matrix::t()const
	{
		Matrix total = Matrix(cols, rows);
		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
				total.data[j][i] = data[i][j];
		return total;
	}
	Matrix Matrix::inv()const
	{
		Matrix M1_copy = *this;
		int dimension = cols;
		Matrix out = eye(dimension);
		double ** input = M1_copy.data;
		double ** output = out.data;
		int i, j, k;
		for (i = 0; i<dimension; ++i)
		{
			for (j = 0; j<dimension; ++j)
			{
				if (input[j][i] != 0)
				{
					swap(input, output, dimension, 0, j);
					break;
				}
			}
			for (j = 0; j<dimension; ++j)
			{
				if (j == 0)
				{
					for (k = dimension - 1; k >= 0; --k)
						output[j][k] /= input[j][i];
					for (k = dimension - 1; k >= i; --k)
						input[j][k] /= input[j][i];
				}
				else
				{
					for (k = dimension - 1; k >= 0; --k)
						output[j][k] = output[j][k] - input[j][i] / input[0][i] * output[0][k];
					for (k = dimension - 1; k >= i; --k)
						input[j][k] = input[j][k] - input[j][i] / input[0][i] * input[0][k];
				}
			}
			swap(input, output, dimension, 0, (i + 1) % dimension);
		}
		reorderOutput(output, dimension);
		return out;
	}
	Matrix Matrix::operator/(Matrix & obj)const
	{
		return *this * obj.inv();
	}
	Matrix Matrix::operator/(double obj)const
	{
		Matrix total = *this;
		double num = 0.0;
		for (int i = 0; i < rows; i++)
			for (int j = 0; j < cols; j++)
			{
				total.data[i][j] = data[i][j] / obj;
			}
		return total;
	}



}
