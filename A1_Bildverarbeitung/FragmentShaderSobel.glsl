//
// Fragment Shader für Sobel Operator
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

uniform float part = 0.05;
uniform float sobelx[9] = {	1, 2, 1,
							0, 0, 0,
						   -1,-2,-1};
uniform float sobely[9] = {	1, 0,-1,
							2, 0,-2,
							1, 0,-1};
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
	vec4 texel = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 texelx = vec4(0.0, 0.0, 0.0, 1.0);
	vec4 texely = vec4(0.0, 0.0, 0.0, 1.0);
	float brightness = param1.x * part;
	float contrast = 1 - param1.y * part;

    for (int i = 0; i < sobelx.length; i++) {
        texelx +=  sobelx[i] * texture(textureMap, texCoords + offsets[i]);
    }

	for (int i = 0; i < sobely.length; i++) {
        texely +=  sobely[i] * texture(textureMap, texCoords + offsets[i]);
    }

	texel = sqrt(texelx * texelx + texely * texely);

    fragColor =	contrast * (texel - 0.5) + 0.5 + brightness;
}

