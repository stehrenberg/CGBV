// Ausgangssoftware des 1. Praktikumsversuchs
// zur Vorlesung Computergrafik und Bildverarbeitung
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
#include <ImageLoader/ImageLoader.h>
#include <AntTweakBar.h>
#include "Timer.h"
GLMatrixStack modelViewMatrix;
GLMatrixStack projectionMatrix;
GLGeometryTransform transformPipeline;
GLFrustum viewFrustum;
GLBatch initialBatch;
GLBatch modelBatch;


#define PASS_THROUGH  0
#define BRI_CONTRAST  1
#define SHARPEN       2
#define DILATION      3
#define EROSION       4
#define LAPLACIAN     5
#define SOBEL         6
#define BEWMIT        7
#define GAUSS3x3      8
#define GAUSS5x5      9
#define GAUSS7x7      10
#define TOTAL_SHADERS 11

GLuint fShader[TOTAL_SHADERS];  // high-level shader object handles

const std::string shaderNames[TOTAL_SHADERS] = { "Passthrough", "Brightness_Contrast", "Sharpen", "Dilatation", "Erosion", "Laplace", "Sobel", "BewMit", "Gauss3x3", "Gauss5x5",
"Gauss7x7" };

GLint whichShader = PASS_THROUGH;       // default shader
GLint secondShader = PASS_THROUGH;       // second shader
GLint thirdShader = PASS_THROUGH;       // third shader
GLint fourthShader = PASS_THROUGH;       // 4th shader

GLint mainMenu, shaderMenu, secondShaderMenu, thirdShaderMenu, fourthShaderMenu, passMenu;  // menu handles

GLint numPasses = 0;

//Textur Id für die Texture-Map
const GLuint NumTex = 7;
GLuint TexId[NumTex + 1];

//Dateinamen für die Texture-Maps
const std::string TextureMapNames[] = {
	"../Texturen/e43_color_1280_720.bmp",
	"../Texturen/e43_sw_1280_720.bmp",
	"../Texturen/brickwork.jpg",
	"../Texturen/zonenplatte.bmp",
	"../Texturen/calculator.jpg",
	"../Texturen/Block6.BMP",
	"../Texturen/FLOOR.BMP"
};


// Rotationsgroessen
static GLfloat xRot = 0.0f;
static GLfloat yRot = 0.0f;
// Globale Variablen für Texturen und Filter
GLint winWidth = 1280;
GLint winHeight = 720;
static GLint texNum = 0;
static GLint filterNum = 0;

// Parameter für Shadersteuerung (4 Komponenten)
GLfloat Param1[4] = { 0.0, 0.0, 0.0, 0.0 };

//fps counter
int	FPS = 0, frames = 0;	// FPS and frame counter
int lastFPS = 0;			// last FPS check time
char fpsCount[256] = { 0 };


void TW_CALL GetShader1CB(void *value, void *clientData)
{
	*((GLuint*)value) = whichShader;
}

void TW_CALL GetShader2CB(void *value, void *clientData)
{
	*((GLuint*)value) = secondShader;
}

void TW_CALL GetShader3CB(void *value, void *clientData)
{
	*((GLuint*)value) = thirdShader;
}

void TW_CALL GetShader4CB(void *value, void *clientData)
{
	*((GLuint*)value) = fourthShader;
}

void TW_CALL SetShader1CB(const void *value, void *clientData)
{
	whichShader = *((GLuint*)value);
}

void TW_CALL SetShader2CB(const void *value, void *clientData)
{
	secondShader = *((GLuint*)value);
}

void TW_CALL SetShader3CB(const void *value, void *clientData)
{
	thirdShader = *((GLuint*)value);
}

void TW_CALL SetShader4CB(const void *value, void *clientData)
{
	fourthShader = *((GLuint*)value);
}

//GUI
TwBar *bar;
void InitGUI()
{
	std::string enumString;
	for (int i = 0; i < TOTAL_SHADERS; i++)
	{
		enumString += shaderNames[i];
		enumString += ',';
	}

	TwType shaderType = TwDefineEnumFromString("ShaderType", enumString.c_str());

	bar = TwNewBar("TweakBar");
	TwDefine(" TweakBar size='200 400'");
	TwAddVarRW(bar, "Passes", TW_TYPE_INT32, &numPasses, "min='0' max='3'");
	TwAddVarCB(bar, "First Shader", shaderType, SetShader1CB, GetShader1CB, NULL, NULL);
	TwAddVarCB(bar, "Second Shader", shaderType, SetShader2CB, GetShader2CB, NULL, NULL);
	TwAddVarCB(bar, "Third Shader", shaderType, SetShader3CB, GetShader3CB, NULL, NULL);
	TwAddVarCB(bar, "Fourth Shader", shaderType, SetShader4CB, GetShader4CB, NULL, NULL);
	//Hier weitere GUI Variablen anlegen. Für Farbe z.B. den Typ TW_TYPE_COLOR4F benutzen
}


//Timer 
Timer timer;
//---------------------------------------------------------------------------------------------
// Show FPS
//---------------------------------------------------------------------------------------------
void CalcFPS()
{
	if (timer.getTotalTime() >= 1.0f)
	{
		sprintf(fpsCount, "%d FPS x1000", FPS);					// build title string
		glutSetWindowTitle(fpsCount);
		FPS = 0;
		timer.restart();

	}
	else
	{
		FPS++;
	}

}

void CreateBatch()
{
	modelBatch.Reset();
	//Modell = bildschirmfüllendes Quad
	modelBatch.Begin(GL_TRIANGLE_STRIP, 4, 1);

	modelBatch.MultiTexCoord2f(0, 0.0, 0.0);
	modelBatch.Vertex3f(0.0, 0.0, 0.0);

	modelBatch.MultiTexCoord2f(0, 0.0, winHeight);
	modelBatch.Vertex3f(0.0, 1.0, 0.0);

	modelBatch.MultiTexCoord2f(0, winWidth, 0.0);
	modelBatch.Vertex3f(1.0, 0.0, 0.0);

	modelBatch.MultiTexCoord2f(0, winWidth, winHeight);
	modelBatch.Vertex3f(1.0, 1.0, 0.0);

	modelBatch.End();
}

void CreateGeometry()
{
	CreateBatch();

	initialBatch.Begin(GL_TRIANGLE_STRIP, 4, 1);

	initialBatch.MultiTexCoord2f(0, 0.0, 0.0);
	initialBatch.Vertex3f(0.0, 0.0, 0.0);

	initialBatch.MultiTexCoord2f(0, 0.0, 720);
	initialBatch.Vertex3f(0.0, 1.0, 0.0);

	initialBatch.MultiTexCoord2f(0, 1280, 0.0);
	initialBatch.Vertex3f(1.0, 0.0, 0.0);

	initialBatch.MultiTexCoord2f(0, 1280, 720);
	initialBatch.Vertex3f(1.0, 1.0, 0.0);

	initialBatch.End();

	//Shader Programme laden
	char fullFileName[255];

	for (int i = 0; i < TOTAL_SHADERS; i++)
	{
		sprintf(fullFileName, "./FragmentShader%s.glsl", shaderNames[i].c_str());
		fShader[i] = gltLoadShaderPairWithAttributes("./VertexShader.glsl", fullFileName, 2,
			GLT_ATTRIBUTE_VERTEX, "vVertex",
			GLT_ATTRIBUTE_TEXTURE0, "vTexCoord");

		gltCheckErrors(fShader[i]);
	}
}

// Aufruf draw scene
void RenderScene(void)
{
	CalcFPS();

	// Clearbefehle für den color buffer und den depth buffer
	glClear(GL_COLOR_BUFFER_BIT);

	// Speichere den matrix state
	modelViewMatrix.PushMatrix();

	glBindTexture(GL_TEXTURE_RECTANGLE, TexId[texNum]);
	//setze den Shader für das Rendern
	glUseProgram(fShader[whichShader]);

	// Model View Projection Matrix setzen
	glUniformMatrix4fv(glGetUniformLocation(fShader[whichShader], "mvpMatrix"), 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());
	// interaktive Parameter für Shader setzen
	glUniform4fv(glGetUniformLocation(fShader[whichShader], "param1"), 1, Param1);

	//Zeichne Model
	initialBatch.Draw();

	// Hole die im Stack gespeicherten Transformationsmatrizen wieder zurück
	modelViewMatrix.PopMatrix();

	glBindTexture(GL_TEXTURE_RECTANGLE, TexId[NumTex]);

	for (int pass = 0; pass < numPasses; pass++)
	{
		// Copy original scene to texture
		glCopyTexImage2D(GL_TEXTURE_RECTANGLE, 0, GL_RGB, 0, 0, winWidth, winHeight, 0);

		if (secondShader != 0 && (thirdShader == 0 || pass == 0))
		{
			glUseProgram(fShader[secondShader]);
			glUniformMatrix4fv(glGetUniformLocation(fShader[secondShader], "mvpMatrix"), 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());
			glUniform4fv(glGetUniformLocation(fShader[secondShader], "param1"), 1, Param1);
		}

		if ((thirdShader != 0 && pass == 1) || (thirdShader != 0 && fourthShader == 0 && pass > 0))
		{
			glUseProgram(fShader[thirdShader]);
			glUniformMatrix4fv(glGetUniformLocation(fShader[thirdShader], "mvpMatrix"), 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());
			glUniform4fv(glGetUniformLocation(fShader[thirdShader], "param1"), 1, Param1);
		}

		if (fourthShader != 0 && pass > 1)
		{
			glUseProgram(fShader[fourthShader]);
			glUniformMatrix4fv(glGetUniformLocation(fShader[fourthShader], "mvpMatrix"), 1, GL_FALSE, transformPipeline.GetModelViewProjectionMatrix());
			glUniform4fv(glGetUniformLocation(fShader[fourthShader], "param1"), 1, Param1);
		}

		// Lösche Framebuffer Farbe
		glClear(GL_COLOR_BUFFER_BIT);

		//Zeichne Model
		modelBatch.Draw();
	}

	TwDraw();
	gltCheckErrors(fShader[whichShader]);
	// Vertausche Front- und Backbuffer
	glutSwapBuffers();
	glutPostRedisplay();
}

// Initialisierung des Rendering Kontextes
void SetupRC()
{
	// Blauer Hintergrund
	glClearColor(0.12f, 0.35f, 0.674f, 0.0f);

	transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);

	//Erzeuge eindeutige Namen (Nummern) für Texturen und lade sie
	glGenTextures(NumTex + 1, TexId);

	img::ImageLoader imgLoader;
	for (int i = 0; i < NumTex; ++i)
	{
		//Aktive Textur setzen
		glBindTexture(GL_TEXTURE_RECTANGLE, TexId[i]);

		int width, height, type, internalformat;
		bool jpg;
		//Textur einlesen. Bei JPEG bildern das topdown flag auf true setzen, sonst stehen die Bilder auf den Kopf.
		if (i == 2) jpg = true;
		else jpg = false;
		// SW-Texturen erhalten das Attribut GL_LUMINANCE, Farbtexturen das Attribut GL_RGBA
		if (i == 1 || i == 3)
		{
			type = GL_LUMINANCE;
			internalformat = GL_LUMINANCE;
		}
		else
		{
			type = GL_RGBA;
			internalformat = GL_BGR;
		}
		unsigned char* data = imgLoader.LoadTextureFromFile(TextureMapNames[i], &width, &height, jpg);
		//Textur hochladen, bei JPEG bildern muss GL_BGR verwendet werden
		glTexImage2D(GL_TEXTURE_RECTANGLE, 0, type, width, height,
			0, internalformat, GL_UNSIGNED_BYTE, data);
		delete[] data;

		//Zugriffsflags setzen, wichtig!
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
		glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	}

	glBindTexture(GL_TEXTURE_RECTANGLE, TexId[NumTex]);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTexParameterf(GL_TEXTURE_RECTANGLE, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);


	//erzeuge die geometrie
	CreateGeometry();
	InitGUI();
}

void ShutDownRC()
{
	//Aufräumen
	for (int i = 0; i < TOTAL_SHADERS; i++)
	{
		glDeleteProgram(fShader[i]);
	}
	glDeleteTextures(NumTex + 1, TexId);
	TwTerminate();
}

void SpecialKeys(int key, int x, int y)
{
	if (key == GLUT_KEY_UP)
		xRot -= 5.0f;

	if (key == GLUT_KEY_DOWN)
		xRot += 5.0f;

	if (key == GLUT_KEY_LEFT)
		yRot -= 5.0f;

	if (key == GLUT_KEY_RIGHT)
		yRot += 5.0f;

	if (xRot > 356.0f)
		xRot = 0.0f;

	if (xRot < -1.0f)
		xRot = 355.0f;

	if (yRot > 356.0f)
		yRot = 0.0f;

	if (yRot < -1.0f)
		yRot = 355.0f;

	if (key == GLUT_KEY_F1)
		texNum += 1;

	if (key == GLUT_KEY_F2)
		texNum -= 1;

	if (key == GLUT_KEY_F3)
		filterNum += 1;

	if (key == GLUT_KEY_F4)
		filterNum -= 1;

	if (texNum == NumTex)
		texNum = NumTex - 1;

	if (texNum < 0)
		texNum = 0;

	if (filterNum > 9)
		filterNum = 9;

	if (filterNum < 0)
		filterNum = 0;

	if (key == GLUT_KEY_F5)
		Param1[0] += 1.0;

	if (key == GLUT_KEY_F6)
		Param1[0] -= 1.0;

	if (key == GLUT_KEY_F7)
		Param1[1] += 1.0;

	if (key == GLUT_KEY_F8)
		Param1[1] -= 1.0;

	if (key == GLUT_KEY_F9)
		Param1[2] += 1.0;

	if (key == GLUT_KEY_F10)
		Param1[2] -= 1.0;

	if (key == GLUT_KEY_F11)
		Param1[3] += 1.0;

	if (key == GLUT_KEY_F12)
		Param1[3] -= 1.0;

	TwEventKeyboardGLUT(key, x, y);
	// Zeichne das Window neu
	glutPostRedisplay();
}


void ChangeSize(int w, int h)
{
	//glutReshapeWindow(winWidth, winHeight);

	// Verhindere eine Division durch Null
	if (h == 0)
		h = 1;
	// Setze den Viewport gemaess der Window-Groesse
	glViewport(0, 0, w, h);
	// Ruecksetzung des Projection matrix stack
	projectionMatrix.LoadIdentity();
	viewFrustum.SetOrthographic(0, 1, 0, 1, -1, 1);
	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
	// Ruecksetzung des Model view matrix stack
	modelViewMatrix.LoadIdentity();
	modelViewMatrix.Translate(0, 0, 0);

	winWidth = w;
	winHeight = h;

	TwWindowSize(w, h);

	CreateBatch();
}

int main(int argc, char* argv[])
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("Aufgabe 1 - Bildverarbeitung");
	atexit(ShutDownRC);

	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		//Veralteter Treiber etc.
		std::cerr << "Error: " << glewGetErrorString(err) << "\n";
		return 1;
	}
	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(SpecialKeys);
	glutDisplayFunc(RenderScene);

	glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
	glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT); // same as MouseMotion
	glutKeyboardFunc((GLUTkeyboardfun)TwEventKeyboardGLUT);

	TwInit(TW_OPENGL, NULL);
	SetupRC();
	glutMainLoop();
	ShutDownRC();
	return 0;
}
