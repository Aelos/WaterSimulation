uniform sampler2D texture;


void main()
{   
    vec4 tempcolor = clamp(texture2D(texture, gl_TexCoord[0].xy),0,1);
    gl_FragColor = tempcolor;
}