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
	Mesh3D* plane = createWaterPlane();
	
	// move plane so it is lower than camera
	plane->translateWorld( Vector3(0.0,-10.0,0.0) );

	m_meshes.push_back(plane);
	
	// put a light in the sky
	m_light.translateWorld( Vector3(0,5,0) );

	// set camera to look at world coordinate center
    set_scene_pos(Vector3(0.0, 0.0, 0.0), 2.0);
	
	// load shaders
	m_diffuseShader.create("diffuse.vs", "diffuse.fs");
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
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	
	for (std::vector<Mesh3D*>::iterator mIt = m_meshes.begin(); mIt != m_meshes.end(); ++mIt)
    {
        Mesh3D *mesh = *mIt;
        draw_mesh(mesh);
	}
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