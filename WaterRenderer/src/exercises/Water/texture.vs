uniform mat4 modelworld;
uniform mat4 worldcamera;
uniform mat4 projection;

uniform mat3 modelworldNormal;
uniform mat3 worldcameraNormal;

uniform vec3 lightposition;

varying vec3 normal;
varying vec3 lightDir;


void main()
{	  
	
	vec3 vertex = vec3( worldcamera * modelworld * gl_Vertex );

	lightDir = normalize(vertex - lightposition);

    normal = normalize( worldcameraNormal * modelworldNormal * gl_Normal );

	gl_Position = projection * worldcamera * modelworld * gl_Vertex;

	// get texture coordinate
	gl_TexCoord[0]  = gl_MultiTexCoord0;
}
