//=============================================================================
//                                                                            
//   Exercise code for Introduction to Computer Graphics
//   LGG - EPFL
//   Thibaut Weise, Yuliy Schwartzburg
//                                                                            
//=============================================================================

#include "WaterRenderer.h"

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	
	WaterRenderer window("Water Viewer", 800, 800);
	
	glutMainLoop();
}
