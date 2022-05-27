#include "Matr.h"
Matr Matr::matr_without(const int r, const int c) const
{
	Matr m(size() - 1);
	for (int i = 0, rk = 0; i < size(); i++)
	{
		if (r == i) continue;

		for (int j = 0, rc = 0; j < size(); j++)
		{
			if (c == j) continue;
			m.matr[rk][rc] = matr[i][j];
			++rc;
		}
		++rk;
	}
	return m;
}

float determinant(const Matr& matr)
{
	if (matr.size() == 1)
		return matr[0][0];
	if (matr.size() == 2)
		return matr[0][0] * matr[1][1] - matr[0][1] * matr[1][0];
	if (matr.size() == 3)
	{
		return matr[0][0] * matr[1][1] * matr[2][2] +
			matr[0][1] * matr[1][2] * matr[2][0] +
			matr[0][2] * matr[1][0] * matr[2][1] -
			matr[0][2] * matr[1][1] * matr[2][0] -
			matr[0][1] * matr[1][0] * matr[2][2] -
			matr[0][0] * matr[1][2] * matr[2][1];
	}
	float det = 0;
	for (int i = 0; i < matr.size(); i++)
	{
		det += pow(-1, i) * matr[0][i] * determinant(matr.matr_without(0, i));
	}
	return det;
}

Matr invert(const Matr& matr)
{
	Matr m(matr.size());
	for (int i = 0; i < matr.size(); i++)
	{
		for (int j = 0; j < matr.size(); j++)
		{
			m[j][i] = pow(-1, i + j) * determinant(matr.matr_without(i, j));
		}
	}
	return m / determinant(matr);
}
