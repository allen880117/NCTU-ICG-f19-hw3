#include "FreeImage.h"
#include "glew.h"
#include "glut.h"
#include "shader.h"
#include <iostream>
#include <string>
#include <cmath>
#define PI 3.14159265358
using namespace std;

int windowSize[2] = { 600, 600 };
void display();
void idle();
void reshape(GLsizei w, GLsizei h);
void keyboard(unsigned char key, int x, int y);

GLuint program;
GLuint vboName;

//Initialize the shaders 
void shaderInit();
void textureInit();

// Texture
GLuint texture;

//Number of vertices that will be sent to shader
int verticeNumber = 3;

//Earth Variables
GLfloat earthColor[3] = { 0.55f, 0.85f, 0.95f };
GLfloat earthPosition[3] = { 0.0f, 0.0f, 0.0f };
GLfloat earthOffset[3] = { 0.0f, 0.0f, 0.0f };
float earthRadius = 1;
//int earthSlice = 360;
//int earthStack = 180;
int earthSlice = 20;
int earthStack = 10;

//Light Ball Variables
GLfloat lightPosition[3] = { 1.1f, 1.0f, 1.3f };
//GLfloat lightPosition[3] = { 0.0f, 0.0f, 1.3f };
GLfloat lightBallColor[3] = { 0.4f, 0.5f, 0.0f };
float lightBallRadius = 0.05;
int   lightBallSlice = 360;
int   lightBallStack = 180;

GLfloat ks = 0.0f;
GLfloat kd = 0.0f;

// opt
int opt = 0;
int border_opt = 0;

// time
int   timeflow = 0;
float angle = 0.0f;

//Storing vertex datas that will be sent to shader
class VertexAttribute {
public:
	GLfloat position[3];
	GLfloat texcoord[2];
	GLfloat normal[3];
	void setPosition(float x, float y, float z) {
		position[0] = x;
		position[1] = y;
		position[2] = z;
	};
	void setCoord(float x, float y) {
		texcoord[0] = x;
		texcoord[1] = y;
	}
	void setNormal(float x, float y, float z) {
		normal[0] = x;
		normal[1] = y;
		normal[2] = z;
	};
};

//Collecting vertex datas
VertexAttribute *drawTriangle();
VertexAttribute *shaderSphere(float c[3], int slice, int stack, float radius);
void drawShaderSphere(int slice, int stack);

int main(int argc, char** argv) {	
	glutInit(&argc, argv);
	glutInitWindowSize(windowSize[0], windowSize[1]);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutCreateWindow("ComputerGraphicsDemo");

	glewInit();
	shaderInit();
	// === WA === Texture Initialize
	textureInit();

	glutDisplayFunc(display);
	glutReshapeFunc(reshape);
	glutKeyboardFunc(keyboard);
	glutIdleFunc(idle);
	glutMainLoop();
	return 0;
}

# if 0
void shaderInit() {
	GLuint vert = createShader("Shaders/example.vert", "vertex");
	GLuint frag = createShader("Shaders/example.frag", "fragment");
	program = createProgram(vert, frag);

	//Generate a new buffer object
	glGenBuffers(1, &vboName);
	glBindBuffer(GL_ARRAY_BUFFER, vboName);

	//Copy vertex data to the buffer object
	VertexAttribute *vertices;
	vertices = drawTriangle();
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute) * verticeNumber, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, position)));
	glBindBuffer(GL_ARRAY_BUFFER, 0);
}
#endif
void shaderInit() {
	GLuint vert = createShader("Shaders/example.vert", "vertex");
	GLuint frag = createShader("Shaders/example.frag", "fragment");
	program = createProgram(vert, frag);

	//Generate a new buffer object
	glGenBuffers(1, &vboName);
	glBindBuffer(GL_ARRAY_BUFFER, vboName);

	//Copy vertex data to the buffer object
	VertexAttribute *vertices;
	vertices = shaderSphere(earthPosition, earthSlice, earthStack, earthRadius);
	verticeNumber = earthSlice * (earthStack + 1) * 2;
	glBufferData(GL_ARRAY_BUFFER, sizeof(VertexAttribute) * verticeNumber, vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, position)));
	
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, texcoord)));
	
	glEnableVertexAttribArray(2);
	glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(VertexAttribute), (void*)(offsetof(VertexAttribute, normal)));

	glBindBuffer(GL_ARRAY_BUFFER, 0);
}

void textureInit() { // =TEXTURE= //
	// Enable Texture
	glEnable(GL_TEXTURE_2D);
	
	// Load the Texture
	FIBITMAP* pImage = FreeImage_Load(FreeImage_GetFileType("earth_texture_map.jpg", 0), "earth_texture_map.jpg");
	FIBITMAP* p32BitsImage = FreeImage_ConvertTo32Bits(pImage);
	int iWidth = FreeImage_GetWidth(p32BitsImage);
	int iHeight = FreeImage_GetHeight(p32BitsImage);

	// Generate Texture
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);

	// Set the Texture warping parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// Set the texture filtering parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// Specifies a texture environment
	glTexEnvf(GL_TEXTURE_2D, GL_TEXTURE_ENV_MODE, GL_MODULATE);

	// Generate a two-dimensional texture image
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA8, iWidth, iHeight, 0, 
		GL_BGRA, GL_UNSIGNED_BYTE, (void*)FreeImage_GetBits(p32BitsImage));

	glGenerateMipmap(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D, 0);
	FreeImage_Unload(p32BitsImage);
	FreeImage_Unload(pImage);
}

void display() {   
	// viewing and modeling transformation
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(0.0f, 0.0f, 5.6f,// eye
		0.0f, 0.0f, 0.0f,// center
		0.0f, 1.0f, 0.0f);// up

	// projection transformation
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(45, (GLfloat)512 / (GLfloat)512, 1, 500);

	// viewport transformation
	glViewport(0, 0, windowSize[0], windowSize[1]);

	//Clear the buffer
	glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
	glClearDepth(1.0f);
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LEQUAL);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glMatrixMode(GL_MODELVIEW);
	//glRotatef(angle, 0.0, 1.0, 0.0);

	GLfloat pmtx[16];
	GLfloat mmtx[16];
	glGetFloatv(GL_PROJECTION_MATRIX, pmtx); // Get Projection-View Matrix
	glGetFloatv(GL_MODELVIEW_MATRIX, mmtx); // Get Model-View Matrix
 	GLint pmatLoc = glGetUniformLocation(program, "Projection");
	GLint mmatLoc = glGetUniformLocation(program, "ModelView");
	GLint texLoc = glGetUniformLocation(program, "earthTexture");

	GLint lightLoc = glGetUniformLocation(program, "LightPosition");
	GLint optLoc = glGetUniformLocation(program, "opt");
	GLint borderOptLoc = glGetUniformLocation(program, "border_opt");
	GLint ksLoc = glGetUniformLocation(program, "ks");
	GLint kdLoc = glGetUniformLocation(program, "kd");
	GLint earthColorLoc = glGetUniformLocation(program, "EarthColor");
	GLint angleLoc = glGetUniformLocation(program, "angle");

	glUseProgram(program); // USE PROGRAM

	glUniformMatrix4fv(pmatLoc, 1, GL_FALSE, pmtx);
	glUniformMatrix4fv(mmatLoc, 1, GL_FALSE, mmtx);

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(texLoc, 0);

	glUniform3f(lightLoc, lightPosition[0], lightPosition[1], lightPosition[2]);
	glUniform1i(optLoc, opt);
	glUniform1i(borderOptLoc, border_opt);
	glUniform1f(ksLoc, ks);
	glUniform1f(kdLoc, kd);

	glUniform3f(earthColorLoc, earthColor[0], earthColor[1], earthColor[2]);
	glUniform1f(angleLoc, angle * PI / 180.0f );

	// === WA === glDrawArrays(GL_TRIANGLE_STRIP, 0, 8);
	drawShaderSphere(earthSlice, earthStack); // WA_MODE

	glUseProgram(0);
	glPopMatrix();

	glPushMatrix();
	glTranslatef(lightPosition[0], lightPosition[1], lightPosition[2]);
	glColor3f(lightBallColor[0], lightBallColor[1], lightBallColor[2]);
	gluSphere(gluNewQuadric(), lightBallRadius, lightBallSlice, lightBallStack);
	glPopMatrix();

	glutSwapBuffers();
}

VertexAttribute *drawTriangle() {
	VertexAttribute *vertices;
	vertices = new VertexAttribute[verticeNumber];
	vertices[0].setPosition(0,0,0);
	vertices[1].setPosition(1,0,0);
	vertices[2].setPosition(0,1,0);
	return vertices;
}

void reshape(GLsizei w, GLsizei h) {
	windowSize[0] = w;
	windowSize[1] = h;
}

void keyboard(unsigned char key, int x, int y) {
	switch (key)
	{
	case '1':
		ks -= 0.1;
		if (ks < 0.0f) ks = 0.0f;
		break;
	case '2':
		ks += 0.1;
		if (ks > 1.0f) ks = 1.0f;
		break;
	case '3':
		kd -= 0.1;
		if (kd < 0.0f) kd = 0.0f;
		break;
	case '4':
		kd += 0.1;
		if (kd > 1.0f) kd = 1.0f;
		break;
	case 'B': case 'b':
		opt++;
		opt = opt % 3;
		break;
	case 'E': case 'e':
		border_opt++;
		border_opt = border_opt % 2;
		break;
	case 'W': case'w':
		earthOffset[1] += 0.1f;
		break;
	case 'A': case'a':
		earthOffset[0] -= 0.1f;
		break;
	case 'S': case's':
		earthOffset[1] -= 0.1f;
		break;
	case 'D': case'd':
		earthOffset[0] += 0.1f;
		break;
	case 'T': case 't':
		timeflow++;
		timeflow = timeflow % 2;
		break;
	default:
		break;
	}
}

void idle() {
	glutPostRedisplay();
	if (timeflow == 1) {
		angle += 0.3f;
		if (angle >= 360.0f) angle = 0.0f;
	}
}

void drawShaderSphere(int slice, int stack) {
	int step = (stack + 1) * 2;
	int counter = 0;
	for (int i = 0; i < slice; i++) {
		glDrawArrays(GL_TRIANGLE_STRIP, counter, step);
		counter += step;
	}
}

VertexAttribute *shaderSphere(float c[3], int slice, int stack, float radius) {
	VertexAttribute *vertices;
	vertices = new VertexAttribute[slice*(stack+1)*2];
	int counter = 0;
	float coordXCounter = 1.0f;
	float coordYCounter = 1.0f;
	float stepX = (float)1.0f/slice;
	float stepY = (float)1.0f/(stack+1);
	double x, y, z;
	double slice_step = 2 * PI / slice, stack_step = PI / stack;
	float cx = c[0];
	float cy = c[1];
	float cz = c[2];
	for (int i = 0; i < slice; i++) {

		coordXCounter = 1.0f - i*stepX;
		coordYCounter = 1.0f;
		
		for (int j = 0; j < stack + 1; j++) {
			x = cx + radius * sin(j * stack_step) * cos(i*slice_step);
			y = cy + radius * cos(j * stack_step);
			z = cz + radius * sin(j * stack_step) * sin(i*slice_step);
			vertices[counter].setPosition(x, y, z);
			vertices[counter].setCoord(coordXCounter, coordYCounter);
			vertices[counter].setNormal(x-cx, y-cy, z-cz);
			counter++;

			x = cx + radius * sin(j * stack_step) * cos((i + 1)*slice_step);
			y = cy + radius * cos(j * stack_step);
			z = cz + radius * sin(j * stack_step) * sin((i + 1)*slice_step);
			vertices[counter].setPosition(x, y, z);
			vertices[counter].setCoord(coordXCounter - stepX, coordYCounter);
			vertices[counter].setNormal(x-cx, y-cy, z-cz);
			counter++;
			
			coordYCounter -= stepY;
			if (coordYCounter <= 0.0f) stepY = -stepY;
		}

		stepY = -stepY;
	}
	return vertices;
}