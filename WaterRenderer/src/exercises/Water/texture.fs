uniform sampler2D texture;
uniform vec4 lightcolor;

varying vec3 normal;
varying vec3 worldCoordinates;
varying vec3 lightDir;

void main()
{   

    float dot = dot(-lightDir, normal);
    if (dot<0) {
        dot=0;
    }

    if(worldCoordinates.y<-2) {
        gl_FragColor = vec4(0.2,0.3,0.5,0.5);
    } else {
        vec4 tempcolor = clamp(dot * texture2D(texture, gl_TexCoord[0].xy) * lightcolor + lightcolor*0.05,0,1);
        gl_FragColor = tempcolor;
    }
}