const float pi = 3.14159;
uniform float waterHeight;
uniform float time;
uniform int numWaves;
uniform float amplitude;
uniform float wavelength;
uniform float speed;
uniform vec2 direction;

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
float wave(int i, float x, float y) {
    float frequency = 2*pi/wavelength;
    float phase = speed * frequency;
    float theta = dot(direction, vec2(x, y));
    return amplitude * sin(theta * frequency + time * phase);
}

float waveHeight(float x, float y) {
    float height = 0.0;
	for (int i = 0; i < numWaves; ++i)
        height += wave(i, x, y);
	return height;
}

float dWavedx(int i, float x, float y) {
    float frequency = 2*pi/wavelength;
    float phase = speed * frequency;
    float theta = dot(direction, vec2(x, y));
    float A = amplitude * direction.x * frequency;
    return A * cos(theta * frequency + time * phase);
}

float dWavedy(int i, float x, float y) {
    float frequency = 2*pi/wavelength;
    float phase = speed * frequency;
    float theta = dot(direction, vec2(x, y));
    float A = amplitude * direction.y * frequency;
    return A * cos(theta * frequency + time * phase);
}

vec3 waveNormal(float x, float y) {
    float dx = 0.0;
    float dy = 0.0;
    for (int i = 0; i < numWaves; ++i) {
        dx += dWavedx(i, x, y);
        dy += dWavedy(i, x, y);
    }
	vec3 n = vec3(-dx, 1.0, -dy);
    return normalize(n);
}

////////////////////////////////////


void main()
{
	vec4 pos = gl_Vertex;
    
	pos.y = waterHeight + waveHeight(pos.x, pos.z);
	
	vec3 worldNormal = waveNormal(pos.x, pos.z);
    gl_Position = projection * worldcamera * modelworld * pos;

	// Create incident and normal vectors
	vec3 I = normalize(pos.xyz/pos.w - eyePos.xyz);
	vec3 N = normalize(modelworldNormal * gl_Normal);
	
	// Calculate reflected and refracted vectors
	reflectedVector = reflect(I, worldNormal);
	refractedVector = refract(I, worldNormal, etaRatio);
	
	// Appoximation of the fresnel equation
	refFactor = fresnelBias+fresnelScale*pow(1+dot(I,worldNormal), fresnelPower);
}