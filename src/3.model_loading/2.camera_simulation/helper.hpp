
#include <GL/glut.h>			// This is assuming that you have glut.h set in your include path.
//the view changing parameter
const GLfloat ViewAngleOffset=0.01;
//const GLfloat ViewAngleLROffset=10*ViewAngleOffset;
//const GLfloat ZoomAngleOffset=0.5;
//const GLfloat ZoomRatioOffset=0.5;
const GLfloat ZoomRadiusOffset=0.05;

//the view init parameter
const GLfloat ViewStartRadius=5;
const GLfloat ViewStartTheta=60;
const GLfloat ViewStartPhi=20;

//const GLfloat ViewStartRatio=1;

//model start state parameter
const GLfloat StartAxisAnglex=0;
const GLfloat StartAxisAngley=0;
const GLfloat StartAxisAnglez=0;

//the view volume init parameter
const GLfloat SIZE= 1000;			// To deal with the view volume
const GLfloat NEAR_Z= 0.001;
const GLfloat FAR_Z= 100.0;

//color advance offset
const GLint MaximumColorNumber=10;
const GLint ColorAdvanceStep=10;
const GLint ColorStartIndex=3;
const GLfloat ColorAdvanceOffset=0.09;
const GLfloat DefaultAxisLength=10;

//constant PI
const GLfloat PI= 3.14159265;		// An excessively abused used constant !!

//cockpit relative size
const GLfloat CockpitWidth=1.5;
const GLfloat CockpitHeight=1.0;
const GLfloat Front_Rear_Ratio=0.15;


//skid relative size
const GLfloat SkidSupportHeight=0.5;
const GLfloat SkidSupportDistance=2;
const GLfloat SkidWidth=10;
const GLfloat SkidRadius=0.1;
const GLfloat SkidJointOffset=0.8;
const GLfloat BoomLength=10;
const GLfloat TailRotorSupportRadius=0.3;
const GLfloat TailRotorSupportOffset=TailRotorSupportRadius*0.5;

//utility function for drawing models
void drawOval(GLfloat x, GLfloat y, GLfloat z);
void drawSphere(float fRadius, GLint slices, GLint rings);
void drawCylinder(float fTopR, float fBottomR, float fHeight);
void drawDisk(GLdouble innerRadius, GLdouble outerRadius, GLint slices, GLint rings,
	GLdouble startAngle, GLdouble sweepAngle);
void drawShearCylinder(GLfloat topRad, GLfloat bottomRad, GLfloat height, GLfloat angle);
void drawPatialTorus(GLfloat rCrossSection, GLfloat rAxial, GLfloat sweepAngle);

//small helper functions
GLfloat deg2rad(GLfloat degree);
void setColor();// a little fancy random color generator
void initColorArray();



//model helper functions for each module

void drawSkidSupport(const GLfloat SkidSupportDistance, const GLfloat SkidSupportHeight);


void divider_x(void(*pfunc)(), GLfloat start, GLfloat end, GLint num);
void divider_y(void(*pfunc)(), GLfloat down, GLfloat up, GLint num);
void drawRotor();
void drawHelicopter();

void drawScene();

//setup functions
void init (void);
void showMenu(void);
void showReferenceAxis(void);


/****************************************************************************************/
void showReferenceAxis(void)
						// Draw the reference axis
{
	if(bAxis)
	{
	 glPushAttrib(GL_ALL_ATTRIB_BITS);
						// Pushes attributes like current color to attribute stack.
	 glBegin(GL_LINES);
						// X axis red
      glColor3f(1, 0, 0);
      glVertex3f(0, 0, 0);
      glVertex3f(DefaultAxisLength, 0, 0);
						// Y axis green
      glColor3f(0, 1, 0);
      glVertex3f(0, 0, 0);
      glVertex3f(0, DefaultAxisLength, 0);
						// Z axis blue
      glColor3f(0, 0, 1);
      glVertex3f(0, 0, 0);
      glVertex3f(0, 0, DefaultAxisLength);
	  glEnd();
	  glPopAttrib();
						// // Pops attributes like current color from the attribute stack and sets as current.
	}
}


/****************************************************************************************/
void init (void)
						// Initializes the gl Graphics env and the program variables.
{
	glMatrixMode(GL_PROJECTION);
							// Set the current openGL matrix to GL_PROJECTION ie projection matrix.
	glLoadIdentity();
						// Load identitiy values in the above.
	if(ePrjn == PERSPECTIVE)
	{
		gluPerspective(fovy, (GLfloat)W/(GLfloat)H, NEAR_Z, FAR_Z);
							// This sets the view volume to be a Perspective one.
	}
	else
	{
		fViewVol = 50.0;
		glOrtho(-fViewVol, fViewVol, -fViewVol, fViewVol, NEAR_Z, FAR_Z);
							// This sets the view volume to be a Orthographic one.
	}


	glMatrixMode(GL_MODELVIEW);	
							// Change the current matrix mode to Model-View matrix.
	glLoadIdentity();
							

	glClearColor (0.5, 0.5, 0.5, 0.0);
						// set the background color to black.

	glShadeModel(GL_SMOOTH);
	glEnable(GL_DEPTH_TEST);
	bAxis = true;
	
	bWire = false;
	glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
	showMenu();

	initColorArray();
}


/****************************************************************************************/

/******************************************************************************/

void drawScene()
{
	int i;
	const GLfloat GroundHeight=-4;
	const GLfloat TrafficSignLength=1, TrafficSignWidth=0.5;
	const GLfloat Sign_Y_Offset=0;
	const GLfloat View_Size=100;
	const GLfloat BallonSize=0.5;

	glPushMatrix();
		glColor4f(0.0, 0.8, 0.0,1);
		glLineWidth(2);
		for (i=0; i<100; i++)
		{	
			glBegin(GL_LINES);
			glVertex3f(-View_Size, GroundHeight,
				-View_Size+i*TrafficSignLength*2);
			glVertex3f(View_Size, GroundHeight, 
				-View_Size+i*TrafficSignLength*2);
			
			glVertex3f(-View_Size+i*TrafficSignLength*2, GroundHeight, -View_Size);
			glVertex3f(-View_Size+i*TrafficSignLength*2, GroundHeight, View_Size);
			glEnd();		
		}

		glColor4f(1,0,0.7,1);
		glPushMatrix();
			glTranslatef(-20, 2, 6);
			for (i=0; i<10; i++)
			{
				glTranslatef(TrafficSignLength*8, 0, 0);  
		
				glScalef(1, 1.3, 1);
				glColor4f(1,0.9-i*0.1,0.1+i*0.1,1);
				glutSolidSphere(BallonSize, 30, 30);
				glBegin(GL_LINES);
					glVertex3f(0, -BallonSize/1.3,0);
					glVertex3f(0, -BallonSize*2,0);
				glEnd();

			}
		glPopMatrix();
		glPushMatrix();
			glTranslatef(-20, 2, -6);
			for (i=0; i<10; i++)
			{
				glTranslatef(TrafficSignLength*8, 0, 0);  
				glScalef(1, 1.3, 1);
				glColor4f(1,0.1+i*0.1,0.9-i*0.1,1);
				glutSolidSphere(BallonSize, 30, 30);
				glBegin(GL_LINES);
					glVertex3f(0, -BallonSize/1.3,0);
					glVertex3f(0, -BallonSize*2,0);
				glEnd();
			}
		glPopMatrix();
			glTranslatef(-10, 2, 0);
			glColor4f(0.1, 0.6, 0.9, 0.9);
			glScalef(1, 1.3, 1);
			glutSolidSphere(BallonSize, 30, 30);
			glBegin(GL_LINES);
				glVertex3f(0, -BallonSize/1.3,0);
				glVertex3f(0, -BallonSize*2,0);
			glEnd();			

	glPopMatrix();

}

/******************************************************************************/
/****************************************************************************************/
void showMenu(void)
				// Self Explanatory.
{
	printf("\n\n\n\n COMP471 ASSIGNMENT3 HELICOPTER. ");
	printf("\n---------------------------------------------------------------");
	printf("\n Operations: ");
	printf("\n Use w/W to toggle between wireframe and shaded model.");
	printf("\n Use o/O to toggle between ortho and perspective views.");
	printf("\n Use a/A to show/hide the reference axis <x = RED, y = GREEN, z = BLUE>.");
	printf("\n Use the f/F,b/B keys to approach and leave model.");
	printf("\n Use arrow up/down keys for camera rotations up/down about origin.");
	printf("\n Use arrow left/right keys for camera rotations left/right about origin.");

	printf("\n Use s/S keys to start/stop spinning of rotor of helicopter");
	printf("\n Use 1 keys to toggle first-person camera view.");
	printf("\n Use 3 keys to toggle third-person camera view.");

	printf("\n Use p/P keys to pitch helicopter and first-person view camera.");
	
	printf("\n Use y/Y keys to yaw helicopter and first-person view camera.");
	printf("\n Use r/R keys to roll helicopter and first-person view camera.");
	
	printf("\n Use t/T keys to move helicopter backward/forward along x-axis \
		and first-person and third-person view camera.");
	
	printf("\n Use h/H keys to move helicopter left/right along z-axis \
		and first-person and third-person view camera.");

	printf("\n Use i/I key to move third-person view camera up/down about \
		cockpit of helicopter\n");
	
	printf("\n Use k/K key to rotate third-person view camera up/down about \
		cockpit of helicopter\n");

	printf("\n Use j/J key to move third-person view camera left/right about \
		cockpit of helicopter\n");

	printf("\n Use l/L key to rotate third-person view camera left/right about \
		cockpit of helicopter\n");

	printf("\n Use d/D to restore all default states.");
	printf("\n Use m/M to see the menu again.");
	
	printf("\n Use escape key to exit.\n");

}

int print()

{
        glutInit(&argc, argv);	
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitWindowSize(SIZE, SIZE);
	glutInitWindowPosition(10, 10);
	glutCreateWindow("A primitive model of a helicopter ! ~nick");
	init();	
}




