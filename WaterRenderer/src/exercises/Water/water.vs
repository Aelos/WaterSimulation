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

uniform float amplitude3;
uniform float wavelength3;
uniform float speed3;
uniform vec2 direction3;

uniform float amplitude4;
uniform float wavelength4;
uniform float speed4;
uniform vec2 direction4;
uniform int time4;

///////////

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

float wave3(float x, float y) {
	float frequency = 2*pi/wavelength3;
    float phase = speed3 * frequency;
    float theta = dot(direction3, vec2(x, y));
    return amplitude3 * sin(theta * frequency + time * phase);
}

float wave4(float x, float y){

	vec2 wavepos = direction4*(time4-time)*speed4;
	if( x < wavepos.x - wavelength4/4 * normalize(direction4).x || x > wavepos.x + wavelength4/4 * normalize(direction4).x) {
		return 0;
	} else {
		float frequency = 2*pi/wavelength4;
		float phase = speed4 * frequency;
		float theta = dot(direction4, vec2(x, y));
    	return amplitude4 * cos(theta*frequency + (time-time4) * phase);
	}
}

float waveHeight(float x, float y) {
	return wave1(x,y) + wave2(x,y) + wave3(x,y) + wave4(x,y);
}

float dWavedx1(float x, float y) {
    float frequency = 2*pi/wavelength1;
    float phase = speed1 * frequency;
    float theta = dot(direction1, vec2(x, y));
    float A = amplitude1 * direction1.x * frequency;
    return A * cos(theta * frequency + time * phase);
}

float dWavedy1(float x, float y) {
    float frequency = 2*pi/wavelength1;
    float phase = speed1 * frequency;
    float theta = dot(direction1, vec2(x, y));
    float A = amplitude1 * direction1.y * frequency;
    return A * cos(theta * frequency + time * phase);
}

float dWavedx2(float x, float y) {
    float frequency = 2*pi/wavelength2;
    float phase = speed2 * frequency;
    float theta = dot(direction2, vec2(x, y));
    float A = amplitude2 * direction2.x * frequency;
    return A * cos(theta * frequency + time * phase);
}

float dWavedy2(float x, float y) {
    float frequency = 2*pi/wavelength2;
    float phase = speed2 * frequency;
    float theta = dot(direction2, vec2(x, y));
    float A = amplitude2 * direction2.y * frequency;
    return A * cos(theta * frequency + time * phase);
}

float dWavedx3(float x, float y) {
    float frequency = 2*pi/wavelength3;
    float phase = speed3 * frequency;
    float theta = dot(direction3, vec2(x, y));
    float A = amplitude3 * direction3.x * frequency;
    return A * cos(theta * frequency + time * phase);
}

float dWavedy3(float x, float y) {
    float frequency = 2*pi/wavelength3;
    float phase = speed3 * frequency;
    float theta = dot(direction3, vec2(x, y));
    float A = amplitude3 * direction3.y * frequency;
    return A * cos(theta * frequency + time * phase);
}

float dWavedx4(float x, float y) {
    vec2 wavepos = direction4*(time4-time)*speed4;
	if( x < wavepos.x - wavelength4/4) {
		return 0;
	} else if (x > wavelength4/4 + wavepos.x) {
		return 0;
	} else {
		float frequency = 2*pi/wavelength4;
		float phase = speed4 * frequency;
		float theta = dot(direction4, vec2(x, y));
		float A = amplitude4 * direction4.x * frequency;
		return -A * sin(theta * frequency + (time-time4) * phase);
	}
}

float dWavedy4(float x, float y) {
    vec2 wavepos = direction4*(time4-time)*speed4;
	if( x < wavepos.x - wavelength4/4) {
		return 0;
	} else if (x > wavepos.x + wavelength4/4) {
		return 0;
	} else {
		float frequency = 2*pi/wavelength4;
		float phase = speed4 * frequency;
		float theta = dot(direction4, vec2(x, y));;
		float A = amplitude4 * direction4.y * frequency;
		return -A * sin(theta * frequency + (time-time4) * phase);
	}
}

vec3 waveNormal(float x, float y) {
    float dx = dWavedx1(x, y) + dWavedx2(x, y) + dWavedx3(x, y) + dWavedx4(x,y);
    float dy = dWavedy1(x, y) + dWavedy2(x, y) + dWavedy3(x, y) + dWavedy4(x,y);
	vec3 n = vec3(-dx, 5.0, -dy);
    return normalize(n);
}

////////////////////////////////////


void main()
{
	vec4 pos =  modelworld * gl_Vertex;
    
	pos.y = waterHeight + waveHeight(pos.x,pos.z);
	vec3 worldNormal = waveNormal(pos.x, pos.z);
    gl_Position = projection * worldcamera * pos;

	// Create incident and normal vectors
	vec3 I = normalize(pos.xyz/pos.w - eyePos.xyz);

	// Calculate reflected and refracted vectors
	reflectedVector = reflect(I, worldNormal);
	refractedVector = refract(I, worldNormal, etaRatio);
	
	// Appoximation of the fresnel equation
	refFactor = fresnelBias+fresnelScale*pow(1+dot(I,worldNormal), fresnelPower);
}