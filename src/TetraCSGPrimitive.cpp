/////////////////////////////////////////////////////////////////////
// Standard include files
#include <iostream>
#include <vector>
#include <wx/wx.h>
#include <GL/gl.h>
#include <GL/glu.h>

/////////////////////////////////////////////////////////////////////
// User-Defined include files
#include "TetraCSGPrimitive.h"

/////////////////////////////////////////////////////////////////////
// CSGObject class member functions implementation begins

void
CSGObject :: Render ()
{
	glBegin (GL_TRIANGLES);
		for (unsigned int i = 0; i < triangles.size (); i++)
		{
			if (triangles[i] != NULL)
			{
				glNormal3dv ((triangles[i])->normal);
				for (int k = 0; k < 3; k++)
				{
					if (triangles[i]->points[k])
					glVertex3dv (triangles[i]->points[k]->vertex);
				}
			}
		}
	glEnd ();
}

bool
CSGObject :: InsideBounds (CSGPoint* pt, BoundBox *bounds)
{
	if (pt->GetX() >= bounds->x && pt->GetX() <= (bounds->x+bounds->w) &&
			pt->GetY() >= bounds->y && pt->GetY() <= (bounds->y+bounds->h) &&
			pt->GetZ() >= bounds->z && pt->GetZ() <= (bounds->z+bounds->d))
	{
		pt->inside = true;
		return true;
	}
	return false;
}

CSGPoint*
CSGObject :: GetIntersectionPoint (CSGTriangle *tri, 
								   CSGPoint *sp, CSGPoint *ep)
{
	double x, y, z, x1, y1, z1, x2, y2, z2, 
		x3, y3, z3, x4, y4, z4, x5, y5, z5, 
		maxx, maxy, maxz, minx, miny, minz;
	double t, pos, neg, first, second;

	/* initialize the local variables
		* so that it is easy to use them
		* instead of accessing the variables
		* directly */
	x1 = tri->points[0]->GetX ();
	y1 = tri->points[0]->GetY ();
	z1 = tri->points[0]->GetZ ();

	x2 = tri->points[1]->GetX ();
	y2 = tri->points[1]->GetY ();
	z2 = tri->points[1]->GetZ ();

	x3 = tri->points[2]->GetX ();
	y3 = tri->points[2]->GetY ();
	z3 = tri->points[2]->GetZ ();

	x4 = sp->GetX (); 
	y4 = sp->GetY (); 
	z4 = sp->GetZ ();

	x5 = ep->GetX (); 
	y5 = ep->GetY (); 
	z5 = ep->GetZ ();

	
	/* checking if the coordinates are within the range
		* by finding the bounding cube of the triangle from
		* maxx, maxy, maxz and minx, miny, minz values */
	
	/* find max and min coordinates */
	maxx = (x1 > x2) ? ((x1 > x3) ? x1 : x3) : ((x2 > x3) ? x2 : x3);
	maxy = (y1 > y2) ? ((y1 > y3) ? y1 : y3) : ((y2 > y3) ? y2 : y3);
	maxz = (z1 > z2) ? ((z1 > z3) ? z1 : z3) : ((z2 > z3) ? z2 : z3);
	
	minx = (x1 < x2) ? ((x1 < x3) ? x1 : x3) : ((x2 < x3) ? x2 : x3);
	miny = (y1 < y2) ? ((y1 < y3) ? y1 : y3) : ((y2 < y3) ? y2 : y3);
	minz = (z1 < z2) ? ((z1 < z3) ? z1 : z3) : ((z2 < z3) ? z2 : z3);

	/* check if the coordinates are within in the range */
	if ((x4 < minx && x5 < minx) || (x4 > maxx && x5 > maxx) || 
		(y4 < miny && y5 < miny) || (y4 > maxy && y5 > maxy) ||
		(z4 < minz && z5 < minz) || (z4 > maxz && z5 > maxz))
		return NULL;
	
	/* finding the intersection point, by finding the
	 * determinant of the matrices, as mentioned in the
	 * URL - mathworld.wolfram.com/Line-PlaneIntersection.html
	 */
	pos = x3*y1*z4 - x3*y4*z1 - x4*y1*z3;
	neg = x2*y4*z3 - x2*y3*z4 + x3*y2*z4 - x3*y4*z2 - 
			x4*y2*z3 + x4*y3*z2 + x1*y3*z4 - x1*y4*z3 -
			x4*y3*z1 - x1*y2*z4 + x1*y4*z2 + x2*y1*z4 -
			x2*y4*z1 - x4*y1*z2 + x4*y2*z1;

	first = -x1*y2*z3 + x1*y3*z2 + x2*y1*z3 - x2*y3*z1 -
			x3*y1*z2 + x3*y2*z1;
	second = x2*y3*z5 - x2*y5*z3 - x3*y2*z5 - x2*y1*z5 +
			x3*y5*z2 + x5*y2*z3 - x5*y3*z2 + x2*y5*z1 +
			x5*y1*z2 + x1*y2*z5 - x1*y3*z5 + x1*y5*z3 - x3*y1*z5
			- x1*y5*z2 - x5*y2*z1 + x3*y5*z1 + x5*y1*z3 - x5*y3*z1;
	first = first - neg + pos;
	second = second + neg + pos;

	/* if second is	zero, division results
		* in a floating point exception. And if
		* first is not zero, and second is zero,
		* it means that there is no point of
		* intersection */
	if (second != 0)
		t = first/second;
	else if (first == 0 && second == 0)
		t = 0;
	else
		return NULL;

	/* find the point of intersection
		* and return the point */
	x = x4 + (x5 - x4)*t;
	y = y4 + (y5 - y4)*t;
	z = z4 + (z5 - z4)*t;

	minx = (x4 < x5) ? x4 : x5;
	maxx = (x4 > x5) ? x4 : x5;
	miny = (y4 < y5) ? y4 : y5;
	maxy = (y4 > y5) ? y4 : y5;
	minz = (z4 < z5) ? z4 : z5;
	maxz = (z4 > z5) ? z4 : z5;
	
	if (tri->IsInside (x, y, z))
	{
		if (x == sp->GetX () && y == sp->GetY () && z == sp->GetZ ())
			return sp;
		else if (x == ep->GetX () && y == ep->GetY () && z == ep->GetZ ())
			return ep;
		else if (x >= minx && x <= maxx && y >= miny && y <= maxy &&
							z >= minz && z <= maxz)
			return new CSGPoint (x, y, z);
	}
	return NULL;
}

void
CSGObject :: ClipTriangle (CSGTriangle *tri, CSGObject *clip, 
						   bool internal, CSGObject *result)
{
	/* if all the points of the triangle are inside
	 * or outside, add the triangle to result */

	CSGPoint *pt;
	std::vector<CSGPoint*> points;

	for (int i = 0; i < 3; i++)
	{
		if (tri->points[i]->inside == internal &&
			tri->points[(i+1)%3]->inside == internal)
		{
			if (i == 0)
				points.push_back (tri->points[i]);
			if (i != 2)
				points.push_back (tri->points[(i+1)%3]);

			continue;
		}

		for (unsigned int k = 0; k < clip->triangles.size (); k++)
		{
			pt = GetIntersectionPoint (clip->triangles[k],
							tri->points[i], tri->points[(i+1)%3]);
			if (pt != NULL)
				break;
		}
		if (tri->points[i]->inside == internal && i == 0)
			points.push_back (tri->points[i]);
		if (pt != NULL)
			points.push_back (pt);
		if (tri->points[(i+1)%3]->inside == internal && i != 2)
			points.push_back (tri->points[(i+1)%3]);
	}

	size_t size = points.size ()-2;
	size = (size < 0) ? 0 : size;
	for (size_t i = 0; i < size; i++)
	{
		CSGTriangle *t = new CSGTriangle ();
		t->SetNormal (tri->normal[0], tri->normal[1], tri->normal[2]);
		t->SetPoints (points[0], points[i+1], points[i+2]);
		result->triangles.push_back (t);
	}
}

void
CSGObject :: Clip (CSGObject *obj, CSGObject *clip, bool internal, CSGObject *result)
{
	for (unsigned int i = 0; i < obj->triangles.size (); i++)
	{
		for (unsigned  k = 0; k < 3; k++)
		{
			InsideBounds (obj->triangles[i]->points[k], clip->GetBoundBox ());
		}

		ClipTriangle (obj->triangles[i], clip, internal, result);
	}
}

// CSGObject class member functions implementation ends
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// TetraCSGPrimitive class member functions implementation begins

TetraCSGPrimitive::TetraCSGPrimitive ()
{
}

double	TetraCSGPrimitive::GetX (void) 		
{ 
	return x; 
}

void TetraCSGPrimitive::SetX (double x) 
{
	this->x = x; 
}

double TetraCSGPrimitive::GetY (void)
{
	return y; 
}

void TetraCSGPrimitive::SetY (double y)
{
	this->y = y; 
}

double TetraCSGPrimitive::GetZ (void)
{
	return z; 
}

void TetraCSGPrimitive::SetZ (double z)
{
	this->z = z; 
}

// TetraCSGPrimitive class member functions implementation ends
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// TetraCSGParallelopiped class member functions implementation begins

TetraCSGParallelopiped :: TetraCSGParallelopiped (double x, double y, double z, 
						double w, double h, double d)
{
	SetX (x); SetY (y); SetZ (z);
	SetWidth (w);
	SetHeight (h);
	SetDepth (d);
	modified = true;
	object = CreateCSGObject (x, y, z, w, h, d);
}

TetraCSGParallelopiped :: ~TetraCSGParallelopiped ()
{
	if (object != NULL)
	{
		for (unsigned int i = 0; i < object->triangles.size (); i++)
			delete object->triangles[i];
		object->triangles.clear();
		delete object;
		object = NULL;
	}
}

CSGObject*
TetraCSGParallelopiped :: CreateCSGObject (double x, double y, double z,
										 double w, double h, double d)
{
	int DivisionsInX, DivisionsInY;
	double dx, dy, dz;
	int pointsPerSide;
	
	DivisionsInX = 4;
	DivisionsInY = 4;

	dx = width / DivisionsInX;
	dy = height / DivisionsInY;
	dz = depth / DivisionsInX;

	pointsPerSide = (DivisionsInX + 1) * (DivisionsInY + 1);

	object = new CSGObject ();
	object->points.resize (6 * pointsPerSide);

	/* create points at first to prevent duplication */

	/* points in front and back faces */
	for (int i = 0; i <= DivisionsInY; i++)
	{
		for (int j = 0 ; j <= DivisionsInX; j++)
		{
			object->points[i*(DivisionsInX+1) + j].SetCoord (x + j*dx, y + i*dy, z);
			object->points[pointsPerSide + i*(DivisionsInX+1) + j].SetCoord (x + j*dx, y + i*dy, z+d);
		}
	}

	/* points in left and right faces */
	for (int i = 0; i <= DivisionsInY; i++)
	{
		for (int j = 0; j <= DivisionsInX; j++)
		{
			object->points[pointsPerSide*2 + i*(DivisionsInX+1) + j].SetCoord (x, y + i*dy, z + j*dz);
			object->points[pointsPerSide*3 + i*(DivisionsInX+1) + j].SetCoord (x + w, y + i*dy, z + j*dz);
		}
	}

	/* points in top and bottom faces */
	for (int i = 0; i <= DivisionsInY; i++)
	{
		for (int j = 0; j <= DivisionsInX; j++)
		{
			object->points[pointsPerSide*4 + i*(DivisionsInX+1) + j].SetCoord (x + j*dx, y, z + i*dz);
			object->points[pointsPerSide*5 + i*(DivisionsInX+1) + j].SetCoord (x + j*dx, y + h, z + i*dz);
		}
	}

	/* create triangle faces from points */

	/* front and back faces */
	for (int i = 0; i < DivisionsInY; i++)
	{
		for (int j = 0; j < DivisionsInX; j++)
		{
			CSGTriangle *t;

			/* front face */
			t = new CSGTriangle ();
			t->SetPoints (&object->points[i*(DivisionsInX+1) + j], 
					&object->points[(i+1)*(DivisionsInX+1) + j], 
					&object->points[(i+1)*(DivisionsInX+1) + j + 1]);
			t->SetNormal (0.0, 0.0, -1.0);
			object->triangles.push_back (t);

			t = new CSGTriangle ();
			t->SetPoints (&object->points[i*(DivisionsInX+1) + j], 
					&object->points[(i+1)*(DivisionsInX+1) + j + 1], 
					&object->points[i*(DivisionsInX+1) + j + 1]);
			t->SetNormal (0.0, 0.0, -1.0);
			object->triangles.push_back (t);

			/* back face */
			t = new CSGTriangle ();
			t->SetPoints (&object->points[pointsPerSide + i*(DivisionsInX+1) + j], 
					&object->points[pointsPerSide + i*(DivisionsInX+1) + j + 1],
					&object->points[pointsPerSide + (i+1)*(DivisionsInX+1) + j]);
			t->SetNormal (0.0, 0.0, 1.0);
			object->triangles.push_back (t);

			t = new CSGTriangle ();
			t->SetPoints (&object->points[pointsPerSide + i*(DivisionsInX+1) + j + 1], 
					&object->points[pointsPerSide + (i+1)*(DivisionsInX+1) + j + 1], 
					&object->points[pointsPerSide + (i+1)*(DivisionsInX+1) + j]);
			t->SetNormal (0.0, 0.0, 1.0);
			object->triangles.push_back (t);
		}
	}

	/* left and right faces */
	for (int i = 0; i < DivisionsInY; i++)
	{
		for (int j = 0; j < DivisionsInX; j++)
		{
			CSGTriangle *t;

			/* left face */
			t = new CSGTriangle ();
			t->SetPoints (&object->points[pointsPerSide*2 + i*(DivisionsInX+1) + j], 
					&object->points[pointsPerSide*2 + (i+1)*(DivisionsInX+1) + j + 1], 
					&object->points[pointsPerSide*2 + (i+1)*(DivisionsInX+1) + j]);
			t->SetNormal (-1.0, 0.0, 0.0);
			object->triangles.push_back (t);

			t = new CSGTriangle ();
			t->SetPoints (&object->points[pointsPerSide*2 + i*(DivisionsInX+1) + j], 
					&object->points[pointsPerSide*2 + i*(DivisionsInX+1) + j + 1], 
					&object->points[pointsPerSide*2 + (i+1)*(DivisionsInX+1) + j + 1]);
			t->SetNormal (-1.0, 0.0, 0.0);
			object->triangles.push_back (t);

			/* right face */
			t = new CSGTriangle ();
			t->SetPoints (&object->points[pointsPerSide*3 + i*(DivisionsInX+1) + j], 
					&object->points[pointsPerSide*3 + i*(DivisionsInX+1) + j + 1],
					&object->points[pointsPerSide*3 + (i+1)*(DivisionsInX+1) + j + 1]);
			t->SetNormal (1.0, 0.0, 0.0);
			object->triangles.push_back (t);

			t = new CSGTriangle ();
			t->SetPoints (&object->points[pointsPerSide*3 + i*(DivisionsInX+1) + j], 
					&object->points[pointsPerSide*3 + (i+1)*(DivisionsInX+1) + j + 1], 
					&object->points[pointsPerSide*3 + (i+1)*(DivisionsInX+1) + j]);
			t->SetNormal (1.0, 0.0, 0.0);
			object->triangles.push_back (t);
		}
	}

	/* top and bottom faces */
	for (int i = 0; i < DivisionsInY; i++)
	{
		for (int j = 0; j < DivisionsInX; j++)
		{
			CSGTriangle *t;

			/* top face */
			t = new CSGTriangle ();
			t->SetPoints (&object->points[pointsPerSide*4 + i*(DivisionsInX+1) + j], 
					&object->points[pointsPerSide*4 + (i+1)*(DivisionsInX+1) + j], 
					&object->points[pointsPerSide*4 + (i+1)*(DivisionsInX+1) + j + 1]);
			t->SetNormal (0.0, -1.0, 0.0);
			object->triangles.push_back (t);

			t = new CSGTriangle ();
			t->SetPoints (&object->points[pointsPerSide*4 + i*(DivisionsInX+1) + j], 
					&object->points[pointsPerSide*4 + (i+1)*(DivisionsInX+1) + j + 1], 
					&object->points[pointsPerSide*4 + i*(DivisionsInX+1) + j + 1]);
			t->SetNormal (0.0, -1.0, 0.0);
			object->triangles.push_back (t);

			/* bottom face */
			t = new CSGTriangle ();
			t->SetPoints (&object->points[pointsPerSide*5 + i*(DivisionsInX+1) + j], 
					&object->points[pointsPerSide*5 + (i+1)*(DivisionsInX+1) + j + 1],
					&object->points[pointsPerSide*5 + (i+1)*(DivisionsInX+1) + j]);
			t->SetNormal (0.0, 1.0, 0.0);
			object->triangles.push_back (t);

			t = new CSGTriangle ();
			t->SetPoints (&object->points[pointsPerSide*5 + i*(DivisionsInX+1) + j], 
					&object->points[pointsPerSide*5 + (i+1)*(DivisionsInX+1) + j + 1], 
					&object->points[pointsPerSide*5 + i*(DivisionsInX+1) + j + 1]);
			t->SetNormal (0.0, 1.0, 0.0);
			object->triangles.push_back (t);
		}
	}

	object->SetBoundBox ( new BoundBox (this->GetX(), this->GetY(), this->GetZ(), 
						this->GetWidth(), this->GetHeight(), this->GetDepth()));
	return object;
}

CSGObject*
TetraCSGParallelopiped :: GetObject ()
{
	modified = false;
	delete object;
	object = CreateCSGObject (GetX(), GetY(), GetZ(), 
		GetWidth(), GetHeight(), GetDepth());
	return object;
}

bool
TetraCSGParallelopiped :: IsModified ()
{
	return modified;
}


// TetraCSGParallelopiped class member functions implementation ends
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// TetraCSGSphere class member functions implementation begins

TetraCSGSphere :: TetraCSGSphere (double x, double y, double z, double radius)
{
	SetX (x); SetY (y); SetZ (z);
	SetRadius (radius);
	modified = true;
	object = CreateCSGObject (x, y, z, radius);
}

TetraCSGSphere :: ~TetraCSGSphere ()
{
	if (object != NULL)
	{
		for (unsigned int i = 0; i < object->triangles.size (); i++)
			delete object->triangles[i];
		object->triangles.clear();
		delete object;
		object = NULL;
	}
}

CSGObject*
TetraCSGSphere :: CreateCSGObject (double x, double y, double z, double radius)
{
	/*CSGTriangle *p = new CSGTriangle;
	CSGPoint *pts = new CSGPoint;*/
	object = new CSGObject ();
	object->SetBoundBox (new BoundBox (x - radius, y - radius, z - radius, 
		radius*2, radius*2, radius*2));
	return object;
}

CSGObject*
TetraCSGSphere :: GetObject ()
{
	modified = false;
	delete object;
	object = CreateCSGObject (GetX(), GetY(), GetZ(), GetRadius());
	return object;
}

bool
TetraCSGSphere :: IsModified ()
{
	return modified;
}

// TetraCSGSphere class member functions implementation ends
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// TetraCSGFrustum class member functions implementation begins

TetraCSGFrustum :: TetraCSGFrustum (double x, double y, double z,
						double topside, double bottomside, double length)
{
	SetX (x); SetY (y); SetZ (z);
	SetTopSide (topside);
	SetBottomSide (bottomside);
	SetLength (length);
	modified = true;
	object = CreateCSGObject (x, y, z, topside, bottomside, length);
}

TetraCSGFrustum :: ~TetraCSGFrustum ()
{
	if (object != NULL)
	{
		for (unsigned int i = 0; i < object->triangles.size (); i++)
			delete object->triangles[i];
		object->triangles.clear();
		delete object;
		object = NULL;
	}
}

CSGObject*
TetraCSGFrustum :: CreateCSGObject (double x, double y, double z,
								  double ts, double bs, double len)
{
	CSGTriangle *p = new CSGTriangle[12];
	CSGPoint *pts = new CSGPoint[8];

	object = new CSGObject ();

	pts[0].SetCoord ( x-bottomside/2, y, z-bottomside/2);
	pts[1].SetCoord ( x-topside/2, y+length, z-topside/2);
	pts[2].SetCoord ( x+topside/2, y+length, z-topside/2);
	pts[3].SetCoord ( x+bottomside/2, y, z-bottomside/2);
	pts[4].SetCoord ( x-bottomside/2, y, z+bottomside/2);
	pts[5].SetCoord ( x-topside/2, y+length, z+topside/2);
	pts[6].SetCoord ( x+topside/2, y+length, z+topside/2);
	pts[7].SetCoord ( x+bottomside/2, y, z+bottomside/2);

	/* front */
	p[0].SetPoints ( &pts[0], &pts[1], &pts[2]);
	p[0].SetNormal (0.0, 0.0, -1.0);
	p[1].SetPoints ( &pts[0], &pts[2], &pts[3]);
	p[1].SetNormal (0.0, 0.0, -1.0);
	object->triangles.push_back (&p[0]);
	object->triangles.push_back (&p[1]);

	/* back */
	p[2].SetPoints ( &pts[4], &pts[5], &pts[6]);
	p[2].SetNormal (0.0, 0.0, 1.0);
	p[3].SetPoints ( &pts[4], &pts[6], &pts[7]);
	p[3].SetNormal (0.0, 0.0, 1.0);
	object->triangles.push_back (&p[2]);
	object->triangles.push_back (&p[3]);

	/* top */
	p[4].SetPoints ( &pts[4], &pts[0], &pts[3]);
	p[4].SetNormal (0.0, -1.0, 0.0);
	p[5].SetPoints ( &pts[4], &pts[3], &pts[7]);
	p[5].SetNormal (0.0, -1.0, 0.0);
	object->triangles.push_back (&p[4]);
	object->triangles.push_back (&p[5]);

	/* left */
	p[6].SetPoints ( &pts[4], &pts[5], &pts[1]);
	p[6].SetNormal (-1.0, 0.0, 0.0);
	p[7].SetPoints ( &pts[4], &pts[1], &pts[0]);
	p[7].SetNormal (-1.0, 0.0, 0.0);
	object->triangles.push_back (&p[6]);
	object->triangles.push_back (&p[7]);

	/* right */
	p[8].SetPoints (&pts[3], &pts[2], &pts[6]);
	p[8].SetNormal (1.0, 0.0, 0.0);
	p[9].SetPoints (&pts[3], &pts[6], &pts[7]);
	p[9].SetNormal (1.0, 0.0, 0.0);
	object->triangles.push_back (&p[8]);
	object->triangles.push_back (&p[9]);

	/* bottom */
	p[10].SetPoints (&pts[5], &pts[1], &pts[2]);
	p[10].SetNormal (0.0, 1.0, 0.0);
	p[11].SetPoints (&pts[5], &pts[2], &pts[6]);
	p[11].SetNormal (0.0, 1.0, 0.0);
	object->triangles.push_back (&p[10]);
	object->triangles.push_back (&p[11]);

	/* class member variable points*/
	/*object->points = pts;*/

	double maxside = (topside > bottomside)? topside : bottomside;
	object->SetBoundBox (new BoundBox (x-maxside/2, y, z-maxside/2, 
		maxside*2, length, maxside*2));

	return object;
}

CSGObject*
TetraCSGFrustum :: GetObject ()
{
	modified = false;
	delete object;
	object = CreateCSGObject (GetX(), GetY(), GetZ(), 
		GetTopSide(), GetBottomSide(), GetLength());
	return object;
}

bool
TetraCSGFrustum :: IsModified ()
{
	return modified;
}

// TetraCSGFrustum class member functions implementation ends
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// TetraCSGCylinder class member functions implementation begins

TetraCSGCylinder :: TetraCSGCylinder (double x, double y, double z,
						double radius, double length)
{
	SetX (x); SetY (y); SetZ (z);
	SetRadius (radius); 
	SetLength (length);
	modified = true;
	object = CreateCSGObject (x, y, z, radius, length);
}

TetraCSGCylinder :: ~TetraCSGCylinder ()
{
	if (object != NULL)
	{
		for (unsigned int i = 0; i < object->triangles.size (); i++)
			delete object->triangles[i];
		object->triangles.clear();
		delete object;
		object = NULL;
	}
}

CSGObject*
TetraCSGCylinder :: CreateCSGObject (double x, double y, double z,
						double radius, double length)
{
	object = new CSGObject ();
	object->SetBoundBox (new BoundBox (x - radius, y, z, 
					radius*2, length, radius*2));
	return object;
}

CSGObject*
TetraCSGCylinder :: GetObject ()
{
	modified = false;
	delete object;
	object = CreateCSGObject (GetX(), GetY(), GetZ(), 
		GetRadius(), GetLength());
	return object;
}

bool
TetraCSGCylinder :: IsModified ()
{
	return modified;
}

// TetraCSGCylinder class member functions implementation ends
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// TetraCSGUnion class member functions implementation begins

CSGObject*
TetraCSGUnion :: GetObject ()
{
	if (IsModified ())
	{
		if (object != NULL)
			delete object;

		object = new CSGObject ();

		CSGObject :: Clip (GetLeftChild()->GetObject (), 
					GetRightChild()->GetObject (), false, object);
		CSGObject :: Clip (GetRightChild ()->GetObject(), 
					GetLeftChild()->GetObject (), false, object);
	}

	return object;
}

bool
TetraCSGUnion :: IsModified ()
{
	return (GetLeftChild ()->IsModified () || GetRightChild ()->IsModified ());
}

// TetraCSGUnion class member functions implementation ends
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// TetraCSGIntersect class member functions implementation begins

CSGObject*
TetraCSGIntersect :: GetObject ()
{
	if (IsModified ())
	{
		if (object != NULL)
			delete object;

		object = new CSGObject ();

		CSGObject :: Clip (GetLeftChild()->GetObject (), 
					GetRightChild()->GetObject (), true, object);
		CSGObject :: Clip (GetRightChild ()->GetObject(), 
					GetLeftChild()->GetObject (), true, object);
	}

	return object;
}

bool
TetraCSGIntersect :: IsModified ()
{
	return (GetLeftChild ()->IsModified () || GetRightChild ()->IsModified ());
}

// TetraCSGIntersect class member functions implementation ends
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// TetraCSGDifference class member functions implementation begins

CSGObject*
TetraCSGDifference :: GetObject ()
{
	if (IsModified ())
	{
		if (object != NULL)
			delete object;

		object = new CSGObject ();

		CSGObject :: Clip (GetLeftChild()->GetObject (), 
					GetRightChild()->GetObject (), false, object);
		CSGObject :: Clip (GetRightChild ()->GetObject(), 
					GetLeftChild()->GetObject (), true, object);
	}

	return object;
}

bool
TetraCSGDifference :: IsModified ()
{
	return (GetLeftChild ()->IsModified () || GetRightChild ()->IsModified ());
}

// TetraCSGDifference class member functions implementation ends
/////////////////////////////////////////////////////////////////////
