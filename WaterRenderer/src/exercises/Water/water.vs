const float pi = 3.14159;
uniform float waterHeight;
uniform float time;
uniform int numWaves;

uniform float amplitude1;
uniform float wavelength1;
uniform float speed1;
uniform vec2 direction1;

uniform float amplitude2;
uniform float wavelength2;
uniform float speed2;
uniform vec2 direction2;

///////////

uniform mat4 modelworld;
uniform mat4 worldcamera;
uniform mat4 projection;

uniform mat3 modelworldNormal;
uniform mat3 worldcameraNormal;

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


///////////////////////////////

float wave1(float x, float y) {
	float frequency = 2*pi/wavelength1;
    float phase = speed1 * frequency;
    float theta = dot(direction1, vec2(x, y));
    return amplitude1 * sin(theta * frequency + time * phase);
}

float wave2(float x, float y) {
	float frequency = 2*pi/wavelength2;
    float phase = speed2 * frequency;
    float theta = dot(direction2, vec2(x, y));
    return amplitude2 * sin(theta * frequency + time * phase);
}

float waveHeight(float x, float y) {
	return wave1(x,y);// + wave2(x,y);
}

float dWavedx1(float x, float y) {
    float frequency = 2*pi/wavelength1;
    float phase = speed1 * frequency;
    float theta = dot(direction1, vec2(x, y));
    float A = amplitude1 * direction1.x * frequency;
    return A * cos(theta * frequency + time * phase);
}

float dWavedx2(float x, float y) {
    float frequency = 2*pi/wavelength2;
    float phase = speed2 * frequency;
    float theta = dot(direction2, vec2(x, y));
    float A = amplitude2 * direction2.x * frequency;
    return A * cos(theta * frequency + time * phase);
}

float dWavedy1(float x, float y) {
    float frequency = 2*pi/wavelength1;
    float phase = speed1 * frequency;
    float theta = dot(direction1, vec2(x, y));
    float A = amplitude1 * direction1.y * frequency;
    return A * cos(theta * frequency + time * phase);
}

float dWavedy2(float x, float y) {
    float frequency = 2*pi/wavelength2;
    float phase = speed2 * frequency;
    float theta = dot(direction2, vec2(x, y));
    float A = amplitude2 * direction2.y * frequency;
    return A * cos(theta * frequency + time * phase);
}

vec3 waveNormal(float x, float y) {
    float dx = dWavedx1(x, y);// + dWavedx2(x,y);
    float dy = dWavedy1(x, y);// + dWavedy2(x,y);
	vec3 n = vec3(-dx, 1.0, -dy);
    return normalize(n);
}

////////////////////////////////////


void main()
{
	vec4 pos = gl_Vertex;
    
	pos.y = waterHeight + waveHeight(pos.x,pos.z);
	vec3 worldNormal = waveNormal(pos.x, pos.z);
    gl_Position = projection * worldcamera * modelworld * pos;

	// Create incident and normal vectors
	vec3 I = normalize(pos.xyz/pos.w - eyePos.xyz);

	// Calculate reflected and refracted vectors
	reflectedVector = reflect(I, worldNormal);
	refractedVector = refract(I, worldNormal, etaRatio);
	
	// Appoximation of the fresnel equation
	refFactor = fresnelBias+fresnelScale*pow(1+dot(I,worldNormal), fresnelPower);
}