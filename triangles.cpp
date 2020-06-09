#include <cmath>
#include <cstdio>
#include <Windows.h>
#include <ctime>
#include <GL\GL.H>
#include <GL\GLU.H>
#include <GL\glut.h>


class triangle {
	public:
	float vertice[3][3];
	float normal[3];
	float center[3]; // centroid
	float axe[3]; // rotation axe
	float angle, angle0; // rotation angles, around centroid and origin.
	float delta, delta0; // angle increment
	unsigned char color[4];

	triangle(){}

	triangle(float *v1, float *v2, float *v3) {
		for (int i = 0; i < 3; ++i) {
			center[i] = (1/3.0f)*(v1[i] + v2[i] + v3[i]);
			vertice[0][i] = v1[i] - center[i]; // To draw in correct place:
			vertice[1][i] = v2[i] - center[i];
			vertice[2][i] = v3[i] - center[i];
		}
		normal[0] = (v2[1] - v1[1])*(v3[2] - v1[2]) - (v2[2] - v1[2])*(v3[1] - v1[1]);
		normal[1] = (v2[2] - v1[2])*(v3[0] - v1[0]) - (v2[0] - v1[0])*(v3[2] - v1[2]);
		normal[2] = (v2[0] - v1[0])*(v3[1] - v1[1]) - (v2[1] - v1[1])*(v3[0] - v1[0]);
		float f = 1.0f/sqrt(normal[0]*normal[0] + normal[1]*normal[1] + normal[2]*normal[2]);
		for (int i = 0; i < 3; ++i)
			normal[i] *= f;
		angle = angle0 = 0.0f;
		delta = delta0 = 0.0f;
	}

	void set_color(int r, int g, int b, int a = 255) {
		color[0] = r%256;
		color[1] = g%256;
		color[2] = b%256;
		color[3] = a%256;
	}

	void set_axe(float axex, float axey, float axez) {
		axe[0] = axex; axe[1] = axey; axe[2] = axez;
	}

	void rotate() {
		angle += delta;
		if(angle > 180.0f)
			angle -= 360.0f;
	}

	void rotate0() {
		angle0 += delta0;
		if(angle0 > 180.0f)
			angle0 -= 360.0f;
	}

	void set_delta_angle(float d) { delta = d; }

	void set_delta_angle0(float d) { delta0 = d; }

	void drawgl() {
		glPushMatrix();
		glRotatef(angle0, axe[0], axe[1], axe[2]); // rotation around origin
		
			glPushMatrix();
				glTranslatef(center[0], center[1], center[2]);
				glRotatef(angle, axe[0], axe[1], axe[2]); // rotation around centroid
				glColor4ubv(color);
				glNormal3fv(normal);
				glBegin(GL_POLYGON); // triangle area
					for (int i = 0; i < 3; ++i)
						glVertex3fv(vertice[i]);
				glEnd();
				glColor3ub(0, 0, 0); //black
				glBegin(GL_LINE_LOOP); // triangle border
					for (int i = 0; i < 3; ++i)
						glVertex3fv(vertice[i]);
				glEnd();
			glPopMatrix();
		glPopMatrix();
	}
};

// Global variables
int SCREEN_WIDTH = 600, SCREEN_HEIGHT = 600;
int numTriangles = 24;
triangle *Triangle[24];
int framespersecond = 30;

void idlefunc()
{
	int end = clock() + CLOCKS_PER_SEC / framespersecond;
	static double factor_pos = 1.0055;
	static int count = 0;

	if (++count > 120)
	{
		factor_pos = 1.0/factor_pos;
		count = 0;
	}

	for (int i = 0; i < numTriangles; ++i)
	{
		Triangle[i]->center[0] *= factor_pos; // spread out triangles in x direction
		Triangle[i]->center[1] *= factor_pos; // spread out triangles in y direction
		Triangle[i]->rotate(); // rotate around centroid
		Triangle[i]->rotate0(); // rotate around origin
	}
	glutPostRedisplay();

	while(clock() < end); //wait 1/framespersecond seconds
}

void MyDisplay(void) {
	// The new scene
	glClear(GL_COLOR_BUFFER_BIT);
	glLoadIdentity();
	for(int i = 0; i < numTriangles; ++i)
		Triangle[i]->drawgl();
	// The end of scene
	glutSwapBuffers();
}

void Resize(int w, int h) {
	SCREEN_WIDTH = w; SCREEN_HEIGHT = h;
	glViewport(0, 0, w, h);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	if (w < h)
		glOrtho(-1.0,1.0,-h/(double)w,h/(double)w,-10.0,10.0);
	else
		glOrtho(-w/(double)h,w/(double)h,-1.0,1.0,-10.0,10.0);
	glMatrixMode(GL_MODELVIEW);
	glutPostRedisplay();
}

void MyInit(void) {
	glutReshapeFunc(Resize); // register the reshape function
	glutIdleFunc(idlefunc);
	glClearColor(1.0, 1.0, 1.0, 0.0);//select clearing (background) color
									 /* initialize viewing values */
}

#define TRISIZE 0.19f
void initTriangles()
{
	float v1[3] = {0.0f, 0.0f, 0.0f};
	float v2[3] = {TRISIZE, 0.0f, 0.0f};
	float v3[3] = {0.0f, TRISIZE, 0.0f};

	Triangle[0] = new triangle(v1, v2, v3);
	Triangle[0]->set_color(0, 0, 255); // blue
	Triangle[0]->set_axe(0.0f, 0.0f, 1.0f);
	Triangle[0]->set_delta_angle(3.0f);
	Triangle[0]->set_delta_angle0(-2.0f);

	Triangle[1] = new triangle();
	*Triangle[1] = *Triangle[0]; // cloning the first triangle
	Triangle[1]->center[0] += TRISIZE;
	Triangle[1]->set_color(255, 102, 0); // orange

	Triangle[2] = new triangle();
	*Triangle[2] = *Triangle[1];
	Triangle[2]->center[0] += TRISIZE;
	Triangle[2]->set_color(255, 0, 255); // margenta

	Triangle[3] = new triangle();
	*Triangle[3] = *Triangle[0];
	Triangle[3]->center[1] += TRISIZE;
	Triangle[3]->set_color(0, 255, 0); // green

	Triangle[4] = new triangle();
	*Triangle[4] = *Triangle[3];
	Triangle[4]->center[0] += TRISIZE;
	Triangle[4]->set_color(255, 255, 0); // yellow

	Triangle[5] = new triangle();
	*Triangle[5] = *Triangle[3];
	Triangle[5]->center[1] += TRISIZE;
	Triangle[5]->set_color(255, 0, 0); // red

	Triangle[1]->set_delta_angle0(2.0f);
	Triangle[3]->set_delta_angle0(2.0f);

	for(int j = 6; j < 24; j += 6)
		for(int i = 0; i < 6; ++i) {
			Triangle[i + j] = new triangle();
			*Triangle[i + j] = *Triangle[i + j - 6];
			float t = Triangle[i + j]->center[1];
			Triangle[i + j]->center[1] = Triangle[i + j]->center[0];
			Triangle[i + j]->center[0] = -t;
			for(int k = 0; k < 3; ++k) {
				t = Triangle[i + j]->vertice[k][1];
				Triangle[i + j]->vertice[k][1] = Triangle[i + j]->vertice[k][0];
				Triangle[i + j]->vertice[k][0] = -t;
			}
		}
}

int main(int argc, char** argv) { //<- for normal API
	initTriangles();
	glutInit(&argc, argv);
	// use double buffer to get better results on animation
	// use depth buffer for hidden surface removal
	glutInitDisplayMode( GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH );
	
	glutInitWindowSize( SCREEN_WIDTH, SCREEN_HEIGHT );
	glutCreateWindow("Exercise #2");//create widnow, hello title bar

	MyInit();
	glutDisplayFunc(MyDisplay);//
	glutMainLoop();//enter main loop and process events
	return 0;
}
