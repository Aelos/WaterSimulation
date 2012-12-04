uniform sampler2D texture;

varying vec3 normal;
varying vec3 lightDir;

void main()
{   

    float dot = dot(-lightDir, normal);
    if (dot<0) {
    dot=0;
    }

    vec4 tempcolor = clamp((texture2D(texture, gl_TexCoord[0].xy)),0,1);
    gl_FragColor = tempcolor;
}