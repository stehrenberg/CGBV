//
// Fragment Shader für 3x3 Gauss Tiefpass-Filter
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
float pi = 3.14159265359;
float sigma = param1.x * part + 0.00000000001;
float twoTimeSquareSigma = 2 * sigma * sigma;
float twoTimePiSquareSigma = pi * twoTimeSquareSigma;
uniform vec2 offsets[9] = {		vec2(-1,  1), vec2( 0,  1), vec2( 1,  1),
								vec2(-1,  0), vec2( 0,  0), vec2( 1,  0),
								vec2(-1, -1), vec2( 0, -1), vec2( 1, -1) };

void main() {
	vec4 texel = vec4(0.0, 0.0, 0.0, 1.0);
	float sum = 0.0;	

    for (int i = 0; i < offsets.length; i++) {
		float x = offsets[i].x;
		float y = offsets[i].y;		
		float ePower = exp(-(x * x + y * y) / twoTimeSquareSigma);
		
		float factor = (1 / twoTimePiSquareSigma) * ePower;		
		sum += factor;

        texel += texture(textureMap, texCoords + offsets[i]) * factor;
    }

    fragColor =	texel / sum;
}