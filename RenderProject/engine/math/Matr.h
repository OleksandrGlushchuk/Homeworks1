#pragma once
#include <math.h>
#include <initializer_list>
#include <algorithm>
#include <limits>
#include "vec3.h"

template<int sz>
class Matr
{
	float matr[sz][sz];
public:
	Matr() {}
	inline const int size() const { return sz; }
	inline Matr operator*(float k) const
	{
		Matr<sz> m;

		for (int i = 0; i < m.size(); i++)
		{
			for (int j = 0; j < m.size(); j++)
			{
				m.matr[i][j] = matr[i][j] * k;
			}
		}
		return m;
	}
	inline Matr operator/(float k) const
	{
		return this->operator*(1.f / k);
	}
	inline Matr operator*(const Matr& m) const
	{
		Matr<sz> n;
		float sum = 0;
		for (int i = 0; i < m.size(); i++)
		{
			for (int j = 0; j < m.size(); j++)
			{
				sum = 0;
				for (int k = 0; k < m.size(); k++)
				{
					sum += matr[i][k] * m.matr[k][j];
				}
				n.matr[i][j] = sum;
			}
		}
		return n;
	}

	inline float* const & operator[](int r)
	{
		return matr[r];
	}
	inline const float* const & operator[](int r) const
	{
		return matr[r];
	}

	inline Matr<sz-1> matr_without(const int r, const int c) const
	{
		Matr<sz-1> m;
		for (int i = 0, rk = 0; i < sz; i++)
		{
			if (r == i) continue;

			for (int j = 0, rc = 0; j < sz; j++)
			{
				if (c == j) continue;
				m.matr[rk][rc] = matr[i][j];
				++rc;
			}
			++rk;
		}
		return m;
	}

	inline float determinant() const;

	inline Matr invert() const;

	inline static Matr identity();

	inline static void fill_row(float * const & row, const std::initializer_list<float> &init);

	inline static void add_to_row(float* const& row, const Vec3& vec);

	friend class Matr<sz + 1>;
};

template<>
class Matr<1>
{
	float matr[1][1];
public:
	Matr() {}
	inline const int size() const { return 1; }
	inline Matr operator*(float k) const
	{
		Matr<1> m;

		for (int i = 0; i < m.size(); i++)
		{
			for (int j = 0; j < m.size(); j++)
			{
				m.matr[i][j] = matr[i][j] * k;
			}
		}
		return m;
	}
	inline Matr operator/(float k) const
	{
		return this->operator*(1.f / k);
	}
	inline Matr operator*(const Matr& m) const
	{
		Matr<1> n;
		float sum = 0;
		for (int i = 0; i < m.size(); i++)
		{
			for (int j = 0; j < m.size(); j++)
			{
				sum = 0;
				for (int k = 0; k < m.size(); k++)
				{
					sum += matr[i][k] * m.matr[k][j];
				}
				n.matr[i][j] = sum;
			}
		}
		return n;
	}

	inline float* const& operator[](int r)
	{
		return matr[r];
	}
	inline const float* const& operator[](int r) const
	{
		return matr[r];
	}

	inline Matr<1> matr_without(const int r, const int c) const
	{
		Matr<1> m;
		m.matr[0][0] = std::numeric_limits<float>::infinity();
		return m;
	}

	inline float determinant() const
	{
		return matr[0][0];
	}

	inline Matr invert() const
	{
		Matr m;
		m.matr[0][0] = 1.f / matr[0][0];
		return m;
	}

	inline static Matr identity()
	{
		Matr<1> m;
		m.matr[0][0] = 1.f;
		return m;
	}

	inline static void fill_row(float* const& row, const std::initializer_list<float>& init);
	friend class Matr<2>;
};

template<int sz>
inline Matr<sz> Matr<sz>::identity()
{
	Matr<sz> m;
	for (int i = 0; i < sz; i++)
	{
		for (int j = 0; j < sz; j++)
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

template<int sz>
inline void Matr<sz>::fill_row(float* const& row, const std::initializer_list<float>& init)
{
	std::copy(init.begin(), init.end(), row);
}

template<int sz>
inline float Matr<sz>::determinant() const
{
	if (sz == 1)
		return matr[0][0];
	if (sz == 2)
		return matr[0][0] * matr[1][1] - matr[0][1] * matr[1][0];
	if (sz == 3)
	{
		return matr[0][0] * matr[1][1] * matr[2][2] +
			matr[0][1] * matr[1][2] * matr[2][0] +
			matr[0][2] * matr[1][0] * matr[2][1] -
			matr[0][2] * matr[1][1] * matr[2][0] -
			matr[0][1] * matr[1][0] * matr[2][2] -
			matr[0][0] * matr[1][2] * matr[2][1];
	}
	float det = 0;
	for (int i = 0; i < sz; i++)
	{
		det += pow(-1, i) * matr[0][i] * this->matr_without(0, i).determinant();
	}
	return det;
}

template<int sz>
inline Matr<sz> Matr<sz>::invert() const
{
	Matr<sz> m;
	for (int i = 0; i < sz; i++)
	{
		for (int j = 0; j < sz; j++)
		{
			m[j][i] = pow(-1, i + j) * matr_without(i, j).determinant();
		}
	}
	return m / determinant();
}

template<int sz>
inline void Matr<sz>::add_to_row(float* const& row, const Vec3& vec)
{
	row[0] += vec.e[0];
	row[1] += vec.e[1];
	row[2] += vec.e[2];
}