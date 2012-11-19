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
    m_meshes.push_back(createPlane());
	
	// move cube to (0,0,1)
	m_meshes[0]->scaleWorld( Vector3(100,100,100) );
    m_meshes[0]->translateWorld( Vector3(-40.0,-15.0,0.0) );

	m_light.translateWorld( Vector3(0,4,0) );

	// set camera to look at world coordinate center
    set_scene_pos(Vector3(0.0, 0.0, 0.0), 5.0);
	
	// load cube shader
	m_planeShader.create("plane.vs", "plane.fs");
}

Mesh3D* 
WaterRenderer::
createPlane()
{
    // initialize Mesh3D
    Mesh3D *cube = new Mesh3D();
    
	// setup uniform cube with side length 0.5 and center of cube being (0,0,0)
	std::vector< Vector3 > cubeVertices;
	std::vector< Vector3 > cubeNormals;
	std::vector< Vector3 > cubeColors;
	std::vector< unsigned int > cubeIndices;
	float d = 0.25;
    
	
	// front
	cubeVertices.push_back(Vector3( 1, 0, 1));
	cubeVertices.push_back(Vector3( 1, 0, -1));
	cubeVertices.push_back(Vector3( -1, 0, 1));
	cubeVertices.push_back(Vector3( -1, 0, -1));
	for(int k = 0; k < 4; k++) cubeNormals.push_back(Vector3(0, 1, 0));
	for(int k = 0; k < 4; k++) cubeColors.push_back(Vector3(0.1, 0.4, 1.0));
	cubeIndices.push_back(0);
	cubeIndices.push_back(1);
	cubeIndices.push_back(2);
	
	cube->setIndices(cubeIndices);
	cube->setVertexPositions(cubeVertices);
	cube->setVertexNormals(cubeNormals);
	cube->setVertexColors(cubeColors);
    

    return cube;
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
            
            // delete existing cubes
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

void
WaterRenderer::
draw_mesh(Mesh3D *mesh)
{	
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
}

//-----------------------------------------------------------------------------

void 
WaterRenderer::
draw_scene(DrawMode _draw_mode)
{
	// clear screen
	glEnable(GL_DEPTH_TEST);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//glEnable(GL_CULL_FACE);
	
    // first bind the shader
	m_planeShader.bind(); 
	
	// set parameters to send to the shader
	m_planeShader.setMatrix4x4Uniform("WorldCameraTransform", m_camera.getTransformation().Inverse());
	m_planeShader.setMatrix3x3Uniform("WorldCameraNormalTransform", m_camera.getTransformation().Transpose());
	m_planeShader.setMatrix4x4Uniform("ProjectionMatrix", m_camera.getProjectionMatrix());

    for (std::vector<Mesh3D*>::iterator mIt = m_meshes.begin(); mIt != m_meshes.end(); ++mIt)
    {
        Mesh3D *plane = *mIt;
        
        // besides during we can apply transformations just before rendering:
        // save the original transformation of the cube
        Matrix4 originalTransformation = plane->getTransformation();
        
        // rotate the cube before rendering
        plane->rotateObject(Vector3(0,1,0), M_PI/4);
        
        // send the model parameters to the shader
        m_planeShader.setMatrix4x4Uniform("ModelWorldTransform", plane->getTransformation() );
        m_planeShader.setMatrix3x3Uniform("ModelWorldNormalTransform", plane->getTransformation().Inverse().Transpose());
        
		Vector3 lightPosInCamera = m_camera.getTransformation().Inverse()*m_light.origin();;
		m_planeShader.setVector3Uniform("lightposition", lightPosInCamera.x, lightPosInCamera.y, lightPosInCamera.z);
		
        // render the cube
        draw_mesh(plane);
        
        // then reset the original transformation
        plane->setTransformation( originalTransformation );
    }
	
    // for illustration render a small sphere at the world center
	Matrix4 ident;
	ident.loadIdentity();
	m_planeShader.setMatrix4x4Uniform("ModelWorldTransform", ident );
	m_planeShader.setMatrix3x3Uniform("ModelWorldNormalTransform", ident );
	
	glColor3f(1.0,1.0,1.0); // set sphere color to white
	glutSolidSphere( 0.05, 10, 10 );
	
	// finally, unbind the shader
	m_planeShader.unbind();
		
}