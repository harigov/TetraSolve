# TetraSolve

TetraSolve is a finite element package, capable of solving 3D Tetrahedron elements, and plotting the results. The software comes with an easy to use GUI.

All operations, data required to perform operations are part of **Tetrahedron** class. A Tetrahedron element’s basic data comprises of following:

## Nodes

Nodes are the basic 3D points using which elements are formed. Nodes in Tetrahedron class are stored in “Nodes” vector. A point in 3D requires 3 coordinates, i.e., x, y, z (in Cartesian form) and therefore declared as a `Node` structure. `Nodes` vector stores `Node` objects which can then be accessed just like an array. For ex., Nodes [1] gives first node. (Unlike array, Nodes vector starts from 1, this is done because Elements stores the index of a node).

## Elements

Elements are the actual Tetrahedron elements and are formed from four nodes. It is represented by `Element` structure. An element in the tetrahedron program stores the indices of four nodes in the `Nodes` vector, and an index of material in `Materials` vector and a Temperature rise for this particular element. `Elements` vector stores the objects of `Element` structure.

## Materials

Material properties are required to solve the structural analysis problem. In this program, each element can have a different material selected. A vector `Materials` stores all the supported materials where each material is represented by `Material` structure. Each `Material` holds information about:

- Young’s Modulus `E`
- Nusselt’s Number `Nu` and 
- Thermal Conductivity `Alpha`

## Displacements

`Displacements` vector stores the displacement boundary conditions. Because boundary conditions are not given at every DOF, the software has to store both the value at a DOF, and the DOF itself. Therefore, displacement is represented using a `Displacement` structure, which contains both the `index` and `value` at any particular DOF. This Displacement vector is accessed for zero, just like C/C++ arrays. (Unless otherwise specified, all vectors in this program start from 1).

## Forces/Loads

Each DOF in a node can be subjected to load. By default, every DOF is subjected to 0 load. When you add a load `L` at DOF `d`, the corresponding DOFs (i.e., d) load is assigned a value of L. When you remove load at DOF d, then its load is reassigned as 0. `Forces` vector holds the load value at each DOF.

## Multipoint Constraints

When you want to constrain two DOFs such that displacements in both the DOF are interlinked with respect to some equation of the following form,

```
B1j+B2j+B3=0
```

you use multipoint constraints.

## Basic procedure employed to solve the structural analysis problem

A finite element problem is normally generalized to following simple equation.

```
KQ = F
```

Where
- K = Global Stiffness Matrix
- Q = Displacement vector
- F = Load Vector

The program attempts to implement this procedure for many tetrahedron elements. `K` matrix for an element can be determined using stress-strain matrix `D` and strain-displacement matrix `B`.  After finding the `D` matrix and `B` matrix, we determine the `K` matrix using the volume integral

```
Ke = ∫∫∫[B]TDB dV
```

After Stiffness matrix for an element is obtained, it is assembled to global stiffness matrix, which is then subjected to boundary conditions, and then the equation `KQ=F` is solved for displacements. After the displacements are obtained, then we substitute those displacement values in the same equation to obtain reaction forces. Stresses can be obtained using `D` matrix, which represents the stress-strain matrix.

```
Stresses = D * Strains
```

Because we know displacements, we can use the basic strength of materials relation `strain = change in length/ original length` to obtain strains. 

## Banded Matrix

Stiffness matrix is very large in size. Almost half of the matrix is filled with zeroes. We can reduce the memory required by using memory for only non-zero elements, which are mostly in the diagonal of the matrix. This width of diagonal with non-zero elements is called `bandwidth` of the matrix. A matrix which has got only elements from these diagonals is called Banded Matrix. Using this banded matrix, we can operate on a much larger number of Tetrahedron elements.

## Penalty Parameter

Elements in the banded matrix are small valued. Therefore, we need to multiply them with a suitable magnification factor to get good results. Penalty parameter is this multiplicative factor. Penalty parameter is determined based on the max valued element in the banded matrix and then added to banded matrix elements.

## Implementation of Adaptive Mesher

Adaptive mesher implemented in the software is based on Jurgen Bey’s Tetrahedron mesh refinement method (with small number of changes). The following algorithm explains the way mesher is programmed.:

- **Step 1:** The connectivity table of elements is not efficient way of representing a model for meshing. Therefore, we first convert the model into Boundary Representation model, with an exception that faces are not considered. Nodes, Edges and Elements are formed from connectivity table.

- **Step 2:** After the Boundary Representation model is formed, every element is checked for refinement criterion* and all the edges of elements requiring refinement are marked for refinement.

- **Step 3:** Each and every element of which there are at least one edge to be refined are subjected to meshing algorithm

- **Step 4:** All the meshed elements are added to a new Tetrahedron element. And the tetrahedron is returned to the main application.

*refinement criterion used presently is to select the highly stressed elements for meshing.


## TetraSolve GUI

The GUI program is designed to make TetraSolve easy to use. GUI is designed using [wxWidgets](www.wxwidgets.com). The Tetrahedron UI is designed to form a thin layer over the basic tetrahedron solving engine. In that, it supports addition and removal of nodes, elements, materials, etc., and also it allows list/plot them. We can have a contour plot of the stresses after solving the problem. Note that, before solving the problem, if you try to use it, it will not produce any image at all. If the image produced by it is garbled, please use view > replot to plot the image again. For now, there is no export as an image facility in the software. You have to use the basic window/screen printing facility provided by the OS. In windows, you use Print Screen key for that.

## What more can be added to the application?

- The application currently needs the user to add all nodes/elements manually. Instead a mesher can be implemented, which takes care of dividing elements/nodes for the sake of user. This can be an excellent addition to this application.
- Allowing a user to add nodes and elements graphically using point and click method makes it easier for people to solve their problems.
- Currently, when you try to remove a node, and if the node is part of an element, it does not delete that node, but displays an error message. It does the same for materials, forces, etc., It should be changed to ask user if he wants to delete the elements which are using this node, and react appropriately.
- The application could have been developed to “self heal”. That is, presently it exits whenever some error like memory allocation failure occurs. But it can be changed to a better method of waiting for some time and asking for memory, or using another memory manager. And the better option could be to use some sort of garbage collector.
- This application currently is developed so that it is easy to use, easy to maintain, and easy to understand code. And the code is NOT developed to be efficient in terms of speed. Although care is taken to see that it is efficient, but it is definitely slower than other codes, which are only developed to be fast and memory efficient. This should be changed so that the code is both efficient and easy to understand. 
