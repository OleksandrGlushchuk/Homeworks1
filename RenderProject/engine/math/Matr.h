#pragma once
#include <vector>
class Matr
{
	std::vector< std::vector<float>> matr;
public:
	Matr(int size = 0)
	{
		matr.resize(size);
		for (int i = 0; i < size; i++)
		{
			matr[i].resize(size);
			matr[i][i] = 1.f;
		}
	}
	Matr matr_without(const int r, const int c) const;
	inline const int size() const { return matr.size(); }
	inline std::vector<float>& operator[](int r)
	{
		return matr[r];
	}
	inline const std::vector<float>& operator[](int r) const
	{
		return matr[r];
	}
	inline Matr operator*(float k) const
	{
		Matr m(size());
		for (int i = 0; i < m.size(); i++)
		{
			for (int j = 0; j < m.size(); j++)
			{
				m[i][j] = matr[i][j] * k;
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
		Matr n(4);
		float sum = 0;
		for (int i = 0; i < m.size(); i++)
		{
			for (int j = 0; j < m.size(); j++)
			{
				sum = 0;
				for (int k = 0; k < m.size(); k++)
				{
					sum += matr[i][k] * m[k][j];
				}
				n[i][j] = sum;
			}
		}
		return n;
	}
};

float determinant(const Matr& matr);

Matr invert(const Matr& matr);