#pragma once
#include <math.h>
#include <initializer_list>
#include <algorithm>
#include <limits>
#include "vec3.h"

template<uint32_t SIZE, typename TYPE = float>
class Matr
{
	TYPE matr[SIZE][SIZE];
public:
	Matr() {}
	Matr(const std::initializer_list<std::initializer_list<TYPE>>& initializer)
	{
		for (uint32_t i = 0; i < SIZE; ++i)
		{
			std::copy((initializer.begin() + i)->begin(), (initializer.begin() + i)->end(), matr[i]);
		}
	}

	inline const uint32_t size() const { return SIZE; }
	inline Matr<SIZE, TYPE> operator*(TYPE k) const
	{
		Matr<SIZE, TYPE> m;

		for (uint32_t i = 0; i < m.size(); ++i)
		{
			for (uint32_t j = 0; j < m.size(); ++j)
			{
				m.matr[i][j] = matr[i][j] * k;
			}
		}
		return m;
	}
	inline Matr<SIZE, TYPE> operator/(TYPE k) const
	{
		return this->operator*(1.f / k);
	}
	inline Matr<SIZE, TYPE> operator*(const Matr<SIZE, TYPE>& m) const
	{
		Matr<SIZE, TYPE> n;
		TYPE sum = 0;
		for (uint32_t i = 0; i < m.size(); ++i)
		{
			for (uint32_t j = 0; j < m.size(); ++j)
			{
				sum = 0;
				for (uint32_t k = 0; k < m.size(); ++k)
				{
					sum += matr[i][k] * m.matr[k][j];
				}
				n.matr[i][j] = sum;
			}
		}
		return n;
	}

	inline TYPE* const& operator[](uint32_t r)
	{
		return matr[r];
	}
	inline const TYPE* const& operator[](uint32_t r) const
	{
		return matr[r];
	}

	inline Matr<SIZE - 1, TYPE> matr_without(const uint32_t r, const uint32_t c) const
	{
		Matr<SIZE - 1, TYPE> m;
		for (uint32_t i = 0, rk = 0; i < SIZE; ++i)
		{
			if (r == i) continue;

			for (uint32_t j = 0, rc = 0; j < SIZE; ++j)
			{
				if (c == j) continue;
				m.matr[rk][rc] = matr[i][j];
				++rc;
			}
			++rk;
		}
		return m;
	}

	inline Matr<SIZE, TYPE> operator=(const Matr<SIZE, TYPE>& m)
	{
		for (uint32_t i = 0; i < SIZE; ++i)
		{
			for (uint32_t j = 0; j < SIZE; ++j)
			{
				matr[i][j] = m.matr[i][j];
			}
		}
		return *this;
	}

	inline TYPE determinant() const;

	inline Matr<SIZE, TYPE> invert() const;

	inline static Matr<SIZE, TYPE> identity();

	inline Matr<SIZE, TYPE> transpose() const;

	inline static void fill_row(TYPE* const& row, const std::initializer_list<TYPE>& init);

	inline static void add_to_row(float* const& row, const Vec3& vec);

	friend class Matr<SIZE + 1, TYPE>;
};

template<typename TYPE>
class Matr<1, TYPE>
{
	TYPE matr[1][1];
public:
	Matr() {}
	inline const uint32_t size() const { return 1; }
	inline Matr<1, TYPE> operator*(TYPE k) const
	{
		Matr<1, TYPE> m;

		for (uint32_t i = 0; i < m.size(); ++i)
		{
			for (uint32_t j = 0; j < m.size(); ++j)
			{
				m.matr[i][j] = matr[i][j] * k;
			}
		}
		return m;
	}
	inline Matr<1, TYPE> operator/(TYPE k) const
	{
		return this->operator*(1.f / k);
	}
	inline Matr<1, TYPE> operator*(const Matr<1, TYPE>& m) const
	{
		Matr<1, TYPE> n;
		TYPE sum = 0;
		for (uint32_t i = 0; i < m.size(); ++i)
		{
			for (uint32_t j = 0; j < m.size(); ++j)
			{
				sum = 0;
				for (uint32_t k = 0; k < m.size(); ++k)
				{
					sum += matr[i][k] * m.matr[k][j];
				}
				n.matr[i][j] = sum;
			}
		}
		return n;
	}

	inline TYPE* const& operator[](uint32_t r)
	{
		return matr[r];
	}
	inline const TYPE* const& operator[](uint32_t r) const
	{
		return matr[r];
	}

	inline Matr<1, TYPE> matr_without(const uint32_t r, const uint32_t c) const
	{
		Matr<1, TYPE> m;
		m.matr[0][0] = std::numeric_limits<TYPE>::infinity();
		return m;
	}

	inline TYPE determinant() const
	{
		return matr[0][0];
	}

	inline Matr<1, TYPE> invert() const
	{
		Matr<1,TYPE> m;
		m.matr[0][0] = 1.f / matr[0][0];
		return m;
	}

	inline static Matr<1, TYPE> identity()
	{
		Matr<1, TYPE> m;
		m.matr[0][0] = 1.f;
		return m;
	}

	inline static void fill_row(TYPE* const& row, const std::initializer_list<TYPE>& init);
	friend class Matr<2, TYPE>;
};

template<uint32_t SIZE, typename TYPE>
inline Matr<SIZE, TYPE> Matr<SIZE, TYPE>::identity()
{
	Matr<SIZE, TYPE> m;
	for (uint32_t i = 0; i < SIZE; ++i)
	{
		for (uint32_t j = 0; j < SIZE; ++j)
		{
			if (i == j)
			{
				m.matr[i][j] = 1.f;
				continue;
			}
			m.matr[i][j] = 0;
		}
	}
	return m;
}

template<uint32_t SIZE, typename TYPE>
inline Matr<SIZE, TYPE> Matr<SIZE, TYPE>::transpose() const
{
	Matr<SIZE, TYPE> result;
	for (uint32_t row = 0; row < SIZE; ++row)
	{
		for (uint32_t col = 0; col < SIZE; ++col)
		{
			result.matr[col][row] = matr[row][col];
		}
	}
	return result;
}

template<uint32_t SIZE, typename TYPE>
inline void Matr<SIZE, TYPE>::fill_row(TYPE* const& row, const std::initializer_list<TYPE>& init)
{
	std::copy(init.begin(), init.end(), row);
}

template<uint32_t SIZE, typename TYPE>
inline TYPE Matr<SIZE, TYPE>::determinant() const
{
	if (SIZE == 1)
		return matr[0][0];
	if (SIZE == 2)
		return matr[0][0] * matr[1][1] - matr[0][1] * matr[1][0];
	if (SIZE == 3)
	{
		return matr[0][0] * matr[1][1] * matr[2][2] +
			matr[0][1] * matr[1][2] * matr[2][0] +
			matr[0][2] * matr[1][0] * matr[2][1] -
			matr[0][2] * matr[1][1] * matr[2][0] -
			matr[0][1] * matr[1][0] * matr[2][2] -
			matr[0][0] * matr[1][2] * matr[2][1];
	}
	TYPE det = 0;
	for (uint32_t i = 0; i < SIZE; ++i)
	{
		det += powf(-1, i) * matr[0][i] * this->matr_without(0, i).determinant();
	}
	return det;
}

template<uint32_t SIZE, typename TYPE>
inline Matr<SIZE, TYPE> Matr<SIZE, TYPE>::invert() const
{
	Matr<SIZE, TYPE> m;
	for (uint32_t i = 0; i < SIZE; ++i)
	{
		for (uint32_t j = 0; j < SIZE; ++j)
		{
			m[j][i] = powf(-1, i + j) * matr_without(i, j).determinant();
		}
	}
	return m / determinant();
}

template<uint32_t SIZE, typename TYPE>
inline void Matr<SIZE, TYPE>::add_to_row(float* const& row, const Vec3& vec)
{
	row[0] += vec.e[0];
	row[1] += vec.e[1];
	row[2] += vec.e[2];
}