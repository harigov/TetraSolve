#ifndef __TETRA_CSG_PRIMITIVE_H__
#define __TETRA_CSG_PRIMITIVE_H__

#include <wx/wx.h>
#include <wx/treectrl.h>
#include <vector>


/** This type defines the type a
 * CSG item represents.
 * \see CSGObject
 * \see TetraCSGItem
 */
enum TetraCSGItemType { 
	/** Represents nothing */
	NONE, 
	/** Represents a primitive of 
	 * parallelopiped */
	PRIM_PARALLELOPIPED = 1,
	/** Represents a primitive of
	 * sphere */
	PRIM_SPHERE = 2,
	/** Represents a primitive of
	 * frustum */
	PRIM_FRUSTUM = 4,
	/** Represents a primitive of
	 * cylinder */
	PRIM_CYLINDER = 8,
	/** Represents all types of primitives
	 * when the type of a CSG item is ORed
	 * with this value, the result represents
	 * whether the item is a primitive or not */
	PRIMITIVE = 15,
	/** Represents a regularized
	 * union boolean operation */
	BOOL_UNION = 16, 
	/** Represents a regularized
	 * intersect boolean operation */
	BOOL_INTERSECT = 32,
	/** Represents a regularized
	 * difference boolean operation */
	BOOL_DIFFERENCE = 64,
	/** Represents all types of
	 * boolean operations, similar
	 * to PRIMITIVE */
	BOOL_OPER = 112
};

/** \brief CSGPoint class represents a
 * 3D point in Cartesian coordinates.
 *
 * Apart from the coordinates of a 3D
 * point, it also stores a boolean flag
 * which represents whether this point
 * is within a bounding volume or not */
class CSGPoint
{
public:
	/** stores the coordinates of the point
	 * This is only public so that it can
	 * be used as a vertex vector while
	 * rendering CSG objects 
	 * \warning Don't operate on this,
	 * instead use SetCoord method
	 * */
	double vertex[3];
	/** Boolean flag, which represents whether
	 * this point is within in a 3D volume, while
	 * clipping triangles. This is used to improve
	 * performance */
	bool inside;

	/** Default constructor
	 * Initializes inside to false, and set's 
	 * coordinates of this point to origin (0,0,0) */
	CSGPoint () : inside(false) { SetCoord (0.0, 0.0, 0.0); };

	/** Constructor which provides interface to initialize
	 * coordinates. Initializes inside to false. */
	CSGPoint (double x, double y, double z)
	{
		inside = false;
		SetCoord (x, y, z);
	};
	
	/** Returns X coordinate of the point */
	double GetX() { return vertex[0]; };
	/** Returns Y coordinate of the point */
	double GetY() { return vertex[1]; };
	/** Returns Z coordinate of the point */
	double GetZ() { return vertex[2]; };

	/** Sets coordinates of the point */
	inline void SetCoord (double x, double y, double z)
	{
		vertex[0] = x; vertex[1] = y; vertex[2] = z;
	};
};

/** \brief
 * Represents a triangle in 3D space.
 *
 * It stores 3 points of a triangle, along
 * with normal vector. Normal vector is
 * used when rendering the triangle.
 * 3D primitives are formed from triangles */
class CSGTriangle
{
public:
	/** Normal vector of this triangle
	 * This is used while rendering the
	 * triangle. A normal vector is required
	 * by OpenGL for lighting calculations */
	double normal[3];
	/** Pointers to three points, representing
	 * three vertices of the triangle 
	 * \see CSGPoint */
	CSGPoint *points[3];

	/** Default constructor. Initializes all the
	 * points pointers to NULL. Normal vector is
	 * initialized to 0, 0, 0 */
	CSGTriangle ()
	{
		points[0] = points[1] = points[2] = NULL;
		SetNormal (0.0, 0.0, 0.0);
	};

	/** This function sets the three vertices of the
	 * triangle 
	 * \see CSGPoint */
	inline void SetPoints (CSGPoint *p1, CSGPoint *p2, CSGPoint *p3) 
	{
		points[0] = p1; points[1] = p2; points[2] = p3;
	};

	/** Sets normal vector of this triangle */
	inline void SetNormal (double normalx, double normaly, double normalz)
	{
		normal[0] = normalx; normal[1] = normaly; normal[2] = normalz;
	};

	/** This function checks whether a point represented
	 * by x, y, z coordinates passed as arguments to this
	 * function is inside this triangle or not.
	 * \return
	 * Returns true if the point is within this triangle.
	 * false other wise. */
	bool IsInside (double x, double y, double z)
	{
		CSGPoint p (x, y, z);
		if (SameSide (points[0], points[1], points[2], &p) &&
				SameSide (points[1], points[2], points[0], &p) &&
				SameSide (points[2], points[0], points[1], &p))
			return true;

		return false;
	};

private:
	/* performs the crossproduct of two vectors.
	 * Here two points are used as vectors */
	CSGPoint *CrossProduct (CSGPoint *a, CSGPoint *b)
	{
		CSGPoint *res = new CSGPoint ();
		res->SetCoord (a->GetY() * b->GetZ() - b->GetY() * a->GetZ(),
						a->GetZ() * b->GetX() - b->GetZ() * a->GetX(),
						a->GetX() * b->GetY() - b->GetX() * a->GetY());
		return res;
	};
	
	/* performs the dot product of two vectors 
	 * Here two points are used as vectors */
	double DotProduct (CSGPoint *a, CSGPoint *b)
	{
		return ((a->GetX() * b->GetX()) + 
				(a->GetY() * b->GetY()) + 
				(a->GetZ() * b->GetZ()));
	};

	/* Checks whether a point p is same side of the
	 * line formed by points a and b, as that of
	 * point c. Returns true if it is, false otherwise */
	bool SameSide (CSGPoint *a, CSGPoint *b, CSGPoint *c, CSGPoint *p)
	{
		CSGPoint p1 (b->GetX() - a->GetX(), 
					b->GetY() - a->GetY(), 
					b->GetZ() - a->GetZ());
		CSGPoint p2 (p->GetX() - a->GetX(),
					p->GetY() - a->GetY(),
					p->GetZ() - a->GetZ());
		CSGPoint p3 (c->GetX() - a->GetX(),
					c->GetY() - a->GetY(),
					c->GetZ() - a->GetZ());
		CSGPoint *cp1 = CrossProduct (&p1, &p2);
		CSGPoint *cp2 = CrossProduct (&p1, &p3);

		if (DotProduct (cp1, cp2) >= 0) 
				return true;

		return false;
	};
};

/** \brief BoundBox represents a bounding volume.
 *
 * BoundBox is a cubic volume, used to store the
 * bounds of a primitive. The bounds are used
 * for preliminarily checking, whether a point
 * lies with in bounds or not, and marking it */
class BoundBox
{
public:
	double x, y, z, w, h, d;

	/** constructor. volume is defined by
	 * a cube from (x, y, z) to (x+w, y+h, z+d)
	 * \param x
	 * x coordinate of the left side of the 
	 * bottom point of the backside of the cube 
	 * \param y
	 * y coordinate of the left side of the 
	 * bottom point of the backside of the cube
	 * \param z
	 * z coordinate of the left side of the 
	 * bottom point of the backside of the cube
	 * \param w
	 * width of the cube
	 * \param h
	 * height of the cube
	 * \param d
	 * depth of the cube
	 * */
	BoundBox (double x, double y, double z, 
		double w, double h, double d)
	{
		this->x = x; this->y = y; this->z = z;
		this->w = w; this->h = h; this->d = d;
	};
};

/////////////////////////////////////////////////////////////////////
// CSGObject class declaration begins

/**\brief CSGObject represents a primitive
 * when divided into triangles.
 *
 * CSGObject is an object formed from triangles.
 * Every primitive is actually a friendly interface
 * to a CSGObject
 */
class CSGObject
{
public:
	/** Bounding volume of this object */
	BoundBox *bounds;
	/** All the points forming the triangles
	 * are stored in this field, so that 
	 * when two triangles are sharing same 
	 * vertex, memory is not wasted. This is
	 * also useful in clipping, where in each
	 * point of an object is checked whether
	 * it is inside or outside of another 
	 * object bounding volume */
	std::vector<CSGPoint> points;
	/** Triangles are stored in a vector.
	 * \see CSGTriangle */
	std::vector<CSGTriangle*> triangles;

	/** This is the actual part wherein all
	 * the triangles are rendered */
	void Render ();

	/** Clips out part of an object, which is
	 * either inside or outside another object
	 * based on the "internal" flag.
	 * \param obj
	 * Object to be clipped
	 * \param clip
	 * Object to be clipped against
	 * \param internal
	 * If internal is true, the part of obj inside
	 * clip is returned, else outside part is returned.
	 * \param result
	 * Resulting triangles from the clipping are added
	 * into result object */
	static void Clip (CSGObject *obj, CSGObject *clip, 
						bool internal, CSGObject* result);

	/** Returns the bounding volume of this object.
	 * This function does not calculate the bounds,
	 * User of the CSGObject class has to set the
	 * bounds manually */
	BoundBox	*GetBoundBox ()
	{
		return bounds;
	};
	/** Sets the bounding volume
	 * \see BoundBox */
	void		SetBoundBox (BoundBox *bounds)
	{
		this->bounds = bounds;
	};

private:
	/** Returns the intersection point of an edge formed
	 * from points sp and ep, with the triangle tri.
	 * \param tri
	 * Triangle to which the edge intersection is to be checked.
	 * \param sp
	 * Start point of the edge
	 * \param ep
	 * End point of the edge
	 * \return point of intersection */
	static CSGPoint*	GetIntersectionPoint (CSGTriangle *tri, 
									CSGPoint *sp, CSGPoint *ep);
	/** Clips a triangle.
	 * Any part of the triangle tri, that is either inside
	 * or outside of the object clip, is formed into triangles
	 * and added into result. Whether which part the function
	 * will clip is determined by the parameter internal.
	 * \param tri
	 * Triangle which is to be clipped
	 * \param clip
	 * Object to which the triangle is the be clipped
	 * \param internal
	 * Flag based on which the triangle inside the object
	 * or outside the object is returned.
	 * \param result
	 * Result object to which the clipped triangle is added */
	static void			ClipTriangle (CSGTriangle *tri, CSGObject *clip, 
									bool internal, CSGObject *result);
	/** Checks whether a point is inside a 3D bounding
	 * volume or not
	 * \param pt
	 * Point which is to be checked
	 * \param bounds
	 * Bounding volume
	 * \return
	 * True, if the point is within the bounding volume, false otherwise.
	 */
	static bool			InsideBounds (CSGPoint *pt, BoundBox *bounds);
};

// CSGObject class declaration ends
/////////////////////////////////////////////////////////////////////

/** \brief Super class of all primitive classes
 * and boolean operation classes
 *
 * This is an abstract class, which provides
 * common interface to primitive and boolean
 * operation classes */
 class TetraCSGItem : public wxTreeItemData
{
public:
	/* Returns the CSGObject 
	 * \see CSGObject */
	virtual CSGObject	*GetObject () = 0;
	/* Returns the type of this item
	 * \see TetraCSGItemType */
	virtual TetraCSGItemType GetType () = 0;

	virtual bool IsModified () = 0;
	virtual wxString ToString () = 0;
};

/////////////////////////////////////////////////////////////////////
// class TetraCSGPrimitive declaration begins

/** \brief Abstract base class of all primitives
 *
 * This class is a subclass of TetraCSGItem.
 */
class TetraCSGPrimitive : public TetraCSGItem
{
protected:
	TetraCSGPrimitive ();
public:

	/** Getter function for X coordinate */
	double	GetX (void);
	/** Setter function for X coordinate */
	void	SetX (double x);

	/** Getter function for Y coordinate */
	double	GetY (void);
	/** Setter function for Y coordinate */
	void	SetY (double y);

	/** Getter function for Z coordinate */
	double	GetZ (void);
	/** Setter function for Z coordinate */
	void	SetZ (double z);

	/** Returns the type of this primitive */
	TetraCSGItemType	GetType () = 0;
	/** Returns CSGObject */
	virtual CSGObject*	GetObject (void) = 0;

	virtual bool IsModified () = 0;

	virtual wxString ToString () = 0;

protected:
	double x, y, z;
};

// class TetraCSGPrimitive declaration ends
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// class TetraCSGParallelopiped declaration begins

/** \brief Parallelopiped primitive
 */
class TetraCSGParallelopiped : public TetraCSGPrimitive
{
public:
	/** Constructor */
	TetraCSGParallelopiped (double x, double y, double z, 
							double w, double h, double d);
	/** Destructor */
	~TetraCSGParallelopiped ();
	
	/** Returns the CSGObject */
	virtual CSGObject*	GetObject (void);
	/** Creates a CSGObject for this primitive */
	CSGObject*			CreateCSGObject (double x, double y, double z, 
							double w, double h, double d);
	/** Returns PRIM_PARALLELOPIPED */
	TetraCSGItemType	GetType () { return PRIM_PARALLELOPIPED; };

	/** Returns the width of the parallelopiped */
	inline double	GetWidth (void) 	{ return width; };
	/** Sets the width of the parallelopiped */
	inline void 	SetWidth (double w)	{ modified = true; width = w; };

	/** Returns the height of the parallelopiped */
	inline double	GetHeight (void)	{ return height; };
	/** Sets the height of the parallelopiped */
	inline void		SetHeight (double h){ modified = true; height = h; };

	/** Returns the depth of the parallelopiped */
	inline double	GetDepth (void)		{ return depth; };
	/** Sets the depts of the parallelopiped */
	inline void		SetDepth (double d)	{ modified = true; depth = d; };

	virtual bool IsModified ();

	virtual wxString ToString () { return wxT("Parallelopiped"); };

private:
	double width, height, depth;
	CSGObject *object;
	bool modified;
};

// class TetraCSGParallelopiped declaration ends
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// class TetraCSGSphere declaration begins

/** \brief Sphere primitive */
class TetraCSGSphere : public TetraCSGPrimitive
{
public:
	/** constructor */
	TetraCSGSphere (double x, double y, double z, double radius);
	/** destructor */
	~TetraCSGSphere ();

	/** Creates a CSGObject for sphere primitive */
	CSGObject *CreateCSGObject (double x, double y, double z, double radius);

	virtual CSGObject	*GetObject (void);

	TetraCSGItemType	GetType () { return PRIM_SPHERE; };

	/** Getter function for radius */
	inline double	GetRadius (void) { return radius; };
	/** Setter function for radius */
	inline void		SetRadius (double r) { modified = true; radius = r; };

	virtual bool IsModified ();

	virtual wxString ToString () { return wxT("Sphere"); };

private:
	double radius;
	CSGObject *object;
	bool modified;
};

// class TetraCSGSphere declaration ends
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// class TetraCSGFrustum declaration begins

/** \brief Frustum primitive */
class TetraCSGFrustum : public TetraCSGPrimitive
{
public:
	/** constructor */
	TetraCSGFrustum (double x, double y, double z,
						double topside, double bottomside, double length);
	/** destructor */
	~TetraCSGFrustum ();

	/** Creates a CSGObject for Frustum */
	CSGObject *CreateCSGObject (double x, double y, double z,
						double ts, double bs, double len);

	virtual CSGObject	*GetObject (void);

	TetraCSGItemType	GetType () { return PRIM_FRUSTUM; };

	/** Getter function for topside property */
	inline double	GetTopSide (void) 	{ return topside; };
	/** Setter function for topside property */
	inline void		SetTopSide (double ts) { modified = true; topside = ts; };

	/** Getter function for bottomside property */
	inline double	GetBottomSide (void) 	{ return bottomside; };
	/** Setter function for bottomside property */
	inline void		SetBottomSide (double bs){ modified = true; bottomside = bs; };

	/** Getter function for length */
	inline double	GetLength (void) 		{ return length; };
	/** Setter function for length */
	inline void		SetLength (double len) 	{ modified = true; length = len; };

	virtual bool IsModified ();

	virtual wxString ToString () { return wxT("Frustum"); };

private:
	double topside, bottomside, length;
	CSGObject *object;
	bool modified;
};

// class TetraCSGFrustum declaration ends
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// class TetraCSGCylinder declaration begins

/** Cylinder primitive */
class TetraCSGCylinder : public TetraCSGPrimitive
{
public:
	/** constructor */
	TetraCSGCylinder (double x, double y, double z,
						double radius, double length);
	/** destructor */
	~TetraCSGCylinder ();

	/** Creates a CSGObject for cylinder */
	CSGObject* CreateCSGObject (double x, double y, double z,
						double radius, double length);

	virtual CSGObject	*GetObject (void);

	TetraCSGItemType	GetType () { return PRIM_CYLINDER; };

	/** Getter function for radius property */
	inline double	GetRadius (void) 		{ return radius; };
	/** Setter function for radius property */
	inline void		SetRadius (double r) 	{ modified = true; radius = r; };

	/** Getter function for length property */
	inline double	GetLength (void) 		{ return length; };
	/** Setter function for length property */
	inline void		SetLength (double len) 	{ modified = true; length = len; };

	virtual bool IsModified ();

	virtual wxString ToString () { return wxT("Cylinder"); };

private:
	double radius, length;
	CSGObject *object;
	bool modified;
};

// class TetraCSGCylinder declaration ends
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// TetraCSGBoolOper class declaration begins

/** Super class of all boolean operation
 * classes. It implements TetraCSGItem. */
class TetraCSGBoolOper : public TetraCSGItem
{
public:
	virtual CSGObject*	GetObject () = 0;
	virtual TetraCSGItemType	GetType () = 0;

	/** Getter function for left child */
	TetraCSGItem *GetLeftChild (void)
	{
		return left;
	};

	/** Setter function for left child */
	void	SetLeftChild (TetraCSGItem* left)
	{
		this->left = left;
	};

	/** Getter function for right child */
	TetraCSGItem* GetRightChild (void)
	{
		return right;
	};

	/** Setter function for right child */
	void SetRightChild (TetraCSGItem* right)
	{
		this->right = right;
	};

	virtual bool IsModified () = 0;

	virtual wxString ToString () = 0;

protected:
	TetraCSGItem *left, *right;
};

// TetraCSGBoolOper class declaration ends
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// TetraCSGUnion class declaration begins

/** Union boolean operation class */
class TetraCSGUnion : public TetraCSGBoolOper
{
	CSGObject *object;
	BoundBox* bounds;
public:
	TetraCSGUnion () { object = NULL; bounds = NULL; };

	virtual CSGObject*	GetObject ();

	TetraCSGItemType	GetType () { return BOOL_UNION; };

	virtual bool IsModified ();

	virtual wxString ToString () { return wxT("Union"); };
};

// TetraCSGUnion class declaration ends
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// TetraCSGIntersect class declaration begins

/** Intersect boolean operation class */
class TetraCSGIntersect : public TetraCSGBoolOper
{
	CSGObject *object;
	BoundBox *bounds;
public:
	TetraCSGIntersect () { object = NULL; bounds = NULL; };

	virtual CSGObject*	GetObject ();

	TetraCSGItemType	GetType () { return BOOL_INTERSECT; };

	virtual bool IsModified ();

	virtual wxString ToString () { return wxT("Intersect"); };
};

// TetraCSGIntersect class declaration ends
/////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////
// TetraCSGDifference class declaration begins

/** Difference boolean operation class */
class TetraCSGDifference : public TetraCSGBoolOper
{
	CSGObject *object;
	BoundBox *bounds;
public:
	TetraCSGDifference () { object = NULL; bounds = NULL; };

	virtual CSGObject*	GetObject ();

	TetraCSGItemType	GetType () { return BOOL_DIFFERENCE; };

	virtual bool IsModified ();

	virtual wxString ToString () { return wxT("Difference"); };
};

// TetraCSGDifference class declaration ends
/////////////////////////////////////////////////////////////////////

#endif /* __TETRA_CSG_PRIMITIVE_H__ */

