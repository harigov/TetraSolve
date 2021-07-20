CC=g++
CXXOPTS=-c `wx-config --cxxflags` -I /usr/X11R6/include 
LINK=g++
LINKOPTS=`wx-config --libs ` -L foldbar/foldbar.so -L symex/symex.so -L tetgen/tetgen.so
OBJ_FILES=AdaptiveMesher.o				\
					AddDisplacementDialog.o \
					AddElementDialog.o			\
					AddForceDialog.o				\
					AddMaterialDialog.o			\
					AddMPConstraintDialog.o \
					AddNodeDialog.o					\
					Debug.o									\
					HelpAboutDialog.o				\
					TetgenInterface.o				\
					TetraCSGModel.o					\
					TetraCSGPrimitive.o			\
					TetraCSGTree.o					\
					TetraGLCanvas.o					\
					TetraGUI.o							

TetraGUI:$(OBJ_FILES)
	$(LINK) $(LINKOPTS) $(OBJ_FILES) -o TetraGUI

AdaptiveMesher.o:AdaptiveMesher.cpp AdaptiveMesher.h
	$(CC) $(CXXOPTS) AdaptiveMesher.cpp -o AdaptiveMesher.o

AddDisplacementDialog.o:AddDisplacementDialog.cpp AddDisplacementDialog.h
	$(CC) $(CXXOPTS) AddDisplacementDialog.cpp -o AddDisplacementDialog.o

AddElementDialog.o: AddElementDialog.cpp AddElementDialog.h
	$(CC) $(CXXOPTS) AddElementDialog.cpp -o AddElementDialog.o

AddForceDialog.o:AddForceDialog.cpp AddForceDialog.h
	$(CC) $(CXXOPTS) AddForceDialog.cpp -o AddForceDialog.o

AddMaterialDialog.o:AddMaterialDialog.cpp AddMaterialDialog.h
	$(CC) $(CXXOPTS) AddMaterialDialog.cpp -o AddMaterialDialog.o

AddMPConstraintDialog.o:AddMPConstraintDialog.cpp AddMPConstraintDialog.h
	$(CC) $(CXXOPTS) AddMPConstraintDialog.cpp -o AddMPConstraintDialog.o

AddNodeDialog.o:AddNodeDialog.cpp AddNodeDialog.h
	$(CC) $(CXXOPTS) AddNodeDialog.cpp -o AddNodeDialog.o

Debug.o:Debug.cpp Debug.h
	$(CC) $(CXXOPTS) Debug.cpp -o Debug.o

HelpAboutDialog.o:HelpAboutDialog.cpp HelpAboutDialog.h
	$(CC) $(CXXOPTS) HelpAboutDialog.cpp -o HelpAboutDialog.o

TetgenInterface.o:TetgenInterface.cpp TetgenInterface.h
	$(CC) $(CXXOPTS) TetgenInterface.cpp -o TetgenInterface.o

TetraCSGModel.o:TetraCSGModel.cpp TetraCSGModel.h
	$(CC) $(CXXOPTS) TetraCSGModel.cpp -o TetraCSGModel.o

TetraCSGPrimitive.o:TetraCSGPrimitive.cpp TetraCSGPrimitive.h
	$(CC) $(CXXOPTS) TetraCSGPrimitive.cpp -o TetraCSGPrimitive.o

TetraCSGTree.o:TetraCSGTree.cpp TetraCSGTree.h
	$(CC) $(CXXOPTS) TetraCSGTree.cpp -o TetraCSGTree.o

TetraGLCanvas.o:TetraGLCanvas.cpp TetraGLCanvas.h
	$(CC) $(CXXOPTS) TetraGLCanvas.cpp -o TetraGLCanvas.o

TetraGUI.o:TetraGUI.cpp TetraGUI.h
	$(CC) $(CXXOPTS) TetraGUI.cpp -o TetraGUI.o

Main.o:Main.cpp
	$(CC) $(CXXOPTS) Main.cpp -o Main.o

clean:
	rm -rf $(OBJ_FILES) TetraGUI
