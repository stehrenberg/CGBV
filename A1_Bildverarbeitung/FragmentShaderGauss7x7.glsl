//
// Fragment Shader für 7x7 Gauss Tiefpassfilter
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

uniform vec2 offsets[49] = {	vec2(-3,  3), vec2(-2,  3), vec2(-1,  3), vec2(0,  3), vec2(1,  3), vec2(2,  3), vec2(3,  3), 
								vec2(-3,  2), vec2(-2,  2), vec2(-1,  2), vec2(0,  2), vec2(1,  2), vec2(2,  2), vec2(3,  2),
								vec2(-3,  1), vec2(-2,  1), vec2(-1,  1), vec2(0,  1), vec2(1,  1), vec2(2,  1), vec2(3,  1),
								vec2(-3,  0), vec2(-2,  0), vec2(-1,  0), vec2(0,  0), vec2(1,  0), vec2(2,  0), vec2(3,  0),
								vec2(-3, -1), vec2(-2, -1), vec2(-1, -1), vec2(0, -1), vec2(1, -1), vec2(2, -1), vec2(3, -1),
								vec2(-3, -2), vec2(-2, -2), vec2(-1, -2), vec2(0, -2), vec2(1, -2), vec2(2, -2), vec2(3, -2),
								vec2(-3, -3), vec2(-2, -3), vec2(-1, -3), vec2(0, -3), vec2(1, -3), vec2(2, -3), vec2(3, -3) };

void main()
{

	vec4 texel = vec4(0.0, 0.0, 0.0, 1.0);

	float sumWeights = 0;
	
	int sizeGauss = 7;
	int elements = sizeGauss*sizeGauss;

	float sigma = param1.x/4 + 1;
	sigma = sigma < 0.001 ? 0.001 : sigma;

	float f1 = 1/(2*3.141*sigma*sigma);
	float f2 = 2*sigma*sigma;

	float weights[7*7];

	//zusammenbauen des 7x7 filterkerns
	for(int x = -3; x <= 3; x++) {
		for(int  y= -3; y <= 3; y++) {
			weights[(y+3)+sizeGauss*(x+3)] = f1*exp(-(x*x + y*y))/f2;
		}
	}


    for (int i = 0; i < elements; i++)
    {
        texel += texture(textureMap, texCoords + offsets[i]) * weights[i];
		sumWeights += weights[i];
    }

    fragColor =		texel/sumWeights ;
}