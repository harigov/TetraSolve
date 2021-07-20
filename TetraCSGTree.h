#ifndef __TETRA_CSG_TREE_H__
#define __TETRA_CSG_TREE_H__

/////////////////////////////////////////////////////////////////////
// Standard include files


/////////////////////////////////////////////////////////////////////
// User-Defined include files
#include "TetraCSGPrimitive.h"

/////////////////////////////////////////////////////////////////////
// TetraCSGTree class declaration begins

class TetraCSGTree
{
public:
	TetraCSGItem *root;

public:
	TetraCSGTree ();
	~TetraCSGTree ();

	void	Insert (TetraCSGPrimitive* prim, TetraCSGBoolOper *oper);
	void	Remove (TetraCSGPrimitive* prim);
	void	Modify (TetraCSGPrimitive* prim, TetraCSGBoolOper *oper);
};

// TetraCSGTree class declaration ends
/////////////////////////////////////////////////////////////////////

#endif /* __TETRA_CSG_TREE_H__ */
