/***********************************************************************************
         CTIS164 - Official Homework 1
====================================================================================
STUDENT : Mehmet Bulut 21903372
SECTION : 002
HOMEWORK : Official Homework 1 for 2021 Spring Semester (Submission: 1 March 2021)
====================================================================================
PROBLEMS :
NOT EXACTLY A PROBLEM RELATED TO REQUIREMENTS!!! 
Although after size increase it will be impossible to create overlapping cars, previous cars will overlap
====================================================================================
ADDITIONAL FEATURES :

1. direction of movement can be changed by pressing RIGHT and LEFT ARROW KEYS  =>goto=> onSpecialKeyDown() => //DIRECTION

2. velocity of the car can be increased up to 100 pixel per second [pixel/s] or or down to 1 pixel/s with UP and DOWN ARROW KEYS  =>goto=> onSpecialKeyDown() => //VELOCITY
2.1. Velocity displayed on the right down corner =>goto=> display()
2.2. Speed on/off displayed on the right down corner =>goto=> display() 

// 3. onTimer2() function created 
// 3. onTimer2() function created for the smoke effect  =>goto=> main() => //DISPLAY SMOKE => glutTimerFunc( smokeTime, onTimer2, 0 );

4. size of the car can be changed with W/w (to increase), S/s (to decrease) [limit is 20]
4.1. size displayed on the right down corner =>goto=> display()

5. RGB color values of the car can be changed with R, G, B (to increase), r, g, b (to dicrease), space to reset =>goto=> onKeyDown() => //RGB COLOR VALUES
5.1. RGB color values displayed on the right down corner =>goto=> display()

6. Amount of 10 subpart have been used (some shapes have been repeated)
***********************************************************************************/

#ifdef _MSC_VER
#define _CRT_SECURE_NO_WARNINGS
#endif
#include <GL/glut.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include <stdarg.h>

#define WINDOW_WIDTH  1400
#define WINDOW_HEIGHT 800

#define TIMER_PERIOD  16 // Period for the timer. 1000/16 = 60 fps
#define TIMER_ON      1 // 0:disable timer, 1:enable timer

#define D2R 0.0174532
#define CAR_COUNT 10

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

// MB Global Declarations
bool overlap = false;
bool start = false;

int x_onMove, y_onMove;
int x_onClick = 0, y_onClick = 0;
int cntClick = -1;
// Variables for ADDITIONAL FEATURES
bool smoke = false;
int smokeTime = 32;
bool speed = false;
int velocity = 1;
char direction = 'R';
int size = 10;
int r = 0, g = 0, b = 0;

typedef struct{
   int id;
   int x_car, y_car;
} car_t;

car_t car[CAR_COUNT];

void circle( int x, int y, int r )
{
// to draw circle, center at (x,y)
// radius r

#define PI 3.1415
   float angle ;
   glBegin( GL_POLYGON ) ;
   for ( int i = 0 ; i < 100 ; i++ )
   {
      angle = 2*PI*i/100;
      glVertex2f( x+r*cos(angle), y+r*sin(angle)) ;
   }
   glEnd();
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
   float angle;
   glLineWidth( 7 ); // change the thickness of circle_wire
   glBegin(GL_LINE_LOOP);
   for (int i = 0; i < 100; i++){
      angle = 2 * PI*i / 100;
      glVertex2f(x + r*cos(angle), y + r*sin(angle));
   }
   glEnd();
}

void print(int x, int y, const char *string, void *font )
{
   int len, i ;

   glRasterPos2f( x, y );
   len = (int) strlen( string );
   for ( i =0; i<len; i++ )
   {
      glutBitmapCharacter( font, string[i]);
   }
}

void vprint(int x, int y, void *font, const char *string , ...)
{
   // display text with variables.
   // vprint(-winWidth / 2 + 10, winHeight / 2 - 20, GLUT_BITMAP_8_BY_13, "ERROR: %d", numClicks);
   // OpenGL coordinate system

   va_list ap;
   va_start ( ap, string );
   char str[1024] ;
   vsprintf( str, string, ap ) ;
   va_end(ap) ;
   
   int len, i ;
   glRasterPos2f( x, y );
   len = (int) strlen( str );
   for ( i =0; i<len; i++ )
   {
      glutBitmapCharacter( font, str[i]);
   }
}

void vprint2(int x, int y, float size, const char *string, ...) 
{
   // vprint2(-50, 0, 0.35, "00:%02d", timeCounter);

   va_list ap;
   va_start(ap, string);
   char str[1024];
   vsprintf(str, string, ap);
   va_end(ap);
   glPushMatrix();
      glTranslatef(x, y, 0);
      glScalef(size, size, 1);
      
      int len, i;
      len = (int)strlen(str);
      for (i = 0; i<len; i++)
      {
         glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
      }
   glPopMatrix();
}

// DISPLAY THE CAR AS ONE PIECE
void MBdisplayCar()
{ 
   if(start)
      for(int i = 0; i <= cntClick && i < CAR_COUNT; i++)
      {
         // RECTANGLE (Bottom of the car)
         glColor3ub( 88 + r, 214 + g, 141 + b );
         glPointSize( 10 );
            glRectf( car[i].x_car - 10*size, car[i].y_car -2.5*size, car[i].x_car + 10*size, car[i].y_car + 2.5*size );

         // QUAD (Top of the cor)
         glColor3ub( 231 + r, 76 + g, 60 + b );
         glBegin( GL_QUADS );
            glVertex2f( car[i].x_car - 8*size, car[i].y_car + 2.5*size );
            glVertex2f( car[i].x_car - 6*size, car[i].y_car + 6*size );
            glVertex2f( car[i].x_car + 6*size, car[i].y_car + 6*size );
            glVertex2f( car[i].x_car + 8*size, car[i].y_car + 2.5*size );
         glEnd();

         // CIRCLE (tires)
         glColor3f(0, 0, 0);
            circle_wire( car[i].x_car + 7*size, car[i].y_car - 2.5*size, 2*size );
            circle_wire( car[i].x_car - 7*size, car[i].y_car - 2.5*size, 2*size );

           // LINES (inner lines for tires)
         glLineWidth(5);
         glColor3f(0, 0, 0);
         glBegin(GL_LINES);
            // line 1
            glVertex2f( car[i].x_car + 7*size, car[i].y_car - 0.5*size);
            glVertex2f( car[i].x_car + 7*size, car[i].y_car - 4.5*size);
            // line 2
            glVertex2f( car[i].x_car - 7*size, car[i].y_car - 0.5*size);
            glVertex2f( car[i].x_car - 7*size, car[i].y_car - 4.5*size);
            // line 3
            glVertex2f( car[i].x_car + 9*size, car[i].y_car - 2.5*size);
            glVertex2f( car[i].x_car + 5*size, car[i].y_car - 2.5*size);
            // line 4
            glVertex2f( car[i].x_car -5*size, car[i].y_car - 2.5*size);
            glVertex2f( car[i].x_car -9*size, car[i].y_car - 2.5*size);
         glEnd();
         
         // TRIANGLE STRIP (left windows of the car)
         glColor3ub(133, 193, 233);
         glBegin( GL_TRIANGLE_STRIP );
            glVertex2f( car[i].x_car - 6.5*size, car[i].y_car + 3*size ); // 0
            glVertex2f( car[i].x_car - 5*size, car[i].y_car + 5*size ); // 1
            glVertex2f( car[i].x_car - 3.5*size, car[i].y_car + 3*size ); // 2
            glVertex2f( car[i].x_car - 2*size, car[i].y_car + 5*size ); // 3
         glEnd();

         // right windows of the car
         glColor3ub(133, 193, 233);
         glBegin( GL_TRIANGLE_STRIP );
            glVertex2f( car[i].x_car + 3.0*size, car[i].y_car + 3.0*size ); // 0
            glVertex2f( car[i].x_car + 1.5*size, car[i].y_car + 5.0*size ); // 1
            glVertex2f( car[i].x_car + 6.0*size, car[i].y_car + 3.0*size ); // 2
            glVertex2f( car[i].x_car + 4.5*size, car[i].y_car + 5.0*size ); // 3
         glEnd();

         // lines for left glasses
         glLineWidth(1);
         glColor3f(1, 1, 1);
         glBegin(GL_LINES);
            glVertex2f( car[i].x_car - 5.0*size, car[i].y_car + 5.0*size ); // 1
            glVertex2f( car[i].x_car - 3.5*size, car[i].y_car + 3.0*size ); // 2
         glEnd();

         // lines for right glasses
         glLineWidth(1);
         glColor3f(1, 1, 1);
         glBegin(GL_LINES);
            glVertex2f( car[i].x_car + 3.0*size, car[i].y_car + 3.0*size ); // 0
            glVertex2f( car[i].x_car + 4.5*size, car[i].y_car + 5.0*size ); // 3
         glEnd();

         // LINE STRIP (where the smoke goes out)
         glColor3ub(135, 54, 0);
         glLineWidth(5);
         glBegin( GL_LINE_STRIP );
            glVertex2f( car[i].x_car - 10.0*size, car[i].y_car -1.5*size);
            glVertex2f( car[i].x_car - 11.5*size, car[i].y_car -1.5*size);
            glVertex2f( car[i].x_car - 12.0*size, car[i].y_car -2.5*size);
         glEnd();

         // TRIANGLES (place for ID)
         glColor3ub( 52 + r, 152 + g, 219 + b);
         glBegin( GL_TRIANGLES );
            glVertex2f( car[i].x_car, car[i].y_car + 1*size );
            glVertex2f( car[i].x_car - 3*size, car[i].y_car - 1.4*size );
            glVertex2f( car[i].x_car + 3*size, car[i].y_car -1.4*size );
         glEnd();

         // CIRCLE (for headlight of the car)
         glColor3f(1, 1, 1);
         circle(car[i].x_car + 8.5*size, car[i].y_car + 1*size, 1*size);

         // POINT (for the middle part of headlight of the car)
         glColor3f(0.7, 0.7, 0);
         glPointSize(5);
         glBegin( GL_POINTS );
            glVertex2f(car[i].x_car + 8.5*size, car[i].y_car + 1*size);
         glEnd();

         // ID of the car
         glColor3ub( 0 + r, 0 + g, 0 + b);
         vprint( car[i].x_car-5, car[i].y_car - 10, GLUT_BITMAP_8_BY_13, "%d" , car[i].id + 1);

         // POLYGON (transparent flashing smoke of the car)
         if(smoke)
         {
         glColor4f( 1, 1, 1 , 0.5);
         glBegin( GL_POLYGON );
            glVertex2f( car[i].x_car - 12*size, car[i].y_car - 3*size );
            glVertex2f( car[i].x_car - 15*size, car[i].y_car - 3*size ); 
            glVertex2f( car[i].x_car - 16*size, car[i].y_car - 2*size ); 
            glVertex2f( car[i].x_car - 18*size, car[i].y_car - 1.5*size ); 
            glVertex2f( car[i].x_car - 17*size, car[i].y_car - 0.5*size );
            glVertex2f( car[i].x_car - 17*size, car[i].y_car + 2*size ); 
            glVertex2f( car[i].x_car - 15*size, car[i].y_car + 1*size );  
         glEnd();
         glColor3f( 0, 0, 0);
         }
      }
}

void display() 
{
// CALLER FUNCTION: glutDisplayFunc(display);
   glClearColor( 0.4, 0.4, 0.4, 0 ); 
   glClear( GL_COLOR_BUFFER_BIT );

   //===================================================================
   //==============================MB_CODE==============================

   MBdisplayCar();
   // Display Name, Surname and ID of Student
   glColor4f( 0, 0, 0, 0.5 );
   glRectf(WINDOW_WIDTH / 2 - 80 - WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 + 20, WINDOW_WIDTH / 2 + 170 - WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 50);
   glColor3f( 1, 1, 1 );
   vprint( WINDOW_WIDTH / 2 - 30 - WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 20, GLUT_BITMAP_TIMES_ROMAN_24,"Mehmet Bulut");
   vprint( WINDOW_WIDTH / 2 - 30 - WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - 40, GLUT_BITMAP_TIMES_ROMAN_24,"   21903372");
   // MOUSE POSITION
   glColor3f( 1, 1, 1 );
   vprint( WINDOW_WIDTH - 180 - WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - WINDOW_HEIGHT + 30, GLUT_BITMAP_8_BY_13,"( x: %d", x_onMove);
   vprint( WINDOW_WIDTH - 90 - WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - WINDOW_HEIGHT + 30, GLUT_BITMAP_8_BY_13,"y: %d )", y_onMove);

   // VELOCITY POSITION
   glColor3f( 1, 1, 0 );
   vprint( WINDOW_WIDTH - 135 - WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - WINDOW_HEIGHT + 45, GLUT_BITMAP_8_BY_13, "pixel/s: %d", velocity);

   // SPEED ON/OFF DISPLAY
   glColor3f( 1, 0.5, 0.5);
   if(speed)
      vprint( WINDOW_WIDTH - 135 - WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - WINDOW_HEIGHT + 60, GLUT_BITMAP_8_BY_13,"speed: ON");
   else
      vprint( WINDOW_WIDTH - 135 - WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - WINDOW_HEIGHT + 60, GLUT_BITMAP_8_BY_13,"speed: OFF");

   // SIZE OF THE CAR
   glColor3f( 1, 0.5, 1);
   vprint( WINDOW_WIDTH - 135 - WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - WINDOW_HEIGHT + 75, GLUT_BITMAP_8_BY_13, "size: %d", size);

   // COLOR OF THE CAR
   glColor3f( 1, 0, 0);
   vprint( WINDOW_WIDTH - 200 - WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - WINDOW_HEIGHT + 90, GLUT_BITMAP_8_BY_13, "r: %d ", r);
   glColor3f( 0, 1, 0);
   vprint( WINDOW_WIDTH - 130 - WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - WINDOW_HEIGHT + 90, GLUT_BITMAP_8_BY_13, "g: %d ", g);
   glColor3f( 0, 0, 1);
   vprint( WINDOW_WIDTH - 60 - WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - WINDOW_HEIGHT + 90, GLUT_BITMAP_8_BY_13, "b: %d ", b);
   //===================================================================

   glutSwapBuffers();
}

void onKeyDown(unsigned char key, int x, int y )
{
/* 
CALLER FUNCTION: glutKeyboardFunc(onKeyDown);
FUNCTION PROPERTIES: onKeyDown()
// The x and y callback parameters indicate the mouse location in window relative coordinates when the key was pressed.
// key: integer value of ASCII characters like ESC, a,b,c..,A,B,..Z etc.
*/
   // exit when ESC is pressed.
   if ( key == 27 )
      exit(0);

   //===================================================================
   //==============================MB_CODE==============================

   // DIRECTION
   if ( key == 'w' || key == 'W')
   {
      if(size < 20)
      size++;
   }
   else if ( key == 's' || key == 'S')
      if (size > 5)
      size--;
   
   // RGB COLOR VALUES
   switch(key)
   {
      case 'r': --(r += 256) %= 256; break;
      case 'R': ++r %= 256; break;
      case 'g': --(g += 256) %= 256; break;
      case 'G': ++g %= 256; break;
      case 'b': --(b += 256) %= 256; break;
      case 'B': ++b %= 256; break;
      case ' ': r = g = b = 0; break;
   }
   //===================================================================

   // to refresh the window it calls display() function
   glutPostRedisplay() ;
}

void onKeyUp(unsigned char key, int x, int y )
{
/*
CALLER FUNCTION: glutKeyboardUpFunc(onKeyUp);
FUNCTION PROPERTIES: onKeyUp()
// The x and y callback parameters indicate the mouse location in window relative coordinates when the key was pressed.
// key: integer value of ASCII characters like ESC, a,b,c..,A,B,..Z etc.
// if exit() function executed, after a key pressed assumed released happened inevitably 
*/
   // exit when ESC is pressed.
   if ( key == 27 )
      exit(0);  
   // to refresh the window it calls display() function
   glutPostRedisplay() ;
}

void onSpecialKeyDown( int key, int x, int y )
{
/*
CALLER FUNCTION: glutSpecialFunc(onSpecialKeyDown);
   FUNCTION DEFINITION: onSpecialKeyDown()
// The x and y callback parameters indicate the mouse location in window 
      relative coordinates when the key was pressed.
// key:
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
*/
   // To on/off speed
   if(key == GLUT_KEY_F1)
      speed = !speed;

   //===================================================================
   //==============================MB_CODE==============================

   // DIRECTION
   if ( key == GLUT_KEY_RIGHT)
      direction = 'R';
   else if ( key == GLUT_KEY_LEFT)
      direction = 'L';

   // VELOCITY
   if ( key == GLUT_KEY_UP) {  
      if(velocity < 100)
         velocity+=1;
   }
   else if ( key == GLUT_KEY_DOWN)
      if(velocity > 1)
         velocity-=1;

   //===================================================================

   switch (key) {
      case GLUT_KEY_UP: up = true; break;
      case GLUT_KEY_DOWN: down = true; break;
      case GLUT_KEY_LEFT: left = true; break;
      case GLUT_KEY_RIGHT: right = true; break;
   }
   // to refresh the window it calls display() function
   glutPostRedisplay() ;
}

void onSpecialKeyUp( int key, int x, int y )
{
/*
CALLER FUNCTION: glutSpecialUpFunc(onSpecialKeyUp);
FUNCTION DEFINITION: onSpecialKeyUp()
// The x and y callback parameters indicate the mouse location in window relative coordinates when the key was pressed.
//// key: 
// Special Key like GLUT_KEY_F1, F2, F3,...
// Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
FUNCTION PROPERTIES: onSpecialKeyUp()
// if exit() function executed, after a key pressed assumed released happened inevitably
*/
   switch (key) {
   case GLUT_KEY_UP: up = false; break;
   case GLUT_KEY_DOWN: down = false; break;
   case GLUT_KEY_LEFT: left = false; break;
   case GLUT_KEY_RIGHT: right = false; break;
   }
   // to refresh the window it calls display() function
   glutPostRedisplay() ;
}

void onClick( int button, int state, int x, int y )
{
// CALLER FUNCTION: glutMouseFunc(onClick);
// button(s) == GLUT_LEFT_BUTTON , GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON
// state(s)  == GLUT_UP , GLUT_DOWN [are press and release cycle]
// x, y  is the coordinate of the point that mouse clicked.

   //===================================================================
   //==============================MB_CODE==============================

   // Callbacks of x and y GLUT coordinates have been converted to OpenGL and globalized.
   if ( button == GLUT_LEFT_BUTTON  && state == GLUT_DOWN) 
   {  
      start = true;
      int x_local = x - WINDOW_WIDTH / 2, y_local = WINDOW_HEIGHT / 2 - y;

      // to not create overlapping car
      for (int i = 0; i <= cntClick; i++)
         if(!(y_local >= car[i].y_car + 10*size || y_local <= car[i].y_car - 10*size || x_local >= car[i].x_car + 24*size || x_local <= car[i].x_car - 24*size))
            overlap = true;
      
      // initialize the location of each car until limit CAR_COUNT
      if(cntClick < CAR_COUNT && !overlap)
      {
         x_onClick = x - WINDOW_WIDTH / 2; 
         y_onClick = WINDOW_HEIGHT / 2 - y;
         cntClick ++;
         car[cntClick].x_car = x_onClick;
         car[cntClick].y_car = y_onClick;
         car[cntClick].id = cntClick;
         //bubbleSort(car, CAR_COUNT);
      }
      overlap = false;
   }
   //===================================================================
   // to refresh the window it calls display() function
    glutPostRedisplay() ;
}

void onResize( int w, int h )
{
/*
CALLER FUNCTION: glutReshapeFunc(onResize);
FUNCTION DEFINITION: onResize()
// This function is called when the window size changes.
FUNCTION PROPERTIES: onResize()
// w : is the new width of the window in pixels.
// h : is the new height of the window in pixels
*/
   winWidth = w;
   winHeight = h;
   glViewport( 0, 0, w, h ) ;
   glMatrixMode( GL_PROJECTION );
   glLoadIdentity();
   glOrtho( -w/2, w/2, -h/2, h/2, -1, 1);
   glMatrixMode( GL_MODELVIEW);
   glLoadIdentity();
   display(); // refresh window.
}

void onMoveDown( int x, int y ) 
{
/*
CALLER FUNCTION: glutMotionFunc()
FUNCTION DEFINITION: glutMotionFunc()
// set the motion callbacks for the current window.
// When the user moves the mouse pointer while any of the mouse button is down.
*/
   // =====================================================
   // =============== WRITE YOUR CODES HERE ===============
   // =====================================================
   // to refresh the window it calls display() function   
   glutPostRedisplay() ;
}

void onMove( int x, int y ) 
{
// CALLER FUNCTION: glutPassiveMotionFunc()

   //===================================================================
   //==============================MB_CODE==============================
   
   // Callbacks of x and y GLUT coordinates have been converted to OpenGL and globalized.
   x_onMove = x - WINDOW_WIDTH / 2; 
   y_onMove = WINDOW_HEIGHT / 2 - y;

   //===================================================================

   glutPostRedisplay() ; // to refresh the window it calls display() function
}

#if TIMER_ON == 1
void onTimer( int v ) 
{
   glutTimerFunc( TIMER_PERIOD, onTimer, 0 );

   //===================================================================
   //==============================MB_CODE==============================

   switch(direction)
   {
      case 'R': // default when program starts
         if(speed && start)
            for(int i = 0; i <= cntClick && i < CAR_COUNT; i++)
               if(car[i].x_car >= WINDOW_WIDTH / 2+120)
                  car[i].x_car = -WINDOW_WIDTH / 2-120;
               else
                  car[i].x_car += velocity;
         break;
      case 'L': 
         if(speed && start)
            for(int i = 0; i <= cntClick && i < CAR_COUNT; i++)
               if(car[i].x_car <= -WINDOW_WIDTH / 2-120)
                  car[i].x_car = WINDOW_WIDTH / 2+120;
               else
                  car[i].x_car -= velocity;
         break;
   }
   //===================================================================

   glutPostRedisplay(); // to refresh the window it calls display() function
}
#endif

void onTimer2(int v)
{
   glutTimerFunc( smokeTime, onTimer2, 0 );
   smoke = !smoke;
   glutPostRedisplay(); // to refresh the window it calls display() function
}

void Init() 
{
   // Smoothing shapes
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

int main( int argc, char *argv[] ) {
                        //               //
                        // CREATE WINDOW //
                        //               //
   glutInit(&argc, argv );
   glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );
   glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
   //glutInitWindowPosition(100, 100);

   // Enter name of the Window as string such as "Template File"
   glutCreateWindow("Official Homework 1");

                        //               //
                        // PAINT/DISPLAY //
                        //               //
   // sets the display callback for the current window. [Paint/Display Event]
   glutDisplayFunc(display);

   // sets the reshape callback for the current window. [Resize Event]
   glutReshapeFunc(onResize);

                     //                       //
                     // KEYBOARD REGISTRATION //
                     //                       //
   // If a user presses, releases any key on the keyboard respectively that has an ASCII code such as letters, numbers,space bar, Tab, Enter key, Escape key. [ASCII(printable) Keyboard Events]
   glutKeyboardFunc(onKeyDown); glutKeyboardUpFunc(onKeyUp);

   // F1, F2, Arrow keys are not ASCII keys. They are treated specially. To detect the key presses and releases for those special(control) keys. [Special(control) Keyboard Events]
   glutSpecialFunc(onSpecialKeyDown); glutSpecialUpFunc(onSpecialKeyUp);

                     //                    //
                     // MOUSE REGISTRATION //
                     //                    //
   // sets the mouse callback for the current window. [Mouse Click Event]
   glutMouseFunc(onClick);

   // When the user moves the mouse pointer within the window. Each movement results in a new event. [Mouse Passive Move Event]
   glutPassiveMotionFunc(onMove);

   // When the user moves the mouse pointer while any of the mouse button is down. [Mouse Motion Event]
   glutMotionFunc(onMoveDown);

   #if  TIMER_ON == 1
   // timer event
   glutTimerFunc( TIMER_PERIOD, onTimer, 0 );
   #endif

   // DISPLAY SMOKE
   glutTimerFunc( smokeTime, onTimer2, 0 );
   
   Init();
   
   // Starts Event Dispatcher
   glutMainLoop();
}