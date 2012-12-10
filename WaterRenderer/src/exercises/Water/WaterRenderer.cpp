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
	
	// put a light in the sky
	//m_light.translateWorld( Vector3(0,0,0) );
	//lightColor = Vector4(1, 1, 1, 1.0);

	watch.start();

	// set camera to look at world coordinate center
    set_scene_pos(Vector3(0.0, 0.0, 0.0), 2);
	
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
		case 'r':
			// reset camera position
			m_camera.setIdentity();
			set_scene_pos(Vector3(0.0, 0.0, 0.0), 5);
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
	
	float d = 2;
    
	planeVertices.push_back(Vector3( d, 0,-d));
	planeVertices.push_back(Vector3(-d, 0,-d));
	planeVertices.push_back(Vector3(-d, 0, d));
	planeVertices.push_back(Vector3( d, 0, d));
	planeIndices.push_back(0);
	planeIndices.push_back(1);
	planeIndices.push_back(2);
	planeIndices.push_back(0);
	planeIndices.push_back(2);
	planeIndices.push_back(3);
	for(int k = 0; k < 4; k++) planeNormals.push_back(Vector3(0,1,0));

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
    
	// setup uniform cube with side length 16 and center of cube being (0,0,0)
	std::vector< Vector3 > cubeVertices;
	std::vector< unsigned int > cubeIndices;
	float d = 8;
    
	
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
	glEnableClientState(GL_NORMAL_ARRAY);
	
	m_waterShader.bind();

	glVertexPointer( 3, GL_DOUBLE, 0, m_water.getVertexPointer() );
	glNormalPointer( GL_DOUBLE, 0, m_water.getNormalPointer() );

	//set the shader parameters

	m_waterShader.setFloatUniform("waterHeight", -4);
    m_waterShader.setIntUniform("numWaves", 1);

	

	int i = 1;
	float amplitude = 0.5f / (i + 1);
	float wavelength = 8 * M_PI / (i + 1);
	float speed = 1.0f + 2*i;
	Vector2 direction = (1, 1);

	m_waterShader.setFloatUniform("amplitude", amplitude);
	m_waterShader.setFloatUniform("wavelength", wavelength);
	m_waterShader.setFloatUniform("speed", speed);
	m_waterShader.setVector2Uniform("direction", direction.x, direction.y);

	m_waterShader.setMatrix4x4Uniform("worldcamera", m_camera.getTransformation().Inverse());
	m_waterShader.setMatrix4x4Uniform("projection", m_camera.getProjectionMatrix());
	m_waterShader.setMatrix3x3Uniform("worldcameraNormal", m_camera.getTransformation().Transpose());

	m_waterShader.setMatrix4x4Uniform("modelworld", m_water.getTransformation() );
	m_waterShader.setMatrix3x3Uniform("modelworldNormal", m_water.getTransformation().Inverse().Transpose());
	
	m_waterShader.setVector3Uniform("eyePos", m_camera.origin().x, m_camera.origin().y, m_camera.origin().z);
	m_waterShader.setFloatUniform("fresnelBias", 0);
	m_waterShader.setFloatUniform("fresnelScale", 2);
	m_waterShader.setFloatUniform("fresnelPower", 1);
	m_waterShader.setFloatUniform("etaRatio", 1.5);

	glBindTexture(GL_TEXTURE_CUBE_MAP, CubMapTextureID);
	m_waterShader.setIntUniform("env", 0);
	
	glDrawElements( GL_TRIANGLES, m_water.getNumberOfFaces()*3, GL_UNSIGNED_INT, m_water.getVertexIndicesPointer() );

	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	m_waterShader.unbind();

	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	
	glDisable(GL_TEXTURE_CUBE_MAP);
	glDisable(GL_NORMALIZE);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
}
