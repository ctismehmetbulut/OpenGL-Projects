/***********************************************************************************
         CTIS164 - Official Homework 2
====================================================================================
STUDENT : Mehmet Bulut 21903372
SECTION : 002
HOMEWORK : Official Homework 2 for 2021 Spring Semester (Submission: 22 March 2021)
====================================================================================
PROBLEMS : 
*** KEYNOTE for "3. RECALL (MEMORY)"
   Although recall functionality tested many times, user should be cautious that 
   rapid changes while drawing the shapes (or **ghosting) may cause instability and result in undefined behaviour or even can crash the program due to wrong memory allocation.
KEYNOTE2: 
In order for **ghost drawing to handled by the program, while changing the type of the shape; user ought to move the cursor.
====================================================================================
ADDITIONAL FEATURES :

1. EXTRA SHAPES
Apart from lines, user can draw "triangles" with 3 points, "quadrangles" with 4 points, "rectangles" with 2 points, "circles" with 1 point
1.2. Radius of the circle can be increased or decreased with "up" or "down" arrow keys.

2. **GHOST DRAWING
With the help of drawGhost(), initializeGhost(), onMove() functions user will be able to see the possible outcome of his/her drawing while moving the cursor.

3. ***RECALL (MEMORY)
If user want to continue with another shape without completing the current shape, he/she can draw another shape(s), and 
when he/she select that unfinished shape type again (with the F keys), he/she can continue his/her drawing of that shape without hustle.
=> It works both with drawing shapes and ghost drawing

4. MORE PROGRAMMER READABLE CIRCLE FUNCTIONS
drawCircle(), drawCircleGhost(), drawSelector() functions created with an "angle" style instead of default "PI" style after researched about parametric circle equations.
The purpose is to make it more readable from a programmer's percpective. 
4.1. drawCircle(), drawCircleGhost() functions uses point_t structure type to call by value with less verbose code ("point_t point" instead of "int x" and "int y").

5. BAR DRAWING DISABLED
In the onClick() function user is prohibited from creating shapes in the top and bottom bars.

6. PERFORMANCE FIX
Dynamic memory allocation is commonly used to improve performance and prevent "memory leakage"
e.g: shape, allpoints, circle_points

7. DISPLAY MOUSE POSITION
Mouse position is display at the right corner of the bottom bar.
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

#define WINDOW_WIDTH  1200
#define WINDOW_HEIGHT 800

#define TIMER_PERIOD  1000 // Period for the timer.
#define TIMER_ON         0 // 0:disable timer, 1:enable timer

#define COLOR 255.0
#define LIMIT 30

#define D2R 0.0174532
#define PI 3.1415

/* Global Variables for Template File */
bool up = false, down = false, right = false, left = false;
int  winWidth, winHeight; // current Window width and height

            // Mehmet Bulut's Global Variables
// TO CONTTINUE WHERE THE SHAPE LEFT
bool is_it_new = true;
bool programstart;
int cnt_click = -1;
int cnt_shape = -1; // -1
bool change_happened = false;

typedef enum {start, end, line, triangle, quadrangle, rectangle, mycircle}type_n;

// FOR F SPECIAL KEYS
type_n currentF = start, previousF = start;

typedef struct{
   int x, y;
} point_t;

typedef struct{
   point_t point[4];
   type_n type;
   int last_corner;
   bool finish;
} shape_t;

typedef struct{
   point_t p;
   bool finish;
} point_detail_t;

// CAUSE MEMORY PROBLEM
shape_t *shape = (shape_t *)malloc(sizeof(shape_t)*LIMIT);
point_t *allpoints = (point_t *)malloc(sizeof(point_t)*LIMIT);
shape_t ghost;

point_t p_onClick;
point_t p_onMove;
point_detail_t special_start;
point_detail_t special_end;
int radius = 50;

void circle( int x, int y, int r )
{
// to draw circle, center at (x,y)
// radius r

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
   float angle;

   glBegin(GL_LINE_LOOP);
   for (int i = 0; i < 100; i++)
   {
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
   // vprint2(-50, 0, 0.35, "00:%02d", time_counter);
   glPointSize(1);
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

void drawLine(int x1, int y1, int x2, int y2, int linewidth, double r, double g, double b)
{
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glColor4f( r, g, b , 0.7);
   glLineWidth(linewidth);
   glBegin(GL_LINES);
      glVertex2f(x1, y1);
      glVertex2f(x2, y2);
   glEnd();
   glDisable(GL_BLEND);
}

void drawLineForGrid(int x1, int y1, int x2, int y2, int linewidth, double r, double g, double b)
{
   glColor4f( r, g, b , 0.5);
   glLineWidth(linewidth);
   glBegin(GL_LINES);
      glVertex2f(x1, y1);
      glVertex2f(x2, y2);
   glEnd();
}

void drawGrid(int width, int height, int distance)
{
   // VERTICAL LINES
   for(int x = -width; x <= width; x += distance)
      drawLineForGrid(x, height, x, -height, 1, 0, 1, 1);
   // HORIZONTAL LINES
   for(int y = -height; y <= height; y += distance)
      drawLineForGrid(width, y, -width, y, 1, 0, 1, 0);
}

void drawTriangle(point_t p1, point_t p2, point_t p3, double r, double g, double b)
{
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glColor4f( r, g, b, 0.7);
   glBegin(GL_TRIANGLES);
      glVertex2f(p1.x, p1.y);
      glVertex2f(p2.x, p2.y);
      glVertex2f(p3.x, p3.y);
   glEnd();
   glDisable(GL_BLEND);
}

void drawTriangleGhost(point_t p1, point_t p2, point_t p3, int linewidth, double r, double g, double b)
{
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glColor4f( r, g, b, 0.7);
   glLineWidth(linewidth);
   glBegin(GL_LINE_LOOP);
      glVertex2f(p1.x, p1.y);
      glVertex2f(p2.x, p2.y);
      glVertex2f(p3.x, p3.y);
   glEnd();
   glDisable(GL_BLEND);
}

void drawQuadrangle(point_t p1, point_t p2, point_t p3, point_t p4, double r, double g, double b)
{
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glColor4f( r, g, b, 0.7 );
   glBegin( GL_QUADS ); 
      glVertex2f(p1.x, p1.y);
      glVertex2f(p2.x, p2.y);
      glVertex2f(p3.x, p3.y);
      glVertex2f(p4.x, p4.y);
   glEnd();
   glDisable(GL_BLEND);
}

void drawQuadrangleGhost(point_t p1, point_t p2, point_t p3, point_t p4, int linewidth, double r, double g, double b)
{
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glColor4f( r, g, b, 0.7 );
   glLineWidth(linewidth);
   glBegin( GL_LINE_LOOP );
      glVertex2f(p1.x, p1.y);
      glVertex2f(p2.x, p2.y);
      glVertex2f(p3.x, p3.y);
      glVertex2f(p4.x, p4.y);
   glEnd();
   glDisable(GL_BLEND);
}

void drawCircle(point_t p, double radius, double r, double g, double b)
{
   double angle = PI/180;
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glColor4f( r, g, b, 0.7 );
   glLineWidth(3);
   glBegin( GL_POLYGON );
   for(int t = 0; t <= 360; t+=5)
      glVertex2f(radius*cos(t*angle) + p.x, radius*sin(t*angle) + p.y);
   glEnd();
   glDisable(GL_BLEND);
}

void drawCircleWire(point_t p, double radius, double r, double g, double b)
{
   double angle = PI/180;
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glColor4f( r, g, b, 0.7 );
   glLineWidth(3);
   glBegin( GL_LINE_STRIP );
   for(int t = 0; t <= 360; t+=5)
      glVertex2f(radius*cos(t*angle) + p.x, radius*sin(t*angle) + p.y);
   glEnd();
   glDisable(GL_BLEND);
}

void drawCircleGhost(point_t p, double radius, double r, double g, double b)
{
   double angle = PI/180;
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   glColor4f( r, g, b, 0.7 );
   glLineWidth(2);
   glBegin( GL_LINE_STRIP );
   for(int t = 0; t <= 360; t+=5)
      glVertex2f(radius*cos(t*angle) + p.x, radius*sin(t*angle) + p.y);
   glEnd();
   glDisable(GL_BLEND);
}

void drawSelector(int x, int y)
{
   x+=15;
   y+=5;
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
   double angle = PI/180;
   glColor4f(1, 1, 0, 0.5);
   glBegin( GL_POLYGON );
   for(int t = 0; t <= 360; t+=5)
      glVertex2f(20*cos(t*angle) + x, 20*sin(t*angle) + y);
   glEnd();
   glDisable(GL_BLEND);
   glColor3f(1, 1, 1);
}

void drawSpecial(point_t p, char ch)
{
   drawCircle( p, 40, 1, 1, 1 );
   drawCircleWire( p, 40, 0, 0, 0 );
   glColor3f(0, 0, 0);
   glLineWidth(3);
   vprint2(p.x - 20, p.y - 20, 0.3, "%c", ch);
}

void tempPrint()
{
   for(int i = 0; i <= cnt_shape && i < LIMIT; i++)
   {
      printf("i: %d type: %d finish: %d last_corner: %d\n", i, shape[i].type, shape[i].finish, shape[i].last_corner);
   }
   printf("**************************************************\n");
   for(int i = 0; i <= cnt_shape && i < LIMIT; i++)
   {
      for(int k = 0; k <= shape[i].last_corner; k++)
         printf("shape[%d].point[%d].x: %4d\tshape[%d].point[%d].y: %4d\n", i, k, shape[i].point[k].x, i, k, shape[i].point[k].y);
   }
   printf("\nchange_happened: %d\tcnt_shape: %d\n", change_happened, cnt_shape); 
   printf("is_it_new: %d\n", is_it_new);
   printf("\n============================================================================\n\n");
}

void swapPoints(point_t *p1, point_t *p2)
{
   point_t tempPoint;
   tempPoint = *p1;
   *p1 = *p2;
   *p2 = tempPoint;
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

void drawShapes()
{
   double r, g, b;
   // DRAW SHAPES
   for(int i = 0; i <= cnt_shape && i < LIMIT; i++)
   {
   if(shape[i].finish == true)
      if(special_end.finish && special_start.finish)
      {
         if(shape[i].type == line)
         { 
            if( intersect(shape[i].point[0], shape[i].point[1], special_start.p, special_end.p) )
               r = 1, g = 0, b = 0;
            else
               r = 1, g = 1, b = 0;
            drawLine(shape[i].point[0].x, shape[i].point[0].y, shape[i].point[1].x, shape[i].point[1].y, 3, r, g, b);
         }
         if(shape[i].type == triangle)
         {
            // ASSUMPTION STARTED AS THERE IS NO INTERSECTION
            bool flag = false; 
            for(int k = 0; k <= 2; k++)
               if( intersect(shape[i].point[k % 3], shape[i].point[ (k+1) % 3], special_start.p, special_end.p) )
                  flag = true; // IF INTERSECTION EXIST
            
            if(flag)
               r = 1, g = 0, b = 0;
            else
               r = 1, g = 1, b = 0;
            drawTriangle(shape[i].point[0], shape[i].point[1], shape[i].point[2], r, g, b);
         }
         if(shape[i].type == quadrangle)
         {
            // ASSUMPTION STARTED AS THERE IS NO INTERSECTION
            bool flag = false; 
            for(int k = 0; k <= 3; k++)
               if( intersect(shape[i].point[k % 4], shape[i].point[ (k+1) % 4], special_start.p, special_end.p) )
                  flag = true; // IF INTERSECTION EXIST
            
            if(flag)
               r = 1, g = 0, b = 0;
            else
               r = 1, g = 1, b = 0;
            drawQuadrangle(shape[i].point[0], shape[i].point[1], shape[i].point[2], shape[i].point[3], r, g, b);
         }
         if(shape[i].type == rectangle)
         {
            point_t side, p_M, p_K;

            // abs MATH FUNCTION IS USED TO PREVENT side.x FROM BEING NEGATIVE INTEGER
            side.x = abs( shape[i].point[1].x - shape[i].point[0].x );

            // TWO VIRTUAL POINT FOR THE RECTANGLE SO THAT INTERSECTION WILL BE APLICABLE
            p_M.y = shape[i].point[0].y;
            p_K.y = shape[i].point[1].y;

            if(shape[i].point[0].x < shape[i].point[1].x)
            {
               p_K.x = shape[i].point[1].x - side.x;
               p_M.x = shape[i].point[0].x + side.x;
            }
            else
            {
               p_K.x = shape[i].point[1].x + side.x;
               p_M.x = shape[i].point[0].x - side.x;
            }

            if( intersect(shape[i].point[0], p_M, special_start.p, special_end.p) )
               r = 1, g = 0, b = 0;
            else if( intersect(p_M, shape[i].point[1], special_start.p, special_end.p) )
               r = 1, g = 0, b = 0;
            else if( intersect(shape[i].point[1], p_K, special_start.p, special_end.p) )
               r = 1, g = 0, b = 0;
            else if( intersect(p_K, shape[i].point[0], special_start.p, special_end.p) )
               r = 1, g = 0, b = 0;
            else
               r = 1, g = 1, b = 0;

            glColor3f(r, g, b);
            glRectf(shape[i].point[0].x, shape[i].point[0].y, shape[i].point[1].x, shape[i].point[1].y);
         }
         if(shape[i].type == mycircle)
         {
            int t_diff = 5;
            point_t *circle_points = (point_t *)malloc(sizeof(point_t)*(360/t_diff)); 

            bool flag = false;
            double angle = PI/180;
            int m = 0;
            for(int t = 0; t < 360; t+= t_diff)
            {
               circle_points[m].x = (radius+1)*cos(t*angle) + shape[i].point[0].x;
               circle_points[m].y = (radius+1)*sin(t*angle) + shape[i].point[0].y; 
               ++m;
            }
            int u;
            for(int n = 0; n < 360/t_diff; n++)
            {
               u = (n+1)%(360/t_diff);
               if( intersect(circle_points[n], circle_points[u], special_start.p, special_end.p) )
                  flag = true;
            }
            free(circle_points);
            if(flag)
               r = 1, g = 0, b = 0;
            else
               r = 1, g = 1, b = 0;
            drawCircle(shape[i].point[0], radius, r, g, b);
         }
      }
      else
      {
         r = 1, g = 1, b = 0;
         if(shape[i].type == line)
            drawLine(shape[i].point[0].x, shape[i].point[0].y, shape[i].point[1].x, shape[i].point[1].y, 3, r, g, b);
         if(shape[i].type == triangle)
            drawTriangle(shape[i].point[0], shape[i].point[1], shape[i].point[2], r, g, b);
         if(shape[i].type == quadrangle)
            drawQuadrangle(shape[i].point[0], shape[i].point[1], shape[i].point[2], shape[i].point[3], r, g, b);
         if(shape[i].type == rectangle)
         {
            glColor3f(r, g, b);
            glRectf(shape[i].point[0].x, shape[i].point[0].y, shape[i].point[1].x, shape[i].point[1].y);
         }
         if(shape[i].type == mycircle)
            drawCircle(shape[i].point[0], radius, r, g, b);
      }
   }
   // DRAW SPECIALS (start, end)
   glColor3f(1, 1, 1);
   if(special_end.finish && special_start.finish)
      drawLine(special_start.p.x, special_start.p.y, special_end.p.x, special_end.p.y, 3, 0, 0, 0);
   if(special_start.finish)
      drawSpecial(special_start.p, 'S');
   if(special_end.finish)
      drawSpecial(special_end.p, 'E');

   glLineWidth(1);
   // DRAW POINTS
   for(int i = 0; i <= cnt_click && i < LIMIT; i++)
      drawCircle( allpoints[i], 6 , 0, 0, 0);
   {
      /*glColor3f(1, 1, 0);
      vprint2( allpoints[i].x, allpoints[i].y, 0.3,"%d", i);
      glColor3f(1, 1, 1);
      vprint2( allpoints[i].x + 30, allpoints[i].y, 0.2,"(%d, %d)", allpoints[i].x,allpoints[i].y); */
   }
}

void drawGhost()
{
   glEnable (GL_LINE_STIPPLE);
   glLineStipple(4, 0xAAAA);
   /*
   // DRAW GHOST POINTS
   for(int i = 0; i <= cnt_click && i < LIMIT; i++)
   {
      glColor3f(1, 0, 0);
      circle( allpoints[i].x, allpoints[i].y, 10 );
      glColor3f(1, 1, 0);
      vprint2( allpoints[i].x, allpoints[i].y, 0.3,"%d", i);
      glColor3f(1, 1, 1);
      vprint2( allpoints[i].x + 30, allpoints[i].y, 0.2,"(%d, %d)", allpoints[i].x,allpoints[i].y);
   }
*/
   double r, g, b;
   // DRAW GHOST
   {
   if( ghost.finish == false )
      if( special_end.finish && special_start.finish )
      {
         if(ghost.type == line)
         { 
            if( intersect(ghost.point[0], p_onMove, special_start.p, special_end.p) )
               r = 1, g = 0, b = 0;
            else
               r = 1, g = 1, b = 0;
            drawLine(ghost.point[0].x, ghost.point[0].y, p_onMove.x, p_onMove.y, 1, r, g, b);
         }
         if(ghost.type == triangle)
         {
            if(ghost.last_corner == 0)
            {
               if( intersect(ghost.point[0], p_onMove, special_start.p, special_end.p) )
                  r = 1, g = 0, b = 0;
               else
                  r = 1, g = 1, b = 0;
               drawLine(ghost.point[0].x, ghost.point[0].y, p_onMove.x, p_onMove.y, 1, r, g, b);
            }
            else if(ghost.last_corner == 1)
            {
               if( intersect(ghost.point[0], p_onMove, special_start.p, special_end.p) || intersect(ghost.point[1], p_onMove, special_start.p, special_end.p) || intersect(ghost.point[0], ghost.point[1], special_start.p, special_end.p))
                  r = 1, g = 0, b = 0;
               else
                  r = 1, g = 1, b = 0;
               drawTriangleGhost(ghost.point[0], ghost.point[1], p_onMove, 2, r, g, b);
            }
         }
         if(ghost.type == quadrangle)
         {
            if(ghost.last_corner == 0)
            {
               if( intersect(ghost.point[0], p_onMove, special_start.p, special_end.p) )
                  r = 1, g = 0, b = 0;
               else
                  r = 1, g = 1, b = 0;
               drawLine(ghost.point[0].x, ghost.point[0].y, p_onMove.x, p_onMove.y, 1, r, g, b);
            }
            else if(ghost.last_corner == 1)
            {
               if( intersect(ghost.point[0], p_onMove, special_start.p, special_end.p) || intersect(ghost.point[1], p_onMove, special_start.p, special_end.p) || intersect(ghost.point[0], ghost.point[1], special_start.p, special_end.p))
                  r = 1, g = 0, b = 0;
               else
                  r = 1, g = 1, b = 0;
               drawTriangleGhost(ghost.point[0], ghost.point[1], p_onMove, 2, r, g, b);
            }
            else if(ghost.last_corner == 2)
            {
               if( intersect(ghost.point[0], p_onMove, special_start.p, special_end.p) || intersect(ghost.point[2], p_onMove, special_start.p, special_end.p)  || intersect(ghost.point[1], ghost.point[2], special_start.p, special_end.p) || intersect(ghost.point[0], ghost.point[1], special_start.p, special_end.p))
                  r = 1, g = 0, b = 0;
               else
                  r = 1, g = 1, b = 0;
               drawQuadrangleGhost(ghost.point[0], ghost.point[1], ghost.point[2], p_onMove, 2, r, g, b);
            }
            
            
            
         }
         if(ghost.type == rectangle)
         {
            point_t side, p_M, p_K;

            // abs MATH FUNCTION IS USED TO PREVENT side.x FROM BEING NEGATIVE INTEGER
            side.x = abs( p_onMove.x - ghost.point[0].x );

            // TWO VIRTUAL POINT FOR THE RECTANGLE SO THAT INTERSECTION WILL BE APLICABLE
            p_M.y = ghost.point[0].y;
            p_K.y = p_onMove.y;

            if(ghost.point[0].x < p_onMove.x)
            {
               p_K.x = p_onMove.x - side.x;
               p_M.x = ghost.point[0].x + side.x;
            }
            else
            {
               p_K.x = p_onMove.x + side.x;
               p_M.x = ghost.point[0].x - side.x;
            }

            if( intersect(ghost.point[0], p_M, special_start.p, special_end.p) )
               r = 1, g = 0, b = 0;
            else if( intersect(p_M, p_onMove, special_start.p, special_end.p) )
               r = 1, g = 0, b = 0;
            else if( intersect(p_onMove, p_K, special_start.p, special_end.p) )
               r = 1, g = 0, b = 0;
            else if( intersect(p_K, ghost.point[0], special_start.p, special_end.p) )
               r = 1, g = 0, b = 0;
            else
               r = 1, g = 1, b = 0;

            glColor3f(r, g, b);
            drawQuadrangleGhost(ghost.point[0], p_M, p_onMove, p_K, 2, r, g, b);
         }
         if(currentF == mycircle)
         {
            int t_diff = 5;
            point_t *circle_points = (point_t *)malloc(sizeof(point_t)*(360/t_diff)); 

            bool flag = false;
            double angle = PI/180;
            int m = 0;
            for(int t = 0; t < 360; t+= t_diff)
            {
               circle_points[m].x = (radius+1)*cos(t*angle) + p_onMove.x;
               circle_points[m].y = (radius+1)*sin(t*angle) + p_onMove.y; 
               ++m;
            }
            int u;
            for(int n = 0; n < 360/t_diff; n++)
            {
               u = (n+1)%(360/t_diff);
               if( intersect(circle_points[n], circle_points[u], special_start.p, special_end.p) )
                  flag = true;
            }
            free(circle_points);
            if(flag)
               r = 1, g = 0, b = 0;
            else
               r = 1, g = 1, b = 0;
            drawCircleGhost(p_onMove, radius, r, g, b);
         }
      }
      else
      {
         r = 1, g = 1, b = 0;
         if(ghost.type == line)
            drawLine(ghost.point[0].x, ghost.point[0].y, p_onMove.x, p_onMove.y, 1, r, g, b);

         if(ghost.type == triangle)
            if(ghost.last_corner == 0)
               drawLine(ghost.point[0].x, ghost.point[0].y, p_onMove.x, p_onMove.y, 1, r, g, b);
            else if(ghost.last_corner == 1)
               drawTriangleGhost(ghost.point[0], ghost.point[1], p_onMove, 2, r, g, b);
   
         if(ghost.type == quadrangle)
            if(ghost.last_corner == 0)
               drawLine(ghost.point[0].x, ghost.point[0].y, p_onMove.x, p_onMove.y, 1, r, g, b);
            else if(ghost.last_corner == 1)
               drawTriangleGhost(ghost.point[0], ghost.point[1], p_onMove, 2, r, g, b);
            else if(ghost.last_corner == 2)
               drawQuadrangleGhost(ghost.point[0], ghost.point[1], ghost.point[2], p_onMove, 2, r, g, b);

         if(ghost.type == rectangle)
         {
            point_t side, p_M, p_K;
            // abs MATH FUNCTION IS USED TO PREVENT side.x FROM BEING NEGATIVE INTEGER
            side.x = abs( p_onMove.x - ghost.point[0].x );
            // TWO VIRTUAL POINT FOR THE RECTANGLE SO THAT INTERSECTION WILL BE APLICABLE
            p_M.y = ghost.point[0].y;
            p_K.y = p_onMove.y;

            if(ghost.point[0].x < p_onMove.x)
            {
               p_K.x = p_onMove.x - side.x;
               p_M.x = ghost.point[0].x + side.x;
            }
            else
            {
               p_K.x = p_onMove.x + side.x;
               p_M.x = ghost.point[0].x - side.x;
            }
            r = 1, g = 1, b = 0;
            glColor3f(r, g, b);
            drawQuadrangleGhost(ghost.point[0], p_M, p_onMove, p_K, 2, r, g, b);
         }
         if(currentF == mycircle)
            drawCircleGhost(p_onMove, radius, 1, 1, 0);
      }
   }
/*
   // DRAW GHOST SPECIALS (start, end)
   glColor3f(1, 1, 1);
   if(special_end.finish && special_start.finish)
      drawLine(special_start.p.x, special_start.p.y, special_end.p.x, special_end.p.y, 3, 0, 0, 0);
   if(special_start.finish)
      drawSpecial(special_start.p, 'S');
   if(special_end.finish)
      drawSpecial(special_end.p, 'E');
*/
   glDisable (GL_LINE_STIPPLE);
}

void displayCurrentState()
{
   glColor3f(1, 1, 1);
   switch(currentF)
   {
      case start:
         drawSelector(-WINDOW_WIDTH/2+20, -WINDOW_HEIGHT/2+20);
      break;
      case end:
         drawSelector( -WINDOW_WIDTH/2+20 + 1*WINDOW_WIDTH/12, -WINDOW_HEIGHT/2+20 );
      break;
      case line: 
         drawSelector( -WINDOW_WIDTH/2+20 + 2*WINDOW_WIDTH/12, -WINDOW_HEIGHT/2+20 );
      break;
      case triangle:
         drawSelector( -WINDOW_WIDTH/2+20 + 3*WINDOW_WIDTH/12, -WINDOW_HEIGHT/2+20 );
      break;
      case quadrangle:
         drawSelector( -WINDOW_WIDTH/2+20 + 4.5*WINDOW_WIDTH/12, -WINDOW_HEIGHT/2+20 );
      break; 
      case rectangle:
         drawSelector( -WINDOW_WIDTH/2+20 + 6.5*WINDOW_WIDTH/12, -WINDOW_HEIGHT/2+20 );
      break;
      case mycircle:
         drawSelector( -WINDOW_WIDTH/2+20 + 8*WINDOW_WIDTH/12, -WINDOW_HEIGHT/2+20 );
      break;
   }
   vprint( -WINDOW_WIDTH/2+20, -WINDOW_HEIGHT/2+20, GLUT_BITMAP_TIMES_ROMAN_24 , "F1: start");
   vprint( -WINDOW_WIDTH/2+20 + 1*WINDOW_WIDTH/12, -WINDOW_HEIGHT/2+20, GLUT_BITMAP_TIMES_ROMAN_24 , "F2: end");
   vprint( -WINDOW_WIDTH/2+20 + 2*WINDOW_WIDTH/12, -WINDOW_HEIGHT/2+20, GLUT_BITMAP_TIMES_ROMAN_24 , "F3: line");
   vprint( -WINDOW_WIDTH/2+20 + 3*WINDOW_WIDTH/12, -WINDOW_HEIGHT/2+20, GLUT_BITMAP_TIMES_ROMAN_24 , "F4: triangle");
   vprint( -WINDOW_WIDTH/2+20 + 4.5*WINDOW_WIDTH/12, -WINDOW_HEIGHT/2+20, GLUT_BITMAP_TIMES_ROMAN_24 , "F5: quadrangle");
   vprint( -WINDOW_WIDTH/2+20 + 6.5*WINDOW_WIDTH/12, -WINDOW_HEIGHT/2+20, GLUT_BITMAP_TIMES_ROMAN_24 , "F6: rectangle");
   vprint( -WINDOW_WIDTH/2+20 + 8*WINDOW_WIDTH/12, -WINDOW_HEIGHT/2+20, GLUT_BITMAP_TIMES_ROMAN_24 , "F7: circle");
}

void display()
{
   // OpenGL Coordinate System
// CALLER FUNCTION: glutDisplayFunc(display);
    glClearColor( 90/COLOR, 90/COLOR, 90/COLOR , 0 ); 
    glClear( GL_COLOR_BUFFER_BIT );
   // DRAW GRID
   drawGrid(WINDOW_WIDTH, WINDOW_HEIGHT, 50);
   // TOP BAR
   glColor3f( 0, 0, 0 );
   glRectf( -WINDOW_WIDTH/2, WINDOW_HEIGHT/2-49, WINDOW_WIDTH/2, WINDOW_HEIGHT/2 );
   glColor3f( 1, 1, 1 );
   vprint( 240, 370, GLUT_BITMAP_9_BY_15, "UP or DOWN KEYS to change radius: %d", radius);
   // BOTTOM BAR
   glColor3f( 0, 0, 0 );
   glRectf( -WINDOW_WIDTH/2, -WINDOW_HEIGHT/2, WINDOW_WIDTH/2, -WINDOW_HEIGHT/2+50 );
   // STUDENT INFO
   glColor3f( 1, 1, 1 );
   glLineWidth(1);
    vprint(-WINDOW_WIDTH/2 + 20, WINDOW_HEIGHT/2 - 30, GLUT_BITMAP_TIMES_ROMAN_24, "Mehmet Bulut 21903372");
    glColor3f(1, 1, 0);
    vprint(-WINDOW_WIDTH/2 + WINDOW_HEIGHT/2 + 100, 370, GLUT_BITMAP_TIMES_ROMAN_24, "HW #2 – 2020–2021 Spring"); 
    // MOUSE POSITION
   glColor3f( 1, 1, 1 );
   vprint( WINDOW_WIDTH - 180 - WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - WINDOW_HEIGHT + 30, GLUT_BITMAP_8_BY_13,"( x: %d", p_onMove.x);
   vprint( WINDOW_WIDTH - 90 - WINDOW_WIDTH / 2, WINDOW_HEIGHT / 2 - WINDOW_HEIGHT + 30, GLUT_BITMAP_8_BY_13,"y: %d )", p_onMove.y);
   // F SPECIAL KEY ENABLE/DISABLE DISPLAY
   displayCurrentState();
   // DRAW SHAPES, SPECIALS, POINTS
   if(programstart)
      drawShapes();
   // DRAW GHOST
   if(!(currentF == start || currentF == end))
      drawGhost();
    glutSwapBuffers();
}

void onKeyDown(unsigned char key, int x, int y )
{
// CALLER FUNCTION: glutKeyboardFunc(onKeyDown);
//   FUNCTION PROPERTIES: onKeyDown()
   // The x and y callback parameters indicate the mouse location in window relative coordinates when the key was pressed.
   // key: integer value of ASCII characters like ESC, a,b,c..,A,B,..Z etc.
   // exit when ESC is pressed.
   if ( key == 27 ) 
   {
      free(shape);
      free(allpoints);
      exit(0);
   }
   glutPostRedisplay(); // to refresh the window it calls display() function
}

void onKeyUp(unsigned char key, int x, int y )
{
   if ( key == 27 )
      exit(0);
   glutPostRedisplay() ;
}

void onSpecialKeyDown( int key, int x, int y )
{
/* CALLER FUNCTION: glutSpecialFunc(onSpecialKeyDown);
   FUNCTION DEFINITION: onSpecialKeyDown()
   // The x and y callback parameters indicate the mouse location in window relative coordinates when the key was pressed.
   // Special Key like GLUT_KEY_F1, F2, F3,...
   // Arrow Keys, GLUT_KEY_UP, GLUT_KEY_DOWN, GLUT_KEY_RIGHT, GLUT_KEY_RIGHT */
   switch(key) {
      case GLUT_KEY_F1: currentF = start; break; 
      case GLUT_KEY_F2: currentF = end; break;
      case GLUT_KEY_F3: currentF = line; break;
      case GLUT_KEY_F4: currentF = triangle; break;
      case GLUT_KEY_F5: currentF = quadrangle; break;
      case GLUT_KEY_F6: currentF = rectangle; break;
      case GLUT_KEY_F7: currentF = mycircle; break;
      case GLUT_KEY_UP: 
         if(radius < 200)
            radius+= 5; 
      break;
      case GLUT_KEY_DOWN: 
         if(radius > 30)
            radius-= 5; 
      break;
   }
   // IF CONDITION IS SATISFIED, IT MEANS CHANGE HAPPENED
      if(currentF != previousF ) // && corner != -1
         change_happened = true;
      
      previousF = currentF; 
   glutPostRedisplay() ; // to refresh the window it calls display() function
}
   
void onSpecialKeyUp( int key, int x, int y )
{
   glutPostRedisplay() ;
}

void initializeShapes()
{
   int temp_cnt_shape;
   if(cnt_shape == -1)
   {
      change_happened = false;
      ++cnt_shape;
   }
   // IF YOU PRESS ANOTHER SHAPE AND TRIGGER change_happened = true THEN else PART OF THE if(is_it_new) WILL TAKE THE CHARGE OF THE EVENT
   if(change_happened)
   {
      // TO CONTINUE WHERE ITS LEFT
      for(int i = 0; i <= cnt_shape && i < LIMIT; i++) // <= CHANGE <
      {
         if(shape[i].finish == false && shape[i].type == currentF)
         {
            temp_cnt_shape = i;
            is_it_new = false;
         }
      }
   }
   if(is_it_new) 
   {
      if(change_happened)
      {
      //printf("cnt_shape: %d => FINISHED!!\n", cnt_shape);
      change_happened = false;
      ++cnt_shape;
      }
      shape[cnt_shape].type = currentF;
      // MIDDLE
      ++(shape[cnt_shape].last_corner);
      //printf("is_it_new TRUE => shape[%d].last_corner: %d\n\n", cnt_shape, shape[cnt_shape].last_corner);
      shape[cnt_shape].point[shape[cnt_shape].last_corner] = p_onClick;
      // END
      if( shape[cnt_shape].type == line && shape[cnt_shape].last_corner == 1 || shape[cnt_shape].type == triangle && shape[cnt_shape].last_corner == 2 || shape[cnt_shape].type == rectangle && shape[cnt_shape].last_corner == 1 || shape[cnt_shape].type == mycircle && shape[cnt_shape].last_corner == 0 )
         shape[cnt_shape++].finish = true;
      else if(shape[cnt_shape].type == quadrangle && shape[cnt_shape].last_corner == 3) 
      {
         // QUADRANGLE SHAPE FIX
         if(intersect(shape[cnt_shape].point[0], shape[cnt_shape].point[3], shape[cnt_shape].point[1], shape[cnt_shape].point[2]))
            swapPoints(&shape[cnt_shape].point[2], &shape[cnt_shape].point[3]);
         else if(intersect(shape[cnt_shape].point[0], shape[cnt_shape].point[1], shape[cnt_shape].point[2], shape[cnt_shape].point[3]))
            swapPoints(&shape[cnt_shape].point[0], &shape[cnt_shape].point[3]);
         shape[cnt_shape++].finish = true;
      }
   }
   //===================================================================================================================
   else
   {
      is_it_new = true;
      // MIDDLE
      ++(shape[temp_cnt_shape].last_corner);
      //printf("is_it_new FALSE => shape[%d].last_corner: %d\n\n", temp_cnt_shape, shape[temp_cnt_shape].last_corner);
      shape[temp_cnt_shape].point[shape[temp_cnt_shape].last_corner] = p_onClick;
      // END
      if( shape[temp_cnt_shape].type == line && shape[temp_cnt_shape].last_corner == 1 || shape[temp_cnt_shape].type == triangle && shape[temp_cnt_shape].last_corner == 2 || shape[temp_cnt_shape].type == rectangle && shape[temp_cnt_shape].last_corner == 1 || shape[temp_cnt_shape].type == mycircle && shape[temp_cnt_shape].last_corner == 0 )
         shape[temp_cnt_shape].finish = true;
      else if(shape[temp_cnt_shape].type == quadrangle && shape[temp_cnt_shape].last_corner == 3)
      {
         // QUADRANGLE SHAPE FIX
         if(intersect(shape[temp_cnt_shape].point[0], shape[temp_cnt_shape].point[3], shape[temp_cnt_shape].point[1], shape[temp_cnt_shape].point[2]))
            swapPoints(&shape[temp_cnt_shape].point[2], &shape[temp_cnt_shape].point[3]);
         else if(intersect(shape[temp_cnt_shape].point[0], shape[temp_cnt_shape].point[1], shape[temp_cnt_shape].point[2], shape[temp_cnt_shape].point[3]))
            swapPoints(&shape[temp_cnt_shape].point[0], &shape[temp_cnt_shape].point[3]);
         shape[temp_cnt_shape].finish = true;
      }
   }
   //tempPrint();
   //printf("\n============================================================================\n\n");
}

void onClick( int button, int state, int x, int y )
{
// CALLER FUNCTION: glutMouseFunc(onClick);
// FUNCTION PROPERTIES: onClick()
   // button(s) == GLUT_LEFT_BUTTON , GLUT_MIDDLE_BUTTON, GLUT_RIGHT_BUTTON
   // state(s)  == GLUT_UP , GLUT_DOWN [are press and release cycle]
   // x, y  is the coordinate of the point that mouse clicked.
   if(button == GLUT_LEFT_BUTTON && state == GLUT_DOWN && y > 49 &&  y < 750 )
   {
      programstart = true;

      // CONVERSION FROM GLUT TO OPENGL
         p_onClick.x  = x - WINDOW_WIDTH / 2;
         p_onClick.y = WINDOW_HEIGHT / 2 - y;

      // CLICK INCREMENT
      if(!(currentF == start || currentF == end))
         cnt_click ++;

      if(cnt_click < LIMIT)
      {
         // INITIALIZATION OF POINTS AND SHAPES
         if(!(currentF == start || currentF == end))
         {
            *(allpoints + cnt_click) = p_onClick;
            initializeShapes();
         }
      }
      // INITIALIZATION OF SPECIALS
         if(currentF == start)
         {
            special_start.p = p_onClick;
            special_start.finish = true;
         }
         if(currentF == end)
         {
            special_end.p = p_onClick;
            special_end.finish = true;
         }
   }
// =====================================================
   glutPostRedisplay() ; 
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
//FUNCTION DEFINITION: glutMotionFunc()
   // set the motion callbacks for the current window.
   // When the user moves the mouse pointer while any of the mouse button is down.
//FUNCTION PROPERTIES: applicable for all glut functions
   // GLUT to OpenGL coordinate conversion:
   //  x - winWidth / 2 = x_openGL
   //  winHeight / 2 - y = y_openGL
   glutPostRedisplay() ; // to refresh the window it calls display() function 
}

void initializeGhost()
{
if(shape[cnt_shape].type != mycircle)
{
   ghost = shape[cnt_shape];
   // TO CONTINUE WHERE ITS LEFT
   bool flag = true;
   for(int i = 0; i <= cnt_shape && i < LIMIT && flag; i++) // <= CHANGE <
      if(shape[i].finish == false )
         if( shape[i].type == currentF )
         {
            ghost = shape[i];
            flag = false;
         }
   if(flag)
      ghost = shape[cnt_shape+1];

   // QUADRANGLE GHOST FIX
   if(ghost.type == quadrangle && ghost.last_corner == 2)
      if(intersect(ghost.point[0], p_onMove, ghost.point[1], ghost.point[2]))
         swapPoints(&ghost.point[2], &p_onMove);
      else if(intersect(ghost.point[0], ghost.point[1], ghost.point[2], p_onMove))
         swapPoints(&ghost.point[0], &p_onMove);
} 
   //printf("GHOST:: last_corner: %d\tfinish: %d\ttype: %d\t\tcurrentF: %d\tchange_happened: %d\tcnt_shape: %d\n", ghost.last_corner, ghost.finish, ghost.type, currentF, change_happened, cnt_shape);
}

void onMove( int x, int y ) 
{
// CALLER FUNCTION: glutPassiveMotionFunc()

   // Callbacks of x and y GLUT coordinates have been converted to OpenGL and globalized.
   p_onMove.x = x - WINDOW_WIDTH / 2; 
   p_onMove.y = WINDOW_HEIGHT / 2 - y;

   if(!(currentF == start || currentF == end))
      initializeGhost();
   // to refresh the window it calls display() function
   glutPostRedisplay() ;
}

#if TIMER_ON == 1
void onTimer( int v ) 
{
   // Timer Events
   glutTimerFunc( TIMER_PERIOD, onTimer, 0 );

   /*
      // PROPER USAGE OF ON TIMER EVENT
      // in the globe => int counter = 0;
      void onTimer(int v) 
      {
         counter++;
         if (counter <= duration) 
         {
            printf("%d clicks in %d seconds.\n", clicks, counter);
            glutTimerFunc(1000, onTimer, 0);
         }
         else //logically stop the timer (not invoke onTimer again)
            printf("End of counting.\n");
      }
   */

   // =====================================================
   // =============== WRITE YOUR CODES HERE ===============
   time_counter++;
   // =====================================================

   // to refresh the window it calls display() function
   glutPostRedisplay(); // display()
}
#endif

void Init() 
{
   for(int i = 0; i < LIMIT; i++)
   {
      shape[i].last_corner = -1;
      shape[i].finish = false;
      shape[i].type = start;
      for(int k = 0; k < 4; k++)
      {
         shape[i].point[k].x = 0;
         shape[i].point[k].y = 0;
      }
   }
   // Smoothing shapes
   glEnable(GL_BLEND);
   glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA); 
}

int main( int argc, char *argv[] ) {
   Init();

   glutInit(&argc, argv );
   glutInitDisplayMode( GLUT_RGB | GLUT_DOUBLE );
   glutInitWindowSize(WINDOW_WIDTH, WINDOW_HEIGHT);
   //glutInitWindowPosition(100, 100);

   // Enter name of the Window as string such as "Template File"
   glutCreateWindow("Official Homework 2");

   // sets the display callback for the current window. [Paint/Display Event]
   glutDisplayFunc(display);

   // sets the reshape callback for the current window. [Resize Event]
   glutReshapeFunc(onResize);

                     //                       //
                     // KEYBOARD REGISTRATION //
                     //                       //
   // If a user presses any key on the keyboard that has an ASCII code such as letters, numbers,space bar, Tab, Enter key, Escape key. [ASCII(printable) Keyboard Events]
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

   glutMainLoop();
}