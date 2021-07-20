#ifndef __ADAPTIVE_MESHER_HEADER__
#define __ADAPTIVE_MESHER_HEADER__

/////////////////////////////////////////////////////////////////////
// Include files

#include <vector>
#include <algorithm>

using namespace std;

/////////////////////////////////////////////////////////////////////
// AdaptiveMesher class declaration begins

class AdaptiveMesher
{
public:

	struct tagEdge;

	typedef struct tagEdge EDGE;
	typedef struct tagEdge* PEDGE;
	
	struct tagNode {
		REAL x, y, z;
		vector<PEDGE> Edges;
	};

	typedef struct tagNode NODE;
	typedef struct tagNode* PNODE;
	
	struct tagEdge {
		NODE* Nodes[2];
		bool Refine;
	};

	struct tagElement {
		PNODE Nodes[4];
		PEDGE Edges[6];
		int elmIndex;
	};
	
    typedef struct tagElement ELEMENT;
    typedef struct tagElement* PELEMENT;

	REAL MaxPropertyValue;

	
	AdaptiveMesher (Tetrahedron<REAL> *t);
	~AdaptiveMesher ();

	/* It implements the algorithm explained in
	clearproc.png file */
	void Mesh ();
	Tetrahedron<REAL>* GetMeshedTetrahedron ();

private:

	static const int EDGES_PER_ELEMENT = 6;
	static const int NODES_PER_ELEMENT = 4;

	Tetrahedron<REAL> *tetra;	

	vector<PNODE> Nodes, MeshedNodes;
	vector<PEDGE> Edges, MeshedEdges;
	vector<PELEMENT> Elements, MeshedElements;

	/* Form B-Rep model from the connectivity table */
	void FormBRepModel ();
	/* Find the max value of the property choosen for
	 * checking the refinement of an element */
	void FindMaxPropertyValue ();
	/* Mark elements for refinement */
	void MarkElements ();
	/* Mesh elements marked for refinement */
	void MeshElements ();
	/* Returns true if the element requires refinement */
	bool NeedsRefinement (PELEMENT elm);
	/* Checks if two nodes are at the same point */
	bool isSameNode (PNODE node1, PNODE node2);
	/* Checks if two edges are intersecting each other */
	bool isIntersectingEdge (PEDGE srcedge, PEDGE destedge);
	
	/* Returns the mid node of an edge */
	PNODE GetMidNode (vector<PNODE>&, const PEDGE& edge);
	/* Form a tetrahedron using the Beys Method */
	PELEMENT FormTetrahedron (vector<PELEMENT>&, vector<PEDGE>&, const PNODE);
	/* Returns the index of the edge, formed between two
	 * nodes, within the edges vector */
	int GetEdgeIndex (const vector<PEDGE>&, PNODE, PNODE);

	/* Returns the node pointer, which correctly points to
	 * node which can be used to form an edge, which does not
	 * intersect with any other edge */
	PNODE GetCorrectNode (vector<PEDGE>&, vector<PNODE>&, PEDGE, PEDGE);
	/* Create a Node at the 3D point and add to the vector
	 * based on the addToList value */
	PNODE CreateTetraNode (vector<PNODE>&, REAL x, REAL y, REAL z, bool addToList = true);
	/* Create a Node at the same point as that of the node
	 * passed */
	PNODE CreateTetraNode (vector<PNODE>&, PNODE, bool addToList = false);
	/* Create an edge between two nodes and add it into
	 * edges vector based on the addToList value */
	PEDGE CreateTetraEdge (vector<PEDGE>&, PNODE n1, PNODE n2, bool addToList = true);
	/* Creata an edge between the same nodes that the edge
	 * passed exists between */
	PEDGE CreateTetraEdge (vector<PEDGE>& Edges, PEDGE edge);
	/* Create edges between the nodes */
	PEDGE* CreateEdgesFromNodes (vector<PEDGE>&, PNODE* nodes);
	/* Create an element from the nodes and edges, and add
	 * it into the elements vector based on the addToList
	 * value */
	PELEMENT CreateTetraElement (vector<PELEMENT>&, PNODE*, PEDGE*, bool addToList = true);
};

// AdaptiveMesher class declaration ends
/////////////////////////////////////////////////////////////////////

#endif /* __ADAPTIVE_MESHER_HEADER__ */

