//***********************************************************************
/*	Written by Jaime Yu
 *	ID: 5428998
 *  jaime.yu@gmail.com
 *  source+exec can be found for a limited amount of time at www.travvik.com/proj/game_prototype.7z
 *
 *	Game Prototype for COMP 471
 *	Fall 2008
 *
 *	I'm sorry the code is all over the place
 *	I was a one man coder and speed was the priority 
 *	over clean code. 
 * 
 *	If you want to insert custom cars, you replace the code for
 *	drawJeep, drawCar, drawVan, drawTruck functions.
 *	note that the current functions themselves contain errors and are prone
 *	to unexpected results if you play with them. Other team members wrote them and will be deleted 
 *	after the final submission as i don't have the time to rewrite them.
 *
 *
 *
 *	To load bitmaps into memory, I use the CBMPLoader libary to load
 *	convert bitmaps to raw and then store into memory. 
 *	I then use its pointers to draw textures. 
 *	
 *  As I said, speed was the priority, not clean code
 *  and it was easier than rewriting NeHe's code at
 *  http://nehe.gamedev.net/data/lessons/lesson.asp?lesson=06
 *
 *  The actual texture code I use are derived from the Red Book. 
 * 
 *  even though I used a 3rd party library, I ommited its 
 *  house cleaning functions b/c I didn't want to use the library
 *  any more than I had to.
 *
 *  for mp3s, i used the http://www.inet.hr/~zcindori/mp3class/
 *  as i said speed over clean code
 *
 *  the benefit of using this library is that it runs every instance 
 *	as a seperate thread (process even maybe?) so you can overlap music or sound fx. 
 *  I didn't overlap any songs or snd fx 
 *  (do you know how annoying it is to hear loud noises during debuggin?) but if you want, 
 *  you can set it up to do it anytime. 
 *
 *
 *  The theory to the game engine is based on my previous 
 *  experience designing maze solving algorithms for
 *  COEN 390. There are some similarities, but nothing was reused. 
 *
 *  As such, there is an AI in the code but he is just a random number
 *  generator. A friend suggested I implement the robot's maze algorithm 
 *  into this game to provide 1 player action. Maybe later...
 *
 *  Known problems:
 *  Synchronization errors -- there are two major threads
                           -- its unnoticeable b/c the game runs 
							  faster than the eye can pick it up
						   -- and theres a very very low chance of it occuring
							  since major variables dont change between cycle updates. 
 *  Animation code -- this is due to the synchronization error
					  because i interpolate data during the display update cycle
					  which fights when it matches the movement calculation cycle
				   -- code has been commented out to avoid the problem
 *  various state variables problems which are theoritically possible, 
 *  view angles    -- which aren't really big issues
 *  Opacity layers in textures wreck havok and become garbled.
                   -- its cause i didn't put the opengl function for alpha layers
				      so it gets confused and outputs garbage. So don't add
					  textures with opacity layers!!
 *
 *
 *  fun facts, u can replace int with GLint and float with GLfloat and have the code work 100%
 *  fun facts, THERES A LOT OF GARBAGE CODE HERE. Its based on previous assignments and its too 
               dangerous to remove it. if it ain't broke, don't touch it. 
 *
 *
 *
 *
 *	BIG HINT!!! Use the search function (ctrl+f) to find where the functions are located
 *				saved me a lot of scrolling time. your mouse will thank you. 
 */
//***********************************************************************

#include <GL/glut.h>	//opengl toolkit
#include <iostream>		//dos outputs
#include <windows.h>	//solely used for the clear screen function in dos.
						//funny thing is, i'm sure glut.h already calls it
#include <math.h>		//MATH!

#include <stdio.h>		//these are standard libraries
#include <stdlib.h>		//i don't remember if i use them
						//probably safer to leave them here

#include <time.h>		//i create timers with this
#include <list>			//i forget why this is here. i think its for the lists of vertices

#include "CBMPLoader.h"	//bmp loader

//mp3 headers
#include "mp3_src/wmp3.h"	//mp3 player


using namespace std;

//***********************************************************************
// constants 
// to do for future assignments
// how come i never removed the above statement? 

const float PI = 3.14159;		//to the 5th decimal:D btw: needless to say, it causes precision errors 
const int screen_orig = 600;	//600x600 window		

const int FIELD_LENGTH = 52;	//u can use this to make the field bigger
const int FIELD_WIDTH = 52;		//why 52? for some inane programming error
								//only 50 of the 52 spaces are seen in the arrays

const int P1PosXStart = 40;		//obvious
const int P1PosYStart = 40;
const int P2PosXStart = 10;
const int P2PosYStart = 10;


const int NORTH = 0;			//usually i use #define but its not official C++ code.
const int EAST = 1;				//does the same thing so i'm not complaining
const int SOUTH = 2;
const int WEST = 3;

//const int WAIT = 250; //ticks
const int WAIT = 250;	//ticks -- change this to change speed of game
						//remove the const and u can actually increase game speed dynamically
						//
						//yea i know, i wrote it twice.

//***********************************************************************



//empty space
//no seriously, does anyone read this stuff?



//***********************************************************************
//GLOBAL VARIABLES

int rot = 0;				//amount of rotation applied to cube
							//its now usd as the master rotate angle
bool viewAngleYZ = true; 	//needed to figure out whether camera is on Z or ZY axis.

float add_me = 0;

float zoomZ = 1;			//field of view according to z location
float fovyZoom = 30;		//field of view according to y to x angle 

int angleX = 0;
int angleY = -90;
int angleZ = 0;


int screen_w = 300;			//garbage code left over
int screen_h = 300;			


float loc_x = 0;			//camera locations
float loc_y = 0;
float loc_z = 5;

float old_loc_x, old_loc_y, old_loc_z = 0;



float lookAtX = 0;			//camera is looking at
float lookAtY = 0;
float lookAtZ = 0;

float upVectorX = 0;		//camera roll on Z axis
float upVectorY = 0;
float upVectorZ = 0;		//probably not used

bool rot_stop = false;		//is rotating?
bool rot_way = false;		//which way is it rotating?
bool AA = false;

//game code starts here

bool GODMODE = false;		//obvious
bool AI = false;			//i believe the AI code is commented out
							//but i wouldn't set this to true just in case. 

CBMPLoader Texture;			//texture 


WMp3* themeSong = new WMp3; // create instance of WMp3 
WMp3* gameSong = new WMp3;
WMp3* dontGiveUp = new WMp3;

bool isSongPlaying = false; //i wrote this forgetting that the mp3s run 
							//in a seperate thread. 
							//totally unneccessary

//junky code for the 3rd person view
	int lookx1, looky1 = 0;
	int lookx2, looky2 = 0;
	int lookposx1, lookposy1 = 0;
	int lookposx2, lookposy2 = 0;


bool caravansPos[FIELD_WIDTH+1][FIELD_LENGTH+1];	//no honestly, the code acts weird
bool gameOver = false;		//obvious
bool P1Collision = false;
bool P2Collision = false;

float lastP1PosX;
float lastP1PosY;
float lastP2PosX;
float lastP2PosY;

//animation between clock ticks
//disabled
float frame;			
bool updatingPos = false;	//this was supposed to be a semaphore lock
							//yea, it didn't work

//P1 pos
int P1PosX = 0;
int P1PosY = 0;
int P1Direction = 1;//n

//P2 pos
int P2PosX = 50;
int P2PosY = 50;
int P2Direction = 3;//s


//time

clock_t ticks;	//master clock


//time constraints -- this makes sure the game runs smoothly on any PC
clock_t startTimer;
clock_t endTimer;

//FPS counter
int frameCount =1;
clock_t endCounter = clock() + 1000;	//1 sec = 1000msec

//states bools
bool menustate = true;
bool PressStart = true;
bool P1ChooseCar = false;
bool P2ChooseCar = false;
bool ReadyScreen = false;

bool firstpersonview = false;
bool p2view = false;
bool p1view = false;
bool viewnorm = true;


//car selection states
bool P1jeep = false;
bool p1car = true;
bool p1truck = false;
bool p1van = false;

bool P2jeep = false;
bool p2car = true;
bool p2truck = false;
bool p2van = false;

int p1choice = 0;	//i choose you, Pikachu!

int p2choice = 0;	//CHARAZOR FTW!

//textures
CBMPLoader front;
CBMPLoader back;
CBMPLoader rightt;	//ok the two 't' is beccause right and left are already used.
CBMPLoader leftt;	//i don't really care to find out where
CBMPLoader top;
CBMPLoader bottom;
CBMPLoader logo;
CBMPLoader background;
CBMPLoader instructions;

CBMPLoader p2win;
CBMPLoader p1win;
CBMPLoader losers;
GLuint texturelist[13];		//a smart thing to do is replace 13 with a variable since
							//i have to use gentextlist or something another later


//lights
bool islightOn = false;		//do i really have to?


bool pause = false;			//seriously

GLfloat mat_emission[] = { 0.0, 0.3, 0.3, 0.6 };	//uhhh not used, its based off the redbook and i was playing around
GLfloat mat_transparent[] = { 1.0, 0.5, 0.5, 0.6 };	//for the transparency effect

//END GLOBAL VARIABLES
//***********************************************************************


//***********************************************************************
//FUNCTIONS

void rotate(int);		//rotate cube by int amounts

void displayCube();		//draw the cube
void displayAxis();		//draw the axis
void displaySky();		//not used


void objects();			//call this to draw objects on screen
void screen();			//perspective and camera position
void resize(int, int);	//when the user changes window size, it keeps the proper aspect ratio
void idleRotate();		//allows the cube to rotate without user interaction

void keyboard(unsigned char,int,int);	//kb cmds
void mouseButton(int, int, int, int);	//ms cmds

void printInstructions();	//shows the instructions to the applicaton

void textureLoad();		//does nothing


void drawCaravans();	//does nothing i think too

void material();		//sets materials to object

void moveFWD();			//moves the players by 1 unit fwd

void timeSetup();		//creates the timer
void testCollision();	//i'm guessing it tests collision? ;)
void fillCaravanEmpty();	//fills the visited array with false bools

void loadMusic();		//seriously...

void idleMoveCars();	//i'm not sure whats in this function anymore
void idleFPS();			//fps calculator
void idleCamera();		//not used... does it even exist?
void gameAlgorithm();	//this is fundamental to the game
						//all the movement calculations are done here

void text(char*);		//draws text to screen. found in one of the tutor's examples 
						//it was beautifully simple and i couldn't resist using it. 
void textLargE(char*);	//same with BIGGER font. 
						

void kbArrows(int, int, int);	//captures arrow keys
void showMenu();				//menu

//not my code and has problems
//code needs to be replaced with debugged versions
//or it can be replaced with a unit cube and it works fine
void drawWheel();
void drawJeep();
void drawVan();
void drawTruck();
void drawCar();
//back to my code now


void loadTextures();	//loads the textures into memory
void setupLights();		//positions lights and their colors
void drawDrivenTile();	//draws a tile thats been visited
void drawEmptyTile();	//draws a title that no one has visited. 
void displayPlayer1Car();	//draws a vehicle to screen depending on their choice
void displayPlayer2Car();

void displayCubeTexture();	//i reused my cube code to build a quick and dirty room for the game

void drawLogo();			//wrongly named- this is the splash screen for the menu
void drawInstructions();	//menu's instruction page
void drawBKG();				//draws the menu's background

void endGame();				//Rick Astley lives here

void splitScreen();			//i finally got split screen working
							//this controls the viewports and cameras
							//and then uses objects() to draw to screen

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
	cout << "Game Prototype application written by Jaime Yu, ID: 5428998. FALL 2008" ;
	cout << endl;
	cout << "jaime.yu@gmail.com " <<endl;
	cout << endl;
	cout << "*****************************************************************" <<endl;

	printInstructions();
	
	//end introduction
//***********************************************************************

//***********************************************************************
	//initiations of glut
	glutInit(&argc, argv);
	
	glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH );	//color | double buffer | ignores things you cannot see. 

	glEnable (GL_DEPTH_TEST);

//***********************************************************************


	
//***********************************************************************	

	//initialize windows
	glutInitWindowSize( screen_orig, screen_orig);
	glutInitWindowPosition( 100, 100);

	glutCreateWindow( "Jaime Yu, 5428998, COMP 471 GAME PROTOTYPE" );	


//***********************************************************************

//	setupLights();
	fillCaravanEmpty();
	textureLoad();
	loadMusic();
	loadTextures();

//	themeSong->Play();



//lights
	//old light code used for testing
/*
   GLfloat ambient[] = { 0.0, 0.0, 0.0, 1.0 };
   GLfloat diffuse[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat position[] = { 0.0, 3.0, 2.0, 0.0 };
   GLfloat lmodel_ambient[] = { 0.4, 0.4, 0.4, 1.0 };
   GLfloat local_view[] = { 0.0 };

   glClearColor(0.0, 0.1, 0.1, 0.0);
   glEnable(GL_DEPTH_TEST);
   glShadeModel(GL_SMOOTH);

   glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
   glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
   glLightfv(GL_LIGHT0, GL_POSITION, position);
   glLightModelfv(GL_LIGHT_MODEL_AMBIENT, lmodel_ambient);
   glLightModelfv(GL_LIGHT_MODEL_LOCAL_VIEWER, local_view);
*/
//   glEnable(GL_LIGHTING);
//   glEnable(GL_LIGHT0);



//***********************************************************************
	

	glutReshapeFunc(resize);
	glutMouseFunc(mouseButton);	//catch mouse clicks
	glutKeyboardFunc(keyboard);	//catch kb keys
	glutSpecialFunc(kbArrows);	//draw objects

	//glutDisplayFunc(objects);	//draw objects
	glutDisplayFunc(splitScreen);	//replaces objects
	
	glutIdleFunc(gameAlgorithm);	//game algorithms 



//***********************************************************************
//***********************************************************************	

	

//***********************************************************************	

	
//***********************************************************************	

	
	glutMainLoop(); 		//main loop


//***********************************************************************
	//The application will never ever arrive here.
	//So don't code here!
	
	cout << "MAGIC!" <<endl;

	return 0;
}


//***********************************************************************

void gameAlgorithm()
{

	idleMoveCars();	//oohhhh its my AI
	idleFPS();
//	idleCamera();
	idleRotate();

	//lights
	//setupLights();
}

void idleMoveCars()
{

	//The AI lives here!
	//oh look, how cute, hes playing against himself. 

	ticks = clock();

	if (endTimer <= ticks)
	{
		moveFWD(); //increment

		endTimer = clock() + WAIT;	
		
		//AI
		if (AI == true)
		{
			P1Direction = rand()%4;
			P2Direction = rand()%4;
		}
	}

}

void idleFPS()
{
	//frame counter

	if (ticks > endCounter)
	{
		cout << "FPS: " << frameCount <<endl;
		frameCount = 1;
		endCounter = ticks + 1000;
	}

}

void idleCamera()
{
	//not even used
	//this came from my cube code

	//camera code


		//*********************************************************
	
	// the following code moves the point where the camera looks at
	//
	// loc is where the camera is located
	// the addition part is the relative position(to the center of the camear) 
	// of what the camera is looking at. 
	//
	// the reason you see so many PI/180 is because the trig functions are in radians! NOT deg!


	lookAtX = loc_x + ( cos( angleY*PI/180 ) *cos( angleX*PI/180 )  );

	lookAtY = loc_y + ( sin( angleX*PI/180 )  ); 
	
	lookAtZ = loc_z + ( sin( angleY*PI/180 ) *cos( angleX*PI/180 )  );
	
	
	//*********************************************************


	
	if (viewAngleYZ == false)		//camera is looking @ 45deg angle to center on Y-Z axis//the X-Y-Z 45 deg view is cooler though
		{

			loc_x = 0;
			loc_y = 5;
			loc_z = 5;

			lookAtX = 0;
			lookAtY = 0;
			lookAtZ = 0;

		}
//camera roll
}
//***********************************************************************
void idleRotate()
{

	if ((rot_stop == false) && rot_way == true)
		rotate(1);
	else if ((rot_stop == false) && rot_way == false)
		rotate(-1);

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

//	glViewport(0, 0, screen_w, screen_h);
	glViewport(0, 0, screen_orig, screen_orig);				//the cube with glperspective makes the cube the same size at all times

	glMatrixMode( GL_PROJECTION );
	glLoadIdentity();

//	float anglefov =0;
//	float def = screen_orig/screen_w;

//	anglefov = screen_orig / def ;


	
	//	gluPerspective( anglefov, 1, 1,100 ); 	//makes sure the cube looks how we want it

//figures out the right zoom level. 
if ( (menustate == false ) && (firstpersonview == true))
	fovyZoom = 75;
else
	fovyZoom = 30;

	gluPerspective( fovyZoom, 1, 0.1,200 ); 

	//gluPerspective( fovyZoom,1.0,0.1,100 ); 	//
	glMatrixMode( GL_MODELVIEW );

	glClearColor( 0, 0, 0, 0 );	//bkg is now blk
	glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_ACCUM_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);  




	glLoadIdentity();		// clear modelview


//********************************************************************************************

	

//lookatfunction

//glRotatef(angleZ, 0,0,1);

//	gluLookAt( loc_x, loc_y,loc_z , lookAtX, lookAtY, lookAtZ, 0, 1,0); //camera goes to 0,0,1.5 and looks at 0,0,0


//this controls which camera to use depending on game mode

if (menustate == true)
{
	gluLookAt( 25, 100 , 25, 25, 0, 25, 1.0, 0.0, 0.0); 
	objects();

}
else if (firstpersonview == true) //(if (menustate ==true)
{

			if (P1Direction == 0)
			{
				lookposx1 = P1PosX ;
				lookposy1 = P1PosY -5 ;
				lookx1 = P1PosX;
				looky1 = P1PosY + 10;
			}
			else if (P1Direction == 1)
			{
				lookposx1 = P1PosX - 5;
				lookposy1 = P1PosY ;
				lookx1 = P1PosX + 10;
				looky1 = P1PosY ;
			}
			else if (P1Direction == 2)
			{
				lookposx1 = P1PosX;
				lookposy1 = P1PosY + 5 ;
				lookx1 = P1PosX;
				looky1 = P1PosY - 10;
			}
			else if (P1Direction == 3)
			{
				lookposx1 = P1PosX + 5;
				lookposy1 = P1PosY;
				lookx1 = P1PosX - 10 ;
				looky1 = P1PosY ;
			}

//p2			
			if (P2Direction == 0)
			{
				lookposx2 = P2PosX ;
				lookposy2 = P2PosY -5 ;
				lookx2 = P2PosX;
				looky2 = P2PosY + 10;
			}
			else if (P2Direction == 1)
			{
				lookposx2 = P2PosX - 5;
				lookposy2 = P2PosY ;
				lookx2 = P2PosX + 10;
				looky2 = P2PosY ;
			}
			else if (P2Direction == 2)
			{
				lookposx2 = P2PosX;
				lookposy2 = P2PosY + 5 ;
				lookx2 = P2PosX;
				looky2 = P2PosY - 10;
			}
			else if (P2Direction == 3)
			{
				lookposx2 = P2PosX + 5;
				lookposy2 = P2PosY;
				lookx2 = P2PosX - 10 ;
				looky2 = P2PosY ;
			}			
	/*******************/
	//the split screen code
	glEnable(GL_SCISSOR_TEST);

	//player 1 top half
    glViewport(0, 0, 600, 600);        
    glScissor(0, 300, 600, 300);
	glLoadIdentity();	
	gluLookAt( lookposx1, 1, lookposy1, P1PosX, 0, P1PosY, 0.0, 1.0, 0.0);
    //gluLookAt( P1PosX, 5, P1PosY, lookx1, 0, looky1, 0.0, 1.0, 0.0);
	//gluLookAt( 1, 1, 1, 0, 0, 0, 0.0, 1.0, 0.0);
	objects();

	//drawJeep();
	//player 2 bottom half	
    glViewport(0, 0, 600, 300);        
    glScissor(100, 0, 500, 300);

	        
	glLoadIdentity();
	gluLookAt( lookposx2, 1, lookposy2, P2PosX, 0, P2PosY, 0.0, 1.0, 0.0);

  //  glDisable(GL_SCISSOR_TEST);

	objects();

	//drawJeep();

	//the overhead gameMAP
   glViewport(0, 0, 100, 100);        
    glScissor(0, 0, 100, 100);

	        
	glLoadIdentity();
	//gluLookAt( lookposx2, 1, lookposy2, P2PosX, 0, P2PosX, 0.0, 1.0, 0.0);

	gluLookAt( 25, 60 , 25, 25, 0, 25, 0.0, 0.0, 1.0);
	objects();
    glDisable(GL_SCISSOR_TEST);

	/************************/

	//gluLookAt( P1PosX, 2, P1PosY, lookx, 0, looky, 0.0, 1.0, 0.0);
}
else if (p1view == true) ////if (menustate ==true)
{
	gluLookAt( P1PosX, 10 , P1PosY, P1PosX, 0, P1PosY, 1.0, 0.0, 0.0);
	objects();
}
else if (p2view == true) ////if (menustate ==true)
{
	//gluLookAt( P2PosX, 10 , P2PosY, P2PosX, 10, P2PosY, 1.0, 0.0, 0.0);
	gluLookAt( P2PosX, 2 , P2PosY, P1PosX, 0, P1PosY, 0.0, 1.0, 0.0);
		objects();

}
else if (viewnorm == true)
{
	gluLookAt( 25, 110 , 25, 25, 0, 25, 0.0, 0.0, 1.0);
	objects();
	
}


//scissor view test

/*	
    glEnable(GL_SCISSOR_TEST);

    glViewport(0, 0, 300, 300);            \
    glScissor(0, 0, 300, 300);
    projection(300, 300, 1);               \
    //glCallList(torus_list);
	displayCube();
    
    glViewport(0, 300, 300, 300);        \
    glScissor(0, 300, 300, 300);
    projection(300, 300, 0);               \
    //glCallList(torus_list);
	//displayCube();

    glDisable(GL_SCISSOR_TEST);

  //if the code looks familiar, its cause its from the tutor's examples.
  //Thanks!
*/


}

//***********************************************************************



//***********************************************************************
void material()
{
	//i believe this came from redbook examples. 

	 GLfloat no_mat[] = { 0.0, 0.0, 0.0, 1.0 };
   GLfloat mat_ambient[] = { 0.7, 0.7, 0.7, 1.0 };
   GLfloat mat_ambient_color[] = { 0.8, 0.8, 0.2, 1.0 };
   GLfloat mat_diffuse[] = { 0.7, 0.7, 0.7, 1.0 };
   GLfloat mat_specular[] = { 1.0, 1.0, 1.0, 1.0 };
   GLfloat no_shininess[] = { 0.0 };
   GLfloat low_shininess[] = { 5.0 };
   GLfloat high_shininess[] = { 100.0 };
   GLfloat mat_emission[] = {0.3, 0.2, 0.2, 0.0};

   glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

/*  draw sphere in first row, first column
 *  diffuse reflection only; no ambient or specular  
 */
   glPushMatrix();

//   glMaterialfv(GL_FRONT, GL_AMBIENT, mat_ambient_color);
//   glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_diffuse);
//  glMaterialfv(GL_FRONT, GL_SPECULAR, no_mat);
   glMaterialfv(GL_FRONT, GL_SHININESS, no_shininess);
//  glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);

   glEnable(GL_COLOR_MATERIAL);
	glColorMaterial(GL_FRONT, GL_DIFFUSE);
}





void drawCaravans()	//draws the floor
{
//				if ((i != P1PosX) && (j != P1PosY))
//					if ((i != P2PosX) && (j != P2PosY))

	for (int i = 0; i<FIELD_WIDTH-1; i++)
		for (int j = 0; j<FIELD_LENGTH-1; j++)
			if (caravansPos[i][j] == true)
				{
		
				glPushMatrix();

					glTranslatef( i, 0 ,j);
					//glutSolidSphere(0.5,5,5);
					drawEmptyTile();

				glPopMatrix();
				}
			else //(caravansPos[i][j] == false)
				{
		
				glPushMatrix();

					glTranslatef( i, 0 ,j);
					//glutSolidSphere(0.5,5,5);
					drawDrivenTile();

				glPopMatrix();
				}

			
		
}



void splitScreen()
{
	setupLights();	//lights
	screen();
	//objects();

	frameCount++;

	glutSwapBuffers();		//buffer swap!
	glutPostRedisplay();
}


void objects()	//draw objects to screen
{

	//screen();

	if(menustate == true)
		showMenu();
	else
	{
		//draw environment
		glPushMatrix();
			glTranslatef(25,0,25);

			glScalef(FIELD_LENGTH,20,FIELD_LENGTH);
			displayCubeTexture();
		glPopMatrix();
		
		//GAMEOVER CODE DISPLAY ON SCREEN
		if (gameOver == true)
			//exit(0);
		{	
			firstpersonview = false;
			p2view = false;
			p1view = false;
			viewnorm = true;

			endGame();
			
			if (P2Collision == true)
			{
				glColor3f(0, 0, 0);
				glRasterPos2i(30,10);
				text("Player 1 Wins!");
				
			}
			else if (P1Collision == true)
			{
				glColor3f(0, 0, 0);
				glRasterPos2i(30,10);
				text("Player 2 Wins!");
			}
			else
			{
				glColor3f(0, 0, 0);
				glRasterPos2i(30,10);
				text("You are both Losers!");
			}
			if(isSongPlaying == false)  //absolutely useless
			{
				dontGiveUp->Play();
				isSongPlaying = true;	//absolutely useless
			}
			gameSong->Stop();
			//cout << "WINNER!" <<endl;
			//WAIT = 100000000;
		}
		//END
		
		drawCaravans();
/*		
		glPushMatrix();	
		
		
		displayAxis();			//draw AXIS OF EVIL
		
		glPopMatrix();
		
		
		glPushMatrix();
		
		
		glRotatef(rot,0.5,-0.5,0.5);		//rotates the cube by rot aruond the axis of (0.5,-0.5, 0.5) and (-0.5, 0.5,-0.5).
		
		
		displayCube();			//draw unit cube
		
		
		
		glPopMatrix();
*/
		
		//P1
		glPushMatrix();

		//the following code is how i interpolated the current position and the next
		//creates a smooth animation 
		//but it also has problems when the gamealgorithm updates the data

		/*
		int test = clock();
		int diff = endTimer-test;
		if (endTimer <0)
		diff = 0;
		
		  cout <<diff<<endl;
		  
			GLfloat test1 = P1PosX;
			GLfloat test2 = P1PosY;
			
			  test1 += (diff/1000);
			  test2 += (diff/1000);
			  glTranslatef(  test1, 0.0 , test2);
			  
				
				  if (ticks < endTimer)
				  {
				  if (P1Direction == WEST)
				  lastPosX -= 10;//(1/WAIT);
				  //else
				  //	{}
				  
					}
					//else
					//lastPosX = P1PosX;
					
					  glTranslatef(  lastPosX, 0.0 , P1PosY);
		*/
		
		/*
		float outputP1, outputP2;
		
		//500 ms
		
		ticks = clock();//have to put it into floats!
		
		if (updatingPos != true)
		{
			if (ticks < endTimer - 50)
			{
				frame++;
				outputP1 = lastP1PosX + (P1PosX - lastP1PosX) * ((float)ticks / (float)endTimer);
				outputP2 = lastP1PosY + (P1PosY - lastP1PosY) * ((float)ticks / (float)endTimer);
				//cout << "outputP1: " << outputP1 << "	outputP2: " <<outputP1 <<endl;
			}
			else
			{
				outputP1 = P1PosX ;
				outputP2 = P2PosY ;
			}
		}
		
		*/


		//P1
		//glTranslatef( (float)outputP1, 0.0 , (float)outputP2 );
		glTranslatef( P1PosX, 0 ,P1PosY);
		//displayCube();			//draw unit cube
		displayPlayer1Car();		
		
		glPopMatrix();
		
		//P2
		glPushMatrix();
		
		glTranslatef( P2PosX, 0 ,P2PosY);
//		glRotatef(180, 0,0,1);
//		displayCube();			//draw unit cube
		displayPlayer2Car();
		
		glPopMatrix();
		
		//floor
		/*
		old floor code
		glPushMatrix();
		
		
		glTranslatef(25,-50,25);
		
		glScalef(100,50,100);
		glRotatef(-90,1,0,0);
		
		displayCube();
		
		
		glPopMatrix();
		*/
		



}




	//frame counter


}


//***********************************************************************




//***********************************************************************
void displayAxis()
{


	glBegin(GL_LINES);
		// x axis
		glColor3f(1, 0, 0);	//red
		glVertex3f(0, 0, 0);
		glVertex3f(10000, 0, 0);

		// y axis
		glColor3f(0, 1, 0);	//green
		glVertex3f(0, 0, 0);
		glVertex3f(0, 10000, 0);

		// z axis
		glColor3f(0, 0, 1); //blue
		glVertex3f(0, 0, 0);
		glVertex3f(0, 0, 10000);

	glEnd();

}

//***********************************************************************/

void textureLoad()
{
		
//no one lives here
//originally the texture code belongs here
//but i forgot
// and then created a new one later

}

void displaySky()
{

	//junk trying to build a sky box
	//comes from assignment 2. 

	/*
  glEnable(GL_TEXTURE_2D);
	Texture.LoadBitmap("tex.bmp");

		// Generate the texture id.
	glGenTextures(1, &Texture.ID);
   
   // Here we bind the texture and set up the filtering.  Linear
   // gives up a nice look while NEAREST the quality is not as good.
   glBindTexture(GL_TEXTURE_2D, Texture.ID);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

   // Load the image data into the OpenGL texture that is currently
   // binded.
	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, Texture.imageWidth,
	                  Texture.imageHeight, GL_RGB, GL_UNSIGNED_BYTE,
	                  Texture.image);





glShadeModel(GL_SMOOTH);

glBindTexture(GL_TEXTURE_2D, Texture.ID);

   glBegin(GL_QUADS);
      glTexCoord2f(0.0f, 0.0f); glVertex3f(-0.5f, -0.5f, 0.5f);
      glTexCoord2f(1.0f, 0.0f); glVertex3f(0.5f, -0.5f, 0.5f);
      glTexCoord2f(1.0f, 1.0f); glVertex3f(0.5f, 0.5f, 0.5f);
      glTexCoord2f(0.0f, 1.0f); glVertex3f(-0.5f, 0.5f, 0.5f);

   glEnd();

			//displayCube();
			//glutSolidSphere(10, 10, 10);    //(GLdouble radius, GLint slices, GLint stacks);

			glDisable(GL_TEXTURE_2D);


			Texture.FreeImage();

  
	*/
	  }



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
	//oh this is complicated
	//good luck

	switch(key)
	{
	case 27:	//the ESC key. 
		cout << "Exiting application!"<<endl;
		exit(1);
		break;
	case 'p': case 'P':
		
		if (pause == true)
		{
			cout << "PAUSE off!" <<endl;
			pause = false;
			//WAIT = 250;
		}
			else
		{
			cout << "PAUSE on!" <<endl;
			pause = true;
			//WAIT = 100000000;
		}
		break;
	case 'r':case 'R':	//stops rotation and resets the camera
		cout << "Reseting and stopping rotation!" <<endl;
		//rot_stop = true;
		//rot = 0;		//The cube resets its rotation and camera
		fovyZoom = 45;
		 zoomZ = 1;			//field of view according to z location
		 fovyZoom = 30;		//field of view according to y to x angle 

		 angleX = 0;
		 angleY = -90;
		 angleZ = 0;


		 //screen_w = 300;			//initial screen size is 400x400
		 //screen_h = 300;			


		 loc_x = 0;			//camera locations
		 loc_y = 0;
		 loc_z = 5;

		 lookAtX = 0;			//camera is looking at
		 lookAtY = 0;
		 lookAtZ = 0;
		
		 viewAngleYZ = true;

		 
		  caravansPos[FIELD_WIDTH+1][FIELD_LENGTH+1];
		  gameOver = false;
		  P1Collision = false;
		  P2Collision = false;

		  //P1 pos
		   P1PosX = P1PosXStart;
		   P1PosY = P1PosYStart;
		   P1Direction = 0;//n
		  
		  //P2 pos
		   P2PosX = P2PosXStart;
		   P2PosY = P2PosYStart;
		   P2Direction = 2;//s
		  
		   menustate = true;
		   PressStart = true;
		   P1ChooseCar = false;
		   P2ChooseCar = false;
		   ReadyScreen = false;
		  
		  //car selection
		   P1jeep = false;
		   p1car = true;
		   p1truck = false;
		   p1van = false;
		  
		   P2jeep = false;
		   p2car = true;
		   p2truck = false;
		   p2van = false;
		  
		   p1choice = 0;
		  
		   p2choice = 0;
		   themeSong->Stop();
		   dontGiveUp->Stop();
		   gameSong->Stop();
		   isSongPlaying = false;
		   pause = false;
		
		   firstpersonview = false;
			p2view = false;
			p1view = false;
			viewnorm = true;

			fillCaravanEmpty();
		//	WAIT = 250;
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
	
			loc_x += zoomZ*cos( angleY*PI/180 ) *cos( angleX*PI/180 );

			loc_y += zoomZ*sin( angleX*PI/180 );

			loc_z += zoomZ*sin( angleY*PI/180 )*cos( angleX*PI/180 );



		break;
	case 'b':case 'B':

			zoomZ = -0.1;
	
			loc_x += zoomZ*cos( angleY*PI/180 )*cos( angleX*PI/180 );

			loc_y += zoomZ*sin( angleX*PI/180 );

			loc_z += zoomZ*sin( angleY*PI/180 )*cos( angleX*PI/180 );	
		





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
		if (firstpersonview == true)
		{	
			if (P1Direction <= 0)
				P1Direction = 3;
			else
				P1Direction--;
			
		}
		else if (P1Direction != WEST)

			P1Direction = EAST;




		break;
	case 31: case 'd':
	
		if (firstpersonview == true)
		{	
			//P1Direction++;
			if (P1Direction == 3)
				P1Direction = 0;
			else
				P1Direction++;
			
		}
		else if (P1Direction != EAST)
			P1Direction = WEST;



		break;

//up down	
	case 'g': case 'G':
		if (GODMODE == true)
		{
			GODMODE = false;
			cout << "GODMODE IS OFF" <<endl;
		}
		else if (GODMODE == false)
		{
			GODMODE = true;
			cout << "GODMODE IS ON" <<endl;
		}
		
		break;
	case 29: case 'w':
		if (firstpersonview == true)
		{	
		}
		else if (P1Direction != SOUTH)
			P1Direction = NORTH;



		break;
	case 28: case 's':
		if (firstpersonview == true)
		{
		}
		else if (P1Direction != NORTH)
			P1Direction = SOUTH;
		
		break;

	case '[':


			angleZ++;


		break;
	case ']':
	
			angleZ--;


		break;		

	case '1':
		if (P1Direction == 0)
			P1Direction = 3;
		else
			P1Direction--;
			
		
		break;
	case '2':
		if (P1Direction == 3)
			P1Direction = 0;
		else 
			P1Direction++;
			
	
	break;
	case 'm': case 'M':
		//themeSong->Play(); // start playing
		themeSong->Stop();
		gameSong->Stop();
		break;
	case 'l': case 'L':
		
		if (islightOn == false)
			islightOn = true;
		else
			islightOn = false;
		
		if (islightOn == true)
		{	
			glEnable(GL_LIGHT1);								// Enable Light One
			glEnable(GL_LIGHTING);
		}
		else
		{
			glDisable(GL_LIGHT1);								// Enable Light One
			glDisable(GL_LIGHTING);			
		}

		break;

	case '3':
		AI = false;
		break;

	case 'v': case 'V':
	
	if(menustate == false)
	{
		if ( firstpersonview == true)
		{
			firstpersonview = false;
			p2view = true;
			p1view = false;
			viewnorm = false;
		}
		else if (p2view == true)
		{
			firstpersonview = false;
			p2view = false;
			p1view = true;
			viewnorm = false;
		}
		else if ( p1view == true)
		{
			firstpersonview = false;
			p2view = false;
			p1view = false;
			viewnorm = true;

		}
		else if (viewnorm == true)
		{
			firstpersonview = true;
			p2view = false;
			p1view = false;
			viewnorm = false;
		}
	}

	default:
		cout << "Press 'h' for help. " <<endl;
		break;
	}


}


//***********************************************************************



//***********************************************************************

void mouseButton(int button, int state, int x, int y) 
{

	//left here
	//

	if (state == GLUT_DOWN)		//a mouse button is pressed down!
	{
			if (button == GLUT_LEFT_BUTTON) //check if its the left button, right button don't do anything
				{	
				if (P2Direction == 0)
					P2Direction = 3;
				else
					P2Direction--;
				}
			
			else if (button == GLUT_RIGHT_BUTTON)
			{

				if (P2Direction == 3)
					P2Direction = 0;

				else
					P2Direction++;
			}


	}

}

//***********************************************************************


//***********************************************************************
void printInstructions()
{
	cout << "Instructions: " <<endl;
	cout << "l toggles lights." <<endl;
	cout << "r resets the game." <<endl;
	cout << "m turns off the music." <<endl;
	cout << "p toggles the pause." <<endl;
	cout << "v toggles the view." <<endl;
	cout << "g toggles the GODMODE." <<endl;
	cout << "*******************************************************************" <<endl;
}

//***********************************************************************
void loadMusic()
{

	//ahh isn't life easier
   	
	if( !themeSong->OpenMp3File("tigre.mp3",2000) ) // open mp3 file, wave output buffer 2000 ms
	{	//FAILED
            delete themeSong;
    }
   	if( !gameSong->OpenMp3File("chromeo.mp3",2000) ) // open mp3 file, wave output buffer 2000 ms
	{	//FAILED
            delete gameSong;
    }
	   	if( !dontGiveUp->OpenMp3File("rick.mp3",2000) ) // open mp3 file, wave output buffer 2000 ms
	{	//FAILED
            delete gameSong;
    }

}

void fillCaravanEmpty()
{
	for (int i = 0; i<FIELD_WIDTH; i++)
		for (int j = 0; j<FIELD_LENGTH; j++)
		{
			caravansPos[i][j] = false;	//largely inefficient, prob easier to just make it once and reference it
		}
}

void moveFWD()
{
	//moves the car in the right direction

if ((menustate == false) && (gameOver == false) && (pause == false) )
{
	caravansPos[P1PosX][P1PosY] = true;
	caravansPos[P2PosX][P2PosY] = true;

	updatingPos = true;
	lastP1PosX = P1PosX;
	lastP1PosY = P1PosY;

	lastP2PosX = P2PosX;
	lastP2PosY = P2PosY;
	updatingPos = false;

	//p1
	if ( ( P1Direction == NORTH) && (P1PosY < FIELD_LENGTH-2) )
		P1PosY++;
	else if ( ( P1Direction == SOUTH) && (P1PosY > 0) )
		P1PosY--;
	else if ( ( P1Direction == EAST) && (P1PosX < FIELD_WIDTH-2) )
		P1PosX++;
	else if ( ( P1Direction == WEST) && (P1PosX >0) )
		P1PosX--;

	
		//p2
	if ( ( P2Direction == NORTH) && (P2PosY < FIELD_LENGTH-2) )
		P2PosY++;
	else if ( ( P2Direction == SOUTH) && (P2PosY > 0) )
		P2PosY--;
	else if ( ( P2Direction == EAST) && (P2PosX < FIELD_WIDTH-2) )
		P2PosX++;
	else if ( ( P2Direction == WEST) && (P2PosX > 0) )
		P2PosX--;



//!!!!!!!!! COLLISION TEST
	testCollision();

	}
//cout << "P1x " <<P1PosX << "P1Y " << P1PosY <<"dir: " << P1Direction <<endl;
//cout << "P2x " <<P2PosX << "P2Y " << P2PosY <<"dir: " << P2Direction <<endl;




/*	
	if ( (P1PosX == P2PosX) && ( P1PosY==P2PosY ) )
		gameOver = true;

	if (gameOver == true)
		exit(0);
*/
}

void timeSetup()
{

ticks = clock();

//cout <<"TICKS: " << ticks <<endl;
//cout <<"SECONDS: " << ticks/CLOCKS_PER_SEC <<endl;
//cout <<"TICKS PER SECOND" << CLOCKS_PER_SEC  <<endl;

}

void testCollision()
{
	
	if (GODMODE == false)
	{
		if ( (P1PosX == P2PosX) && ( P1PosY==P2PosY ) )
			gameOver = true;
		else if ( caravansPos[P1PosX][P1PosY] == true)
		{
			gameOver = true;
			P1Collision = true;
		}
		else if ( caravansPos[P2PosX][P2PosY] == true)
		{
			gameOver = true;
			P2Collision = true;
		}
		
		
		
		
		//if ( (GODMODE == false) && (gameOver == true) )
		if (gameOver == true)
		{
			glRasterPos2i(5, 5);
			text("WINNER!");
			cout << "WINNER!" <<endl;
			//WAIT = 100000000;
			//fillCaravanEmpty();
		}
	}

}



//i really don't like the code below, its... 
//messy?

//***************************************************************** RAYS code
void drawWheel()
{

	GLfloat wheelsV [][3]=
	{

		{0.00,  0.10, 1},
		{0.02,  0.09, 1},
		{0.06,  0.06, 1},
		{0.09,  0.02, 1},
		{0.10,  0.00, 1},

		{0.09,  -0.02, 1},
		{0.06,  -0.06, 1},
		{0.02,  -0.09, 1},
		{0.00,  -0.10, 1},
		
		{-0.02,  -0.09, 1},
		{-0.06,  -0.06, 1},
		{-0.09,  -0.02, 1},
		{-0.10,  -0.00, 1},
		
		{-0.09,  0.02, 1},
		{-0.06,  0.06, 1},
		{-0.02,  0.09, 1},
		{-0.00,  0.10, 1},
		
		{0.00,  0.10, 0.97},
		{0.02,  0.09, 0.97},
		{0.06,  0.06, 0.97},
		{0.09,  0.02, 0.97},
		{0.10,  0.00, 0.97},

		{0.09,  -0.02, 0.97},
		{0.06,  -0.06, 0.97},
		{0.02,  -0.09, 0.97},
		{0.00,  -0.10, 0.97},
		
		{-0.02,  -0.09, 0.97},
		{-0.06,  -0.06, 0.97},
		{-0.09,  -0.02, 0.97},
		{-0.10,  -0.00, 0.97},
		
		{-0.09,  0.02, 0.97},
		{-0.06,  0.06, 0.97},
		{-0.02,  0.09, 0.97},
		{-0.00,  0.10, 0.97},
						
	};

	glBegin(GL_QUADS);

		glColor3f(0.6, 0.6, 0.8);
		glVertex3fv(wheelsV[0]);
		glVertex3fv(wheelsV[1]);
		glVertex3fv(wheelsV[18]);
		glVertex3fv(wheelsV[17]);

		glVertex3fv(wheelsV[1]);
		glVertex3fv(wheelsV[2]);
		glVertex3fv(wheelsV[19]);
		glVertex3fv(wheelsV[18]);

		glVertex3fv(wheelsV[2]);
		glVertex3fv(wheelsV[3]);
		glVertex3fv(wheelsV[20]);
		glVertex3fv(wheelsV[19]);
		
		glVertex3fv(wheelsV[3]);
		glVertex3fv(wheelsV[4]);
		glVertex3fv(wheelsV[21]);
		glVertex3fv(wheelsV[20]);

		glVertex3fv(wheelsV[4]);
		glVertex3fv(wheelsV[5]);
		glVertex3fv(wheelsV[22]);
		glVertex3fv(wheelsV[21]);

		glVertex3fv(wheelsV[5]);
		glVertex3fv(wheelsV[6]);
		glVertex3fv(wheelsV[23]);
		glVertex3fv(wheelsV[22]);

		glVertex3fv(wheelsV[6]);
		glVertex3fv(wheelsV[7]);
		glVertex3fv(wheelsV[24]);
		glVertex3fv(wheelsV[23]);

		glVertex3fv(wheelsV[7]);
		glVertex3fv(wheelsV[8]);
		glVertex3fv(wheelsV[25]);
		glVertex3fv(wheelsV[24]);
			
		glVertex3fv(wheelsV[8]);
		glVertex3fv(wheelsV[9]);
		glVertex3fv(wheelsV[26]);
		glVertex3fv(wheelsV[25]);
		
		glVertex3fv(wheelsV[9]);
		glVertex3fv(wheelsV[10]);
		glVertex3fv(wheelsV[27]);
		glVertex3fv(wheelsV[26]);

		glVertex3fv(wheelsV[10]);
		glVertex3fv(wheelsV[11]);
		glVertex3fv(wheelsV[28]);
		glVertex3fv(wheelsV[27]);

		glVertex3fv(wheelsV[11]);
		glVertex3fv(wheelsV[12]);
		glVertex3fv(wheelsV[29]);
		glVertex3fv(wheelsV[28]);

		glVertex3fv(wheelsV[12]);
		glVertex3fv(wheelsV[13]);
		glVertex3fv(wheelsV[30]);
		glVertex3fv(wheelsV[29]);

		glVertex3fv(wheelsV[13]);
		glVertex3fv(wheelsV[14]);
		glVertex3fv(wheelsV[31]);
		glVertex3fv(wheelsV[30]);

		glVertex3fv(wheelsV[14]);
		glVertex3fv(wheelsV[15]);
		glVertex3fv(wheelsV[32]);
		glVertex3fv(wheelsV[31]);
			
		glVertex3fv(wheelsV[15]);
		glVertex3fv(wheelsV[16]);
		glVertex3fv(wheelsV[33]);
		glVertex3fv(wheelsV[32]);
	glEnd();

	glBegin(GL_POLYGON);
		glColor3f(0.6, 0.6, 0.6);
		glVertex3fv(wheelsV[0]);
		glVertex3fv(wheelsV[1]);
		glVertex3fv(wheelsV[2]);
		glVertex3fv(wheelsV[3]);
		glVertex3fv(wheelsV[4]);
		glVertex3fv(wheelsV[5]);
		glVertex3fv(wheelsV[6]);
		glVertex3fv(wheelsV[7]);
		glVertex3fv(wheelsV[8]);
		glVertex3fv(wheelsV[9]);
		glVertex3fv(wheelsV[10]);
		glVertex3fv(wheelsV[11]);
		glVertex3fv(wheelsV[12]);
		glVertex3fv(wheelsV[13]);
		glVertex3fv(wheelsV[14]);
		glVertex3fv(wheelsV[15]);
		glVertex3fv(wheelsV[16]);

	glEnd();

	glBegin(GL_POLYGON);
		glColor3f(0.6, 0.6, 0.6);
		glVertex3fv(wheelsV[17]);
		glVertex3fv(wheelsV[18]);
		glVertex3fv(wheelsV[19]);
		glVertex3fv(wheelsV[20]);
		glVertex3fv(wheelsV[21]);
		glVertex3fv(wheelsV[22]);
		glVertex3fv(wheelsV[23]);
		glVertex3fv(wheelsV[24]);
		glVertex3fv(wheelsV[25]);
		glVertex3fv(wheelsV[26]);
		glVertex3fv(wheelsV[27]);
		glVertex3fv(wheelsV[28]);
		glVertex3fv(wheelsV[29]);
		glVertex3fv(wheelsV[30]);
		glVertex3fv(wheelsV[31]);
		glVertex3fv(wheelsV[32]);
		glVertex3fv(wheelsV[33]);

	glEnd();
}
void drawJeep()
{
glPushMatrix();
	// The eight points of the truck body
	GLfloat jeepBodyV [][3]=
	{
		{-0.3,  0.2,  0.4},		// Vertex[0]
		{ 0.3,  0.2,  0.4},		// Vertex[1]
		{-0.3,  0.1,  0.45},	// Vertex[2]
		{ 0.3,  0.1,  0.45},	// Vertex[3]
		{-0.3,  0.2, 0.25},		// Vertex[4]
		{ 0.3,  0.2, 0.25},		// Vertex[5]
		{-0.3,  0.3, 0.2},		// Vertex[6]
		{ 0.3,  0.3, 0.2},		// Vertex[7]
		{-0.3,	0.3, 0.0},		// Vertex[8]  
		{ 0.3,	0.3, 0.0},		// Vertex[9]
		{-0.3,  0.2, 0.0},		// Vertex[10]
		{ 0.3,  0.2, 0.0},		// Vertex[11]
		{-0.3,  0.2, -0.4},		// Vertex[12]
		{ 0.3,  0.2, -0.4},		// Vertex[13]
		{-0.3,  0.1, -0.45},	// Vertex[14]
		{ 0.3,  0.1, -0.45},	// Vertex[15]
		{-0.3,  0.0, -0.4},		// Vertex[16]
		{ 0.3,  0.0, -0.4},		// Vertex[17]
		{-0.3,  0.0,  0.4},		// Vertex[18]
		{ 0.3,  0.0,  0.4},		// Vertex[19]

		{ -0.3,  0.2,  -0.23},		// Vertex[20]
		{ -0.3,  0.3,  -0.23},		// Vertex[21]
		{ 0.3,   0.3,  -0.23},		// Vertex[22]
		{ 0.3,   0.2,  -0.23},		// Vertex[23]
		
		{ -0.3,  0.2,  -0.2},		// Vertex[24]
		{ -0.3,  0.3,  -0.2},		// Vertex[25]
		{ 0.3,   0.3,  -0.2},		// Vertex[26]
		{ 0.3,   0.2,  -0.2},		// Vertex[27]
		
		{ -0.28,  0.2,  -0.23},		// Vertex[28]
		{ -0.28,  0.29,  -0.23},	// Vertex[29]
		{ 0.28,   0.29,  -0.23},	// Vertex[30]
		{ 0.28,   0.2,  -0.23},		// Vertex[31]
		
		{ -0.28,  0.2,  -0.2},		// Vertex[32]
		{ -0.28,  0.29,  -0.2},		// Vertex[33]
		{ 0.28,   0.29,  -0.2},		// Vertex[34]
		{ 0.28,   0.2,  -0.2},		// Vertex[35]
		
	};

	glBegin(GL_QUADS);

		glColor3f(0, 0.3, 0);
		glVertex3fv(jeepBodyV[0]);
		glVertex3fv(jeepBodyV[1]);
		glVertex3fv(jeepBodyV[3]);
		glVertex3fv(jeepBodyV[2]);

		glColor3f(0.0, 0.6, 0.4);
		glVertex3fv(jeepBodyV[4]);
		glVertex3fv(jeepBodyV[5]);
		glVertex3fv(jeepBodyV[1]);
		glVertex3fv(jeepBodyV[0]);

		glColor3f(1, 1, 1);
		glVertex3fv(jeepBodyV[6]);
		glVertex3fv(jeepBodyV[7]);
		glVertex3fv(jeepBodyV[5]);
		glVertex3fv(jeepBodyV[4]);

		glColor3f(0.0, 0.2, 0.2);
		glVertex3fv(jeepBodyV[8]);
		glVertex3fv(jeepBodyV[9]);
		glVertex3fv(jeepBodyV[7]);
		glVertex3fv(jeepBodyV[6]);

		glColor3f(0.0, 0.2, 0.0);
		glVertex3fv(jeepBodyV[10]);
		glVertex3fv(jeepBodyV[11]);
		glVertex3fv(jeepBodyV[9]);
		glVertex3fv(jeepBodyV[8]);

		glColor3f(0.0, 0.6, 0.4);
		glVertex3fv(jeepBodyV[12]);
		glVertex3fv(jeepBodyV[13]);
		glVertex3fv(jeepBodyV[11]);
		glVertex3fv(jeepBodyV[10]);

		glVertex3fv(jeepBodyV[14]);
		glVertex3fv(jeepBodyV[15]);
		glVertex3fv(jeepBodyV[13]);
		glVertex3fv(jeepBodyV[12]);

		glVertex3fv(jeepBodyV[17]);
		glVertex3fv(jeepBodyV[16]);
		glVertex3fv(jeepBodyV[14]);
		glVertex3fv(jeepBodyV[15]);
			
		glVertex3fv(jeepBodyV[19]);
		glVertex3fv(jeepBodyV[18]);
		glVertex3fv(jeepBodyV[16]);
		glVertex3fv(jeepBodyV[17]);
		
		glVertex3fv(jeepBodyV[19]);
		glVertex3fv(jeepBodyV[18]);
		glVertex3fv(jeepBodyV[2]);
		glVertex3fv(jeepBodyV[3]);
	glEnd();

	glBegin(GL_POLYGON);
		glColor3f(0.0, 0.6, 0.1);
		glVertex3fv(jeepBodyV[3]);
		glVertex3fv(jeepBodyV[1]);
		glVertex3fv(jeepBodyV[5]);
		glVertex3fv(jeepBodyV[7]);
		glVertex3fv(jeepBodyV[9]);
		glVertex3fv(jeepBodyV[11]);
		glVertex3fv(jeepBodyV[13]);
		glVertex3fv(jeepBodyV[15]);
		glVertex3fv(jeepBodyV[17]);
		glVertex3fv(jeepBodyV[19]);
	glEnd();

	glBegin(GL_POLYGON);
		glColor3f(0.0, 0.6, 0.1);
		glVertex3fv(jeepBodyV[2]);
		glVertex3fv(jeepBodyV[0]);
		glVertex3fv(jeepBodyV[4]);
		glVertex3fv(jeepBodyV[6]);
		glVertex3fv(jeepBodyV[8]);
		glVertex3fv(jeepBodyV[10]);
		glVertex3fv(jeepBodyV[12]);
		glVertex3fv(jeepBodyV[14]);
		glVertex3fv(jeepBodyV[16]);
		glVertex3fv(jeepBodyV[18]);
	glEnd();

	glBegin(GL_QUADS);
		glColor3f(0.0, 0.4, 0.0);
		glVertex3fv(jeepBodyV[20]);
		glVertex3fv(jeepBodyV[21]);
		glVertex3fv(jeepBodyV[25]);
		glVertex3fv(jeepBodyV[24]);

		glVertex3fv(jeepBodyV[21]);
		glVertex3fv(jeepBodyV[22]);
		glVertex3fv(jeepBodyV[26]);
		glVertex3fv(jeepBodyV[25]);

		glVertex3fv(jeepBodyV[22]);
		glVertex3fv(jeepBodyV[26]);
		glVertex3fv(jeepBodyV[27]);
		glVertex3fv(jeepBodyV[23]);

		glVertex3fv(jeepBodyV[29]);
		glVertex3fv(jeepBodyV[33]);
		glVertex3fv(jeepBodyV[32]);
		glVertex3fv(jeepBodyV[28]);
		
		glVertex3fv(jeepBodyV[30]);
		glVertex3fv(jeepBodyV[34]);
		glVertex3fv(jeepBodyV[33]);
		glVertex3fv(jeepBodyV[29]);

		glVertex3fv(jeepBodyV[30]);
		glVertex3fv(jeepBodyV[34]);
		glVertex3fv(jeepBodyV[35]);
		glVertex3fv(jeepBodyV[31]);

		glColor3f(0, 0.3, 0);
		glVertex3fv(jeepBodyV[24]);
		glVertex3fv(jeepBodyV[25]);
		glVertex3fv(jeepBodyV[33]);
		glVertex3fv(jeepBodyV[32]);

		glVertex3fv(jeepBodyV[25]);
		glVertex3fv(jeepBodyV[26]);
		glVertex3fv(jeepBodyV[34]);
		glVertex3fv(jeepBodyV[33]);

		glVertex3fv(jeepBodyV[26]);
		glVertex3fv(jeepBodyV[34]);
		glVertex3fv(jeepBodyV[35]);
		glVertex3fv(jeepBodyV[27]);

		glVertex3fv(jeepBodyV[21]);
		glVertex3fv(jeepBodyV[29]);
		glVertex3fv(jeepBodyV[28]);
		glVertex3fv(jeepBodyV[20]);

		glVertex3fv(jeepBodyV[22]);
		glVertex3fv(jeepBodyV[30]);
		glVertex3fv(jeepBodyV[29]);
		glVertex3fv(jeepBodyV[21]);

		glVertex3fv(jeepBodyV[22]);
		glVertex3fv(jeepBodyV[23]);
		glVertex3fv(jeepBodyV[31]);
		glVertex3fv(jeepBodyV[30]);
	glEnd();
glPopMatrix();

	glPushMatrix(); 
	glRotatef(-90,0,1,0);
	glTranslatef(0.2,0,-0.67);
	drawWheel();
	glPopMatrix(); 

	glPushMatrix(); 
	glRotatef(-90,0,1,0);
	glTranslatef(-0.2,0,-0.67);
	drawWheel();
	glPopMatrix(); 

	glPushMatrix(); 
	glRotatef(90,0,1,0);
	glTranslatef(0.2,0,-0.67);
	drawWheel();
	glPopMatrix(); 

	glPushMatrix(); 
	glRotatef(90,0,1,0);
	glTranslatef(-0.2,0,-0.67);
	drawWheel();
	glPopMatrix(); 

	
}
void drawVan()
{
glPushMatrix();
	// The eight points of the truck body
	GLfloat vanBodyV [][3]=
	{
		{-0.4,  0.2,  0.45},			// Vertex[0]
		{ 0.4,  0.2,  0.45},			// Vertex[1]
		{-0.4,  0.0,  0.45},			// Vertex[2]
		{ 0.4,  0.0,  0.45},			// Vertex[3]
		{-0.4,  0.25, 0.4},			// Vertex[4]
		{ 0.4,  0.25, 0.4},			// Vertex[5]
		{-0.4,  0.30, 0.25},		// Vertex[6]
		{ 0.4,  0.30, 0.25},		// Vertex[7]
		{-0.4,	0.45, 0.15},		// Vertex[8]  
		{ 0.4,	0.45, 0.15},		// Vertex[9]
		{-0.4,  0.45, -0.28},		// Vertex[10]
		{ 0.4,  0.45, -0.28},		// Vertex[11]
		{-0.4,  0.25, -0.5},		// Vertex[12]
		{ 0.4,  0.25, -0.5},		// Vertex[13]
		{-0.4,  0.0, -0.5},			// Vertex[14]
		{ 0.4,  0.0, -0.5},			// Vertex[15]
				
	};

	glBegin(GL_QUADS);

		glColor3f(0.0, 0.6, 0.8);
		glVertex3fv(vanBodyV[0]);
		glVertex3fv(vanBodyV[1]);
		glVertex3fv(vanBodyV[3]);
		glVertex3fv(vanBodyV[2]);

		glColor3f(0.2, 0.0, 0.6);
		glVertex3fv(vanBodyV[4]);
		glVertex3fv(vanBodyV[5]);
		glVertex3fv(vanBodyV[1]);
		glVertex3fv(vanBodyV[0]);

		
		glVertex3fv(vanBodyV[6]);
		glVertex3fv(vanBodyV[7]);
		glVertex3fv(vanBodyV[5]);
		glVertex3fv(vanBodyV[4]);

		glColor3f(1.0, 1.0, 1.0);
		glVertex3fv(vanBodyV[8]);
		glVertex3fv(vanBodyV[9]);
		glVertex3fv(vanBodyV[7]);
		glVertex3fv(vanBodyV[6]);

		glColor3f(0.0, 0.6, 0.8);
		glVertex3fv(vanBodyV[10]);
		glVertex3fv(vanBodyV[11]);
		glVertex3fv(vanBodyV[9]);
		glVertex3fv(vanBodyV[8]);

		glColor3f(1.0, 1.0, 1.0);
		glVertex3fv(vanBodyV[12]);
		glVertex3fv(vanBodyV[13]);
		glVertex3fv(vanBodyV[11]);
		glVertex3fv(vanBodyV[10]);

		glColor3f(0.0, 0.6, 0.8);
		glVertex3fv(vanBodyV[14]);
		glVertex3fv(vanBodyV[15]);
		glVertex3fv(vanBodyV[13]);
		glVertex3fv(vanBodyV[12]);

		glVertex3fv(vanBodyV[3]);
		glVertex3fv(vanBodyV[2]);
		glVertex3fv(vanBodyV[14]);
		glVertex3fv(vanBodyV[15]);
	
	glEnd();

	glBegin(GL_POLYGON);
		glColor3f(0.0, 0.6, 0.9);
		glVertex3fv(vanBodyV[3]);
		glVertex3fv(vanBodyV[1]);
		glVertex3fv(vanBodyV[5]);
		glVertex3fv(vanBodyV[7]);
		glVertex3fv(vanBodyV[9]);
		glVertex3fv(vanBodyV[11]);
		glVertex3fv(vanBodyV[13]);
		glVertex3fv(vanBodyV[15]);
	glEnd();

	glBegin(GL_POLYGON);
		glColor3f(0.0, 0.6, 0.9);
		glVertex3fv(vanBodyV[2]);
		glVertex3fv(vanBodyV[0]);
		glVertex3fv(vanBodyV[4]);
		glVertex3fv(vanBodyV[6]);
		glVertex3fv(vanBodyV[8]);
		glVertex3fv(vanBodyV[10]);
		glVertex3fv(vanBodyV[12]);
		glVertex3fv(vanBodyV[14]);
	glEnd();


	glPopMatrix();

	glPushMatrix(); 
	glRotatef(-90,0,1,0);
	glTranslatef(0.2,0,-0.58);
	drawWheel();
	glPopMatrix(); 

	glPushMatrix(); 
	glRotatef(-90,0,1,0);
	glTranslatef(-0.2,0,-0.58);
	drawWheel();
	glPopMatrix(); 

	glPushMatrix(); 
	glRotatef(90,0,1,0);
	glTranslatef(0.2,0,-0.58);
	drawWheel();
	glPopMatrix(); 

	glPushMatrix(); 
	glRotatef(90,0,1,0);
	glTranslatef(-0.2,0,-0.58);
	drawWheel();
	glPopMatrix(); 

}


void drawTruck()
{
glPushMatrix();
	// The eight points of the truck body
	GLfloat truckBodyV [][3]=
	{
		{-0.4,  0.3,  0.45},			// Vertex[0]
		{ 0.4,  0.3,  0.45},			// Vertex[1]
		{-0.4,  0.0,  0.5},			// Vertex[2]
		{ 0.4,  0.0,  0.5},			// Vertex[3]
		{-0.4,  0.3, -0.5},			// Vertex[4]
		{ 0.4,  0.3, -0.5},			// Vertex[5]
		{-0.4,  0.0, -0.5},			// Vertex[6]
		{ 0.4,  0.0, -0.5},			// Vertex[7]
	};

	glBegin(GL_QUADS);

		// Front face
		glColor3f(0.4, 0.0, 0.0);
		glVertex3fv(truckBodyV[0]);
		glVertex3fv(truckBodyV[1]);
		glVertex3fv(truckBodyV[3]);
		glVertex3fv(truckBodyV[2]);

		
		// Top face
		glColor3f(0.6, 0.0, 0.0);
		glVertex3fv(truckBodyV[0]);
		glVertex3fv(truckBodyV[4]);
		glVertex3fv(truckBodyV[5]);
		glVertex3fv(truckBodyV[1]);
	

		

		// Right face
		glVertex3fv(truckBodyV[1]);
		glVertex3fv(truckBodyV[5]);
		glVertex3fv(truckBodyV[7]);
		glVertex3fv(truckBodyV[3]);

		// Bottom face
		glVertex3fv(truckBodyV[2]);
		glVertex3fv(truckBodyV[6]);
		glVertex3fv(truckBodyV[7]);
		glVertex3fv(truckBodyV[3]);

		// Left face
		glVertex3fv(truckBodyV[0]);
		glVertex3fv(truckBodyV[4]);
		glVertex3fv(truckBodyV[6]);
		glVertex3fv(truckBodyV[2]);

		// Back face
		glVertex3fv(truckBodyV[4]);
		glVertex3fv(truckBodyV[5]);
		glVertex3fv(truckBodyV[7]);
		glVertex3fv(truckBodyV[6]);				
		
	glEnd();

	// The eight points of the truck front top
	GLfloat truckFrontTopV [][3]=
	{
		{-0.4,  0.6, 0.3},			// Vertex[0]
		{ 0.4,  0.6, 0.3},			// Vertex[1]
		{-0.4,  0.3, 0.35},			// Vertex[2]
		{ 0.4,  0.3, 0.35},			// Vertex[3]
		{-0.4,  0.6, 0.1},			// Vertex[4]
		{ 0.4,  0.6, 0.1},			// Vertex[5]
		{-0.4,  0.3, 0.1},			// Vertex[6]
		{ 0.4,  0.3, 0.1},			// Vertex[7]
	};

	glBegin(GL_QUADS);

		// Front face
		
		glColor3f(1.0, 1.0, 1.0);
		glVertex3fv(truckFrontTopV[0]);
		glVertex3fv(truckFrontTopV[1]);
		glVertex3fv(truckFrontTopV[3]);
		glVertex3fv(truckFrontTopV[2]);

		// Top face		
		glColor3f(0.5, 0.0, 0.0);
		glVertex3fv(truckFrontTopV[0]);
		glVertex3fv(truckFrontTopV[4]);
		glVertex3fv(truckFrontTopV[5]);
		glVertex3fv(truckFrontTopV[1]);

		// Right face
		
		glVertex3fv(truckFrontTopV[1]);
		glVertex3fv(truckFrontTopV[5]);
		glVertex3fv(truckFrontTopV[7]);
		glVertex3fv(truckFrontTopV[3]);

		// Bottom face
		glVertex3fv(truckFrontTopV[2]);
		glVertex3fv(truckFrontTopV[6]);
		glVertex3fv(truckFrontTopV[7]);
		glVertex3fv(truckFrontTopV[3]);

		// Left face
		glVertex3fv(truckFrontTopV[0]);
		glVertex3fv(truckFrontTopV[4]);
		glVertex3fv(truckFrontTopV[6]);
		glVertex3fv(truckFrontTopV[2]);

		// Back face
		glVertex3fv(truckFrontTopV[4]);
		glVertex3fv(truckFrontTopV[5]);
		glVertex3fv(truckFrontTopV[7]);
		glVertex3fv(truckFrontTopV[6]);				
		
	glEnd();

	// The eight points of the truck Back top
	GLfloat truckBackTopV [][3]=
	{
		{-0.4,  0.6, 0.0},			// Vertex[0]
		{ 0.4,  0.6, 0.0},			// Vertex[1]
		{-0.4,  0.3, 0.0},			// Vertex[2]
		{ 0.4,  0.3, 0.0},			// Vertex[3]
		{-0.4,  0.6, -0.5},			// Vertex[4]
		{ 0.4,  0.6, -0.5},			// Vertex[5]
		{-0.4,  0.3, -0.5},			// Vertex[6]
		{ 0.4,  0.3, -0.5},			// Vertex[7]
	};

	glBegin(GL_QUADS);

		// Front face
		glColor3f(0.6, 0.4, 0.0);
		glVertex3fv(truckBackTopV[0]);
		glVertex3fv(truckBackTopV[1]);
		glVertex3fv(truckBackTopV[3]);
		glVertex3fv(truckBackTopV[2]);

		// Top face	
		glColor3f(0.6, 0.4, 0.0);
		
		glVertex3fv(truckBackTopV[0]);
		glVertex3fv(truckBackTopV[4]);
		glVertex3fv(truckBackTopV[5]);
		glVertex3fv(truckBackTopV[1]);

		// Right face
		glColor3f(0.8, 0.4, 0.2);
		glVertex3fv(truckBackTopV[1]);
		glVertex3fv(truckBackTopV[5]);
		glVertex3fv(truckBackTopV[7]);
		glVertex3fv(truckBackTopV[3]);

		// Bottom face
		glColor3f(0.6, 0.4, 0.0);
		glVertex3fv(truckBackTopV[2]);
		glVertex3fv(truckBackTopV[6]);
		glVertex3fv(truckBackTopV[7]);
		glVertex3fv(truckBackTopV[3]);

		// Left face
		glColor3f(0.8, 0.4, 0.2);
		glVertex3fv(truckBackTopV[0]);
		glVertex3fv(truckBackTopV[4]);
		glVertex3fv(truckBackTopV[6]);
		glVertex3fv(truckBackTopV[2]);

		// Back face
		glColor3f(0.6, 0.4, 0.0);
		glVertex3fv(truckBackTopV[4]);
		glVertex3fv(truckBackTopV[5]);
		glVertex3fv(truckBackTopV[7]);
		glVertex3fv(truckBackTopV[6]);				
		
	glEnd();
	glPopMatrix();

	glPushMatrix(); 
	glRotatef(-90,0,1,0);
	glTranslatef(0.2,0,-0.57);
	drawWheel();
	glPopMatrix(); 

	glPushMatrix(); 
	glRotatef(-90,0,1,0);
	glTranslatef(-0.2,0,-0.57);
	drawWheel();
	glPopMatrix(); 

	glPushMatrix(); 
	glRotatef(90,0,1,0);
	glTranslatef(0.2,0,-0.57);
	drawWheel();
	glPopMatrix(); 

	glPushMatrix(); 
	glRotatef(90,0,1,0);
	glTranslatef(-0.2,0,-0.57);
	drawWheel();
	glPopMatrix(); 

}



void drawCar()
{

	// The eight points of the car body
	GLfloat carBodyV [][3]=
	{
		{-0.3,  0.2,  0.25},			// Vertex[0]
		{ 0.3,  0.2,  0.25},			// Vertex[1]
		{-0.3,  0.0,  0.25},			// Vertex[2]
		{ 0.3,  0.0,  0.25},			// Vertex[3]
		{-0.3,  0.2, -0.45},			// Vertex[4]
		{ 0.3,  0.2, -0.45},			// Vertex[5]
		{-0.3,  0.0, -0.45},			// Vertex[6]
		{ 0.3,  0.0, -0.45},			// Vertex[7]
	};

	glBegin(GL_QUADS);

		// Top face
		glColor3f(1.0, 1.0, 0.0);
		glVertex3fv(carBodyV[0]);
		glVertex3fv(carBodyV[4]);
		glVertex3fv(carBodyV[5]);
		glVertex3fv(carBodyV[1]);

		// Right face
		glColor3f(1.0, 0.4, 0.2);
		glVertex3fv(carBodyV[1]);
		glVertex3fv(carBodyV[5]);
		glVertex3fv(carBodyV[7]);
		glVertex3fv(carBodyV[3]);

		// Bottom face
		glColor3f(1.0, 1.0, 0.0);
		glVertex3fv(carBodyV[2]);
		glVertex3fv(carBodyV[6]);
		glVertex3fv(carBodyV[7]);
		glVertex3fv(carBodyV[3]);

		// Left face
		glColor3f(1.0, 0.4, 0.2);
		glVertex3fv(carBodyV[0]);
		glVertex3fv(carBodyV[4]);
		glVertex3fv(carBodyV[6]);
		glVertex3fv(carBodyV[2]);

		// Back face
		glColor3f(1.0, 1.0, 0.0);
		glVertex3fv(carBodyV[4]);
		glVertex3fv(carBodyV[5]);
		glVertex3fv(carBodyV[7]);
		glVertex3fv(carBodyV[6]);				
		
	glEnd();


	// The eight points of the car hood
	GLfloat carHoodV [][3]=
	{
		{-0.25,  0.10, 0.5},		// Vertex[4]
		{ 0.25,  0.10, 0.5},		// Vertex[5]
		{-0.25,  0.0, 0.5},			// Vertex[6]
		{ 0.25,  0.0, 0.5},			// Vertex[7]

		{-0.3,  0.2,  0.25},		// Vertex[0]
		{ 0.3,  0.2,  0.25},		// Vertex[1]
		{-0.3,  0.0,  0.25},		// Vertex[2]
		{ 0.3, -0.0,  0.25},		// Vertex[3]
	};

	glBegin(GL_QUADS);

		// Front face
		glColor3f(1.0, 0.6, 0.0);
		glVertex3fv(carHoodV[0]);
		glVertex3fv(carHoodV[1]);
		glVertex3fv(carHoodV[3]);
		glVertex3fv(carHoodV[2]);

		// Top face
		glColor3f(1.0, 1.0, 0.0);
		glVertex3fv(carHoodV[0]);
		glVertex3fv(carHoodV[4]);
		glVertex3fv(carHoodV[5]);
		glVertex3fv(carHoodV[1]);

		// Right face
		glColor3f(1.0, 0.4, 0.2);
		glVertex3fv(carHoodV[1]);
		glVertex3fv(carHoodV[5]);
		glVertex3fv(carHoodV[7]);
		glVertex3fv(carHoodV[3]);

		// Bottom face
		glColor3f(1.0, 1.0, 0.0);
		glVertex3fv(carHoodV[2]);
		glVertex3fv(carHoodV[6]);
		glVertex3fv(carHoodV[7]);
		glVertex3fv(carHoodV[3]);

		// Left face
		glColor3f(1.0, 0.4, 0.2);
		glVertex3fv(carHoodV[0]);
		glVertex3fv(carHoodV[4]);
		glVertex3fv(carHoodV[6]);
		glVertex3fv(carHoodV[2]);

		// Back face
		glColor3f(1.0, 1.0, 0.0);
		glVertex3fv(carHoodV[4]);
		glVertex3fv(carHoodV[5]);
		glVertex3fv(carHoodV[7]);
		glVertex3fv(carHoodV[6]);				
		
	glEnd();

	// The eight points of the car top
	GLfloat carTopV [][3]=
	{
		{-0.2,  0.3,  0.15},			// Vertex[0]
		{ 0.2,  0.3,  0.15},			// Vertex[1]
		{-0.2,  0.2,  0.25},			    // Vertex[2]
		{ 0.2,  0.2,  0.25},			    // Vertex[3]
		{-0.2,  0.3, -0.15},			// Vertex[4]
		{ 0.2,  0.3, -0.15},			// Vertex[5]
		{-0.2,  0.2, -0.25},			// Vertex[6]
		{ 0.2,  0.2, -0.25},			// Vertex[7]
	};

	glBegin(GL_QUADS);

		// Front face
		glColor3f(1.0, 1.0, 1.0);
		glVertex3fv(carTopV[0]);
		glVertex3fv(carTopV[1]);
		glVertex3fv(carTopV[3]);
		glVertex3fv(carTopV[2]);

		// Top face
		glColor3f(0.95, 1.0, 0.8);
		glVertex3fv(carTopV[0]);
		glVertex3fv(carTopV[4]);
		glVertex3fv(carTopV[5]);
		glVertex3fv(carTopV[1]);

		// Right face
		glColor3f(1.0, 1.0, 0.8);
		glVertex3fv(carTopV[1]);
		glVertex3fv(carTopV[5]);
		glVertex3fv(carTopV[7]);
		glVertex3fv(carTopV[3]);

		// Bottom face
		glColor3f(1.0, 1.0, 1.0);
		glVertex3fv(carTopV[2]);
		glVertex3fv(carTopV[6]);
		glVertex3fv(carTopV[7]);
		glVertex3fv(carTopV[3]);

		// Left face
		glColor3f(1.0, 1.0, 0.8);
		glVertex3fv(carTopV[0]);
		glVertex3fv(carTopV[4]);
		glVertex3fv(carTopV[6]);
		glVertex3fv(carTopV[2]);

		// Back face
		glVertex3fv(carTopV[4]);
		glVertex3fv(carTopV[5]);
		glVertex3fv(carTopV[7]);
		glVertex3fv(carTopV[6]);				
		
	glEnd();
	
	// The eight points of the car spoiler
	GLfloat carSpoilerV [][3]=
	{
		{-0.3,  0.265, -0.45},			// Vertex[0]
		{ 0.3,  0.265, -0.45},			// Vertex[1]
		{-0.3,  0.2,   -0.35},			// Vertex[2]
		{ 0.3,  0.2,   -0.35},			// Vertex[3]
		{-0.3,  0.2,   -0.45},			// Vertex[4]
		{ 0.3,  0.2,   -0.45},			// Vertex[5]
	};

	glBegin(GL_QUADS);

		// Front face
		glColor3f(0.9, 0.9, 0.0);
		glVertex3fv(carSpoilerV[0]);
		glVertex3fv(carSpoilerV[1]);
		glVertex3fv(carSpoilerV[3]);
		glVertex3fv(carSpoilerV[2]);


		// Right face
		glColor3f(1.0, 0.4, 0.2);
		glVertex3fv(carSpoilerV[0]);
		glVertex3fv(carSpoilerV[2]);
		glVertex3fv(carSpoilerV[4]);
		glVertex3fv(carSpoilerV[0]);
		
		
		// Back face
		glColor3f(1.0, 1.0, 0.0);
		glVertex3fv(carSpoilerV[4]);
		glVertex3fv(carSpoilerV[0]);
		glVertex3fv(carSpoilerV[1]);
		glVertex3fv(carSpoilerV[5]);
		
		// Left face
		glColor3f(1.0, 0.4, 0.2);
		glVertex3fv(carSpoilerV[5]);
		glVertex3fv(carSpoilerV[1]);
		glVertex3fv(carSpoilerV[3]);
		glVertex3fv(carSpoilerV[5]);
				
	glEnd();

	glPushMatrix(); 
	glRotatef(-90,0,1,0);
	glTranslatef(0.2,0,-0.67);
	drawWheel();
	glPopMatrix(); 

	glPushMatrix(); 
	glRotatef(-90,0,1,0);
	glTranslatef(-0.2,0,-0.67);
	drawWheel();
	glPopMatrix(); 

	glPushMatrix(); 
	glRotatef(90,0,1,0);
	glTranslatef(0.2,0,-0.67);
	drawWheel();
	glPopMatrix(); 

	glPushMatrix(); 
	glRotatef(90,0,1,0);
	glTranslatef(-0.2,0,-0.67);
	drawWheel();
	glPopMatrix(); 


}
//******************************************************
//ok, we're back to my code


void text(char* string)
{
    char* p;

    for (p = string; *p; p++)
	       glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *p);	//heres a fun fact, *p hates integers
															//i guess thats obvious though...

	//       glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
}

void textLarge(char* string)
{
    char* p;

    for (p = string; *p; p++)
	{
		glPushMatrix();
		glScalef(5,5,5);
		glutBitmapCharacter(GLUT_BITMAP_8_BY_13, *p);
		glPopMatrix();
	}
	//       glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, *p);
}


void kbArrows(int key, int x, int y)
{
	
	switch(key)
	{
	case GLUT_KEY_LEFT:     //rotate to the left
	if (menustate == true)
	{
		if (P1ChooseCar == true)
		{
			p1choice--;
			if (p1choice == -1)
				p1choice = 3; 
			p1choice %=4;
		}
		
		if (p1choice == 0)
		{
			P1jeep = false;
			p1car = true;
			p1truck = false;
			p1van = false;		
		}
		else if (p1choice == 1)
		{
			P1jeep = true;
			p1car = false;
			p1truck = false;
			p1van = false;		
		}
		else if (p1choice == 2)
		{
			P1jeep = false;
			p1car = false;
			p1truck = true;
			p1van = false;		
		}
		else if (p1choice == 3)
		{
			P1jeep = false;
			p1car = false;
			p1truck = false;
			p1van = true;		
		}



		//P2
		if (P2ChooseCar == true)
		{
			p2choice--;
			if (p2choice == -1)
				p2choice = 3; 
			p2choice %=4;
		}
		
		if (p2choice == 0)
		{
			P2jeep = false;
			p2car = true;
			p2truck = false;
			p2van = false;		
		}
		else if (p2choice == 1)
		{
			P2jeep = true;
			p2car = false;
			p2truck = false;
			p2van = false;		
		}
		else if (p2choice == 2)
		{
			P2jeep = false;
			p2car = false;
			p2truck = true;
			p2van = false;		
		}
		else if (p2choice == 3)
		{
			P2jeep = false;
			p2car = false;
			p2truck = false;
			p2van = true;		
		}
	}
	else
	{ 
		if ( firstpersonview == true)
		{
			P2Direction++;
			if (P2Direction >3)
				P2Direction = 0;

			//P2Direction %= 3;
			
		}
		else if (P2Direction != WEST)
			P2Direction = EAST;
	}
		break;
		
	case GLUT_KEY_RIGHT :      //rotate to the right
	
	if (menustate == true)
	{
		if (P1ChooseCar == true)
		{
			p1choice++;
			p1choice %=4;
		}
		
		if (p1choice == 0)
		{
			P1jeep = false;
			p1car = true;
			p1truck = false;
			p1van = false;		
		}
		else if (p1choice == 1)
		{
			P1jeep = true;
			p1car = false;
			p1truck = false;
			p1van = false;		
		}
		else if (p1choice == 2)
		{
			P1jeep = false;
			p1car = false;
			p1truck = true;
			p1van = false;		
		}
		else if (p1choice == 3)
		{
			P1jeep = false;
			p1car = false;
			p1truck = false;
			p1van = true;		
		}


		//P2
		if (P2ChooseCar == true)
		{
			p2choice++;
			p2choice %=4;
		}
		
		if (p2choice == 0)
		{
			P2jeep = false;
			p2car = true;
			p2truck = false;
			p2van = false;		
		}
		else if (p2choice == 1)
		{
			P2jeep = true;
			p2car = false;
			p2truck = false;
			p2van = false;		
		}
		else if (p2choice == 2)
		{
			P2jeep = false;
			p2car = false;
			p2truck = true;
			p2van = false;		
		}
		else if (p2choice == 3)
		{
			P2jeep = false;
			p2car = false;
			p2truck = false;
			p2van = true;		
		}
	
	}
	else //game mode
	{ 
		if ( firstpersonview == true)
		{
			P2Direction--;
			if (P2Direction < 0)
				P2Direction = 3;
			
		}
		else if (P2Direction != EAST)
			P2Direction = WEST;
	}
	
		break;
		
		//up down
	case GLUT_KEY_UP:      //rotate up
		if (firstpersonview == false)
			if (P2Direction != SOUTH)
				P2Direction = NORTH;

		
		break;
	case GLUT_KEY_DOWN:      //rotate down
		if (firstpersonview == false)
			if (P2Direction != NORTH)
				P2Direction = SOUTH;
	
		
		break;
		
	case GLUT_KEY_F1:
		if ( PressStart == true )
		{
			PressStart = false;
			P1ChooseCar = true;
		}
		else if (P1ChooseCar == true)
		{
			P1ChooseCar = false;
			P2ChooseCar = true;
		}
		else if (P2ChooseCar == true)
		{
			P2ChooseCar = false;
			ReadyScreen = true;
		}
		else if (menustate == true)
		{
			menustate = false;
			gameOver = false;
			P1Collision = false;
			P2Collision = false;
			
			//P1 pos
			P1PosX = P1PosXStart;
			P1PosY = P1PosYStart;
			P1Direction = 0;//n
			
			//P2 pos
			P2PosX = P2PosXStart;
			P2PosY = P2PosYStart;
			P2Direction = 2;//s

			//WAIT = 250;
		}
		break;
	default:                //stop pressing the wrong keys. here have the instructions
		cout << "Press 'h' for help. " <<endl;
		break;


	}
}
 
void showMenu()
{
	//figures out which menu state to be in

	if (PressStart == true)	// this is the splash screen
	{
		themeSong->Play();

		drawLogo();
		glPushMatrix();
			glTranslatef(15,0,5);
			glScalef(10,10,10);
			glRotatef(rot,1,0,0);
			glRotatef(-90,0,0,1);
			drawJeep();


		glPopMatrix();

		glPushMatrix();
			glTranslatef(15,0,17);
			glScalef(10,10,10);
			glRotatef(rot,1,0,0);
			glRotatef(-90,0,0,1);

			drawCar();
		glPopMatrix();

		glPushMatrix();
			glTranslatef(15,0,33);
			glScalef(10,10,10);
			glRotatef(rot,1,0,0);
			glRotatef(-90,0,0,1);
			drawVan();
		glPopMatrix();

		glPushMatrix();
			glTranslatef(15,0,46);
			glScalef(10,10,10);
			glRotatef(rot,1,0,0);
			glRotatef(-90,0,0,1);
			drawTruck();
		glPopMatrix();

		//glRasterPos3i(5, 5,10);
		//text("Press F1!");

		//glRasterPos3i(10, 10,10);
		//text("Start Screen!");
		//glutBitmapCharacter
        //glutBitmapCharacter(GLUT_BITMAP_HELVETICA_18, );

	}

	else if (P1ChooseCar == true)	//this is p1 choice
	{
		drawBKG();
		if (p1car == true)
		{

			glColor3f( 0, 0, 0  ); //PURPLE bottom
			glRasterPos3i(20, 30,15);
			textLarge("Press F1 to Select the Car");

			glPushMatrix();
				glTranslatef(25,0,25);
				glScalef(26,26,26);
				glRotatef(rot,1,0,0);
				glRotatef(-90,0,0,1);
				drawCar();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(20,0,25);
				glScalef(26,5,26);
				glRotatef(rot,1,0,0);
				glRotatef(-270,0,0,1);
				
				  glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);	
				  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_transparent);
				  glEnable (GL_BLEND);
				  glDepthMask (GL_FALSE);
				  glBlendFunc (GL_SRC_ALPHA, GL_ONE);

				drawCar();

				  glDepthMask (GL_TRUE);
				  glDisable (GL_BLEND);
			glPopMatrix();
		}
		else if (P1jeep == true)
		{
			glColor3f( 0, 0, 0  ); //PURPLE bottom
			glRasterPos3i(20, 30,15);
			textLarge("Press F1 to Select the Jeep");

			glPushMatrix();
				glTranslatef(25,0,25);
				glScalef(26,26,26);
				glRotatef(rot,1,0,0);
				glRotatef(-90,0,0,1);
				drawJeep();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(20,0,25);
				glScalef(26,5,26);
				glRotatef(rot,1,0,0);
				glRotatef(-270,0,0,1);
				
				  glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);	
				  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_transparent);
				  glEnable (GL_BLEND);
				  glDepthMask (GL_FALSE);
				  glBlendFunc (GL_SRC_ALPHA, GL_ONE);

				drawJeep();

				  glDepthMask (GL_TRUE);
				  glDisable (GL_BLEND);
			glPopMatrix();
		}
		else if (p1truck == true)
		{			
			glColor3f( 0, 0, 0  ); //PURPLE bottom
			glRasterPos3i(20, 30,15);
			textLarge("Press F1 to Select the Truck");

			glPushMatrix();
				glTranslatef(25,0,25);
				glScalef(26,26,26);
				glRotatef(rot,1,0,0);
				glRotatef(-90,0,0,1);
				drawTruck();
			glPopMatrix();

			glPushMatrix();
				glTranslatef(20,0,25);
				glScalef(26,5,26);
				glRotatef(rot,1,0,0);
				glRotatef(-270,0,0,1);
				
				  glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);	
				  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_transparent);
				  glEnable (GL_BLEND);
				  glDepthMask (GL_FALSE);
				  glBlendFunc (GL_SRC_ALPHA, GL_ONE);

				drawTruck();

				  glDepthMask (GL_TRUE);
				  glDisable (GL_BLEND);
			glPopMatrix();

		}
		else if (p1van == true)
		{
			glColor3f( 0, 0, 0  ); //PURPLE bottom
			glRasterPos3i(20, 0,15);
			textLarge("Press F1 to Select the Mini Van");

			glPushMatrix();
				glTranslatef(25,30,25);
				glScalef(15,15,15);
				glRotatef(rot,1,0,0);
				glRotatef(-90,0,0,1);
				drawVan();
			
			glPopMatrix();

			glPushMatrix();
				glTranslatef(20,0,25);
				glScalef(20,5,20);
				glRotatef(rot,1,0,0);
				glRotatef(-270,0,0,1);
				
				  glMaterialfv(GL_FRONT, GL_EMISSION, mat_emission);	
				  glMaterialfv(GL_FRONT, GL_DIFFUSE, mat_transparent);
				  glEnable (GL_BLEND);
				  glDepthMask (GL_FALSE);
				  glBlendFunc (GL_SRC_ALPHA, GL_ONE);

				drawVan();

				  glDepthMask (GL_TRUE);
				  glDisable (GL_BLEND);
			glPopMatrix();
		}

		
		
		glColor3f( 0, 0, 0  ); //PURPLE bottom
		


		//glRasterPos3i(1, 5,10);
		//text("Press F1!");

		glRasterPos3i(45, 0,10);
		text("Player 1, Select your car!!");

		glRasterPos3i(10, 10,5);
		text("Use the left and right keyboard buttons to change cars");
	}
	else if (P2ChooseCar == true)	// this is p2 choice
	{
		drawBKG();
		if (p2car == true)
		{

			glColor3f( 0, 0, 0  ); //PURPLE bottom
			glRasterPos3i(20, 0,15);
			textLarge("Press F1 to Select the Car");

			glPushMatrix();
				glTranslatef(25,0,25);
				glScalef(26,26,26);
				glRotatef(rot,1,0,0);
				glRotatef(-90,0,0,1);
				drawCar();
			glPopMatrix();
		}
		else if (P2jeep == true)
		{
			glColor3f( 0, 0, 0  ); //PURPLE bottom
			glRasterPos3i(20, 0,15);
			textLarge("Press F1 to Select the Jeep");

			glPushMatrix();
				glTranslatef(25,0,25);
				glScalef(26,26,26);
				glRotatef(rot,1,0,0);
				glRotatef(-90,0,0,1);
				drawJeep();
			glPopMatrix();
		}
		else if (p2truck == true)
		{			
			glColor3f( 0, 0, 0  ); //PURPLE bottom
			glRasterPos3i(20, 0,15);
			textLarge("Press F1 to Select the Truck");

			glPushMatrix();
				glTranslatef(25,0,25);
				glScalef(26,26,26);
				glRotatef(rot,1,0,0);
				glRotatef(-90,0,0,1);
				drawTruck();
			glPopMatrix();
		}
		else if (p2van == true)
		{
			glColor3f( 0, 0, 0  ); //PURPLE bottom
			glRasterPos3i(20, 0,15);
			textLarge("Press F1 to Select the Mini Van");

			glPushMatrix();
				glTranslatef(25,0,25);
				glScalef(26,26,26);
				glRotatef(rot,1,0,0);
				glRotatef(-90,0,0,1);
				drawVan();
			glPopMatrix();
		}

		
		
		glColor3f( 0, 0, 0  ); //PURPLE bottom
		


		//glRasterPos3i(1, 5,10);
		//text("Press F1!");

		glRasterPos3i(45, 0,10);
		text("Player 2, Select your car!!");

		glRasterPos3i(10, 0,5);
		text("Use the left and right keyboard buttons to change cars");

	}
	else if (ReadyScreen == true)	// this is the screen with the instructions-- after this screen game plays
	{
		drawInstructions();
		
		//Sleep(1000);


		glColor3f( 0, 0, 0  ); //PURPLE bottom

		/*
		glRasterPos3i(5, 5,10);
		text("Press F1 TO BEGIN GAME!");

		glRasterPos3i(10, 10,10);
		text("Show instructions");
		*/
		themeSong->Stop();
		gameSong->Play();
		
			pause = false;
			firstpersonview = false;
			p2view = false;
			p1view = false;
			viewnorm = true;
	}




}


void loadTextures()
{
	
   //glShadeModel(GL_FLAT);
	
//here we're loading the bmps into memory
//the thing is that the library i'm using seems to be written for single textures
//so what i'm doing is using his library to create a pointer to the converted images
//and then using that pointer in my code

	front.LoadBitmap("front.bmp");
	back.LoadBitmap("back.bmp");
	
	rightt.LoadBitmap("right.bmp");
	leftt.LoadBitmap("left.bmp");
	top.LoadBitmap("top.bmp");
	bottom.LoadBitmap("bottom.bmp");
	
	background.LoadBitmap("bkg.bmp");
	instructions.LoadBitmap("instructions.bmp");
	logo.LoadBitmap("logo.bmp");
	
	p1win.LoadBitmap("p1win.bmp");
	p2win.LoadBitmap("p2win.bmp");
	losers.LoadBitmap("losers.bmp");


//end loading bmps into memory

//store the pointers in my opengl app

	// Generate the texture id
	// i have 6 textures and i'm storing them in an array of 6 reference-able memory spaces
	glGenTextures(13, texturelist);
	


//Load the BACK picture into opengl
	glBindTexture(GL_TEXTURE_2D, texturelist[1]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, back.imageWidth,
		back.imageHeight, GL_RGB, GL_UNSIGNED_BYTE,
		back.image);
//end BACK load
	
	//the reason back is in front of front is for debugging purposes

//Load the FRONT picture into opengl
	glBindTexture(GL_TEXTURE_2D, texturelist[0]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, front.imageWidth,
		front.imageHeight, GL_RGB, GL_UNSIGNED_BYTE,
		front.image);
//end FRONT load

//Load the RIGHT picture into opengl
	glBindTexture(GL_TEXTURE_2D, texturelist[2]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, rightt.imageWidth,
		rightt.imageHeight, GL_RGB, GL_UNSIGNED_BYTE,
		rightt.image);
//end RIGHT load


//Load the LEFT picture into opengl
	glBindTexture(GL_TEXTURE_2D, texturelist[3]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, leftt.imageWidth,
		leftt.imageHeight, GL_RGB, GL_UNSIGNED_BYTE,
		leftt.image);
//end LEFT load

//Load the TOP picture into opengl
	glBindTexture(GL_TEXTURE_2D, texturelist[4]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, top.imageWidth,
		top.imageHeight, GL_RGB, GL_UNSIGNED_BYTE,
		top.image);
//end TOP load

//Load the BOTTOM picture into opengl
	glBindTexture(GL_TEXTURE_2D, texturelist[5]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, bottom.imageWidth,
		bottom.imageHeight, GL_RGB, GL_UNSIGNED_BYTE,
		bottom.image);
//end BOTTOM load


//Load the bkg picture into opengl
	glBindTexture(GL_TEXTURE_2D, texturelist[6]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, background.imageWidth,
		background.imageHeight, GL_RGB, GL_UNSIGNED_BYTE,
		background.image);
//end BACK load


//Load the bkg picture into opengl
	glBindTexture(GL_TEXTURE_2D, texturelist[7]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, logo.imageWidth,
		logo.imageHeight, GL_RGB, GL_UNSIGNED_BYTE,
		logo.image);
//end BACK load

//Load the bkg picture into opengl
	glBindTexture(GL_TEXTURE_2D, texturelist[8]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, instructions.imageWidth,
		instructions.imageHeight, GL_RGB, GL_UNSIGNED_BYTE,
		instructions.image);
//end BACK load


	
	//Load the bkg picture into opengl
	glBindTexture(GL_TEXTURE_2D, texturelist[9]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, p2win.imageWidth,
		p2win.imageHeight, GL_RGB, GL_UNSIGNED_BYTE,
		p2win.image);
//end BACK load

		//Load the bkg picture into opengl
	glBindTexture(GL_TEXTURE_2D, texturelist[10]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, p1win.imageWidth,
		p1win.imageHeight, GL_RGB, GL_UNSIGNED_BYTE,
		p1win.image);
//end BACK load

		//Load the bkg picture into opengl
	glBindTexture(GL_TEXTURE_2D, texturelist[11]);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_NEAREST);
	

	gluBuild2DMipmaps(GL_TEXTURE_2D, GL_RGB, losers.imageWidth,
		losers.imageHeight, GL_RGB, GL_UNSIGNED_BYTE,
		losers.image);
//end BACK load

}


void setupLights()
{
		
	glPushMatrix();

	//more red book stuff
	//gotta learn somewhere

glEnable(GL_COLOR_MATERIAL);

glLightModeli(GL_LIGHT_MODEL_TWO_SIDE,0);


	GLfloat LightAmbient[]=		{ (cos(rot*PI/180)), 0.2f, 0.4f, 1.0f };	//isn't cosine functions great? its always b/w 0 n 1
	GLfloat LightDiffuse[]=		{ 0.4f, 0.4f, 0.4f, 1.0f };
	GLfloat LightPosition[]=	{ (cos(rot*PI/180)), 20.0f, 6.0f, 1.0f };

	glLightfv(GL_LIGHT1, GL_AMBIENT, LightAmbient);		// Setup The Ambient Light
	//glLightfv(GL_LIGHT1, GL_DIFFUSE, LightDiffuse);		// Setup The Diffuse Light
	glLightfv(GL_LIGHT1, GL_POSITION,LightPosition);	// Position The Light


	

	glPopMatrix();	

}

void drawEmptyTile()
{
	      //bottom****************************

		glEnable(GL_TEXTURE_2D);
	   	glBindTexture(GL_TEXTURE_2D, texturelist[5]);

	   	glBegin(GL_QUADS);


		   glColor3f( 1, 1, 1  ); //PURPLE bottom
		   
		   glTexCoord2f(0.0, 0.0);
		   glVertex3f( -0.5, -0.5, 0.5);
       
		   glTexCoord2f(1.0, 0.0);
		   glVertex3f( 0.5, -0.5, 0.5);
       
		   glTexCoord2f(1.0, 1.0);
		   glVertex3f( 0.5, -0.5, -0.5);
       
		   glTexCoord2f(0.0, 1.0);
		   glVertex3f( -0.5, -0.5, -0.5);


	   glEnd();
	   glDisable(GL_TEXTURE_2D);
       //bottom****************************
}

void drawDrivenTile()
{
	      //bottom****************************

glEnable(GL_TEXTURE_2D);	
	   	glBindTexture(GL_TEXTURE_2D, texturelist[0]);

	   	glBegin(GL_QUADS);


		   glColor3f( 1, 1, 1  ); //PURPLE bottom
		   
		   glTexCoord2f(0.0, 0.0);
		   glVertex3f( -0.5, -0.5, 0.5);
       
		   glTexCoord2f(1.0, 0.0);
		   glVertex3f( 0.5, -0.5, 0.5);
       
		   glTexCoord2f(1.0, 1.0);
		   glVertex3f( 0.5, -0.5, -0.5);
       
		   glTexCoord2f(0.0, 1.0);
		   glVertex3f( -0.5, -0.5, -0.5);


	   glEnd();
       //bottom****************************
glDisable(GL_TEXTURE_2D);	

}

void displayPlayer2Car()
{

			glPushMatrix();

			//	glRasterPos3i(P2PosX,0, P2PosY);
			//	text("Player 2");

			//glTranslatef(P2PosX,0,P2PosY);	
			
			if (P2Direction == 0)
				glRotatef(0,0,1,0);
			else if (P2Direction == 1)
				glRotatef(90,0,1,0);
			if (P2Direction == 2)
				glRotatef(180,0,1,0);
			else if (P2Direction == 3)
				glRotatef(270,0,1,0);
			
			if (p2car == true)
		{
				
				drawCar();
		
		}
		else if (P2jeep == true)
		{
				drawJeep();
		}
		else if (p2truck == true)
		{
				drawTruck();
		}
		else if (p2van == true)
		{
			drawVan();
		}

		glPopMatrix();
}

void displayPlayer1Car()
{

			glPushMatrix();
			//glTranslatef(P2PosX,0,P2PosY);	
			
			if (P1Direction == 0)
				glRotatef(0,0,1,0);
			else if (P1Direction == 1)
				glRotatef(90,0,1,0);
			if (P1Direction == 2)
				glRotatef(180,0,1,0);
			else if (P1Direction == 3)
				glRotatef(270,0,1,0);
			
			if (p1car == true)
		{
				
				drawCar();
		
		}
		else if (P1jeep == true)
		{
				drawJeep();
		}
		else if (p1truck == true)
		{
				drawTruck();
		}
		else if (p1van == true)
		{
			drawVan();
		}

		glPopMatrix();
}

void displayCubeTexture()	//i removed the top off the cube
{
	//Build the square faces
	
	/* 
	texture cube
	i basically copied the old cube and mapped textures to it
	
	*/


	glEnable(GL_TEXTURE_2D);	   
	   
		glBindTexture(GL_TEXTURE_2D, texturelist[4]);
//		glShadeModel(GL_SMOOTH);
	glBegin(GL_QUADS);
	
	//front ****************************


	   glColor3f( 1, 1, 1 ); //white; helps makes the pic clear
   
	   glTexCoord2f(0.0, 0.0);
	   glVertex3f( -0.5, -0.5, 0.5);
       
	   glTexCoord2f(1.0, 0.0);
	   glVertex3f( 0.5, -0.5, 0.5);
       
	   glTexCoord2f(1.0, 1.0);
	   glVertex3f( 0.5, 0.5, 0.5);
       
	   glTexCoord2f(0.0, 1.0);
	   glVertex3f( -0.5, 0.5, 0.5);

	   glEnd();
       //front ****************************

       //back  ****************************



	   glBindTexture(GL_TEXTURE_2D, texturelist[1]);


	glBegin(GL_QUADS);
	   glColor3f( 1, 1, 1 ); //white 

	   glTexCoord2f(0.0, 0.0);
	   glVertex3f( -0.5, -0.5, -0.5);
       
	   glTexCoord2f(1.0, 0.0);
	   glVertex3f( 0.5, -0.5, -0.5);
       
	   glTexCoord2f(1.0, 1.0);
	   glVertex3f( 0.5, 0.5, -0.5);
       
	   glTexCoord2f(0.0, 1.0);
	   glVertex3f( -0.5, 0.5, -0.5);
		
	   glEnd();
       //back  ****************************


	   	//glDisable(GL_TEXTURE_2D);		//VERY IMPORTANT! YOU NEED THIS   

	   
       //top   ****************************
/*
	   		glBindTexture(GL_TEXTURE_2D, texturelist[4]);

       	glBegin(GL_QUADS);

		
	   glColor3f( 1, 1, 1  ); //Yellow top

	   glTexCoord2f(0.0, 0.0);
       glVertex3f( -0.5, 0.5, 0.5);

	   glTexCoord2f(1.0, 0.0);
       glVertex3f( 0.5, 0.5, 0.5);

	   glTexCoord2f(1.0, 1.0);
       glVertex3f( 0.5, 0.5, -0.5);

	   glTexCoord2f(0.0, 1.0);
       glVertex3f( -0.5, 0.5, -0.5);

	   glEnd();

  */
       //top   ****************************
	   
	   
       //bottom****************************


	   		glBindTexture(GL_TEXTURE_2D, texturelist[5]);

	   	glBegin(GL_QUADS);


		   glColor3f( 1, 1, 1  ); //PURPLE bottom
		   
		   glTexCoord2f(0.0, 0.0);
		   glVertex3f( -0.5, -0.5, 0.5);
       
		   glTexCoord2f(1.0, 0.0);
		   glVertex3f( 0.5, -0.5, 0.5);
       
		   glTexCoord2f(1.0, 1.0);
		   glVertex3f( 0.5, -0.5, -0.5);
       
		   glTexCoord2f(0.0, 1.0);
		   glVertex3f( -0.5, -0.5, -0.5);


	   glEnd();
       //bottom****************************
	   
       //right ****************************
	   
	   		glBindTexture(GL_TEXTURE_2D, texturelist[2]);

	   
	   glBegin(GL_QUADS);



       glColor3f( 1, 1, 1  );  //Fuscia? RIGHT

	   glTexCoord2f(0.0, 0.0);
       glVertex3f( 0.5, -0.5, 0.5);

	   glTexCoord2f(1.0, 0.0);
       glVertex3f( 0.5, -0.5, -0.5);

	    glTexCoord2f(1.0, 1.0);
       glVertex3f( 0.5, 0.5, -0.5);

	   glTexCoord2f(0.0, 1.0);
       glVertex3f( 0.5, 0.5, 0.5);

	   glEnd();
       //right ****************************
	   
       //left  ****************************
	   
	   		glBindTexture(GL_TEXTURE_2D, texturelist[3]);

	   
	   glBegin(GL_QUADS);

       glColor3f( 1, 1, 1  ); //WHITE LEFT

	   glTexCoord2f(0.0, 0.0);
       glVertex3f( -0.5, -0.5, 0.5);

	   glTexCoord2f(1.0, 0.0);
       glVertex3f( -0.5, -0.5, -0.5);

	    glTexCoord2f(1.0, 1.0);
       glVertex3f( -0.5, 0.5, -0.5);

	   glTexCoord2f(0.0, 1.0);
       glVertex3f( -0.5, 0.5, 0.5);
       glEnd();
       //left  ****************************


	   glFlush();
	   	   glDisable(GL_TEXTURE_2D);

	   
}

void drawInstructions()
{

	glPushMatrix();
	glTranslatef(25,0,25);	
	glScalef(73,73,73);
	glRotatef(-90,0,1,0);
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D, texturelist[8]);

	   
glBegin(GL_QUADS);


		   glColor3f( 1, 1, 1  ); //PURPLE bottom
		   
		   glTexCoord2f(0.0, 0.0);
		   glVertex3f( -0.5, -0.5, 0.5);
       
		   glTexCoord2f(1.0, 0.0);
		   glVertex3f( 0.5, -0.5, 0.5);
       
		   glTexCoord2f(1.0, 1.0);
		   glVertex3f( 0.5, -0.5, -0.5);
       
		   glTexCoord2f(0.0, 1.0);
		   glVertex3f( -0.5, -0.5, -0.5);


	   glEnd();
       //left  ****************************

	   	   glDisable(GL_TEXTURE_2D);

glPopMatrix();
}

void drawLogo()
{
	glPushMatrix();
	glTranslatef(25,0,25);	
	glScalef(73,73,73);
	glRotatef(-90,0,1,0);
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D, texturelist[7]);

	   
glBegin(GL_QUADS);


		   glColor3f( 1, 1, 1  ); //PURPLE bottom
		   
		   glTexCoord2f(0.0, 0.0);
		   glVertex3f( -0.5, -0.5, 0.5);
       
		   glTexCoord2f(1.0, 0.0);
		   glVertex3f( 0.5, -0.5, 0.5);
       
		   glTexCoord2f(1.0, 1.0);
		   glVertex3f( 0.5, -0.5, -0.5);
       
		   glTexCoord2f(0.0, 1.0);
		   glVertex3f( -0.5, -0.5, -0.5);


	   glEnd();
       //left  ****************************

	   	   glDisable(GL_TEXTURE_2D);

glPopMatrix();
}

void drawBKG()
{
	
	glPushMatrix();
	glTranslatef(25,0,25);	
	glScalef(73,73,73);
	glRotatef(-90,0,1,0);
	glEnable(GL_TEXTURE_2D);
	
	glBindTexture(GL_TEXTURE_2D, texturelist[6]);

	   
glBegin(GL_QUADS);


		   glColor3f( 1, 1, 1  ); //PURPLE bottom
		   
		   glTexCoord2f(0.0, 0.0);
		   glVertex3f( -0.5, -0.5, 0.5);
       
		   glTexCoord2f(1.0, 0.0);
		   glVertex3f( 0.5, -0.5, 0.5);
       
		   glTexCoord2f(1.0, 1.0);
		   glVertex3f( 0.5, -0.5, -0.5);
       
		   glTexCoord2f(0.0, 1.0);
		   glVertex3f( -0.5, -0.5, -0.5);


	   glEnd();
       //left  ****************************

	   	   glDisable(GL_TEXTURE_2D);

glPopMatrix();
}

void endGame()
{
	
	glPushMatrix();
	glTranslatef(25,100.8,25);	
	glScalef(3,3,3);
	glRotatef(180,0,1,0);
	glEnable(GL_TEXTURE_2D);

if (P1Collision == true)
	glBindTexture(GL_TEXTURE_2D, texturelist[9]);
else if (P2Collision == true)
	glBindTexture(GL_TEXTURE_2D, texturelist[10]);	
else
	glBindTexture(GL_TEXTURE_2D, texturelist[11]);

	   
glBegin(GL_QUADS);


		   glColor3f( 1, 1, 1  ); //PURPLE bottom
		   
		   glTexCoord2f(0.0, 0.0);
		   glVertex3f( -0.5, -0.5, 0.5);
       
		   glTexCoord2f(1.0, 0.0);
		   glVertex3f( 0.5, -0.5, 0.5);
       
		   glTexCoord2f(1.0, 1.0);
		   glVertex3f( 0.5, -0.5, -0.5);
       
		   glTexCoord2f(0.0, 1.0);
		   glVertex3f( -0.5, -0.5, -0.5);


	   glEnd();
       //left  ****************************

	   	   glDisable(GL_TEXTURE_2D);

glPopMatrix();


}


//and we're done. thanks for coming. 
//and if you dropped something along the way...
//good luck trying to figure your way back in. 