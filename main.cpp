#ifdef _APPLE_
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

#include <stdlib.h>
#include <sstream>
#include <string>
#include <windows.h>
#include <iostream>
#include <conio.h>
#include <math.h>
#include <GL/freeglut.h>

// keycodes
#define VK_A 0x41
#define VK_D 0x44
#define VK_W 0x57
#define VK_S 0x53
#define PI 3.1415926535897932384626433832795

int width = 700;
int height = 700;
int Rad = 150;

int racket_width = 100;
int racket_height = 10;
int racket_speed = 4;

//nets
int net_width = 300;
int net_height = 10;

// down racket position
float racket_down_x = 350.0f - racket_width/2;
float racket_down_y = 20.0f;

// up racket position
float racket_up_x = 350.0f - racket_width/2;
float racket_up_y = height - 20;

int interval = 1000 / 60;
// ball
float ball_position_x = width / 2;
float ball_position_y = height / 2;
float ball_dir_x = 0.0f;
float ball_dir_y = -1.0f;
int ball_size = 8;
int ball_speed = 5;
// score
int score_down = 0;
int score_up = 0;

static void idle(void)
{
    glutPostRedisplay();
}

void drawRect(float x, float y, float width, float height)
{
    glBegin(GL_QUADS);
    glVertex2f(x, y);
    glVertex2f(x + width, y);
    glVertex2f(x + width, y + height);
    glVertex2f(x, y + height);
    glEnd();
}
void DrawHalfCircle (float cx, float cy, float r, int num_sg){
 glBegin(GL_LINE_LOOP);
    for (int ii = 0; ii < num_sg; ii++)   {
        float theta = 3.1415926f * float(ii) / float(num_sg);//get the current angle
        float x = Rad * cosf(theta);//calculate the x component
        float y = Rad * sinf(theta);//calculate the y component
        glVertex2f(x + cx, y + cy);//output vertex
    }
    glEnd();
}
void vec2_norm(float& x, float &y)
{
    // sets a vectors length to 1 (which means that x + y == 1)
    float length = sqrt((x * x) + (y * y));
    if (length != 0.0f)
    {
        length = 1.0f / length;
        x *= length;
        y *= length;
    }
}
void updateBall()
{
    ball_position_x += ball_dir_x * ball_speed;
    ball_position_y += ball_dir_y * ball_speed;

    // hit by down racket?
    if (ball_position_y < racket_down_y + racket_height && ball_position_x > racket_down_x &&
        ball_position_x < racket_down_x + racket_width && ball_position_y > racket_down_y)
    {
        // set fly direction depending on where it hit the racket
        // (t is 0.5 if hit at lefr, 0 at center, -0.5 at right)
        float t = ((ball_position_x - racket_down_x) / racket_width) - 0.5f;
        ball_dir_y = fabs(ball_dir_y); // force it to be positive
        ball_dir_x = t;
    }

    // hit by upper racket?

    if (ball_position_y > racket_up_y - racket_height && ball_position_x > racket_up_x &&
        ball_position_x < racket_up_x + racket_width && ball_position_y < racket_up_y)
    {
        // set fly direction depending on where it hit the racket
         // (t is 0.5 if hit at lefr, 0 at center, -0.5 at right)
        float t = ((ball_position_x - racket_up_x) / racket_width) - 0.5f;
        ball_dir_y = -fabs(ball_dir_y); // force it to be negative
        ball_dir_x = t;
    }

    // hit down wall?
    if (ball_position_x < 0)
    {
        ball_dir_x = fabs(ball_dir_x); // force it to be negative
    }

    // hit up wall?
    if (ball_position_x > width)
    {
        ball_dir_x = -fabs(ball_dir_x); // force it to be positive
    }

    // hit top wall?
    if (ball_position_y > height)
    {
        //in net
        if (ball_position_x > width/2-net_width/2 && ball_position_x <  width/2-net_width/2 + net_width)
            {
            ++score_up;
            ball_position_x = width / 2;
            ball_position_y = height / 2;
            ball_dir_y = -fabs(ball_dir_y); // force it to be positive
            ball_dir_x = 0;
            }
        else
           ball_dir_y = -fabs(ball_dir_y);

    }

    // hit bottom wall?
    if (ball_position_y < 0)
    {
        //in net
        if(ball_position_x > width/2-net_width/2 && ball_position_x <  width/2-net_width/2 + net_width)
        {
        ++score_down;
        ball_position_x = width / 2;
        ball_position_y = height / 2;
        ball_dir_y = fabs(ball_dir_y); // force it to be negative
        ball_dir_x = 0;
        }
        else
            ball_dir_y = fabs(ball_dir_y);
    }

    // length of dir stays at 1
    vec2_norm(ball_dir_x, ball_dir_y);

}
void enable2D(int width, int height)
{
    glViewport(0, 0, width, height);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    glOrtho(0.0f, width, 0.0f, height, 0.0f, 1.0f);
    glMatrixMode (GL_MODELVIEW);
    glLoadIdentity();
}
std::string int2str(int x)
{
    // converts int to string
    std::stringstream ss;
    ss << x;
    return ss.str( );
}
void drawText(float x, float y, std::string text)
{
    glColor3f(0, 1, 0);
    glRasterPos2f(x, y);
    glutBitmapString(GLUT_BITMAP_HELVETICA_18, (const unsigned char*)text.c_str());
    glColor3f(1, 1, 1);
}
void keyboard()
{
    // down racket
    if (GetAsyncKeyState(VK_A) && racket_down_x > 0)
        racket_down_x -= racket_speed;
    if (GetAsyncKeyState(VK_D) && racket_down_x + racket_width < width)
        racket_down_x += racket_speed;
    if (GetAsyncKeyState(VK_S) && racket_down_y > 0)
        racket_down_y -= racket_speed;
    if (GetAsyncKeyState(VK_W) && racket_down_y < height/2 - 50)
        racket_down_y += racket_speed;


    // up racket
    if (GetAsyncKeyState(VK_LEFT) && racket_up_x > 0)
        racket_up_x -= racket_speed;
    if (GetAsyncKeyState(VK_RIGHT)&& racket_up_x + racket_width < width)
        racket_up_x += racket_speed;
    if (GetAsyncKeyState(VK_DOWN) && racket_up_y > height/2 + 50)
        racket_up_y -= racket_speed;
    if (GetAsyncKeyState(VK_UP)   && racket_up_y + racket_height< height)
        racket_up_y  += racket_speed;
}

void update(int value)
{
    keyboard();
    updateBall();
    // Call update() again in 'interval' milliseconds
    glutTimerFunc(interval, update, 0);

    // Redisplay frame
    glutPostRedisplay();
}
const int sides = 20;  // The amount of segment to create the circle
const double radius = 5; // The radius of the circle

void DrawCircle(float cx, float cy, float r, int num_segments) {
    glBegin(GL_LINE_LOOP);

    for (int ii = 0; ii < num_segments; ii++)   {
        float theta = 2.0f * 3.1415926f * float(ii) / float(num_segments);//get the current angle
        float x = r * cosf(theta);//calculate the x component
        float y = r * sinf(theta);//calculate the y component
        glVertex2f(x + cx, y + cy);//output vertex
    }
    glEnd();
}
void drawTable()
{

    glColor3f(0, 1, 1);
    //drawRect(0, height/2 , width, 2);
    DrawCircle(width/2, height/2,50,100);

    glPushMatrix();
    glScalef(0.5,0.5,0.0);

    DrawCircle(width, height,50,100);
    glPopMatrix();


    glColor3f(0, 0, 1);
    glPointSize(15.0);
    glEnable(GL_POINT_SMOOTH);
    glBegin (GL_POINTS);
    glVertex2f(width / 2,height/2);
    glDisable(GL_POINT_SMOOTH);
    glEnd();

    //central line
    glColor3f(0.8,1.0,1.0);
    glLineWidth(3);
    glBegin(GL_LINES);
    glVertex2f(0,height/2);
    glVertex2f(width,height/2);
    glEnd();

    //upper line
    glPushMatrix();
    glTranslated(0,50,0);
    glColor3f(0, 0, 1);
    glBegin(GL_LINES);
    glVertex2f(0,height/2);
    glVertex2f(width,height/2);
    glEnd();
    glPopMatrix();

    //line under
    glPushMatrix();
    glTranslated(0,-50,0);
    glBegin(GL_LINES);
    glVertex2f(0,height/2);
    glVertex2f(width,height/2);
    glEnd();

    glPopMatrix();
    //left line
    glPushMatrix();
    glTranslated(352,0,0);
    glRotated(90.,0,0,1);
    glPushMatrix();
    glColor3f(0.8,1.0,1.0);
    glBegin(GL_LINES);
    glVertex2f(0,height/2);
    glVertex2f(width,height/2);
    glEnd();

    glPopMatrix();

    //right line
    glPushMatrix();
    glTranslated(352+width-3,0,0);
    glRotated(90.,0,0,1);
    glPushMatrix();
    glColor3f(0.8,1.0,1.0);
    glBegin(GL_LINES);
    glVertex2f(0,height/2);
    glVertex2f(width,height/2);
    glEnd();

    glPopMatrix();

    //down down line
    glPushMatrix();
    glTranslated(0,height/2-3,0);
    glColor3f(0.8,1.0,1.0);
    glBegin(GL_LINES);
    glVertex2f(0,height/2);
    glVertex2f(width,height/2);
    glEnd();
    glPopMatrix();

    //up up line
    glPushMatrix();
    glTranslated(0,-height/2+3,0);
    glColor3f(0.8,1.0,1.0);
    glBegin(GL_LINES);
    glVertex2f(0,height/2);
    glVertex2f(width,height/2);
    glEnd();
    glPopMatrix();


    glColor3f(1, 0, 0.1);

    //nets
    drawRect(width/2-net_width/2, -5, net_width, net_height);

    glPushMatrix();
    glTranslated(0,height,0);
    drawRect(width/2-net_width/2, -5, net_width, net_height);
    glPopMatrix();

    DrawHalfCircle(width/2, height-700 , 3, 1000);

    glPushMatrix();
    glTranslated(700,height+350,0);
    glRotated(180.,0,0,1);
    DrawHalfCircle(width/2, height/2, 30, 1000);
    glPopMatrix();

    glColor3f(1, 1, 1);


    glRasterPos2f(280., 450.);
    glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)"AIR HOCKEY");

    glRasterPos2f(250.,200.);
    glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)"by Eduard Barbulescu");
    glRasterPos2f(250.,170.);
    glutBitmapString(GLUT_BITMAP_TIMES_ROMAN_24, (const unsigned char*)"& Luca Mihai Calin");


}
void drawBall()
{
    glPointSize(15.0);
    glEnable(GL_POINT_SMOOTH);
    glBegin (GL_POINTS);
    glVertex2f(ball_position_x,ball_position_y);
    glEnd();
    glDisable(GL_POINT_SMOOTH);
}
void draw()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();

    //draw table
    drawTable();

    // draw rackets
    drawRect(racket_down_x, racket_down_y, racket_width, racket_height);
    drawRect(racket_up_x, racket_up_y, racket_width, racket_height);
    // draw ball (

    drawBall();
    //draw score
    drawText(width - 80, height - 35, int2str(score_down) + ":" + int2str(score_up));

    glutSwapBuffers();

}

int main(int argc, char *argv[])
{
    glutInit(&argc, argv);
    glutInitWindowSize(height,width);
    glutInitWindowPosition(10,10);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutCreateWindow("Air hockey");

    //glutReshapeFunc(resize);
    glutDisplayFunc(draw);
    glutTimerFunc(interval, update, 0);
    //glutKeyboardFunc(key);
    glutIdleFunc(idle);

    enable2D(width, height);
    glColor3f(1.0f, 1.0f, 1.0f);
    glutMainLoop();

    return EXIT_SUCCESS;
}
