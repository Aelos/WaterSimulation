uniform mat4 modelworld;
uniform mat4 worldcamera;
uniform mat4 projection;

varying vec3 texCoord;

void main()
{	  	
	gl_Position = projection * worldcamera * gl_Vertex;

	texCoord = vec3(gl_Vertex);
}