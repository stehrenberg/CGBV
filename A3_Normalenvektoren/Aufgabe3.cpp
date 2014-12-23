// Ausgangssoftware des 3. Praktikumsversuchs 
// zur Vorlesung Echtzeit-3D-Computergrahpik
// von Prof. Dr. Alfred Nischwitz
// Programm umgesetzt mit der GLTools Library
#include <iostream>
#ifdef WIN32
#include <windows.h>
#endif

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
GLBatch geometryBatch;
GLuint shaders;

// Prototypen
void CreateTriangle(void);

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

void InitGUI()
{
	bar = TwNewBar("TweakBar");
	TwDefine(" TweakBar size='200 400'"); 
	TwAddVarRW(bar,"Model Rotation",TW_TYPE_QUAT4F,&rotation,"");
	TwAddVarRW(bar,"Light Position", TW_TYPE_DIR3F,&light_pos,"group=Light axisx=-x axisy=-y axisz=-z");
	//Hier weitere GUI Variablen anlegen. Für Farbe z.B. den Typ TW_TYPE_COLOR4F benutzen
}
void CreateGeometry()
{
	CreateTriangle();
	
	//Shader Programme laden. Die letzen Argumente geben die Shader-Attribute an. Hier wird Vertex und Normale gebraucht.
	shaders =  gltLoadShaderPairWithAttributes("VertexShader.glsl", "FragmentShader.glsl", 2, 
		GLT_ATTRIBUTE_VERTEX, "vVertex", 
		GLT_ATTRIBUTE_NORMAL, "vNormal");
	
	gltCheckErrors(shaders);
}

void CreateTriangle() {
	geometryBatch.Begin(GL_TRIANGLES,3);

	geometryBatch.Normal3f(0,0,1);
	geometryBatch.Vertex3f(-1,-1,0);

	geometryBatch.Normal3f(0,0,1);
	geometryBatch.Vertex3f(0,1,0);

	geometryBatch.Normal3f(0,0,1);
	geometryBatch.Vertex3f(1,-1,0);

	geometryBatch.End();
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
	geometryBatch.Draw();

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
