#include "Math.h"



Math::Math()
{
}


Math::~Math()
{
}

float Math::Dot(xyz* xyz1, xyz* xyz2)
{
	return (xyz1->x * xyz2->x) + (xyz1->y * xyz2->y) + (xyz1->z * xyz2->z);
}

xyz Math::CrossProduct(xyz * xyz1, xyz * xyz2)
{
	xyz crossProduct;

	float crossX;
	float crossY;
	float crossZ;

	crossX = (xyz1->y * xyz2->z) - (xyz1->z * xyz2->y);
	crossY = (xyz1->z * xyz2->x) - (xyz1->x * xyz2->z);
	crossZ = (xyz1->x * xyz2->y) - (xyz1->y * xyz2->x);

	crossProduct = { crossX, crossY, crossZ };

	return crossProduct;
}

xyz Math::Normal(xyz * point1, xyz * point2, xyz * point3)
{
	xyz normal;

	xyz v1 = { (point2->x - point1->x), (point2->y - point1->y), (point2->z - point1->z) };
	xyz v2 = { (point3->x - point1->x), (point3->y - point1->y), (point3->z - point1->z) };

	xyz crossProduct = CrossProduct(&v1, &v2);

	float distance = sqrt(pow((v2.x - v1.x), 2) + pow((v2.y - v1.y), 2) + pow((v2.z - v1.z), 2));

	float normalX = crossProduct.x / distance;
	float normalY = crossProduct.y / distance;
	float normalZ = crossProduct.z / distance;

	normal = { normalX, normalY, normalZ };

	return normal;
}

Plane Math::PlaneVal(xyz* point1, xyz* point2, xyz* point3)
{
	Plane plane;
	xyz normal = Normal(point1, point2, point3);
	float d = Dot(&normal, point1);

	plane = { normal, d };

	return plane;
}

float Math::CalculatePlaneValForPoint(Plane * plane, xyz * point)
{
	return Dot(&plane->normal, point) + plane->d;
}
