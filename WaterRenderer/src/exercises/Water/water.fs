// Cube map
uniform samplerCube env;

// Reflected and refracted vectors
varying vec3 reflectedVector, refractedVector;

// Reflection factor based on fresnel equation
varying float refFactor;

varying vec3 position;
varying vec3 worldNormal;
varying vec3 eyeNormal;
uniform vec3 eyePos;
varying float ratioRefract;

void main()
{
            vec3 eye = normalize(eyePos - position);
            vec3 r = reflect(eye, worldNormal);
            vec3 refract = refract(eye,worldNormal,ratioRefract);

    // Read cube map
    vec4 reflectedColor = textureCube(env, r);
    vec4 refractedColor = textureCube(env, refract);
    // Mix reflected and refracted colors
    vec4 color = mix(refractedColor, reflectedColor, refFactor);
    color.a = 0.5;

    gl_FragColor = color;
}