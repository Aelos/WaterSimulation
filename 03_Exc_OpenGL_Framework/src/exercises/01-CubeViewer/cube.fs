varying vec4 color;
varying vec3 normal;



void main()
{
    // Exercise 3.3.2
    gl_FragColor = vec4(0.3, 0.3, 0.6, 1.0);
    vec3 light = vec3(0,0,1);
    float dot = dot(light, normal);
    gl_FragColor = color*dot;
    

    // need this line so OpenGL doesn't optimize out the variables -- remove in your solution
    // {
    normal; color;
    // }
}