#ifndef VECTOR_2D_H
#define VECTOR_2D_H

#include <math.h>

struct Vector2d_s
{
	Vector2d_s(float x, float y):
		fX(x),
		fY(y)
	{
		//empty
	}

	Vector2d_s(const Vector2d_s &a, const Vector2d_s &b)
	{
		fX = b.fX - a.fX;
		fY = b.fY - a.fY;
	}

	void Normalize()
	{
		float len = sqrt((fX * fX) + (fY * fY));
		fX /= len;
		fY /= len;
	}

	void Add(const Vector2d_s &a)
	{
		fX += a.fX;
		fY += a.fY;
	}

	void Scale(float v)
	{
		fX *= v;
		fY *= v;
	}

	void Rotate90()
	{
		float t = fX;
		fX = -fY;
		fY = t;
	}

	float fX, fY;
};

#endif