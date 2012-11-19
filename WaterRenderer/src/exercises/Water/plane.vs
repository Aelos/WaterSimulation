uniform mat4 ModelWorldTransform;
uniform mat4 WorldCameraTransform;
uniform mat4 ProjectionMatrix;

uniform mat3 WorldCameraNormalTransform;
uniform mat3 ModelWorldNormalTransform;

uniform vec3 lightposition;

varying vec3 normal;
varying vec4 color;
varying vec3 lightDir;

void main()
{
	gl_Position = ProjectionMatrix*(WorldCameraTransform*(ModelWorldTransform*(gl_Vertex)));

    color = gl_Color;
    
	vec3 vertex = vec3( WorldCameraTransform * ModelWorldTransform * gl_Vertex );
	
	lightDir = normalize(vertex - lightposition);

    normal = normalize(WorldCameraNormalTransform*(ModelWorldNormalTransform*(gl_Normal)));
    
}