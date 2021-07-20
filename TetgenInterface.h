#ifndef __TETGEN_INTERFACE_H__
#define __TETGEN_INTERFACE_H__

#include <iostream>

#include "TetraSolve.h"
#include "Tetrahedron.h"
#include "TetraCSGPrimitive.h"

class TetgenInterface
{
	public:
		static Tetrahedron<REAL>* Tetrahedralize (Tetrahedron<REAL>*);
		static Tetrahedron<REAL>* GetFEMModel (CSGObject *object);
		
		static int	GetIndex (vector<CSGPoint>&, CSGPoint*);
};

#endif /* __TETGEN_INTERFACE__ */
