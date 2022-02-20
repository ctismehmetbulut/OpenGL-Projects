/***********************************************************************************
         CTIS164 - Official Homework 4
====================================================================================
STUDENT : Mehmet Bulut 21903372
SECTION : 002
HOMEWORK : Official Homework 4 for 2021 Spring Semester (Submission: 10 May 2021)
====================================================================================
PROBLEMS :
Some minor delays when bullets hit the light balls 
====================================================================================
ADDITIONAL FEATURES :
1- Light balls reflects from the surfaces even from the sloped interfaces!!!
2- Three 3 light ball and more limitless bullet exist owing to linked list implementation
3- Bullets optimizes their direction to the closest light balls
4- If the bullets are visible to the player, they will be slow down
5- Pause will pause/continue the flow of the program and player can move with the arrow keys 
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
#include <stdbool.h>
#include "vec.h"

#define WINDOW_WIDTH  1400
#define WINDOW_HEIGHT 800
#define TIMER_PERIOD  16 // Period for the timer.
#define TIMER_ON       1 // 0:disable timer, 1:enable timer

#define FOV  15    // Field of View Angle
#define D2R 0.0174532

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

typedef struct{
   double r, g, b;
} color_t;

typedef struct {
	vec_t pos;
	double angle;
   color_t color;
   vec_t velocity;
   double radius;
   double speed;
   bool visible;
} object_t;

typedef struct{
   vec_t pos;
   vec_t N;
} vertex_t;

object_t E = { {600, 200}, 270 };
object_t light = { {-600, 200}, 270, {1, 1, 1}, {2, 3} };
object_t tank = { {-600, 200}, 270, {1, 1, 1}, {2, 3} };
//object_t P = { {-600, 200}, 270, {1, 1, 1}, {2, 3} };

double speed_object = 5;
double speed_bullet = 5;

vec_t Start = {-800, -250.5}, End = {800, -100.5}; // SLOPE LINE SEGMENT

vec_t p_onClick = {0 , 0};

vec_t push_back;

int cntbullet = 0;

bool click = false, flow = true;

/*
typedef struct{
   vec_t pos;
   color_t color;
   vec_t velocity; // velocity of the light source
} light_t; */

//****************************LINKED_LIST_LIBRARY*************************************
typedef object_t LType;

typedef struct node_s
{
	LType data;
	struct node_s *next;
} node_t;

node_t *getnode(void)
{
	node_t *node;
	node = (node_t *)malloc(sizeof(node_t));
	node->next = NULL;
	return (node);
}

void addAfter(node_t *p, LType item)
{
	node_t *newp;
	newp = getnode();
	newp->data = item;
	newp->next = p->next;
	p->next = newp;
}

node_t *addBeginning(node_t *headp, LType item)
{
	node_t *newp;
	newp = getnode();
	newp->data = item;
	newp->next = headp;
	return (newp);
}

node_t *deleteFirst(node_t *headp, LType *item) 
{
	node_t *del = headp;
	*item = del->data;
	headp = del->next;
	free(del);
	return (headp);
}

void deleteAfter(node_t *p, LType *item)
{
	node_t *del;
	del = p->next;
	*item = del->data;
	p->next = del->next;
	free(del);
}

node_t *destroy(node_t *hp)
{
   node_t *del;
   while( hp != NULL)
   {
      del = hp;
      hp = hp->next;
      free(del);
   }
   return hp;
}

//function that displays the list
void displayList(node_t *hp)
{
   hp = hp->next;
	if (hp == NULL)
		printf("The List is EMPTY !!!\n");
	else
	{
		while (hp != NULL)
		{
			printf("x: %4.0f   y:%4.0f   angle: %4.0f   ->\n", hp->data.pos.x, hp->data.pos.y, hp->data.angle);
			hp = hp->next;
		}
		printf("NULL\n");
	}
}
//************************************************************************************

node_t *headlight = NULL, *headbullet = NULL;

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

void drawCircle(vec_t p, double radius, color_t color)
{
   double angle = PI/180;
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glColor3f( color.r, color.g, color.b );
   glBegin( GL_POLYGON );
   for(int t = 0; t <= 360; t+=5)
      glVertex2f(radius*cos(t*angle) + p.x, radius*sin(t*angle) + p.y);
   glEnd();
   glDisable(GL_BLEND);
}

void circle_wire(int x, int y, int r)
{
#define PI 3.1415
   float angle;
   glLineWidth(1); // change the thickness of circle_wire
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

// LIGHT SIMULATON FUNCTIONS

// when the distance is max, its impact is 0
// when the distance is min, its impact is 1
double distanceImpact(double d, double max)
{
   return (-1.0 / max) * d + 1;
}

color_t mulColor(double k, color_t c)
{
   //k = k/2 + 0.5;
   //printf("k: %.2f\n", k);
   return { k * c.r, k * c.g, k * c.b };
}

color_t calculateColor(object_t L, vertex_t B, double max)
{
   vec_t BL = subV(L.pos, B.pos);
   double factor = dotP(unitV(BL), B.N) * distanceImpact(magV(BL), max);
   return mulColor(factor, L.color);
}

color_t addColor(color_t c1, color_t c2) {
   return { c1.r + c2.r, c1.g + c2.g, c1.b + c2.b };
}
/*
// helps us draw the shapes more easily by creating an abstraction layer
// it needs "p.position" and "p.angle" inside the "p"
vec_t posfind(object_t p, vec_t position)
{
   vec_t prev = position;
   position.x = fabs(position.x);
   position.y = fabs(position.y);

   vec_t vRight = addV(p.pos, pol2rec({ position.x, p.angle - 90 }));
   vec_t vLeft = addV(p.pos, pol2rec({ position.x, p.angle + 90 }));

   vec_t vUpRight = addV(vRight, pol2rec({ position.y, p.angle }));
   vec_t vUpLeft = addV(vLeft, pol2rec({ position.y, p.angle }));

   vec_t vDownRight = addV(vRight, pol2rec({ position.y, p.angle + 180 }));
   vec_t vDownLeft = addV(vLeft, pol2rec({ position.y, p.angle + 180 }));

   position = prev;

   if( position.y > 0 )
   {
      if( position.x > 0 )
         return vUpRight;
      else
         return vUpLeft;
   }
   else
   {
      if( position.x > 0 )
         return vDownRight;
      else
         return vDownLeft;
   }
}
*/
void vertex(vec_t vector)
{
   polar_t polar = rec2pol(vector);

   glVertex2f(vector.x, vector.y);
}

vec_t vertex(vec_t P, vec_t Tr, double angle)
{
   vec_t result;
   angle *= D2R;
   result.x = (P.x * cos(angle) - P.y * sin(angle)) + Tr.x;
   result.y = (P.x * sin(angle) + P.y * cos(angle)) + Tr.y;
   glVertex2d(result.x, result.y);
   return result;
}

void drawObject(object_t P)
{
   //vertex(posfind(P, {0*c, 0*c}));
   //vertex(posfind(P, {0*c, 2*c}));
   double c = 30; // coefficient

	drawCircle(P.pos, P.radius, P.color);
}

void drawPlayer(object_t P)
{
   //vertex(posfind(P, {0*c, 0*c}));
   //vertex(posfind(P, {0*c, 2*c}));
   double c = 30; // coefficient

   glColor3f(P.color.r, P.color.g, P.color.b);
   glBegin(GL_LINES);
   vertex({0*c, 0*c}, P.pos, P.angle);
   vertex({2*c, 0*c}, P.pos, P.angle);
	glEnd();

	drawCircle(P.pos, 20, P.color);

   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
      vec_t E1 = addV(P.pos, pol2rec({ 1612, P.angle - FOV }));
      vec_t E2 = addV(P.pos, pol2rec({ 1612, P.angle + FOV}));
      // Field of View Area
      glColor4f(1, 1, 1, 0.3);
      glBegin(GL_TRIANGLES);
      glVertex2f(P.pos.x, P.pos.y);
      glVertex2f(E1.x, E1.y);
      glVertex2f(E2.x, E2.y);
      glEnd();
   glDisable(GL_BLEND);
}

void drawBullet(object_t B)
{
   vec_t pos = B.pos;
   double angle = B.angle;

	glColor3f(1, 1, 1);
	//circle_wire(B.pos.x, B.pos.y, 10);

   double c = 10; // coefficient

   glColor3f(0.3, 0.3, 0.3);
   glBegin(GL_TRIANGLES);
   vertex({0*c, 0*c}, pos, angle);
   vertex({1*c, -2*c}, pos, angle);
   vertex({-1*c, -2*c}, pos, angle);
	glEnd();

   glColor3f(1, 1, 0);
   glBegin(GL_QUADS);
   vertex({-1*c, -2*c}, pos, angle);
   vertex({-1*c, -6*c}, pos, angle);
   vertex({1*c, -6*c}, pos, angle);
   vertex({1*c, -2*c}, pos, angle);
	glEnd();

   //printf("angle of bullet: %.2f\n", angle);

	// if player sees the enemy, its color is green.
	//if (E.visible) glColor3f(0, 1, 0);
	//else glColor3f(1, 0, 0);
}

// takes angle in radian
vec_t rotateV(vec_t v, double rotate_angle)
{
   rotate_angle /= D2R;
   return pol2rec( {magV(v), angleV(v) + rotate_angle} );
}

void illumination(object_t source)
{
   double y = -250;
   y -= 300.0/600.0;

   for(double x = -800; x <= 800; x++) // 1600 times loop
   {
      vertex_t ver = { {x, y}, rotateV({ 0, 1 }, atan2(150, 1600)) };
      //color_t res = calculateColor(source, ver, 300); // 800

      node_t *addr = headlight; //
      color_t res = { 0, 0, 0 }; //
      while( addr->next != NULL ) //
      {
         res = addColor( res, calculateColor(addr->next->data, ver, 300) ); //
         addr = addr->next;
      }
      //res = addColor( res, calculateColor(tank, ver, 300) ); //

      vec_t S = {x, y}, E = {x, y -150};
      vec_t SE = subV(E, S);
      vec_t newSE = rotateV( SE, atan2(150, 1600) ); // atan2(150, 600)
      vec_t newE = addV(newSE, S);

      //if(res.r > 0)
      glLineWidth(2);
      glBegin(GL_LINES);
      glColor3f(res.r, res.g, res.b);
      //printf("x: %.2f, r: %.2f\n", x, res.r); // , g: %.2f, b: %.2f
         glVertex2f(S.x, S.y);
      glColor3f(0, 0, 0);
         glVertex2f(newE.x, newE.y); // x, y - 150
      glEnd();

      y += 150.0/1600.0;
   //printf("x: %.2f, y: %.2f\n", x, y);
/*
      printf("angleV(SE): %.2f, angleV(newSE): %2.f ::: angleBetween SE_newSE: %.2f\n", angleV(SE), angleV(newSE), angleBetween2V(SE, newSE));
      printf("atan2: %.2f\n", atan2(150, 600) / D2R);

      printf("======================================\n");
      printf("S: x: %.2f y: %.2f\n", S.x, S.y);
      printf("E: x: %.2f y: %.2f\n", E.x, E.y);
      printf("newE: x: %.2f y: %.2f\n", newE.x, newE.y);
      printf("____________________________________________\n\n");
*/
   }
   //vec_t v = rotateV({0, 1}, atan2(6, 8));
   //printf("x: %.2f, y: %.2f\n", v.x, v.y);
   //printf("angleV(v): %.2f, magV(v): %.2f\n",  angleV(v), magV(v));
}

// finds the shortest distance from M to the line segment passing through Start and End
double shortestDistance (vec_t M, vec_t START, vec_t END)
{
   M.x;
   M.y;
   double x1 = START.x;
   double y1 = START.y;
   double x2 = END.x;
   double y2 = END.y;
   double Dx = x2 - x1;
   double Dy = y2 - y1;

   double t = Dy * (M.y-y1) + Dx * (M.x-x1);

   if( Dx != 0 && Dy != 0 )
      t /= ( Dx*Dx + Dy*Dy );

   vec_t K = { x1+Dx*t, y1+Dy*t };

   return magV(subV(K, M));
}

void display()
{
   //CALLER FUNCTION: glutDisplayFunc(display);
   glClearColor( 0, 0 , 0 , 0 );
   glClear( GL_COLOR_BUFFER_BIT );

   glColor3f(0.5, 0.5, 1);
   glRectf(-700, -250, 700, 400);

   drawPlayer(tank);

   glColor3f(0, 0, 0);
   glRectf(-800, -400, 700, -250);

   node_t *addr = headlight;
   
   while( addr->next != NULL)
   {
      drawObject(addr->next->data);
      addr = addr->next;
   }

   illumination(headlight->next->data);

   node_t *addr2 = headbullet;
   
   while( addr2->next != NULL)
   {
      drawBullet(addr2->next->data);
      addr2 = addr2->next;
   }
   
   glColor3f(1, 1, 0);
   vprint( 0, -300, GLUT_BITMAP_TIMES_ROMAN_24,"PRESS SPACE TO PAUSE/CONTINUE... Arrow Keyse to Move the Player");

   glutSwapBuffers();
}

void onKeyDown(unsigned char key, int x, int y )
{
/*
   CALLER FUNCTION: glutKeyboardFunc(onKeyDown);

   FUNCTION PROPERTIES: onKeyDown()
   // The x and y callback parameters indicate the mouse location in window
      relative coordinates when the key was pressed.
   // key: integer value of ASCII characters like ESC, a,b,c..,A,B,..Z etc.
*/
   // exit when ESC is pressed.
   if ( key == 27 )
      exit(0);
   
   if ( key == ' ' )
      flow = !flow;


   // to refresh the window it calls display() function
   glutPostRedisplay() ;
}

void onKeyUp(unsigned char key, int x, int y )
{
   /*
   CALLER FUNCTION: glutKeyboardUpFunc(onKeyUp);

   FUNCTION PROPERTIES: onKeyUp()
   // The x and y callback parameters indicate the mouse location in window
      relative coordinates when the key was pressed.
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
   // =====================================================
   // =============== WRITE YOUR CODES HERE ===============
   // =====================================================
   switch (key) {
   case GLUT_KEY_UP: up = true; break;
   case GLUT_KEY_DOWN: down = true; break;
   case GLUT_KEY_LEFT: left = true; break;
   case GLUT_KEY_RIGHT: right = true; break;
   }

   // to refresh the window it calls display() function
   //glutPostRedisplay() ;
}

void onSpecialKeyUp( int key, int x, int y )
{
/*
   CALLER FUNCTION: glutSpecialUpFunc(onSpecialKeyUp);

   FUNCTION DEFINITION: onSpecialKeyUp()
   // The x and y callback parameters indicate the mouse location in window
      relative coordinates when the key was pressed.
   // key:
   // Special Key like GLUT_KEY_F1, F2, F3,...
   // Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT

   FUNCTION PROPERTIES: onSpecialKeyUp()
   // if exit() function executed, after a key pressed assumed released happened inevitably

*/
   // =====================================================
   // =============== WRITE YOUR CODES HERE ===============
   // =====================================================
   switch (key) {
   case GLUT_KEY_UP: up = false; break;
   case GLUT_KEY_DOWN: down = false; break;
   case GLUT_KEY_LEFT: left = false; break;
   case GLUT_KEY_RIGHT: right = false; break;
   }

   // to refresh the window it calls display() function
   //glutPostRedisplay() ;
}

void onClick( int button, int state, int x, int y )
{
/*
   CALLER FUNCTION: glutMouseFunc(onClick);

   FUNCTION DEFINITION: onClick()
   // When a click occurs in the window, it provides which button

   FUNCTION PROPERTIES: onClick()
   // button(s) == GLUT_LEFT_BUTTON , GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON
   // state(s)  == GLUT_UP , GLUT_DOWN [are press and release cycle]
   // x, y  is the coordinate of the point that mouse clicked.

   SYNTAX:
   void onClick(int button, int state, int x, int y) { };
*/
   p_onClick.x = x - WINDOW_WIDTH / 2;
   p_onClick.y = WINDOW_HEIGHT / 2 - y;

   if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && flow)
      click = true;
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

   FUNCTION PROPERTIES: applicable for all glut functions
   // GLUT to OpenGL coordinate conversion:
   //  x - winWidth / 2 = x_openGL
   //  winHeight / 2 - y = y_openGL
*/
   // =====================================================
   // =============== WRITE YOUR CODES HERE ===============
   // =====================================================

   // to refresh the window it calls display() function
   //glutPostRedisplay() ;
}

void onMove( int x, int y )
{
/*
   CALLER FUNCTION: glutPassiveMotionFunc()

   FUNCTION DEFINITION: glutPassiveMotionFunc()
   // set the passive motion callbacks for the current window.
   // When the user moves the mouse pointer within the window. Each movement results in a new event.

   FUNCTION PROPERTIES: applicable for all glut functions
   // GLUT to OpenGL coordinate conversion:
   //  x - winWidth / 2 = x_openGL
   //  winHeight / 2 - y = y_openGL
*/

   // =====================================================
   // =============== WRITE YOUR CODES HERE ===============
   // =====================================================

   // to refresh the window it calls display() function
   //glutPostRedisplay() ;
}

void turn(double *angle, double turn)
{
   *angle += turn;
   if (*angle < 0)
      (*angle) += 360;
   if (*angle >= 360)
      (*angle) -= 360;
   //printf("rotate angle: %0.2f\n", *angle);
}

// p1 and p2 is a line. Likewise p3 and p4 is a line
bool intersect(vec_t p1, vec_t p2, vec_t p3, vec_t p4)
{
   double t1_numerator = (p4.x-p3.x)*(p3.y-p1.y) - (p4.y-p3.y) * (p3.x-p1.x);
   double t2_numerator = (p2.x-p1.x)*(p3.y-p1.y) - (p2.y-p1.y) * (p3.x-p1.x);
   double t_denominator = (p4.x-p3.x)*(p2.y-p1.y) - (p4.y-p3.y) * (p2.x-p1.x);

   double t1 = t1_numerator / t_denominator;
   double t2 = t2_numerator / t_denominator;

   if( 0.0 <= t1 && t1 <= 1.0 && 0.0 <= t2 && t2 <= 1.0 )
      return true;
   else
      return false;
}

bool collision(object_t source)
{
   double radius = 20;
   int t_diff = 5;
   vec_t *circle_points = (vec_t *)malloc(sizeof(vec_t)*(360/t_diff));

   bool flag = false;
   double angle = PI/180;
   int m = 0;
   for(int t = 0; t < 360; t+= t_diff)
   {
      circle_points[m].x = (radius+1)*cos(t*angle) + source.pos.x;
      circle_points[m].y = (radius+1)*sin(t*angle) + source.pos.y;
      ++m;
   }
   int u;
   for(int n = 0; n < 360/t_diff; n++)
   {
      u = (n+1)%(360/t_diff);
      if( intersect(circle_points[n], circle_points[u], Start, End) )
      flag = true;
   }
   free(circle_points);
   return flag;
}

bool collide2Circle(object_t c1, object_t c2)
{
   //if(!( c1.pos.x == c2.pos.x && c1.pos.y == c2.pos.y ))
   {
      if( 1 < magV( subV(c1.pos, c2.pos) ) && magV( subV(c1.pos, c2.pos) ) <= 40)
         return true;
      else
         return false;
   }
   //return false;
}
/*
void elasticCollision(object_t *c1, object_t *c2)
{
   vec_t x1 = (*c1).pos;
   vec_t x2 = (*c2).pos;
   
   vec_t v1 = (*c1).velocity;
   vec_t v2 = (*c2).velocity;

   double m1 = (*c1).radius;
   double m2 = (*c2).radius;

   (*c1).velocity = subV(   v1, mulV(  ( 2*m2/(m1+m2) ) * (dotP(subV(v1, v2), subV(x1, x2)) / (magV(subV(x1, x2)) * magV(subV(x1, x2))) ) , subV(x1, x2)  )   );

   (*c2).velocity = subV(   v2, mulV(  ( 2*m1/(m1+m2) ) * (dotP(subV(v2, v1), subV(x2, x1)) / (magV(subV(x2, x1)) * magV(subV(x2, x1))) ) , subV(x2, x1)  )   );

   //(*c1).velocity = mulV(-1, (*c1).velocity); 

   //(*c2).velocity = mulV(-1, (*c2).velocity); 
}
*/
// if one bullet and one light source collided remove bullet
void collisionLightAndBullet(node_t **head_bullet, node_t **head_light, int *cnt)
{
   object_t deleted_bullet, deleted_light;
   node_t *addr = (*head_bullet);
   while(addr->next != NULL) // IF BULLET IS NOT FINISHED
   {
      //======================LIGHT_LOOP======================================================//
      node_t *addr2 = (*head_light), *closest = (*head_light);
      double min = magV(subV(addr->next->data.pos, addr2->next->data.pos));
      
      while(addr->next != NULL && addr2->next != NULL) // IF BULLET AND LIGHT IS NOT FINISHED
      {
         if( magV(subV(addr->next->data.pos, addr2->next->data.pos)) < min )
         {
            min = magV(subV(addr->next->data.pos, addr2->next->data.pos));
            closest = addr2;
         }
         addr2 = addr2->next;
      }
      //======================LIGHT_LOOP======================================================//

               //IF COLLIDE
      if( magV(subV(addr->next->data.pos, closest->next->data.pos)) < 1 )
      {
         //DELETE CURRENT_BULLET
         deleteAfter(addr, &deleted_bullet);
         (*cnt)--;
      }
      else if( magV(subV(addr->next->data.pos, closest->next->data.pos)) < 2 )
      {
         //DELETE CURRENT_BULLET
         deleteAfter(addr, &deleted_bullet);
         (*cnt)--;
      }
      else
      {
         // velocity vector from Enemy to Player
         addr->next->data.velocity = mulV( addr->next->data.speed, unitV(subV(closest->next->data.pos, addr->next->data.pos)));

                  // VISIBILITY PROBLEM
         // Check if the Player sees the Enemy, (visibility)
         vec_t V = pol2rec({ 1, tank.angle });
         vec_t W = unitV(  subV(addr->next->data.pos, tank.pos) );
         addr->next->data.visible = dotP(V, W) > cos( FOV * D2R);  // E.vis:true , P sees E.
         // IF SEE SLOW DOWN THE BULLETS
         if(addr->next->data.visible == true)
            addr->next->data.velocity = mulV(1.0/4, addr->next->data.velocity);
         
         addr->next->data.pos = addV(addr->next->data.pos, addr->next->data.velocity); // chase

         addr->next->data.angle = rec2pol(unitV(subV(closest->next->data.pos, addr->next->data.pos))).angle - 90; // rotate according to light

         addr2 = addr2->next;
      }

      // IF BULLET IS FINISHED
      if(addr->next != NULL)
         addr = addr->next;
   }
}

// reflect the light sources from the surfaces, walls etc.
void reflectMoveLight(node_t **head_light)
{
   object_t deleted_light;
   node_t *addr = (*head_light);
   while(addr->next != NULL) // IF LIGHT IS NOT FINISHED
   {  
     // move light with its velocity
      addr->next->data.pos = addV( addr->next->data.pos, addr->next->data.velocity);

                                       // REFLECTION

      ///////////////////////////COLLISION_RESOLUTION/////////////////////
      vec_t Normal = rotateV({ 0, 1 }, atan2(150, 1600));
      double sD = shortestDistance(addr->next->data.pos, Start, End);
      if( 0 < sD && sD < 20 )
      {
         addr->next->data.pos = addV( addr->next->data.pos, mulV(20-sD, Normal));
         //push_back = addV( addr->next->data.pos, mulV(10, Normal));
         //printf("FIRST:: sD: %.2f\n", sD);
      }
      else if( 19.5 < sD && sD < 23)
         ;//printf("SECOND:: sD: %.2f\n", sD);
      ////////////////////////////////////////////////////////////////////
      
      // Reflection
      if(addr->next->data.pos.y > WINDOW_HEIGHT/2 - 20)
         addr->next->data.velocity.y *= -1;
         
      if(addr->next->data.pos.x > WINDOW_WIDTH/2 - 20 || addr->next->data.pos.x < -WINDOW_WIDTH/2 + 20 ) 
         addr->next->data.velocity.x *= -1;
         
      if(collision(addr->next->data))
      {
         //printf(" START:: angleV(addr->next->data.velocity): %f\n", angleV(addr->next->data.velocity));
         addr->next->data.velocity.y *= -1; 
         //printf(" BEFORE:: angleV(addr->next->data.velocity): %f\n", angleV(addr->next->data.velocity));

         if(angleV(addr->next->data.velocity) < 90)
            addr->next->data.velocity = rotateV( addr->next->data.velocity, -atan2(150, 1600) ); // atan2(150, 1600)
         else addr->next->data.velocity = rotateV( addr->next->data.velocity, atan2(150, 1600) ); // atan2(150, 1600)
         //printf(" AFTER:: angleV(addr->next->data.velocity): %f\n\n", angleV(addr->next->data.velocity));
      }

      //node_t *circularlist = NULL, *ptr = (*head_light)->next, *current = addr;
/*
      while( ptr != NULL )
      {
         if( ptr->next == NULL )
         {
            //if(!( current == ptr && current == ptr->next))
               if( collide2Circle(ptr->data, (*head_light)->next->data));
                  printf("COLLIDE\n");
         }
         else
         {
            //if(!( current == ptr && current == ptr->next))
               if( collide2Circle(ptr->data, ptr->next->data) );
                  printf("COLLIDE\n");
         }
         ptr = ptr->next;
      } 
*/
/*
               // CIRCULAR LINKED LIST IMPLEMENTATION
      // Create another copy of the light linked list
      while ( ptr != NULL )
      {
         circularlist = addBeginning(circularlist, ptr->data);
         ptr = ptr->next;
      }
      // from hp till the NULL
      ptr = circularlist;
      while( ptr->next != NULL )
         ptr = ptr->next;
      // make it Circular Linked List
      ptr->next = circularlist;

               // ELASTIC COLLISION
      ptr = circularlist;
      while( ptr->next != circularlist )
      {
         if( collide2Circle(ptr->data, ptr->next->data) )
         {
            elasticCollision(&ptr->data, &ptr->next->data);
            printf("COLLIDE\n");
         }
         else
         {
             
            printf("NP\n");
         }
         ptr = ptr->next;
      }
*/
      addr = addr->next;
   }
}

void addBullet(node_t **head_bullet, int *cnt)
{
   node_t *addr = (*head_bullet);
   if( click)
   {
      addAfter(addr, {p_onClick, 270, {0, 0, 0}, {0, 0}, 0, 5});
      click = false;
      (*cnt)++;
   }
}

#if TIMER_ON == 1
void onTimer( int v )
{
   glutTimerFunc( TIMER_PERIOD, onTimer, 0 ); // Timer Events

if( flow )
{
   // Change orientation and/or position of the Player
	if (left) turn(&tank.angle, 3); // P.angle += 3
	if (right) turn(&tank.angle, -3); // P.angle -= 3


   //if( -WINDOW_HEIGHT/2 <= tank.pos.y && tank.pos.y <= WINDOW_HEIGHT/2 && -WINDOW_WIDTH/2 + 20 <= tank.pos.x && tank.pos.x <= WINDOW_WIDTH/2 - 20)
   {
      if (up) tank.pos = addV(tank.pos, pol2rec({ speed_object, tank.angle }));
	   if (down) tank.pos = subV(tank.pos, pol2rec({ speed_object, tank.angle }));
   }

   reflectMoveLight(&headlight);
   collisionLightAndBullet(&headbullet, &headlight, &cntbullet);
   addBullet(&headbullet, &cntbullet);
}
	// flee or chase
/*
   // velocity vector from Enemy to Player
	E.velocity = mulV( speed_bullet, unitV(subV(light.pos, E.pos)));

	// do not allow Enemy to get out of the game arena.
   //if (-WINDOW_WIDTH/2 <= E.pos.x && E.pos.x <= WINDOW_WIDTH/2 && -WINDOW_HEIGHT/2 <= E.pos.y && E.pos.y <= WINDOW_HEIGHT/2)
   if( magV(subV(E.pos, light.pos)) > 1 )
   {
      E.pos = addV(E.pos, E.velocity); // chase
      E.angle = rec2pol(unitV(subV(light.pos, E.pos))).angle - 90; // rotate according to light
   }
*/
   // to refresh the window it calls display() function
   glutPostRedisplay(); // display()
}
#endif

void Init()
{
   headlight = addBeginning(headlight, light);
   addAfter(headlight, { {600, 200}, 270, {0, 1, 0}, {2, 3}, 20 });
   addAfter(headlight, { {0, 200}, 270, {1, 0, 0}, {3, 2}, 20 });
   addAfter(headlight, { {-600, 200}, 270, {0, 0, 1}, {-3, 2}, 20 });

   headbullet = addBeginning(headbullet, E);
   //addAfter(headbullet, {{600, 200}, 270, {0, 1, 0}, {0, 0}, 5});

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
   glutCreateWindow("Mehmet Bulut Official Homework 4, Chase and Lighting Simulation");

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
   glutTimerFunc( TIMER_PERIOD, onTimer, 0 ) ;
   #endif

   Init();

   // Starts Event Dispatcher
   glutMainLoop();
}