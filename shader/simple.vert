#version 330 core

uniform mat4 mvpMatrix;
uniform vec4 color;

in vec4 vertex;

out vec4 c;

void main()
{   
    gl_Position = mvpMatrix * vertex;    
	c = color;
}