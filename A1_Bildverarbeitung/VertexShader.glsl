//
// The gouraud vertex shader.
// ---------------------------------
// Angepasst für Core Profile
//
// @author: Link Alexis Constantin, Andreas Klein
// @lecturer: Prof. Dr. Alfred Nischwitz
//
// (c)2010 Hochschule München, HM
//
// ---------------------------------
#version 400
//Attribute 
in vec4 vVertex;
in vec2 vTexCoord;

uniform mat4 mvpMatrix;
//Texturkoordinaten an Fragmentshader übergeben
smooth out vec2 texCoords;

void main()
{	
	
	gl_Position = mvpMatrix * vVertex ;
	// texcoords übergeben.
	texCoords = vTexCoord;
}