#pragma once

#include "objfilemodel.h"

struct Plane
{
	xyz normal;
	float d;
};

class Math
{
public:
	Math();
	~Math();

	float Dot(xyz* xyz1, xyz* xyz2);
	xyz CrossProduct(xyz* xyz1, xyz* xyz2);
	xyz Normal(xyz* point1, xyz* point2, xyz* point3);

	Plane PlaneVal(xyz* point1, xyz* point2, xyz* point3);

	float CalculatePlaneValForPoint(Plane* plane, xyz* point);
	int Sign(float number);
	xyz PlaneIntersection(Plane * p, xyz * point1, xyz * point2);

	bool InTriangle(xyz * triangle, xyz * triangle1, xyz * triangle2, xyz * point);

};

