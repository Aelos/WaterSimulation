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
#include "../../utils/StopWatch.h"


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
	Mesh3D m_water;
	Vector4 waterColor;
	Mesh3D m_skybox;
	GLuint CubMapTextureID;
	StopWatch watch;
	float startingTime;
	float currentTime;
	int sceneSize;

	float amplitude1;
	float wavelength1;
	float speed1;
	Vector2 direction1;

	float amplitude2;
	float wavelength2;
	float speed2;
	Vector2 direction2;

	float amplitude3;
	float wavelength3;
	float speed3;
	Vector2 direction3;

	float amplitude4;
	float wavelength4;
	float speed4;
	Vector2 direction4;

	int time4;
	bool isRadial;
	Mesh3D* createPlane();
	Mesh3D* createCube();
	void generateCubeMap();
	void idle();
	void draw_scene(DrawMode _draw_mode);
	void draw_skybox();
	void draw_water();
};



#endif