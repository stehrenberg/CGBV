//
// Angepasst für Core Profile
// ---------------------------------
//
// @author: Link Alexis Constantin, Andreas Klein
// @lecturer: Prof. Dr. Alfred Nischwitz
//
// (c)2010 Hochschule München, HM
//
// ---------------------------------
#version 130

uniform sampler2D TextureMap;

in vec4 color ;
in vec2 TexCoord;
smooth out vec4 fragColor;

void main()
{
	fragColor = color * texture(TextureMap, TexCoord) ;
	//fragColor = vec4(TexCoord, 0.0, 1.0);
}
}