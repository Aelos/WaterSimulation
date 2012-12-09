uniform samplerCube cubemap;

varying vec3 texCoord;

void main()
{   
    vec4 tempcolor = textureCube(cubemap, texCoord);
    gl_FragColor = tempcolor;
}