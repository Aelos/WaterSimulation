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

	// create uniform cube
	Mesh3D* sky = createWaterPlane();
	
	// move plane so it is lower than camera
	sky->translateWorld( Vector3(0.0, 6.0, 0.0) );

	load_mesh();

	m_meshes.push_back(sky);
	
	m_water.translateWorld( Vector3(0,-6,0) );
	m_water.scaleObject( Vector3 (20,20,20) );

	// put a light in the sky
	m_light.translateWorld( Vector3(0,5,0) );

	// set camera to look at world coordinate center
    set_scene_pos(Vector3(0.0, 0.0, 0.0), 2.0);
	
	// load shaders
	m_diffuseShader.create("diffuse.vs", "diffuse.fs");
	m_textureShader.create("texture.vs", "texture.fs");
}

// Create the water plane with its vertices and normals and colors.
Mesh3D* 
WaterRenderer::
createWaterPlane()
{
    
	Mesh3D *water = new Mesh3D();
    
	std::vector< Vector3 > waterVertices;
	std::vector< Vector3 > waterNormals;
	std::vector< Vector3 > waterColors;
	std::vector< unsigned int > waterIndices;
	float d = 0.25;
    
	waterVertices.push_back(Vector3( 15, 0, 15));
	waterVertices.push_back(Vector3( 15, 0, -15));
	waterVertices.push_back(Vector3( -15, 0, 15));
	waterVertices.push_back(Vector3( -15, 0, -15));
	for(int k = 0; k < 4; k++) waterNormals.push_back(Vector3(0, 1, 0));
	for(int k = 0; k < 4; k++) waterColors.push_back(Vector3(0.3, 0.3, 0.8));
	waterIndices.push_back(0);
	waterIndices.push_back(1);
	waterIndices.push_back(2);
	waterIndices.push_back(1);
	waterIndices.push_back(2);
	waterIndices.push_back(3);
	
	water->setIndices(waterIndices);
	water->setVertexPositions(waterVertices);
	water->setVertexNormals(waterNormals);
	water->setVertexColors(waterColors);
    

    return water;
}

void
WaterRenderer::
load_mesh()
{
	Vector3 bbmin, bbmax;
	double radius;
	Vector3 center;
	
	// load mesh from obj
	Mesh3DReader::read( "water.obj", m_water, "water.mtl");
			
	// calculate normals
	if(!m_water.hasNormals())
		m_water.calculateVertexNormals();
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

	draw_textured(&m_water);
}

void
WaterRenderer::
draw_mesh(Mesh3D *mesh)
{	

	glEnable(GL_DEPTH_TEST);
	
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
	
	glEnable(GL_DEPTH_TEST);
	
	m_textureShader.bind(); 
	
	// set parameters to send to the shader
	m_textureShader.setMatrix4x4Uniform("worldcamera", m_camera.getTransformation().Inverse());
	m_textureShader.setMatrix4x4Uniform("projection", m_camera.getProjectionMatrix());
	m_textureShader.setMatrix4x4Uniform("modelworld", mesh->getTransformation() );

	m_textureShader.setMatrix3x3Uniform("modelworldNormal", mesh->getTransformation().Inverse().Transpose());
	m_textureShader.setMatrix3x3Uniform("worldcameraNormal", mesh->getTransformation().Transpose());
	

	mesh->getMaterial(0).m_diffuseTexture.bind();
	m_textureShader.setIntUniform("texture", mesh->getMaterial(0).m_diffuseTexture.getLayer());
	


	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	glVertexPointer( 3, GL_DOUBLE, 0, mesh->getVertexPointer() );
	glNormalPointer( GL_DOUBLE, 0, mesh->getNormalPointer() );
	glTexCoordPointer( 2, GL_DOUBLE, 0, mesh->getUvTextureCoordPointer() );
	
	glDrawElements( GL_TRIANGLES, mesh->getNumberOfFaces()*3, GL_UNSIGNED_INT, mesh->getVertexIndicesPointer() );
	
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_VERTEX_ARRAY);

	// finally, unbind the shader
	mesh->getMaterial(0).m_diffuseTexture.unbind();
	m_textureShader.unbind();
}

void
drawEnvironmentMapped() {
	glEnable(GL_TEXTURE_CUBE_MAP_EXT); 
	
	GLubyte face[6][64][64][3];
	for (int i=0; i<6; i++) { 
		glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X_EXT + i, 
		0,                  //level 
		GL_RGB8,            //internal format 
		64,                 //width 
		64,                 //height 
		0,                  //border 
		GL_RGB,             //format 
		GL_UNSIGNED_BYTE,   //type 
		&face[i][0][0][0]); // pixel data
	}

	glDisable(GL_TEXTURE_CUBE_MAP_EXT);
}
