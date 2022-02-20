/***********************************************************************************
         CTIS164 - Official Homework 3
====================================================================================
STUDENT : Mehmet Bulut 21903372
SECTION : 002
HOMEWORK : Official Homework 3 for 2021 Spring Semester (Submission: 25 April 2021)
====================================================================================
PROBLEMS :
 Satellite keep showing after each F1 key press
====================================================================================
ADDITIONAL FEATURES :
1- Tank (firing object) can be rotated with the onMove() function and can be fired with onClick() function 
2- Linked list are implemented for multiple bullet firing up to AMMO macro at a time on the scene 
2.1- Linked listes traversed for collision detection purposes and those who collide (both object and bullet) is removed 
from the object and bullet linked lists.
3.1- Bullet looks primitively like a bullet, there is no basic point bullets
3.2- Tank's angle and position is displayed along with the mouse location for reference purposes
4- Additional third object is used and moved along the sinusodial line with the harmonic motion
5- Additional bullet type is added that perform projectile motion, however angle of the bullet stayed same unfortunately :((
5.2- throwing velocity of the bullet change be either incremented or decremented with (+,-) 
6.1- Object that move on the orbit is also rotate itself (similar to earth moving around the sun and rotate by itself)
6.2- Our Fourth object, namely satellite orbit around another object, namely car.
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

#define WINDOW_WIDTH  1400
#define WINDOW_HEIGHT 800

#define TIMER_PERIOD  16 // Period for the timer. // 16
#define TIMER_ON      true // false:disable timer, true:enable timer

#define D2R 0.0174533
#define MAX_ARR 30
#define AMMO 20
#define MAX_OBJECT 3
#define PI 3.1415

// Global Variables for Template File
bool up, down, right, left;
int  winWidth, winHeight; // current Window width and height

// Mehmet Bulut's Global Declarations of Variables and Structures
bool fire, fire2, flow, game_over; // NOT PAUSED
bool first_time = true, bullet_display, object_moves;
int cnt_bullet, cnt_object, cnt_motion_bullet;
double degree, motion_velocity, angle;

//double coefficient[MAX_OBJECT];
//bool allowance;
//double min_distance;

enum type_e{garbage, plane, car, star, satellite};
enum move_type_e{standard, motion};

typedef struct{
   double x, y;
} point_t;

typedef struct{
   point_t velocity;
   double initial_angle;
   double degree;
} motion_t;

typedef struct{
   point_t pos;
   double angle; // in degree form NOT radius
   double speed;
   type_e type;
   double orbit_angle; // if it is a plane
   motion_t motion;
   point_t arr[MAX_ARR]; // holds the points that will be used for collision detection
   point_t center;
   int size; // size of the array
} object_t;

typedef struct{
   double r, g, b; // between 0 and 1
} color_t;

object_t tank;
point_t p_onClick, p_onMove, p_onMoveDown, centre; // static mouse pointer // dynamic mouse pointer

//******************************LINKED_LIST_LIBRARY****************************************************
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
			printf("x: %4.0f   y:%4.0f   angle: %4.0f   speed: %.0f  ->\n", hp->data.pos.x, hp->data.pos.y, hp->data.angle, hp->data.speed);
			hp = hp->next;
		}
		printf("NULL\n");
	}
}

void displayMotionBulletList(node_t *hp)
{
   hp = hp->next;
	if (hp == NULL)
		printf("The List is EMPTY !!!\n");
	else
	{
		while (hp != NULL)
		{
			printf("velocity:\tx: %4.0f   y:%4.0f   angle: %4.0f ->\n", hp->data.motion.velocity.x, hp->data.motion.velocity.y, hp->data.angle);
			hp = hp->next;
		}
		printf("NULL\n");
	}
}

void displayListObjects(node_t *hp)
{
   //hp = hp->next;
	if (hp == NULL)
		printf("The List is EMPTY !!!\n");
	else
	{
		while (hp != NULL)
		{
         switch(hp->data.type)
         {
            case garbage: printf("garbage\n"); break;
            case plane: printf("plane\n"); break;
            case car: printf("car\n"); break;
            case star: printf("star\n"); break;
            case satellite: printf("satellite\n"); break;
         }
			hp = hp->next;
		}
		printf("NULL\n");
	}
}
//******************************************************************************************************

node_t *bullet_list = NULL, *object_list = NULL, *motion_bullet_list = NULL;

double A, //amplitude
      fq,  //frequency
      C,   //horizontal phase shift
      B;   //vertical phase shift

double f(double x) {
   return A * sin((fq * x + C) * D2R) + B;
}

void InitObjects()
{
   object_t object;
   object = { {-50, 50}, 0, 5, plane, 0};
   addAfter(object_list, object);

   double coefficient_car = 12;
   object = { {-WINDOW_WIDTH/2-14.5*coefficient_car, 100}, 0, 5, car};
   addAfter(object_list, object);

   object = { {-50, 50}, 0, 5, star, 0};
   addAfter(object_list, object);

   object = { {0, 0}, 0, 5, satellite, 0};
   addAfter(object_list, object);
}

void Init()
{
   up = false, down = false, right = false, left = false;
   fire = false, fire2 = false, flow = false, game_over = false, bullet_display = true, object_moves = false;
   cnt_bullet = 0, cnt_object = 2, cnt_motion_bullet = 0, motion_velocity = 20, degree = 0;
   A = 100, //amplitude
   fq = 1,  //frequency
   C = 0,   //horizontal phase shift
   B = 0,   //vertical phase shift
   angle = 0;
   //min_distance = 150;
   tank = {{WINDOW_WIDTH/5, -WINDOW_HEIGHT/2+60}, 360, 5}; // pos, angle, speed
   for (int i = 0; i < MAX_ARR; i++)
      tank.arr[i] = {0, 0};

   object_t object;
   if( first_time )
   {
      // UNUSED GARBAGE BULLET AND OBJECT INSERTED TO THE LIST
      bullet_list = addBeginning(bullet_list, tank);
      motion_bullet_list = addBeginning(motion_bullet_list, tank);

      object = { {-50, 50}, 0, 5, garbage};
      object_list = addBeginning(object_list, object);
   }
   // Smoothing shapes
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void drawCircle( point_t p, double r, color_t color )
{
   float angle ;
   glColor3d(color.r, color.g, color.b);
   glLineWidth(1);
   glPointSize(1);
   glBegin( GL_POLYGON ) ;
   for ( int i = 0 ; i < 100 ; i++ )
   {
      angle = 2*PI*i/100;
      glVertex2f( p.x+r*cos(angle), p.y+r*sin(angle)) ;
   }
   glEnd();
}

void drawCircleWire(point_t p, double r, color_t color, double thickness)
{
   float angle;
   glLineWidth(thickness); // change the thickness of circle_wire
   glColor3d(color.r, color.g, color.b);
   glBegin(GL_LINE_LOOP);
   for (int i = 0; i < 100; i++){
      angle = 2 * PI*i / 100;
      glVertex2f(p.x + r*cos(angle), p.y + r*sin(angle));
   }
   glEnd();
}

void print(int x, int y, const char *string, void *font )
{
   int len, i ;
   glRasterPos2f( x, y );
   len = (int) strlen( string );
   for ( i =0; i<len; i++ )
      glutBitmapCharacter( font, string[i]);
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
      glutBitmapCharacter( font, str[i]);
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
         glutStrokeCharacter(GLUT_STROKE_ROMAN, str[i]);
   glPopMatrix();
}

// distance between two points.
double distance( point_t p1, point_t p2 ) 
{
   return sqrtf( (p1.x-p2.x)*(p1.x-p2.x) + (p1.y-p2.y)*(p1.y-p2.y) );
}

// p1 and p2 is a line. Likewise p3 and p4 is a line
bool intersect(point_t p1, point_t p2, point_t p3, point_t p4)
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

// check if two array of points in the form of GL_LINE_LOOP intersect or not
bool collide(point_t arr_i[], point_t center_i, int size_i, point_t arr_k[], point_t center_k, int size_k)
{
   for(int i = 0; i < size_i; i++)
      for(int k = 0; k < size_k; k++)
         if( intersect( arr_i[i%size_i], arr_i[(i+1)%size_i], arr_k[k%size_k], arr_k[(k+1)%size_k] ) )
            return true;
   return false;
}

// Apply translate (move) and rotation transformations 
point_t vertex(point_t P, point_t Tr, double angle) 
{
   point_t result;
   result.x = (P.x * cos(angle) - P.y * sin(angle)) + Tr.x;
   result.y = (P.x * sin(angle) + P.y * cos(angle)) + Tr.y;
   glVertex2d(result.x, result.y);
   return result;
}

// vertex WITHOUT glVertex2d
point_t rotateTransfer(point_t P, point_t Tr, double angle) 
{
   point_t result;
   result.x = (P.x * cos(angle) - P.y * sin(angle)) + Tr.x;
   result.y = (P.x * sin(angle) + P.y * cos(angle)) + Tr.y;
   return result;
}

void drawEllipse(point_t p, point_t radius, color_t color) // radius = width-height
{
   double angle = PI/180;
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glColor4f( color.r, color.g, color.b, 0.7 );
   glLineWidth(3);
   glBegin( GL_POLYGON );
   for(int t = 0; t <= 360; t+=5)
      glVertex2f(radius.x*cos(t*angle) + p.x, radius.y*sin(t*angle) + p.y);
   glEnd();
   glDisable(GL_BLEND);
}

void drawEllipseWire(point_t p, point_t radius, color_t color) // radius = width-height
{
   double angle = PI/180;
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glColor4f( color.r, color.g, color.b, 0.7 );
   glLineWidth(3);
   glBegin( GL_LINE_STRIP );
   for(int t = 0; t <= 360; t+=5)
      glVertex2f(radius.x*cos(t*angle) + p.x, radius.y*sin(t*angle) + p.y);
   glEnd();
   glDisable(GL_BLEND);
}

void drawTank(object_t tank)
{
   double angle = tank.angle * D2R;
   double temp_angle = angle;
   double coefficient = 7;
   point_t pos = tank.pos;
                        glPointSize(1);
   //angle = 0;
   // Main body
   glColor3f(1, 1, 0);
   glBegin(GL_QUADS);
      vertex({5*coefficient, 4*coefficient}, pos, angle);
      vertex({-5*coefficient, 4*coefficient}, pos, angle);
      vertex({-5*coefficient, -5*coefficient}, pos, angle);
      vertex({5*coefficient, -5*coefficient}, pos, angle);
   glEnd();
   // Front
   glColor3f(0.5, 0.5, 0.5);
   glBegin(GL_QUADS);
      vertex({-1*coefficient, 6*coefficient}, pos, angle);
      vertex({-4*coefficient, 4*coefficient}, pos, angle);
      vertex({4*coefficient, 4*coefficient}, pos, angle);
      vertex({1*coefficient, 6*coefficient}, pos, angle);
   glEnd();
   // Back
   glColor3f(0.5, 0.5, 0.5);
   glBegin(GL_QUADS);
      vertex({-3*coefficient, -5*coefficient}, pos, angle);
      vertex({-2*coefficient, -6*coefficient}, pos, angle);
      vertex({2*coefficient, -6*coefficient}, pos, angle);
      vertex({3*coefficient, -5*coefficient}, pos, angle);
   glEnd();
   // Wings
      // Right Wing
      glColor3f(0, 0.5, 0);
      glBegin(GL_QUADS);
         vertex({-5*coefficient, +8*coefficient}, pos, angle);
         vertex({-9*coefficient, +8*coefficient}, pos, angle);
         vertex({-9*coefficient, -8*coefficient}, pos, angle);
         vertex({-5*coefficient, -8*coefficient}, pos, angle);
      glEnd();
      // Left Wing
      glBegin(GL_QUADS);
         vertex({5*coefficient, +8*coefficient}, pos, angle);
         vertex({9*coefficient, +8*coefficient}, pos, angle);
         vertex({9*coefficient, -8*coefficient}, pos, angle);
         vertex({5*coefficient, -8*coefficient}, pos, angle);
      glEnd();

   angle = temp_angle;
   // Firing part of the tank
   glColor3f(0, 1, 0);
   glBegin(GL_QUADS);
      vertex({+1*coefficient, +17*coefficient}, pos, angle);
      vertex({-1*coefficient, +17*coefficient}, pos, angle);
      vertex({-1*coefficient, +3*coefficient}, pos, angle);
      vertex({+1*coefficient, +3*coefficient}, pos, angle);
   glEnd();
   // Top Turner
   glColor3f(0, 0, 1);
   glBegin(GL_POLYGON);
      vertex({+2*coefficient, +3*coefficient}, pos, angle);
      vertex({-2*coefficient, +3*coefficient}, pos, angle);
      vertex({-3*coefficient, +2*coefficient}, pos, angle);
      vertex({-3*coefficient, -2*coefficient}, pos, angle);
      vertex({-2*coefficient, -3*coefficient}, pos, angle);
      vertex({+2*coefficient, -3*coefficient}, pos, angle);
      vertex({+3*coefficient, -2*coefficient}, pos, angle);
      vertex({+3*coefficient, +2*coefficient}, pos, angle);
   glEnd();
}

void drawBullet(object_t bullet, point_t arr[], int *size_arr, point_t *center)
{
   double angle = bullet.angle * D2R;
   double coefficient = 7;
   point_t pos = bullet.pos;
   glPointSize(1);
   glColor3f(1, 0, 1);
   glBegin(GL_QUADS);
      arr[3] = vertex({0.5*coefficient, -1.5*coefficient}, pos, angle);
      arr[4] = vertex({0.5*coefficient, 0.5*coefficient}, pos, angle);
      arr[1] = vertex({-0.5*coefficient, 0.5*coefficient}, pos, angle);
      arr[2] = vertex({-0.5*coefficient, -1.5*coefficient}, pos, angle);
   glEnd();

   glColor3f(0, 1, 1);
   glBegin(GL_TRIANGLES);
      arr[0] = vertex({0*coefficient, 1.5*coefficient}, pos, angle);
      arr[1] = vertex({-0.5*coefficient, 0.5*coefficient}, pos, angle);
      arr[4] = vertex({0.5*coefficient, 0.5*coefficient}, pos, angle);
   glEnd();
// middle
   *center = rotateTransfer({0*coefficient, 0*coefficient}, pos, angle);
   *size_arr = 5;
}

void drawPlane(object_t plane, point_t arr[], int *size_arr, point_t *center)
{
   double angle = plane.angle * D2R;
   double coefficient = 7;
   point_t pos = plane.pos;
                           glPointSize(1);
   // nose
   glColor3f(1, 1, 0);
   glBegin(GL_TRIANGLES);
      arr[0] = vertex({0*coefficient,11*coefficient}, pos, angle);
      arr[1] = vertex({-3*coefficient,6*coefficient}, pos, angle);
      arr[17] = vertex({3*coefficient,6*coefficient}, pos, angle);
   glEnd();
   // main body
   glColor3f(1, 0, 1);
   glBegin(GL_QUADS);
         arr[1] = vertex({-3*coefficient,6*coefficient}, pos, angle);
         arr[6] = vertex({-3*coefficient,-5*coefficient}, pos, angle);
         arr[12] = vertex({3*coefficient,-5*coefficient}, pos, angle);
         arr[17] = vertex({3*coefficient,6*coefficient}, pos, angle);
   glEnd();
      // wings
   // left wing
   glColor3f(1, 0, 0);
   glBegin(GL_QUADS);
      arr[2] = vertex({-3*coefficient,2*coefficient}, pos, angle);
      arr[3] = vertex({-11*coefficient,0*coefficient}, pos, angle);
      arr[4] = vertex({-11*coefficient,-2*coefficient}, pos, angle);
      arr[5] = vertex({-3*coefficient,-2*coefficient}, pos, angle);
   glEnd();
   // right wing
   glColor3f(1, 0, 0);
   glBegin(GL_QUADS);
      arr[16] = vertex({3*coefficient,2*coefficient}, pos, angle);
      arr[15] = vertex({11*coefficient,0*coefficient}, pos, angle);
      arr[14] = vertex({11*coefficient,-2*coefficient}, pos, angle);
      arr[13] = vertex({3*coefficient,-2*coefficient}, pos, angle);
   glEnd();
   // back wings
   glColor3f(0.5, 0.5, 1);
   glBegin(GL_POLYGON);
      arr[6] = vertex({-3*coefficient,-5*coefficient}, pos, angle);
      arr[7] = vertex({-6*coefficient,-9*coefficient}, pos, angle);
      arr[8] = vertex({-4*coefficient,-11*coefficient}, pos, angle);
      arr[9] = vertex({0*coefficient,-7*coefficient}, pos, angle);
      arr[10] = vertex({4*coefficient,-11*coefficient}, pos, angle);
      arr[11] = vertex({6*coefficient,-9*coefficient}, pos, angle);
      arr[12] = vertex({3*coefficient,-5*coefficient}, pos, angle);
   glEnd();
//middle
   *center = rotateTransfer({0,0}, pos, angle);
   *size_arr = 18;
}

void drawCar(object_t car, point_t arr[], int *size_arr, point_t *center)
{
   double angle = car.angle * D2R;
   double coefficient = 12;
   point_t pos = car.pos;
                           glPointSize(1);
   glBegin(GL_QUADS);
   // top body
   glColor3f(0, 0.5, 1);
      arr[0] = vertex({-6*coefficient, 6*coefficient}, pos, angle);
      arr[1] = vertex({-8*coefficient, 2*coefficient}, pos, angle);
      arr[12] = vertex({8*coefficient, 2*coefficient}, pos, angle);
      arr[13] = vertex({6*coefficient, 6*coefficient}, pos, angle);
   // main body
   glColor3f(0.5, 0, 0);
      arr[2] = vertex({-10*coefficient, 2*coefficient}, pos, angle);
      arr[3] = vertex({-10*coefficient, -5*coefficient}, pos, angle);
      arr[10] = vertex({10*coefficient, -5*coefficient}, pos, angle);
      arr[11] = vertex({10*coefficient, 2*coefficient}, pos, angle);
   // left window
   glColor3f(0.8, 0.8, 1);
      vertex({-5*coefficient, 5*coefficient}, pos, angle);
      vertex({-6*coefficient, 3*coefficient}, pos, angle);
      vertex({-3*coefficient, 3*coefficient}, pos, angle);
      vertex({-2*coefficient, 5*coefficient}, pos, angle);
   // right window
   glColor3f(0.8, 0.8, 1);
      vertex({2*coefficient, 5*coefficient}, pos, angle);
      vertex({3*coefficient, 3*coefficient}, pos, angle);
      vertex({6*coefficient, 3*coefficient}, pos, angle);
      vertex({5*coefficient, 5*coefficient}, pos, angle);
   glEnd();

   drawCircleWire(vertex({-5*coefficient, -5*coefficient}, pos, angle), 3*coefficient, {0.5,0.5,0.5}, 5);
   drawCircleWire(vertex({5*coefficient, -5*coefficient}, pos, angle), 3*coefficient, {0.5,0.5,0.5}, 5);

   glLineWidth(1);
   glBegin(GL_LINES);
   glColor3f(1, 1, 1);
   // left window line
      vertex({-5*coefficient, 5*coefficient}, pos, angle);
      vertex({-3*coefficient, 3*coefficient}, pos, angle);
   // right window line
      vertex({3*coefficient, 3*coefficient}, pos, angle);
      vertex({5*coefficient, 5*coefficient}, pos, angle);
   glEnd();

   glLineWidth(5);
   glColor3f(0.5, 0.5, 0.5);
   glBegin(GL_LINES);
   // left tire line
      arr[4] = vertex({-8*coefficient, -5.3*coefficient}, pos, angle);
      arr[6] = vertex({-2*coefficient, -5.3*coefficient}, pos, angle);
      arr[5] = vertex({-5*coefficient, -8*coefficient}, pos, angle);
      vertex({-5*coefficient, -2*coefficient}, pos, angle);
   // right tire line
      arr[7] = vertex({2*coefficient, -5.3*coefficient}, pos, angle);
      arr[9] = vertex({8*coefficient, -5.3*coefficient}, pos, angle);
      arr[8] = vertex({5*coefficient, -8*coefficient}, pos, angle);
      vertex({5*coefficient, -2*coefficient}, pos, angle);
   glEnd();

   glColor3f(0.5, 0, 0);
   glBegin(GL_TRIANGLES);
      vertex({0*coefficient, 1*coefficient}, pos, angle);
      vertex({-3*coefficient, -2*coefficient}, pos, angle);
      vertex({3*coefficient, -2*coefficient}, pos, angle);
   glEnd();

   glColor3f(1, 1, 1);
   point_t printp = rotateTransfer({-2*coefficient, -1.5*coefficient}, pos, angle);
   vprint( printp.x, printp.y, GLUT_BITMAP_8_BY_13,"My Car");
// middle
   *center = rotateTransfer({0*coefficient, 0*coefficient}, pos, angle);
   *size_arr = 14;
}

void drawStar(object_t star, point_t arr[], int *size_arr, point_t *center)
{
   double angle = star.angle * D2R;
   double coefficient = 10;
   point_t pos = star.pos;
   glColor3d(1, 1, 0);
   glBegin( GL_TRIANGLES );
   // 1
   arr[0] = vertex({0*coefficient, 8*coefficient}, pos, angle);
   vertex({-4*coefficient, 0*coefficient}, pos, angle);
   vertex({4*coefficient, 0*coefficient}, pos, angle);
   arr[1] = rotateTransfer({2*coefficient, 4.5*coefficient}, pos, angle);
   // 2
   arr[2] = vertex({-6*coefficient, 6*coefficient}, pos, angle);
   vertex({-3*coefficient, -3*coefficient}, pos, angle);
   vertex({3*coefficient, 3*coefficient}, pos, angle);
   arr[3] = rotateTransfer({-4.5*coefficient, 2*coefficient}, pos, angle);
   // 3
   arr[4] = vertex({-8*coefficient, 0*coefficient}, pos, angle);
   vertex({0*coefficient, -4*coefficient}, pos, angle);
   vertex({0*coefficient, 4*coefficient}, pos, angle);
   arr[5] = rotateTransfer({-4.5*coefficient, -2*coefficient}, pos, angle);
   // 4
   arr[6] = vertex({-6*coefficient, -6*coefficient}, pos, angle);
   vertex({3*coefficient, -3*coefficient}, pos, angle);
   vertex({-3*coefficient, 3*coefficient}, pos, angle);
   arr[7] = rotateTransfer({0*coefficient, 8*coefficient}, pos, angle);
   // 5
   arr[8] = vertex({0*coefficient, -8*coefficient}, pos, angle);
   vertex({4*coefficient, 0*coefficient}, pos, angle);
   vertex({-4*coefficient, 0*coefficient}, pos, angle);
   arr[9] = rotateTransfer({-2*coefficient, -4.5*coefficient}, pos, angle);
   // 6
   arr[10] = vertex({6*coefficient, -6*coefficient}, pos, angle);
   vertex({3*coefficient, 3*coefficient}, pos, angle);
   vertex({-3*coefficient, -3*coefficient}, pos, angle);
   arr[11] = rotateTransfer({4.5*coefficient, -2*coefficient}, pos, angle);
   // 7
   arr[12] = vertex({8*coefficient, 0*coefficient}, pos, angle);
   vertex({0*coefficient, 4*coefficient}, pos, angle);
   vertex({0*coefficient, -4*coefficient}, pos, angle);
   arr[13] = rotateTransfer({4.5*coefficient, 2*coefficient}, pos, angle);
   // 8
   arr[14] = vertex({6*coefficient, 6*coefficient}, pos, angle);
   vertex({-3*coefficient, 3*coefficient}, pos, angle);
   vertex({3*coefficient, -3*coefficient}, pos, angle);
   arr[15] = rotateTransfer({2*coefficient, 4.5*coefficient}, pos, angle);
   glEnd();
   //arr[-0] = vertex({-0*coefficient, -0*coefficient}, pos, angle);
   *center = rotateTransfer({0*coefficient, 0*coefficient}, pos, angle);
   *size_arr = 16;
}

void drawSatellite(object_t satellite, point_t arr[], int *size_arr, point_t *center)
{
   double angle = satellite.angle * D2R;
   double coefficient = 4;
   point_t pos = satellite.pos;
   //drawEllipse({0}, {10*coefficient+pos.x, 5*coefficient+pos.y}, {1, 0, 0});
                        glLineWidth(50);
   glColor3f(1, 0, 1);
   glBegin(GL_LINE_STRIP);
      arr[2] = vertex({-6*coefficient, -3*coefficient}, pos, angle);
      arr[1] = vertex({-6*coefficient, 10*coefficient}, pos, angle);
      arr[0] = vertex({-10*coefficient, 10*coefficient}, pos, angle);
   glEnd();
      glColor3f(1, 0, 1);
   glBegin(GL_LINE_STRIP);
      arr[3] = vertex({6*coefficient, -3*coefficient}, pos, angle);
      arr[4] = vertex({6*coefficient, 10*coefficient}, pos, angle);
      arr[5] = vertex({10*coefficient, 10*coefficient}, pos, angle);
   glEnd();
                        glLineWidth(1);
   glColor3f(0, 1, 1);
   glBegin(GL_QUADS);
      arr[2] = vertex({-6*coefficient, -3*coefficient}, pos, angle);
      arr[3] = vertex({6*coefficient, -3*coefficient}, pos, angle);
      arr[6] = vertex({6*coefficient, 3*coefficient}, pos, angle);
      arr[7] = vertex({-6*coefficient, 3*coefficient}, pos, angle);
   glEnd();
// middle
   *center = rotateTransfer({0*coefficient, 0*coefficient}, pos, angle);
   *size_arr = 8;
}

void display() 
{
   glClearColor( 0, 0 , 0 , 0 ); 
   glClear( GL_COLOR_BUFFER_BIT );
   // BARRRIER
   glLineWidth(5);
   glColor3f(1, 1, 1);
   glBegin(GL_LINE_LOOP);
      glVertex2d(WINDOW_WIDTH/2-1, WINDOW_HEIGHT/2-1);
      glVertex2d(-WINDOW_WIDTH/2+1, WINDOW_HEIGHT/2-1);
      glVertex2d(-WINDOW_WIDTH/2, -WINDOW_HEIGHT/2);
      glVertex2d(WINDOW_WIDTH/2, -WINDOW_HEIGHT/2);
   glEnd();
   glLineWidth(1);

   glColor3f( 1, 1, 1 );
   if(first_time)
      vprint( -100, 0, GLUT_BITMAP_TIMES_ROMAN_24,"PRESS F1 TO START...");
   else if(game_over)
      vprint( -100, 0, GLUT_BITMAP_TIMES_ROMAN_24,"GAME OVER! YOU WIN!!");

   drawTank(tank);
   node_t *addr = bullet_list->next;
   while(addr != NULL)
   {
      drawBullet(addr->data, addr->data.arr, &addr->data.size, &addr->data.center);
      addr = addr->next;
   }
    
   node_t *motion_addr = motion_bullet_list->next;
   while(motion_addr != NULL)
   {
      drawBullet(motion_addr->data, motion_addr->data.arr, &motion_addr->data.size, &motion_addr->data.center);

      //glColor3f(1, 1, 1);
      //for (int i = 0; i < motion_addr->data.size; i++)
         //vprint( motion_addr->data.arr[i].x, motion_addr->data.arr[i].y, GLUT_BITMAP_8_BY_13,"%d", i);
      //vprint( motion_addr->data.center.x, motion_addr->data.center.y, GLUT_BITMAP_8_BY_13,"center");

      motion_addr = motion_addr->next;
   }
   double temp_x;
   node_t *addr2 = object_list->next;
   while(object_moves && addr2 != NULL)
   {
      if(addr2->data.type == plane)
         drawPlane(addr2->data, addr2->data.arr, &addr2->data.size, &addr2->data.center);
      else if(addr2->data.type == car)
      {
         centre = addr2->data.pos;
         drawCar(addr2->data, addr2->data.arr, &addr2->data.size, &addr2->data.center);
      }
      else if(addr2->data.type == star)
         drawStar(addr2->data, addr2->data.arr, &addr2->data.size, &addr2->data.center);
      else if (addr2->data.type == satellite)
        drawSatellite(addr2->data, addr2->data.arr, &addr2->data.size, &addr2->data.center);
      addr2 = addr2->next;
   }
   // BACKGROUND OF DIRECTIVES
   glColor3f( 0.5, 0.5, 0.5 );
   glRectf(-WINDOW_WIDTH/2, WINDOW_HEIGHT/2-80, WINDOW_WIDTH/2, WINDOW_HEIGHT/2);
   // KEYBOARD AND MOUSE DIRECTIVES
   glColor3f( 1, 1, 1 ); 
   vprint( -WINDOW_WIDTH/2+15, WINDOW_HEIGHT/2-55, GLUT_BITMAP_HELVETICA_18,"<Spacebar,Left Click> Fire;  <R,r> Motion_Bullet_Fire;  <F1> Pause/Restart;  <Up/Down,A,a/D,d Arrow Keys> Right/Left Move;  <Right/Left Arrow Keys> Right/Left Turn");
   // DYNAMIC MOUSE POSITION
   glColor3f( 0, 1, 1 );
   vprint( WINDOW_WIDTH/2-WINDOW_WIDTH/5, -WINDOW_HEIGHT/2+WINDOW_HEIGHT/14, GLUT_BITMAP_8_BY_13,"Mouse Pointer( x: %3.0f y: %3.0f )", p_onMove.x, p_onMove.y);
   // TANK POSITION AND ANGLE
   glColor3f( 1, 0, 1 );
   vprint( WINDOW_WIDTH/2-WINDOW_WIDTH/6, +WINDOW_HEIGHT/2-WINDOW_HEIGHT/8, GLUT_BITMAP_8_BY_13,"Tank Pos( x: %3.0f y: %3.0f )", tank.pos.x, tank.pos.y);
   glColor3f( 1, 1, 0 );
   vprint( WINDOW_WIDTH/2-WINDOW_WIDTH/10, -WINDOW_HEIGHT/2+WINDOW_HEIGHT/10, GLUT_BITMAP_8_BY_13,"Tank angle: %.0f", tank.angle);
   glColor3f( 1, 1, 1 );
   vprint( WINDOW_WIDTH/2-WINDOW_WIDTH/4, -WINDOW_HEIGHT/2+WINDOW_HEIGHT/8, GLUT_BITMAP_8_BY_13,"motion_bullet velocity (+,-) to change %.0f", motion_velocity);

/* 
   glPointSize(10);
   glColor3f(1, 1, 1);
   glBegin(GL_POINTS);
      glVertex2d(0, 0);
   glEnd();
   glPointSize(1);
   //glColor3f(1, 1, 1);
      //for (int i = 0; i < addr->data.size; i++)
         //vprint( addr->data.arr[i].x, addr->data.arr[i].y, GLUT_BITMAP_8_BY_13,"%d", i);
      //vprint( addr->data.center.x, addr->data.center.y, GLUT_BITMAP_8_BY_13,"center");
*/
   glutSwapBuffers();
}

void move(point_t *pos, double *angle, double *speed) 
{
   pos->x += ( *speed * cos( (*angle + 90) * D2R ) );
   pos->y += ( *speed * sin( (*angle + 90) * D2R ) );
   if (*angle < 0) 
      *angle += 360;
   if (*angle >= 360) 
      *angle -= 360;
}

point_t move_return(point_t pos, double *angle, double speed) 
{
   pos.x += ( speed * cos( (*angle + 90) * D2R ) );
   pos.y += ( speed * sin( (*angle + 90) * D2R ) );
   if (*angle < 0) 
      *angle += 360;
   if (*angle >= 360) 
      *angle -= 360;
   return pos;
}

void moveReverse(point_t *pos, double *angle, double *speed) 
{
   pos->x -= ( *speed * cos( (*angle + 90) * D2R ) );
   pos->y -= ( *speed * sin( (*angle + 90) * D2R ) );
   if (*angle < 0) 
      *angle += 360;
   if (*angle >= 360) 
      *angle -= 360;
}

void move_x(point_t *pos, double *speed) 
{
   pos->x += *speed;
}

void move_y(point_t *pos, double *speed) 
{
   pos->y += *speed;
}

void motion_move(point_t *pos, double *angle, point_t *velocity) 
{
   pos->x += ( (*velocity).x * cos( (*angle + 90) * D2R ) );
   pos->y += ( (*velocity).y * sin( (*angle + 90) * D2R ) );
   if (*angle < 0) 
      *angle += 360;
   if (*angle >= 360) 
      *angle -= 360;
}

void moveCircular( point_t orbit_center, point_t *pos, double radius, double *angle)
{
   orbit_center.x += radius*cos(*angle*D2R);
   orbit_center.y += radius*sin(*angle*D2R);
   *pos = orbit_center;
   (*angle)++;
}

void moveSinusoidal(point_t *pos, double *x)
{
   (*pos).x = *x;
   (*pos).y = f(*x);
   if( (*pos).x <= +WINDOW_WIDTH/2 + 70 )
      (*x)+= 5;
   else
      (*x) = -WINDOW_WIDTH/2 - 70;
}

void turn(double *angle, double turn) 
{
   *angle += turn;
   if (*angle < -360) 
      *angle += 720;
   if (*angle >= 720) 
      *angle -= 720;
}

// FOR TANK // boundaries could be 0 - 359 
void turn_limited(double *angle, double turn) 
{
   //if(185-turn <= tank->angle && tank->angle <= 355-turn)
   if(0 < (*angle) && (*angle) <= 90-turn || 270-turn <= (*angle) && (*angle) <= 360)
      *angle += turn;
   
   if(*angle == 0)
      *angle = 360; 
   if (*angle < 0) 
      *angle += 360;
   if (*angle >= 360) 
      *angle -= 360;
}

void onKeyDown(unsigned char key, int x, int y )
{
   //CALLER FUNCTION: glutKeyboardFunc(onKeyDown);
   // The x and y callback parameters indicate the mouse location in window relative coordinates when the key was pressed.
   // key: integer value of ASCII characters like ESC, a,b,c..,A,B,..Z etc.
   
   // exit when ESC is pressed.
   if ( key == 27 )
      exit(0);

   if( key == 'R' | key == 'r' )
      if(flow)
         fire2 = true;

   switch( key ){
      case 'D': case 'd': up = true; break;
      case 'A': case 'a': down = true; break;
      case ' ': 
         if(flow) 
            fire = true; 
      break;
      case '+': 
      {
         motion_velocity++;
      } break;
      case '-': 
      {
         motion_velocity--; 
      } break;
   }
   glutPostRedisplay(); // to refresh the window it calls display() function
}

void onKeyUp(unsigned char key, int x, int y )
{
   //CALLER FUNCTION: glutKeyboardUpFunc(onKeyUp);
   // The x and y callback parameters indicate the mouse location in window relative coordinates when the key was pressed.
   // key: integer value of ASCII characters like ESC, a,b,c..,A,B,..Z etc.
   // if exit() function executed, after a key pressed assumed released happened inevitably 
  // exit when ESC is pressed.
   if ( key == 27 )
      exit(0);

   if( key == 'R' |- key == 'r' )
      fire2 = false;

   switch( key ){
      case 'D': case 'd': up = false; break;
      case 'A': case 'a': down = false; break;
      case ' ': fire = false; break;
   }
   glutPostRedisplay(); // to refresh the window it calls display() function
}

void onSpecialKeyDown( int key, int x, int y )
{
   //CALLER FUNCTION: glutSpecialFunc(onSpecialKeyDown);
   // The x and y callback parameters indicate the mouse location in window relative coordinates when the key was pressed.
                  // key:
   // Special Key like GLUT_KEY_F1, F2, F3,...
   // Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
   switch (key) {
      case GLUT_KEY_UP: up = true; break;
      case GLUT_KEY_DOWN: down = true; break;
      case GLUT_KEY_LEFT: left = true; break;
      case GLUT_KEY_RIGHT: right = true; break;
      case GLUT_KEY_F1: {
            flow = !flow;

         if(first_time)
         {
            first_time = false;
            flow = true;
         }
         if( game_over )
         {
            game_over = false;
            Init();
            InitObjects();
            flow = true;
         }
      } break;
   }
   glutPostRedisplay(); // to refresh the window it calls display() function
}

void onSpecialKeyUp( int key, int x, int y )
{
   //CALLER FUNCTION: glutSpecialUpFunc(onSpecialKeyUp);
   // The x and y callback parameters indicate the mouse location in window relative coordinates when the key was pressed.
                  // key: 
   // Special Key like GLUT_KEY_F1, F2, F3,...
   // Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT
   // if exit() function executed, after a key pressed assumed released happened inevitably
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
   //CALLER FUNCTION: glutMouseFunc(onClick);
   // button(s) == GLUT_LEFT_BUTTON , GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON
   // state(s)  == GLUT_UP , GLUT_DOWN [are press and release cycle]
   // x, y  is the coordinate of the point that mouse clicked.
   if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && flow)
         fire = true;
   //p_onClick.x = x - WINDOW_WIDTH / 2;
   //p_onClick.y = WINDOW_HEIGHT / 2 - y;
   //glutPostRedisplay() ; 
}

void onResize( int w, int h )
{
   //CALLER FUNCTION: glutReshapeFunc(onResize);
   //FUNCTION DEFINITION: onResize()
   // This function is called when the window size changes.
   //FUNCTION PROPERTIES: onResize()
   // w : is the new width of the window in pixels.
   // h : is the new height of the window in pixels
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
   //CALLER FUNCTION: glutMotionFunc()
   // When the user moves the mouse pointer while any of the mouse button is down.
   //FUNCTION PROPERTIES: applicable for all glut functions
   // =====================================================
   // GLUT to OpenGL coordinate conversion:
   //p_onMoveDown.x = x - WINDOW_WIDTH / 2;
   //p_onMoveDown.y = WINDOW_HEIGHT / 2 - y;
   // =====================================================
   //glutPostRedisplay(); // to refresh the window it calls display() function 
}

void onMove( int x, int y ) 
{
   p_onMove.x = x - WINDOW_WIDTH / 2;
   p_onMove.y = WINDOW_HEIGHT / 2 - y;
   if(flow && p_onMove.y > -330)
   {
      double angle_with_mouse = atan2(p_onMove.y-tank.pos.y, p_onMove.x-tank.pos.x)/D2R;
      angle_with_mouse += 270;
      if(angle_with_mouse < 0)
         angle_with_mouse+=360;
      else if(angle_with_mouse >= 360)
         angle_with_mouse-=360;
      tank.angle = angle_with_mouse;
      //glutPostRedisplay(); // to refresh the window it calls display() function
   }
}

// if one bullet and one object collided do the necessary // make the bullet move
void collisionAndBulletMove(node_t **head_bullet, node_t **head_object, int *cnt, move_type_e mv)
{
   object_t deleted_bullet, deleted_object;
   node_t *addr = (*head_bullet);
   while(addr->next != NULL) // IF BULLET IS NOT FINISHED
   {
      // IF BULLET IS INSIDE THE SCREEN
      if(-WINDOW_WIDTH/2 <= addr->next->data.arr[1].x && addr->next->data.arr[1].x <= WINDOW_WIDTH/2 && -WINDOW_HEIGHT/2 <= addr->next->data.arr[1].y && addr->next->data.arr[1].y <= WINDOW_HEIGHT/2-70)
      {
         if(mv == standard)
            move(&addr->next->data.pos, &addr->next->data.angle, &addr->next->data.speed);
         else if(mv == motion)
            motion_move(&addr->next->data.pos, &addr->next->data.angle, &addr->next->data.motion.velocity);

         if(bullet_display == false && addr->next != NULL)
         {
            deleteAfter(addr, &deleted_bullet);
            (*cnt)--;
         }
         
         node_t *addr2 = (*head_object);
         while(addr->next != NULL && addr2->next != NULL) // IF BULLET AND OBJECT IS NOT FINISHED
         {
            //IF COLLIDE
            if( collide( addr->next->data.arr, addr->next->data.center, addr->next->data.size, addr2->next->data.arr, addr2->next->data.center ,addr2->next->data.size ) )
            {
               //DELETE CURRENT_BULLET AND CURRENT_OBJECT
               deleteAfter(addr, &deleted_bullet);
               (*cnt)--;
               deleteAfter(addr2, &deleted_object);
               cnt_object--;
            }
            else
               addr2 = addr2->next;
         }
         // IF BULLET IS FINISHED
         if(addr->next != NULL)
            addr = addr->next;
      }
      else 
      {
         deleteAfter(addr, &deleted_bullet);
         (*cnt)--;
      }
   }
}

bool objectMoves(node_t **head_object)
{
   node_t *addr = (*head_object);
   while(addr->next != NULL) // IF BULLET AND OBJECT IS NOT FINISHED
   {
      switch(addr->next->data.type){
         case plane: 
         {
            moveSinusoidal(&addr->next->data.pos, &degree);
         } break;
         case car:  
         {
            double coefficient_car = 12;
            if(addr->next->data.pos.x >= +WINDOW_WIDTH/2+14.5*coefficient_car)
               addr->next->data.pos = {-WINDOW_WIDTH/2-14.5*coefficient_car, 100};
            move_x(&addr->next->data.pos, &addr->next->data.speed);
         } break;
         case star:
         {     
            moveCircular( {-WINDOW_WIDTH/4,0}, &addr->next->data.pos, 150, &addr->next->data.orbit_angle);
            turn(&addr->next->data.angle, 5); 
         } break;
         case satellite:
         {
            moveCircular( centre, &addr->next->data.pos, 150, &addr->next->data.orbit_angle);
         } break;
         default: ; break;
      }
      addr = addr->next;
   }
   return true;
}

void accelerate(node_t **head_bullet)
{
   double atan2_angle, bullet_angle, initial_angle;
   node_t *addr = (*head_bullet);
   while(addr->next != NULL)
   {
      addr->next->data.motion.velocity.y -= 1;
      addr = addr->next;
   }
}
/*
void centerBecomeClosestOne(node_t **head_object)
{
   node_t *addr = (*head_object);
   node_t *marked, *satellite_ptr;
   double max = 0, distant;

   while(addr->next != NULL)
   {
      if(addr->next->data.type == satellite)
         satellite_ptr = addr->next;
      addr = addr->next;
   }

   while(addr->next != NULL)
   {
      distant = distance(addr->next->data.center, satellite_ptr->data.center);
      if( distant > max)
      {
         max = distant;
         marked = addr->next;
      }
      addr = addr->next;
   }
   marked->data.pos = centre;
}
*/
#if TIMER_ON == true
void onTimer( int v ) 
{
   glutTimerFunc( TIMER_PERIOD, onTimer, 0 ); // Timer Events
   if(flow)
   {
      if(tank.angle == 0)
         tank.angle = 360;
      // turn the player clockwise direction
      if ( right ) // && ( 90 < tank.angle%360 && tank.angle%360 || 90 < tank.angle%360 && tank.angle%360 )
         turn_limited(&tank.angle, -5); 
      // turn the player counter-clockwise direction
      if ( left )
         turn_limited(&tank.angle, 5); 
      // move forward
      if ( up && tank.pos.x <= WINDOW_WIDTH/2-50)
         tank.pos.x += 5; 
         //move(&tank.pos, &tank.angle, &tank.speed);
      // move backward
      if ( down && tank.pos.x >= -WINDOW_WIDTH/2+50) 
      {
         tank.pos.x -= 5; 
         //moveReverse(&tank.pos, &tank.angle, &tank.speed);
      }
      if( fire && cnt_bullet <= AMMO )
      {
         object_t bullet = tank;
         bullet.pos = move_return(tank.pos, &tank.angle, 17*7);
         bullet.speed = tank.speed * 2;
         addAfter(bullet_list, bullet);
         cnt_bullet++;
         fire = false;
      }
      if( fire2 && cnt_motion_bullet <= AMMO/4 )
      {
         object_t motion_bullet = tank;
         motion_bullet.pos = move_return(tank.pos, &tank.angle, 17*7);
         motion_bullet.motion.velocity = {motion_velocity, motion_velocity};
         motion_bullet.motion.initial_angle = atan2(motion_bullet.motion.velocity.y, motion_bullet.motion.velocity.x)/D2R;

         addAfter(motion_bullet_list, motion_bullet);
         cnt_motion_bullet++;
         fire2 = false;
      }
      collisionAndBulletMove(&bullet_list, &object_list, &cnt_bullet, standard);
      accelerate(&motion_bullet_list);
      collisionAndBulletMove(&motion_bullet_list, &object_list, &cnt_motion_bullet, motion);
      object_moves = objectMoves(&object_list);
      //centerBecomeClosestOne(&object_list);
   }
   
   if( game_over)
   {
      bullet_display = false;
      collisionAndBulletMove(&bullet_list, &object_list, &cnt_bullet, standard);
      collisionAndBulletMove(&motion_bullet_list, &object_list, &cnt_motion_bullet, motion);
   }

   if(object_list->next == NULL)
   {
      flow = false;
      game_over = true;
   }
   //displayMotionBulletList(motion_bullet_list);
   //displayListObjects(object_list);
   glutPostRedisplay(); // / to refresh the window it calls display() function
}
#endif

int main( int argc, char *argv[] ) {
                        // CREATE WINDOW //
   glutInit(&argc, argv );
   glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );
   glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
   //glutInitWindowPosition(100, 100);
   // Enter name of the Window as string such as "Template File"
   glutCreateWindow("Mehmet Bulut Official Homework 3 \"Hitting the Targets\"");
                        // PAINT/DISPLAY //
   // sets the display callback for the current window. [Paint/Display Event]
   glutDisplayFunc(display);
   // sets the reshape callback for the current window. [Resize Event]
   glutReshapeFunc(onResize);
                     // KEYBOARD REGISTRATION //
   // If a user presses, releases any key on the keyboard respectively that has an ASCII code such as letters, numbers,space bar, Tab, Enter key, Escape key. [ASCII(printable) Keyboard Events]
   glutKeyboardFunc(onKeyDown); glutKeyboardUpFunc(onKeyUp);
   // F1, F2, Arrow keys are not ASCII keys. They are treated specially. To detect the key presses and releases for those special(control) keys. [Special(control) Keyboard Events]
   glutSpecialFunc(onSpecialKeyDown); glutSpecialUpFunc(onSpecialKeyUp);
                     // MOUSE REGISTRATION //
   // sets the mouse callback for the current window. [Mouse Click Event]
   glutMouseFunc(onClick);
   // When the user moves the mouse pointer within the window. Each movement results in a new event. [Mouse Passive Move Event]
   glutPassiveMotionFunc(onMove);
   // When the user moves the mouse pointer while any of the mouse button is down. [Mouse Motion Event]
   glutMotionFunc(onMoveDown);
   #if  TIMER_ON == true
   // timer event
   glutTimerFunc( TIMER_PERIOD, onTimer, 0 );
   #endif
   Init();
   glutMainLoop(); // Starts Event Dispatcher
}