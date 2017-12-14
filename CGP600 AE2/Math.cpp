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

	float distance = sqrt(pow((v2.x - v1.x), 2) + pow((v2.y - v1.y), 2) + pow((v2.z - v1.z), 2));

	xyz crossProduct = CrossProduct(&v1, &v2);


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

int Math::Sign(float number) // returns 1 or -1 depending on sign of number, or 0 if zero
{
	return (number < 0.0f ? -1 : (number > 0.0f ? 1 : 0));
}

xyz Math::PlaneIntersection(Plane* p, xyz* point1, xyz* point2)
{

	//xyz pointOfIntersection;
	xyz planeNormal = p->normal;
	xyz pointOnRay;
	float planeD = p->d;

	float rayX = point2->x - point1->x;
	float rayY = point2->y - point1->y;
	float rayZ = point2->z - point1->z;

	xyz ray = { rayX, rayY, rayZ };

	float t = (-planeD - Dot(&planeNormal, point1)) / Dot(&planeNormal, &ray);

	pointOnRay = { point1->x + (ray.x * t), point1->y + (ray.y * t), point1->z + (ray.z * t) };

	return pointOnRay;
}

bool Math::InTriangle(xyz* triangle, xyz* triangle1, xyz* triangle2, xyz* point)
{
	xyz AB = { triangle1->x - triangle->x, triangle1->y - triangle->y, triangle1->z - triangle->z };
	xyz AP = { point->x - triangle->x, point->y - triangle->y, point->z - triangle->z };

	xyz BC = { triangle2->x - triangle1->x, triangle2->y - triangle1->y, triangle2->z - triangle1->z };
	xyz BP = { point->x - triangle1->x, point->y - triangle1->y, point->z - triangle1->z };

	xyz CA = { triangle->x - triangle2->x, triangle->y - triangle2->y, triangle->z - triangle2->z };
	xyz CP = { point->x - triangle2->x, point->y - triangle2->y, point->z - triangle2->z };

	xyz crossABAP = CrossProduct(&AP, &AB);
	xyz crossBCBP = CrossProduct(&BP, &BC);
	xyz crossCACP = CrossProduct(&CP, &CA);

	float dot1 = Dot(&crossABAP, &crossBCBP);
	float dot2 = Dot(&crossBCBP, &crossCACP);
	float dot3 = Dot(&crossCACP, &crossABAP);

	if ((dot1 && dot2 && dot3) >= 0)
	{
		return true;
	}

	return false;
}