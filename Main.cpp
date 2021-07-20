#include <iostream>
#include <cstdio>

#include "TetraGUI.h"
#include "Matrix.h"
#include "Tetrahedron.h"

using namespace std;

int main()
{
  TetraGUIApp tgApp;
  return 0;

	// Tetrahedron<double> *t = CreateTetrahedronFromFile<double> ("TETRA3D.INP");
	// PrintMembers (t);

	// cin.get ();
	// t->Solve ();

	// printf ("node#     x-displ      y-displ      z-displ\n");
	// for (int i = 0; i < t->nNodes; i++) 
	// {
	// 	printf(" %4d  %11.4e  %11.4e  %11.4e\n",i+1,t->Forces(0,3*i),
	// 								t->Forces(0,3*i+1),t->Forces(0,3*i+2));
	// }
	// 
	// cin.get();
	// delete t;
	// return 0;
}
