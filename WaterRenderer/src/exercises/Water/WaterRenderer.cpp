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
	m_sky.clear();
}

void 
WaterRenderer::
init()
{
    // initialize parent
    TrackballViewer::init();

	load_water();
	load_sky();
	
	//m_meshes.push_back(sky);
	
	m_water.translateWorld( Vector3(0,-4,0) );
	m_water.scaleObject( Vector3 (5,5,5) );

	m_sky.scaleObject( Vector3 (10,10,10) );

	// put a light in the sky
	m_light.translateWorld( Vector3(0,0,0) );
	lightColor = Vector4(1, 1, 1, 1.0);

	// set camera to look at world coordinate center
    set_scene_pos(Vector3(0.0, 0.0, 0.0), 2);
	
	generateCubeMap();
	
	// load shaders
	m_skyShader.create("skybox.vs", "skybox.fs");
	m_waterShader.create("water.vs", "water.fs");
}

void
WaterRenderer::
load_water()
{
	// load mesh from obj
	Mesh3DReader::read( "water.obj", m_water, "water.mtl");
			
	// calculate normals
	if(!m_water.hasNormals()) {
		m_water.calculateVertexNormals();
	}
}

void
WaterRenderer::
load_sky()
{
	// load mesh from obj
	Mesh3DReader::read( "sky.obj", m_sky, "sky.mtl");
			
	// calculate normals
	if(!m_sky.hasNormals()) {
		m_sky.calculateVertexNormals();
	}
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

//-----------------------------------------------------------------------------

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
	draw_sky();

	glEnable(GL_DEPTH_TEST);
	draw_water();
}

void
WaterRenderer::
draw_sky() {
	
	m_skyShader.bind(); 
	
	// set parameters to send to the shader
	m_skyShader.setMatrix4x4Uniform("worldcamera", m_camera.getTransformation().Inverse());
	m_skyShader.setMatrix4x4Uniform("projection", m_camera.getProjectionMatrix());
	m_skyShader.setMatrix4x4Uniform("modelworld", m_sky.getTransformation() );
	
	m_sky.getMaterial(0).m_diffuseTexture.bind();
	m_skyShader.setIntUniform("texture", m_sky.getMaterial(0).m_diffuseTexture.getLayer());

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glVertexPointer( 3, GL_DOUBLE, 0, m_sky.getVertexPointer() );
	glNormalPointer( GL_DOUBLE, 0, m_sky.getNormalPointer() );
	glTexCoordPointer( 2, GL_DOUBLE, 0, m_sky.getUvTextureCoordPointer() );
	
	for(unsigned int i = 0; i < m_sky.getNumberOfParts(); i++)
	{
		glDrawElements( GL_TRIANGLES, m_sky.getNumberOfFaces(i)*3, GL_UNSIGNED_INT, m_sky.getVertexIndicesPointer(i) );
	}
	
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	m_sky.getMaterial(0).m_diffuseTexture.unbind();

	m_skyShader.unbind();
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
	for (i = 0; i < 6; i++)
	{
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
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	m_waterShader.bind();

	glVertexPointer( 3, GL_DOUBLE, 0, m_water.getVertexPointer() );
	glNormalPointer( GL_DOUBLE, 0, m_water.getNormalPointer() );
	glTexCoordPointer( 2, GL_DOUBLE, 0, m_water.getUvTextureCoordPointer() );
	
	//set the shader parameters
	m_waterShader.setMatrix4x4Uniform("worldcamera", m_camera.getTransformation().Inverse());
	m_waterShader.setMatrix4x4Uniform("projection", m_camera.getProjectionMatrix());
	m_waterShader.setMatrix4x4Uniform("modelworld", m_water.getTransformation() );

	m_waterShader.setVector3Uniform("eyePos", m_camera.origin().x, m_camera.origin().y, m_camera.origin().z);
	m_waterShader.setFloatUniform("fresnelBias", 4);
	m_waterShader.setFloatUniform("fresnelScale", 4);
	m_waterShader.setFloatUniform("fresnelPower", 1);
	m_waterShader.setFloatUniform("etaRatio", 1);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, CubMapTextureID);
	m_waterShader.setIntUniform("env", 0);

	//Draw the water plane
	
	glDrawElements( GL_TRIANGLES, m_water.getNumberOfFaces()*3, GL_UNSIGNED_INT, m_water.getVertexIndicesPointer() );

	
	
	glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
	m_waterShader.unbind();

	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);
	
	glDisable(GL_TEXTURE_CUBE_MAP);
	glDisable(GL_NORMALIZE);
	glDisable(GL_TEXTURE_GEN_S);
	glDisable(GL_TEXTURE_GEN_T);
	glDisable(GL_TEXTURE_GEN_R);
}
