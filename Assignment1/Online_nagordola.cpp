#include <bits/stdc++.h>

#ifdef __linux__
#include <GL/glut.h>
#include <GL/glu.h>
#include <GL/gl.h>
#elif WIN32
#include <glut.h>
#include <windows.h>
#endif

using namespace std;

/*
xz plane, y upword
*/

#define pi (2*acos(0.0))

double angle=0;

double camAX=0;
double camAY=1;
double camAZ=0;
double camLX=0;
double camLY=0;
double camLZ=0;
double camX=15;
double camY=15;
double camZ=15;

class point
{
    public:

	double x,y,z;
    point(){
        x=0;
        y=0;
        z=0;
    }

    point(double a, double b, double c){
        x=a;
        y=b;
        z=c;
    }

    point cross(point p){ //p x point
        point pp;
        pp.x=z*p.y-y*p.z;
        pp.y=x*p.z-z*p.x;
        pp.z=y*p.x-x*p.y;
        return pp;
    }

    void norm(){
        double sq=sqrt(x*x+y*y+z*z);
        x=x*1.0/sq;
        y=y*1.0/sq;
        z=z*1.0/sq;
    }

};


void init(){
    glClearColor(0,0,0.0,0);
    glEnable(GL_DEPTH_TEST);
}


void axis(){
    glPushMatrix();    
    glBegin(GL_LINES);
    glColor3f(.5,.9,.9);
    glVertex3f(-100,0,0);
    glVertex3f(100,0,0);
    glColor3f(.9,.5,.9);
    glVertex3f(0,100,0);
    glVertex3f(0,-100,0);
    glColor3f(.9,.9,.5);
    glVertex3f(0,0,100);
    glVertex3f(0,0,-100);
    glEnd();
    glPopMatrix();
}

void square(double a){
    glPushMatrix();
    glTranslatef(-a/2,0,-a/2);
    glBegin(GL_QUADS);
    glVertex3f(0,0,0);
    glVertex3f(0,0,a);
    glVertex3f(a,0,a);
    glVertex3f(a,0,0);
    glEnd();
    glPopMatrix();
}

void bound(double a,double l){
    glBegin(GL_QUADS);
    glVertex3f(-a,0,0);
    glVertex3f(a,0,0);
    glVertex3f(a,l,0);
    glVertex3f(-a,l,0);
    glEnd();
}

void boundary(){    
    for(int i=0; i<4; i++){
        glPushMatrix();
        glColor3f(1,0,0);
        glRotatef(90*i,0,1,0);
        glTranslatef(0,0,15);
        bound(15,3);
        glPopMatrix();
   }
}
void board(double a){
    int c;
    for(int i=-50; i<=50; i++){
        for(int j=-50; j<=50; j++){
            c=(i+j)%2;
            c=c*c;
            glColor3f(c,c,c);

            glPushMatrix();
            glTranslatef(i*a,0,j*a);
            square(a);
            glPopMatrix();
        }
    }
}


void cameraSpecialKey(int key, int x, int y){

    switch(key){
        case GLUT_KEY_UP:   //forward  
            camZ-=1;          
            break;
        case GLUT_KEY_DOWN: //backward
            camZ+=1;
            break;
        case GLUT_KEY_RIGHT:    //r
            break;
        case GLUT_KEY_LEFT: //l
            break;
        case GLUT_KEY_PAGE_UP:  //up
            break;
        case GLUT_KEY_PAGE_DOWN:    //down
            break;
        default:
            break;
    }

    
}

void keyboard(unsigned char key, int x, int y){
    switch(key){
        case '1':   //rot l  
            // rp.x=rig.x*cos(rotateSpeed * M_PI / 180.0f)+forw.x*sin(rotateSpeed * M_PI / 180.0f);
            if(angle<90)
            angle=angle+30;
            break;
        case '2':  //rot r
            if(angle>0)
            angle=angle-30;

            break;
        case '3':   //rot up
            break;
        case '4':    //rot down
            break;
        case '5':     //tilt anitclock
            break;
        case '6':   //tilt clock
            break;
        
        case 'w':
            break;
        case 's':
            break;
        default:
            break;
    }
}

void drawCircle(double radius,int segments)
{
    int i;
    struct point points[100];
    glColor3f(0.7,0.7,0.7);
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw segments using generated points
    for(i=0;i<segments;i++)
    {
        glBegin(GL_LINES);
        {
			glVertex3f(points[i].x,0,points[i].y);
			glVertex3f(points[i+1].x,0,points[i+1].y);
        }
        glEnd();
    }
}

void drawSphere(double radius,int slices,int stacks)
{
	 point points[100][100];
	int i,j;
	double h,r;
	//generate points
	for(i=0;i<=stacks;i++)
	{
		h=radius*sin(((double)i/(double)stacks)*(pi/2));
		r=radius*cos(((double)i/(double)stacks)*(pi/2));
		for(j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*pi);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*pi);
			points[i][j].z=h;
            points[i][j].norm();
            points[i][j].x*=radius;
            points[i][j].y*=radius;
            points[i][j].z*=radius;
		}
	}
	//draw quads using generated points
	for(i=0;i<stacks;i++)
	{
		for(j=0;j<slices;j++)
		{   
            if(j%2)
                glColor3f(1,0,1);
            else
                glColor3f(0,1,1);

			glBegin(GL_QUADS);{
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}

// void drawSp(){
//     glPushMatrix();
//     glColor3f(1,1,0);
//     glTranslatef(0,1.5,0);
//     drawSphere(1.5,8,20);
//     glPopMatrix();
// }

void drawSqLine(){
    glPushMatrix();
    glTranslatef(10,0,0);
    glRotatef(angle,0,0,1);

    glBegin(GL_LINES);
    glVertex3f(0,0,0);
    glVertex3f(1,-5,0);

    glVertex3f(0,0,0);
    glVertex3f(-1,-5,0);
    glEnd();
    
    glTranslatef(0,-5,0);
    square(2);
    glPopMatrix();
}

void drawSQL(){
    glPushMatrix();
    for(int i=0; i<6; i++){
        glRotatef(60*i,0,1,0);
        drawSqLine();
    }
    glPopMatrix();

}

void drawSquareArr(){
    glPushMatrix();
    glColor3f(1,1,1);
    
    // glRotatef(90,0,0,1);
    drawSQL();
    glPopMatrix();
}
void display(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear prev drawing
    glLoadIdentity();   //reset origin
    //draw
    
//eye or camera
    
    gluLookAt(camX,camY,camZ,camLX,camLY,camLZ,camAX,camAY,camAZ);
   
    
    axis();
    // board(5);
    // boundary();

    // drawSphere(1.5,16,20);
    drawCircle(10,40);  
    drawSquareArr(); 
    // glPopMatrix();
    glutSwapBuffers(); //double buffer mode
}

void reshape(int w, int h){
    glViewport(1,0,w,h);   //window te ja dekha jai
    glMatrixMode(GL_PROJECTION); //change to proj matrix
    glLoadIdentity();
    gluPerspective(60,1,1.0,200.0);
    glMatrixMode(GL_MODELVIEW);
}

void timer(int a){
    glutPostRedisplay();
    glutTimerFunc(1000/60,timer,0);
}


int main(int argc, char** argv){
    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);

    glutInitWindowPosition(500,100);
    glutInitWindowSize(500,500);

    glutCreateWindow("1905108");
    glutReshapeFunc(reshape);
    gluLookAt(camX,camY,camZ,camLX,camLY,camLZ,camAX,camAY,camAZ);
    
    
    glutDisplayFunc(display);
    glutTimerFunc(0,timer,0);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(cameraSpecialKey);

    init();

    glutMainLoop();
}