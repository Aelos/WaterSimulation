#ifndef WATERVIEWER_HH
#define WATERVIEWER_HH

#include <string>
#include <vector>

#include "../../gl/TrackballViewer.h"
#include "../../gl/shader.h"
#include "../../gl/Mesh3D.h"
#include "../../gl/Light3D.h"

class WaterRenderer : public TrackballViewer
{
public:
    WaterRenderer(const char* _title, int _width, int _height);
    ~WaterRenderer();
    
protected:
    Mesh3D *createPlane();
	
protected:
    
	// overloaded GUI function
    virtual void init();
    virtual void keyboard(int key, int x, int y);
    virtual void special(int key, int x, int y);
    
    virtual void draw_scene(DrawMode _draw_mode);
    
    
protected:
	
	Shader m_planeShader;
	Light3D m_light;
	
	// meshes
    std::vector< Mesh3D* > m_meshes;
	
	void draw_mesh(Mesh3D *mesh);
};



#endif