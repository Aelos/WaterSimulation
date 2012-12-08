//displacement section //

const float pi = 3.14159;
const float wavelength = pi / 4;
const float speed = 1.05;
const float amplitude = 0.3;
const vec2 direction = vec2(1,1);
const float waterHeight = -0.5;

varying vec3 position;
varying vec3 worldNormal;
varying vec3 eyeNormal;
varying float  ratioRefract;


////////////////////////////

uniform mat4 modelworld;
uniform mat4 worldcamera;
uniform mat4 projection;

// Position of the view eye in world space
uniform vec3 eyePos;

// Fresnel parameters
uniform float fresnelBias, fresnelScale, fresnelPower;

// Ratio of indices of refraction
uniform float etaRatio;

// Reflected and refracted vectors
varying vec3 reflectedVector, refractedVector;

// Reflection factor based on fresnel equation
varying float refFactor;



float wave(float x, float z) {

    float frequency = 2*pi/wavelength;
    float phase = speed * frequency;

    float theta = dot(direction, vec2(x, z));
    return amplitude * sin(theta * frequency +  phase);
}


float waveHeight(float x, float y) {
    float height = 0.0;
        height += wave( x, y);
    return height;
}

float dWavedx( float x, float y) {
    float frequency = 2*pi/wavelength;
    float phase = speed * frequency;
    float theta = dot(direction, vec2(x, y));
    float A = amplitude * direction.x * frequency;
    return A * cos(theta * frequency +  phase);
}

float dWavedy( float x, float y) {
    float frequency = 2*pi/wavelength;
    float phase = speed * frequency;
    float theta = dot(direction, vec2(x, y));
    float A = amplitude * direction.y * frequency;
    return A * cos(theta * frequency +  phase);
}

vec3 waveNormal(float x, float z) {
    float dx = 0.0;
    float dy = 0.0;
    
    dx += dWavedx( x, z);
    dy += dWavedy( x, z);
    
	vec3 n = vec3(-dx,1.0, -dy);
    return normalize(n);
}



void main()
{
	// displacememnt section //
	vec3 worldCoordinates = vec3(modelworld*gl_Vertex);
	vec4 NworldCoordinates = vec4(worldCoordinates.x,worldCoordinates.y,worldCoordinates.z,1.0);
	vec4 pos = gl_Vertex;
	vec3 normal;
	
		
		pos.y = waterHeight + waveHeight(pos.x, pos.z);
		position = pos.xyz / pos.w;
		worldNormal = waveNormal(pos.x, pos.z);

		normal = gl_NormalMatrix * worldNormal;
    
		

	
	/////////////////////////////

	// Create incident and normal vectors
	vec3 I = normalize(pos.xyz - eyePos.xyz);
	vec3 N = normalize(normal);
	
	// Calculate reflected and refracted vectors
	reflectedVector = reflect(I, N);
	refractedVector = refract(I, N, etaRatio);
	ratioRefract = etaRatio;
	// Appoximation of the fresnel equation
	refFactor = fresnelBias+fresnelScale*pow(1+dot(I,N), fresnelPower);
	
	// Transform vertex
	gl_TexCoord[0] = gl_MultiTexCoord0;
	gl_Position = projection * worldcamera * modelworld * pos;
}