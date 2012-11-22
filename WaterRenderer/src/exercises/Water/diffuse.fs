varying vec4 color;
varying vec3 normal;
varying vec3 lightDir;


void main()
{
    float dot = dot(-lightDir, normal);
    if (dot<0) {
    dot=0;
    }

    vec4 tempcolor = clamp((dot*color),0,1);
    gl_FragColor = clamp (tempcolor + color*0.05,0,1);
}