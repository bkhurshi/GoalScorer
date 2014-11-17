
#include <GL/glut.h>

#define HAPTIC // comment this line to take the haptic off
   

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
    //hlLoadIdentity();
    
    // Fit haptic workspace to view volume.
    //hluFitWorkspace(projection);

	HLdouble minPoint[3], maxPoint[3];
	minPoint[0]=-10;
	minPoint[1]=-10;
	minPoint[2]=-10;
	maxPoint[0]=10;
	maxPoint[1]=10;
	maxPoint[2]=10;
hluFitWorkspaceBox(modelview, minPoint, maxPoint);

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
    glTranslatef(0, 0, cameraDistance);
    glRotatef(cameraAngleX, 1, 0, 0);   // pitch
    glRotatef(cameraAngleY, 0, 1, 0);   // heading

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
    static const double kCursorRadius = 0.5;
    static const double kCursorHeight = 1.5;
    static const int kCursorTess = 15;
    HLdouble proxyxform[16];

    GLUquadricObj *qobj = 0;

    glPushAttrib(GL_CURRENT_BIT | GL_ENABLE_BIT | GL_LIGHTING_BIT);
    glPushMatrix();

    if (!gCursorDisplayList)
    {
        gCursorDisplayList = glGenLists(1);
        glNewList(gCursorDisplayList, GL_COMPILE);
        qobj = gluNewQuadric();
               
        gluCylinder(qobj, 0.0, kCursorRadius, kCursorHeight,
                    kCursorTess, kCursorTess);
        glTranslated(0.0, 0.0, kCursorHeight);
        gluCylinder(qobj, kCursorRadius, 0.0, kCursorHeight / 5.0,
                    kCursorTess, kCursorTess);
    
        gluDeleteQuadric(qobj);
        glEndList();
    }
    
    // Get the proxy transform in world coordinates for haptic device.
    hlGetDoublev(HL_PROXY_TRANSFORM, proxyxform);
    glMultMatrixd(proxyxform);

    // Apply the local cursor scale factor.
    glScaled(gCursorScale, gCursorScale, gCursorScale);

    glEnable(GL_COLOR_MATERIAL);
    glColor3f(0.0, 0.5, 1.0);

    glCallList(gCursorDisplayList);

    glPopMatrix(); 
    glPopAttrib();
}
#endif
// haptic code finish

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
	#ifdef HAPTIC
	initHL(); //initialize haptic device
	#endif
	glutMainLoop();
	return 0;             /* ANSI C requires main to return int. */
}
