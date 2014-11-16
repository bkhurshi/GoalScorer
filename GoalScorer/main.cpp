
#include <GL/glut.h>

GLfloat light_diffuse[] = {1.0, 1.0, 1.0, 1.0};  /* Red diffuse light. */
GLfloat light_position[] = {0.0, 10.0, 0.0, 1.0};  /* Infinite light location. */

/** Walls **/
GLfloat wallN[6][3] = {  /* Normals for the 6 faces of a cube. */
	{1.0, 0.0, 0.0}, {0.0, -1.0, 0.0}, {-1.0, 0.0, 0.0},
	{0.0, 1.0, 0.0}, {0.0, 0.0, -1.0}, {0.0, 0.0, 1.0} };

	GLint wallF[6][4] = {  /* Vertex indices for the 6 faces of a cube. */
		{0, 1, 2, 3}, {3, 2, 6, 7}, {7, 6, 5, 4},
		{4, 5, 1, 0}, {5, 6, 2, 1}, {7, 4, 0, 3} };
		GLfloat wallV[8][3];  /* Will be filled in with X,Y,Z vertexes. */
;
void drawWalls(void)
{
	GLfloat green[4] = {0.0, 1.0, 0.0, 1.0};
	GLfloat brown[4] = {0.7, 0.5, 0.2, 1.0};
	for (int i = 0; i < 6; i++) {
		glBegin(GL_QUADS);
		if(i == 3)
		{
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, green);
		}
		else
		{
			glMaterialfv(GL_FRONT, GL_AMBIENT_AND_DIFFUSE, brown);
		}
		//glNormal3fv(&wallN[i][0]);
		glVertex3fv(&wallV[wallF[i][0]][0]);
		glVertex3fv(&wallV[wallF[i][1]][0]);
		glVertex3fv(&wallV[wallF[i][2]][0]);
		glVertex3fv(&wallV[wallF[i][3]][0]);
		glEnd();
	}
}

void drawNet(void)
{
	GLfloat black[4] = {1.0, 1.0, 1.0, 1.0};
	glBegin(GL_QUADS);
	glNormal3f(0.0, 0.0, 1.0);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, black); 
	glVertex3f(3.65, -10.0, -59.9);
	glVertex3f(3.65, -7.56, -59.9);
	glVertex3f(-3.65, -7.56, -59.9);
	glVertex3f(-3.65, -10.0, -59.9);
	glEnd();
}

void drawBall(void)
{
	GLfloat red[4] = {1.0, 0.0, 0.0, 1.0};
	GLUquadric* qobj = gluNewQuadric();
	glPushMatrix();
	glTranslatef(10.0, -9.0, -5.0);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, red); 
	gluSphere(qobj, 1, 9, 9);
	glColor3f(0,0,1.);
	glPopMatrix();
	gluDeleteQuadric(qobj);
	glEndList();
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawWalls();
	drawNet();
	drawBall();
	glutSwapBuffers();
}

void init(void)
{
	/* Setup cube vertex data. */
	wallV[0][0] = wallV[1][0] = wallV[2][0] = wallV[3][0] = -30;
	wallV[4][0] = wallV[5][0] = wallV[6][0] = wallV[7][0] = 30;
	wallV[0][1] = wallV[1][1] = wallV[4][1] = wallV[5][1] = -10;
	wallV[2][1] = wallV[3][1] = wallV[6][1] = wallV[7][1] = 10;
	wallV[1][2] = wallV[2][2] = wallV[5][2] = wallV[6][2] = -60;
	wallV[0][2] = wallV[3][2] = wallV[4][2] = wallV[7][2] = 0.1;

	/* Enable a single OpenGL light. */
	glLightfv(GL_LIGHT0, GL_AMBIENT_AND_DIFFUSE, light_diffuse);
	glLightfv(GL_LIGHT0, GL_POSITION, light_position);
	glEnable(GL_LIGHT0);
	glEnable(GL_LIGHTING);


	/* Use depth buffering for hidden surface elimination. */
	  glEnable(GL_DEPTH_TEST);

	/* Setup the view of the cube. */
	glMatrixMode(GL_PROJECTION);
	gluPerspective( /* field of view in degree */ 50.0,
		/* aspect ratio */ 1.777,
		/* Z near */ 30.0, /* Z far */ 200.0);
	glMatrixMode(GL_MODELVIEW);
	gluLookAt(0.0, 0.0, 30.0,  /* eye is at (0,0,5) */
		0.0, 0.0, 0.0,      /* center is at (0,0,0) */
		0.0, 1.0, 0.);      /* up is in positive Y direction */
}

int main(int argc, char **argv)
{
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800, 450); // window size
	glutInitWindowPosition(100, 100); // window location
	int handle = glutCreateWindow("Goal Scorer"); // param is the title of window
	//glutFullScreen();
	glutDisplayFunc(display);
	init();
	glutMainLoop();
	return 0;             /* ANSI C requires main to return int. */
}
