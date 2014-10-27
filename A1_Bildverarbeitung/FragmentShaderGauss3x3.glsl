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

uniform vec2 offsets[9] = {		vec2(-1,  1),
								vec2(-1,  0),
								vec2(-1, -1),
								vec2( 0,  1),
								vec2( 0,  0),
								vec2( 0, -1),
								vec2( 1,  1),
								vec2( 1,  0),
								vec2( 1, -1) };


void main()
{

	vec4 texel = vec4(0.0, 0.0, 0.0, 1.0);

	float sumWeights = 0;
	
	int sizeGauss = 3;
	int elements = sizeGauss*sizeGauss;

	float sigma = param1.x;
	sigma = sigma < 0.001 ? 0.001 : sigma;

	float f1 = 1/(2*3.141*sigma*sigma);
	float f2 = 2*sigma*sigma;

	float weights[9];

	//zusammenbauen des 3x3 filterkerns
	for(int x = -1; x <= 1; x++) {
		for(int  y= -1; y <= 1; y++) {
			weights[(y+1)+sizeGauss*(x+1)] = f1*exp(-(x*x + y*y))/f2;
		}
	}


    for (int i = 0; i < elements; i++)
    {
        texel += texture(textureMap, texCoords + offsets[i]) * weights[i];
		sumWeights += weights[i];
    }

    fragColor =		texel/sumWeights ;
}