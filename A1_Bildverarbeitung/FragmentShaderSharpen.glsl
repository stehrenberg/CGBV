//
// Fragment Shader für Sharpen Operator
// Angepasst für Core Profile
// ---------------------------------
//
// @author: Prof. Dr. Alfred Nischwitz
// @lecturer: Prof. Dr. Alfred Nischwitz
//
// (c)2011 Hochschule München, HM
//
// ---------------------------------
#version 400


smooth in vec2 texCoords;			// pixelbezogene Texturkoordinate
out vec4 fragColor;					// Ausgabewert mit 4 Komponenten zwischen 0.0 und 1.0
uniform samplerRect textureMap;		// Sampler für die Texture Map
uniform vec4 param1;				// param1.x +=F5, -=F6, param1.y +=F7, -=F8, param1.z +=F9, -=F10, param1.w +=F11, -=F12

uniform float sharpen[9] =	{	-1,-1,-1,
								-1, 8,-1,
							    -1,-1,-1};

uniform float part = 0.05;

uniform vec2 offsets[9] = {		vec2(-1,  1),
								vec2(-1,  0),
								vec2(-1, -1),
								vec2( 0,  1),
								vec2( 0,  0),
								vec2( 0, -1),
								vec2( 1,  1),
								vec2( 1,  0),
								vec2( 1, -1) };

void main() {
	float factor = param1.z * part;
	vec4 texel = vec4(0.0, 0.0, 0.0, 1.0);

    for (int i = 0; i < 9; i++) {
        texel += texture(textureMap, texCoords + offsets[i]) * sharpen[i];
    }

    fragColor =	texture(textureMap, texCoords) + texel * factor;
}