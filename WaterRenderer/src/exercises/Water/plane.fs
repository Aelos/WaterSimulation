varying vec4 color;
varying vec3 normal;
varying vec3 lightDir;


void main()
{
    float dot = dot(-lightDir, normal);
    gl_FragColor = clamp(color * dot,0,1);
    
    normal; color;
  
}