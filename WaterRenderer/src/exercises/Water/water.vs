const float pi = 3.14159;
uniform float waterHeight;
uniform float time;
uniform int numWaves;
uniform int isRadial;
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

float wave(float x, float y,float wavelength,float speed,float amplitude, vec2 direction) {
	if (isRadial == 1) {
		direction = normalize(vec2(x,y));
	}
	float frequency = 2*pi/wavelength;
    float phase = speed * frequency;
    float theta = dot(direction, vec2(x, y));
	float steepness = 1 / (amplitude * frequency);
    return steepness * amplitude * sin(theta * frequency + time * phase);
}


float wave4(float x, float y){
	vec2 direction = direction4;
		if (isRadial == 1) {
		direction = normalize(vec2(x,y));
	}
	vec2 wavepos = direction*(time4-time)*speed4;
	if( x < wavepos.x - wavelength4/4 * normalize(direction).x || x > wavepos.x + wavelength4/4 * normalize(direction).x) {
		return 0;
	} else {
		float frequency = 2*pi/wavelength4;
		float phase = speed4 * frequency;
		float theta = dot(direction, vec2(x, y));
		float steepness = 1 / (amplitude4 * frequency);
    	return steepness * amplitude4 * cos(theta*frequency + (time-time4) * phase);
	}
}

float waveHeight(float x, float y) {
	return wave(x,y,wavelength1,speed1,amplitude1,direction1) +
		 wave(x,y,wavelength2,speed2,amplitude2,direction2) +
		  wave(x,y,wavelength3,speed3,amplitude3,direction3) + wave4(x,y);
}

float dWavedx(float x, float y,float wavelength,float speed,float amplitude, vec2 direction) {
	if (isRadial == 1) {
		direction = normalize(vec2(x,y));
	}
    float frequency = 2*pi/wavelength;
    float phase = speed * frequency;
    float theta = dot(direction, vec2(x, y));
    float A = amplitude * direction.x * frequency;
	float steepness = 1 / (amplitude * frequency);
    return steepness * A * cos(theta * frequency + time * phase);
}

float dWavedy(float x, float y,float wavelength,float speed,float amplitude, vec2 direction) {
	if (isRadial == 1) {
		direction = normalize(vec2(x,y));
	}
    float frequency = 2*pi/wavelength;
    float phase = speed * frequency;
    float theta = dot(direction, vec2(x, y));
    float A = amplitude * direction.y * frequency;
	float steepness = 1 / (amplitude * frequency);
    return steepness * A * cos(theta * frequency + time * phase);
}



float dWavedx4(float x, float y) {
	vec2 direction = direction4;
	if (isRadial == 1) {
		direction = normalize(vec2(x,y));
	}
    vec2 wavepos = direction*(time4-time)*speed4;
	if( x < wavepos.x - wavelength4/4) {
		return 0;
	} else if (x > wavelength4/4 + wavepos.x) {
		return 0;
	} else {
		float frequency = 2*pi/wavelength4;
		float phase = speed4 * frequency;
		float theta = dot(direction, vec2(x, y));
		float A = amplitude4 * direction.x * frequency;
		float steepness = 1 / (amplitude4 * frequency);
		return -A * steepness * sin(theta * frequency + (time-time4) * phase);
	}
}

float dWavedy4(float x, float y) {
	vec2 direction = direction4;
	if (isRadial == 1) {
		direction = normalize(vec2(x,y));
	}
    vec2 wavepos = direction*(time4-time)*speed4;
	if( x < wavepos.x - wavelength4/4) {
		return 0;
	} else if (x > wavepos.x + wavelength4/4) {
		return 0;
	} else {
		float frequency = 2*pi/wavelength4;
		float phase = speed4 * frequency;
		float theta = dot(direction4, vec2(x, y));
		float steepness = 1 / (amplitude4 * frequency);
		float A = amplitude4 * direction.y * frequency;
		return -A * steepness * sin(theta * frequency + (time-time4) * phase);
	}
}

vec3 waveNormal(float x, float y) {
    float dx = dWavedx(x, y,wavelength1,speed1,amplitude1,direction1) +
		 dWavedx(x, y,wavelength2,speed2,amplitude2,direction2) 
		 + dWavedx(x, y,wavelength3,speed3,amplitude3,direction3)
		  + dWavedx4(x,y);
    float dy = dWavedy(x, y,wavelength1,speed1,amplitude1,direction1) +
			 dWavedy(x, y,wavelength2,speed2,amplitude2,direction2) +
			  dWavedy(x, y,wavelength3,speed3,amplitude3,direction3) +
			   dWavedy4(x,y);
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