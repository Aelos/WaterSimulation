#include "WaterRenderer.h"

WaterRenderer::
WaterRenderer(const char* _title, int _width, int _height)
: TrackballViewer(_title, _width, _height)
{
    // initialize meshes
    init();
}

WaterRenderer::
~WaterRenderer()
{
	m_water.clear();
	m_skybox.clear();
}

void 
WaterRenderer::
init()
{
    // initialize parent
    TrackballViewer::init();

	m_water = *createPlane();
	m_skybox = *createCube();
	
	watch.start();
	startingTime = watch.stop();

	// set camera to look at world coordinate center
    sceneSize = 4;
	set_scene_pos(Vector3(0.0, 0.0, 0.0), sceneSize);
	
	generateCubeMap();
	
	// load shaders
	m_skyShader.create("skybox.vs", "skybox.fs");
	m_waterShader.create("water.vs", "water.fs");
}

void
WaterRenderer::
keyboard(int key, int x, int y)
{
	double scale = 0.01;
	
	switch (key)
	{			
		case '-':
			// reset camera position
			if(sceneSize>1) {
				sceneSize*=0.5;
				m_skybox.scaleObject(Vector3 (0.5,0.5,0.5));
				m_water.scaleObject(Vector3 (0.5,0.5,0.5));
				m_camera.setIdentity();
				set_scene_pos(Vector3(0.0, 0.0, 0.0), sceneSize);
			}
			break;

		case '+':
			sceneSize*=2;
			m_skybox.scaleObject(Vector3 (2.0,2.0,2.0));
			m_water.scaleObject(Vector3 (2.0,2.0,2.0));
			// reset camera position
			m_camera.setIdentity();
			set_scene_pos(Vector3(0.0, 0.0, 0.0), sceneSize);
			break;

		default:
			TrackballViewer::keyboard(key, x, y);
			break;
	}
	
	glutPostRedisplay();
}

void
WaterRenderer::
special(int key, int x, int y)
{
	TrackballViewer::special(key, x, y);	
	glutPostRedisplay();
}

Mesh3D* 
WaterRenderer::
createPlane()
{
    // initialize Mesh3D
    Mesh3D *plane = new Mesh3D();
    
	std::vector< Vector3 > planeVertices;
	std::vector< unsigned int > planeIndices;
	std::vector< Vector3 > planeNormals;

	float d = 50;
	int n=300;
    
    float a=2*d/(n-1);
    float b=2*d/(n-1);
	for(int i=0;i<n;i++){
		for(int j=0;j<n;j++){
			planeVertices.push_back(Vector3(-(d)+i*b, 0,-(d)+j*a));
		}
	}
	
	for(int i=0;i<n-1;i++){
		for(int j=0;j<n-1;j++){
			
			planeIndices.push_back(n*i+j+1);
			planeIndices.push_back(n*i+j);
			planeIndices.push_back(n*(i+1)+j);
			planeIndices.push_back(n*i+j+1);
			planeIndices.push_back(n*(i+1)+j);
			planeIndices.push_back(n*(i+1)+j+1);

			
		}
	}

	for(int k = 0; k < n*n; k++) planeNormals.push_back(Vector3(0,1,0));

	plane->setIndices(planeIndices);
	plane->setVertexPositions(planeVertices);
	plane->setVertexNormals(planeNormals);

    return plane;
}


Mesh3D* 
WaterRenderer::
createCube()
{
    // initialize Mesh3D
    Mesh3D *cube = new Mesh3D();
    
	// setup uniform cube with side length 32 and center of cube being (0,0,0)
	std::vector< Vector3 > cubeVertices;
	std::vector< unsigned int > cubeIndices;
	float d = 16;
    
	
	// front
	cubeVertices.push_back(Vector3(-d,-d, d));
	cubeVertices.push_back(Vector3( d,-d, d));
	cubeVertices.push_back(Vector3( d, d, d));
	cubeVertices.push_back(Vector3(-d, d, d));
	cubeIndices.push_back(0);
	cubeIndices.push_back(1);
	cubeIndices.push_back(2);
	cubeIndices.push_back(0);
	cubeIndices.push_back(2);
	cubeIndices.push_back(3);
	
	
	// right
	cubeVertices.push_back(Vector3( d,-d,-d));
	cubeVertices.push_back(Vector3( d,-d, d));
	cubeVertices.push_back(Vector3( d, d, d));
	cubeVertices.push_back(Vector3( d, d,-d));
	cubeIndices.push_back(4);
	cubeIndices.push_back(5);
	cubeIndices.push_back(6);
	cubeIndices.push_back(4);
	cubeIndices.push_back(6);
	cubeIndices.push_back(7);
	
	
	// back
	cubeVertices.push_back(Vector3( d,-d,-d));
	cubeVertices.push_back(Vector3(-d,-d,-d));
	cubeVertices.push_back(Vector3(-d, d,-d));
	cubeVertices.push_back(Vector3( d, d,-d));
	cubeIndices.push_back(8);
	cubeIndices.push_back(9);
	cubeIndices.push_back(10);
	cubeIndices.push_back(8);
	cubeIndices.push_back(10);
	cubeIndices.push_back(11);
	
    
	// left
	cubeVertices.push_back(Vector3(-d,-d, d));
	cubeVertices.push_back(Vector3(-d,-d,-d));
	cubeVertices.push_back(Vector3(-d, d,-d));
	cubeVertices.push_back(Vector3(-d, d, d));
	cubeIndices.push_back(12);
	cubeIndices.push_back(13);
	cubeIndices.push_back(14);
	cubeIndices.push_back(12);
	cubeIndices.push_back(14);
	cubeIndices.push_back(15);
	
    
	// top
	cubeVertices.push_back(Vector3(-d, d,-d));
	cubeVertices.push_back(Vector3( d, d,-d));
	cubeVertices.push_back(Vector3( d, d, d));
	cubeVertices.push_back(Vector3(-d, d, d));
	cubeIndices.push_back(16);
	cubeIndices.push_back(17);
	cubeIndices.push_back(18);
	cubeIndices.push_back(16);
	cubeIndices.push_back(18);
	cubeIndices.push_back(19);
	
    
	// bottom
	cubeVertices.push_back(Vector3( d,-d,-d));
	cubeVertices.push_back(Vector3(-d,-d,-d));
	cubeVertices.push_back(Vector3(-d,-d, d));
	cubeVertices.push_back(Vector3( d,-d, d));
	cubeIndices.push_back(20);
	cubeIndices.push_back(21);
	cubeIndices.push_back(22);
	cubeIndices.push_back(20);
	cubeIndices.push_back(22);
	cubeIndices.push_back(23);
	
	
	cube->setIndices(cubeIndices);
	cube->setVertexPositions(cubeVertices);
	
    return cube;
}

void
WaterRenderer::
generateCubeMap() {
	GLenum CubeMapTarget[6] = {           // Liste des targets pour la création des textures de CubeMap
		GL_TEXTURE_CUBE_MAP_POSITIVE_X,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_X,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Y,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Y,
		GL_TEXTURE_CUBE_MAP_POSITIVE_Z,
		GL_TEXTURE_CUBE_MAP_NEGATIVE_Z,
	};

	char CubeMapFileName[6][30]={	       // Liste des noms de fichiers des textures
		"textures/PositiveX.bmp",
		"textures/NegativeX.bmp",
		"textures/PositiveY.bmp",
		"textures/NegativeY.bmp",
		"textures/PositiveZ.bmp",
		"textures/NegativeZ.bmp"
	};

	char CubeMapFileName2[6][30]={	       // Liste des noms de fichiers des textures
		"textures/lostvalley_north.bmp",
		"textures/lostvalley_south.bmp",
		"textures/lostvalley_up.bmp",
		"textures/lostvalley_down.bmp",
		"textures/lostvalley_east.bmp",
		"textures/lostvalley_west.bmp"
	};

	
	//Chargement en mémoire des textures
	
	// On génère un ID de texture unique pour les 6 textures du CubeMap
	glGenTextures(1, &CubMapTextureID); 				    
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubMapTextureID);

	int i=0;
	for (i = 0; i < 6; i++) {
		Bitmap * img = new Bitmap();
		if (img -> loadBMP(CubeMapFileName2[i])) {
			glTexImage2D(CubeMapTarget[i], 0, GL_RGB, img->width, img->height, 0, GL_RGB, GL_UNSIGNED_BYTE, img->data);
		} 
	}

	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP);
}

//-----------------------------------------------------------------------------

void
WaterRenderer::
idle() {
	glutPostRedisplay();
}


//Draws the whole scene
void 
WaterRenderer::
draw_scene(DrawMode _draw_mode)
{ 
	// clear screen
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glDisable(GL_CULL_FACE);
	glEnable(GL_MULTISAMPLE);

	glDisable(GL_DEPTH_TEST);
	draw_skybox();

	glEnable(GL_DEPTH_TEST);
	draw_water();
}

void
WaterRenderer::
draw_skybox() {

	m_skyShader.bind(); 
	
	// set parameters to send to the shader
	m_skyShader.setMatrix4x4Uniform("modelworld", m_skybox.getTransformation());
	m_skyShader.setMatrix4x4Uniform("worldcamera", m_camera.getTransformation().Inverse());
	m_skyShader.setMatrix4x4Uniform("projection", m_camera.getProjectionMatrix());
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubMapTextureID);
	m_skyShader.setIntUniform("cubemap", 0);

	glEnableClientState(GL_VERTEX_ARRAY);
	glVertexPointer( 3, GL_DOUBLE, 0, m_skybox.getVertexPointer() );

	for(unsigned int i = 0; i < m_skybox.getNumberOfParts(); i++)
	{
		glDrawElements( GL_TRIANGLES, m_skybox.getNumberOfFaces(i)*3, GL_UNSIGNED_INT, m_skybox.getVertexIndicesPointer(i) );
	}
	
	glDisableClientState(GL_VERTEX_ARRAY);

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	
	m_skyShader.unbind();

}

void::
WaterRenderer::
draw_water() {

	glEnable(GL_TEXTURE_GEN_S);
	glEnable(GL_TEXTURE_GEN_T);
	glEnable(GL_TEXTURE_GEN_R);
	glEnable(GL_TEXTURE_CUBE_MAP);
	glEnable(GL_NORMALIZE);

	glEnableClientState(GL_VERTEX_ARRAY);
	
	m_waterShader.bind();

	glVertexPointer( 3, GL_DOUBLE, 0, m_water.getVertexPointer() );
	
	//set the shader parameters

	m_waterShader.setFloatUniform("waterHeight", -2*sceneSize);
    
	currentTime = watch.stop();
	m_waterShader.setFloatUniform("time", (currentTime - startingTime)/1000 );

	float amplitude1 = 0.03;
	float wavelength1 = 0.5 * M_PI;
	float speed1 = 3.0f;
	Vector2 direction1 = (1, 1);

	float amplitude2 = 0.6;
	float wavelength2 = 4.0 * M_PI;
	float speed2 = 3.0f;
	Vector2 direction2 = (1, 1);

	float amplitude3 = 0.3;
	float wavelength3 = 10.0 * M_PI;
	float speed3 = 3.0f;
	Vector2 direction3 = (1, 1);

	m_waterShader.setFloatUniform("amplitude1", amplitude1);
	m_waterShader.setFloatUniform("wavelength1", wavelength1);
	m_waterShader.setFloatUniform("speed1", speed1);
	m_waterShader.setVector2Uniform("direction1", direction1.x, direction1.y);

	m_waterShader.setFloatUniform("amplitude2", amplitude2);
	m_waterShader.setFloatUniform("wavelength2", wavelength2);
	m_waterShader.setFloatUniform("speed2", speed2);
	m_waterShader.setVector2Uniform("direction2", direction2.x, direction2.y);
	
	m_waterShader.setFloatUniform("amplitude3", amplitude3);
	m_waterShader.setFloatUniform("wavelength3", wavelength3);
	m_waterShader.setFloatUniform("speed3", speed3);
	m_waterShader.setVector2Uniform("direction3", direction3.x, direction3.y);

	m_waterShader.setMatrix4x4Uniform("worldcamera", m_camera.getTransformation().Inverse());
	m_waterShader.setMatrix4x4Uniform("projection", m_camera.getProjectionMatrix());
	
	m_waterShader.setMatrix4x4Uniform("modelworld", m_water.getTransformation() );
	
	m_waterShader.setVector3Uniform("eyePos", m_camera.origin().x, m_camera.origin().y, m_camera.origin().z);
	m_waterShader.setFloatUniform("fresnelBias", 0);
	m_waterShader.setFloatUniform("fresnelScale", 2);
	m_waterShader.setFloatUniform("fresnelPower", 1);
	m_waterShader.setFloatUniform("etaRatio", 0.66);

	glBindTexture(GL_TEXTURE_CUBE_MAP, CubMapTextureID);
	m_waterShader.setIntUniform("env", 0);
	
	glDrawElements( GL_TRIANGLES, m_water.getNumberOfFaces()*3, GL_UNSIGNED_INT, m_water.getVertexIndicesPointer() );

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	m_waterShader.unbind();

	glDisableClientState(GL_VERTEX_ARRAY);
	
	glDisable(GL_TEXTURE_CUBE_MAP);
	glDisable(GL_NORMALIZE);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
}