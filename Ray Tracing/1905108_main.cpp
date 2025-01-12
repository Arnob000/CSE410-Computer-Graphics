#include <bits/stdc++.h>
#include "1905108_classes.hpp"

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
xy plane, z upword
*/

#define pi (2*acos(0.0))

//camera class
Camera kamera(80,80,80);
//objs
vector<unique_ptr<Object>> objects;
vector<unique_ptr<PointLight>> pointlights;
vector<unique_ptr<SpotLight>> spotlights;

int recursionLevel;
double winHeight=800,winWidth=800;
int imgWidth=800,imgHeight=800;
double viewAngle=60.0;

//testing here

void loadData(){
    cout<<"In load"<<endl;
    recursionLevel=1;
    int pixelNo=0;
    int objs=0;
    int plightsource=0;
    int spotlightsource=0;

    string objName="";

    unique_ptr<Object> temp(nullptr);
    unique_ptr<PointLight> pl(nullptr);
    unique_ptr<SpotLight> sl(nullptr);
    

    cin>>recursionLevel;
    cin>>pixelNo;

    cin>>objs;

    //
    point p,p1,p2,p3;
    double val=0;
    int v=0;
    double a=0,b=0,c=0,d=0,e=0,f=0,g=0,h=0,i=0,j=0;

    while(objs>0){
        objs--;
        cin>>objName;

        if(objName=="sphere"){
            cin>>p.x>>p.y>>p.z;
            cin>>val;
            temp.reset(new Sphere(p,val));

            cin>>p.x>>p.y>>p.z;
            temp->setColor(p.x,p.y,p.z);

            cin>>p.x>>p.y>>p.z;
            cin>>val;
            temp->setCoEfficients(p.x,p.y,p.z,val);

            cin>>v;
            temp->setShine(v);

            temp->print();
            objects.push_back(move(temp));
        }
        else if(objName=="triangle"){
            cin>>p1.x>>p1.y>>p1.z;
            cin>>p2.x>>p2.y>>p2.z;
            cin>>p3.x>>p3.y>>p3.z;
            
            temp.reset(new Triangle(p1,p2,p3));

            cin>>p.x>>p.y>>p.z;
            temp->setColor(p.x,p.y,p.z);

            cin>>p.x>>p.y>>p.z;
            cin>>val;
            temp->setCoEfficients(p.x,p.y,p.z,val);

            cin>>v;
            temp->setShine(v);

            temp->print();
            objects.push_back(move(temp));
        }
        else if(objName=="general"){
            cin>>a>>b>>c>>d>>e>>f>>g>>h>>i>>j;
            cin>>p.x>>p.y>>p.z;
            cin>>p1.x>>p1.y>>p1.z;
            
            temp.reset(new General(p,p1,a,b,c,d,e,f,g,h,i,j));

            cin>>p.x>>p.y>>p.z;
            temp->setColor(p.x,p.y,p.z);

            cin>>p.x>>p.y>>p.z;
            cin>>val;
            temp->setCoEfficients(p.x,p.y,p.z,val);

            cin>>v;
            temp->setShine(v);

            temp->print();
            objects.push_back(move(temp));
        }
    }

    cin>>plightsource;

    while(plightsource>0){
        plightsource--;
        cin>>p.x>>p.y>>p.z;
        cin>>p1.x>>p1.y>>p1.z;

        pl.reset(new PointLight(p,p1.x,p1.y,p1.z));
        pl->print();
        pointlights.push_back(move(pl));
    }

    cin>>spotlightsource;

    while(spotlightsource>0){
        spotlightsource--;

        cin>>p.x>>p.y>>p.z;
        cin>>p1.x>>p1.y>>p1.z;
        cin>>p2.x>>p2.y>>p2.z;
        cin>>val;

        sl.reset(new SpotLight(p,p1.x,p1.y,p1.z,p2,val));
        sl->print();
        spotlights.push_back(move(sl));        
    }

    temp.reset(new Floor(500,20));
    temp->setCoEfficients(0.2,0.3,0.3,0.2);
    temp->setShine(2);
    objects.push_back(move(temp));
}

int imgno=0;


void capture(){
    // init bitmap
    imgno++;
    ostringstream oss;
    oss << imgno;
    string name = "Output_1" + oss.str();
    name+=".bmp";

    bitmap_image image(imgWidth,imgHeight);
    //init with black
    for (unsigned int x = 0; x < imgWidth; ++x){
        for (unsigned int y = 0; y < imgHeight; ++y){
            image.set_pixel(x,y,0,0,0);
        }
    }

    double planeDist=(winHeight/2.0)/tan(viewAngle*M_PI/360.0);

    point topleft,temp;
    topleft=kamera.l();
    topleft=topleft.mul(planeDist,planeDist,planeDist);
    temp=kamera.r();
    temp=temp.mul(winWidth/2,winWidth/2,winWidth/2);
    topleft=topleft.sub(temp);
    temp=kamera.u();
    temp=temp.mul(winHeight/2,winHeight/2,winHeight/2);
    topleft=topleft.add(temp);
    temp=kamera.eye();
    topleft=topleft.add(temp);

    double du=1.0*winWidth/imgWidth;
    double dv=1.0*winHeight/imgHeight;
    temp=kamera.r();
    temp=temp.mul(0.5*du,0.5*du,0.5*du);
    topleft=topleft.add(temp);
    temp=kamera.u();
    temp=temp.mul(0.5*dv,0.5*dv,0.5*dv);
    topleft=topleft.sub(temp);

    double t=0,tmin=0;
    int nearest=-1;
    point currpixel;
    // unique_ptr<Ray> ray(nullptr);
    Ray ray;
    point color,intersect;

    for(int i=0; i<imgWidth; i++){
        for(int j=0; j<imgHeight; j++){
            temp=kamera.r();
            temp=temp.mul(i*du,i*du,i*du);
            currpixel=topleft.add(temp);
            temp=kamera.u();
            temp=temp.mul(j*dv,j*dv,j*dv);
            currpixel=currpixel.sub(temp);
            tmin=numeric_limits<double>::max();
            nearest=-1;

            // ray.reset(new Ray(kamera.eye(), currpixel.sub(kamera.eye())));
            ray.start=kamera.eye();
            ray.dir=currpixel.sub(kamera.eye());
            ray.dir.norm();
            for(int k=0; k<objects.size(); k++){
                if(objects[k]->intersect(ray,color,0,intersect,t)){
                    if(t<tmin){
                        tmin=t;
                        nearest=k;
                    }
                }                
            }
            if(nearest!=-1){
                if(objects[nearest]->intersect(ray,color,1,intersect,t)){
                    color.fix();
                    image.set_pixel(i, j, 255*color.x,255*color.y,255*color.z);
                    // cout<<"fin color "<<color.x<<" "<<color.y<<" "<<color.z<<endl;
                }
            }
                  
        }
    }

    image.save_image(name);
    cout<<"Captured "<<name<<endl;
}





void init(){
    glClearColor(0,0,0.0,0);
    glEnable(GL_DEPTH_TEST);
}


//camera keyboards
void cameraSpecialKey(int key, int x, int y){
    switch(key){
        case GLUT_KEY_UP:   //forward            
            kamera.forward();
            break;
        case GLUT_KEY_DOWN: //backward
            kamera.backward();
            break;
        case GLUT_KEY_RIGHT:    //r
            kamera.right(); 
            break;
        case GLUT_KEY_LEFT: //l
            kamera.left();
            break;
        case GLUT_KEY_PAGE_UP:  //up
            kamera.moveUp();  
            break;
        case GLUT_KEY_PAGE_DOWN:    //down
            kamera.down(); 
            break;
        default:
            break;
}

    
}

void keyboard(unsigned char key, int x, int y){
    switch(key){
        case '0':
            capture();
            break;
        case '1':   //rot l  
            kamera.rotateLeft();
            break;
        case '2':  //rot r
            kamera.rotateRight();
            break;
        case '3':   //rot up
            kamera.lookUp();
            break;
        case '4':    //rot down
            kamera.lookDown();
            break;
        case '5':     //tilt clock
            kamera.tiltClockwise();
            break;
        case '6':   //tilt clock
            kamera.tiltAntiClockwise();
            break;
        
        case 'w':
            kamera.goUp();
            break;
        case 's':
            kamera.goDown();
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
    
    gluLookAt(kamera.camX,kamera.camY,kamera.camZ,kamera.camLX,kamera.camLY,kamera.camLZ,kamera.camAX,kamera.camAY,kamera.camAZ);

    for(int i=0; i<objects.size(); i++){
        objects[i]->draw();
    }

    
    glutSwapBuffers(); //double buffer mode
}

void reshape(int w, int h){
    glViewport(1,0,w,h);   //window te ja dekha jai
    glMatrixMode(GL_PROJECTION); //change to proj matrix
    glLoadIdentity();
    gluPerspective(viewAngle,1,1.0,1000.0);
    glMatrixMode(GL_MODELVIEW);
}

void timer(int a){
    glutPostRedisplay();
    glutTimerFunc(1000/60,timer,0);
}


int main(int argc, char** argv){
    freopen("scene.txt","r",stdin);

    freopen("out.txt","w",stdout);

    glutInit(&argc,argv);
    glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);


   
    kamera.init();
    loadData();

    glutInitWindowPosition(500,100);
    glutInitWindowSize(winWidth,winHeight);

    glutCreateWindow("1905108_Ray_Tracing");
    glutReshapeFunc(reshape);
    gluLookAt(kamera.camX,kamera.camY,kamera.camZ,kamera.camLX,kamera.camLY,kamera.camLZ,kamera.camAX,kamera.camAY,kamera.camAZ);
    
    
    glutDisplayFunc(display);
    glutTimerFunc(0,timer,0);
    glutKeyboardFunc(keyboard);
    glutSpecialFunc(cameraSpecialKey);

    init();

    glutMainLoop();
}