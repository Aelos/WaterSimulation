#ifndef MESH_VIEWER_HH
#define MESH_VIEWER_HH

#include <string>
#include <vector>

#include "../../gl/TrackballViewer.h"
#include "../../gl/shader.h"
#include "../../gl/Mesh3D.h"
#include "../../gl/Light3D.h"
#include "../../gl/fbo.h"
#include "../../utils/Mesh3DReader.h"
#include "../../utils/Bitmap.h"

class WaterRenderer : public TrackballViewer
{
public:
    WaterRenderer(const char* _title, int _width, int _height);
    ~WaterRenderer();
    
protected:
    Mesh3D *createWaterPlane();
	
protected:
    
	// overloaded GUI function
    virtual void init();
    virtual void keyboard(int key, int x, int y);
    virtual void special(int key, int x, int y);
     
protected:
	
	Shader m_skyShader;
	Shader m_waterShader;
	Light3D m_light;
	Vector4 lightColor;
	Mesh3D m_water;
	Mesh3D m_sky;
	GLuint CubMapTextureID;
	
	void load_water();
	void load_sky();
	void generateCubeMap();
	void draw_scene(DrawMode _draw_mode);
	void draw_sky();
	void draw_water();
};



#endif