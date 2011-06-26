/*
wadlib
Copyright (c) 2011 Bruno Sanches  http://code.google.com/p/wadlib

This software is provided 'as-is', without any express or implied warranty.
In no event will the authors be held liable for any damages arising from the use of this software.
Permission is granted to anyone to use this software for any purpose, 
including commercial applications, and to alter it and redistribute it freely, 
subject to the following restrictions:

1. The origin of this software must not be misrepresented; you must not claim that you wrote the original software. If you use this software in a product, an acknowledgment in the product documentation would be appreciated but is not required.
2. Altered source versions must be plainly marked as such, and must not be misrepresented as being the original software.
3. This notice may not be removed or altered from any source distribution.
*/


#ifndef VECTOR_2D_H
#define VECTOR_2D_H

#include <math.h>

//Simple and straight forward vector for basic operations
//Not the best in the world, not optimized, but can handle the needed :)
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