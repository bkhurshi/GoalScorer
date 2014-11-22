
#include <windows.h>
#include <GL/glut.h>
#include <stdio.h> 
#include <stdlib.h>
#include <time.h>  

#include <iostream>
#include <fstream>
#include <sstream>
#include <iomanip>

using namespace std;


#define HAPTIC // comment this line to take the haptic off

void mouseCB(int button, int stat, int x, int y);
void keyboardCB(unsigned char key, int x, int y);
void idleCB();
void reshapeCB(int w, int h);

void showInfo();

int goalsScored = 0;
void *font = GLUT_BITMAP_8_BY_13;
bool mouseLeftDown;
bool mouseRightDown;
float mouseX, mouseY;


ofstream logFile;
int trialNumber;


GLfloat ballM = 1;
const GLfloat ballDorig[3] = {10.0, -9.0, -10.0};
GLfloat ballDold[3] = {10.0, -9.0, -10.0};
GLfloat ballDnew[3] = {10.0, -9.0, -10.0};
GLfloat ballVold[3] = {0.0, 0.0, 0.0};
GLfloat ballVnew[3] = {0.0, 0.0, 0.0};
GLfloat ballPold[3] = {0.0, 0.0, 0.0};
GLfloat ballPnew[3] = {0.0, 0.0, 0.0};


// haptic code begin
#ifdef HAPTIC

#include <HL/hl.h>
#include <HDU/hduMatrix.h>
#include <HDU/hduError.h>
#include <HLU/hlu.h>


/* Haptic device and rendering context handles. */
static HHD ghHD = HD_INVALID_HANDLE;
static HHLRC ghHLRC = 0;

/* Shape id for shape we will render haptically. */
HLuint gWallsId;
HLuint gNetId;
HLuint gBallId;

HLdouble prevProxyTransform[16];
HLdouble proxytransform[16];

GLfloat cursorM = 0.25*ballM;
// position
GLfloat cursorDold[3] = {0.0, 0.0, 0.0};
GLfloat cursorDnew[3] = {0.0, 0.0, 0.0};
GLfloat cursorVold[3] = {0.0, 0.0, 0.0};
GLfloat cursorVnew[3] = {0.0, 0.0, 0.0};
// momentum
GLfloat cursorPold[3] = {0.0, 0.0, 0.0};
GLfloat cursorPnew[3] = {0.0, 0.0, 0.0};
boolean touched = false;

#define CURSOR_SCALE_SIZE 60
static double gCursorScale;
static GLuint gCursorDisplayList = 0;
#else
#include <math.h>
#endif

// haptic code finish

// haptic code begin
#ifdef HAPTIC
void exitHandler(void);
void initHL();
void drawSceneHaptics();
void drawHapticCursor();
void updateWorkspace();
#endif
// haptic code finish

// haptic callback
#ifdef HAPTIC
// hello bally--time to make it move when pushed
void HLCALLBACK touchShapeCallback(HLenum event, HLuint object, HLenum thread, 
                                   HLcache *cache, void *userdata)
{
	touched = true;

	for (int i = 0; i < 3; i++) {
		ballPold[i] = ballPnew[i];
		ballPnew[i] = ballM*ballVnew[i] + cursorM*cursorVnew[i];
	}

	for (int i = 0; i < 3; i++) {
		ballVold[i] = ballVnew[i];
		ballVnew[i] = ballVold[i]*((ballM - cursorM) / (ballM + cursorM)) + cursorVnew[i]*((2*cursorM)/(ballM + cursorM));
	}
}

// in case we need it--probably good for walls and such
void HLCALLBACK touchShapeCallback2(HLenum event, HLuint object, HLenum thread, 
                                   HLcache *cache, void *userdata)
{

}
#endif

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
GLfloat wallP[3][2] = {{-30., 30.0}, {-10.0, 10.0}, {-60.0, 0.1}};

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

GLfloat netP[2][2] = {{-4, 4}, {-10.0, -5}};
GLfloat netZ = -59.9;
void drawNet(void)
{
	GLfloat black[4] = {1.0, 1.0, 1.0, 1.0};
	glBegin(GL_QUADS);
	glNormal3f(0.0, 0.0, 1.0);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, black); 
	glVertex3f(netP[0][0], netP[1][0], netZ);
	glVertex3f(netP[0][0], netP[1][1], netZ);
	glVertex3f(netP[0][1], netP[1][1], netZ);
	glVertex3f(netP[0][1], netP[1][0], netZ);
	glEnd();
}

GLdouble ballR = 1.0;
GLdouble gravity = -.001;
GLdouble resistance = 0.01;
void calcNewBallPos() {
	for(int i = 0; i < 3; i++)
	{
		ballDold[i] = ballDnew[i];
		ballDnew[i] = ballDold[i] + ballVnew[i];
	}
}

void drawBall(void)
{
	// re-draw ball in new position
	GLfloat red[4] = {1.0, 0.0, 0.0, 1.0};
	GLUquadric* qobj = gluNewQuadric();
	glPushMatrix();
	calcNewBallPos();
	glTranslatef(ballDnew[0], ballDnew[1], ballDnew[2]);
	glMaterialfv(GL_FRONT, GL_DIFFUSE, red); 
	gluSphere(qobj, ballR, 9, 9);
	glColor3f(0,0,1.);
	glPopMatrix();
	gluDeleteQuadric(qobj);
	glEndList();
}

void ballPhysics(void)
{
	// update velocity
	for(int i = 0; i < 3; i++)
	{
		ballVold[i] = ballVnew[i];
	}
	
	// decay (air-resistance)
	for(int i = 0; i < 3; i++)
	{
		ballVnew[i] = ballVnew[i] - ballVnew[i] * resistance;
	}

	// wall Detection
	for(int i = 0; i < 3; i++)
	{
		if(ballDnew[i] - ballR <= wallP[i][0] || ballDnew[i] + ballR >= wallP[i][1])
		{
			ballVnew[i] = - ballVnew[i];
		}
	}
	
	// gravity
	if(ballDnew[1] - 1 > -10 && ballDnew[1] - 1 < 10)
	{		
		ballVnew[1] = ballVnew[1] + gravity;
	}
}

void goalDetection(void)
{
	if(ballDnew[2] - ballR > netZ)
	{
		ballPhysics();
		return; // not a goal
	}

	if ((ballDnew[0] - ballR >= netP[0][0] && ballDnew[0] + ballR <= netP[0][1]) && 
		(ballDnew[1] - ballR >= netP[1][0] && ballDnew[1] + ballR <= netP[1][1]))
	{
		// in Net
		goalsScored++;
		const time_t rawTime = time(NULL);
		const tm curTime = *localtime(&rawTime);
		logFile << goalsScored << "," << curTime.tm_hour << ":" << curTime.tm_min << ":" << curTime.tm_sec << "\n";
		
		// reset ball to start at original position
		for(int i = 0; i < 3; i++)
		{
			ballDnew[i] = ballDorig[i];
			ballDold[i] = ballDorig[i];
			ballVnew[i] = 0.0;
			ballVold[i] = 0.0;
			ballPnew[i] = 0.0;
			ballPold[i] = 0.0;
		}
	}
	else
	{
		ballPhysics();
	}
}

void display(void)
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	drawWalls();
	drawNet();
	drawBall();

	goalDetection();
	#ifdef HAPTIC
	drawSceneHaptics();
	drawHapticCursor();
	#endif
	showInfo();
	glutSwapBuffers();
}

void init(void)
{
	/* Setup cube vertex data. */
	wallV[0][0] = wallV[1][0] = wallV[2][0] = wallV[3][0] = wallP[0][0];
	wallV[4][0] = wallV[5][0] = wallV[6][0] = wallV[7][0] = wallP[0][1];
	wallV[0][1] = wallV[1][1] = wallV[4][1] = wallV[5][1] = wallP[1][0];
	wallV[2][1] = wallV[3][1] = wallV[6][1] = wallV[7][1] = wallP[1][1];
	wallV[1][2] = wallV[2][2] = wallV[5][2] = wallV[6][2] = wallP[2][0];
	wallV[0][2] = wallV[3][2] = wallV[4][2] = wallV[7][2] = wallP[2][1];

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

	glutIdleFunc(idleCB);                       // redraw when idle
	glutReshapeFunc(reshapeCB);
	glutKeyboardFunc(keyboardCB);
	glutMouseFunc(mouseCB);
}

void exitGracefully() {
	logFile.close();
}

// haptic code begin
#ifdef HAPTIC
/*******************************************************************************
 Initialize the HDAPI.  This involves initing a device configuration, enabling
 forces, and scheduling a haptic thread callback for servicing the device.
*******************************************************************************/
void initHL()
{
    HDErrorInfo error;

    ghHD = hdInitDevice(HD_DEFAULT_DEVICE);
    if (HD_DEVICE_ERROR(error = hdGetError()))
    {
        hduPrintError(stderr, &error, "Failed to initialize haptic device");
        fprintf(stderr, "Press any key to exit");
        getchar();
        exit(-1);
    }
    
    ghHLRC = hlCreateContext(ghHD);
    hlMakeCurrent(ghHLRC);

    // Enable optimization of the viewing parameters when rendering
    // geometry for OpenHaptics.
    hlEnable(HL_HAPTIC_CAMERA_VIEW);

    // Generate id's for the teapot shape.
    gWallsId = hlGenShapes(1);
	gNetId = hlGenShapes(2);
	gBallId = hlGenShapes(3);

	for (int i = 0; i < 16; i++) {
		prevProxyTransform[i] = 0.0;
		proxytransform[i] = 0.0;
	}

	 // Setup event callbacks.
    hlAddEventCallback(HL_EVENT_TOUCH, gBallId, HL_CLIENT_THREAD, 
                       &touchShapeCallback, NULL);
    hlAddEventCallback(HL_EVENT_TOUCH, gNetId, HL_CLIENT_THREAD, 
                       &touchShapeCallback2, NULL);
	hlAddEventCallback(HL_EVENT_TOUCH, gWallsId, HL_CLIENT_THREAD, 
                       &touchShapeCallback2, NULL);


    hlTouchableFace(HL_FRONT); // define force feedback from front faces of teapot
}

/*******************************************************************************
 This handler is called when the application is exiting.  Deallocates any state 
 and cleans up.
*******************************************************************************/
void exitHandler()
{
    // Deallocate the sphere shape id we reserved in initHL.
    hlDeleteShapes(gWallsId, 1);
	hlDeleteShapes(gNetId, 2);
	hlDeleteShapes(gBallId, 3);

    // Free up the haptic rendering context.
    hlMakeCurrent(NULL);
    if (ghHLRC != NULL)
    {
        hlDeleteContext(ghHLRC);
    }

    // Free up the haptic device.
    if (ghHD != HD_INVALID_HANDLE)
    {
        hdDisableDevice(ghHD);
    }
}

/*******************************************************************************
 Use the current OpenGL viewing transforms to initialize a transform for the
 haptic device workspace so that it's properly mapped to world coordinates.
*******************************************************************************/
void updateWorkspace()
{
    GLdouble modelview[16];
    GLdouble projection[16];
    GLint viewport[4];

    glGetDoublev(GL_MODELVIEW_MATRIX, modelview);
    glGetDoublev(GL_PROJECTION_MATRIX, projection);
    glGetIntegerv(GL_VIEWPORT, viewport);

    hlMatrixMode(HL_TOUCHWORKSPACE);

	//hlPushMatrix();
    hlLoadIdentity();
	//hlScaled(2, 2, 2);
	//hlPopMatrix();

	//hlWorkspace(-30*1, 30*1, -10*1, 10*1, -30*1, 30*1);
	//hlOrtho(-30*1, 30*1, -10*1, 10*1, 0.1*1, -60*1);
	//hlWorkspace(-80, -80, -70, 80, 80, 20);
	hlWorkspace(-10.0, -10.0, -5.0,
		        10.0,  10.0, 5.0);
	hlOrtho(0.0, 1.0,
		    0.0, 1.0,
			0.0, 1.0);
    
    // Fit haptic workspace to view volume.
    hluFitWorkspace(projection);
	/*
	HLdouble minPoint[3], maxPoint[3];
	minPoint[0]=-10;
	minPoint[1]=-10;
	minPoint[2]=-10;
	maxPoint[0]=10;
	maxPoint[1]=10;
	maxPoint[2]=10;
	hluFitWorkspaceBox(modelview, minPoint, maxPoint);
	*/

    // Compute cursor scale.
    gCursorScale = hluScreenToModelScale(modelview, projection, viewport);
    gCursorScale *= CURSOR_SCALE_SIZE;
}
/*******************************************************************************
 The main routine for rendering scene haptics.
*******************************************************************************/
void drawSceneHaptics()
{    
    // Start haptic frame.  (Must do this before rendering any haptic shapes.)
    hlBeginFrame();

    // Set material properties for the shapes to be drawn.
    hlMaterialf(HL_FRONT_AND_BACK, HL_STIFFNESS, 0.7f);
    hlMaterialf(HL_FRONT_AND_BACK, HL_DAMPING, 0.1f);
    hlMaterialf(HL_FRONT_AND_BACK, HL_STATIC_FRICTION, 0.2f);
    hlMaterialf(HL_FRONT_AND_BACK, HL_DYNAMIC_FRICTION, 0.3f);

    // Start a new haptic shape.  Use the feedback buffer to capture OpenGL 
    // geometry for haptic rendering.
    hlBeginShape(HL_SHAPE_FEEDBACK_BUFFER, gWallsId);

    // Use OpenGL commands to create geometry.
    glPushMatrix();

    // tramsform camera
    //glTranslatef(0, 0, cameraDistance);
    //glRotatef(cameraAngleX, 1, 0, 0);   // pitch
    //glRotatef(cameraAngleY, 0, 1, 0);   // heading

    //if(dlUsed)
    //    glCallList(listId);     // render with display list
    //else
        drawWalls();           // render with vertex array, glDrawElements()

    glPopMatrix();


    // End the shape.
    hlEndShape();

	// Other shape
	hlBeginShape(HL_SHAPE_FEEDBACK_BUFFER, gNetId);
	glPushMatrix();
		drawNet();
	glPopMatrix();
	hlEndShape();

	// Other shape
	hlBeginShape(HL_SHAPE_FEEDBACK_BUFFER, gBallId);
	glPushMatrix();
		drawBall();
	glPopMatrix();
	hlEndShape();

    // End the haptic frame.
    hlEndFrame();

	// Call any event callbacks that have been triggered.
    hlCheckEvents();


}
/*******************************************************************************
 Draws a 3D cursor for the haptic device using the current local transform,
 the workspace to world transform and the screen coordinate scale.
*******************************************************************************/
void drawHapticCursor()
{
		//cout<<"redraw"<<endl;
		static const double kCursorRadius = 0.05;
		static const int kCursorTess = 15;
		//HLdouble proxytransform[16];

		GLUquadricObj *qobj = 0;

		glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
		glPushMatrix();

		if (!gCursorDisplayList)
		{
			//cout<<"draw cursor display list"<<endl;
			gCursorDisplayList = glGenLists(1);
			glNewList(gCursorDisplayList, GL_COMPILE);
			qobj = gluNewQuadric();
			glPushMatrix();
			glColor3f(1.,0,0); 
			gluSphere(qobj, kCursorRadius*2 * 4, kCursorTess, kCursorTess);
			glColor3f(0,0,1.);
			gluCylinder(qobj,kCursorRadius * 4,kCursorRadius * 4,10,100,5);
			glPopMatrix();
			gluDeleteQuadric(qobj);
			glEndList();
		}  

		// Apply the local position/rotation transform of the haptic device proxy.
		for (int i = 0; i < 16; i++) {
			prevProxyTransform[i] = proxytransform[i];
		}
		hlGetDoublev(HL_PROXY_TRANSFORM, proxytransform);
		glMultMatrixd(proxytransform);
		boolean changeCursorVnew = false;
		for (int i = 0; i < 3; i++) {
			cursorDold[i] = cursorDnew[i];
			cursorDnew[i] = proxytransform[i+12];
			if (cursorDnew[i] != cursorDold[i])
				changeCursorVnew = true;
		}
		if (changeCursorVnew) {
			for (int i = 0; i < 3; i++) {
				cursorVnew[i] = cursorDnew[i] - cursorDold[i];
			}
		}
        
		// Apply the local cursor scale factor.
		glScaled(gCursorScale, gCursorScale, gCursorScale);
	
		glEnable(GL_NORMALIZE);
		glEnable(GL_COLOR_MATERIAL);
		glColor3f(0.0, 0.5, 1.0);
		glCallList(gCursorDisplayList);
		glPopMatrix(); 
		glPopAttrib();
}
#endif
// haptic code finish

///////////////////////////////////////////////////////////////////////////////
// write 2d text using GLUT
// The projection matrix must be set to orthogonal before call this function.
///////////////////////////////////////////////////////////////////////////////
void drawString(const char *str, int x, int y, float color[4], void *font)
{
    glPushAttrib(GL_LIGHTING_BIT | GL_CURRENT_BIT); // lighting and color mask
    glDisable(GL_LIGHTING);     // need to disable lighting for proper text color

    glColor4fv(color);          // set text color
    glRasterPos2i(x, y);        // place text position

    // loop all characters in the string
    while(*str)
    {
        glutBitmapCharacter(font, *str);
        ++str;
    }

    glEnable(GL_LIGHTING);
    glPopAttrib();
}

void showInfo() {
	// backup current model-view matrix
    glPushMatrix();                     // save current modelview matrix
    glLoadIdentity();                   // reset modelview matrix

    // set to 2D orthogonal projection
    glMatrixMode(GL_PROJECTION);     // switch to projection matrix
    glPushMatrix();                  // save current projection matrix
    glLoadIdentity();                // reset projection matrix
    gluOrtho2D(0, 400, 0, 300);  // set to orthogonal projection

    float color[4] = {1, 1, 1, 1};

    stringstream ss;

	ss << "Trial #" << trialNumber << ". You can press the Esc key at any time to abort the program." << ends;
	drawString(ss.str().c_str(), 1, 1, color, font);
	ss.str("");

	ss << "You have scored " << goalsScored << " goals so far!" << ends;
	drawString(ss.str().c_str(), 1, 10, color, font);
	ss.str("");

	if (goalsScored >= 5) {
		ss << "Congratulations, you have scored all 5 goals!" << ends;
		drawString(ss.str().c_str(), 1, 25, color, font);
		ss.str("");
		exitGracefully();
		exit(0);
	}

    // restore projection matrix
    glPopMatrix();                   // restore to previous projection matrix

    // restore modelview matrix
    glMatrixMode(GL_MODELVIEW);      // switch to modelview matrix
    glPopMatrix();                   // restore to previous modelview matrix
}

void reshapeCB(int w, int h)
{
	glViewport(0, 0, (GLsizei)w, (GLsizei)h);
	/*
    // set viewport to be the entire window
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);

    // set perspective viewing frustum
    float aspectRatio = (float)w / h;
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    //glFrustum(-aspectRatio, aspectRatio, -1, 1, 1, 100);
    gluPerspective(60.0f, (float)(w)/h, 1.0f, 1000.0f); // FOV, AspectRatio, NearClip, FarClip

    // switch to modelview matrix in order to set scene
    glMatrixMode(GL_MODELVIEW);
	*/
	#ifdef HAPTIC
	updateWorkspace();
	#endif
}

void keyboardCB(unsigned char key, int x, int y)
{
    switch(key)
    {
    case 27: // ESCAPE
        //clearSharedMem();
		exitGracefully();
        exit(0);
        break;

    default:
        ;
    }
    glutPostRedisplay();
}

void mouseCB(int button, int state, int x, int y)
{
    mouseX = x;
    mouseY = y;

    if(button == GLUT_LEFT_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            mouseLeftDown = true;
        }
        else if(state == GLUT_UP)
            mouseLeftDown = false;
    }

    else if(button == GLUT_RIGHT_BUTTON)
    {
        if(state == GLUT_DOWN)
        {
            mouseRightDown = true;
        }
        else if(state == GLUT_UP)
            mouseRightDown = false;
    }
}

void idleCB()
{
	#ifdef HAPTIC
	HLerror error;

    while (HL_ERROR(error = hlGetError()))
    {
        fprintf(stderr, "HL Error: %s\n", error.errorCode);
        
        if (error.errorCode == HL_DEVICE_ERROR)
        {
            hduPrintError(stderr, &error.errorInfo,
                "Error during haptic rendering\n");
        }
    }
	#endif

    glutPostRedisplay();
}

int main(int argc, char **argv)
{
	srand (time(NULL));
	trialNumber = rand() % 1000000;
	char str[20];
	sprintf(str,"logs\\2DTrial#%i.csv\0", trialNumber);
	logFile.open(str);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(800*2, 450*2); // window size
	glutInitWindowPosition(100, 100); // window location
	int handle = glutCreateWindow("Goal Scorer"); // param is the title of window
	//glutFullScreen();
	glutDisplayFunc(display);
	init();
	#ifdef HAPTIC
	initHL(); //initialize haptic device
	#endif
	logFile << "2D\n" << trialNumber << "\n";
	const time_t rawTime = time(NULL);
	const tm curTime = *localtime(&rawTime);
	logFile << "Start," << curTime.tm_hour << ":" << curTime.tm_min << ":" << curTime.tm_sec << "\n";
	glutMainLoop();
	exitGracefully();
	return 0;             /* ANSI C requires main to return int. */
}
