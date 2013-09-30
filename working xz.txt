//***********************************************************************
/*	Written by Jaime Yu
 *	ID: 5428998
 *	Assignment 2 for COMP 471
 *	Fall 2008
 */
//***********************************************************************

#include <GL/glut.h>
#include <iostream>
#include <windows.h>
#include <math.h>

using namespace std;

//***********************************************************************
// constants 
// to do for future assignments

const float PI = 3.14159;


//***********************************************************************


//***********************************************************************
//GLOBAL VARIABLES

int rot = 0;				//amount of rotation applied to cube
bool viewHeadOn = true; 	//needed to figure out whether camera is on Z or ZY axis.

float add_me = 0;

float zoomZ = 1;			//field of view according to z location
float fovyZoom = 30;		//field of view according to y to x angle 

int angleX =0;
int angleY =90;
int angleZ =0;


int screen_w = 100;			//initial screen size is 400x400
int screen_h = 100;			

float loc_x = 0;			//camera locations
float loc_y = 0;
float loc_z = 5;

float lookAtX = 0;			//camera is looking at
float lookAtY = 0;
float lookAtZ = 0;

int upVectorY = 1;		//camera roll on Z axis
int upVectorX = 0;
int upVectorZ = 360;		//probably not used

bool rot_stop = true;		//is rotating?
bool rot_way = false;		//which way is it rotating?
bool AA = false;




//END GLOBAL VARIABLES
//***********************************************************************


//***********************************************************************
//FUNCTIONS

void rotate(int);		//rotate cube by int amounts

void displayCube();		//draw the cube
void displayAxis();		//draw the axis

void objects();			//call this to draw objects on screen
void screen();			//perspective and camera position
void resize(int, int);	//when the user changes window size, it keeps the proper aspect ratio
void idleRotate();		//allows the cube to rotate without user interaction

void keyboard(unsigned char,int,int);	//kb cmds
void mouseButton(int, int, int, int);	//ms cmds

void printInstructions();	//shows the instructions to the applicaton

//END FUNCTIONS
//***********************************************************************


//***********************************************************************
int main(int argc, char** argv)
{

	//system("cls");
//***********************************************************************
	//Introduction
	//
	//
	cout << "*****************************************************************" <<endl;
	cout << "Unit Cube application written by Jaime Yu, ID: 5428998. FALL 2008" ;
	cout << endl;
	cout << "Assignment 02 " <<endl;
	cout << endl;
	cout << "*****************************************************************" <<endl;

	printInstructions();
	
	//end introduction
//***********************************************************************

//***********************************************************************
	//initiations of glut
	glutInit(&argc, argv);
	
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );	//color | double buffer | ignores things you cannot see. 

//***********************************************************************


	
//***********************************************************************	

	//initialize windows
	glutInitWindowSize( 600, 600);
	glutInitWindowPosition( 100, 100);

	glutCreateWindow( "Jaime Yu, 5428998, COMP 471 Assignment 2" );	


//***********************************************************************



//***********************************************************************
	
	glutReshapeFunc(resize);
	glutMouseFunc(mouseButton);	//catch mouse clicks
	glutKeyboardFunc(keyboard);	//catch kb keys

	glutDisplayFunc(objects);	//draw objects
	glutIdleFunc(idleRotate);	//rotate objects when idle
	



//***********************************************************************
//***********************************************************************	

	

//***********************************************************************	

	
//***********************************************************************	

	
	glutMainLoop(); 		//main loop


//***********************************************************************
	//The application will never ever arrive here.
	//So don't code here!
	return 0;
}


//***********************************************************************



//***********************************************************************
void idleRotate()
{

	if ((rot_stop == false) && rot_way == true)
		rotate(1);
	else if ((rot_stop == false) && rot_way == false)
		rotate(-1);

//	glutSwapBuffers();
//	glutPostRedisplay();		//refresh screen



}

//***********************************************************************



//***********************************************************************

void rotate( int angle)
{
	rot += angle;		//  increment the rotation angle
	rot %=360;		//  rot mod 360 keeps it within the ranges 0 - 359
}

//***********************************************************************

//***********************************************************************

void resize(int w, int h)
{
	screen_w = w;	//i'm going for simplicity
	screen_h = h;	//just updates global variables

	cout << "Someone changed the size of the window! It is now " << w << "x" << h << "px." <<endl;





}

//***********************************************************************


//***********************************************************************

void screen()
{
	glEnable(GL_DEPTH_TEST);	//enable buffering, i have it set to double buffering

//	glViewport(0, 0, screen_w, screen_w);
	glViewport(0, 0, 600, 600);				//the cube with glperspective makes the cube the same size at all times

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

//gluPerspective( fovyZoom,(GLfloat)screen_w/(GLfloat)screen_h, 1,100 ); 	//makes sure the cube looks how we want it

	gluPerspective( fovyZoom, 1, 0.1,100 ); 

	//gluPerspective( fovyZoom,1.0,0.1,100 ); 	//
	glMatrixMode( GL_MODELVIEW );

	glClearColor( 0, 0, 0, 0 );	//bkg is now blk
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);  

	glLoadIdentity();		// clear modelview


	
	lookAtX = loc_x-cos( angleY*PI/180 );
	lookAtY = 0;
	lookAtZ = loc_z-sin( angleY*PI/180 );




	gluLookAt( loc_x, loc_y,loc_z , lookAtX, lookAtY, lookAtZ, 0, 1,0); //camera goes to 0,0,1.5 and looks at 0,0,0


	
	
	
	
	
/*	
	if (viewHeadOn == false)		//camera is looking directly at the cube on the Z axis
				//camera is in ZY axis looking down at the cube. you should see 2 faces. 
		gluLookAt(0, 2, 2, 0, 0, 0, 0, 1,0);// LOOK HERE
*/


}

//***********************************************************************


//***********************************************************************

void objects()
{

	screen();

//	gluLookAt(0, 0, 10, 0, 0, 0, 0, 1,0);

//	glTranslatef(0,0,10);


//		glRotatef( upVectorZ, cos(angleX),sin(angleX),tan(angleX));

glPushMatrix();	


	displayAxis();			//draw AXIS OF EVIL

glPopMatrix();


glPushMatrix();


	glRotatef(rot,0.5,-0.5,0.5);		//rotates the cube by rot aruond the axis of (0.5,-0.5, 0.5) and (-0.5, 0.5,-0.5).


	displayCube();			//draw unit cube



glPopMatrix();


glPushMatrix();
	glTranslatef(0,0,-10);

	displayCube();

glPopMatrix();


glPushMatrix();
	glTranslatef(0,0,10);

	displayCube();

glPopMatrix();



glPushMatrix();
	glTranslatef(10,10,0);

//	glTranslatef(0,0,loc_z);

	displayCube();

glPopMatrix();

glPushMatrix();
	glTranslatef(-10,10,0);

	displayCube();

glPopMatrix();

glPushMatrix();
	glTranslatef(10,-10,0);

	displayCube();

glPopMatrix();

glPushMatrix();
	glTranslatef(-10,-10,0);

	displayCube();

glPopMatrix();



glPushMatrix();
	glTranslatef(10,10,10);

	displayCube();

glPopMatrix();

glPushMatrix();
	glTranslatef(-10,10,10);

	displayCube();

glPopMatrix();

glPushMatrix();
	glTranslatef(10,-10,10);

	displayCube();

glPopMatrix();

glPushMatrix();
	glTranslatef(-10,-10,10);

	displayCube();

glPopMatrix();


	glLoadIdentity();




	glutSwapBuffers();		//buffer swap!
	glutPostRedisplay();

}


//***********************************************************************


//***********************************************************************
void displayAxis()
{


	glBegin(GL_LINES);
		// x axis
		glColor3f(1, 0, 0);	//red
		glVertex3f(0, 0, 0);
		glVertex3f(10, 0, 0);

		// y axis
		glColor3f(0, 1, 0);	//green
		glVertex3f(0, 0, 0);
		glVertex3f(0, 10, 0);

		// z axis
		glColor3f(0, 0, 1); //blue
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 10);

	glEnd();

}

//***********************************************************************

//***********************************************************************


void displayCube()
{

	
	//Build the square faces
	glBegin(GL_QUADS);

	/*	fun facts!
	 *	The cube is a unit cube, which has has length, width, depth of (1,1,1).
	 *	The exact center of the cube is at (0,0,0).
	 *	Hence all the 0.5 that you see below.
	 *	
	 *	Important note
	 *	THE CUBE DOES NOT CHANGE POSITION.
	 *	I HAVE NOT USED ANY TRANSFORM FUNCTIONS ON IT. 
	 *	ONLY THE CAMERA MOVES.
	 *	Technically speaking, you can animate the camera to rotate around the cube. 
	 */

	
			//front ****************************
			glColor3f( 0, 1, 0 ); //GREEN FRONT
			glVertex3f( -0.5, -0.5, 0.5);
			glVertex3f( 0.5, -0.5, 0.5);
			glVertex3f( 0.5, 0.5, 0.5);
			glVertex3f( -0.5, 0.5, 0.5);
			//front ****************************
			

			//back  ****************************
			glColor3f( 1, 0, 0 ); //RED BACK
			glVertex3f( -0.5, -0.5, -0.5);
			glVertex3f( 0.5, -0.5, -0.5);
			glVertex3f( 0.5, 0.5, -0.5);
			glVertex3f( -0.5, 0.5, -0.5);
			//back  ****************************

			//top   ****************************
			glColor3f( 1, 1, 0 ); //Yellow top
			glVertex3f( -0.5, 0.5, 0.5);
			glVertex3f( 0.5, 0.5, 0.5);
			glVertex3f( 0.5, 0.5, -0.5);
			glVertex3f( -0.5, 0.5, -0.5);
			//top   ****************************


			//bottom****************************
			glColor3f( 0.5, 0, 0.5 ); //PURPLE bottom
			glVertex3f( -0.5, -0.5, 0.5);
			glVertex3f( 0.5, -0.5, 0.5);
			glVertex3f( 0.5, -0.5, -0.5);
			glVertex3f( -0.5, -0.5, -0.5);
			//bottom****************************

			//right ****************************
			glColor3f( 0, 1, 1 );  //Fuscia? RIGHT
			glVertex3f( 0.5, -0.5, 0.5);
			glVertex3f( 0.5, -0.5, -0.5);
			glVertex3f( 0.5, 0.5, -0.5);
			glVertex3f( 0.5, 0.5, 0.5);
			//right ****************************

			//left  **************************** 
			glColor3f( 1, 1, 1 ); //WHITE LEFT
			glVertex3f( -0.5, -0.5, 0.5);
			glVertex3f( -0.5, -0.5, -0.5);
			glVertex3f( -0.5, 0.5, -0.5);
			glVertex3f( -0.5, 0.5, 0.5);
			//left  ****************************
			
			glEnd();

}

//***********************************************************************


//***********************************************************************
void keyboard(unsigned char key,int x,int y)
{

	switch(key)
	{
	case 27:	//the ESC key. 
		cout << "Exiting application!"<<endl;
		exit(1);
		break;
	case 'r':case 'R':	//stops rotation and resets the camera
		cout << "Reseting and stopping rotation!" <<endl;
		rot_stop = true;
		rot = 0;		//The cube resets its rotation and camera
		cout << "Reseting Zoom!" <<endl;
		fovyZoom = 45;
		cout << "Reseting Camera!" <<endl;
		loc_z = 1;
		loc_y = 1;
		loc_x = 1;

		lookAtX = 1;
		lookAtY = 1;
		lookAtZ = 1;

		upVectorX = 0;
		upVectorY = 0;
		upVectorZ = 360;

		zoomZ = 0;

		break;
	case 'z':case '-': //zoom into the cube
		cout << "Zooming Out. FOVY = " << fovyZoom <<endl;
		if (fovyZoom != 179)
			fovyZoom++;
		else
			cout << "You can't zoom out anymore without inverting the viewport" <<endl;
		break;
	case 'x':case '+':
		cout << "Zooming in. FOVY = " << fovyZoom <<endl;
		if (fovyZoom != 1)
			fovyZoom--;
		else
		{
			cout << "You can't zoom in anymore without inverting the viewport" <<endl;
			fovyZoom = 1;	 //making sure the viewport doesn't invert
		}
			break;
	case 'f':case 'F':

			zoomZ = 0.1;
	
			loc_x += zoomZ*cos( angleY*PI/180 );
			loc_y = 0;
			loc_z += zoomZ*sin( angleY*PI/180 );
/*
			lookAtX = loc_x+cos( angleY*PI/180 );
			lookAtY = 0;
			lookAtZ = loc_z+sin( angleY*PI/180 );
*/



		break;
	case 'b':case 'B':

			zoomZ = -0.1;
	
			loc_x += zoomZ*cos( angleY*PI/180 );
			loc_y = 0;
			loc_z += zoomZ*sin( angleY*PI/180 );	
		
/*
			lookAtX = loc_x+cos( angleY*PI/180 );
			lookAtY = 0;
			lookAtZ = loc_z+sin( angleY*PI/180 );
*/





		break;
	case 'h':case 'H': //show instructions 	
		system("cls"); //clear screen
		printInstructions();	
		break;
		/*
	case 'a':case 'A':
		if (AA == true)
			AA = false;
		else
			AA = true;
		if (AA == true)
			cout << "Anti-Aliasing ON" <<endl;
		else
			cout << "Anti-Aliasing OFF" <<endl;
		break;
		*/
	case 30: case 'a':
		

		angleY--;

/*
			lookAtX = loc_x+cos( angleY*PI/180 );
			lookAtY = 0;
			lookAtZ = loc_z+sin( angleY*PI/180 );
*/




		break;
	case 31: case 'd':
		
		angleY++;


/*
			lookAtX = loc_x+cos( angleY*PI/180 );
			lookAtY = 0;
			lookAtZ = loc_z+sin( angleY*PI/180 );
*/



		break;
	case 29: case 'w':
		//lookAtX--;
		//upVectorY--;
		//cout << "Camera is now looking at [" << lookAtX <<"," << lookAtY <<"," << lookAtZ << "]." <<endl;




		break;
	case 28: case 's':
		//lookAtX++;
		//upVectorY++;
		//cout << "Camera is now looking at [" << lookAtX <<"," << lookAtY <<"," << lookAtZ << "]." <<endl;



		break;

	case '[':

		/*
		if (upVectorX != -1)
			upVectorX -= 0.1;
		//if (upVectorY >=1);
		//	upVectorY += 0.1;

		if (upVectorX <= 0)
			upVectorY = 1;

	 */

//		if (upVectorX == 1)
			upVectorZ += 1;

//		glRotatef(upVectorX, 0,0,1);

		cout << "Roll on Z left" <<endl;
		break;
	case ']':
			upVectorZ -= 1;
			
//		glRotatef(upVectorX, 0,0,1);
		
		cout << "Roll on Z right" <<endl;
		break;
		
	default:
		cout << "Press 'h' for help. " <<endl;
		break;
	}

	
	
		upVectorX %= 360;
		upVectorY %= 360;
		upVectorZ %= 360;
		
//		angleX %= 360;
//		angleY %= 360;
//		angleZ %= 360;
	
	cout << angleX << endl;
	cout << angleY << endl;
	cout << angleZ << endl;
	cout << zoomZ << endl;



	//lookAtX = loc_x+cos( angleX*PI/180 );
	//lookAtY = 0;
	//lookAtZ = loc_z+sin( angleX*PI/180 );


	cout << "Camera at location: ["<< loc_x << ", " << loc_y <<", " << loc_z << "]" <<endl;
	cout << "Camera looking at location: ["<< lookAtX << ", " << lookAtY <<", " << lookAtZ << "]" <<endl;


	
	//	glutSwapBuffers();
//	glutPostRedisplay();		//refresh screen
}


//***********************************************************************



//***********************************************************************

void mouseButton(int button, int state, int x, int y) 
{
	if (state == GLUT_DOWN)		//a mouse button is pressed down!
	{
			if (button == GLUT_LEFT_BUTTON) //check if its the left button, right button don't do anything
				{	
				if (viewHeadOn == true)
					{
					cout << "Changing to 45 degree angle. Camera now located at [0," << loc_z << "," << loc_z << "]." <<endl;
					viewHeadOn = false;	//view on ZY axis
					}
					else
					{
					cout << "Changing to head on view. Camera now located at [0," << loc_z << "," << loc_z << "]."  <<endl;
					viewHeadOn = true;	//view only on Z axis
					}
				}
			
			else if (button == GLUT_RIGHT_BUTTON)
			{

				//the code below could possibly be compressed into a mathmatical equation with %
				if (rot_stop == true && rot_way == false)
				{
					cout << "Rotating!" <<endl;
					rot_stop = false;
					rot_way = true;
				}
				else if (rot_stop == true && rot_way == true)
				{
					cout << "Rotating!" <<endl;
					rot_stop = false;
					rot_way = false;
				}
				else 
				{
					cout << "Rotation stopped!" <<endl;
					rot_stop = true;
				}
			}
	}
//	glutSwapBuffers();
//	glutPostRedisplay();		//refresh screen

}

//***********************************************************************


//***********************************************************************
void printInstructions()
{
	cout << "Instructions: " <<endl;
	cout << "              -The escape key exits the application." <<endl;
	cout << "              -The r key resets the cube." <<endl;
	cout << "              -The + key zooms in." <<endl;
	cout << "              -The - key zooms out." <<endl;
	cout << "              -The f key moves the camera forward." <<endl;
	cout << "              -The b key moves the camera backwards." <<endl;
	cout << "              -The w key makes the camera the look up." <<endl;
	cout << "              -The s key makes the camera the look down." <<endl;
	cout << "              -The a key makes the camera look left." <<endl;
	cout << "              -The d key makes the camera look left." <<endl;
	cout << "              -The [ key makes the camera roll left." <<endl;
	cout << "              -The ] key makes the camera roll left." <<endl;
	cout << "              -The h key displays help." <<endl;
	cout << "              -The left mouse click relocates the camera to [0,loc_z,loc_z]." <<endl;
	cout << "              -The right mouse changes the rotation states. " <<endl;
	cout << endl;
	cout << "Cube color code:    | Axis color code:    |"<<endl;	
	cout << "Green  = Front      | Green  = Y axis     |"<<endl;
	cout << "Red    = Back       | Red    = X Axis     |"<<endl;
	cout << "Yellow = Top        | Blue   = Z Axis     |"<<endl;
	cout << "Purple = Bottom     |                     |"<<endl;
	cout << "Aqua   = Right      |                     |"<<endl;
	cout << "White  = Left       |                     |"<<endl;
	cout << "*******************************************************************" <<endl;
}

//***********************************************************************