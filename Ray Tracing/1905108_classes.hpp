#include "bitmap_image.hpp"
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

#define pi (2*acos(0.0))
#define epsilon 0.00001

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

    void set(double a, double b, double c){
        x=a;
        y=b;
        z=c;
    }

    point add(point p){
        point pp;
        pp.x=x+p.x;
        pp.y=y+p.y;
        pp.z=z+p.z;
        return pp;
    }

    point sub(point p){
        point pp;
        pp.x=x-p.x;
        pp.y=y-p.y;
        pp.z=z-p.z;
        return pp;
    }

    point cross(point p){ //p x point
        point pp;
        pp.x=z*p.y-y*p.z;
        pp.y=x*p.z-z*p.x;
        pp.z=y*p.x-x*p.y;
        return pp;
    }

    double dot(point p){
        double pp;
        pp=x*p.x+y*p.y+z*p.z;
        return pp;
    }

    void norm(){
        double sq=sqrt(x*x+y*y+z*z);
        x=x*1.0/sq;
        y=y*1.0/sq;
        z=z*1.0/sq;
    }

    point square(){
        point p;
        p.x=x*x;
        p.y=y*y;
        p.z=z*z;
        return p;
    }

    point mul(double a, double b, double c){
        point p;
        p.x=a*x;
        p.y=b*y;
        p.z=c*z;
        return p;
    }

    void fix(){ //for color
        if(x<0)x=0;
        if(y<0)y=0;
        if(z<0)z=0;
        if(x>1)x=1;
        if(y>1)y=1;
        if(z>1)z=1;
    }

    void print(){
        cout<<"p: "<<x<<" "<<y<<" "<<z<<endl;
    }

    ~point(){

    }

};


class Camera{

public:

double camX;
double camY;
double camZ;
double camLX;
double camLY;
double camLZ;
double camAX;
double camAY;
double camAZ;
float moveSpeed ;
float rotateSpeed;
double len;


point forw;  //cam forward
point up;    //cam up
point rig;   //cam rig
point globalUp;

point rp;
point fp;

Camera(double ex=20,double ey=20, double ez=20){
    camX=ex;
    camY=ey;
    camZ=ez;
    camLX=0;
    camLY=0;
    camLZ=0;
    camAX=0;
    camAY=0;
    camAZ=1;
    moveSpeed = 0.5f;
    rotateSpeed = 0.5;
    len=0;

    globalUp.set(camAX,camAY,camAZ);    

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

//movement

void forward(){            
    camX +=moveSpeed*forw.x; 
    camY +=moveSpeed*forw.y;
    camZ +=moveSpeed*forw.z; 
    camLX +=moveSpeed*forw.x; 
    camLY +=moveSpeed*forw.y;
    camLZ +=moveSpeed*forw.z; 
}
void backward(){
    camX -=moveSpeed*forw.x; 
    camY -=moveSpeed*forw.y;
    camZ -=moveSpeed*forw.z; 
    camLX -=moveSpeed*forw.x; 
    camLY -=moveSpeed*forw.y;
    camLZ -=moveSpeed*forw.z;
}
void right(){
    camX +=moveSpeed*rig.x; 
    camY +=moveSpeed*rig.y;
    camZ +=moveSpeed*rig.z; 
    camLX +=moveSpeed*rig.x; 
    camLY +=moveSpeed*rig.y;
    camLZ +=moveSpeed*rig.z; 
}
void left(){
    camX -=moveSpeed*rig.x; 
    camY -=moveSpeed*rig.y;
    camZ -=moveSpeed*rig.z; 
    camLX -=moveSpeed*rig.x; 
    camLY -=moveSpeed*rig.y;
    camLZ -=moveSpeed*rig.z; 
}
void moveUp(){
    camX +=moveSpeed*up.x; 
    camY +=moveSpeed*up.y;
    camZ +=moveSpeed*up.z; 
    camLX +=moveSpeed*up.x; 
    camLY +=moveSpeed*up.y;
    camLZ +=moveSpeed*up.z;  
}
void down(){
    camX -=moveSpeed*up.x; 
    camY -=moveSpeed*up.y;
    camZ -=moveSpeed*up.z; 
    camLX -=moveSpeed*up.x; 
    camLY -=moveSpeed*up.y;
    camLZ -=moveSpeed*up.z; 
}


void rotateLeft(){  
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
}
 
void rotateRight(){
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
    }
    

void lookUp(){
    rp.x=forw.x*cos(rotateSpeed * M_PI / 180.0f)+up.x*sin(rotateSpeed * M_PI / 180.0f);
    rp.y=forw.y*cos(rotateSpeed * M_PI / 180.0f)+up.y*sin(rotateSpeed * M_PI / 180.0f);
    rp.z=forw.z*cos(rotateSpeed * M_PI / 180.0f)+up.z*sin(rotateSpeed * M_PI / 180.0f);
//rp is forward
    fp = rp.cross(rig);//up
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
}
void lookDown(){
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
}

void tiltAntiClockwise(){
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
}

void tiltClockwise(){
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
}    


void goUp(){
    camX +=moveSpeed*up.x; 
    camY +=moveSpeed*up.y;
    camZ +=moveSpeed*up.z;

    updateCam();
}

void goDown(){
    camX -=moveSpeed*up.x; 
    camY -=moveSpeed*up.y;
    camZ -=moveSpeed*up.z;

    updateCam();
}

void init(){
    //in main
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

    //gluLookAt(camX,camY,camZ,camLX,camLY,camLZ,camAX,camAY,camAZ);
    //glutKeyboardFunc(keyboard);
    //glutSpecialFunc(cameraSpecialKey);

}

point eye(){
    point p(camX,camY,camZ);
    return p;
}

point l(){ //forward vector
    forw.norm();
    return forw;
}

point r(){
    rig.norm();
    return rig;
}

point u(){
    up.norm();
    return up;
}

    ~Camera(){

    }
};







class Object;
class PointLight;
class SpotLight;
extern vector<unique_ptr<Object>> objects;
extern vector<unique_ptr<PointLight>> pointlights;
extern vector<unique_ptr<SpotLight>> spotlights;
extern int recursionLevel;


class Ray{
    public:
    point start;
    point dir;
    Ray(){ }
    Ray(point s, point d){
        start=s;
        dir=d;
        dir.norm();
    }

    ~Ray(){

    }
};

class PointLight{
public:
    point light_pos;
    double color[3];

    PointLight(){
        color[0]=0;
        color[1]=0;
        color[2]=0;
    }


    PointLight(point p, double r, double g, double b){
        light_pos=p;
        color[0]=r;
        color[1]=g;
        color[2]=b;
    }

    void setPos(point p){
        light_pos=p;
    }

    void setColor(double r, double g, double b){
        color[0]=r;
        color[1]=g;
        color[2]=b;
    }

    point getColor(){
        point p;
        p.x=color[0];
        p.y=color[1];
        p.z=color[2];
        return p;
    }

    void print(){
        cout<<"Point light\n";
        cout<<"X Y Z: "<<light_pos.x<<" "<<light_pos.y<<" "<<light_pos.z<<endl;
        cout<<"R G B: "<<color[0]<<" "<<color[1]<<" "<<color[2]<<endl;        
    }

    void draw(){
        glPushMatrix();
        glPointSize(5);
        glColor3f(color[0],color[1],color[2]);
        glBegin(GL_POINTS);
        glVertex3f(light_pos.x,light_pos.y,light_pos.z);
        glPopMatrix();
    }
    
    ~PointLight(){

    }
};


class SpotLight{
public:
    PointLight point_light;
    point light_dir;
    double cutoff_angle;
    
    SpotLight(){
        cutoff_angle=0;
    }


    SpotLight(point p, double r, double g, double b, point dir, double angle){
        point_light.setPos(p);
        point_light.setColor(r,g,b);
        light_dir=dir;
        cutoff_angle=angle;
    }

    void print(){
        cout<<"Spot light\n";
        point_light.print();
        cout<<"Dir X Y Z: "<<light_dir.x<<" "<<light_dir.y<<" "<<light_dir.z<<endl;
        cout<<"Cutoff: "<<cutoff_angle<<endl;        
    }


    void draw(){
        glPushMatrix();
        glPointSize(5);
        glColor3f(point_light.color[0],point_light.color[1],point_light.color[2]);
        glBegin(GL_POINTS);
        glVertex3f(point_light.light_pos.x,point_light.light_pos.y,point_light.light_pos.z);
        glPopMatrix();
    }

    ~SpotLight(){

    }
};


class Object{
    public:
    string name;
    point reference_point;
    double height, width, length;
    double color[3];
    double coEfficients[4];
    int shine;

    Object(){
        name="";
        reference_point.set(0,0,0);
        height=0;
        width=0;
        length=0;
        shine=1;

        color[0]=0;
        color[1]=0;
        color[2]=0;

        coEfficients[0]=0;
        coEfficients[1]=0;
        coEfficients[2]=0;
        coEfficients[3]=0;
    }

    virtual void draw(){}

    
    void setColor(double r, double g, double b){
        color[0]=r;
        color[1]=g;
        color[2]=b;
    }

    void setShine(int s){
        shine=s;
    }

    void setCoEfficients(double amb, double diff, double spec, double ref){
        coEfficients[0]=amb;
        coEfficients[1]=diff;
        coEfficients[2]=spec;
        coEfficients[3]=ref;
    }

    virtual void print(){
        cout<<name<<endl;
        cout<<"Ref point "<<reference_point.x<<" "<<reference_point.y<<" "<<reference_point.z<<endl;
        cout<<"H: "<<height<<endl;
        cout<<"W: "<<width<<endl;
        cout<<"L: "<<length<<endl;
        cout<<"SHINE: "<<shine<<endl;

        cout<<"R G B: "<<color[0]<<" "<<color[1]<<" "<<color[2]<<endl;

        cout<<"coeff: "<<coEfficients[0]<<" "
        <<coEfficients[1]<<" "
        <<coEfficients[2]<<" "
        <<coEfficients[3]<<endl<<endl;
    }
    

    virtual bool intersect(Ray ray, point &colour, int level, point& intersection, double &t){
        t=-1.0;
        return false;
    }

    void Coloring(Ray ray, point &colour, point intersection, point normal, int level){
        //amb
        colour=colour.mul(coEfficients[0],coEfficients[0],coEfficients[0]);

        //lighting
        Ray newray;
        point newcolor,newintersect;
        double newt=0,newtmin=0;
        int nearest=-1; 
        double lam=0;
        double phong=0;

        //point lightColoring
        for(int i=0; i<pointlights.size(); i++){
            newray.start=pointlights[i]->light_pos;
            newray.dir=intersection.sub(newray.start);
            newray.dir.norm();
            newtmin=numeric_limits<double>::max();
            nearest=-1;

            for(int k=0; k<objects.size(); k++){    //can be optimized
                if(objects[k]->intersect(newray,newcolor,0,newintersect,newt)){
                    if(newt<newtmin){
                        newtmin=newt;
                        nearest=k;
                    }
                }                
            }
            if(nearest!=-1){
                if(objects[nearest]->intersect(newray,newcolor,0,newintersect,newt)){
                    if((newintersect.x<=intersection.x+epsilon || newintersect.x>=intersection.x-epsilon)&&
                    (newintersect.y<=intersection.y+epsilon || newintersect.y>=intersection.y-epsilon)&&
                    (newintersect.z<=intersection.z+epsilon || newintersect.z>=intersection.z-epsilon)){
                        //no shadow
                        //cal diff and spec
                        //diff                        
                        lam=normal.dot(newray.dir.mul(-1,-1,-1));
                        // cout<<"lambart "<<lam<<endl;
                        lam=max(lam,0.0);

                        newcolor=pointlights[i]->getColor();
                        newcolor=newcolor.mul(color[0],color[1],color[2]);
                        newcolor=newcolor.mul(lam,lam,lam);
                        newcolor=newcolor.mul(coEfficients[1],coEfficients[1],coEfficients[1]);
                        colour=colour.add(newcolor);
                        // cout<<"color";Coloring
                        // colour.print();

                        //spec-original eq e err
                        // phong=normal.dot(newray.dir);
                        // phong=phong*2;
                        // newcolor=normal.mul(phong,phong,phong);
                        // newcolor=newcolor.sub(newray.dir);
                        // newcolor.norm();
                        // phong=newcolor.dot(ray.dir); //(2(NL)N-L)V
                        // phong=pow(phong,shine);
                        // cout<<"phong "<<phong<<endl;
                        // phong=max(phong,0.0);

                        //blinn torrence
                        newray.dir.norm();
                        ray.dir.norm();
                        newcolor=ray.dir.add(newray.dir);
                        newcolor.norm();
                        normal.norm();
                        phong=normal.dot(newcolor.mul(-1,-1,-1));
                        phong=pow(phong,shine);
                        // cout<<"phong "<<phong<<endl;
                        phong=max(phong,0.0);

                        newcolor=pointlights[i]->getColor();
                        newcolor=newcolor.mul(color[0],color[1],color[2]);
                        newcolor=newcolor.mul(phong,phong,phong);
                        newcolor=newcolor.mul(coEfficients[2],coEfficients[2],coEfficients[2]);
                        colour=colour.add(newcolor);

                        // cout<<"color";
                        // colour.print();
                    }
                }
            }

        }

        newt=0,newtmin=0;
        nearest=-1; 
        lam=0;
        phong=0;
        double cut=0;
        //spot light
        for(int i=0; i<spotlights.size(); i++){
            newray.start=spotlights[i]->point_light.light_pos;
            newray.dir=intersection.sub(newray.start);
            newray.dir.norm();
            newtmin=numeric_limits<double>::max();
            nearest=-1;

            for(int k=0; k<objects.size(); k++){    //can be optimized
                if(objects[k]->intersect(newray,newcolor,0,newintersect,newt)){
                    if(newt<newtmin){
                        newtmin=newt;
                        nearest=k;
                    }
                }                
            }
            if(nearest!=-1){
                if(objects[nearest]->intersect(newray,newcolor,0,newintersect,newt)){
                    if((newintersect.x<=intersection.x+epsilon || newintersect.x>=intersection.x-epsilon)&&
                    (newintersect.y<=intersection.y+epsilon || newintersect.y>=intersection.y-epsilon)&&
                    (newintersect.z<=intersection.z+epsilon || newintersect.z>=intersection.z-epsilon)){
                        //no shadow
                        //cal diff and spec
                        cut=newray.dir.dot(spotlights[i]->light_dir);
                        if(cut>=cos(spotlights[i]->cutoff_angle*M_PI/180.0)){
                            //if inside cutoff
                            //diff     
                            cut=pow(cut,1.5);
                            lam=normal.dot(newray.dir.mul(-1,-1,-1));
                            // cout<<"lambart "<<lam<<endl;
                            lam=max(lam,0.0);

                            newcolor=spotlights[i]->point_light.getColor();
                            newcolor=newcolor.mul(color[0],color[1],color[2]);                            
                            newcolor=newcolor.mul(cut,cut,cut);
                            newcolor=newcolor.mul(lam,lam,lam);
                            newcolor=newcolor.mul(coEfficients[1],coEfficients[1],coEfficients[1]);
                            colour=colour.add(newcolor);
                            // cout<<"color";
                            // colour.print();

                            //spec-original eq e err
                            // phong=normal.dot(newray.dir);
                            // phong=phong*2;
                            // newcolor=normal.mul(phong,phong,phong);
                            // newcolor=newcolor.sub(newray.dir);
                            // newcolor.norm();
                            // phong=newcolor.dot(ray.dir); //(2(NL)N-L)V
                            // phong=pow(phong,shine);
                            // cout<<"phong "<<phong<<endl;
                            // phong=max(phong,0.0);

                            //blinn torrence
                            newray.dir.norm();
                            ray.dir.norm();
                            newcolor=ray.dir.add(newray.dir);
                            newcolor.norm();
                            normal.norm();
                            phong=normal.dot(newcolor.mul(-1,-1,-1));
                            phong=pow(phong,shine);
                            // cout<<"phong "<<phong<<endl;
                            phong=max(phong,0.0);

                            newcolor=spotlights[i]->point_light.getColor();
                            newcolor=newcolor.mul(color[0],color[1],color[2]);
                            newcolor=newcolor.mul(cut,cut,cut);
                            newcolor=newcolor.mul(phong,phong,phong);
                            newcolor=newcolor.mul(coEfficients[2],coEfficients[2],coEfficients[2]);
                            colour=colour.add(newcolor);

                            // cout<<"color";
                            // colour.print();
                        }                        
                    }
                }
            }

        }


        if(level>=recursionLevel)
            return;

        //recursion
        //ref ray
        double va=0;
        point rray;
        point dray;
        va=normal.dot(ray.dir);
        va=va*2;
        rray=normal.mul(va,va,va);
        rray=ray.dir.sub(rray);
        rray.norm();
        dray=intersection.mul(rray.x*0.02,rray.y*0.02,rray.z*0.02);
        dray=dray.add(intersection);
        Ray reflectRay(dray,rray);
        // dray = intersection.sub(normal.mul(0.001,0.001,0.001)); 
        // dray = dray.add(rray.mul(0.002,0.002,0.002)); 
        // Ray reflectRay(dray, rray);

        point reflectColor,reflectInter;
        double reflectT=0, minrefT=numeric_limits<double>::max();
        int refNearest=-1;
        for(int k=0; k<objects.size(); k++){
            if(objects[k]->intersect(reflectRay,reflectColor,0,reflectInter,reflectT)){
                if(reflectT<minrefT){
                    minrefT=reflectT;
                    refNearest=k;
                }
            }                
        }
        if(refNearest!=-1){
            if(objects[refNearest]->intersect(reflectRay,reflectColor,level+1,reflectInter,reflectT)){
                reflectColor.fix();                
                // cout<<"ref color "<<reflectColor.x<<" "<<reflectColor.y<<" "<<reflectColor.z<<endl;
                reflectColor=reflectColor.mul(coEfficients[3],coEfficients[3],coEfficients[3]);
                colour=colour.add(reflectColor);
            }
        }
    }

    virtual ~Object(){
        color[0]=0;
        color[1]=0;
        color[2]=0;

        coEfficients[0]=0;
        coEfficients[1]=0;
        coEfficients[2]=0;
        coEfficients[3]=0;
    }
};


class Sphere:public Object{
    public:
    Sphere(point center, double radius){
        name="sphere";
        reference_point=center;
        length=radius;
    }

    void draw(){
        glPushMatrix();
        glTranslatef(reference_point.x,reference_point.y,reference_point.z);
        glColor3f(color[0],color[1],color[2]);
        glutSolidSphere(length, 48, 48);
        glPopMatrix();
    }

    bool intersect(Ray ray, point &colour, int level, point& intersection, double &t){
        point ro = ray.start.sub(reference_point); //translate ray start
        double a = ray.dir.x * ray.dir.x + ray.dir.y * ray.dir.y + ray.dir.z * ray.dir.z;
        double b = 2.0 * (ro.x * ray.dir.x + ro.y * ray.dir.y + ro.z * ray.dir.z);
        double c = ro.x * ro.x + ro.y * ro.y + ro.z * ro.z - length * length;
        double d = b * b - 4 * a * c;
        t=-1.0;
        bool in=false;
       
        if (d < 0) {
            // No intersection
            return false;
        }            

        // check ray start
        if(c<0){ //ray start is inside
            t = (-b + sqrt(d)) / (2.0 * a);
            in=true;
        }
        else{ //outside //have to be chacked for Ro.Ro==r2, degeneracies
            t = (-b - sqrt(d)) / (2.0 * a);
        }
              
        if (t <= 0) { //intersection point is behind, if err look here, t==0?
            return false;
        }
        
        point demo;
        demo.x=ray.dir.x * t;
        demo.y=ray.dir.y * t;
        demo.z=ray.dir.z * t;
        intersection = ray.start.add(demo);
        if(level==0)
            return true;

        //level=1 color
        colour.x=color[0];
        colour.y=color[1];
        colour.z=color[2];

        point normal;
        normal=intersection.sub(reference_point);
        if(in){
            normal=normal.mul(-1,-1,-1);
        }
        normal.norm();
        // --------------

        Coloring(ray, colour,intersection,normal,level);

        
        return true;
    }

};


class Triangle:public Object{
    point p1;
    point p2;
    point p3;
    public:
    Triangle(point pp1, point pp2, point pp3){
        name="triangle";
        p1=pp1;
        p2=pp2;
        p3=pp3;
    }

    void draw(){
        glPushMatrix();
        glColor3f(color[0],color[1],color[2]);
        glBegin(GL_TRIANGLES);
        glVertex3f(p1.x,p1.y,p1.z);
        glVertex3f(p2.x,p2.y,p2.z);
        glVertex3f(p3.x,p3.y,p3.z);
        glEnd();
        glPopMatrix();
    }

    void print(){
        cout<<name<<endl;
        cout<<"point"<<endl;
        cout<<"p1 "<<p1.x<<" "<<p1.y<<" "<<p1.z<<endl;
        cout<<"p2 "<<p2.x<<" "<<p2.y<<" "<<p2.z<<endl;
        cout<<"p3 "<<p3.x<<" "<<p3.y<<" "<<p3.z<<endl;
        cout<<"Ref point "<<reference_point.x<<" "<<reference_point.y<<" "<<reference_point.z<<endl;
        cout<<"H: "<<height<<endl;
        cout<<"W: "<<width<<endl;
        cout<<"L: "<<length<<endl;
        cout<<"SHINE: "<<shine<<endl;

        cout<<"R G B: "<<color[0]<<" "<<color[1]<<" "<<color[2]<<endl;

        cout<<"coeff: "<<coEfficients[0]<<" "
        <<coEfficients[1]<<" "
        <<coEfficients[2]<<" "
        <<coEfficients[3]<<endl<<endl;
    }

    // determinent
    double det(double a, double b, double c, double d) {
        return a * d - b * c;
    }

    // Cramer's rule
    bool cramer(vector<vector<double>> A,  vector<double> B, double &t) {
        // det of A
        double detA = det(A[0][0], A[0][1], A[1][0], A[1][1]) * A[2][2]
                    - det(A[0][0], A[0][2], A[1][0], A[1][2]) * A[2][1]
                    + det(A[0][1], A[0][2], A[1][1], A[1][2]) * A[2][0];

        // no unique solution
        if (detA == 0) {
            t=-1;
            return false;
        }

        // solutions
        double detB=0;

        detB = det(B[0], A[0][1], B[1], A[1][1]) * A[2][2]
                    - det(B[0], A[0][2], B[1], A[1][2]) * A[2][1]
                    + det(A[0][1], A[0][2], A[1][1], A[1][2]) * B[2];

        double beta=detB/detA;  //beta

        detB = det(A[0][0], B[0], A[1][0], B[1]) * A[2][2]
            - det(A[0][0], A[0][2], A[1][0], A[1][2]) * B[2]
            + det(B[0], A[0][2], B[1], A[1][2]) * A[2][0];

        double gamma =detB/detA;  //gamma
        
        detB = det(A[0][0], A[0][1], A[1][0], A[1][1]) * B[2]
            - det(A[0][0], B[0], A[1][0], B[1]) * A[2][1]
            + det(A[0][1], B[0], A[1][1], B[1]) * A[2][0];

        t=detB/detA;  //t

        // cout << "x1"<< " = " << beta << endl;
        // cout << "x2"<< " = " << gamma << endl;
        // cout << "b+g"<< " = " << (beta+gamma) << endl;
        
        // cout << "x3"<< " = " << t << endl;
        
        
        if((beta+gamma)<1 && beta>0 && gamma>0 && t>0)
            return true;
        else
            return false;
    }

    bool intersect(Ray ray, point &colour, int level, point& intersection, double &t){
        point m,n,o,temp;
        m=p1.sub(p2);   //ax-bx
        n=p1.sub(p3);   //ax-cx
        o=p1.sub(ray.start);    //ax-Rox
        vector<vector<double>> A = {{m.x,n.x,ray.dir.x}, {m.y,n.y,ray.dir.y}, {m.z,n.z,ray.dir.z}};
        vector<double> B = {o.x,o.y,o.z};
        t=-1;
        bool flag=cramer(A,B,t);

        if(flag==false)
            return false;
        

        temp.x = t * ray.dir.x;
        temp.y = t * ray.dir.y;
        temp.z = t * ray.dir.z;

        intersection = ray.start.add(temp);

        //intersect==ray origin
        if(intersection.x==ray.start.x && intersection.y==ray.start.y && intersection.z==ray.start.z)
            return false;

        if(level==0)
            return true;

        colour.x=color[0];
        colour.y=color[1];
        colour.z=color[2];

        point normal;
        m=m.mul(-1,-1,-1);
        n=n.mul(-1,-1,-1);
        normal=m.cross(n);
        if(ray.dir.dot(normal)>0){
            normal=normal.mul(-1,-1,-1);
        }
        normal.norm();
        Coloring(ray, colour,intersection,normal,level);

        return true;
    }


};

class General : public Object {
    double A, B, C, D, E, F, G, H, I, J;

public:
    General(point p, point l, double aa, double bb, double cc, double dd, double ee, double ff, double gg, double hh, double ii, double jj) {
        name = "general";
        reference_point = p;
        length = l.x;
        width = l.y;
        height = l.z;
        setConst(aa, bb, cc, dd, ee, ff, gg, hh, ii, jj);
    }

    void setConst(double aa, double bb, double cc, double dd, double ee, double ff, double gg, double hh, double ii, double jj) {
        A = aa;
        B = bb;
        C = cc;
        D = dd;
        E = ee;
        F = ff;
        G = gg;
        H = hh;
        I = ii;
        J = jj;
    }

    void draw(){
    }

    void print(){
        cout<<name<<endl;
        cout<<"Ref point "<<reference_point.x<<" "<<reference_point.y<<" "<<reference_point.z<<endl;
        cout<<"H: "<<height<<endl;
        cout<<"W: "<<width<<endl;
        cout<<"L: "<<length<<endl;
        cout<<"SHINE: "<<shine<<endl;
        cout<<"Const"<<A<<" "<<B<<" "<<C<<" "<<D<<" "<<E<<" "<<F<<" "<<G<<" "<<H<<" "<<I<<" "<<J<<endl;

        cout<<"R G B: "<<color[0]<<" "<<color[1]<<" "<<color[2]<<endl;

        cout<<"coeff: "<<coEfficients[0]<<" "
        <<coEfficients[1]<<" "
        <<coEfficients[2]<<" "
        <<coEfficients[3]<<endl<<endl;
    }

    bool intersect(Ray ray, point& colour, int level, point& intersection, double& t) override {
        point rd,ro;
        bool flag=false;
        rd=ray.dir.square();
        ro=ray.start.square();
        double a = A * rd.x +
                   B * rd.y +
                   C * rd.z +
                   D * ray.dir.x * ray.dir.y +
                   E * ray.dir.x * ray.dir.z +
                   F * ray.dir.y * ray.dir.z;

        double b = 2 * (A * ray.dir.x * ray.start.x +
                        B * ray.dir.y * ray.start.y +
                        C * ray.dir.z * ray.start.z +
                        D * (ray.dir.x * ray.start.y + ray.dir.y * ray.start.x) +
                        E * (ray.dir.x * ray.start.z + ray.dir.z * ray.start.x) +
                        F * (ray.dir.y * ray.start.z + ray.dir.z * ray.start.y) +
                        G * ray.dir.x +
                        H * ray.dir.y +
                        I * ray.dir.z);

        double c = A * ro.x +
                   B * ro.y +
                   C * ro.z +
                   D * ray.start.x * ray.start.y +
                   E * ray.start.x * ray.start.z +
                   F * ray.start.y * ray.start.z +
                   G * ray.start.x +
                   H * ray.start.y +
                   I * ray.start.z +
                   J;

        double discriminant = b * b - 4 * a * c;
        if (discriminant < epsilon)
            return false;
        
        // if(a<epsilon || a>-1*epsilon)
        // {
        //     t=-1.0*c/b;
        //     if(t<0)
        //         return false;
        //     intersection = ray.start.add(ray.dir.mul(t,t,t));
        //     if (!checkBound(intersection))
        //         return false;
        //     flag=true;
        // }
        // else{
            double t1 = (-b + sqrt(discriminant)) / (2 * a);
            double t2 = (-b - sqrt(discriminant)) / (2 * a);
           
            if (t1 < epsilon && t2 < epsilon)
                return false;

            t=min(t1,t2);
            t2=max(t1,t2);
            
            if(t>0){
                intersection = ray.start.add(ray.dir.mul(t,t,t));
                if (checkBound(intersection))
                    flag=true;
            }
            
            if(t2>0 && flag==false){
                intersection = ray.start.add(ray.dir.mul(t2,t2,t2));
                if (checkBound(intersection)){
                    flag=true;
                    t=t2;
                }
            }
        // }

        if(flag==false)
            return false;

        if(level==0)
            return true;

        //level=1 color
        colour.x=color[0];
        colour.y=color[1];
        colour.z=color[2];

        point normal = calculateNormal(intersection);
        Coloring(ray, colour, intersection, normal, level);
        

        return true;
    }

    point calculateNormal(point intersection) const {
        point normal;
        point p1 = intersection.mul(2 * A, D, E);
        point p2 = intersection.mul(D, 2 * B, F);
        point p3 = intersection.mul(E, F, 2 * C);
        normal.x=p1.x+p1.y+p1.z+G;
        normal.y=p2.x+p2.y+p2.z+H;
        normal.z=p3.x+p3.y+p3.z+I;
        normal.norm();
        return normal;
    }

    bool checkBound(point intersection) const {
        return (length <= 0 || (intersection.x >= reference_point.x && intersection.x <= reference_point.x + length)) &&
               (width <= 0 || (intersection.y >= reference_point.y && intersection.y <= reference_point.y + width)) &&
               (height <= 0 || (intersection.z >= reference_point.z && intersection.z <= reference_point.z + height));
    }
};


class Floor: public Object{
    public:
    Floor(double fWidth=0, double tileWidth=0){
        name="floor";
        reference_point.x=(-1*fWidth)/2.0;
        reference_point.y=(-1*fWidth)/2.0;
        reference_point.z=0;
        width=fWidth;   //obj width-> floor width
        length=tileWidth; //obj length-> tile width
        color[0]=0.5;
        color[1]=0.5;
        color[2]=0.5;
    }

    void square(){
        glBegin(GL_QUADS);
        glVertex3f(0,0,0);
        glVertex3f(0,length,0);
        glVertex3f(length,length,0);
        glVertex3f(length,0,0);
        glEnd();
    }

    void draw(){
        int c;
        int no=int(width/length);
        for(int i=-no/2; i<=no/2; i++){
            for(int j=-no/2; j<=no/2; j++){
                c=(i+j)%2;
                c=c*c;
                glColor3f(c,c,c);

                glPushMatrix();
                glTranslatef(i*length,j*length,0);
                square();
                glPopMatrix();
            }
        }

    }

     bool intersect(Ray ray, point &colour, int level, point& intersection, double &t){
        int no=int(width/length);
        no=no/2;
        double bound=no*length;
        point n(0,0,1);
        point p(0,0,0);
        point temp;

        double denominator = n.x * ray.dir.x + n.y * ray.dir.y + n.z * ray.dir.z;
        if (denominator < epsilon && denominator >-1*epsilon) // parallel
            return false;

        temp=ray.start.sub(p);
        t = -((n.x * temp.x) + (n.y * temp.y) + (n.z * temp.z)) / denominator;
        if (t < 0) // Intersection behind ray's start
            return false;

        temp.x = t * ray.dir.x;
        temp.y = t * ray.dir.y;
        temp.z = t * ray.dir.z;

        intersection = ray.start.add(temp);

        //intersect==ray origin
        if(intersection.x==ray.start.x && intersection.y==ray.start.y && intersection.z==ray.start.z)
            return false;

        if(intersection.x>bound || intersection.x<(-1)*bound || intersection.y>bound || intersection.y<(-1)*bound)
            return false;

        if(level==0)
            return true;

        // level=1, color 
        int a=floor(intersection.x/length);
        int b=floor(intersection.y/length);
        a=(a+b)%2;
        b=a*a;

        colour.x=b;
        colour.y=b;
        colour.z=b;

        point normal(0,0,1);
        if(ray.dir.dot(normal)>0){
            normal=normal.mul(-1,-1,-1);
        }
        normal.norm();
        Coloring(ray, colour,intersection,normal,level);

        return true;   
    }

};




