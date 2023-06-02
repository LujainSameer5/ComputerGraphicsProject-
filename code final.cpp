#define _CRT_SECURE_NO_WARNINGS
#include<windows.h>
#include <GL/glut.h>
#include <stdio.h>
#include <GL/gl.h>
#include <math.h>
#include <stdlib.h>
#include<cstdio>
#include <vector>
#include <cstdlib>
#include <iostream>
#define PI 3.14159265358979323846
#define SNOWSIZE 5000


time_t t3;
float rotationAngle = 0; //rotation used in snow

GLint win_width = 1000,
win_hight = 1000;

float transValueX;
float transValueY;
float transValueZ;

GLfloat light_ambient[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat light_diffuse[] = { 1.0, 1.0, 1.0, 1.0 };//for color
GLfloat light_specular[] = { 1.0, 1.0, 1.0, 1.0 };
GLfloat mat_specular[] = { 0.0, 0.0, 1.0, 1.0 };
GLfloat light_position[] = { 3.0, 5.0, 10.0, 1.0 };
GLfloat shininess[] = { 128 };


GLuint LoadTexture(const char* filename)
{
	GLuint texture;
	int width, height;
	unsigned char* data;

	FILE* file;
	file = fopen(filename, "rb");

	if (file == NULL) {
		printf("file not found");
		return 0;
	}
	width = 300;
	height = 225;
	data = (unsigned char*)malloc(width * height * 3);
	//int size = fseek(file,);
	fread(data, width * height * 3, 1, file);
	fclose(file);

	for (int i = 0; i < width * height; ++i)
	{
		int index = i * 3;
		unsigned char B, R;
		B = data[index];
		R = data[index + 2];

		data[index] = R;
		data[index + 2] = B;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexImage2D(GL_TEXTURE_2D,
		0,
		GL_RGB,
		width,
		height,
		0,
		GL_RGB,
		GL_UNSIGNED_BYTE,
		data);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	gluBuild2DMipmaps(GL_TEXTURE_2D, 3, width, height, GL_RGB, GL_UNSIGNED_BYTE, data);
	free(data);

	return texture;
}

//we took rain+drop methods from here we changed alot in it turn rain to snow and many more
//https://stackoverflow.com/questions/53814299/creating-rain-drops
//in sturcture drop we defined all varibales needed to create snow
struct drop {
	float x = 400;
	float y = 400;
	float inc = 0.01;
	float radius = 5;
	float scale = 1.0;
	float rotationAngle = 0;
	float rotationInc = 1;
};

drop rain[SNOWSIZE]; //made object of drop called rain ith given snowsize

void initSnow() {
	srand((unsigned)time(&t3));
	for (int i = 0; i < SNOWSIZE; i++) {
		rain[i].x = -100 + rand() % win_width;
		rain[i].y = rand() % win_hight;
		rain[i].inc = 1.5 + (float)(rand() % 100) / 1000.0;
		rain[i].radius = (float)(rand() % 8);
		rain[i].scale = (float)(rand() % 20000) / 1000.0;
		rain[i].rotationAngle = (float)(rand() % 3000) / 1000.0;
		rain[i].rotationInc = (float)(rand() % 100) / 1000.0;
		if ((rand() % 100) > SNOWSIZE) {
			rain[i].rotationInc = -rain[i].rotationInc;
		}
	}


}

void drawParticleShape1(int i) {

	glBegin(GL_LINES);
	glVertex2d(rain[i].x, rain[i].y);
	glVertex2d(rain[i].x, (rain[i].y + rain[i].radius * 0.1));
	glEnd();
	glBegin(GL_POINTS);
	glVertex2d(rain[i].y, rain[i].x);
	glEnd();
}


void drawDrop(int i) {
	glColor3f(1.0, 1.0, 1.0);
	glLineWidth(7);
	drawParticleShape1(i);
	rain[i].y -= rain[i].inc;
	if (rain[i].y < -10.0) {
		rain[i].y = win_hight;
	}

}


void drawSnow() {
	for (int i = 0; i < SNOWSIZE + 100; i++) {
		drawDrop(i);
	}
}

float elapsedTime = 0, base_time = 0, fps = 0, frames;

void calcFPS() {
	elapsedTime = glutGet(GLUT_ELAPSED_TIME);
	if (elapsedTime - base_time > 1000.0) {
		fps = frames * 1000.0 / elapsedTime - base_time;
		printf("fps: %f", fps);
		base_time = elapsedTime;
		frames = 0;
	}
	frames++;
}


//for cars
GLfloat tx = 0.0;
GLfloat ty = 0.0;
static float car_run = 0.0;
static float car_run2 = 0.0;
//for clouds
static float cloudRight = 0.0;
static float cloudLeft = 0.0;
float t = 0;//for sound

void init(void) //withhout this we dont have an output
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	glOrtho(-50, 50, -50, 25, -25, 5);

	transValueX = 0;
	transValueY = 0;
	transValueZ = 1;

	////////
	// create light 0
	//////
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	/////////////////////////////////////////

}

//circle was taken from here but we made few changes it
//https://stackoverflow.com/questions/22444450/drawing-circle-with-opengl
void circle(GLfloat rx, GLfloat ry, GLfloat cx, GLfloat cy) {

	glBegin(GL_TRIANGLE_FAN);
	glVertex2f(cx, cy); //create a triangle fan with given coridnates

	for (int i = 0; i <= 100; i++)
	{
		float angle = 2.0f * 3.1416f * i / 100;

		float x = rx * cosf(angle); //
		float y = ry * sinf(angle);

		glVertex2f((x + cx), (y + cy));
	}
	glEnd();
}


void Sun() {

	glPushMatrix();
	glTranslatef(30.0f, 4.0f, 0.0f); //move sun to the right and upwards
	glRotatef(0, 0.0f, 0.0f, -2.0f);

	//create the biggest circle
	glColor4f(4.0f, 1.0f, 0.4f, 0.1f);
	circle(5, 5, 4.5, 11.5);

	//create the bigger circle
	glColor3f(4.0f, 1.0f, 0.4f);
	circle(3, 3, 4.5, 11.5);

	//create the smaller circule
	glColor4f(0.9f, 0.9f, 0.9f, 0.22f);//used 4f here to make it transparent
	circle(2, 2, 4.5, 11.5);
	glPopMatrix();


}

void sky() {

	glColor3f(0.2, 0.6, 1.0);
	glBegin(GL_POLYGON);
	glVertex2f(-80, -20); //left down angle
	glVertex2f(80, -30); //right down angle 
	glVertex2f(50, 35); //right up angle
	glVertex2f(-50, 35); //left up angle
	glEnd();

}


void cloud11() {

	glPushMatrix();
	glTranslatef(-40.0f, 6.0, 0.0f);
	glRotatef(0, 0.0f, 0.0f, -1.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(2, 2, 14, 11);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(2, 2, 16.5, 11);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(2, 2, 19.5, 11);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(2, 2, 15.5, 13.2);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(2, 2, 18.3, 13.2);

	glPopMatrix();

}



void cloud22() {

	glPushMatrix();
	glTranslatef(-15.0f, 4.0, 0.0f);
	glRotatef(0, 0.0f, 0.0f, -1.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(2, 2, 14, 11);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(2, 2, 16.5, 11);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(2, 2, 19.5, 11);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(2, 2, 15.5, 13.2);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(2, 2, 18.3, 13.2);

	glPopMatrix();

}




void cloud33() {

	glPushMatrix();
	glTranslatef(-30.0f, 8.0, 0.0f);
	glRotatef(0, 0.0f, 0.0f, -1.0f);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(1, 1, 19, 10);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(1, 1, 20.5, 10);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(1, 1, 22, 10);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(1, 1, 19.5, 11.2);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(1, 1, 21.3, 11.2);

	glPopMatrix();

}



void cloud1()
{
	glPushMatrix();
	glTranslatef(15.0f, 5.0f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.9f);
	circle(3, 3, 4.5, 11.5);

	glTranslatef(2.4f, 1.6f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.9f);
	circle(3, 3, 4.5, 11.5);

	glTranslatef(-4.0f, 1.0f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.9f);
	circle(3, 3, 4.5, 11.5);

	glTranslatef(-3.0f, 1.0f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
	circle(3, 3, 4.5, 11.5);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(10.0f, 5.0f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.9f);
	circle(3, 3, 4.5, 11.5);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(15.0f, 9.0f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
	circle(3, 3, 4.5, 11.5);
	glPopMatrix();

}

void cloud2()
{
	glPushMatrix();
	glTranslatef(-15.0f, 2.0f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.9f);
	circle(3, 3, 4.5, 11.5);

	glTranslatef(2.4f, 1.6f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.9f);
	circle(3, 3, 4.5, 11.5);

	glTranslatef(-4.0f, 1.0f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.9f);
	circle(3, 3, 4.5, 11.5);

	glTranslatef(2.0f, 1.0f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.9f);
	circle(3, 3, 4.5, 11.5);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-10.0f, 5.0f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.9f);
	circle(3, 3, 4.5, 11.5);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-10.0f, 3.0f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.9f);
	circle(3, 3, 4.5, 11.5);
	glPopMatrix();

}

void cloud3()
{
	glPushMatrix();
	glTranslatef(-40.0f, 2.0f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.9f);
	circle(3, 3, 4.5, 11.5);

	glTranslatef(2.4f, 1.6f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.9f);
	circle(3, 3, 4.5, 11.5);

	glTranslatef(-4.0f, 1.0f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.9f);
	circle(3, 3, 4.5, 11.5);

	glTranslatef(2.0f, 1.0f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.9f);
	circle(3, 3, 4.5, 11.5);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-36.0f, 5.0f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 0.9f);
	circle(3, 3, 4.5, 11.5);
	glPopMatrix();
	glPushMatrix();
	glTranslatef(-35.0f, 3.0f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.9f);
	circle(3, 3, 4.5, 11.5);
	glPopMatrix();

}


void Road() {

	glColor3ub(22, 21, 21);//make it black, used 3ub becasue a secondary color was needed
	glBegin(GL_QUADS);
	glVertex2f(-50, -50); //left down angle
	glVertex2f(50, -50); //right down angle
	glVertex2f(50, -35); //right up angle
	glVertex2f(-50, -35); //left up angle

	glColor3ub(252, 249, 249); //white line in middle and edges of road
	glBegin(GL_LINES);
	glVertex2f(-50, -42.5);
	glVertex2f(50, -42.5);
	glEnd();

}

void ground() {

	glPushMatrix();
	glTranslatef(0.0f, 30.0f, 0.0f);
	glRotatef(0, 0.0f, 0.0f, -1.0f);

	GLuint texture1;
	texture1 = LoadTexture("rock.bmp");

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture1);

	glColor3f(0.0, 0.6, 0.0);
	glBegin(GL_POLYGON);
	glVertex2f(-60, -65);//left
	glTexCoord2f(-60, -65);
	glVertex2f(60, -65);
	glTexCoord2f(60, -65);
	glVertex2f(60, -40);
	glTexCoord2f(60, -40);
	glVertex2f(-60, -40);
	glTexCoord2f(-60, -40);
	glEnd();
	//glEnd();
	glDisable(GL_TEXTURE_2D);
	glPopMatrix();

}

void flower1() {
	glPushMatrix();

	glBegin(GL_LINES);
	glColor3f(0, 0.5, 0);
	glVertex2f(-6, -21);
	glVertex2f(-6, -24);
	glEnd();
	glColor3f(1.0f, 0.0f, 0.0f);
	circle(-0.5, -0.5, -5.0, -20);
	circle(-0.5, -0.5, -7.0, -20);
	circle(-0.5, -0.5, -6.0, -21);
	circle(-0.5, -0.5, -6.0, -19);
	glColor3f(1.0f, 1.0f, 0.0f);
	circle(-0.5, -0.5, -6.0, -20);

	glPopMatrix();
}
void flower2() {
	glPushMatrix();
	glBegin(GL_LINES);
	glColor3f(0, 0.5, 0);
	glVertex2f(-19, -18);
	glVertex2f(-19, -21);
	glEnd();
	glColor3f(1.0f, 0.0f, 1.0f);
	circle(-0.5, -0.5, -20.0, -17);
	circle(-0.5, -0.5, -18.0, -17);
	circle(-0.5, -0.5, -19, -16);
	circle(-0.5, -0.5, -19, -18);
	glColor3f(1.0f, 1.0f, 0.0f);
	circle(-0.5, -0.5, -19, -17);

	glPopMatrix();
}
void flower3() {
	glPushMatrix();
	glBegin(GL_LINES);
	glColor3f(0, 0.5, 0);
	glVertex2f(19, -18);
	glVertex2f(19, -21);
	glEnd();
	glColor3f(1.0f, 0.0f, 1.0f);
	circle(-0.5, -0.5, 20.0, -17);
	circle(-0.5, -0.5, 18.0, -17);
	circle(-0.5, -0.5, 19, -16);
	circle(-0.5, -0.5, 19, -18);
	glColor3f(1.0f, 1.0f, 0.0f);
	circle(-0.5, -0.5, 19, -17);

	glPopMatrix();
}
void flower4() {
	glPushMatrix();
	glBegin(GL_LINES);
	glColor3f(0, 0.5, 0);
	glVertex2f(-39, -16);
	glVertex2f(-39, -19);
	glEnd();
	glColor3f(1.0f, 0.0f, 0.0f);
	circle(-0.5, -0.5, -40, -15);
	circle(-0.5, -0.5, -38.0, -15);
	circle(-0.5, -0.5, -39.0, -14);
	circle(-0.5, -0.5, -39.0, -16);
	glColor3f(1.0f, 1.0f, 0.0f);
	circle(-0.5, -0.5, -39.0, -15);

	glPopMatrix();
}
void flower5() {
	glPushMatrix();
	glBegin(GL_LINES);
	glColor3f(0, 0.5, 0);
	glVertex2f(41, -13);
	glVertex2f(41, -16);
	glEnd();
	glColor3f(1.0f, 0.0f, 0.0f);
	circle(-0.5, -0.5, 40.0, -12);
	circle(-0.5, -0.5, 42.0, -12);
	circle(-0.5, -0.5, 41.0, -13);
	circle(-0.5, -0.5, 41.0, -11);
	glColor3f(1.0f, 1.0f, 0.0f);
	circle(-0.5, -0.5, 41.0, -12);

	glPopMatrix();
}


void groundSnow() {

	glPushMatrix();
	glTranslatef(0.0f, 30.0f, 0.0f);
	glRotatef(0, 0.0f, 0.0f, -1.0f);

	glColor3f(0.95, 0.95, 0.95);
	glBegin(GL_POLYGON);
	glVertex2f(-60, -65);//left
	glVertex2f(60, -65);
	glVertex2f(60, -40);
	glVertex2f(-60, -40);
	glEnd();

	glPopMatrix();
	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, 0.1, -40);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, 5, -45);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, 25, -45);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, -25, -45);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, -40, -45);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, 0.1, -30);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, 30, -10);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, 0.1, -9);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, 0.1, -5);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, 0.1, -4);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, 0.5, -19);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, 0.5, -15);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, 0.5, -25);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, 10, -20);
	//==============================================
	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, 8, -27);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, 15, -15);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, 5, -30);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, 10, -50);

	//=================================================

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, 30, -19);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, 11, -15);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, 23, -25);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, 27, -20);

	//=====================================================

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, -30, -19);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, -40, -15);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, -35, -25);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, -10, -20);


	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, -29, -19);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, -27, -15);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, -20, -25);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.5, 0.5, -50, -20);



}


float theta, xpos, ypos;

void drawCr(float x1, float y1, float rx1, float ry1)
{

	glBegin(GL_TRIANGLE_FAN);
	for (theta = 0; theta <= (2 * PI); theta += 2 * PI / 1000) {
		xpos = x1 + rx1 * cos(theta);
		ypos = y1 + ry1 * sin(theta);
		glVertex2f(xpos, ypos);
	}
	glEnd();
}


/* 
glPushMatrix();
glTranslatef(0.0, 0.50, 0.0);
glScalef(scaleValue, scaleValue, 1.0);
glColor3f(1.0f, 1.0f, 0.0f);
draw_triangle();
glPopMatrix();
//--------------------------------
	glPushMatrix();
	glTranslatef(15.0f, 5.0f, 0.0f);
	glColor4f(1.0f, 1.0f, 1.0f, 1.9f);
	circle(3, 3, 4.5, 11.5);
*/


void drawSnowMan() {

	glColor3f(1.0f, 1.0f, 1.0f);

	// Draw Body 
	glColor3f(1.0, 1.0, 1.0);
	glBegin(GL_POLYGON);
	drawCr(25.0, -11.0, 5.0, 5.0);

	// Draw Head
	glPushMatrix();
	glScalef(1.0, 1.0, 1.0);
	glColor3f(1.0, 1.0, 1.0);
	drawCr(25.0, -5.0, 3.0, 3.0);
	glPopMatrix();

	// Draw Eyes
	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_POLYGON);
	drawCr(26.0, -3.0, 0.4, 0.4);
	drawCr(24.0, -3.0, 0.4, 0.4);

	// Draw Nose
	glColor3f(0.9, 0.6, 0.1);
	glBegin(GL_POLYGON);
	drawCr(25.0, -4.0, 0.3, 0.3);

}


void TreeSummer() {

	glColor3f(0.5, 0.25, 0.13);
	glBegin(GL_POLYGON);
	glVertex2f(28.9, -2.5);
	glVertex2f(26.8, -2.5);
	glVertex2f(26.8, -12);
	glVertex2f(28.9, -12);
	glEnd();

	glColor3f(0.0f, 0.5f, 0.0f);
	circle(1.5, 1.5, 30.5, -1.5);

	glColor3f(0.0f, 0.5f, 0.0f);
	circle(1.5, 1.5, 28.5, -1.5);

	glColor3f(0.0f, 0.5f, 0.0f);
	circle(1.5, 1.5, 27.5, -1.5);

	glColor3f(0.0f, 0.5f, 0.0f);
	circle(1.5, 1.5, 25, -1.5);

	glColor3f(0.0f, 0.5f, 0.0f);
	circle(1.5, 1.5, 29.5, -0.6);

	glColor3f(0.0f, 0.5f, 0.0f);
	circle(1.5, 1.5, 28.1, 0.6);

	glColor3f(0.0f, 0.5f, 0.0f);
	circle(1.5, 1.5, 26.3, 0.6);

	glColor3f(0.0f, 0.5f, 0.0f);
	circle(2, 2, 28, 1.7);


}


void car()
{
	glPushMatrix();

	glColor3f(1.0, 0.0, 0.0); //car outside

	glTranslatef(tx, ty, 0);
	glBegin(GL_QUADS);
	glVertex2d(-42.0, -48.0);
	glVertex2d(-19.0, -48.0);
	glVertex2d(-19.0, -43.0);
	glVertex2d(-42.0, -43.0);
	glEnd();
	glBegin(GL_QUADS);
	glVertex2d(-37.0, -39.0);
	glVertex2d(-39.0, -43.0);
	glVertex2d(-22.0, -43.0);
	glVertex2d(-24.0, -39.0);
	glEnd();
	glColor3f(0.0f, 0.0f, 0.0f); //car window
	glBegin(GL_QUADS);
	glVertex2d(-36.0, -40.0);
	glVertex2d(-38.0, -44.0);
	glVertex2d(-23.0, -44.0);
	glVertex2d(-25.0, -40.0);
	glEnd();
	glColor3f(1.0f, 0.0f, 0.0f); //line between windows
	glBegin(GL_QUADS);
	glVertex2d(-31.0, -44.0);
	glVertex2d(-29.0, -44.0);
	glVertex2d(-29.0, -40.0);
	glVertex2d(-31.0, -40.0);
	glEnd();
	glColor3f(0.0f, 0.0f, 0.0f);
	circle(2.5, 2.5, -36, -47);
	circle(2.5, 2.5, -26, -47);

	glColor3ub(247, 244, 245); //wheels
	circle(1, 1, -36, -47);
	circle(1, 1, -26, -47);

	glPopMatrix();

}


//car2
void car2()
{
	glPushMatrix();
	glColor3f(1.0, 1.0, 0.0);

	glTranslatef(tx, ty, 0);
	glBegin(GL_QUADS);
	glVertex2d(-22.0, -40.0);
	glVertex2d(1.0, -40.0);
	glVertex2d(1.0, -35.0);
	glVertex2d(-22.0, -35.0);
	glEnd();
	glBegin(GL_QUADS);
	glVertex2d(-17.0, -31.0);
	glVertex2d(-19.0, -35.0);
	glVertex2d(-2.0, -35.0);
	glVertex2d(-4.0, -31.0);
	glEnd();
	glColor3f(0.0f, 0.0f, 0.0f);
	glBegin(GL_QUADS);
	glVertex2d(-16.0, -32.0);
	glVertex2d(-18.0, -36.0);
	glVertex2d(-3.0, -36.0);
	glVertex2d(-5.0, -32.0);
	glEnd();
	glColor3f(1.0f, 1.0f, 0.0f);
	glBegin(GL_QUADS);
	glVertex2d(-11.5, -36.0);
	glVertex2d(-9.5, -36.0);
	glVertex2d(-9.5, -32.0);
	glVertex2d(-11.5, -32.0);
	glEnd();
	glColor3f(0.0f, 0.0f, 0.0f);
	circle(2.5, 2.5, -14.7, -39);
	circle(2.5, 2.5, -4.7, -39);

	glColor3ub(247, 244, 245);
	circle(1, 1, -14.7, -39);
	circle(1, 1, -4.7, -39);

	glPopMatrix();

}



void car_Run() {

	car_run = car_run + 0.6;
	if (car_run > 100)
		car_run = -50;
	glutPostRedisplay();

}

void car_Run2() {

	car_run2 = car_run2 - 0.6;
	if (car_run2 < -100)
		car_run2 = 70;
	glutPostRedisplay();
}

void cloud11_move() {

	cloudRight = cloudRight + 0.02;
	if (cloudRight > 100)
		cloudRight = -70;
	glutPostRedisplay();
}

void cloud22_move() {

	cloudLeft = cloudLeft - 0.01;
	if (cloudLeft < -60)
		cloudLeft = 70;
	glutPostRedisplay();

}


void cloud33_move() {

	cloudRight = cloudRight + 0.02;
	if (cloudRight > 100)
		cloudRight = -50;
	glutPostRedisplay();

}

void House1() {

	GLuint texture;
	texture = LoadTexture("rock.bmp");

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture);


	glColor3f(1, 1, 1);
	glBegin(GL_POLYGON);
	glVertex2f(-3.2, -15);
	glTexCoord2f(-3.2, -15);//left down
	glVertex2f(14, -15);
	glTexCoord2f(14, -15);//right down
	glVertex2f(14, 5);
	glTexCoord2f(14, 5);//right up
	glVertex2f(-3.2, 5);
	glTexCoord2f(-3.2, 5);//left up
	glEnd();
	glDisable(GL_TEXTURE_2D);

	GLfloat white[] = { 1.0f, 1,1,0.01 };
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	glColor3f(0, 0, 0);
	glBegin(GL_POLYGON);
	glVertex2f(-1.7, -6);
	glVertex2f(1.8, -6);
	glVertex2f(1.8, 4.3);
	glVertex2f(-1.7, 4.3);
	glEnd();

	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);


	glColor3f(0.0, 0.0, 0.0); //left window horz line
	glBegin(GL_POLYGON);
	glVertex2f(-0.1, -6); //left down
	glVertex2f(0.0, -6); //right down
	glVertex2f(0.0, 4.3);//right up
	glVertex2f(-0.1, 4.3); //left up
	glEnd();


	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_POLYGON);
	glVertex2f(-1.7, -3.35);
	glVertex2f(1.8, -3.35);
	glVertex2f(1.8, -3.45);
	glVertex2f(-1.7, -3.45);
	glEnd();

	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_POLYGON);
	glVertex2f(-1.7, 1.7);
	glVertex2f(1.8, 1.7);
	glVertex2f(1.8, 1.85);
	glVertex2f(-1.7, 1.85);
	glEnd();



	//GLfloat white[] = { 1.0f, 1,1,0.01 };
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	glLightfv(GL_LIGHT0, GL_SPECULAR, white);
	glMaterialfv(GL_FRONT, GL_SPECULAR, white);
	glColor3f(0, 0, 0);
	glBegin(GL_POLYGON);
	glVertex2f(9, -6);
	glVertex2f(12.3, -6);
	glVertex2f(12.3, 4.3);
	glVertex2f(9, 4.3);
	glEnd();

	glDisable(GL_LIGHT0);
	glDisable(GL_LIGHTING);

	//glColor3f(1.0, 1.0, 1.0); //right window
	//glBegin(GL_POLYGON);
	//glVertex2f(9, -6);
	//glVertex2f(12.3, -6);
	//glVertex2f(12.3, 4.3);
	//glVertex2f(9, 4.3);
	//glEnd();

	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_POLYGON);
	glVertex2f(10.65, -6);
	glVertex2f(10.72, -6);
	glVertex2f(10.72, 4.3);
	glVertex2f(10.65, 4.3);
	glEnd();

	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_POLYGON);
	glVertex2f(9, 1.70);
	glVertex2f(12.3, 1.70);
	glVertex2f(12.3, 1.85);
	glVertex2f(9, 1.85);
	glEnd();

	glColor3f(0.0, 0.0, 0.0);
	glBegin(GL_POLYGON);
	glVertex2f(9, -3.35);
	glVertex2f(12.3, -3.35);
	glVertex2f(12.3, -3.45);
	glVertex2f(9, -3.45);
	glEnd();


	//door
	glColor3f(0.35, 0.35, 0.35);
	glBegin(GL_POLYGON);
	glVertex2f(3, -15);
	glVertex2f(8, -15);
	glVertex2f(8, -7);
	glVertex2f(3, -7);
	glEnd();

	//Knob of door
	glColor3f(0.9, 0.9, 0.9);
	circle(0.3, -0.3, 7, -10);


}



void TreeWinter() {

	glColor3f(0.5, 0.30, 0.12);
	glBegin(GL_POLYGON);
	glVertex2f(34, -6);
	glVertex2f(36, -6);
	glVertex2f(36, -10);
	glVertex2f(34, -10);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(1, 1, 1);
	glVertex2f(30.0, 0);
	glVertex2f(40.0, 0);
	glVertex2f(35, 4);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(1, 1, 1);
	glVertex2f(30.0, -3);
	glVertex2f(40.0, -3);
	glVertex2f(35, 3);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(1, 1, 1);
	glVertex2f(30.0, -6);
	glVertex2f(40.0, -6);
	glVertex2f(35, 2);
	glEnd();

}

void Moutiain() {

	glBegin(GL_TRIANGLES);
	glColor3f(0.4f, 0.30f, 0.12f);
	glVertex2f(-30.0, -10);
	glVertex2f(-40.0, -10);
	glVertex2f(-35, 2);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(0.5, 0.33, 0.12);
	glVertex2f(-35.0, -10);
	glVertex2f(-45.0, -10);
	glVertex2f(-35, 2);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(0.5, 0.33, 0.12);
	glVertex2f(-40.0, -10);
	glVertex2f(-50.0, -10);
	glVertex2f(-35, 2);
	glEnd();


	//==============================================

	glBegin(GL_TRIANGLES);
	glColor3f(0.4, 0.30, 0.12);
	glVertex2f(-20.0, -10);
	glVertex2f(-30.0, -10);
	glVertex2f(-25, 3);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(0.5, 0.30, 0.12);
	glVertex2f(-25.0, -10);
	glVertex2f(-35.0, -10);
	glVertex2f(-25, 3);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(0.5, 0.30, 0.12);
	glVertex2f(30.0, -10);
	glVertex2f(-40.0, -10);
	glVertex2f(-25, 3);
	glEnd();




}


void Summer() {
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	sky();
	Sun();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(transValueX, transValueY, transValueZ, 0, 0, 0, 0, 1, 0);

	/////////////////////////////////////////
	// Exercise 1 TODO: create light 0
	//////
	glMaterialfv(GL_FRONT, GL_SPECULAR, mat_specular);
	glMaterialfv(GL_FRONT, GL_SHININESS, shininess);
	/////////////////////////////////////////
	// Exercise 2 TODO: change the value of the light position and the value of shininess
	//////
	glLightfv(GL_LIGHT0, GL_AMBIENT, light_ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, light_specular);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);

	/////////////////////////////////////////
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);

	glEnable(GL_DEPTH_TEST);

	glDisable(GL_DEPTH_TEST);
	glDisable(GL_LIGHTING);

	Moutiain();
	cloud1();
	//cloud2();
	cloud3();
	cloud11();
	cloud22();
	cloud33();
	ground();
	Road();
	TreeSummer();
	flower1();
	flower2();
	flower3();
	flower4();
	flower5();
	House1();
	car_Run2();

	for (int i = 0; i < 1; i++)
	{
		glPushMatrix();
		glTranslatef(car_run2 + i, 0, 0);
		car2();
		glPopMatrix();
	}



	car_Run();
	for (int i = 0; i < 1; i++)
	{
		glPushMatrix();
		glTranslatef(car_run + i, 0, 0);
		car();
		glPopMatrix();
	}


	cloud11_move();
	for (int i = 0; i < 1; i++)
	{
		glPushMatrix();
		glTranslatef(cloudRight + i, 0, 0);
		cloud11();
		glPopMatrix();
	}

	cloud33_move();
	for (int i = 0; i < 1; i++)
	{
		glPushMatrix();
		glTranslatef(cloudLeft + i, 0, 0);
		cloud33();
		glPopMatrix();
	}

	cloud22_move();
	for (int i = 0; i < 1; i++)
	{
		glPushMatrix();
		glTranslatef(cloudRight + i, 0, 0);
		cloud22();
		glPopMatrix();
	}


	glutSwapBuffers();


}


void MoutiainSnow() {

	//the mountain in the back
	glBegin(GL_TRIANGLES);
	glColor3f(1, 1, 1);
	glVertex2f(-30.0, -10);
	glVertex2f(-40.0, -10);
	glVertex2f(-35, 2);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(1, 1, 1);
	glVertex2f(-35.0, -10);
	glVertex2f(-45.0, -10);
	glVertex2f(-35, 2);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(1, 1, 1);
	glVertex2f(-40.0, -10);
	glVertex2f(-50.0, -10);
	glVertex2f(-35, 2);
	glEnd();


	//==============================================

	glBegin(GL_TRIANGLES);
	glColor3f(1, 1, 1);
	glVertex2f(-20.0, -10);
	glVertex2f(-30.0, -10);
	glVertex2f(-25, 3);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(1, 1, 1);
	glVertex2f(-25.0, -10);
	glVertex2f(-35.0, -10);
	glVertex2f(-25, 3);
	glEnd();

	glBegin(GL_TRIANGLES);
	glColor3f(1, 1, 1);
	glVertex2f(30.0, -10);
	glVertex2f(-40.0, -10);
	glVertex2f(-25, 3);
	glEnd();




}


void Winter() {
	glClear(GL_COLOR_BUFFER_BIT);

	MoutiainSnow();
	groundSnow();
	TreeWinter();
	House1();
	drawSnow();
	drawSnowMan();
	
	//moon

	//first circule of moon 
	glColor3f(1.0f, 1.0f, 0.96f);
	circle(3, 3, 29.5, 17.5);
	//a black circule to go over the white moon
	glColor3f(0.0f, 0.0f, 0.0f);
	circle(3, 3, 28.5, 18.5);

	///starts
	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.2, 0.3, 10.5, 16.5);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.2, 0.3, -5.5, 10.5);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.2, 0.3, -8.5, 12.5);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.2, 0.3, -20, 18.5);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.2, 0.3, -15, 7.5);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.2, 0.3, -25, 11.5);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.2, 0.3, -32, 14.5);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.2, 0.3, 0, 18);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.2, 0.3, -8, 20.5);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.2, 0.3, 8, 13.5);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.2, 0.3, 15, 11.5);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.2, 0.3, 19, 18.5);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.2, 0.3, 14, 20.5);

	glColor3f(1.0f, 1.0f, 1.0f);
	circle(0.2, 0.3, 16, 26.6);


	Road();
	drawSnow();

	//function that moves cars
	car_Run2();
	for (int i = 0; i < 1; i++)
	{
		glPushMatrix();
		glTranslatef(car_run2 + i, 0, 0);
		car2();
		glPopMatrix();
	}


	car_Run();
	for (int i = 0; i < 1; i++)
	{
		glPushMatrix();
		glTranslatef(car_run + i, 0, 0);
		car();
		glPopMatrix();
	}

	glutSwapBuffers();

}


void spe_key(int key, int x, int y) {

	switch (key) {

	case GLUT_KEY_UP:
		t = 1;
		glutPostRedisplay();
		break;



		//stop sound
	case GLUT_KEY_DOWN:
		PlaySound(NULL, NULL, 0);
		t = 0;
		glutPostRedisplay();
		break;

	default:
		break;


	}

}


void handleKeypress(unsigned char key, int x, int y) {

	switch (key) {

	case 's':
		glutDisplayFunc(Summer);
		glutKeyboardFunc(handleKeypress);
		PlaySound(TEXT("summer.wav"), NULL, SND_ASYNC | SND_LOOP);
		glutPostRedisplay();
		break;

	case 'S':
		glutDisplayFunc(Summer);
		glutKeyboardFunc(handleKeypress);
		PlaySound(TEXT("summer.wav"), NULL, SND_ASYNC | SND_LOOP);
		glutPostRedisplay();
		break;

	case 'w':
		glutDisplayFunc(Winter);
		glutKeyboardFunc(handleKeypress);
		PlaySound(TEXT("snow.wav"), NULL, SND_ASYNC | SND_FILENAME);
		drawSnow();
		glutPostRedisplay();
		break;

	case 'W':
		glutDisplayFunc(Winter);
		glutKeyboardFunc(handleKeypress);
		PlaySound(TEXT("snow.wav"), NULL, SND_ASYNC | SND_FILENAME);
		drawSnow();
		glutPostRedisplay();
		break;


	}
}



static
void mouse(int button, int state, int x, int y)
{

	if (button == GLUT_LEFT_BUTTON) {

		if (state == GLUT_UP) {
			glutDisplayFunc(Summer);
			PlaySound(TEXT("summer.wav"), NULL, SND_ASYNC | SND_LOOP);
			glutMouseFunc(mouse);
			glutPostRedisplay();
		}
	}


	if (button == GLUT_RIGHT_BUTTON) {

		if (state == GLUT_UP) {
			glutDisplayFunc(Winter);
			PlaySound(TEXT("snow.wav"), NULL, SND_ASYNC | SND_FILENAME);
			glutMouseFunc(mouse);
			drawSnow();
			glutPostRedisplay();
		}
	}
}



int main(int argc, char** argv) {
	srand(50);
	initSnow();
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE);
	glutInitWindowPosition(30, 0);
	glutInitWindowSize(1000, 650);
	glutCreateWindow("summer vs winter");
	PlaySound(TEXT("summer.wav"), NULL, SND_ASYNC | SND_LOOP);
	glEnable(GL_BLEND); //Enable blending.
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); //Set blending function.
	init();
	glutSpecialFunc(spe_key);
	glutDisplayFunc(Summer);
	//glutIdleFunc(idle); //  define what function to call when the program is idle
	glutKeyboardFunc(handleKeypress);
	glutMouseFunc(mouse);

	glutMainLoop();
	return 0;
}
