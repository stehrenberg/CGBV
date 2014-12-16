// Versuch1c.cpp
// Ausgangssoftware des 1. Praktikumsversuchs 
// zur Vorlesung Echtzeit-3D-Computergrahpik
// von Prof. Dr. Alfred Nischwitz
// Programm umgesetzt mit der GLTools Library und Vertex-Arrays
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
GLBatch kboden;
GLBatch cboden;
GLBatch decke;
GLBatch kopf;
GLBatch fuss;
GLBatch plane;
GLBatch sphereBody;


// Farbvektoren
//M3DVector4f green = (1.0);
//M3DVector4f lightgreen = (1.0);
//M3DVector4f blue = { 0.0f, 0.6f, 1.0f, 1.0f };
//M3DVector4f red = (1.0);
//M3DVector4f yellow = (1.0);
//M3DVector4f grey = {0.235f, 0.235f, 0.235f, 1.0f};
//M3DVector4f skincolored = (1.0);

// Definition der Kreiszahl 
#define GL_PI 3.142f

// Rotationsgroessen
static float rotation[] = { 0, 0, 0, 0 };

// Vektor fuer UFO-Bewegungsmodus
M3DVector3f ufoMove = { 0.0f, 0.0f, 0.0f};
M3DVector3f ufoTurning = { 0.0f, 0.0f, 0.0f };

// Flags fuer Schalter
bool bCull = false;
bool bOutline = false;
bool bDepth = true;
unsigned int scale = 1;
unsigned int tess = 5;
bool bPerspectiveProj = false;
bool bVertAxisInverted = true;

unsigned int tesselation = pow(2, tess);
unsigned int arraySize = 2 * tesselation + 2;
unsigned int doubleArraySize = 2 * arraySize;
unsigned int sphereArraySize = (tesselation*(arraySize - 1) + 1) * 2;
double animationAngle = 0;
float viewAngleX = 0.0f;
float viewAngleY = 0.0f;
int windowWidth = 800;
int windowHeight = 600;

//Prototypen
void CreateCone(float, float, float);
void CreateCube(float, float, float);
void CreateCylinder(float, float, float);
void CreateSphere(float, float, float);
void RenderScene();
void DrawMaennchen(float angle);
void DrawLimbs(float angle);
void DrawUpperLimb(float angle);
void DrawLowerLimb(float angle);
void ChangeSize(int w, int h);

M3DVector4f blue = {0.0, 0.0, 1.0, 1.0};

/////////////////////////////// GROESSE AENDERN ////////////////////////////////////////

void TW_CALL SetScale(const void *value, void *clientData)
{
	//Pointer auf gesetzten Typ casten (der Typ der bei TwAddVarCB angegeben wurde)
	const unsigned int* uintptr = static_cast<const unsigned int*>(value);

	//Setzen der Variable auf neuen Wert
	scale = *uintptr;

	//Hier kann nun der Aufruf gemacht werden um die Geometrie mit neuem Scalefaktor zu erzeugen
	//CreateCone(0.0f, 0.0f, -100.0f);
	CreateCube(-75.0f, 0.0f, 0.0f);
	CreateCylinder(0.0f, 125.0f, 0.0f);
	CreateSphere(100.0f, 0.0f, 0.0f);

	RenderScene();
}

void TW_CALL GetScale(void *value, void *clientData)
{
	//Pointer auf gesetzten Typ casten (der Typ der bei TwAddVarCB angegeben wurde)
	unsigned int* uintptr = static_cast<unsigned int*>(value);

	//Variablen Wert and GUI weiterreichen
	*uintptr = scale;
}

//////////////////////////////////// TESSELIERUNG VARIIEREN ////////////////////////////

void TW_CALL SetTesselation(const void *value, void *clientData)
{
	//Pointer auf gesetzten Typ casten (der Typ der bei TwAddVarCB angegeben wurde)
	const unsigned int* uintptr = static_cast<const unsigned int*>(value);

	//Setzen der Variablen auf neuen Wert
	tess = *uintptr;
	tesselation = pow(2, tess);
	arraySize = 2 * tesselation + 2;
	doubleArraySize = 2 * arraySize;
	sphereArraySize = (tesselation*(arraySize - 1) + 1) * 2;

	//Hier kann nun der Aufruf gemacht werden um die Geometrie mit neuem Tesselierungsfaktor zu erzeugen
	//CreateCone(0.0f, 0.0f, -100.0f);
	CreateCube(-75.0f, 0.0f, 0.0f);
	CreateCylinder(0.0f, 125.0f, 0.0f);
	CreateSphere(100.0f, 0.0f, 0.0f);

	RenderScene();
}

void TW_CALL GetTesselation(void *value, void *clientData)
{
	//Pointer auf gesetzten Typ casten (der Typ der bei TwAddVarCB angegeben wurde)
	unsigned int* uintptr = static_cast<unsigned int*>(value);

	//Variablen Wert and GUI weiterreichen
	*uintptr = tess;
}

//////////////////////////////// PROJEKTIONSMODUS AENDERN ///////////////////////////////

void TW_CALL SetProjection(const void *value, void *clientData)
{

	//Pointer auf gesetzten Typ casten (der Typ der bei TwAddVarCB angegeben wurde)
	const bool* boolptr = static_cast<const bool*>(value);

	//Setzen der Variable auf neuen Wert
	bPerspectiveProj = *boolptr;
	
	// Neuaufruf zum Rendern des Bildes
	ChangeSize(windowWidth, windowHeight);
}

void TW_CALL GetProjection(void *value, void *clientData)
{
	//Pointer auf gesetzten Typ casten (der Typ der bei TwAddVarCB angegeben wurde)
	bool* boolptr = static_cast<bool*>(value);

	//Variablen Wert and GUI weiterreichen
	*boolptr = bPerspectiveProj;
}

/////////////////////////////// INVERSION Y-ACHSE //////////////////////////////////////
void TW_CALL SetYAchsisInversion(const void *value, void *clientData)
{
	//Pointer auf gesetzten Typ casten (der Typ der bei TwAddVarCB angegeben wurde)
	const bool* boolptr = static_cast<const bool*>(value);

	//Setzen der Variable auf neuen Wert
	bVertAxisInverted = *boolptr;

	// Neuaufruf zum Rendern des Bildes
	ChangeSize(windowWidth, windowHeight);
}

void TW_CALL GetYAchsisInversion(void *value, void *clientData)
{
	//Pointer auf gesetzten Typ casten (der Typ der bei TwAddVarCB angegeben wurde)
	bool* boolptr = static_cast<bool*>(value);

	//Variablen Wert and GUI weiterreichen
	*boolptr = bVertAxisInverted;
}

////////////////////////////////////////////////////////////////////////////////


//GUI
TwBar *bar;
void InitGUI() {
	bar = TwNewBar("TweakBar");
	TwDefine(" TweakBar size='200 400'");
	TwAddVarRW(bar, "Model Rotation", TW_TYPE_QUAT4F, &rotation, "");
	TwAddVarRW(bar, "Depth Test?", TW_TYPE_BOOLCPP, &bDepth, "");
	TwAddVarRW(bar, "Culling?", TW_TYPE_BOOLCPP, &bCull, "");
	TwAddVarRW(bar, "Backface Wireframe?", TW_TYPE_BOOLCPP, &bOutline, "");
	TwAddVarCB(bar, "Groesse", TW_TYPE_UINT32, SetScale, GetScale, NULL, "");
	TwAddVarCB(bar, "Tess.stufe", TW_TYPE_UINT32, SetTesselation, GetTesselation, NULL, "");
	TwAddVarCB(bar, "Perspective Projection?", TW_TYPE_BOOLCPP, SetProjection, GetProjection, NULL, "");
	TwAddVarCB(bar, "Y-Achse invertieren?", TW_TYPE_BOOLCPP, SetYAchsisInversion, GetYAchsisInversion, NULL, "");

	//Hier weitere GUI Variablen anlegen. Für Farbe z.B. den Typ TW_TYPE_COLOR4F benutzen
}

void CreateCone(float xShift, float yShift, float zShift) {

	//18 Vertices anlegen
	M3DVector3f* konusVertices = new M3DVector3f[18];
	M3DVector4f* konusColors = new M3DVector4f[18];

	float radius = 50.0f * scale;
	float height = 75.0f * scale;
	
	// Die Spitze des Konus ist ein Vertex, den alle Triangles gemeinsam haben;
	// um einen Konus anstatt einen Kreis zu produzieren muss der Vertex einen positiven z-Wert haben
	m3dLoadVector3(konusVertices[0], xShift, height + yShift, zShift);
	m3dLoadVector4(konusColors[0], 0.0f, 1.0f, 0.0f, 1.0f);

	// Kreise um den Mittelpunkt und spezifiziere Vertices entlang des Kreises
	// um einen Triangle_Fan zu erzeugen
	int iPivot = 1;
	int i = 1;
	for (float angle = 0.0f; angle < (2.0f*GL_PI); angle += (GL_PI / 8))
	{
		// Berechne x und y Positionen des naechsten Vertex
		float x = radius*cos(angle);
		float z = radius*sin(angle);

		// Alterniere die Farbe zwischen Rot und Gruen
		if ((iPivot % 2) == 0)
			m3dLoadVector4(konusColors[i],0.9f, 1.0f, 0.9f, 1.0f);
		else
			m3dLoadVector4(konusColors[i], 0.0f, 0.4f, 0.2f, 1.0f);

		// Inkrementiere iPivot um die Farbe beim naechsten mal zu wechseln
		iPivot++;

		// Spezifiziere den naechsten Vertex des Triangle_Fans
		m3dLoadVector3(konusVertices[i], x + xShift, yShift, z + zShift);
		i++;
	}

	konus.Begin(GL_TRIANGLE_FAN, 18);
	konus.CopyVertexData3f(konusVertices);
	konus.CopyColorData4f(konusColors);
	konus.End();

	delete[] konusVertices;
	delete[] konusColors;



	// Erzeuge einen weiteren Triangle_Fan um den Boden zu bedecken
	M3DVector3f* bodenVertices = new M3DVector3f[18];
	M3DVector4f* bodenColors = new M3DVector4f[18];
	// Das Zentrum des Triangle_Fans ist im Ursprung
	m3dLoadVector3(bodenVertices[0], 0, 0, zShift);
	m3dLoadVector4(bodenColors[0], 1, 0, 0, 1);

	i = 1;
	for (float angle = 0.0f; angle < (2.0f*GL_PI); angle += (GL_PI / 8)) {
		// Berechne x und y Positionen des naechsten Vertex
		float x = radius*sin(angle);
		float z = radius*cos(angle);

		m3dLoadVector4(bodenColors[i], 0.0f, 0.2f, 0.0f, 1.0f);
		// Inkrementiere iPivot um die Farbe beim naechsten mal zu wechseln
		iPivot++;

		// Spezifiziere den naechsten Vertex des Triangle_Fans
		m3dLoadVector3(bodenVertices[i], x + xShift, yShift, z + zShift);
		i++;
	}

	kboden.Begin(GL_TRIANGLE_FAN, 18);
	kboden.CopyVertexData3f(bodenVertices);
	kboden.CopyColorData4f(bodenColors);
	kboden.End();

	delete[] bodenVertices;
	delete[] bodenColors;
}

void CreateCube(float xShift, float yShift, float zShift) {

	float edgeLength = 25.0f * scale;

	M3DVector3f bodenVertices[8];
	M3DVector4f bodenColors[8];

	m3dLoadVector3(bodenVertices[0], -edgeLength + xShift, -edgeLength + yShift, -edgeLength + zShift);
	m3dLoadVector3(bodenVertices[1], edgeLength + xShift, -edgeLength + yShift, -edgeLength + zShift);
	m3dLoadVector3(bodenVertices[2], edgeLength + xShift, edgeLength + yShift, -edgeLength + zShift);
	m3dLoadVector3(bodenVertices[3], -edgeLength + xShift, edgeLength + yShift, -edgeLength + zShift);
	m3dLoadVector3(bodenVertices[4], -edgeLength + xShift, edgeLength + yShift, edgeLength + zShift);
	m3dLoadVector3(bodenVertices[5], -edgeLength + xShift, -edgeLength + yShift, edgeLength + zShift);
	m3dLoadVector3(bodenVertices[6], edgeLength + xShift, -edgeLength + yShift, edgeLength + zShift);
	m3dLoadVector3(bodenVertices[7], edgeLength + xShift, -edgeLength + yShift, -edgeLength + zShift);

	m3dLoadVector4(bodenColors[0], 0.0f, 0.0f, 0.5f, 1);
	m3dLoadVector4(bodenColors[1], 0.0f, 0.0f, 0.5f, 1);
	m3dLoadVector4(bodenColors[2], 0.0f, 0.0f, 0.5f, 1);
	m3dLoadVector4(bodenColors[3], 0.0f, 0.0f, 0.5f, 1);
	m3dLoadVector4(bodenColors[4], 0.0f, 0.0f, 0.7f, 1);
	m3dLoadVector4(bodenColors[5], 0.0f, 0.0f, 0.7f, 1);
	m3dLoadVector4(bodenColors[6], 0.0f, 0.0f, 0.7f, 1);
	m3dLoadVector4(bodenColors[7], 0.0f, 0.0f, 0.7f, 1);

	cboden.Begin(GL_TRIANGLE_FAN, 8);
	cboden.CopyVertexData3f(bodenVertices);
	cboden.CopyColorData4f(bodenColors);
	cboden.End();

	M3DVector3f deckenVertices[8];
	M3DVector4f deckenColors[8];
	m3dLoadVector3(deckenVertices[0], edgeLength + xShift, edgeLength + yShift, edgeLength + zShift);
	m3dLoadVector3(deckenVertices[1], edgeLength + xShift, -edgeLength + yShift, -edgeLength + zShift);
	m3dLoadVector3(deckenVertices[2], edgeLength + xShift, -edgeLength + yShift, edgeLength + zShift);
	m3dLoadVector3(deckenVertices[3], -edgeLength + xShift, -edgeLength + yShift, edgeLength + zShift);
	m3dLoadVector3(deckenVertices[4], -edgeLength + xShift, edgeLength + yShift, edgeLength + zShift);
	m3dLoadVector3(deckenVertices[5], -edgeLength + xShift, edgeLength + yShift, -edgeLength + zShift);
	m3dLoadVector3(deckenVertices[6], edgeLength + xShift, edgeLength + yShift, -edgeLength + zShift);
	m3dLoadVector3(deckenVertices[7], edgeLength + xShift, -edgeLength + yShift, -edgeLength + zShift);

	m3dLoadVector4(deckenColors[0], 0.7f, 0.2f, 0.0f, 1);
	m3dLoadVector4(deckenColors[1], 0.7f, 0.2f, 0.0f, 1);
	m3dLoadVector4(deckenColors[2], 0.7f, 0.2f, 0.0f, 1);
	m3dLoadVector4(deckenColors[3], 0.7f, 0.0f, 0.0f, 1);
	m3dLoadVector4(deckenColors[4], 0.7f, 0.0f, 0.0f, 1);
	m3dLoadVector4(deckenColors[5], 0.7f, 0.0f, 0.0f, 1);
	m3dLoadVector4(deckenColors[6], 0.7f, 0.0f, 0.0f, 1);
	m3dLoadVector4(deckenColors[7], 0.7f, 0.2f, 0.0f, 1);

	decke.Begin(GL_TRIANGLE_FAN, 8);
	decke.CopyVertexData3f(deckenVertices);
	decke.CopyColorData4f(deckenColors);
	decke.End();
}

void CreateCylinder(float xShift, float yShift, float zShift) {

	GLfloat x, z, angle;
	float radius = 50.0f * scale;
	int i = 1;

	M3DVector3f* fussVertices = new M3DVector3f[arraySize];
	M3DVector3f* kopfVertices = new M3DVector3f[arraySize];
	M3DVector3f* planeVertices = new M3DVector3f[doubleArraySize];

	M3DVector4f* fussColors = new M3DVector4f[arraySize];
	M3DVector4f* kopfColors = new M3DVector4f[arraySize];
	M3DVector4f* planeColors = new M3DVector4f[doubleArraySize];

	m3dLoadVector3(fussVertices[0], xShift, -radius + yShift, zShift);
	m3dLoadVector3(kopfVertices[0], xShift, radius + yShift, zShift);
	m3dLoadVector3(planeVertices[0], xShift, -radius + yShift, zShift);
	m3dLoadVector3(planeVertices[1], xShift, radius + yShift, zShift);
	
	m3dLoadVector4(fussColors[0], 0.0f, 0.7f, 0.0f, 1);
	m3dLoadVector4(kopfColors[0], 0.0f, 0.5f, 0.1f, 1);
	m3dLoadVector4(planeColors[0], 0.1f, 0.3f, 0.0f, 1);
	m3dLoadVector4(planeColors[1], 0.1f, 0.3f, 0.0f, 1);

	for (angle = 0.0f; angle <= (2.0f*GL_PI); angle += (GL_PI / tesselation)) {
		// Berechne x und y Positionen des naechsten Vertex
		x = radius*sin(angle);
		z = radius*cos(angle);

		m3dLoadVector3(fussVertices[i], x + xShift, -radius + yShift, z + zShift);
		m3dLoadVector3(kopfVertices[i], -x, radius + yShift, z + zShift);
		m3dLoadVector3(planeVertices[2 * i], x + xShift, -radius + yShift, z + zShift);
		m3dLoadVector3(planeVertices[2 * i + 1], x + xShift, radius + yShift, z + zShift);

		m3dLoadVector4(fussColors[i], 0.0f, 0.7f, 0.0f, 1.0f);
		m3dLoadVector4(kopfColors[i], 0.0f, 0.5f, 0.1f, 1.0f);
		m3dLoadVector4(planeColors[2 * i], 0.1f, 0.3f, 0.0f, 1.0f);
		m3dLoadVector4(planeColors[2 * i + 1], 0.1f, 0.3f, 0.0f, 1.0f);
		i++;
	}

	fuss.Begin(GL_TRIANGLE_FAN, arraySize);
	kopf.Begin(GL_TRIANGLE_FAN, arraySize);
	plane.Begin(GL_TRIANGLE_STRIP, doubleArraySize);
	fuss.CopyVertexData3f(fussVertices);
	fuss.CopyColorData4f(fussColors);
	kopf.CopyVertexData3f(kopfVertices);
	kopf.CopyColorData4f(kopfColors);
	plane.CopyVertexData3f(planeVertices);
	plane.CopyColorData4f(planeColors);
	fuss.End();
	kopf.End();
	plane.End();

	delete[] fussVertices;
	delete[] fussColors;
	delete[] kopfVertices;
	delete[] kopfColors;
	delete[] planeVertices;
	delete[] planeColors;
}

void CreateSphere(float xShift, float yShift, float zShift) {

	float radius = 50.0f  * scale;
	float diameter = radius * 2;
	GLfloat x, z;

	M3DVector3f* bodyVertices = new M3DVector3f[sphereArraySize * 2];
	M3DVector4f* bodyColors = new M3DVector4f[sphereArraySize * 2];

	m3dLoadVector3(bodyVertices[0], xShift, radius - diameter/tesselation + yShift, zShift);
	m3dLoadVector3(bodyVertices[1], 0.0f + xShift, radius - 2*diameter/tesselation + yShift, zShift);
	m3dLoadVector4(bodyColors[0], 0.91f, 0.70f, 0.54f, 1.0f);
	m3dLoadVector4(bodyColors[1], 0.91f, 0.70f, 0.54f, 1.0f);

	int i = 1;

	int colorIndex = 0;

	// Kugel vertikal entlang der Breitengrade durchlaufen
	for (GLfloat latitude = 0.0f; latitude <= GL_PI; latitude += (GL_PI / tesselation)) {

		float sliceRadius = radius * sin(latitude);
		GLfloat y = radius * cos(latitude) + yShift;
		

		// Kugelscheibe entlang der Laengengrade durchlaufen
		for (GLfloat longitude = 0.0f; longitude <= (2.0f*GL_PI); longitude += (GL_PI / tesselation)) {

			float upperRadius = sliceRadius;
			float lowerRadius = radius * sin(latitude + (GL_PI / tesselation));

			x = cos(longitude);
			z = sin(longitude);

			m3dLoadVector3(bodyVertices[2 * i], x*upperRadius + xShift, y, z*upperRadius + zShift);
			m3dLoadVector3(bodyVertices[2 * i + 1], x*lowerRadius + xShift, radius * cos(latitude + GL_PI/tesselation), z*lowerRadius + zShift);

			//fuer Abgabe einkommentieren :)
			//if (colorIndex % 2 == 0) {
			//	m3dLoadVector4(bodyColors[2 * i], 1.0f, 0.8f, 0.2f, 1.0f);
			//	m3dLoadVector4(bodyColors[2 * i + 1], 1.0f, 0.8f, 0.2f, 1.0f);
			//}
			//else {
			//	m3dLoadVector4(bodyColors[2 * i], 0.235f, 0.235f, 0.235f, 1.0f);
			//	m3dLoadVector4(bodyColors[2 * i + 1], 0.235f, 0.235f, 0.235f, 1.0f);
			//}

			// fuer Maennchen einkommentieren :)
			m3dLoadVector4(bodyColors[2 * i], 0.91f, 0.70f, 0.54f, 1.0f);
			m3dLoadVector4(bodyColors[2 * i + 1], 0.91f, 0.70f, 0.54f, 1.0f);
			i++;
		}
		
		colorIndex++;
	}

	sphereBody.Reset();
	sphereBody.Begin(GL_TRIANGLE_STRIP, sphereArraySize);
	sphereBody.CopyVertexData3f(bodyVertices);
	sphereBody.CopyColorData4f(bodyColors);
	sphereBody.End();

	delete[] bodyVertices;
	delete[] bodyColors;
}

void DrawCone() {
	konus.Draw();
	kboden.Draw();
}

void DrawCube() {
	cboden.Draw();
	decke.Draw();
}

void DrawCylinder() {
	kopf.Draw();
	fuss.Draw();
	plane.Draw();
}

void DrawSphere() {
	sphereBody.Draw();
}

void DrawBaeumchen() {

	// Boden
	modelViewMatrix.PushMatrix();
	modelViewMatrix.PushMatrix();
	modelViewMatrix.Translate(0, -25, 0);
	modelViewMatrix.Scale(20, -0.01, 20);
	shaderManager.UseStockShader(GLT_SHADER_FLAT_ATTRIBUTES, transformPipeline.GetModelViewProjectionMatrix());
	modelViewMatrix.PopMatrix();
	DrawCube();

	// Baumstamm
	modelViewMatrix.PushMatrix();
	modelViewMatrix.Translate(0, 0.0, 0);
	modelViewMatrix.PushMatrix();
	modelViewMatrix.Scale(0.3, 0.5, 0.3);
	shaderManager.UseStockShader(GLT_SHADER_FLAT_ATTRIBUTES, transformPipeline.GetModelViewProjectionMatrix());
	modelViewMatrix.PopMatrix();
	DrawCylinder();

	//unterster Kegel
	modelViewMatrix.PushMatrix();
	modelViewMatrix.Translate(0, 25.0, 0);
	modelViewMatrix.Scale(1.5, 1.5, 1.5);
	shaderManager.UseStockShader(GLT_SHADER_FLAT_ATTRIBUTES, transformPipeline.GetModelViewProjectionMatrix());
	DrawCone();

	// mittlerer Kegel
	modelViewMatrix.PushMatrix();
	modelViewMatrix.Translate(0, 40.0, 0);
	modelViewMatrix.Scale(0.75, 0.75, 0.75);
	shaderManager.UseStockShader(GLT_SHADER_FLAT_ATTRIBUTES, transformPipeline.GetModelViewProjectionMatrix());
	DrawCone();	
	
	// Spitze
	modelViewMatrix.PushMatrix();
	modelViewMatrix.Translate(0, 50.0, 0);
	modelViewMatrix.Scale(0.75, 0.75, 0.75);
	shaderManager.UseStockShader(GLT_SHADER_FLAT_ATTRIBUTES, transformPipeline.GetModelViewProjectionMatrix());
	DrawCone();

	modelViewMatrix.PopMatrix();
	modelViewMatrix.PopMatrix();
	modelViewMatrix.PopMatrix();
	modelViewMatrix.PopMatrix();
	modelViewMatrix.PopMatrix();
}

void DrawMaennchen(float angle) {

	//float overallScaleFactor = 0.2 * cos(GL_PI / 200 * angle) + 1;
	float jumpFactor = abs(cos(GL_PI/10 * angle));

	/**
	* Rumpf zeichnen
	**/
	modelViewMatrix.PushMatrix();
	modelViewMatrix.Rotate(angle, 0, -1, 0);
	// generelle Verschiebung aus dem Mittelpunkt heraus
	modelViewMatrix.Translate(200.0f, 0.0f, 0.0f);
	// Verschiebung fuer Huepfbahn
	modelViewMatrix.Translate(0.0f, 30 * jumpFactor, 0);
	//modelViewMatrix.Scale(overallScaleFactor, overallScaleFactor, overallScaleFactor);

	modelViewMatrix.PushMatrix();
	modelViewMatrix.Scale(0.9, 1.0, 0.7);
	shaderManager.UseStockShader(GLT_SHADER_FLAT_ATTRIBUTES, transformPipeline.GetModelViewProjectionMatrix());
	modelViewMatrix.PopMatrix();
	DrawCylinder();

	// Hals
	modelViewMatrix.PushMatrix();
	modelViewMatrix.Translate(0.0f, 55.0f, 0.0f);
	modelViewMatrix.PushMatrix();
	modelViewMatrix.Scale(0.25, 0.15, 0.25);
	shaderManager.UseStockShader(GLT_SHADER_FLAT_ATTRIBUTES, transformPipeline.GetModelViewProjectionMatrix());
	modelViewMatrix.PopMatrix();
	DrawCylinder();

	// Kopf
	modelViewMatrix.PushMatrix();
	modelViewMatrix.Translate(0.0f, 40.0f, 0.0f);
	modelViewMatrix.Scale(0.72, 0.72, 0.72);
	shaderManager.UseStockShader(GLT_SHADER_FLAT_ATTRIBUTES, transformPipeline.GetModelViewProjectionMatrix());
	DrawSphere();
	modelViewMatrix.PopMatrix();
	modelViewMatrix.PopMatrix();
	
	// Giedmaßen zeichnen - abhaengig vom Rumpf!
	DrawLimbs(angle);

	modelViewMatrix.PopMatrix();
}

void DrawLimbs(float angle) {

	// Oberes Glied
	DrawUpperLimb(angle);
	// Unteres Glied
	//DrawLowerLimb(angle);
}

void DrawUpperLimb(float angle) {

	float movement = sin(GL_PI/8 * angle);

	// rechten Arm
	modelViewMatrix.PushMatrix();

	//Schwingbewegung realisieren
	modelViewMatrix.Translate(10.0, 55.0, 0);
	modelViewMatrix.Rotate(movement * 35.0, 1.0, 0.0, 0.0);
	modelViewMatrix.Translate(-10.0, -55.0, 0);

	modelViewMatrix.Translate(60.0f, 10.0f, 0.0f);
	modelViewMatrix.Rotate(30.0, 0.0, 0.0, 1.0);
	modelViewMatrix.PushMatrix();
	modelViewMatrix.Scale(0.2, 0.75, 0.2);
	shaderManager.UseStockShader(GLT_SHADER_FLAT_ATTRIBUTES, transformPipeline.GetModelViewProjectionMatrix());
	modelViewMatrix.PopMatrix();
	DrawCylinder();

	// rechte Hand
	modelViewMatrix.PushMatrix();
	modelViewMatrix.Translate(0.0f, -40.0f, 0.0f);
	modelViewMatrix.PushMatrix();
	modelViewMatrix.Scale(0.2, 0.2, 0.2);
	shaderManager.UseStockShader(GLT_SHADER_FLAT_ATTRIBUTES, transformPipeline.GetModelViewProjectionMatrix());
	modelViewMatrix.PopMatrix();
	DrawSphere();
	modelViewMatrix.PopMatrix();
	modelViewMatrix.PopMatrix();

	/**
	* linkes Bein
	**/
	modelViewMatrix.PushMatrix();

	// Schwingbewegung
	modelViewMatrix.Translate(0.0, -50.0, 0);
	modelViewMatrix.Rotate(movement * 35.0, 1.0, 0.0, 0.0);
	modelViewMatrix.Translate(0.0, 50.0, 0);

	modelViewMatrix.Translate(-20.0f, -80.0f, 0.0f);
	modelViewMatrix.PushMatrix();
	modelViewMatrix.Scale(0.3, 0.8, 0.3);
	shaderManager.UseStockShader(GLT_SHADER_FLAT_ATTRIBUTES, transformPipeline.GetModelViewProjectionMatrix());
	modelViewMatrix.PopMatrix();
	DrawCylinder();

	// linker Fuss
	modelViewMatrix.PushMatrix();
	modelViewMatrix.Translate(0.0f, -47.0f, 10.0f);
	modelViewMatrix.PushMatrix();
	modelViewMatrix.Scale(0.5, 0.3, 0.9);
	shaderManager.UseStockShader(GLT_SHADER_FLAT_ATTRIBUTES, transformPipeline.GetModelViewProjectionMatrix());
	modelViewMatrix.PopMatrix();
	DrawCube();
	modelViewMatrix.PopMatrix();
	modelViewMatrix.PopMatrix();

	/**
	* linker Arm
	**/
	modelViewMatrix.PushMatrix();

	// Schwingbewegung
	modelViewMatrix.Translate(10.0, 55.0, 0);
	modelViewMatrix.Rotate(-movement * 35.0, 1.0, 0.0, 0.0);
	modelViewMatrix.Translate(-10.0, -55.0, 0);

	modelViewMatrix.Translate(-60.0f, 10.0f, 0.0f);
	modelViewMatrix.Rotate(-30.0, 0.0, 0.0, 1.0);
	modelViewMatrix.PushMatrix();
	modelViewMatrix.Scale(0.2, 0.75, 0.2);
	shaderManager.UseStockShader(GLT_SHADER_FLAT_ATTRIBUTES, transformPipeline.GetModelViewProjectionMatrix());
	modelViewMatrix.PopMatrix();
	DrawCylinder();

	// linke Hand
	modelViewMatrix.PushMatrix();
	modelViewMatrix.Translate(0.0f, -40.0f, 0.0f);
	modelViewMatrix.PushMatrix();
	modelViewMatrix.Scale(0.2, 0.2, 0.2);
	shaderManager.UseStockShader(GLT_SHADER_FLAT_ATTRIBUTES, transformPipeline.GetModelViewProjectionMatrix());
	modelViewMatrix.PopMatrix();
	DrawSphere();
	modelViewMatrix.PopMatrix();
	modelViewMatrix.PopMatrix();

	/** rechtes Bein
	*/
	modelViewMatrix.PushMatrix();

	// Schwingbewegung
	modelViewMatrix.Translate(0.0, -50.0, 0);
	modelViewMatrix.Rotate(-movement * 35.0, 1.0, 0.0, 0.0);
	modelViewMatrix.Translate(0.0, 50.0, 0);

	modelViewMatrix.Translate(20.0f, -80.0f, 0.0f);
	modelViewMatrix.Rotate(8.0, 1.0, 0.0, 0.0);
	modelViewMatrix.PushMatrix();
	modelViewMatrix.Scale(0.3, 0.8, 0.3);
	shaderManager.UseStockShader(GLT_SHADER_FLAT_ATTRIBUTES, transformPipeline.GetModelViewProjectionMatrix());
	modelViewMatrix.PopMatrix();
	DrawCylinder();

	// rechter Fuss
	modelViewMatrix.PushMatrix();
	modelViewMatrix.Translate(0.0f, -47.0f, 10.0f);
	modelViewMatrix.Rotate(0.0, 1.0, 0.0, 0.0);
	modelViewMatrix.PushMatrix();
	modelViewMatrix.Scale(0.5, 0.3, 0.9);
	shaderManager.UseStockShader(GLT_SHADER_FLAT_ATTRIBUTES, transformPipeline.GetModelViewProjectionMatrix());
	modelViewMatrix.PopMatrix();
	DrawCube();
	modelViewMatrix.PopMatrix();
	modelViewMatrix.PopMatrix();
	

}

void DrawLowerLimb(float angle) {

}

// Aufruf draw scene
void RenderScene(void) {

	// Clearbefehle für den color buffer und den depth buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Schalte culling ein falls das Flag gesetzt ist
	if (bCull) {
		glEnable(GL_CULL_FACE);
	}
	else {
		glDisable(GL_CULL_FACE);
	}

	// Schalte depth testing ein falls das Flag gesetzt ist
	if (bDepth) {
		glEnable(GL_DEPTH_TEST);
	}
	else {
		glDisable(GL_DEPTH_TEST);
	}

	// Zeichne die Rückseite von Polygonen als Drahtgitter falls das Flag gesetzt ist
	if (bOutline) {
		glPolygonMode(GL_BACK, GL_LINE);
	}
	else {
		glPolygonMode(GL_BACK, GL_FILL);
	}


	// Speichere den matrix state und führe die Rotation durch
	modelViewMatrix.PushMatrix();
	M3DMatrix44f rot;
	m3dQuatToRotationMatrix(rot, rotation);
	modelViewMatrix.MultMatrix(rot);

	//setze den Shader für das Rendern
	shaderManager.UseStockShader(GLT_SHADER_FLAT_ATTRIBUTES, transformPipeline.GetModelViewProjectionMatrix());
	
	// zusaetzliche Transformationen bei persp. Projektion
	if (bPerspectiveProj) {

		// Vektor fuer UFO-Bewegungsmodus draufrechnen
		modelViewMatrix.Translate(ufoMove[0], ufoMove[1], ufoMove[2]);

		// Vektor fuer Drehungen um x- oder y-Achse draufrechnen
		modelViewMatrix.Rotate(viewAngleX, 0.0f, 1.0f, 0.0f );
		modelViewMatrix.Rotate(viewAngleY, 1.0f, 0.0f, 0.0f);

		// Augenpunktstransformation
		modelViewMatrix.Translate(0.0f, 0.0f, -1000.0f);
	}


	//Zeichne Objekte
	modelViewMatrix.PushMatrix();
	modelViewMatrix.Translate(0, -110, 0);
	DrawBaeumchen();
	modelViewMatrix.PopMatrix();
	DrawMaennchen(animationAngle += GL_PI / 300);

	// Hole die im Stack gespeicherten Transformationsmatrizen wieder zurück
	modelViewMatrix.PopMatrix();

	TwDraw();
	// Vertausche Front- und Backbuffer
	glutSwapBuffers();
	glutPostRedisplay();
}

// Initialisierung des Rendering Kontextes
void SetupRC() {
	// Schwarzer Hintergrund
	glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
	// In Uhrzeigerrichtung zeigende Polygone sind die Vorderseiten.
	// Dies ist umgekehrt als bei der Default-Einstellung weil wir Triangle_Fans benützen
	glFrontFace(GL_CW);

	//initialisiert die standard shader
	shaderManager.InitializeStockShaders();
	transformPipeline.SetMatrixStacks(modelViewMatrix, projectionMatrix);

	//erzeuge die geometrie
	CreateCone(0, 0, 0);
	CreateCube(0, 0, 0);
	CreateCylinder(0, 0, 0);
	CreateSphere(0, 0, 0);

	InitGUI();
}

/** params: 
*		key --> pressed key
*		x --> cursor x coordinate
*		y --> cursor y coordinate
*/
void SpecialKeys(int key, int x, int y) {

	float xStep = 5.0f;
	float yStep = 5.0f;
	float zStep = 5.0f;

	if (bVertAxisInverted)
		yStep = -5;

	switch (key) {
		case GLUT_KEY_LEFT:
			ufoMove[0] += xStep;
			//glutPostRedisplay();
			break;
		case GLUT_KEY_RIGHT:
			ufoMove[0] -= xStep;
			//glutPostRedisplay();
			break;
		case GLUT_KEY_UP:
			ufoMove[2] += zStep;
			//glutPostRedisplay();
			break;
		case GLUT_KEY_DOWN:
			ufoMove[2] -= zStep;
			//glutPostRedisplay();
			break;
		case GLUT_KEY_PAGE_UP:
			ufoMove[1] += yStep;
			break;
		case GLUT_KEY_PAGE_DOWN:
			ufoMove[1] -= yStep;
			break;
	}

	TwEventKeyboardGLUT(key, x, y);
	glutPostRedisplay();
}

void Keyboard(unsigned char key, int x, int y) {

	float xStep = 5.0f;
	float yStep = 5.0f;

	if (bVertAxisInverted)
		yStep = -5.0f;
	
	switch (key) {
		case 'w':
			viewAngleY += yStep;
			break;
		case 's':
			viewAngleY -= yStep;
			break;
		case 'a':
			viewAngleX -= xStep;
			break;
		case 'd':
			viewAngleX += xStep;
			break;
	}

	glutPostRedisplay();
}

void ChangeSize(int w, int h) {

	GLfloat nRange = 250.0f;
	windowWidth = w;
	windowHeight = h;

	// Verhindere eine Division durch Null
	if (h == 0) {
		h = 1;
	}
	// Setze den Viewport gemaess der Window-Groesse
	glViewport(0, 0, w, h);
	// Ruecksetzung des Projection matrix stack
	projectionMatrix.LoadIdentity();

	if (bPerspectiveProj) {
		// Definiere das viewing volume (left, right, bottom, top, near, far)
		if (w <= h) {
			viewFrustum.SetPerspective(30.0f, h/w, 0.1, 1000);
		}
		else {
			viewFrustum.SetPerspective(30.0f, w / h, 0.1, 1000);
		}
	}

	else {
		// Definiere das viewing volume (left, right, bottom, top, near, far)
		if (w <= h) {
			viewFrustum.SetOrthographic(-nRange, nRange, -nRange*h / w, nRange*h / w, -nRange, nRange);
		}
		else {
			viewFrustum.SetOrthographic(-nRange*w / h, nRange*w / h, -nRange, nRange, -nRange, nRange);
		}
	}

	projectionMatrix.LoadMatrix(viewFrustum.GetProjectionMatrix());
	// Ruecksetzung des Model view matrix stack
	modelViewMatrix.LoadIdentity();

	TwWindowSize(w, h);
}

void ShutDownRC() {
	TwTerminate();
}

int main(int argc, char* argv[]) {
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(windowWidth, windowHeight);
	glutCreateWindow("Versuch1");

	GLenum err = glewInit();
	if (GLEW_OK != err) {
		// Veralteter Treiber etc.
		std::cerr << "Error: " << glewGetErrorString(err) << "\n";
		return 1;
	}

	glutMouseFunc((GLUTmousebuttonfun)TwEventMouseButtonGLUT);
	glutMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT);
	glutPassiveMotionFunc((GLUTmousemotionfun)TwEventMouseMotionGLUT); // same as MouseMotion
	glutKeyboardFunc(Keyboard);

	glutReshapeFunc(ChangeSize);
	glutSpecialFunc(SpecialKeys);
	glutDisplayFunc(RenderScene);

	TwInit(TW_OPENGL, NULL);
	SetupRC();
	glutMainLoop();
	ShutDownRC();

	return 0;
}
