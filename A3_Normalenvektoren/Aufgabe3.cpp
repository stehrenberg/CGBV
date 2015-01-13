// Ausgangssoftware des 3. Praktikumsversuchs 
// zur Vorlesung Echtzeit-3D-Computergrahpik
// von Prof. Dr. Alfred Nischwitz
// Programm umgesetzt mit der GLTools Library
#include <iostream>
#ifdef WIN32
#include <windows.h>
#endif

#define GL_PI 3.142f

#include <iostream>
#include <GLTools.h>
#include <GLMatrixStack.h>
#include <GLGeometryTransform.h>
#include <GLFrustum.h>
#include <math.h>
#include <math3d.h>
#include <GL/glut.h>
#include <AntTweakBar.h>

GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;
GLGeometryTransform transformPipeline;
GLFrustum viewFrustum;
GLBatch top;
GLBatch bottom;
GLBatch plane;
GLBatch normalsBatch;
GLuint shaders;

/// View space light position
float light_pos[4] = {0.5f,0.1f,-5.0f,1.0f} ;

/// Lichtfarben
float light_ambient[4] = {0.0, 0.0, 0.0, 1.0}; 
float light_diffuse[4] = {0.9f,0.0f,0.5f,1.0f} ;
float light_specular[4] = {1.0f,1.0f,1.0f,1.0f} ;

//Materialeigenschaften
float mat_emissive[4] = {0.0, 0.0, 0.0, 1.0};
float mat_ambient[4]  = {0.0, 0.0, 0.0, 1.0}; 
float mat_diffuse[4]    = {1.0, 1.0, 1.0, 1.0};
float mat_specular[4]   = {1.0, 1.0, 1.0, 1.0};
float specular_power = 10 ;

// Rotationsgroessen
float rotation[] = {0, 0,0,0};

//GUI
TwBar *bar;

// GUI-Schalter
bool showNormals = false;
bool flatShading = true;

// Prototypen
void CreateGeometry(void);
void RenderScene(void);

/////////////////////////////// NORMALEN-VEKTOREN ANZEIGEN //////////////////////////////////////

void TW_CALL SetShowNormals(const void *value, void *clientData)
{
	//Pointer auf gesetzten Typ casten (der Typ der bei TwAddVarCB angegeben wurde)
	const bool* boolptr = static_cast<const bool*>(value);

	//Setzen der Variable auf neuen Wert
	showNormals = *boolptr;
}

void TW_CALL GetShowNormals(void *value, void *clientData)
{
	//Pointer auf gesetzten Typ casten (der Typ der bei TwAddVarCB angegeben wurde)
	bool* boolptr = static_cast<bool*>(value);

	//Variablen Wert and GUI weiterreichen
	*boolptr = showNormals;
}

/////////////////////////////// FLAT-SHADING ANZEIGEN //////////////////////////////////////

void TW_CALL SetFlatShading(const void *value, void *clientData)
{
	//Pointer auf gesetzten Typ casten (der Typ der bei TwAddVarCB angegeben wurde)
	const bool* boolptr = static_cast<const bool*>(value);

	//Setzen der Variable auf neuen Wert
	flatShading = *boolptr;
	
	CreateGeometry();
}

void TW_CALL GetFlatShading(void *value, void *clientData)
{
	//Pointer auf gesetzten Typ casten (der Typ der bei TwAddVarCB angegeben wurde)
	bool* boolptr = static_cast<bool*>(value);

	//Variablen Wert and GUI weiterreichen
	*boolptr = flatShading;
}

/////////////////////////////// MATERIALEIGENSCHAFTEN //////////////////////////////////////



////////////////////////////////////////////////////////////////////////////////

void InitGUI()
{
	bar = TwNewBar("TweakBar");
	TwDefine(" TweakBar size='200 400'"); 
	TwAddVarRW(bar,"Model Rotation",TW_TYPE_QUAT4F,&rotation,"");
	TwAddVarRW(bar,"Light Position", TW_TYPE_DIR3F,&light_pos,"group=Light axisx=-x axisy=-y axisz=-z");
	//Hier weitere GUI Variablen anlegen. Für Farbe z.B. den Typ TW_TYPE_COLOR4F benutzen
	TwAddVarRW(bar, "Light Position", TW_TYPE_DIR3F, &light_pos, "group=Light axisx=-x axisy=-y axisz=-z");
	TwAddVarRW(bar, "light_ambient", TW_TYPE_COLOR3F, &light_ambient, "group=Light axisx=-x axisy=-y axisz=-z");
	TwAddVarRW(bar, "light_diffuse", TW_TYPE_COLOR3F, &light_diffuse, "group=Light axisx=-x axisy=-y axisz=-z");
	TwAddVarRW(bar, "light_specular", TW_TYPE_COLOR3F, &light_specular, "group=Light axisx=-x axisy=-y axisz=-z");
	TwAddVarRW(bar, "mat_emissive", TW_TYPE_COLOR3F, &mat_emissive, "group=Material axisx=-x axisy=-y axisz=-z");
	TwAddVarRW(bar, "mat_ambient", TW_TYPE_COLOR3F, &mat_ambient, "group=Material axisx=-x axisy=-y axisz=-z");
	TwAddVarRW(bar, "mat_diffuse", TW_TYPE_COLOR3F, &mat_diffuse, "group=Material axisx=-x axisy=-y axisz=-z");
	TwAddVarRW(bar, "mat_specular", TW_TYPE_COLOR3F, &mat_specular, "group=Material axisx=-x axisy=-y axisz=-z");
	TwAddVarRW(bar, "Show Normals?", TW_TYPE_BOOLCPP, &showNormals, "");
	TwAddVarCB(bar, "Flat Shading?", TW_TYPE_BOOLCPP, SetFlatShading, GetFlatShading, NULL, "");
}

void CreateGeometry()
{

	float x, y, z;
	
	plane.Reset();
	normalsBatch.Reset();

	top.Begin(GL_TRIANGLES,192);
	plane.Begin(GL_TRIANGLES, 192);
	bottom.Begin(GL_TRIANGLES, 192);
	normalsBatch.Begin(GL_LINES, 576);

	// Deckel
	for (float angle = 0.0f; angle < 2 * GL_PI; angle += GL_PI / 16) {

		x = cos(angle);
		y = 1.0f;
		z = sin(angle);

		top.Normal3f(0, y, 0); 
		top.Vertex3f(0, y, 0);
		top.Normal3f(0, y, 0);
		top.Vertex3f(x, y, z);
		top.Normal3f(0, y, 0);
		top.Vertex3f(cos(angle + GL_PI / 16), y, sin(angle + GL_PI / 16));
		
		normalsBatch.Vertex3f(x, y, z);
		normalsBatch.Vertex3f(x, 2*y, z);
	}



	// Mantel
	for (float angle = 0.0f; angle < 2 * GL_PI; angle += GL_PI / 16) {
		x = cos(angle);
		y = 1.0f;
		z = sin(angle);
		float xNext = cos(angle + GL_PI / 16);
		float zNext = sin(angle + GL_PI / 16);

		if (flatShading) {
			//std::cerr << "bla" << std::endl;
			plane.Normal3f(cos(angle + GL_PI / 32), 0, sin(angle + GL_PI / 32));
			normalsBatch.Vertex3f(x, y, z);
			normalsBatch.Vertex3f(x + cos(angle + GL_PI / 32), y, z + sin(angle + GL_PI / 32));
		} else {
			plane.Normal3f(x, 0, z);
			normalsBatch.Vertex3f(x, y, z);
			normalsBatch.Vertex3f(2 * x, y, 2 * z);
		}
		plane.Vertex3f(x, y, z);		

		if (flatShading) {
			plane.Normal3f(cos(angle + GL_PI / 32), 0, sin(angle + GL_PI / 32));
			normalsBatch.Vertex3f(x, -y, z);
			normalsBatch.Vertex3f(x + cos(angle + GL_PI / 32), -y, z + sin(angle + GL_PI / 32));
		} else {
			plane.Normal3f(x, 0, z);
			normalsBatch.Vertex3f(x, -y, z);
			normalsBatch.Vertex3f(2 * x, -y, 2 * z);
		}
		plane.Vertex3f(x, -y, z);

		if (flatShading) {
			plane.Normal3f(cos(angle + GL_PI / 32), 0, sin(angle + GL_PI / 32));
			normalsBatch.Vertex3f(xNext, y, zNext);
			normalsBatch.Vertex3f(xNext + cos(angle + GL_PI / 32), y, zNext + sin(angle + GL_PI / 32));
		} else {
			plane.Normal3f(xNext, 0, zNext);
			normalsBatch.Vertex3f(xNext, y, zNext);
			normalsBatch.Vertex3f(2 * xNext, y, 2 * zNext);
		}	
		plane.Vertex3f(xNext, y, zNext);

		if (flatShading) {
			plane.Normal3f(cos(angle + GL_PI / 32), 0, sin(angle + GL_PI / 32));
			normalsBatch.Vertex3f(xNext, y, zNext);
			normalsBatch.Vertex3f(xNext + cos(angle + GL_PI / 32), y, zNext + sin(angle + GL_PI / 32));
		} else {
			plane.Normal3f(xNext, 0, zNext);
			normalsBatch.Vertex3f(xNext, y, zNext);
			normalsBatch.Vertex3f(2 * xNext, y, 2 * zNext);
		}			
		plane.Vertex3f(xNext, y, zNext);
		

		if (flatShading) {
			plane.Normal3f(cos(angle + GL_PI / 32), 0, sin(angle + GL_PI / 32));
			normalsBatch.Vertex3f(xNext, -y, zNext);
			normalsBatch.Vertex3f(xNext + cos(angle + GL_PI / 32), -y, zNext + sin(angle + GL_PI / 32));
		} else {
			plane.Normal3f(x, 0, z);
			normalsBatch.Vertex3f(x, -y, z);
			normalsBatch.Vertex3f(2 * x, -y, 2 * z);
		}			
		plane.Vertex3f(x, -y, z);
		

		if (flatShading) {
			plane.Normal3f(cos(angle + GL_PI / 32), 0, sin(angle + GL_PI / 32));
			normalsBatch.Vertex3f(xNext, -y, zNext);
			normalsBatch.Vertex3f(xNext + cos(angle + GL_PI / 32), -y, zNext + sin(angle + GL_PI / 32));
		}
		else {
			plane.Normal3f(xNext, 0, zNext);
			normalsBatch.Vertex3f(xNext, -y, zNext);
			normalsBatch.Vertex3f(2 * xNext, -y, 2 * zNext);
		}			
		plane.Vertex3f(xNext, -y, zNext);
		
	}
		
	for (float angle = 0.0f; angle < 2 * GL_PI; angle += GL_PI / 16) {
		x = cos(angle);
		y = 1.0f;
		z = sin(angle);
		float xNext = cos(angle + GL_PI / 16);
		float zNext = sin(angle + GL_PI / 16);
		// Boden
		bottom.Normal3f(0, -y, 0);
		bottom.Vertex3f(0, -y, 0);
		bottom.Normal3f(0, -y, 0);
		bottom.Vertex3f(x, -y, z);
		bottom.Normal3f(0, -y, 0);
		bottom.Vertex3f(xNext, -y, zNext);
		normalsBatch.Vertex3f(x, -y, z);
		normalsBatch.Vertex3f(x, 2 * -y, z);
	}	
		
	top.End();
	plane.End();
	bottom.End();
	normalsBatch.End();
	
	//Shader Programme laden. Die letzen Argumente geben die Shader-Attribute an. Hier wird Vertex und Normale gebraucht.
	shaders =  gltLoadShaderPairWithAttributes("VertexShader.glsl", "FragmentShader.glsl", 2, 
		GLT_ATTRIBUTE_VERTEX, "vVertex", 
		GLT_ATTRIBUTE_NORMAL, "vNormal");
	
	gltCheckErrors(shaders);
}

// Aufruf draw scene
void RenderScene(void)
{
	// Clearbefehle für den color buffer und den depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	modelViewMatrix.LoadIdentity();
	modelViewMatrix.Translate(0,0,-5);
	// Speichere den matrix state und führe die Rotation durch
	modelViewMatrix.PushMatrix();

	M3DMatrix44f rot;
	m3dQuatToRotationMatrix(rot,rotation);
	modelViewMatrix.MultMatrix(rot);
	
	//setze den Shader für das Rendern
	glUseProgram(shaders);
	// Matrizen an den Shader übergeben
	glUniformMatrix4fv(glGetUniformLocation(shaders, "mvpMatrix"), 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());
	glUniformMatrix4fv(glGetUniformLocation(shaders, "mvMatrix"),  1, GL_FALSE, transformPipeline.GetModelViewMatrix());
	glUniformMatrix3fv(glGetUniformLocation(shaders, "normalMatrix"),  1, GL_FALSE, transformPipeline.GetNormalMatrix(true));
	// Lichteigenschaften übergeben
	glUniform4fv(glGetUniformLocation(shaders, "light_pos_vs"),1,light_pos);
	glUniform4fv(glGetUniformLocation(shaders, "light_ambient"),1,light_ambient);
	glUniform4fv(glGetUniformLocation(shaders, "light_diffuse"),1,light_diffuse);
	glUniform4fv(glGetUniformLocation(shaders, "light_specular"),1,light_specular);
	glUniform1f(glGetUniformLocation(shaders, "spec_power"),specular_power);
	//Materialeigenschaften übergeben
	glUniform4fv(glGetUniformLocation(shaders, "mat_emissive"),1,mat_emissive);
	glUniform4fv(glGetUniformLocation(shaders, "mat_ambient"),1,mat_ambient);
	glUniform4fv(glGetUniformLocation(shaders, "mat_diffuse"),1,mat_diffuse);
	glUniform4fv(glGetUniformLocation(shaders, "mat_specular"),1,mat_specular);
	
	//Zeichne Model
	top.Draw();
	bottom.Draw();
	plane.Draw();
	if (showNormals)
		normalsBatch.Draw();

	// Hole die im Stack gespeicherten Transformationsmatrizen wieder zurück
	modelViewMatrix.PopMatrix();
	// Draw tweak bars
	TwDraw();
	gltCheckErrors(shaders);
	// Vertausche Front- und Backbuffer
	glutSwapBuffers();
	glutPostRedisplay();
}

// Initialisierung des Rendering Kontextes
void SetupRC()
{
	// Schwarzer Hintergrund
	glClearColor( 0.12f,0.35f,0.674f,0.0f ) ;

	// In Uhrzeigerrichtung zeigende Polygone sind die Vorderseiten.
	// Dies ist umgekehrt als bei der Default-Einstellung weil wir Triangle_Fans benützen
	glFrontFace(GL_CCW);
		
	transformPipeline.SetMatrixStacks(modelViewMatrix,projectionMatrix);
	//erzeuge die geometrie
	CreateGeometry();
	InitGUI();
}

void ShutDownRC()
{
	glDeleteProgram(shaders);
	TwTerminate();
}

void ChangeSize(int w, int h)
{
	// Verhindere eine Division durch Null
	if(h == 0)
		h = 1;
	// Setze den Viewport gemaess der Window-Groesse
	glViewport(0, 0, w, h);
	// Ruecksetzung des Projection matrix stack
	projectionMatrix.LoadIdentity();
	viewFrustum.SetPerspective(45,w/(float)h,1,100);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
	// Ruecksetzung des Model view matrix stack
	modelViewMatrix.LoadIdentity();

	// Send the new window size to AntTweakBar
	TwWindowSize(w, h);
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800,600);
	glutCreateWindow("A3 Normalenvektoren");
	atexit(ShutDownRC);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		//Veralteter Treiber etc.
		std::cerr <<"Error: "<< glewGetErrorString(err) << "\n";
		return 1;
	}
	glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
	glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT); // same as MouseMotion
	glutKeyboardFunc((GLUTkeyboardfun)TwEventKeyboardGLUT);
	glutSpecialFunc((GLUTspecialfun)TwEventKeyboardGLUT);

	glutReshapeFunc(ChangeSize);
	glutDisplayFunc(RenderScene);
	TwInit(TW_OPENGL, NULL);
	SetupRC();
	glutMainLoop();
	ShutDownRC();
	return 0;
}
