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
    // clean up any existing mesh
    for (std::vector<Mesh3D*>::iterator mIt = m_meshes.begin(); mIt != m_meshes.end(); ++mIt)
    {
        delete *mIt;
    }
    m_meshes.clear();
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
	
	m_water.translateWorld( Vector3(0,-8,0) );
	m_water.scaleObject( Vector3 (20,20,20) );

	m_sky.scaleObject( Vector3 (15,15,15) );

	// put a light in the sky
	m_light.translateWorld( Vector3(0,0,0) );

	// set camera to look at world coordinate center
    set_scene_pos(Vector3(0.0, 0.0, 0.0), 2.5);
	
	// load shaders
	m_diffuseShader.create("diffuse.vs", "diffuse.fs");
	m_textureShader.create("texture.vs", "texture.fs");
	//m_environmentShader.create("fresnel.vs", "fresnel.fs");
}

void
WaterRenderer::
load_water()
{
	Vector3 bbmin, bbmax;
	double radius;
	Vector3 center;
	
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
	Vector3 bbmin, bbmax;
	double radius;
	Vector3 center;
	
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
            
            // delete existing meshes
            for (std::vector<Mesh3D*>::iterator mIt = m_meshes.begin(); mIt != m_meshes.end(); ++mIt)
            {
                delete *mIt;
            }
            m_meshes.clear();
            
            // reinitialize
            init();
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

	for (std::vector<Mesh3D*>::iterator mIt = m_meshes.begin(); mIt != m_meshes.end(); ++mIt)
    {
        Mesh3D *mesh = *mIt;
        draw_mesh(mesh);
	}

	glDisable(GL_DEPTH_TEST);
	draw_textured(&m_sky);
	glEnable(GL_DEPTH_TEST);
	draw_textured(&m_water);
}

void
WaterRenderer::
draw_mesh(Mesh3D *mesh)
{	

	Vector3 lightPosInCamera(0.0,0.0,0.0);
	lightPosInCamera = m_camera.getTransformation().Inverse()*m_light.origin();
	
	// first bind the shader
	m_diffuseShader.bind(); 
	
	// set parameters to send to the shader
	m_diffuseShader.setMatrix4x4Uniform("WorldCameraTransform", m_camera.getTransformation().Inverse());
	m_diffuseShader.setMatrix3x3Uniform("WorldCameraNormalTransform", m_camera.getTransformation().Transpose());
	m_diffuseShader.setMatrix4x4Uniform("ProjectionMatrix", m_camera.getProjectionMatrix());
	m_diffuseShader.setVector3Uniform("lightposition", lightPosInCamera.x, lightPosInCamera.y, lightPosInCamera.z);
	
	// send the model parameters to the shader
    m_diffuseShader.setMatrix4x4Uniform("ModelWorldTransform", mesh->getTransformation() );
    m_diffuseShader.setMatrix3x3Uniform("ModelWorldNormalTransform", mesh->getTransformation().Inverse().Transpose());
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	
	glVertexPointer( 3, GL_DOUBLE, 0, mesh->getVertexPointer() );
	glNormalPointer( GL_DOUBLE, 0, mesh->getNormalPointer() );
	glColorPointer( 3, GL_DOUBLE, 0, mesh->getColorPointer() );
	
	glDrawElements( GL_TRIANGLES, mesh->getNumberOfFaces()*3, GL_UNSIGNED_INT, mesh->getVertexIndicesPointer() );
	
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	// finally, unbind the shader
	m_diffuseShader.unbind();
}

void
WaterRenderer::
draw_textured(Mesh3D *mesh) {
	
	m_textureShader.bind(); 
	
	// set parameters to send to the shader
	m_textureShader.setMatrix4x4Uniform("worldcamera", m_camera.getTransformation().Inverse());
	m_textureShader.setMatrix4x4Uniform("projection", m_camera.getProjectionMatrix());
	m_textureShader.setMatrix4x4Uniform("modelworld", mesh->getTransformation() );
	
	Vector3 lightPosInCamera(0.0,0.0,0.0);
	lightPosInCamera = m_camera.getTransformation().Inverse()*m_light.origin();
	m_textureShader.setVector3Uniform("lightposition", lightPosInCamera.x, lightPosInCamera.y, lightPosInCamera.z);


	m_textureShader.setMatrix3x3Uniform("modelworldNormal", mesh->getTransformation().Inverse().Transpose());
	m_textureShader.setMatrix3x3Uniform("worldcameraNormal", m_camera.getTransformation().Transpose());
	

	mesh->getMaterial(0).m_diffuseTexture.bind();
	m_textureShader.setIntUniform("texture", mesh->getMaterial(0).m_diffuseTexture.getLayer());

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glVertexPointer( 3, GL_DOUBLE, 0, mesh->getVertexPointer() );
	glNormalPointer( GL_DOUBLE, 0, mesh->getNormalPointer() );
	glTexCoordPointer( 2, GL_DOUBLE, 0, mesh->getUvTextureCoordPointer() );
	
	for(unsigned int i = 0; i < mesh->getNumberOfParts(); i++)
	{
		glDrawElements( GL_TRIANGLES, mesh->getNumberOfFaces(i)*3, GL_UNSIGNED_INT, mesh->getVertexIndicesPointer(i) );
	}
	
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	// finally, unbind the shader
	mesh->getMaterial(0).m_diffuseTexture.unbind();

	m_textureShader.unbind();
}

void::
WaterRenderer::
draw_environment_map() {

	glEnable(GL_DEPTH_TEST);
	m_environmentShader.bind();

	m_textureShader.setVector3Uniform("eyepos", m_camera.origin().x, m_camera.origin().y, m_camera.origin().z);
	m_textureShader.setFloatUniform("fresnelBias", 5);
	m_textureShader.setFloatUniform("fresnelScale", 5);
	m_textureShader.setFloatUniform("fresnelPower", 5);
	m_textureShader.setFloatUniform("etaRatio", 5);

	GLuint g_cubeTexture = 0;
 
	const int CUBE_TEXTURE_SIZE = 256;
 
	glGenTextures(1, &g_cubeTexture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, g_cubeTexture);

    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_BASE_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAX_LEVEL, 0);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        
    std::vector<GLubyte> testData(CUBE_TEXTURE_SIZE * CUBE_TEXTURE_SIZE * 256, 128);
    std::vector<GLubyte> xData(CUBE_TEXTURE_SIZE * CUBE_TEXTURE_SIZE * 256, 255);
 
    for (int loop = 0; loop < 6; ++loop) {
		if(loop) {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + loop, 0, GL_RGBA8,
					CUBE_TEXTURE_SIZE, CUBE_TEXTURE_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, &testData[0]);
		} else {
			glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + loop, 0, GL_RGBA8,
					CUBE_TEXTURE_SIZE, CUBE_TEXTURE_SIZE, 0, GL_RGBA, GL_UNSIGNED_BYTE, &xData[0]);
        }
    }
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);

	m_environmentShader.setIntUniform("env", g_cubeTexture);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glVertexPointer( 3, GL_DOUBLE, 0, m_water.getVertexPointer() );
	glNormalPointer( GL_DOUBLE, 0, m_water.getNormalPointer() );
	glTexCoordPointer( 2, GL_DOUBLE, 0, m_water.getUvTextureCoordPointer() );
	
	glDrawElements( GL_TRIANGLES, m_water.getNumberOfFaces()*3, GL_UNSIGNED_INT, m_water.getVertexIndicesPointer() );
	
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	m_environmentShader.unbind();
}
