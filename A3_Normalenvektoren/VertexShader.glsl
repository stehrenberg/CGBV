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
#version 130
//Attribute 
in vec4 vVertex;
in vec4 vNormal;

uniform mat4 mvpMatrix;
uniform mat4 mvMatrix;
uniform mat3 normalMatrix;
/// Light position in Eye-Space
uniform vec4 light_pos_vs ;
/// Light diffuse color
uniform vec4 light_diffuse ;
uniform vec4 light_ambient ;
/// Light specular color
uniform vec4 light_specular ;
/// Specular power (shininess)
uniform float spec_power;

/// Material parameters
uniform vec4 mat_emissive;
uniform vec4 mat_ambient; 
uniform vec4 mat_diffuse;   
uniform vec4 mat_specular;  

//Übergabe an den Fragment-Shader
out vec4 color;
void main()
{	
	
	// Transformiere Vertex von Objekt- in den Clip-Space
	gl_Position = mvpMatrix * vVertex ;

	// Transformiere Vertex von Object in den Eye-Space
	vec4 vertex_vs = mvMatrix * vVertex;
	vec3 ecPos = vertex_vs.xyz / vertex_vs.w;
	// Berechne Lichtrichtung in Eye-Space
	
	vec3 light_dir_vs = normalize(light_pos_vs.xyz-ecPos) ; // Punkt Lichtquelle
  //vec3 light_dir_vs = normalize(vec3(light_pos_vs.xyz)); // gerichtetes Licht

	// Normalen von Objekt- in Eye-Space transformieren
	vec3 normal_vs = normalize(normalMatrix * (vNormal).xyz) ;
	
	// Betrachtervektor in Eye-Space
	  vec3 view_dir_vs = normalize(-ecPos ) ; // lokaler Betrachtervektor im Eye-Space
    //vec3 light_dir_vs = vec3( 0.0, 0.0, 1.0); // infiniter Betrachtervektor im Eye-Space
	
	// Halfway Vektor für das Phong-Blinn Beleuchtungsmodell berechnen
	vec3 halfway_vs = normalize(view_dir_vs + light_dir_vs ) ;
	
	// Diffuser Term berechnen
	float NdotL = max(dot(normal_vs, light_dir_vs),0.0) ;
	vec4 diffuse_color = NdotL * mat_diffuse * light_diffuse ;

	// Spekularen Term berechnen
	float NdotH = max(dot(normal_vs, halfway_vs),0.0) ;
	vec4 specular_color = pow(NdotH ,spec_power) * mat_specular * light_specular ;

	// Alle Fraben addieren
	color = mat_emissive + light_ambient*mat_ambient + diffuse_color + specular_color;
}