#include "TetraCSGPrimitive.h"
#include "TetgenInterface.h"
#include "tetgen/tetgen.h"
#include <vector>
#include <algorithm>

using namespace std;

Tetrahedron<REAL>*
TetgenInterface :: Tetrahedralize (Tetrahedron<REAL>* tetra)
{
	tetgenio in, out;

	/* 
	 * load all tetrahedron data into in 
	 */
	
	/* load nodal points in tetgenio */
	in.pointlist = new REAL[tetra->nDOFPerNode * tetra->nNodes];
	in.numberofpoints = tetra->nNodes;
	for (int i = 1; i <= tetra->nNodes; i++)
	{
		in.pointlist[(i-1)*tetra->nDOFPerNode + 0] = tetra->Nodes[i].x;	
		in.pointlist[(i-1)*tetra->nDOFPerNode + 1] = tetra->Nodes[i].y;
		in.pointlist[(i-1)*tetra->nDOFPerNode + 2] = tetra->Nodes[i].z;
	}

	/* load tetrahedron element list into tetgenio */
	in.tetrahedronlist = new int[tetra->nElements*tetra->nNodesPerElement];
	in.numberofcorners = tetra->nNodesPerElement;
	in.numberoftetrahedra = tetra->nElements;
	for (int i = 1; i <= tetra->nElements; i++)
	{
		in.tetrahedronlist[(i-1)*tetra->nNodesPerElement + 0] = 
				tetra->Elements[i].Nodes[0]-1;
		in.tetrahedronlist[(i-1)*tetra->nNodesPerElement + 1] = 
				tetra->Elements[i].Nodes[1]-1;
		in.tetrahedronlist[(i-1)*tetra->nNodesPerElement + 2] = 
				tetra->Elements[i].Nodes[2]-1;
		in.tetrahedronlist[(i-1)*tetra->nNodesPerElement + 3] = 
				tetra->Elements[i].Nodes[3]-1;
	}

	/*
	 * call tetrahedralize
	 */
	tetrahedralize ("rq", &in, &out);

	/*
	 * convert the meshed data in out into Tetrahedron
	 * object
	 */
	Tetrahedron<REAL>* tout = new Tetrahedron<REAL> 
				(tetra->nNodesPerElement, tetra->nDIM, 
				 tetra->nDOFPerNode, tetra->nProperties);
	
	for (int i = 0; i < out.numberofpoints; i++)
	{
		tout->AddNode (i+1, out.pointlist[i*3], out.pointlist[i*3+1], 
						out.pointlist[i*3+2]);
	}

	/* Add correct material and temperature */
	for (int i = 0; i < out.numberoftetrahedra; i++)
	{
		tout->AddElement (i+1, 1, 0.0, 
						out.tetrahedronlist[i*3]+1,
						out.tetrahedronlist[i*3+1]+1,
						out.tetrahedronlist[i*3+2]+1,
						out.tetrahedronlist[i*3+3]+1);
	}

	return tout;
}

int 
TetgenInterface :: GetIndex (vector<CSGPoint>& points, CSGPoint *p)
{
	for (unsigned int i = 0; i < points.size (); i++)
	{
		if (points[i].GetX () == p->GetX () && 
			points[i].GetY () == p->GetY () &&
			points[i].GetZ () == p->GetZ ())
			return i;
	}
	return -1;
}

Tetrahedron<REAL>*
TetgenInterface :: GetFEMModel (CSGObject *object)
{
	tetgenio in, out;

	in.pointlist = new REAL[object->points.size() * 3];

	for (unsigned int i = 0; i < object->points.size (); i++)
	{
		in.pointlist[i*3] = object->points[i].GetX();
		in.pointlist[i*3 + 1] = object->points[i].GetY();
		in.pointlist[i*3 + 2] = object->points[i].GetZ();
	}
	in.numberofpoints = object->points.size ();

	in.trifacelist = new int[object->triangles.size() * 3];
	for (unsigned int i = 0; i < object->triangles.size (); i++)
	{
		in.trifacelist[i*3] = GetIndex (object->points, object->triangles[i]->points[0]);
		in.trifacelist[i*3 + 1] = GetIndex (object->points, 
					object->triangles[i]->points[1]);
		in.trifacelist[i*3 + 2] = GetIndex (object->points, 
					object->triangles[i]->points[2]);
	}

	in.numberoftrifaces = object->triangles.size ();

	tetrahedralize ("", &in, &out);

	Tetrahedron<REAL> *tout = new Tetrahedron<REAL> (4, 3, 3, 3);
	
	for (int i = 0; i < out.numberofpoints; i++)
	{
		tout->AddNode (i+1, out.pointlist[i*3], out.pointlist[i*3+1], 
						out.pointlist[i*3+2]);
	}

	tout->AddMaterial (1, 10.0, 0.1, 0.1);
	/* Add correct material and temperature */
	for (int i = 0; i < out.numberoftetrahedra; i++)
	{
		tout->AddElement (i+1, 1, 0.0, 
						out.tetrahedronlist[i*3]+1,
						out.tetrahedronlist[i*3+1]+1,
						out.tetrahedronlist[i*3+2]+1,
						out.tetrahedronlist[i*3+3]+1);
	}
/*
	delete in.pointlist;
	delete in.trifacelist;
*/
	return tout;
}
