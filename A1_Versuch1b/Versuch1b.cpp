// Versuch1b.cpp
// Ausgangssoftware des 1. Praktikumsversuchs 
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

GLShaderManager shaderManager;
GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;
GLGeometryTransform transformPipeline;
GLFrustum viewFrustum;
GLBatch konus;
GLBatch boden;
// Definition der Kreiszahl 
#define GL_PI 3.1415f

// Rotationsgroessen
static float rotation[] = { 0, 0, 0, 0 };

// Flags fuer Schalter
bool bCull = false;
bool bOutline = false;
bool bDepth = true;
bool bSides = false;


//GUI
TwBar *bar;
void InitGUI()
{
	bar = TwNewBar("TweakBar");
	TwDefine(" TweakBar size='200 400'");
	TwAddVarRW(bar, "Model Rotation", TW_TYPE_QUAT4F, &rotation, "");
	TwAddVarRW(bar, "Depth Test?", TW_TYPE_BOOLCPP, &bDepth, "");
	TwAddVarRW(bar, "Culling?", TW_TYPE_BOOLCPP, &bCull, "");
	TwAddVarRW(bar, "Backface Wireframe?", TW_TYPE_BOOLCPP, &bOutline, "");
	TwAddVarRW(bar, "Switch Front / Back?", TW_TYPE_BOOLCPP, &bSides, "");
	//Hier weitere GUI Variablen anlegen. Für Farbe z.B. den Typ TW_TYPE_COLOR4F benutzen
}

void CreateGeometry()
{
	// Triangle Fan mit 18 Vertices anlegen
	konus.Begin(GL_TRIANGLE_FAN, 18);
	// Die Spitze des Konus ist ein Vertex, den alle Triangles gemeinsam haben;
	// um einen Konus anstatt einen Kreis zu produzieren muss der Vertex einen positiven z-Wert haben
	konus.Vertex3f(0, 0, 75);
	konus.Color4f(0, 1, 0, 1);
	// Kreise um den Mittelpunkt und spezifiziere Vertices entlang des Kreises
	// um einen Triangle_Fan zu erzeugen
	int iPivot = 1;
	for (float angle = 0.0f; angle < (2.0f * GL_PI); angle += (GL_PI / 8.0f))
	{
		// Berechne x und y Positionen des naechsten Vertex
		float x = 50.0f*sin(angle);
		float y = 50.0f*cos(angle);

		// Alterniere die Farbe zwischen Rot und Gruen
		if ((iPivot % 2) == 0)
			konus.Color4f(0.235, 0.235, 0.235, 1);
		else
			konus.Color4f(0, 0.6, 1, 1);

		// Inkrementiere iPivot um die Farbe beim naechsten mal zu wechseln
		iPivot++;

		// Spezifiziere den naechsten Vertex des Triangle_Fans
		konus.Vertex3f(x, y, 0);
	}

	// Fertig mit dem Konus
	konus.End();


	// Erzeuge einen weiteren Triangle_Fan um den Boden zu bedecken
	boden.Begin(GL_TRIANGLE_FAN, 18);
	// Das Zentrum des Triangle_Fans ist im Ursprung
	boden.Vertex3f(0.0f, 0.0f, 0.0f);
	for (float angle = (2.0f * GL_PI); angle > 0.0f; angle -= (GL_PI / 8.0f))
	{
		// Berechne x und y Positionen des naechsten Vertex
		float x = 50.0f*sin(angle);
		float y = 50.0f*cos(angle);

		// Alterniere die Farbe zwischen Rot und Gruen
		if ((iPivot % 2) == 0)
			boden.Color4f(1, 0.8, 0.2, 1);
		else
			boden.Color4f(0, 0.8, 0, 1);

		// Inkrementiere iPivot um die Farbe beim naechsten mal zu wechseln
		iPivot++;

		// Spezifiziere den naechsten Vertex des Triangle_Fans
		boden.Vertex3f(x, y, 0);
	}

	// Fertig mit dem Bodens
	boden.End();

}

// Aufruf draw scene
void RenderScene(void)
{
	// Clearbefehle für den color buffer und den depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Schalte culling ein falls das Flag gesetzt ist
	if (bCull) {
		glEnable(GL_CULL_FACE);
		glCullFace(GL_BACK);
	}

	else
		glDisable(GL_CULL_FACE);

	//if (bSides)
	//	glCullFace(GL_BACK);

	// Schalte depth testing ein falls das Flag gesetzt ist
	if (bDepth)
		glEnable(GL_DEPTH_TEST);
	else
		glDisable(GL_DEPTH_TEST);

	// Zeichne die Rückseite von Polygonen als Drahtgitter falls das Flag gesetzt ist
	if (bOutline)
		glPolygonMode(GL_BACK, GL_LINE);
	else
		glPolygonMode(GL_BACK, GL_FILL);

	// Speichere den matrix state und führe die Rotation durch
	modelViewMatrix.PushMatrix();
	M3DMatrix44f rot;
	m3dQuatToRotationMatrix(rot, rotation);
	modelViewMatrix.MultMatrix(rot);

	//setze den Shader für das Rendern und übergebe die Model-View-Projection Matrix
	shaderManager.UseStockShader(GLT_SHADER_FLAT_ATTRIBUTES, transformPipeline.GetModelViewProjectionMatrix());
	//Zeichne Konus und Boden
	konus.Draw();
	boden.Draw();
	//Auf fehler überprüfen
	gltCheckErrors(0);
	// Hole die im Stack gespeicherten Transformationsmatrizen wieder zurück
	modelViewMatrix.PopMatrix();

	TwDraw();
	// Vertausche Front- und Backbuffer
	glutSwapBuffers();
	glutPostRedisplay();
}

// Initialisierung des Rendering Kontextes
void SetupRC()
{
	// Schwarzer Hintergrund
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	// In Uhrzeigerrichtung zeigende Polygone sind die Vorderseiten.
	// Dies ist umgekehrt als bei der Default-Einstellung weil wir Triangle_Fans benützen
	if (bSides)
		glFrontFace(GL_CW);
	else
		glFrontFace(GL_CCW);


	//initialisiert die standard shader
	shaderManager.InitializeStockShaders();
	//Matrix stacks für die Transformationspipeline setzen, damit werden dann automatisch die Matrizen multipliziert
	transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);
	//erzeuge die geometrie
	CreateGeometry();
	InitGUI();
}

void SpecialKeys(int key, int x, int y)
{
	TwEventKeyboardGLUT(key, x, y);
	// Zeichne das Window neu
	glutPostRedisplay();
}


void ChangeSize(int w, int h)
{
	GLfloat nRange = 100.0f;

	// Verhindere eine Division durch Null
	if (h == 0)
		h = 1;
	// Setze den Viewport gemaess der Window-Groesse
	glViewport(0, 0, w, h);
	// Ruecksetzung des Projection matrix stack
	projectionMatrix.LoadIdentity();

	// Definiere das viewing volume (left, right, bottom, top, near, far)
	if (w <= h)
		viewFrustum.SetOrthographic(-nRange, nRange, -nRange*h / w, nRange*h / w, -nRange, nRange);
	else
		viewFrustum.SetOrthographic(-nRange*w / h, nRange*w / h, -nRange, nRange, -nRange, nRange);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
	// Ruecksetzung des Model view matrix stack
	modelViewMatrix.LoadIdentity();

	TwWindowSize(w, h);
}

void ShutDownRC()
{
	TwTerminate();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 600);
	glutCreateWindow("Versuch1");

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		// Veralteter Treiber etc.
		std::cerr << "Error: " << glewGetErrorString(err) << "\n";
		return 1;
	}

	glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
	glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT); // same as MouseMotion
	glutKeyboardFunc((GLUTkeyboardfun)TwEventKeyboardGLUT);

	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(SpecialKeys);
	glutDisplayFunc(RenderScene);

	TwInit(TW_OPENGL, NULL);
	SetupRC();
	glutMainLoop();
	ShutDownRC();

	return 0;
}
