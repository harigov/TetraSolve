#ifndef __TETRA_CSG_MODEL_H__
#define __TETRA_CSG_MODEL_H__

/////////////////////////////////////////////////////////////////////
// Standard include files
#include <cstdio>
#include <cstring>

using namespace std;

/////////////////////////////////////////////////////////////////////
// User defined include files
#include "TetraSolve.h"
#include "Tetrahedron.h"
#include "TetraCSGTree.h"
#include "symex/symex.h"

/////////////////////////////////////////////////////////////////////
// TetraCSGModel class declaration begins

/**
 * \brief TetraCSGModel class represents complete CSG
 * model.
 * When you are reading or writing a CSG model from 
 * a file, you actually treat it as a single model,
 * while actually the model is formed from simple 
 * primitives. This class handles all such issues as
 * reading/writing a CSG model from a file.
 */
class TetraCSGModel
{
private:
	/** tree stores the CSG model
	 * as a tree.
	 * \see TetraCSGTree
	 */
	TetraCSGTree *tree;
public:
	/** Default constructor */
	TetraCSGModel ();

	/** \param fname
	 * Name of the file from which CSG model
	 * data is to be read */
	TetraCSGModel (const char* fname);

	/** Destructor */
	~TetraCSGModel ();

	/* member functions */
	/** Renders the model by constructing
	 * triangles from the model data. The
	 * construction of triangles and clipping
	 * is done by CSGObject
	 * \see CSGObject */
	void				Render ();

	/** Returns the Finite element model generated
	 * from CSG model. */
	Tetrahedron<REAL>*	GetFEMModel ();

	/** Saves the CSG model into a file
	 * \param fname
	 * Name of the file to which the CSG model is to
	 * be written. It will not confirm whether to
	 * overwrite a file or not. 
	 * \return
	 * Success or failure is returned as a boolean 
	 * value. True means success. */
	bool				SaveToFile (const char* fname);

	/** Loads CSG model from a file 
	 * \param fname
	 * Name of the file, from which CSG model data is
	 * to be read. It is an error to pass NULL to this
	 * function
	 * \return
	 * Returns a boolean value. If true, it means model
	 * is successfully loaded.
	 */
	bool				OpenFromFile (const char* fname);

	/** Function to check whether a model is
	 * loaded into this CSGModel object or
	 * not
	 * \return
	 * Returns a boolean value signifying
	 * whether a model is loaded or not */
	bool				IsLoaded ()
	{
		return (tree != NULL && tree->root != NULL);
	};

	/** Getter function for the tree object
	 * \return
	 * Returns a pointer to the tree object.
	 * \warning
	 * Don't free or delete the object, else
	 * the software will segfault */
	TetraCSGTree*		GetTree () 
	{
		return tree;
	};

	/** Setter function for the tree object
	 * \param tree
	 * New model tree on which this model
	 * should operate on.
	 * \warning 
	 * Don't set NULL or any illegal value
	 * it will segfault the application.
	 * Value passed will not be checked */
	void	SetTree (TetraCSGTree* tree)
	{
		this->tree = tree;
	};
	
private:
	TetraCSGItem*		RecursiveParse (symex_list_node_t*);
	void				RecursiveWrite (FILE *fp, TetraCSGItem*);
};

// TetraCSGModel class declaration ends
/////////////////////////////////////////////////////////////////////

#endif /* __TETRA_CSG_MODEL_H__ */
