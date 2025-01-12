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


double camX=3;
double camY=3;
double camZ=3;
double camLX=0;
double camLY=0;
double camLZ=0;
double camAX=0;
double camAY=1;
double camAZ=0;

float pyFactor = 1;
const float pycng=0.05f;
const float moveSpeed = 0.05f;
float rotateSpeed = 0.5;
double len=0;
double radius=1.5;
double spRadius=0;
int subDiv=4;
int spAngle=0;
int spAngleRate=1;
double cyLen=0;
double cyAngleMax=70.5287794;
double cyAngle=70.5287794;



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

point p1(1,0,0);
point p2(0,1,0);
point p3(0,0,1);
point centriod(0,0,0);
std::vector<std::vector<point>> spoint;
int row=0;
int col=0;


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

void drawTriangle(){
    glPushMatrix();
    glBegin(GL_TRIANGLES);
    glVertex3f(p1.x,p1.y,p1.z);
    glVertex3f(p2.x,p2.y,p2.z);
    glVertex3f(p3.x,p3.y,p3.z);
    glEnd();
    glPopMatrix();
}

void pyramid(){
    for(int i=0;i<4;i++){
        if(i%2==0)
            glColor3f(0,1,1);
        else
            glColor3f(1,0,1);
        glPushMatrix();
        glRotatef(90*i,0,1,0);
        drawTriangle();
        glPopMatrix();
    }
}

void octa(){
    glPushMatrix();
    glRotatef(180,1,0,0);
    pyramid();
    glPopMatrix();
    pyramid();
}

void reducePyramid(){
    double invFac=1-pyFactor;
    p1.x=pyFactor+invFac/3.0;
    p1.y=invFac/3.0;
    p1.z=invFac/3.0;

    p2.x=invFac/3.0;
    p2.y=pyFactor+invFac/3.0;
    p2.z=invFac/3.0;

    p3.x=invFac/3.0;
    p3.y=invFac/3.0;
    p3.z=pyFactor+invFac/3.0;
}

//from https://www.songho.ca/opengl/gl_sphere.html
// generate vertices for +X face only by intersecting 2 circular planes
// (longitudinal and latitudinal) at the given longitude/latitude angles
std::vector<float> buildUnitPositiveX(int subdivision)
{
    const float DEG2RAD = acos(-1) / 180.0f;

    std::vector<float> vertices;
    float n1[3];        // normal of longitudinal plane rotating along Y-axis
    float n2[3];        // normal of latitudinal plane rotating along Z-axis
    float v[3];         // direction vector intersecting 2 planes, n1 x n2
    float a1;           // longitudinal angle along Y-axis
    float a2;           // latitudinal angle along Z-axis

    // compute the number of vertices per row, 2^n + 1
    int pointsPerRow = (int)pow(2, subdivision) + 1;

    // rotate latitudinal plane from 45 to -45 degrees along Z-axis (top-to-bottom)
    for(unsigned int i = 0; i < pointsPerRow; ++i)
    {
        // normal for latitudinal plane
        // if latitude angle is 0, then normal vector of latitude plane is n2=(0,1,0)
        // therefore, it is rotating (0,1,0) vector by latitude angle a2
        a2 = DEG2RAD * (45.0f - 90.0f * i / (pointsPerRow - 1));
        n2[0] = -sin(a2);
        n2[1] = cos(a2);
        n2[2] = 0;

        // rotate longitudinal plane from -45 to 45 along Y-axis (left-to-right)
        for(unsigned int j = 0; j < pointsPerRow; ++j)
        {
            // normal for longitudinal plane
            // if longitude angle is 0, then normal vector of longitude is n1=(0,0,-1)
            // therefore, it is rotating (0,0,-1) vector by longitude angle a1
            a1 = DEG2RAD * (-45.0f + 90.0f * j / (pointsPerRow - 1));
            n1[0] = -sin(a1);
            n1[1] = 0;
            n1[2] = -cos(a1);

            // find direction vector of intersected line, n1 x n2
            v[0] = n1[1] * n2[2] - n1[2] * n2[1];
            v[1] = n1[2] * n2[0] - n1[0] * n2[2];
            v[2] = n1[0] * n2[1] - n1[1] * n2[0];

            // normalize direction vector
            float scale = 1 / sqrt(v[0]*v[0] + v[1]*v[1] + v[2]*v[2]);
            v[0] *= scale*spRadius;
            v[1] *= scale*spRadius;
            v[2] *= scale*spRadius;

            // add a vertex into array
            vertices.push_back(v[0]);
            vertices.push_back(v[1]);
            vertices.push_back(v[2]);
        }
    }

    return vertices;
}

void genSpoint() {
    std::vector<float> plist = buildUnitPositiveX(subDiv);
    col = pow(2, subDiv);
    row = plist.size() / (3 * col);

    spoint.resize(row, std::vector<point>(col + 1, point(0.0f, 0.0f, 0.0f)));

    for (int i = 0, k = 0; i < row; ++i) {
        for (int j = 0; j <= col; ++j, k += 3) {
            spoint[i][j].x = plist[k];
            spoint[i][j].y = plist[k + 1];
            spoint[i][j].z = plist[k + 2];
        }
    }
}

void reduceSphere(){
    double invFac = 1 - pyFactor;
    double pos =  pyFactor;
    glTranslatef(pos, 0, 0);
    glScalef(invFac,invFac,invFac);
}

void drawSpPlane() {
    glPushMatrix();
    for (int i = 0; i < row - 2; ++i) {
        for (int j = 0; j < col; ++j) {
            glBegin(GL_QUADS);
            glVertex3f(spoint[i][j].x, spoint[i][j].y, spoint[i][j].z);
            glVertex3f(spoint[i + 1][j].x, spoint[i + 1][j].y, spoint[i + 1][j].z);
            glVertex3f(spoint[i + 1][j + 1].x, spoint[i + 1][j + 1].y, spoint[i + 1][j + 1].z);
            glVertex3f(spoint[i][j + 1].x, spoint[i][j + 1].y, spoint[i][j + 1].z);
            glEnd();
        }
    }

    glPopMatrix();
}



void drawSphere()
{
    glPushMatrix();
    for(int i=0;i<4;i++){
        if(i%2==0)
            glColor3f(0,0,1);
        else
            glColor3f(0,1,0);
        glPushMatrix();
        glRotatef(90*i,0,1,0);
        reduceSphere();
        drawSpPlane();
        glPopMatrix();
    }
    int a=-1;
    glColor3f(1,0,0);
    for(int i=0; i<2; i++){
        glPushMatrix();
        glRotatef(90*a,0,0,1);
        reduceSphere();
        drawSpPlane();
        glPopMatrix();
        a=a*a;
    }
    glPopMatrix();
}

void drawCylinderPart(double radius,double height,int segments)
{
    int i;
    point points[100];
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*pi*cyAngle/360);
        points[i].y=radius*sin(((double)i/(double)segments)*2*pi*cyAngle/360);
    }
   
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

void reduceCylinder(){
    double invFac=1-pyFactor;
    double pos=(1/sqrt(2)-spRadius);
    double fix=spRadius-spRadius*cos(cyLen/2);
    pos=pos*pyFactor+spRadius;
    // cyAngle=cyAngleMax*invFac;
    // glTranslatef(pos,0,0);
    // glScalef(1,pyFactor,1);
    glTranslatef(pos,0,0);
    glScalef(invFac,pyFactor,invFac);
}


void cylinderPartFix(){
    glPushMatrix();
    // glTranslatef(spRadius/sqrt(2),spRadius/sqrt(2),0);
    glRotatef(45,0,0,1);
    reduceCylinder();
    glTranslatef(0,-cyLen/2,0);
    glTranslatef(-spRadius,0,0);
    glRotatef(cyAngle/2,0,1,0);
    drawCylinderPart(spRadius,cyLen,24);
    glPopMatrix();
}

void cylinder(){
    // top 4 cy
    glColor3f(1,1,0.7);
    for(int i=0; i<4; i++){
        glPushMatrix();
        glRotatef(90*i,0,1,0);
        cylinderPartFix();
        glPopMatrix();        
    }
    //mid 4 cy
    for(int i=0; i<4; i++){
        glPushMatrix();
        glRotatef(90*i,0,1,0);
        glRotatef(90,1,0,0);
        cylinderPartFix();
        glPopMatrix();        
    }
    //lower 4
    for(int i=0; i<4; i++){
        glPushMatrix();
        glRotatef(90*i,0,1,0);
        glRotatef(180,1,0,0);
        cylinderPartFix();
        glPopMatrix();        
    }
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

        case 'a':
            spAngle=(spAngle-spAngleRate)%360;
            break;
        case 'd':
            spAngle=(spAngle+spAngleRate)%360;
            break;
        //py
        case ',':
            if(pyFactor>0){
                pyFactor-=pycng;
                reducePyramid();

                // centriod.x=(p1.x+p2.x+p3.x)/3.0;
                // centriod.y=(p1.y+p2.y+p3.y)/3.0;
                // centriod.z=(p1.z+p2.z+p3.z)/3.0;
                // spRadius=centriod.x*centriod.x+centriod.y*centriod.y+centriod.z*centriod.z;
                // spRadius=sqrt(spRadius);

                // cyLen=p1.x*p1.x+p2.y*p2.y;
                // cyLen=sqrt(cyLen);
            }
            break;
        case '.':
            if(pyFactor<1){
                pyFactor+=pycng;
                reducePyramid();

                // centriod.x=(p1.x+p2.x+p3.x)/3.0;
                // centriod.y=(p1.y+p2.y+p3.y)/3.0;
                // centriod.z=(p1.z+p2.z+p3.z)/3.0;
                // spRadius=centriod.x*centriod.x+centriod.y*centriod.y+centriod.z*centriod.z;
                // spRadius=sqrt(spRadius);
                
                // cyLen=p1.x*p1.x+p2.y*p2.y;
                // cyLen=sqrt(cyLen);
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

    glPushMatrix();
    glRotatef(spAngle,0,1,0);
    octa();
    drawSphere();
    cylinder();
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

    //sp
    centriod.x=(p1.x+p2.x+p3.x)/3.0;
    centriod.y=(p1.y+p2.y+p3.y)/3.0;
    centriod.z=(p1.z+p2.z+p3.z)/3.0;
    spRadius=centriod.x*centriod.x+centriod.y*centriod.y+centriod.z*centriod.z;
    spRadius=sqrt(spRadius);
    genSpoint();

    cyLen=p1.x*p1.x+p2.y*p2.y;
    cyLen=sqrt(cyLen);

    glutInitWindowPosition(500,100);
    glutInitWindowSize(500,500);

    glutCreateWindow("1905108_MagicCube");
    glutReshapeFunc(reshape);
    gluLookAt(camX,camY,camZ,camLX,camLY,camLZ,camAX,camAY,camAZ);
    
    
    glutDisplayFunc(display);
    glutTimerFunc(0,timer,0);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(cameraSpecialKey);

    init();

    glutMainLoop();
}