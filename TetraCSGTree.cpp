#include <iostream>

#include "TetraCSGTree.h"

/////////////////////////////////////////////////////////////////////
// TetraCSGTree member functions implementation begins

TetraCSGTree :: TetraCSGTree ()
{
	root = NULL;
}

TetraCSGTree :: ~TetraCSGTree ()
{
}

void
TetraCSGTree :: Insert (TetraCSGPrimitive* prim, TetraCSGBoolOper *oper)
{
	if (root == NULL)
		root = prim;
	else
	{
		oper->SetRightChild (prim);
		oper->SetLeftChild (root);
		
		root = oper;
	}
}

void
TetraCSGTree :: Remove (TetraCSGPrimitive* prim)
{
	TetraCSGBoolOper *parent = NULL;
	TetraCSGItem *cnode = root;
	
	while (cnode != NULL)
	{
		if (cnode == prim)
		{
			root = NULL;
			delete prim;
			return;
		}
		/* if it is a bool operation, check its
		left and right child */
		else if (cnode->GetType () | BOOL_OPER)
		{
			TetraCSGBoolOper *temp = dynamic_cast<TetraCSGBoolOper*> (cnode);
			if (temp->GetRightChild () == prim)
			{
				if (parent == NULL)
					root = temp->GetLeftChild ();
				else
					parent->SetRightChild (temp->GetLeftChild());
				delete prim;
				delete temp;
				return;
			}
			else if (temp->GetLeftChild () == prim)
			{
				if (parent == NULL)
					root = temp->GetRightChild ();
				else
					parent->SetLeftChild (temp->GetRightChild ());
				delete prim;
				delete temp;
				return;
			}

			cnode = temp->GetLeftChild ();
			parent = temp;
		}
	}
}

void
TetraCSGTree :: Modify (TetraCSGPrimitive* prim, TetraCSGBoolOper* oper)
{
	TetraCSGItem *cnode;
	TetraCSGBoolOper *parent = NULL;
	
	cnode = root;
	while (cnode != NULL)
	{
		if (cnode->GetType () | BOOL_OPER)
		{
			TetraCSGBoolOper *temp = (TetraCSGBoolOper*)cnode;
			if (temp->GetRightChild () == prim)
			{
				if (parent == NULL)
					root = oper;
				else
					parent->SetRightChild (oper);
				oper->SetLeftChild (temp->GetLeftChild ());
				oper->SetRightChild (temp->GetRightChild ());

				/*delete temp;*/
				break;
			}

			cnode = temp->GetLeftChild ();
			parent = temp;
		}
	}	
}

// TetraCSGTree member functions implementation ends
/////////////////////////////////////////////////////////////////////

