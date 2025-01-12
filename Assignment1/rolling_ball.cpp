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


double camX=20;
double camY=20;
double camZ=20;
double camLX=0;
double camLY=0;
double camLZ=0;
double camAX=0;
double camAY=1;
double camAZ=0;
//ball
double dx=0;
double dy=0;
double dz=0;
double fx=0;
double fy=0;
double fz=1;
double angle=0;
//boundary
int dirx=1;
int dirz=1;
double boundarylen=15;

const float moveSpeed = 0.8f;
float rotateSpeed = 0.5;

double len=0;
double arrowLen=5;
double rotAngle = 10;     
double arrowAngle = 0.0;    

double radius=1.5;
double fixedAngle = 25.0;

//simulation
bool simState=false;
double tim=2;


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

point forw(0,0,0);  //cam forward
point up(0,0,0);    //cam up
point rig(0,0,0);   //cam rig
point globalUp(camAX,camAY,camAZ);

point rp(0,0,0);
point fp(0,0,0);

point bf(0,0,1);  //ball forward
point bu(0,1,0);    //ball up
point br(-1,0,0);    //ball rig

point tmprp(0,0,0);
point tmpfp(0,0,0);

void init(){
    glClearColor(0,0,0.0,0);
    glEnable(GL_DEPTH_TEST);
}


void axis(){
    glPushMatrix();   
    glLineWidth(1.0); 
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
    glBegin(GL_QUADS);
    glVertex3f(0,0,0);
    glVertex3f(0,0,a);
    glVertex3f(a,0,a);
    glVertex3f(a,0,0);
    glEnd();
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
        glTranslatef(0,0,boundarylen);
        bound(boundarylen,3);
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

void drawCone(double radius,double height,int segments)
{
    int i;
    point points[100];
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw triangles using generated points
    for(i=0;i<segments;i++)
    {
        glBegin(GL_TRIANGLES);
        {
            glVertex3f(0,height,0);
			glVertex3f(points[i].x,0,points[i].y);
			glVertex3f(points[i+1].x,0,points[i+1].y);
        }
        glEnd();
    }
}

void drawCylinder(double radius,double height,int segments)
{
    int i;
    point points[100];
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi);
    }
    //draw triangles using generated points
    for(i=0;i<segments;i++)
    {
        glBegin(GL_QUADS);
        {
			glVertex3f(points[i].x,0,points[i].y);
			glVertex3f(points[i+1].x,0,points[i+1].y);
			glVertex3f(points[i+1].x,height,points[i+1].y);
			glVertex3f(points[i].x,height,points[i].y);
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
			points[i][j].y=h;
			points[i][j].z=r*sin(((double)j/(double)slices)*2*pi);
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
			glBegin(GL_QUADS);{
                if(j%2)
                    glColor3f(1,0,1);
                else
                    glColor3f(0,1,1);
			    //upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
                //lower hemisphere
                if(j%2)
                    glColor3f(0,1,1);
                else
                    glColor3f(1,0,1);
                glVertex3f(points[i][j].x,-points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,-points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,-points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,-points[i+1][j].y,points[i+1][j].z);
			}glEnd();
		}
	}
}


void arrow(double al=arrowLen){
    glPushMatrix();
    drawCylinder(0.07,al,24);
    glTranslatef(0,al,0);
    drawCone(0.23,0.35,24);
    glPopMatrix();
}

void drawSp(double radius){
    glPushMatrix();
    glTranslatef(0,radius,0);
    glColor3f(1,0.8,1);
    arrow(arrowLen);
    glRotatef(angle,br.x,br.y,br.z);
    drawSphere(radius,8,20);
    glPopMatrix();
}

void forwordArrow(double a, double b, double c, double a1, double b1, double c1){
    glPushMatrix();
    glTranslatef(0,radius,0);
    glRotatef(arrowAngle,0,1,0);
    glRotatef(90,1,0,0);
    arrow();
    glPopMatrix();
}

void drawArrow(double a, double b, double c, double a1, double b1, double c1){
    glPushMatrix();
    glTranslatef(0,radius,0);  
    glLineWidth(3.0);
    glBegin(GL_LINES);
    glVertex3f(a,b,c);
    glVertex3f(a1,b1,c1);
    glEnd();
    glPopMatrix();
}

//ball movement
void fixDir() {
    bf = tmpfp;
    br = tmprp;
}

void arrowAngleFix(){
    point ang(0,0,0);
    ang=bf.cross(tmpfp);
    
    // cout<<"ang:"<<acos(tmpfp.x*bf.x+tmpfp.z*bf.z)*180/M_PI<<" "<<ang.y<<endl;
    if(acos(tmpfp.x*bf.x+tmpfp.z*bf.z)*180/M_PI>0){
        if(ang.y>0)
            arrowAngle+=acos(tmpfp.x*bf.x+tmpfp.z*bf.z)*180/M_PI;
        else
            arrowAngle-=acos(tmpfp.x*bf.x+tmpfp.z*bf.z)*180/M_PI;
    }
}

void dir(unsigned char key) {
    if (key == 'i') {
        if ((radius + dx + moveSpeed * bf.x) >= boundarylen || (dx - radius + moveSpeed * bf.x) <= (-1) * boundarylen) {
            dirx = dirx * (-1);
            bf.x = bf.x * dirx;
            // tmpfp.x=bf.x;
            fx = dx + arrowLen/sqrt(2) * bf.x;
            br = bu.cross(bf);
            br.norm();
            arrowAngleFix();
        }
        if ((radius + dz + moveSpeed * bf.z) >= boundarylen || (dz - radius + moveSpeed * bf.z) <= (-1) * boundarylen) {
            dirz = dirz * (-1);
            bf.z = bf.z * dirz;
            // tmpfp.z=bf.z;
            fz = dz + arrowLen/sqrt(2) * bf.z;
            br = bu.cross(bf);
            br.norm();
            arrowAngleFix();
        }
    } else if (key == 'k') {
        if ((radius + dx - moveSpeed * bf.x) >= boundarylen || (dx - radius - moveSpeed * bf.x) <= (-1) * boundarylen) {
            dirx = dirx * (-1);
            bf.x = bf.x * dirx;
            // tmpfp.x=bf.x;
            fx = dx + arrowLen/sqrt(2) * bf.x;
            br = bu.cross(bf);
            br.norm();
            arrowAngleFix();
        }
        if ((radius + dz - moveSpeed * bf.z) >= boundarylen || (dz - radius - moveSpeed * bf.z) <= (-1) * boundarylen) {
            dirz = dirz * (-1);
            bf.z = bf.z * dirz;
            // tmpfp.z=bf.z;
            fz = dz + arrowLen/sqrt(2) * bf.z;
            br = bu.cross(bf);
            br.norm();
            arrowAngleFix();
        }
    }   
    tmpfp=bf;
    tmprp=br;
}

void rotate(unsigned char key) {//update in angle
    double angleChange = (key == 'j') ? rotAngle : -rotAngle;

    tmprp.x = tmprp.x * cos(angleChange*M_PI/180) + tmpfp.x * sin(angleChange*M_PI/180);
    tmprp.y = tmprp.y * cos(angleChange*M_PI/180) + tmpfp.y * sin(angleChange*M_PI/180);
    tmprp.z = tmprp.z * cos(angleChange*M_PI/180) + tmpfp.z * sin(angleChange*M_PI/180);

    tmprp.norm();
    tmpfp = tmprp.cross(bu);
    tmpfp.norm();



    fx = dx+arrowLen/sqrt(2)*tmpfp.x;
    fz = dz+arrowLen/sqrt(2)*tmpfp.z;

    arrowAngle = arrowAngle + angleChange;
    arrowAngle = fmod(arrowAngle, 360.0);
    
}

void moveForward(){
    // fixDir();   //order matters
    // dir('i');
    dx += moveSpeed * bf.x;
    dz += moveSpeed * bf.z;
    fx = dx+arrowLen/sqrt(2)*bf.x;
    fz = dz+arrowLen/sqrt(2)*bf.z;
    angle = angle - abs(fixedAngle);
    angle = fmod(angle, 360.0);
}

void moveBack(){
    fixDir();
    dir('k');
    dx -= moveSpeed * bf.x;
    dz -= moveSpeed * bf.z;
    fx = dx+arrowLen/sqrt(2)*bf.x;
    fz = dz+arrowLen/sqrt(2)*bf.z;
    angle = angle + abs(fixedAngle);
    angle = fmod(angle, 360.0);
}



//simulation
void autoMove(int t){
    if(simState)
        moveForward();
    if(simState)
        glutTimerFunc(tim,autoMove,0);
}

void collition(int t){
    double colTim=0;
    // colTim=50;
    double temp=0;
    if(simState){
        fixDir();
        dir('i');

        //finding time
        if (bf.x>0) {
            temp=boundarylen-dx;
            temp-=radius;
            temp=temp*tim/moveSpeed;
            temp=temp/abs(bf.x);
            // temp=abs(temp);
        }
        else{
            temp=boundarylen+dx;
            temp-=radius;
            temp=temp*tim/moveSpeed;
            temp=temp/abs(bf.x);
            // temp=abs(temp);
        }
        // if(temp<colTim)
            colTim=temp;

        if (bf.y>0) {
            temp=boundarylen-dy;
            temp-=radius;
            temp=temp*tim/moveSpeed;
            temp=temp/abs(bf.y);
            // temp=abs(temp);
        }
        else{
            temp=boundarylen+dy;
            temp-=radius;
            temp=temp*tim/moveSpeed;
            temp=temp/abs(bf.y);
            // temp=abs(temp);
        } 
        if(temp<colTim)
            colTim=temp; 

    }
    if(simState)
    glutTimerFunc(colTim,collition,0);
}

void updateCam(){
    forw.x=camLX-camX;
    forw.y=camLY-camY;
    forw.z=camLZ-camZ;
    len=forw.x*forw.x+forw.y*forw.y+forw.z*forw.z;
    len=sqrt(len);
    up=forw.cross(rig);
    rig=up.cross(forw);
    camAX=up.x;
    camAY=up.y;
    camAZ=up.z;
    forw.norm();
    rig.norm();
    up.norm();
}

void cameraSpecialKey(int key, int x, int y){
    switch(key){
        case GLUT_KEY_UP:   //forward            
            camX +=moveSpeed*forw.x; 
            camY +=moveSpeed*forw.y;
            camZ +=moveSpeed*forw.z; 
            camLX +=moveSpeed*forw.x; 
            camLY +=moveSpeed*forw.y;
            camLZ +=moveSpeed*forw.z; 
            break;
        case GLUT_KEY_DOWN: //backward
            camX -=moveSpeed*forw.x; 
            camY -=moveSpeed*forw.y;
            camZ -=moveSpeed*forw.z; 
            camLX -=moveSpeed*forw.x; 
            camLY -=moveSpeed*forw.y;
            camLZ -=moveSpeed*forw.z; 
            break;
        case GLUT_KEY_RIGHT:    //r
            camX +=moveSpeed*rig.x; 
            camY +=moveSpeed*rig.y;
            camZ +=moveSpeed*rig.z; 
            camLX +=moveSpeed*rig.x; 
            camLY +=moveSpeed*rig.y;
            camLZ +=moveSpeed*rig.z; 
            break;
        case GLUT_KEY_LEFT: //l
            camX -=moveSpeed*rig.x; 
            camY -=moveSpeed*rig.y;
            camZ -=moveSpeed*rig.z; 
            camLX -=moveSpeed*rig.x; 
            camLY -=moveSpeed*rig.y;
            camLZ -=moveSpeed*rig.z; 
            break;
        case GLUT_KEY_PAGE_UP:  //up
            camX +=moveSpeed*up.x; 
            camY +=moveSpeed*up.y;
            camZ +=moveSpeed*up.z; 
            camLX +=moveSpeed*up.x; 
            camLY +=moveSpeed*up.y;
            camLZ +=moveSpeed*up.z;  
            break;
        case GLUT_KEY_PAGE_DOWN:    //down
            camX -=moveSpeed*up.x; 
            camY -=moveSpeed*up.y;
            camZ -=moveSpeed*up.z; 
            camLX -=moveSpeed*up.x; 
            camLY -=moveSpeed*up.y;
            camLZ -=moveSpeed*up.z; 
            break;

        default:
            break;
    }    
}


void keyboard(unsigned char key, int x, int y){
    switch(key){
        case '1':   //rot l  
            rp.x=rig.x*cos(rotateSpeed * M_PI / 180.0f)+forw.x*sin(rotateSpeed * M_PI / 180.0f);
            rp.y=rig.y*cos(rotateSpeed * M_PI / 180.0f)+forw.y*sin(rotateSpeed * M_PI / 180.0f);
            rp.z=rig.z*cos(rotateSpeed * M_PI / 180.0f)+forw.z*sin(rotateSpeed * M_PI / 180.0f);

            fp = rp.cross(up);
            fp.norm();
            rp.norm();

            camLX = fp.x*len + camX;
            camLY = fp.y*len + camY;
            camLZ = fp.z*len + camZ;
            forw=fp;
            rig=rp;
            break;
        case '2':  //rot r
            rp.x=rig.x*cos(rotateSpeed * M_PI / 180.0f)-forw.x*sin(rotateSpeed * M_PI / 180.0f);
            rp.y=rig.y*cos(rotateSpeed * M_PI / 180.0f)-forw.y*sin(rotateSpeed * M_PI / 180.0f);
            rp.z=rig.z*cos(rotateSpeed * M_PI / 180.0f)-forw.z*sin(rotateSpeed * M_PI / 180.0f);

            fp = rp.cross(up);
            fp.norm();
            rp.norm();

            camLX = fp.x*len + camX;
            camLY = fp.y*len + camY;
            camLZ = fp.z*len + camZ;
            forw=fp;
            rig=rp;
            break;
        case '3':   //rot up
            rp.x=forw.x*cos(rotateSpeed * M_PI / 180.0f)+up.x*sin(rotateSpeed * M_PI / 180.0f);
            rp.y=forw.y*cos(rotateSpeed * M_PI / 180.0f)+up.y*sin(rotateSpeed * M_PI / 180.0f);
            rp.z=forw.z*cos(rotateSpeed * M_PI / 180.0f)+up.z*sin(rotateSpeed * M_PI / 180.0f);

            fp = rp.cross(rig);
            fp.norm();
            rp.norm();

            camLX = rp.x*len + camX;
            camLY = rp.y*len + camY;
            camLZ = rp.z*len + camZ;
            up=fp;
            forw=rp;
            camAX = up.x;
            camAY = up.y;
            camAZ = up.z;
            break;
        case '4':    //rot down
            rp.x=forw.x*cos(rotateSpeed * M_PI / 180.0f)-up.x*sin(rotateSpeed * M_PI / 180.0f);
            rp.y=forw.y*cos(rotateSpeed * M_PI / 180.0f)-up.y*sin(rotateSpeed * M_PI / 180.0f);
            rp.z=forw.z*cos(rotateSpeed * M_PI / 180.0f)-up.z*sin(rotateSpeed * M_PI / 180.0f);

            fp = rp.cross(rig);
            fp.norm();
            rp.norm();

            camLX = rp.x*len + camX;
            camLY = rp.y*len + camY;
            camLZ = rp.z*len + camZ;
            up=fp;
            forw=rp;
            camAX = up.x;
            camAY = up.y;
            camAZ = up.z;
            break;
        case '5':     //tilt anitclock
            rp.x=rig.x*cos(rotateSpeed * M_PI / 180.0f)+up.x*sin(rotateSpeed * M_PI / 180.0f);
            rp.y=rig.y*cos(rotateSpeed * M_PI / 180.0f)+up.y*sin(rotateSpeed * M_PI / 180.0f);
            rp.z=rig.z*cos(rotateSpeed * M_PI / 180.0f)+up.z*sin(rotateSpeed * M_PI / 180.0f);

            rp.norm();
            fp = forw.cross(rp);
            fp.norm();

            up=fp;
            rig=rp;
            camAX = up.x;
            camAY = up.y;
            camAZ = up.z;
            break;
        case '6':   //tilt clock
            rp.x=rig.x*cos(rotateSpeed * M_PI / 180.0f)-up.x*sin(rotateSpeed * M_PI / 180.0f);
            rp.y=rig.y*cos(rotateSpeed * M_PI / 180.0f)-up.y*sin(rotateSpeed * M_PI / 180.0f);
            rp.z=rig.z*cos(rotateSpeed * M_PI / 180.0f)-up.z*sin(rotateSpeed * M_PI / 180.0f);

            fp = forw.cross(rp);
            rp.norm();
            fp.norm();

            up=fp;
            rig=rp;
            camAX = up.x;
            camAY = up.y;
            camAZ = up.z;
            break;
        
        case 'w':
            camX +=moveSpeed*up.x; 
            camY +=moveSpeed*up.y;
            camZ +=moveSpeed*up.z;

            updateCam();
            break;
        case 's':
            camX -=moveSpeed*up.x; 
            camY -=moveSpeed*up.y;
            camZ -=moveSpeed*up.z;

            updateCam();
            break;

        //ball move
        case 'i': 
            if(!simState) {
                fixDir(); 
                dir('i');        
                moveForward();
            }
            break;
        case 'k':
            if(!simState) 
                moveBack();
            break;
        // arrow dir
        case 'j':
            rotate(key);
            if(simState)
                glutTimerFunc(0,collition,0);
            break;
        case 'l':            
            rotate(key);
             if(simState)
                glutTimerFunc(0,collition,0);
            // fixDir();
            break;
        
        //simylation: space doesn't work
        case ' ':
            simState=!simState;
            if(simState){
                cout<<"in sim";
                glutTimerFunc(0,autoMove,0);
                glutTimerFunc(0,collition,0);
            }
            break; 
        default:
            break;
    }
    
}


void display(){
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); //clear prev drawing
    glLoadIdentity();   //reset origin
    //draw
    
//eye or camera
    
    gluLookAt(camX,camY,camZ,camLX,camLY,camLZ,camAX,camAY,camAZ);
   
    
    axis();
    board(5);
    boundary();


    glPushMatrix();
    
    glColor3f(0,1,0);
    drawArrow(dx,dy,dz,fx,fy,fz);
    glTranslatef(dx,0,dz);
    glColor3f(0,1,0);
    // forwordArrow(dx,radius,dz,fx,radius,fz);
    drawSp(radius);
    glPopMatrix();
    
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

    forw.x=camLX-camX;
    forw.y=camLY-camY;
    forw.z=camLZ-camZ;
    len=forw.x*forw.x+forw.y*forw.y+forw.z*forw.z;
    len=sqrt(len);
    rig=globalUp.cross(forw);
    up=forw.cross(rig);
    camAX=up.x;
    camAY=up.y;
    camAZ=up.z;
    forw.norm();
    rig.norm();
    up.norm();

    //ball
    // bf.x=fx-dx;
    // bf.z=fz-dz;
    // arrowLen=bf.x*bf.x+bf.z*bf.z;
    // arrowLen=sqrt(arrowLen);
    // bu.y=radius*arrowLen;

    fx = dx+arrowLen/sqrt(2)*bf.x;
    fz = dz+arrowLen/sqrt(2)*bf.z;
    bf.norm();
    br=bu.cross(bf);
    br.norm();
    tmpfp=bf;
    tmprp=br;
    cout<<acos(0.5)*180/M_PI<<endl;
    // fixedAngle=360*dx/(2*radius*pi);


    glutInitWindowPosition(500,100);
    glutInitWindowSize(500,500);

    glutCreateWindow("1905108_RollingBall");
    glutReshapeFunc(reshape);
    gluLookAt(camX,camY,camZ,camLX,camLY,camLZ,camAX,camAY,camAZ);
    
    
    glutDisplayFunc(display);
    glutTimerFunc(0,timer,0);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(cameraSpecialKey);

    init();

    glutMainLoop();
}