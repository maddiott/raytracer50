#include "WorldObject.h"

void WorldObject::SetShapeType(WorldObjectType ShapeType)
{
	mShapeType = ShapeType;
}

WorldObjectType WorldObject::GetShapeType()
{
	return mShapeType;
}

void WorldObject::SetColor(color3 color)
{
	mColor = color;
}

color3 WorldObject::GetColor()
{
	return mColor;
}


void WorldObject::AddTriangle(const triangle3d& NewTriangle)
{
	mTriangles.push_back(NewTriangle);
}

void WorldObject::AddPolygon(const polygon3d& NewPolygon)
{
	mPolygons.push_back(NewPolygon);
}

void WorldObject::AddSphere(const sphere3d& NewSphere)
{
	mSpheres.push_back(NewSphere);
}

point3d WorldObject::TestIntersection(const point3d& ray)
{
	// We want to return the intersection normal if there is an intersection
	// Otherwise return origin
	point3d IntersectionNormal(0, 0, 0);

	switch (mShapeType)
	{
	case WorldObjectType::Point:
		break;
	case WorldObjectType::Sphere:
        IntersectionNormal = SphereIntersection(ray);
		break;
	case WorldObjectType::Triangle:
		break;
	case WorldObjectType::Polygon:
        IntersectionNormal = PolygonIntersection(ray);
		break;
	default:
		break;
	}

    return IntersectionNormal;
}

point3d WorldObject::SphereIntersection(const point3d& RayNormalVector)
{
    for (const sphere3d& sphere : mSpheres)
    {
        // Send out ray (see Glassner chapter 2, this is the algebraic form of the parametric intersection)
        double A = RayNormalVector.x * RayNormalVector.x
            + RayNormalVector.y * RayNormalVector.y
            + RayNormalVector.z * RayNormalVector.z;

        // Origin is defined as 0 so X_o = Y_o = Z_o = 0
        double B = 2 * ((RayNormalVector.x * (-sphere.center.x))
            + (RayNormalVector.y * (-sphere.center.y))
            + (RayNormalVector.z * (-sphere.center.z)));

        double C = (-sphere.center.x) * (-sphere.center.x)
            + (-sphere.center.y) * (-sphere.center.y)
            + (-sphere.center.z) * (-sphere.center.z) - (sphere.radius * sphere.radius);

        // Check intersect
        double discriminant = B * B - 4 * A * C;
        if (discriminant < 0)
        {
            // If discriminant is less than zero we miss
            continue;
        }

        double t0 = (-B - sqrt(discriminant)) / 2;
        double t1 = (-B + sqrt(discriminant)) / 2;

        // Closest hit is the one we care about
        double distance = (t0 < t1) ? t0 : t1;

        point3d intersectVector;
        intersectVector.x = distance * RayNormalVector.x;
        intersectVector.y = distance * RayNormalVector.y;
        intersectVector.z = distance * RayNormalVector.z;

        // Check normal
        point3d normal;
        normal.x = (intersectVector.x - sphere.center.x) / sphere.radius;
        normal.y = (intersectVector.y - sphere.center.y) / sphere.radius;
        normal.z = (intersectVector.z - sphere.center.z) / sphere.radius;

        // Only allowed to hit one sphere
        return normal;
    }

    return point3d(0, 0, 0);
}

point3d WorldObject::TriangleIntersection(const point3d& ray)
{
    return point3d(0, 0, 0);
}

// Algorithm is from Glassner
point3d WorldObject::PolygonIntersection(const point3d& ray)
{
    plane2d planeEquation;

    // Notation from Glassner
    double v0, vd, t;
    point3d intersectionPoint;
    point3d rayDirection(0, 0, 1);
    
    for (const polygon3d& poly: mPolygons)
    {
        // Check if the ray intersects our plane
        vd = dotProduct(rayDirection, poly.normal);
        if (vd >= 0)
        {
            continue;
        }

        // We need to get our point into a parametric equation
        // Assumming that our n > 3 vertices are coplanar
        // Method is from here
        // https://math.stackexchange.com/questions/2177006/how-to-define-a-plane-based-on-4-points
        planeEquation.A = poly.normal.x;
        planeEquation.B = poly.normal.y;
        planeEquation.C = poly.normal.z;

        planeEquation.D = dotProduct(poly.normal, poly.vertices[0]);
        
        // Not necessary, but it matches the math
        v0 = - (dotProduct(poly.normal, ray) + planeEquation.D);
        
        t =  - v0 / vd;

        // Ray hits behind us
        if (t < 0)
        {
            continue;
        }
        
        intersectionPoint.x = ray.x + t * rayDirection.x;
        intersectionPoint.y = ray.y + t * rayDirection.y;
        intersectionPoint.z = ray.z + t * rayDirection.z;

        // At this point we know we intersected the plane, but not the polygon
        int numberCrossings = 0;
        int signHolder = 0;

        std::vector<point2d> projectedPoints{};
        
        int dominantCoordinant;
        if ((abs(planeEquation.A) >= abs(planeEquation.B)) && (abs(planeEquation.A) >= abs(planeEquation.C)))
        {
            dominantCoordinant = 0;
        }
        else if ((abs(planeEquation.B) >= abs(planeEquation.A)) && (abs(planeEquation.B) >= abs(planeEquation.C)))
        {
            dominantCoordinant = 1;
        }
        else
        {
            dominantCoordinant = 2;
        }

        int numberOfVertices = 0;
        for (auto const& polyInt : poly.vertices)
        {
            switch (dominantCoordinant)
            {
            case 0:
                projectedPoints.push_back(point2d(polyInt.y - intersectionPoint.y, polyInt.z - intersectionPoint.z));
                break;
            case 1:
                projectedPoints.push_back(point2d(polyInt.x - intersectionPoint.x, polyInt.z - intersectionPoint.z));
                break;
            case 2:
                projectedPoints.push_back(point2d(polyInt.x - intersectionPoint.x, polyInt.y - intersectionPoint.y));
                break;
            default:
                break;
            }

            numberOfVertices++;
        }
        // Need to add the ray to the list of points

        // Initial sign holder
        signHolder = (projectedPoints[0].V < 0) ? -1 : 1;

        int nextSignHolder = 0;

        for (int i = 0; i < (numberOfVertices - 2); i++)
        {
            int j = (i + 1) % (numberOfVertices);
            nextSignHolder = (projectedPoints[j].V < 0) ? -1 : 1;

            if (nextSignHolder != signHolder)
            {
                if ((projectedPoints[i].U > 0) && (projectedPoints[j].U > 0))
                {
                    numberCrossings++;
                }
                else if ((projectedPoints[i].U > 0) || (projectedPoints[j].U > 0))
                {
                    if ((projectedPoints[i].U - (projectedPoints[i].V)
                        * ((projectedPoints[j].U - projectedPoints[i].U)
                            / ((projectedPoints[j].V - projectedPoints[i].V)))) > 0)
                    {
                        numberCrossings++;
                    }
                }

                signHolder = nextSignHolder;
            }
        }

        // If it's an odd number of crossings we're inside
        if ((numberCrossings % 2) == 1)
        {
            // Think we need to figure out which intersection is closest, but I'll try this for now
            return poly.normal;
        }

    }

    return point3d(0, 0, 0);
}

