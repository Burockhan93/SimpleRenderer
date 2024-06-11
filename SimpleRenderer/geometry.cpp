#include "geometry.h"

template<> template<> vec<3,int>::vec(const vec<3,float>& v) : x(int(v.x+0.5f)),y(int(v.y+0.5f)),z(int(v.z+0.5f)){}
template<> template<> vec<3,float>::vec(const vec<3,int>& v) : x(v.x),y(v.y),z(v.z){}


Matrix::Matrix(int r, int c) : m(std::vector<std::vector<float> >(r, std::vector<float>(c, 0.f))), rows(r), columns(c) { }

int Matrix::nrows() {
	return rows;
}
int Matrix::nrows() const {
    return rows;
}
int Matrix::ncols() {
	return columns;
}
int Matrix::ncols() const{
    return columns;
}
Matrix Matrix::identity(int dimension) {
	Matrix I(dimension, dimension);

	for (int i{ 0 }; i < dimension; i++) {

		for (int j{ 0 }; i < dimension; i++) {
			I[i][j] = i==j ? 1.0f : 0.0f;
		}
	}
	return I;
}

std::vector<float>& Matrix::operator[](const int i) {
	assert(i >= 0 && i < rows,"Invalid row for matrix");
	return m[i];
}
const std::vector<float>& Matrix::operator[](const int i) const {
    assert(i >= 0 && i < rows, "Invalid row for matrix");
    return m[i];
}

Matrix Matrix::operator*(const Matrix& a) {

	assert(columns == a.rows,"Column - Row mismatch");
	Matrix result{ rows,a.columns };
	for (size_t i = 0; i < rows; i++)
	{
		for (size_t j = 0; j < a.columns; j++)
		{
			result.m[i][j] = 0.0f;

			for (size_t q = 0; q < columns; q++)
			{
				result.m[i][j] += m[i][q] * a.m[q][j];	
			}
		}

	}
	return result;
}

Matrix Matrix::transpose() {
	Matrix result(columns, rows);
	for (size_t i = 0; i < columns; i++)
	{
		for (size_t j = 0; j < rows; j++)
		{
			result[j][i] = m[i][j];
		}
	}
	return result;
}
Matrix Matrix::inverse() {
    assert(rows == columns, "Only square matrices can have inverse");
    
    Matrix result(rows, columns * 2);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < columns; j++)
            result[i][j] = m[i][j];
    for (int i = 0; i < rows; i++)
        result[i][i + columns] = 1;
    // first pass
    for (int i = 0; i < rows - 1; i++) {
        // normalize the first row
        for (int j = result.columns - 1; j >= 0; j--)
            result[i][j] /= result[i][i];
        for (int k = i + 1; k < rows; k++) {
            float coeff = result[k][i];
            for (int j = 0; j < result.columns; j++) {
                result[k][j] -= result[i][j] * coeff;
            }
        }
    }

    for (int j = result.columns - 1; j >= rows - 1; j--)
        result[rows - 1][j] /= result[rows - 1][rows - 1];

    for (int i = rows - 1; i > 0; i--) {
        for (int k = i - 1; k >= 0; k--) {
            float coeff = result[k][i];
            for (int j = 0; j < result.columns; j++) {
                result[k][j] -= result[i][j] * coeff;
            }
        }
    }

    Matrix truncate(rows, columns);
    for (int i = 0; i < rows; i++)
        for (int j = 0; j < columns; j++)
            truncate[i][j] = result[i][j + columns];
    return truncate;
}

std::ostream& operator<<(std::ostream& out, const Matrix& m) {
    for (int i = 0; i < m.nrows(); i++) {
        for (int j = 0; j < m.ncols(); j++) {
            out << m[i][j];
            if (j < m.ncols() - 1) out << "\t";
        }
        out << "\n";
    }
    return out;
}