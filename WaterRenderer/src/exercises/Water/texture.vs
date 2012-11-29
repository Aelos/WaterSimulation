uniform mat4 modelworld;
uniform mat4 worldcamera;
uniform mat4 projection;

uniform mat3 modelworldNormal;
uniform mat3 worldCameraNormal;

uniform sampler2D displacementMap;
void main()
{	  
	// transform vertex to camera coordinates
	gl_Position = projection * worldcamera * modelworld * gl_Vertex;
	
	// get texture coordinate
	gl_TexCoord[0]  = gl_MultiTexCoord0;

	
	/*
	vec4 worldPosition = worldcamera*modelworld*gl_Vertex;
	vec4 newPosition;
	vec4 dv;
	float df;
	
	dv = texture2D ( displacementMap, gl_MultiTexCoord0.xy);
	df = cos(dv.y)* 1;
	
	newPosition = (vec4 (modelworldNormal * gl_Normal,1) * df) + worldPosition;
	gl_Position = gl_ModelViewProjectionMatrix * newPosition;
	*/
}
