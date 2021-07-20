#ifndef __TETRAHEDRON_HEADER__
#define __TETRAHEDRON_HEADER__

#include <iostream>
#include <fstream>
#include <vector>
#include <cstdlib>
#include <cstdarg>
#include <cassert>
#include <cmath>

#include "Matrix.h"

template<class T>
class Tetrahedron
{
public:
	int nDOF, nMPC, nDIM, nElements, nNodes, nDisp, nDOFPerNode,
		nMaterials, nNodesPerElement, nProperties;

	struct Element
	{
		vector<int> Nodes;
		int Material;
		T Temp;
		Element():Nodes(4), Material(0) { Temp=T();};
	};

	struct MPConstraint
	{
		T b1, b2, b3;
		int i, j;
		MPConstraint():i(0),j(0){ b1=b2=b3=T(); };
	};

	struct Displacement
	{
		int index;
		T value;
		Displacement():index(0) { value = T(); };
	};
	
	struct Node
	{
		T x, y, z;
		Node() { x = y = z = T(); };
	};
	
	struct Material
	{
		T E, Alpha, Nu;
		Material() { E = Alpha = Nu = T(); };
	};

	typedef Element	ELEMENT;
	typedef MPConstraint MPC;
	typedef Displacement DISPLACEMENT;
	typedef Node NODE;
	typedef Material MATERIAL;

	vector<NODE> Nodes;
	vector<ELEMENT> Elements;
	vector<MATERIAL> Materials;
	vector<T> Forces;	// Forces include reaction forces
	vector<DISPLACEMENT> Loads; // Loads is initial forces
	vector<DISPLACEMENT> Displacements;
	vector<MPC> MPCons;
	vector<T> Reactions;
	
	Matrix<T> *Stresses;
	
	bool isSolved;

	Tetrahedron (int nen, int ndim, int ndn, int npr);
	~Tetrahedron ();

	/* adds a node to the Nodes list at the specified index */
	bool AddNode (int index, T x, T y, T z);
	/* adds an element to the Elements list at the specified index */
	bool AddElement (int index, int Material, T temp, int, int, int, int);
	/* adds a material to the materials list at the specified index */
	bool AddMaterial (int index, T E, T Nu, T Alpha);
	/* adds a force at a dof, which is in the range of 0 and nDOF */
	bool AddForce (int dof, T force);
	/* adds a displacement at a dof */
	bool AddDisplacement (int dof, T disp);
	/* adds a multipoint constraint */
	bool AddMPCons (T b1, int i, T b2, int j, T b3);
	
	/* removes a node at the specified index, from the list */
	bool RemoveNode (int index);
	/* removes an element at the specified index from the elements list */
	bool RemoveElement (int index);
	/* removes a material at index from the materials list */
	bool RemoveMaterial (int index);
	/* removes force at the specified dof */
	bool RemoveForce (int dof);
	/* removes a displacement at the specified dof */
	bool RemoveDisplacement (int dof);
	/* removes a multipoint constraint from the specified index */
	bool RemoveMPCons (int index);

	/* Solve the tetrahedron problem */
	void Solve ();

	/* Write the end results to file */
	bool WriteResultsToFile (const char* fname);
private:
	T det;

	/* calculates bandwidth of the matrix*/
	int CalculateBandwidth ();
	/* Get stress-strain relation matrix */
	Matrix<T>* GetDMatrix (int element);
	/* Get strain-displacement relation matrix */
	Matrix<T>* GetBMatrix (int element);
	/* Get the temperature rise of an element as a vector */
	Matrix<T>* GetTempLoadVector (int element, Matrix<T> *db);
	/* Get element stiffness matrix */
	Matrix<T>* GetElementStiffnessMatrix (int element, Matrix<T> *b, Matrix<T> *d);
	/* Get global stiffness matrix, assembled from element stiffness matrices */
	Matrix<T>* GetGlobalStiffnessMatrix ();
	/* Get element stress vector */
	Matrix<T>* GetElementStressVector (int element);
	/* Solve the stiffness matrix to determine displacements */
	void BandSolver(Matrix<T> *stiffness);
};

//////////////////////////////////////////////////////////////////////////////
//    METHOD IMPLEMENTATION BEGINS

template<class T>
Tetrahedron<T>::Tetrahedron (int nen, int ndim, int ndn, int npr)
				  :Nodes(), Elements(), Materials(), Forces(),
				  Displacements(), MPCons(), Reactions(), Stresses()
{
	/* initialize all of them to zero */
	nNodes = nElements = nDisp = nMaterials = nMPC = 0;
	
	nNodesPerElement = nen;
	nDIM = ndim;
	nDOFPerNode = ndn;
	nProperties = npr;
	
	Stresses = NULL;
	
	/* Used to check if the problem is solved or not */
	isSolved = false;

	det = 0;
}

template<class T>
Tetrahedron<T>::~Tetrahedron()
{
	if (Stresses != NULL)
		delete Stresses;
	Stresses = NULL;
}

template<class T>
bool Tetrahedron<T>::AddNode (int index, T x, T y, T z)
{
	DBG_MSG("AddNode Entered.");
	
	NODE n;
	
	n.x = x;
	n.y = y;
	n.z = z;
	
	/* if the index is greater than the number of nodes,
	then resize the Nodes vector, and then insert */
	if ( index >= nNodes)
	{
		nNodes = index;
		Nodes.resize (index+1);
	}
	
	Nodes[index] = n;
	
	/* nDOF is dependent on nNodes, therefore as the number of
	nodes changes, we have to change the value of nDOF. As Forces
	vector must be of nDOF size, we must resize it to new nDOF size */
	nDOF = nNodes * nDOFPerNode;
	Forces.resize (nDOF);
	
	DBG_MSG("AddNode Leaving.");
	return true;
}

template<class T>
bool Tetrahedron<T>::AddElement (int index, int Material, T Temp, 
							int n1, int n2, int n3, int n4)
{
	DBG_MSG("AddElement Entered.");
	
	ELEMENT e;

	e.Material = Material;
	e.Temp = Temp;
	e.Nodes[0] = n1;
	e.Nodes[1] = n2;
	e.Nodes[2] = n3;
	e.Nodes[3] = n4;
	
	/* same as for nodes, check if index is more than
	number of elements. If yes, then resize the Elements
	vector */
	if (index >= nElements) 
	{
		nElements = index;
		Elements.resize (index+1);
	}
	
	Elements[index] = e;
	
	DBG_MSG("AddElement Leaving.");	
	return true;
}

template<class T>
bool Tetrahedron<T>::AddMaterial (int index, T E, T Nu, T Alpha)
{
	DBG_MSG("AddMaterial Entered.");
	
	MATERIAL m;
	
	m.E = E;
	m.Alpha = Alpha;
	m.Nu = Nu;
	
	/* similar to elements and nodes vector */
	if ( index >= nMaterials)
	{
		nMaterials = index;
		Materials.resize (index+1);
	}
	
	Materials[index] = m;
	
	DBG_MSG("AddMaterial Leaving.");
	return true;
}

template<class T>
bool Tetrahedron<T>::AddForce (int dof, T force)
{
	DBG_MSG("AddForce Entered.");
	
	/* Because the elements in forces vector is initialized
	to zero, we just need to modify the Force at a particular
	dof without worrying about the size */
	if (dof >= 0 && dof < nDOF)
	{
		Forces[dof] = force;
		DISPLACEMENT d;
		d.index = dof;
		d.value = force;
		Loads.push_back (d);
	}
	else 
		return false;
	
	DBG_MSG("AddForce Leaving.");
	return true;
}

template<class T>
bool Tetrahedron<T>::AddDisplacement (int dof, T disp)
{
	DBG_MSG("AddDisplacement Entered.");
	
	/* check if dof specified is with in range
	if yes, then simply push the new Displacement
	into Displacements vector */
	if (dof >= 0 && dof <= nDOF)
	{
		DISPLACEMENT d;
		d.index = dof;
		d.value = disp;

		Displacements.push_back (d);
		/* increment the nDisp counter */
		nDisp++;
		
		DBG_MSG("AddDisplacement Leaving.");
		return true;
	}
	return false;
}

template<class T>
bool Tetrahedron<T>::AddMPCons (T b1, int i, T b2, int j, T b3)
{
	DBG_MSG("AddMPCons Entered.");
	
	/* similar to Displacement */
	if (i <= nDOF && j <= nDOF)
	{
		MPC mpc;
		mpc.b1 = b1;
		mpc.b2 = b2;
		mpc.b3 = b3;
		mpc.i = i;
		mpc.j = j;
		MPCons.push_back (mpc);
		nMPC++;
		
		DBG_MSG("AddMPCons Leaving.");
		return true;
	}

	return false;
}

template<class T>
bool Tetrahedron<T>::RemoveNode (int index)
{
	if (index > 0 && index <= nNodes)
	{
		/* check if this node is part of any element */
		for (int i=1; i <= nElements; i++)
		{
			for (int j = 0; j < nNodesPerElement; j++)
			{
				if (Elements[i].Nodes[j] == index)
					return false;
			}
		}
		
		/* check if dof of this node has any force acting */
		for (int i = 0; i < nDOFPerNode; i++)
		{
			if (Forces[index*nDOFPerNode + i] > 0.0)
				return false;
		}
		
		/* check if a displacement boundary coundition has been
		imposed on this node */
		int temp;
		for (int i = 0; i < nDisp; i++)
		{
			temp = (index-1) * nDOFPerNode;
			if (Displacements[i].index >= temp && 
				Displacements[i].index < (temp+nDOFPerNode))
				return false;
		}
		
		/* no force, element, disp is dependent on this
		node, so we can safely delete this */
		Nodes.erase (Nodes.begin() + index);
		nNodes--;
		
		/* make elements point to the correct node numbers
		after a node is removed */
		for (int i = 1; i <= nElements; i++)
		{
			for (int j = 0; j < nNodesPerElement; j++)
				if (Elements[i].Nodes[j] > index)
					Elements[i].Nodes[j]--;
		}
		
		/* when a node is deleted, the next node will
		be in the index*nDOFPerNode position */
		for (int i = 0; i < nDOFPerNode; i++)
			Forces.erase (Forces.begin() + ((index-1) * nDOFPerNode));
		nDOF -= nDOFPerNode;
		
		/* any displacement dof's must be made to point to
		right node */
		for (int i = 0; i < nDisp; i++)
			if (Displacements[i].index > index)
				Displacements[i].index -= nDOFPerNode;

		return true;
	}
	return false;
}

template<class T>
bool Tetrahedron<T>::RemoveElement (int index)
{
	/* check if index is in range */
	if (index > 0 && index <= nElements)
	{
		/* simply erase the element. there are no
		dependencies to worry about */
		Elements.erase (Elements.begin() + index);
		nElements--;
		return true;
	}
	return false;
}

template<class T>
bool Tetrahedron<T>::RemoveMaterial (int index)
{
	if (index > 0 && index <= nMaterials)
	{
		/* check if any element uses this material */
		for (int i = 1; i < nElements; i++)
		{
			if (Elements[i].Material == index)
				return false;
		}
		/* no element uses this material, so
		we can safely delete this material */
		Materials.erase (Materials.begin() + index);
		nMaterials--;
		
		/* when a material is deleted, the next materials
		index will be decreased by 1. Therefore, make the
		Elements material index to point to the right material */
		for (int i = 1; i < nElements; i++)
		{
			if (Elements[i].Material > index)
				Elements[i].Material--;
		}
		return true;
	}
	return false;
}

template<class T>
bool Tetrahedron<T>::RemoveForce (int dof)
{
	/* reinitialize the force to default force */
	if (dof > 0 && dof < nDOF)
	{
		Forces[dof] = T();
		return true;
	}
	return false;
}

template<class T>
bool Tetrahedron<T>::RemoveDisplacement (int index)
{
	if (index >= 0 && index < nDisp)
	{
		/* remove the displacement without worrying about
		any dependencies */
		Displacements.erase (Displacements.begin() + index);
		nDisp--;
		return true;
	}
	return false;
}

template<class T>
bool Tetrahedron<T>::RemoveMPCons (int index)
{
	if (index > 0 && index < nMPC)
	{
		/* remove the multipoint constraint */
		MPCons.erase (MPCons.begin() + index);
		nMPC--;
		return true;
	}
	return false;
}

template<class T>
int Tetrahedron<T>::CalculateBandwidth ()
{
	DBG_MSG("Entered CalculateBandwidth.");

	int bandwidth = 0, nmax, nmin, n;
	/* find the maximum difference between dof indices
	in a particular element and use it as bandwidth */
	for (int i = 1; i <= nElements; i++) 
	{
		nmin = Elements[i].Nodes[0];
		nmax = nmin;
		for (int j = 0; j < nNodesPerElement; j++) 
		{
			n = Elements[i].Nodes[j];
			if (nmin > n)
				nmin = n;
			if (nmax < n)
				nmax = n;
		}  
		n = nDOFPerNode * (nmax - nmin + 1);
		if (bandwidth < n)
			bandwidth = n;
	}
	/* modify bandwidth to take care of Multipoint constraints */
	for (int i = 0; i < nMPC; i++) 
	{
		n = abs(MPCons[i].i - MPCons[i].j) + 1;
		if (bandwidth < n)
			bandwidth = n;
	}
	
	DBG_MSG("Leaving CalculateBandwidth.");
	return bandwidth;
}

template<class T>
Matrix<T>* Tetrahedron<T>::GetDMatrix (int element)
{
	DBG_MSG("Entered GetDMatrix.");

	/* This function is simply assigning the elements of
	D matrix with correct values computed using stress-strain
	relation matrix */

	Matrix<T> *d;
	try {
		 d = new Matrix<T>(6,6);
	} catch (bad_alloc) {
		ERR_MSG("Memory allocation error");
		exit (1);
	}

	T E, Nu, Alpha, c1, c2, c3, c4;
	E = Materials[Elements[element].Material].E;
	Nu = Materials[Elements[element].Material].Nu;
	Alpha = Materials[Elements[element].Material].Alpha;

	c4 = E/( (1+Nu) * (1-2*Nu) );
	c1 = c4 * (1-Nu);
	c2 = c4 * Nu;
	c3 = 0.5 * E/(1+Nu);

	(*d)(0,0) = c1;
	(*d)(0,1) = c2;
	(*d)(0,2) = c2;
	(*d)(1,0) = c2;
	(*d)(1,1) = c1;
	(*d)(1,2) = c2;
	(*d)(2,0) = c2;
	(*d)(2,1) = c2;
	(*d)(2,2) = c1;
	(*d)(3,3) = c3;
	(*d)(4,4) = c3;
	(*d)(5,5) = c3;

	DBG_MSG("Leaving GetDMatrix.");
	return d;
}

template<class T>
Matrix<T>* Tetrahedron<T>::GetBMatrix (int element)
{
	DBG_MSG("Entered GetBMatrix.");

	/* This function is simply assigning the elements of
	B matrix with correct values computed using strain-displacement
	relation matrix */

	Matrix<T> *b = new Matrix<T>(6,12);
	int n1, n2, n3, n4;
	T x14, x24, x34, y14, y24, y34, z14, z24, z34, 
		 a11, a21, a31, a12, a22, a32, a13, a23, a33;
	
	n1 = Elements[element].Nodes[0];
	n2 = Elements[element].Nodes[1];
	n3 = Elements[element].Nodes[2];
	n4 = Elements[element].Nodes[3];

	x14 = Nodes[n1].x - Nodes[n4].x;
	x24 = Nodes[n2].x - Nodes[n4].x;
	x34 = Nodes[n3].x - Nodes[n4].x;
	
	y14 = Nodes[n1].y - Nodes[n4].y;
	y24 = Nodes[n2].y - Nodes[n4].y;
	y34 = Nodes[n3].y - Nodes[n4].y;
	
	z14 = Nodes[n1].z - Nodes[n4].z;
	z24 = Nodes[n2].z - Nodes[n4].z;
	z34 = Nodes[n3].z - Nodes[n4].z;
	
	det = (x14 * (y24 * z34 - z24 * y34)) + 
		(y14 * (z24 * x34 - x24 * z34)) + 
		(z14 * (x24 * y34 - y24 * x34));

	a11 = (y24 * z34 - z24 * y34) / det;
	a21 = (z24 * x34 - x24 * z34) / det;
	a31 = (x24 * y34 - y24 * x34) / det;
	a12 = (y34 * z14 - z34 * y14) / det;
	a22 = (z34 * x14 - x34 * z14) / det;
	a32 = (x34 * y14 - y34 * x14) / det;
	a13 = (y14 * z24 - z14 * y24) / det;
	a23 = (z14 * x24 - x14 * z24) / det;
	a33 = (x14 * y24 - y14 * x24) / det;

	(*b)(0,0) = a11;
	(*b)(0,3) = a12;
	(*b)(0,6) = a13;
	(*b)(0,9) = -a11 - a12 - a13;
	(*b)(1,1) = a21;
	(*b)(1,4) = a22;
	(*b)(1,7) = a23;
	(*b)(1,10) = -a21 - a22 - a23;
	(*b)(2,2) = a31;
	(*b)(2,5) = a32;
	(*b)(2,8) = a33;
	(*b)(2,11) = -a31 - a32 - a33;
	(*b)(3,1) = a31;
	(*b)(3,2) = a21;
	(*b)(3,4) = a32;
	(*b)(3,5) = a22;
	(*b)(3,7) = a33;
	(*b)(3,8) = a23;
	(*b)(3,10) = (*b)(2,11);
	(*b)(3,11) = (*b)(1,10);
	(*b)(4,0) = a31;
	(*b)(4,2) = a11;
	(*b)(4,3) = a32;
	(*b)(4,5) = a12;
	(*b)(4,6) = a33;
	(*b)(4,8) = a13;
	(*b)(4,9) = (*b)(2,11);
	(*b)(4,11) = (*b)(0,9);
	(*b)(5,0) = a21;
	(*b)(5,1) = a11;
	(*b)(5,3) = a22;
	(*b)(5,4) = a12;
	(*b)(5,6) = a23;
	(*b)(5,7) = a13;
	(*b)(5,9) = (*b)(1,10);
	(*b)(5,10) = (*b)(0,9);

	DBG_MSG("Leaving GetBMatrix.");
	return b;
}

template<class T>
Matrix<T>* Tetrahedron<T>::GetTempLoadVector (int element, Matrix<T> *db)
{
	DBG_MSG("Entered GetTempLoadVector.");

	Matrix<T> *tl;
	try {
		tl = new Matrix<T>(1,12);
	} catch (bad_alloc) {
		ERR_MSG("Memory allocation error.");
		exit (1);
	}
	
	T c , Alpha = Materials[Elements[element].Material].Alpha;
	c = Alpha * Elements[element].Temp;
	for (int i = 0; i < 12; i++) 
	{
		(*tl)(0,i) = c * fabs(det) * 
				((*db)(0,i) + (*db)(1,i) + (*db)(2,i)) / 6;
	}

	DBG_MSG("Leaving GetTempLoadVector.");
	return tl;
}

template<class T>
Matrix<T>* Tetrahedron<T>::GetElementStiffnessMatrix 
					(int index, Matrix<T> *b, Matrix<T> *db)
{
	DBG_MSG("Entered GetElementStiffnessMatrix.");

	Matrix<T> *stiffness;
	try {
		stiffness = new Matrix<T> (12,12);
	} catch (bad_alloc) {
		ERR_MSG("Memory allocation error.");
		exit (1);
	}
	
	/* This is just calculating B'DB = Btranspose * DB */
	for (int i = 0; i < 12; i++) 
	{
		for (int j = 0; j < 12; j++) 
		{
			(*stiffness)(i,j) = 0.;
			for (int k = 0; k < 6; k++) 
			{
				(*stiffness)(i,j) = (*stiffness)(i,j) + 
							fabs(det) * (*b)(k,i) * (*db)(k,j) / 6;
			}
		}
	}

	DBG_MSG("Leaving GetElementStiffnessMatrix.");
	return stiffness;
}

template<class T>
Matrix<T>* Tetrahedron<T>::GetGlobalStiffnessMatrix ()
{
	DBG_MSG("Entered GetGlobalStiffnessMatrix.");

	Matrix<T> *d, *b, *db, *se, *result;
	Matrix<T> *tl;
	int bandwidth, i, j;

	bandwidth = CalculateBandwidth ();

	try {
		result = new Matrix<T> (nDOF, bandwidth);
	} catch (bad_alloc) {
		ERR_MSG("Memory allocation error.");
		exit (1);
	}

	/* calculating the element stiffness matrix of each element
	and then assembling them into global stiffness matrix */
	for (int n = 1; n <= nElements; n++)
	{
		d = GetDMatrix (n);
		b = GetBMatrix (n);
		db = Multiply(d,b);
		se = GetElementStiffnessMatrix (n, b, db);
		tl = GetTempLoadVector (n, db);
		
		DBG_MSG("Reached GetGlobalStiffnessMatrix.");
		
		for (int ii = 0; ii < nNodesPerElement; ii++)
		{
			int nrt = nDOFPerNode * (Elements[n].Nodes[ii]-1);
			for (int it = 0; it < nDOFPerNode; it++) 
			{
				int nr = nrt + it;
				i = nDOFPerNode * ii + it;
				for (int jj = 0; jj < nNodesPerElement; jj++) 
				{
					int nct = nDOFPerNode * (Elements[n].Nodes[jj]-1);
					for (int jt = 0; jt < nDOFPerNode; jt++) 
					{
						j = nDOFPerNode * jj + jt;
						int nc = nct + jt - nr;
						if (nc >= 0)
							(*result)(nr,nc) = (*result)(nr,nc) 
											+ (*se)(i,j);
					}
				}
				Forces[nr] = Forces[nr] + (*tl)(0,i);
			}
		}

		delete d;
		delete b;
		delete db;
		delete se;
		delete tl;
	}

	DBG_MSG("Leaving GetGlobalStiffnessMatrix.");
	return result;
}


template<class T>
Matrix<T>* Tetrahedron<T>::GetElementStressVector (int element)
{
	Matrix<T> *stress,*d, *b, *db;
	T ai1, ai21, ai22, ai2, ai31, ai32, ai3, c1, c2, c3, th, th2;
	T PI = 3.14159265;
	int i,j,in,ii;
	T c,q[12];

	try
	{
		stress = new Matrix<T>(1,9);
	}
	catch (bad_alloc)
	{
		ERR_MSG("Memory allocation error.");
		exit (1);
	};
	
	d = GetDMatrix (element);
	b = GetBMatrix (element);
	
	db = Multiply (d,b);
	
	/* --- stress evaluation (element nodal displacements stored in q() --- */
	for (i = 0; i < 4; i++) {
		in = 3 * (Elements[element].Nodes[i] - 1);
		ii = 3 * i;
		for (j = 0; j < 3; j++) 
		{
			q[ii + j] = Forces[in + j];
		}
	}
		
	c1 = Materials[Elements[element].Material].Alpha * Elements[element-1].Temp;
	
	for (i = 0; i < 6; i++) 
	{
		c = T();
		for (j = 0; j < 12; j++)
		{
			c = c + (*db)(i,j) * q[j];
		}
		(*stress)(0,i) = c - c1 * ((*d)(i,0) + (*d)(i,1) + (*d)(i,2));
	}
	
	/* calculating principle stresses from the normal and shear stresses */
	ai1 = (*stress)(0,0) + (*stress)(0,1) + (*stress)(0,2);
	
	ai21 = (*stress)(0,0) * (*stress)(0,1) + (*stress)(0,1) * (*stress)(0,2) + 
			(*stress)(0,2) * (*stress)(0,0);
	ai22 = (*stress)(0,3) * (*stress)(0,3) + (*stress)(0,4) * (*stress)(0,4) + 
			(*stress)(0,5) * (*stress)(0,5);
	ai2 = ai21 - ai22;
	
	ai31 = (*stress)(0,0) * (*stress)(0,1) * (*stress)(0,2) + 
			2 * (*stress)(0,3) * (*stress)(0,4) * (*stress)(0,5);
	ai32 = (*stress)(0,0) * (*stress)(0,3) * (*stress)(0,3) + 
			(*stress)(0,1) * (*stress)(0,4) * (*stress)(0,4) + 
			(*stress)(0,2) * (*stress)(0,5) * (*stress)(0,5);
	ai3 = ai31 - ai32;

	c1 = ai2 - ai1 * ai1 / 3;
	c2 = -2 * (ai1*ai1*ai1) / 27 + ai1 * ai2 / 3 - ai3;
	c3 = 2 * sqrt((double) -c1 / 3);

	th = -3 * c2 / (c1 * c3);
	th2 = sqrt((double) fabs(1 - th * th));
	if (th == 0)
		th = PI / 2;
	if (th > 0)
		th = atan((double) th2 / th);
	if (th < 0)
		th = PI - atan((double) th2 / th);
	th = th / 3;

	(*stress)(0,6) = ai1 / 3 + c3 * cos(th);
	(*stress)(0,7) = ai1 / 3 + c3 * cos(th + 2 * PI / 3);
	(*stress)(0,8) = ai1 / 3 + c3 * cos(th + 4 * PI / 3);

	return stress;
}

template<class T>
void Tetrahedron<T>::BandSolver (Matrix<T> *stiffness)
{
	DBG_MSG("Entered BandSolver.");

	/* Solves the KQ=F equation to find out displacements */
	int n1,k,nk,i,i1,j,j1,kk, nbw;
	T c1;
	/* ----- band solver ----- */
	n1 = nDOF - 1;
	nbw = stiffness->getColumns();
	/* --- forward elimination --- */
	for (k = 1; k <= n1; k++) 
	{
		nk = nDOF - k + 1;
		if (nk > nbw)
			nk = nbw;
		for (i = 2; i <= nk; i++) 
		{
			c1 = (*stiffness)(k-1,i-1) / (*stiffness)(k-1,0);
			i1 = k + i - 1;
			for (j = i; j <= nk; j++) 
			{
				j1 = j - i + 1;
				(*stiffness)(i1-1,j1-1) = (*stiffness)(i1-1,j1-1) - 
							c1 * (*stiffness)(k-1,j-1);
			}
			Forces[i1-1] = Forces[i1-1] - c1 * Forces[k-1];
		}
	}
	
	/* --- back-substitution --- */
	Forces[nDOF-1] = Forces[nDOF-1] / (*stiffness)(nDOF-1,0);
	for (kk = 1; kk <= n1;kk++) {
		k = nDOF - kk;
		c1 = 1 / (*stiffness)(k-1,0);
		Forces[k-1] = c1 * Forces[k-1];
		nk = nDOF - k + 1;
		if (nk > nbw)
		nk = nbw;
		for (j = 2; j <= nk; j++) 
		{
			Forces[k-1] = Forces[k-1] - c1 * (*stiffness)(k-1,j-1) * 
									Forces[k + j - 2];
		}
	}
	DBG_MSG("Leaving BandSolver.");
}

template<class T>
void Tetrahedron<T>::Solve ()
{
	DBG_MSG("Entered Solve.");
	
	T penalparam = T();
	Matrix<T> *stiffness;
	
	stiffness = GetGlobalStiffnessMatrix ();

	/* ----- decide penalty parameter cnst ----- */
	for (int i = 0; i < nDOF; i++) 
	{
		if (penalparam < (*stiffness)(i,0))
			penalparam = (*stiffness)(i,0);
	}
	penalparam = penalparam * 10000.0;

	int k;
	/* ----- modify for displacement boundary conditions ----- */
	for (int i = 0; i < nDisp; i++)
	{
		k = Displacements[i].index;
		(*stiffness)(k-1,0) = (*stiffness)(k-1,0) + penalparam;
		Forces[k-1] = Forces[k-1] + penalparam * Displacements[i].value;
	}

	int i1, i2, n, m;
	/* ----- modify for multipoint constraints ----- */
	for (int i = 0; i < nMPC; i++)
	{
		i1 = MPCons[i].i-1;
		i2 = MPCons[i].j-1;
		(*stiffness)(i1,0) = (*stiffness)(i1,0) + 
						(penalparam * MPCons[i].b1 * MPCons[i].b1);
		(*stiffness)(i2,0) = (*stiffness)(i2,0) + 
						(penalparam * MPCons[i].b2 * MPCons[i].b2);
		n=i1;
		if (n > i2)
			n = i2;
		m = abs(i2-i1);
		(*stiffness)(n,m) = (*stiffness)(n,m) + 
						(penalparam * MPCons[i].b1 * MPCons[i].b2);
		Forces[i1] = Forces[i1] + (penalparam * MPCons[i].b1 * MPCons[i].b3);
		Forces[i2] = Forces[i2] + (penalparam * MPCons[i].b2 * MPCons[i].b3);
	}
	
	BandSolver (stiffness);

	/* Calculate reactions */
	for (int i = 0; i < nDisp; i++) 
	{
		Reactions.push_back(penalparam * (Displacements[i].value - 
						Forces[Displacements[i].index-1]));
	}
	
	/* -----  stress calculations ----- */
	Stresses = new Matrix<T>(nElements+1,9);
	for (n = 1; n <= nElements; n++) 
	{
		Matrix<T> *s = GetElementStressVector (n);
		
		for (int j = 0; j < 9; j++)
			(*Stresses)(n,j) = (*s)(0,j);
		
		delete s;
	}
	
	isSolved = true;
	
	DBG_MSG("Leaving Solve.");
}

template<class T>
bool Tetrahedron<T>::WriteResultsToFile (const char* fname)
{
	ofstream ofile (fname);
	if (!ofile)
	{
		ERR_MSG("File open failed");
		return false;
	}

	ofile << endl << "RESULTS" << endl;
	ofile << "bandwidth = " << CalculateBandwidth() << endl;
	ofile << "node#     x-displ      y-displ      z-displ" << endl;
	/* write displacements to file */
	for (int i = 0; i < nNodes; i++) 
	{
		ofile << i+1 << Forces[3*i] << Forces[3*i+1] << Forces[3*i+2] << endl;
	}
	
	/* write reactions at different boundary conditions to file */
	for (int i = 0; i < nDisp; i++)
	{
		ofile << Displacements[i].index << Reactions[i] << endl;
	}

	/* write stresses to file */
	for (int i = 1; i <= nElements; i++)
	{
		ofile << "stresses in element no. " << i << endl;
		ofile << "  normal stresses sx,sy,sz" << endl;
		ofile << "  "<< (*Stresses)(i,0) << "  " 
				<< (*Stresses)(i,1) << "  " << (*Stresses)(i,2) << endl;
		ofile << "  shear stresses tyz,txz,txy" << endl;
		ofile << "  " << (*Stresses)(i,3) << "  " << (*Stresses)(i,4) <<
				"  " << (*Stresses)(i,5) << endl;
		ofile << "  principal stresses" << endl;
		ofile << "  " << (*Stresses)(i,6) << "  " << (*Stresses)(i,7)
			<< "  " << (*Stresses)(i,8) << endl;
	}
	return true;
}

//    METHOD IMPLEMENTATION ENDS
//////////////////////////////////////////////////////////////////////////////

template<class T>
Tetrahedron<T>* CreateTetrahedronFromFile (const char* fname)
{
	ifstream ifile;
	int iTemp, nLoads, n;
	int nNodes, nElements, nMaterials, nDIM, nNodesPerElement,
		nDOFPerNode, nDisp, nMPC, nDOF, nProperties;
	char dummy[81], title[81];
	T tTemp, tx, ty, tz;

	ifile.open(fname);
	if (!ifile)
	{
		ERR_MSG("Cannot open output file.");
		exit (0);
	}

	ifile.getline (dummy,80);
	ifile.getline (title,80);
	ifile.getline (dummy,80);
	
	ifile >> nNodes >> nElements >> nMaterials >> nDIM >> nNodesPerElement 
		>> nDOFPerNode; 
	ifile.get (); // to ignore newline char
	
	ifile.getline (dummy, 80);
	ifile >> nDisp >> nLoads >> nMPC;
	ifile.get (); // to ignore newline char
	
	nDOF = nNodes * nDOFPerNode;
	/* material properties E, Nu, Alpha */
	nProperties = 3;

	if (nNodesPerElement != 4 || nDIM != 3 || nDOFPerNode != 3 || 
				nProperties != 3)
		return NULL;
	
	Tetrahedron<T> *t;
	try {
		t = new Tetrahedron<T>(nNodesPerElement, nDIM, nDOFPerNode, nProperties);
	} catch (bad_alloc) {
		ERR_MSG("Memory allocation error.");
		exit (1);
	}

	/* ----- coordinates ----- */
	ifile.getline (dummy,80);
	for (int i = 0; i < nNodes; i++)
	{
		ifile >> n;
		ifile >> tx >> ty >> tz;
		t->AddNode(n, tx, ty, tz);
		
		ifile.get (); // to ignore new line char
	}

	/* ----- connectivity, material, temp-change ----- */
	ifile.getline (dummy,80);
	int n1, n2, n3, n4;
	for (int i = 0; i < nElements; i++)
	{
		ifile >> n;
		ifile >> n1 >> n2 >> n3 >> n4;
		ifile >> iTemp;
		ifile >> tTemp;
		t->AddElement (n, iTemp, tTemp, n1, n2, n3, n4);
		
		ifile.get (); // to ignore new line char
	}

	/* ----- displacement bc  ----- */
	ifile.getline (dummy,80);
	for (int i = 0; i < nDisp; i++) 
	{
		ifile >> iTemp >> tTemp;
		t->AddDisplacement (iTemp, tTemp);
		
		ifile.get (); // to ignore newline char
	}

	/* ----- component loads ----- */
	ifile.getline (dummy,80);
	for (int i = 0; i < nLoads; i++) 
	{
		ifile >> iTemp >> tTemp;
		t->AddForce(iTemp-1,tTemp);
		
		ifile.get (); // to ignore newline char
	}

	/* ----- material properties ----- */
	ifile.getline (dummy,80);
	T E, Alpha, Nu;
	for (int i = 0; i < nMaterials; i++)
	{
		ifile >> iTemp;
		if (iTemp > nMaterials) {
			ERR_MSG("Material index must be less than number of materials");
			exit (1);
		}
		
		ifile >> E >> Alpha >> Nu;
		t->AddMaterial (iTemp, E, Alpha, Nu);
	}

	/* ----- multipoint constraints ----- */
	ifile.getline (dummy, 80);
	T b1, b2, b3;
	int jTemp;
	for(int i = 0; i < nMPC; i++)
	{
		ifile >> b1 >> iTemp >> b2 >> jTemp >> b3;
		t->AddMPCons (b1, iTemp, b2, jTemp, b3);
	}

	ifile.close ();

	return t;
}

#endif /* __TETRAHEDRON_HEADER__ */

