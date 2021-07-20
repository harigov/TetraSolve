
/////////////////////////////////////////////////////////////////////
// Standard include file
#include "TetraCSGModel.h"
#include "TetgenInterface.h"

/////////////////////////////////////////////////////////////////////
// User defined include file


/////////////////////////////////////////////////////////////////////
// TetraCSGModel class member function declaration begins

TetraCSGModel :: TetraCSGModel ()
{
	tree = new TetraCSGTree ();;
}

TetraCSGModel :: TetraCSGModel (const char *fname)
{
	OpenFromFile (fname);
}

TetraCSGModel :: ~TetraCSGModel ()
{
	delete tree;
}

void
TetraCSGModel :: Render ()
{
	if (tree->root != NULL)
	{
		tree->root->GetObject ()->Render ();
	}
}

Tetrahedron<REAL>*
TetraCSGModel :: GetFEMModel ()
{
	return TetgenInterface ::GetFEMModel (tree->root->GetObject ());
}

bool
TetraCSGModel :: SaveToFile (const char *fname)
{
	FILE *fp;
	
	if (IsLoaded () == false)
		return false;

	fp = fopen (fname, "w");
	if (fp == NULL) 
		return false;

	fprintf (fp, "( ");
	RecursiveWrite (fp, tree->root);
	fprintf (fp, " )");
	fclose (fp);

	return true;
}

bool
TetraCSGModel :: OpenFromFile (const char *fname)
{
	symex_list_t *list;
	symex_list_node_t *node;
	FILE* fp;

	fp = fopen (fname, "r");
	if (fp == NULL) {
		wxLogMessage (wxT("ERROR: Unable to open file"));
		return false;
	}

	list = symex_parse_from_file (fp);
	if (list == NULL) 
	{
		fclose (fp);
		wxLogMessage (wxT("ERROR: Unable to parse file"));
		return false;
	}

	node = list->fnode;

	if (node->type == SYMEX_LIST)
	{
		node = (symex_list_node_t*)(node->data);
		tree->root = RecursiveParse (node);
		if (tree->root != NULL)
			return true;
	}

	symex_list_destroy (list);

	return false;
}

TetraCSGItem*
TetraCSGModel :: RecursiveParse (symex_list_node_t *node)
{
	/* first four letters of union
	intersect and difference, used as
	flags */
	int unio = 1, inte = 1, diff = 1;

	assert (node != NULL);

	if (node->type != SYMEX_BYTES) return NULL;

	if (strcmp (static_cast<const char*>(node->data), 
				"parallelopiped") == 0)
	{
		if (node->next == NULL || node->next->data == NULL ||
			node->next->type != SYMEX_LIST) 
			return NULL;

		node = static_cast<symex_list_node_t*>(node->next->data);
		
		double x, y, z, w, h, d;
		
		if (node->type != SYMEX_BYTES) return NULL;
		sscanf (static_cast<const char*>(node->data), "%lf", &x);
		
		if (node->next == NULL ||
			node->next->type != SYMEX_BYTES) 
			return NULL;
		node = node->next;
		sscanf (static_cast<const char*>(node->data), "%lf", &y);
		
		if (node->next == NULL ||
			node->next->type != SYMEX_BYTES) 
			return NULL;
		node = node->next;
		sscanf (static_cast<const char*>(node->data), "%lf", &z);
		
		if (node->next == NULL ||
			node->next->type != SYMEX_BYTES) 
			return NULL;
		node = node->next;
		sscanf (static_cast<const char*>(node->data), "%lf", &w);

		if (node->next == NULL ||
			node->next->type != SYMEX_BYTES) 
			return NULL;
		node = node->next;
		sscanf (static_cast<const char*>(node->data), "%lf", &h);
		
		if (node->next == NULL ||
			node->next->type != SYMEX_BYTES) 
			return NULL;
		node = node->next;
		sscanf (static_cast<const char*>(node->data), "%lf", &d);

		return new TetraCSGParallelopiped (x, y, z, w, h, d);
	}
	else if (strcmp (static_cast<const char*>(node->data), "sphere") == 0)
	{
		if (node->next == NULL || node->next->data == NULL ||
			node->next->type != SYMEX_LIST) 
			return NULL;

		node = static_cast<symex_list_node_t*>(node->next->data);
		
		double x, y, z, r;
		
		if (node->type != SYMEX_BYTES) return NULL;
		sscanf (static_cast<const char*>(node->data), "%lf", &x);
		
		if (node->next == NULL ||
			node->next->type != SYMEX_BYTES) 
			return NULL;
		node = node->next;
		sscanf (static_cast<const char*>(node->data), "%lf", &y);
		
		if (node->next == NULL ||
			node->next->type != SYMEX_BYTES) 
			return NULL;
		node = node->next;
		sscanf (static_cast<const char*>(node->data), "%lf", &z);
		
		if (node->next == NULL ||
			node->next->type != SYMEX_BYTES) 
			return NULL;
		node = node->next;
		sscanf (static_cast<const char*>(node->data), "%lf", &r);

		return new TetraCSGSphere (x, y, z, r);

	}
	else if (strcmp (static_cast<const char*>(node->data), "frustum") == 0)
	{
		if (node->next == NULL || node->next->data == NULL ||
			node->next->type != SYMEX_LIST) 
			return NULL;

		node = static_cast<symex_list_node_t*>(node->next->data);
		
		double x, y, z, ts, bs, l;
		
		if (node->type != SYMEX_BYTES) return NULL;
		sscanf (static_cast<const char*>(node->data), "%lf", &x);
		
		if (node->next == NULL ||
			node->next->type != SYMEX_BYTES) 
			return NULL;
		node = node->next;
		sscanf (static_cast<const char*>(node->data), "%lf", &y);
		
		if (node->next == NULL ||
			node->next->type != SYMEX_BYTES) 
			return NULL;
		node = node->next;
		sscanf (static_cast<const char*>(node->data), "%lf", &z);
		
		if (node->next == NULL ||
			node->next->type != SYMEX_BYTES) 
			return NULL;
		node = node->next;
		sscanf (static_cast<const char*>(node->data), "%lf", &ts);

		if (node->next == NULL ||
			node->next->type != SYMEX_BYTES) 
			return NULL;
		node = node->next;
		sscanf (static_cast<const char*>(node->data), "%lf", &bs);
		
		if (node->next == NULL ||
			node->next->type != SYMEX_BYTES) 
			return NULL;
		node = node->next;
		sscanf (static_cast<const char*>(node->data), "%lf", &l);

		return new TetraCSGFrustum (x, y, z, ts, bs, l);
	}
	else if (strcmp (static_cast<const char*>(node->data), "cylinder") == 0)
	{
		if (node->next == NULL || node->next->data == NULL ||
			node->next->type != SYMEX_LIST) 
			return NULL;
		node = static_cast<symex_list_node_t*>(node->next->data);
		
		double x, y, z, r, h;
		
		if (node->type != SYMEX_BYTES) return NULL;
		sscanf (static_cast<const char*>(node->data), "%lf", &x);
		
		if (node->next == NULL ||
			node->next->type != SYMEX_BYTES) 
			return NULL;
		node = node->next;
		sscanf (static_cast<const char*>(node->data), "%lf", &y);
		
		if (node->next == NULL ||
			node->next->type != SYMEX_BYTES) 
			return NULL;
		node = node->next;
		sscanf (static_cast<const char*>(node->data), "%lf", &z);
		
		if (node->next == NULL ||
			node->next->type != SYMEX_BYTES) 
			return NULL;
		node = node->next;
		sscanf (static_cast<const char*>(node->data), "%lf", &r);

		if (node->next == NULL ||
			node->next->type != SYMEX_BYTES) 
			return NULL;
		node = node->next;
		sscanf (static_cast<const char*>(node->data), "%lf", &h);
		
		return new TetraCSGCylinder (x, y, z, r, h);
	}
	else if ((unio = strcmp (static_cast<const char*>(node->data), "union")) == 0 ||
			(inte = strcmp (static_cast<const char*>(node->data), "intersect")) == 0 ||
			(diff = strcmp (static_cast<const char*>(node->data), "difference")) == 0)
	{
		if (node->next == NULL || node->next->data == NULL ||
			node->next->type != SYMEX_LIST) 
			return NULL;

		TetraCSGItem *tmpnode;
		TetraCSGBoolOper *curnode;

		if (!unio)
			curnode = new TetraCSGUnion ();
		else if (!inte)
			curnode = new TetraCSGIntersect ();
		else if (!diff)
			curnode = new TetraCSGDifference ();

		if (curnode == NULL) 
			return NULL;

		node = node->next;
		if (node->data == NULL) 
			return NULL;
		tmpnode = RecursiveParse (static_cast<symex_list_node_t*>(node->data));
		if (tmpnode == NULL) {
			delete curnode;
			return NULL;
		}
		curnode->SetLeftChild (tmpnode);

		if ((node = static_cast<symex_list_node_t*>(node->data)->next) != NULL && 
			static_cast<symex_list_node_t*>(node)->next != NULL)
			node = node->next;
		tmpnode = RecursiveParse (static_cast<symex_list_node_t*>(node));
		if (tmpnode == NULL) {
			delete curnode;
			return NULL;
		}
		curnode->SetRightChild (tmpnode);

		if (node->next->next != NULL)
			wxLogMessage (wxT("Illegal format of file. Ignoring some elements."));

		return curnode;
	}
	else
	{
		wxLogMessage (wxT("ERROR: Unknown token found"));
		return NULL;
	}
}

void
TetraCSGModel :: RecursiveWrite (FILE *fp, TetraCSGItem *item)
{
	assert (fp != NULL);
	assert (item != NULL);

	TetraCSGBoolOper *oper = NULL;
	TetraCSGPrimitive *prim = NULL;

	if (item->GetType () | BOOL_OPER)
		oper = dynamic_cast<TetraCSGBoolOper*> (item);
	else if (item->GetType () | PRIMITIVE)
		prim = dynamic_cast<TetraCSGPrimitive*> (item);

	switch (item->GetType ())
	{
	case BOOL_UNION:
		fprintf (fp, "union (");
		break;
	case BOOL_INTERSECT:
		fprintf (fp, "intersect (");
		break;
	case BOOL_DIFFERENCE:
		fprintf (fp, "difference (");
		break;
	case PRIM_PARALLELOPIPED:
		fprintf (fp, "parallelopiped (%.3f %.3f %.3f %.3f %.3f %.3f) ",
					prim->GetX(), prim->GetY(), prim->GetZ(),	
					((TetraCSGParallelopiped*)prim)->GetWidth (),
					((TetraCSGParallelopiped*)prim)->GetHeight (),
					((TetraCSGParallelopiped*)prim)->GetDepth ()
				);
		break;
	case PRIM_SPHERE:
		fprintf (fp, "sphere (%.3f %.3f %.3f %.3f) ",
					prim->GetX(), prim->GetY(), prim->GetZ(),	
					((TetraCSGSphere*)prim)->GetRadius ()
				);
		break;
	case PRIM_FRUSTUM:
		fprintf (fp, "frustum (%.3f %.3f %.3f %.3f %.3f %.3f) ",
					prim->GetX(), prim->GetY(), prim->GetZ(),	
					((TetraCSGFrustum*)prim)->GetTopSide (),
					((TetraCSGFrustum*)prim)->GetBottomSide (),
					((TetraCSGFrustum*)prim)->GetLength ()
				);
		break;
	case PRIM_CYLINDER:
		fprintf (fp, "cylinder (%.3f %.3f %.3f %.3f %.3f %.3f) ",
					prim->GetX(), prim->GetY(), prim->GetZ(),	
					((TetraCSGCylinder*)prim)->GetRadius (),
					((TetraCSGCylinder*)prim)->GetLength ()
				);
		break;
	}

	if (item->GetType () | BOOL_OPER)
	{
		RecursiveWrite (fp, oper->GetLeftChild ());
		RecursiveWrite (fp, oper->GetRightChild ());
		fprintf (fp, " ) ");
	}
}

// TetraCSGModel class member functions declaration ends
/////////////////////////////////////////////////////////////////////
