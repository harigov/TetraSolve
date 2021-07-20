/////////////////////////////////////////////////////////////////////
// Include files

#include "TetraSolve.h"
#include "Tetrahedron.h"
#include "AdaptiveMesher.h"

#include <new>
#include <cassert>
#include <cmath>

/////////////////////////////////////////////////////////////////////
// AdaptiveMesher class member functions implementation begins

AdaptiveMesher :: AdaptiveMesher (Tetrahedron<REAL> *t)
{
	DBG_MSG ("AdaptiveMesher :: AdaptiveMesher Entered");

	tetra = t;

	DBG_MSG ("AdaptiveMesher :: AdaptiveMesher Leaving");
}

AdaptiveMesher :: ~AdaptiveMesher ()
{
	DBG_MSG ("AdaptiveMesher :: ~AdaptiveMesher Entered");
	/* nothing to be done for now */
	DBG_MSG ("AdaptiveMesher :: ~AdaptiveMesher Leaving");
}

void 
AdaptiveMesher :: Mesh ()
{
	DBG_MSG ("AdaptiveMesher :: Mesh Entered");

	/* Before proceeding, solve the problem */
	if ( !(tetra->isSolved) ) tetra->Solve ();

	FormBRepModel ();
	FindMaxPropertyValue ();
	MarkElements ();
	MeshElements ();

	DBG_MSG ("AdaptiveMesher :: Mesh Leaving");
}

void
AdaptiveMesher :: FindMaxPropertyValue ()
{
	// tetra->Elements uses one based index
	size_t size = tetra->Elements.size() - 1;
	MaxPropertyValue = 0;
	for (unsigned int i = 0; i < size; i++)
	{
		if ( (*(tetra->Stresses))(i, 0) > MaxPropertyValue)
			MaxPropertyValue  = (*(tetra->Stresses))(i, 0);
	}
}

Tetrahedron<REAL>*
AdaptiveMesher :: GetMeshedTetrahedron ()
{
	DBG_MSG ("AdaptiveMesher :: GetMeshedTetrahedron Entered");

	Tetrahedron<REAL> *meshtetra = new Tetrahedron<REAL>(tetra->nNodesPerElement,
							tetra->nDIM, tetra->nDOFPerNode, tetra->nProperties);

	/* Materials start from index 1 */
	for (unsigned int i = 1; i < tetra->Materials.size(); i++)
	{
		meshtetra->AddMaterial (i, tetra->Materials[i].E, 
								tetra->Materials[i].Nu, tetra->Materials[i].Alpha);
	}

	for (unsigned int i = 0; i < MeshedNodes.size(); i++)
	{
		assert (MeshedNodes[i] != NULL);
		/*SEL_MSG (wxString::Format ("Node %p - (%.2f,%.2f,%.2f) is added", 
								MeshedNodes[i],	MeshedNodes[i]->x, 
								MeshedNodes[i]->y, MeshedNodes[i]->z));*/
		
		meshtetra->AddNode (i+1, MeshedNodes[i]->x, MeshedNodes[i]->y, 
						MeshedNodes[i]->z);
	}

	for (unsigned int i = 0; i < MeshedNodes.size(); i++)
	{
		for (unsigned int j = 0; j < tetra->Displacements.size() ; j++)
		{
			if (tetra->Nodes[(tetra->Displacements[j].index / tetra->nDOFPerNode)].x 
							== MeshedNodes[i]->x &&
				tetra->Nodes[(tetra->Displacements[j].index / tetra->nDOFPerNode)].y 
							== MeshedNodes[i]->y &&
				tetra->Nodes[(tetra->Displacements[j].index / tetra->nDOFPerNode)].z 
							== MeshedNodes[i]->z)
			{
				/*SEL_MSG (wxString::Format ("D %d - %f added to %d", 
										tetra->Displacements[j].index, 
										tetra->Displacements[j].value, i));*/
				meshtetra->AddDisplacement ((i * tetra->nDOFPerNode) + 
								(tetra->Displacements[j].index % tetra->nDOFPerNode) 
								+ 1, tetra->Displacements[j].value);
			}
		}

		for (unsigned int j = 0; j < tetra->Loads.size() ; j++)
		{
			if (tetra->Nodes[(tetra->Loads[j].index / tetra->nDOFPerNode)+1].x 
							== MeshedNodes[i]->x &&
				tetra->Nodes[(tetra->Loads[j].index / tetra->nDOFPerNode)+1].y 
							== MeshedNodes[i]->y &&
				tetra->Nodes[(tetra->Loads[j].index / tetra->nDOFPerNode)+1].z 
							== MeshedNodes[i]->z)
			{
				/*SEL_MSG (wxString::Format ("D %d - %f added to %d", 
										tetra->Loads[j].index, 
										tetra->Loads[j].value, i));*/
				meshtetra->AddForce ((i * tetra->nDOFPerNode) + 
								(tetra->Loads[j].index % tetra->nDOFPerNode),
								tetra->Loads[j].value);
			}
		}
	}


	DBG_MSG ("Addition of nodes complete.");

	for (unsigned int i = 0; i < MeshedElements.size(); i++)
	{
		int index[NODES_PER_ELEMENT] = {0};

		for (int j = 0; j < NODES_PER_ELEMENT; j++)
		{
			assert (MeshedElements[i] != NULL);
			assert (MeshedElements[i]->Nodes[j] != NULL);

			vector<PNODE>::iterator it = find (MeshedNodes.begin(), MeshedNodes.end(),
								MeshedElements[i]->Nodes[j]);
			if (it != MeshedNodes.end())
			{
				index[j] = it - MeshedNodes.begin();
				index[j]++; // to make it 1 based index
			}
			else
			{
				/*ERR_MSG (wxString::Format ("A node %p - (%.2f,%.2f,%.2f) used" 
										" by a meshed element %u is not present"
										"in meshed nodes vector", 
										MeshedElements[i]->Nodes[j], 
										MeshedElements[i]->Nodes[j]->x,
										MeshedElements[i]->Nodes[j]->y, 
										MeshedElements[i]->Nodes[j]->z, i));*/
			}
		}

		/*SEL_MSG (wxString::Format ("Element %d index %d", i, 
								MeshedElements[i]->elmIndex));*/

		meshtetra->AddElement (i+1, 
						tetra->Elements[MeshedElements[i]->elmIndex].Material,
						tetra->Elements[MeshedElements[i]->elmIndex].Temp, 
						index[0], index[1], index[2], index[3]);
	}

	DBG_MSG ("AdaptiveMesher :: GetMeshedTetrahedron Leaving");

	return meshtetra;
}

void 
AdaptiveMesher :: FormBRepModel ()
{
	DBG_MSG ("AdaptiveMesher :: FormBRepModel Entered");

	/* TODO: Throw an exception */
	/*if (!tetra->isSolved) */
		
	for (unsigned int i = 1; i < tetra->Elements.size(); i++)
	{
		PNODE nodes[NODES_PER_ELEMENT];
		for (int j = 0; j < NODES_PER_ELEMENT; j++)
		{
			nodes[j] = CreateTetraNode (
						Nodes,
						tetra->Nodes[tetra->Elements[i].Nodes[j]].x,
						tetra->Nodes[tetra->Elements[i].Nodes[j]].y,
						tetra->Nodes[tetra->Elements[i].Nodes[j]].z
					);
			if (nodes[j] == NULL)
				ERR_MSG ("ERROR: Not enough memory");
		}
	
		PEDGE* edges = CreateEdgesFromNodes (Edges, nodes);
		if (edges == NULL)
			ERR_MSG ("ERROR: Not enough memory");
		
		PELEMENT elm = CreateTetraElement (Elements, nodes, edges);
		if (elm == NULL)
			ERR_MSG ("ERROR: Not enough memory");
		elm->elmIndex = i;
	}

	DBG_MSG ("AdaptiveMesher :: FormBRepModel Leaving");
}

void 
AdaptiveMesher :: MarkElements ()
{
	DBG_MSG ("AdaptiveMesher :: MarkElements Entered");

	/* loop every element and mark edges of elements
	 * which need refinement */
	for (unsigned int i = 0; i < Elements.size() ; i++)
	{
		if (NeedsRefinement (Elements[i]))
		{
			for (int j = 0; j < EDGES_PER_ELEMENT; j++)
			{
				Elements[i]->Edges[j]->Refine = true;
			}
		}
	}

	DBG_MSG ("AdaptiveMesher :: MarkElements Leaving");
}

void 
AdaptiveMesher :: MeshElements ()
{	
	DBG_MSG ("AdaptiveMesher :: MeshElements Entered");

	vector<PNODE> localNodes;
	vector<PEDGE> localEdges;
	
	/* TODO: Refine the marked elements */
	for (unsigned int i = 0; i < Elements.size() ; i++)
	{
		localNodes.clear ();
		localEdges.clear ();	

		int edgeCount = 0;
		/* find the number of edges to be refined */
		for (int j = 0; j < 6; j++)
			if (Elements[i]->Edges[j]->Refine) 
				edgeCount++;

		if (edgeCount == 1)
		{
			/* points to the edge to be refined */
			PEDGE edge;
			for (int j = 0; j < EDGES_PER_ELEMENT; j++)
			{
				if (Elements[i]->Edges[j]->Refine)
				{
					edge = Elements[i]->Edges[j];
					break;
				}
			}

			PNODE nodes[4];
			nodes[0] = NULL; 
			nodes[1] = NULL;
			/* find the opposite edge of the edge to be
			 * refined */
			for (int j = 0; j < EDGES_PER_ELEMENT; j++)
			{
				DBG_MSG ("Came inside this loop");
				PNODE n1 = edge->Nodes[0], en1 = Elements[i]->Edges[j]->Nodes[0],
						n2 = edge->Nodes[1], en2 = Elements[i]->Edges[j]->Nodes[1];
				if ( (n1->x != en1->x || n1->y != en1->y || n1->z != en1->z)&&
					 (n2->x != en1->x || n2->y != en1->y || n2->z != en1->z)&&
					 (n1->x != en2->x || n1->y != en2->y || n1->z != en2->z)&&
					 (n2->x != en2->x || n2->y != en2->y || n2->z != en2->z)
					)
				{
					DBG_MSG ("Came inside if condition");
					nodes[0] = CreateTetraNode (MeshedNodes, 
									Elements[i]->Edges[j]->Nodes[0]);
					nodes[1] = CreateTetraNode (MeshedNodes, 
									Elements[i]->Edges[j]->Nodes[1]);
					break;
				}
			}
			
			/* Now two tetrahedrons are formed from
			 * the given tetra, by dividing it at the mid
			 * point of the edge to be refined */
			nodes[2] = CreateTetraNode (MeshedNodes, edge->Nodes[0]);
			nodes[3] = GetMidNode (MeshedNodes, edge);

			//DBG_MSG (wxString::Format("%d Element", i));

			EDGE**edges = CreateEdgesFromNodes (MeshedEdges, nodes);
			ELEMENT*elm = CreateTetraElement (MeshedElements, nodes, edges);
			elm->elmIndex = Elements[i]->elmIndex;

			nodes[2] = CreateTetraNode (MeshedNodes, edge->Nodes[1]);
			edges = CreateEdgesFromNodes (MeshedEdges, nodes);
			elm = CreateTetraElement (MeshedElements, nodes, edges);
			elm->elmIndex = Elements[i]->elmIndex;
		}
		else if (edgeCount >= 2)
		{
			/* STEP 1
			 * 		Refine each edge and add them to localEdges
			 * 		and localNodes vector */
			for (int j = 0; j < EDGES_PER_ELEMENT; j++)
			{
				PNODE node1, node2;

				node1 = CreateTetraNode (MeshedNodes, 
								Elements[i]->Edges[j]->Nodes[0]);
				CreateTetraNode (localNodes, node1, true);

				node2 = CreateTetraNode (MeshedNodes,
								Elements[i]->Edges[j]->Nodes[1]);
				CreateTetraNode (localNodes, node2, true);
				
				if (Elements[i]->Edges[j]->Refine || edgeCount >= 3)
				{
					PEDGE edge1, edge2;
					PNODE midnode;

					midnode = GetMidNode (MeshedNodes, Elements[i]->Edges[j]);
					CreateTetraNode (localNodes, midnode, true);
					
					edge1 = CreateTetraEdge (localEdges, node1,	midnode);
					edge2 = CreateTetraEdge (localEdges, midnode, node2);

					CreateTetraEdge (node1->Edges, edge1);
					CreateTetraEdge (node2->Edges, edge2);
					CreateTetraEdge (midnode->Edges, edge1);
					CreateTetraEdge (midnode->Edges, edge2);

					/* Create edges between edges */
					for (int k = 0; k < EDGES_PER_ELEMENT; k++)
					{
						/* if both k and j are same, 
						then continue with next edge */
						if (k == j) continue;
						
						if (Elements[i]->Edges[k]->Refine || edgeCount >= 3)
						{
							PNODE midnode2 = GetMidNode (MeshedNodes, 
														Elements[i]->Edges[k]);
							CreateTetraNode (localNodes, midnode2, true);
							PEDGE edge = CreateTetraEdge (localEdges,
											midnode, midnode2, false);
							if (edge == NULL) continue;

							DBG_MSG ("isIntersectingEdge entering");
							bool intersect_flag = false;
							for (unsigned int m = 0; 
										edge != NULL && m < localEdges.size(); m++)
							{
								if (isIntersectingEdge (localEdges[m], edge))
								{
									DBG_MSG ("isIntersectingEdge inside");
									intersect_flag = true;
									break;
								}
							}
							DBG_MSG ("isIntersectingEdge complete");

							if (intersect_flag) continue;

							edge = CreateTetraEdge (localEdges, edge);
							CreateTetraEdge (midnode->Edges, edge);
							CreateTetraEdge (midnode2->Edges, edge);
						}
						else
						{
							PNODE e;

							if ((e = GetCorrectNode (localEdges, localNodes,
												Elements[i]->Edges[j], 
												Elements[i]->Edges[k])) 
									!= NULL)
							{
								PNODE endnode = CreateTetraNode (MeshedNodes, e);
								CreateTetraNode (localNodes, endnode, true);

								PEDGE edge = CreateTetraEdge (localEdges,
												midnode, endnode, false);
								if (edge == NULL) continue;
								
								for (unsigned int m = 0; 
										edge != NULL && m < localEdges.size(); m++)
								{
									if (isIntersectingEdge (localEdges[m], edge))
									{
										continue;
									}
								}
								
								edge = CreateTetraEdge (localEdges, edge);
								CreateTetraEdge (endnode->Edges, edge);
								CreateTetraEdge (midnode->Edges, edge);
							}
						}
					}
				}
				else
				{
					PEDGE edge = CreateTetraEdge (localEdges, node1, node2);
					CreateTetraEdge (node1->Edges, edge);
					CreateTetraEdge (node2->Edges, edge);
				}
			}

			for (unsigned int j = 0; j < localEdges.size() ; j++)
			{
				/*SEL_MSG (wxString::Format ("Edge %d - (%.2f, %.2f, %.2f)"
							" to (%.2f, %.2f, %.2f)", j, 
							localEdges[j]->Nodes[0]->x, localEdges[j]->Nodes[0]->y, 
							localEdges[j]->Nodes[0]->z, localEdges[j]->Nodes[1]->x, 
							localEdges[j]->Nodes[1]->y, localEdges[j]->Nodes[1]->z));*/
			}

			/* form tetrahedrons from nodes */
			for (unsigned int j = 0; j < localNodes.size(); j++)
			{
				PELEMENT elm;
				while ((elm = FormTetrahedron (MeshedElements, 
										localEdges, localNodes[j])) != NULL)
				{
					elm->elmIndex = Elements[i]->elmIndex;
				}
			}
			
			/*wxLogMessage (wxString::Format ("%d elements formed from %d edges", 
									MeshedElements.size(), localEdges.size()));*/

			for (unsigned int j = 0; j < localNodes.size(); j++)
			{
				localNodes[j]->Edges.clear();
				/*SEL_MSG (wxString::Format 
							("localNode %d (%.2f,%.2f,%.2f) added to MeshedNodes",
							j, localNodes[j]->x, localNodes[j]->y, localNodes[j]->z));*/
				CreateTetraNode (MeshedNodes, localNodes[j], true);
				//MeshedNodes.push_back (localNodes[j]);
			}
		}
		/* else add the element as is */
		else
		{
			PNODE *nodes = new PNODE[4];
			for (unsigned int j = 0; j < NODES_PER_ELEMENT; j++)
				nodes[j] = CreateTetraNode (MeshedNodes, Elements[i]->Nodes[j]);
			PEDGE *edges = CreateEdgesFromNodes (MeshedEdges, nodes);
			PELEMENT elm = CreateTetraElement (MeshedElements, nodes, edges);
			elm->elmIndex = Elements[i]->elmIndex;
		}
	}

	DBG_MSG ("AdaptiveMesher :: MeshElements Leaving");
}

AdaptiveMesher::PNODE
AdaptiveMesher :: GetMidNode (vector<PNODE>& lNodes, const PEDGE& edge)
{
	DBG_MSG ("AdaptiveMesher :: GetMidNode Entered");

	PNODE node = CreateTetraNode (lNodes,
		(edge->Nodes[0]->x + edge->Nodes[1]->x)/2,
		(edge->Nodes[0]->y + edge->Nodes[1]->y)/2,
		(edge->Nodes[0]->z + edge->Nodes[1]->z)/2
		);
	DBG_MSG ("AdaptiveMesher :: GetMidNode Leaving");
	return node;
}

/* Forms a tetrahedron using the <TODO:PLACE ALGO NAME>
 * edges vector is required to check for the existence
 * of an edge. After the element is formed, it is pushed
 * into elems vector */
AdaptiveMesher::PELEMENT
AdaptiveMesher :: FormTetrahedron (vector<PELEMENT>& elems, 
						vector<PEDGE>& edges, PNODE orignode)
{
	DBG_MSG ("AdaptiveMesher :: FormTetrahedron Entered");

	static PNODE prevnode = NULL;
	static unsigned int start_edge = 0;
	
	PEDGE edge[EDGES_PER_ELEMENT];
	PNODE node[NODES_PER_ELEMENT];

	/* final element to be returned */
	PELEMENT elm = NULL;
	
	assert (orignode != NULL);

	if (prevnode == orignode && (start_edge+1) < orignode->Edges.size())
		start_edge++;
	else if (prevnode != orignode)
		start_edge = 0;
	else 
		return NULL;

	prevnode = orignode;
	
	edge[0] = orignode->Edges[start_edge];

	node[0] = orignode;
	node[1] = (edge[0]->Nodes[1] == node[0]) 
						? edge[0]->Nodes[0] : edge[0]->Nodes[1];
	
	for (unsigned int i = 0; i < node[1]->Edges.size(); i++)
	{
		edge[1] = node[1]->Edges[i];

		if (edge[1] == edge[0])
			continue;

		int e;
		node[2] = (edge[1]->Nodes[1] == node[1]) 
						? edge[1]->Nodes[0] : edge[1]->Nodes[1];

		/* if an edge exists between node3 and node1, then
		 * this edge1, edge2 and edge3 form a face of
		 * tetrahedron */
		if ((e = GetEdgeIndex (edges, node[0], node[2])) != -1)
		{
			edge[2] = edges[e];

			/* now find a node which has edges with all the
			 * three nodes found till now */
			for (unsigned int j = 0; j < node[2]->Edges.size(); j++)
			{
				edge[3] = node[2]->Edges[j];
				
				if (edge[3] == edge[2] || edge[3] == edge[1])
					continue;

				node[3] = (edge[3]->Nodes[1] == node[2])
								? edge[3]->Nodes[0] : edge[3]->Nodes[1];
			
				int e1, e2;	
				if ((e1 = GetEdgeIndex (edges, node[0], node[3])) != -1 &&
						(e2 = GetEdgeIndex (edges, node[1], node[3])) != -1)
				{
					edge[4] = edges[e1];
					edge[5] = edges[e2];
					
					for (unsigned int k = 0; k < NODES_PER_ELEMENT; k++)
					{
						/*DBG_MSG (wxString::Format("%d (%.2f, %.2f, %.2f)", 
										k, node[k]->x, node[k]->y, node[k]->z));*/
					}
					elm = CreateTetraElement (elems, node, edge);

					DBG_MSG ("AdaptiveMesher :: FormTetrahedron Leaving");

					return elm; /* done return */
				}
			}
		}
	}

	DBG_MSG ("AdaptiveMesher :: FormTetrahedron (NULL) Leaving");

	/* if it comes till here, then we cannot create
	 * a tetra from this node and the edges linked
	 * with it */
	return NULL;
}

/* Checks properties of an element and returns true if the element
needs to be refined */
bool 
AdaptiveMesher :: NeedsRefinement (AdaptiveMesher::PELEMENT elm)
{
	DBG_MSG ("AdaptiveMesher :: NeedsRefinement Entered");
	/* check if an element is to be regularly refined
	 * or not 
	 * MaxPropertyValue is set found by FindMaxPropertyValue function.
	 * Check that if you change anything here.
	 */
	if ( (*(tetra->Stresses))(elm->elmIndex-1, 0) > 0.8 * MaxPropertyValue)
	{
		DBG_MSG ("AdaptiveMesher :: NeedsRefinement Leaving");
		return true;
	}

	DBG_MSG ("AdaptiveMesher :: NeedsRefinement Leaving");

	return false;
}

/* edges vector must not be constant, it will be
modified by GetMidNode call with in the function */
AdaptiveMesher::PNODE 
AdaptiveMesher :: GetCorrectNode (vector<PEDGE>& edges, vector<PNODE>& nodes,
				PEDGE fromedge, PEDGE toedge)
{
	DBG_MSG ("AdaptiveMesher :: GetCorrectNode Entered");

	if ((fromedge->Nodes[0] == toedge->Nodes[0] || 
		fromedge->Nodes[1] == toedge->Nodes[0]) &&
		(GetEdgeIndex (edges, GetMidNode (nodes, fromedge), 
					   toedge->Nodes[0]) == -1))
	{
		DBG_MSG ("AdaptiveMesher :: GetCurrentNode Leaving");
		return toedge->Nodes[0];
	}
	else if ((fromedge->Nodes[0] == toedge->Nodes[1] ||
			fromedge->Nodes[1] == toedge->Nodes[1]) &&
			(GetEdgeIndex (edges, GetMidNode(nodes, fromedge),
						   toedge->Nodes[1]) == -1))
	{
		DBG_MSG ("AdaptiveMesher :: GetCurrentNode Leaving");
		return toedge->Nodes[1];
	}
	
	DBG_MSG ("AdaptiveMesher :: GetCurrentNode (NULL) Leaving");
	return NULL;
}

/*
 * Get the index of an edge between node1 and node2
 * Returns the index if found, else -1 is returned
 */
int
AdaptiveMesher :: GetEdgeIndex (const vector<PEDGE>& Edges, 
									PNODE n1, PNODE n2)
{
	DBG_MSG ("AdaptiveMesher :: GetEdgeIndex Entered");

	for (unsigned int i = 0; i < Edges.size(); i++)
	{
		/* testing both ways, so that an edge's nodes
		 * may be in any order */
		if ((Edges[i]->Nodes[0] == n1 && Edges[i]->Nodes[1] == n2)
			|| (Edges[i]->Nodes[0] == n2 && Edges[i]->Nodes[1] == n1))
		{
			DBG_MSG ("AdaptiveMesher :: GetEdgeIndex Leaving");
			return i;
		}
	}

	DBG_MSG ("AdaptiveMesher :: GetEdgeIndex Leaving");
	return -1;
}

AdaptiveMesher::PELEMENT 
AdaptiveMesher :: CreateTetraElement (vector<PELEMENT>& Elements, 
						PNODE* nodes, PEDGE* edges, bool addToList)
{
	DBG_MSG ("AdaptiveMesher :: CreateTetraElement Entered");

	PELEMENT elm = new (nothrow) ELEMENT;
	if (elm == NULL)
	{
		ERR_MSG ("ERROR: Not enough memory");
	}

	assert (nodes != NULL);
	assert (edges != NULL);

	for (unsigned int i = 0; i < Elements.size(); i++)
	{
		int nodes_match = 0;
		for (unsigned int j = 0; j < NODES_PER_ELEMENT; j++)
		{
			for (unsigned int k = 0; k < NODES_PER_ELEMENT; k++)
			{
				if (isSameNode (Elements[i]->Nodes[j], nodes[k]))
					nodes_match++;
			}
		}

		if (nodes_match == NODES_PER_ELEMENT)
			return Elements[i];
	}
	
	for (int i = 0; i < NODES_PER_ELEMENT; i++)
		elm->Nodes[i] = nodes[i];
	for (int i = 0; i < EDGES_PER_ELEMENT; i++)
		elm->Edges[i] = edges[i];

	elm->elmIndex = -1;

	if (addToList)
		Elements.push_back (elm);
	
	DBG_MSG ("AdaptiveMesher :: CreateTetraElement Leaving");
	return elm;
}

/* Checks if two nodes are same */
bool
AdaptiveMesher :: isSameNode (PNODE node1, PNODE node2)
{
	if (node1->x == node2->x && node1->y == node2->y && node1->z == node2->z)
		return true;

	return false;
}

bool AdaptiveMesher :: isIntersectingEdge ( PEDGE e1,PEDGE e2)
{
	REAL x13, y13, z13, x43, y43, z43, x21, y21, z21;
	REAL d1343,d4321,d1321,d4343,d2121;
	REAL numer,denom;
	REAL mua, mub;
	static const REAL EPS = 0.1;
	
	x13 = e1->Nodes[0]->x - e2->Nodes[0]->x;
	y13 = e1->Nodes[0]->y - e2->Nodes[0]->y;
	z13 = e1->Nodes[0]->z - e2->Nodes[0]->z;
	
	x43 = e2->Nodes[1]->x - e2->Nodes[0]->x;
	y43 = e2->Nodes[1]->y - e2->Nodes[0]->y;
	z43 = e2->Nodes[1]->z - e2->Nodes[0]->z;
	
	if (fabs(x43) < EPS && fabs(y43)  < EPS && fabs(z43) < EPS)
	  return false;
	
	x21 = e1->Nodes[1]->x - e1->Nodes[0]->x;
	y21 = e1->Nodes[1]->y - e1->Nodes[0]->y;
	z21 = e1->Nodes[1]->z - e1->Nodes[0]->z;
	if (fabs(x21) < EPS && fabs(y21) < EPS && fabs(z21) < EPS)
	  return false;
	
	d1343 = x13 * x43 + y13 * y43 + z13 * z43;
	d4321 = x43 * x21 + y43 * y21 + z43 * z21;
	d1321 = x13 * x21 + y13 * y21 + z13 * z21;
	d4343 = x43 * x43 + y43 * y43 + z43 * z43;
	d2121 = x21 * x21 + y21 * y21 + z21 * z21;
	
	denom = d2121 * d4343 - d4321 * d4321;
	if (fabs(denom) < EPS)
	  return false;
	
	numer = d1343 * d4321 - d1321 * d4343;
	
	mua = numer / denom;
	mub = (d1343 + d4321 * (mua)) / d4343;
	
	//SEL_MSG (wxString::Format("mua = %.2f, mub = %.2f", mua, mub));

	REAL px1, py1, pz1, px2, py2, pz2, 
		e1x1, e1x2, e1y1, e1y2, e1z1, e1z2, 
		e2x1, e2x2, e2y1, e2y2, e2z1, e2z2;
	px1 = e1->Nodes[0]->x + mua * x21;
	py1 = e1->Nodes[0]->y + mua * y21;
	pz1 = e1->Nodes[0]->z + mua * z21;
	px2 = e2->Nodes[0]->x + mub * x43;
	py2 = e2->Nodes[0]->y + mub * y43;
	pz2 = e2->Nodes[0]->z + mub * z43;

	(e1->Nodes[0]->x > e1->Nodes[1]->x) 
			? (e1x1 = e1->Nodes[1]->x) 
			: (e1x2 = e1->Nodes[0]->x);
	(e1->Nodes[0]->y > e1->Nodes[1]->y) 
			? (e1y1 = e1->Nodes[1]->y) 
			: (e1y2 = e1->Nodes[0]->z);
	(e1->Nodes[0]->z > e1->Nodes[1]->z) 
			? (e1z1 = e1->Nodes[1]->y) 
			: (e1z2 = e1->Nodes[0]->z);
	(e2->Nodes[0]->x > e2->Nodes[1]->x) 
			? (e2x1 = e2->Nodes[1]->x) 
			: (e2x2 = e2->Nodes[0]->x);
	(e2->Nodes[0]->y > e2->Nodes[1]->y) 
			? (e2y1 = e2->Nodes[1]->y) 
			: (e2y2 = e2->Nodes[0]->z);
	(e2->Nodes[0]->z > e2->Nodes[1]->z) 
			? (e2z1 = e2->Nodes[1]->y) 
			: (e2z2 = e2->Nodes[0]->z);

	if ( fabs(px1-px2) == EPS && fabs(py1-py2) == EPS 
		&& fabs(pz1-pz2) == EPS && px1 > e1x1 && py1 > e1y1 
		&& pz1 > e1z1 && px1 < e1x2 && py1 < e1y2 && pz1 < e1z2 
		&& px1 > e2x1 && py1 > e2y1 && pz1 > e2z1 && px1 < e2x2 
		&& py1 < e2y2 && pz1 < e2z2)
	{
		 return true;
	}

	return false;
}

/* Creates and adds a node to the Nodes vector */
AdaptiveMesher::PNODE 
AdaptiveMesher :: CreateTetraNode (vector<PNODE>& Nodes, 
						REAL x, REAL y, REAL z, bool addToList)
{
	DBG_MSG ("AdaptiveMesher :: CreateTetraNode Entered");

	for (unsigned int i = 0; i < Nodes.size(); i++)
	{
		if (Nodes[i]->x == x && Nodes[i]->y == y 
				    && Nodes[i]->z == z)
		{
			DBG_MSG ("AdaptiveMesher :: CreateTetraNode Leaving");
			return Nodes[i];
		}
	}
	
	PNODE node = new (nothrow) NODE;
	if (node == NULL)
		ERR_MSG ("ERROR: Not enough memory");
	
	node->x = x;
	node->y = y;
	node->z = z;

	if (addToList)
		Nodes.push_back (node);
	
	DBG_MSG ("AdaptiveMesher :: CreateTetraNode Leaving");

	return node;
}

/* creates and adds a new tetranode, if no node
 * is present at that location 
 * lNodes must not be constant, it will be modified 
 */
AdaptiveMesher::PNODE
AdaptiveMesher :: CreateTetraNode (vector<PNODE>&lNodes, PNODE node, bool addToList)
{
	DBG_MSG ("AdaptiveMesher :: CreateTetraNode Entered");

	PNODE n = NULL;

	for (unsigned int i = 0; i < lNodes.size(); i++)
	{	
		if (lNodes[i]->x == node->x && lNodes[i]->y == node->y 
				    && lNodes[i]->z == node->z)
		{
			DBG_MSG ("AdaptiveMesher :: CreateTetraNode Leaving");
			return lNodes[i];
		}
	}

	if (addToList)
	{
		lNodes.push_back (node);
		n = node;
	}
	else
	{
		n = new (nothrow) NODE;
		if (n == NULL)
			ERR_MSG ("ERROR: Not enough memory");
		
		n->x = node->x;
		n->y = node->y;
		n->z = node->z;
		lNodes.push_back (n);
	}

	DBG_MSG ("AdaptiveMesher :: CreateTetraNode Leaving");
	return n;
}

/* Creates and adds an edge to the specified Edges vector */
AdaptiveMesher::PEDGE
AdaptiveMesher :: CreateTetraEdge (vector<PEDGE>& Edges, 
							PNODE n1, PNODE n2, bool addToList)
{
	DBG_MSG ("AdaptiveMesher :: CreateTetraEdge Entered");

	int in;
	PEDGE edge = NULL;
	
	assert (n1 != n2);
	
	if ((in = GetEdgeIndex (Edges, n1, n2)) != -1)
	{
		edge = Edges[in];
	}
	else
	{
		edge = new (nothrow) EDGE;
		if (edge == NULL)
			ERR_MSG ("ERROR: Not enough memory");
		
		edge->Nodes[0] = n1;
		edge->Nodes[1] = n2;
		edge->Refine = false;

		if (addToList)
			Edges.push_back (edge);
	}
	
	DBG_MSG ("AdaptiveMesher :: CreateTetraEdge Leaving");
	return edge;
}

AdaptiveMesher::PEDGE
AdaptiveMesher :: CreateTetraEdge (vector<PEDGE>& Edges, PEDGE edge)
{
	return CreateTetraEdge (Edges, edge->Nodes[0], edge->Nodes[1]);
}

/* Creates edges from the nodes */
AdaptiveMesher::PEDGE* 
AdaptiveMesher :: CreateEdgesFromNodes (vector<PEDGE>& Edges, PNODE* nodes)
{
	DBG_MSG ("AdaptiveMesher :: CreateEdgesFromNodes Entered");
	
	assert (nodes != NULL);
	assert (nodes[0] != NULL);
	assert (nodes[1] != NULL);
	assert (nodes[2] != NULL);
	assert (nodes[3] != NULL);

	PEDGE* edges = new (nothrow) PEDGE[EDGES_PER_ELEMENT];
	if (edges == NULL)
		ERR_MSG ("ERROR: Not enough memory");
	
	for (int j = 0; j < EDGES_PER_ELEMENT; j++)
	{
		if (j < 4)
		{
			edges[j] = CreateTetraEdge (
						Edges,
						nodes[j % NODES_PER_ELEMENT],
						nodes[(j+1) % NODES_PER_ELEMENT]
			);

		}
		else
		{
			edges[j] = CreateTetraEdge (
						Edges,
						nodes[j % NODES_PER_ELEMENT],
						nodes[(j+2) % NODES_PER_ELEMENT]
			);
		}
	}
	
	DBG_MSG ("AdaptiveMesher :: CreateEdgesFromNodes Leaving");
	return edges;
}

// AdaptiveMesher class member function implementation ends
/////////////////////////////////////////////////////////////////////


