//1905108
#include<bits/stdc++.h>
#include "bitmap_image.hpp"
#define PI 2*acos(0.0)

using namespace std;

class point{
public:
    double x,y,z;
    point(){
        x=0;
        y=0;
        z=0;
    }

    point(double a,double b, double c){
        x=a;
        y=b;
        z=c;
    }

    void normalize(){
        double sq=sqrt(x*x+y*y+z*z);
        x=x/sq;
        y=y/sq;
        z=z/sq;
    }

    double dot(point a){
        return x*a.x+y*a.y+z*a.z;
    }

    point cross(point a){
        point r;
        r.x=y*a.z-z*a.y;
        r.y=z*a.x-x*a.z;
        r.z=x*a.y-y*a.x;
        return r;
    }

};

class matrix{
    int row,col;
    double mat[4][4];
    int RR,GG,BB;
public:
    matrix(int r=4,int c=4){
        row=r;
        col=c;
        RR=0;
        GG=0;
        BB=0;

        for(int i=0; i<row; i++){
            for(int j=0; j<col; j++){
                mat[i][j]=0;
            }
        }
    }

    void identity(){
        if(row==col){      
            for(int i=0; i<row; i++){
                for(int j=0; j<col; j++){
                    mat[i][j]=0;
                    if(i==j)
                        mat[i][j]=1;
                }
            }
        }
    }

    void insert(int i, int j, double val){
        mat[i][j]=val;
    }

    double get(int i, int j){
        return mat[i][j];
    }

    matrix product(matrix m){
        matrix result(row,m.col);
        
        if(col!=m.row)
            printf("Matrix mul err");
        else
        for(int i=0; i<row; i++){
            for(int j=0; j<m.col; j++){
                for(int k=0; k<row; k++){
                    result.mat[i][j]+=mat[i][k]*m.mat[k][j];
                }
            }
        }

        return result;
    }

    void genTrans(double tx, double ty, double tz){
        identity();
        mat[0][3]=tx;
        mat[1][3]=ty;
        mat[2][3]=tz;
    }

    void genScaling(double tx, double ty, double tz){
        mat[0][0]=tx;
        mat[1][1]=ty;
        mat[2][2]=tz;
        mat[3][3]=1;
    }

    point R(point x, point a, double angle){
        point r,cr;
        angle=angle*2*PI/360.0;
        double sc=(1-cos(angle))*a.dot(x);
        cr=a.cross(x);
        // cout<<"cos "<<cos(acos(0))<<endl;
        // cout<<"Cross "<<cr.x<<" "<<cr.y<<" "<<cr.z<<endl;
        r.x=x.x*cos(angle)+sc*a.x+cr.x*sin(angle);
        r.y=x.y*cos(angle)+sc*a.y+cr.y*sin(angle);
        r.z=x.z*cos(angle)+sc*a.z+cr.z*sin(angle);
        return r;
    }

    void genRot(double angle, double ax, double ay, double az){
        identity();
        point c1,c2,c3;
        point a(ax,ay,az);
        point i(1,0,0);
        point j(0,1,0);
        point k(0,0,1);
        a.normalize();
        c1=R(i,a,angle);
        c2=R(j,a,angle);
        c3=R(k,a,angle);
        // cout<<"c1 vec "<<c1.x<<" "<<c1.y<<" "<<c1.z<<endl;

        mat[0][0]=c1.x;
        mat[1][0]=c1.y;
        mat[2][0]=c1.z;

        mat[0][1]=c2.x;
        mat[1][1]=c2.y;
        mat[2][1]=c2.z;

        mat[0][2]=c3.x;
        mat[1][2]=c3.y;
        mat[2][2]=c3.z;
    }

    void proj(){
        for(int j=0; j<3; j++){
            for(int i=0; i<3; i++){
                mat[i][j]=mat[i][j]/mat[3][j];
            }
        }
    }

    void setColor(int b,int g,int r){
        RR=r;
        GG=g;
        BB=b;
    }

    tuple<int,int,int> color(){
        return make_tuple(RR,GG,BB);
    }

    void sort(int r=1){
        double temp;
        for(int j=0; j<col-1; j++){
            for(int k=j+1; k<col; k++){
                if(mat[r][k]>mat[r][j]){
                    for(int i=0; i<row; i++){
                        temp=mat[i][j];
                        mat[i][j]=mat[i][k];
                        mat[i][k]=temp;
                    }
                }
            }
        }
    }
};

class stk{
    vector<matrix> v;
    int top;
public:
    stk(){
        top=0;
    }

    void push(matrix m){
        v.push_back(m);
        top++;
    }

    matrix pop(){
        matrix m;
        top--;
        m=v[top];
        v.pop_back();
        return m;
    }
};

void printTraingle(matrix m){
    cout<<fixed<<setprecision(7);
    for(int j=0; j<3; j++){
        for(int i=0; i<3; i++){
            cout<<m.get(i,j)<<" ";
        }
        cout<<"\n";
    }
    cout<<"\n";
}

static unsigned long int g_seed=1;
inline int random(){
    g_seed=(214013*g_seed+2531011);
    return ((g_seed>>16) & 0x7FFF)%256;
}

double** createZBuffer(int width, int height, double z_max) {
    double** zBuffer = new double*[height];
    
    for (int i = 0; i < height; ++i) {
        zBuffer[i] = new double[width];
        for (int j = 0; j < width; ++j) {
            zBuffer[i][j] = z_max;
        }
    }

    return zBuffer;
}

void deleteZBuffer(double** zBuffer, int height) {
    for (int i = 0; i < height; ++i) {
        delete[] zBuffer[i];
    }
    delete[] zBuffer;
}
 //(x,y)(z,y)
double findx(double y, double x1, double y1, double x2, double y2){ //z1-x1
    return x1+(y-y1)*(x2-x1)/(y2-y1);
}


void stage4(vector<matrix> results){
    ifstream config("config.txt");   
    int scrWidth=500,scrHeight=500;
    tuple<int,int,int> color;
    int ind_i=0,ind_j=0;

    double dx,dy,Topy,Leftx,Boty,Rigx;
    double rig=1,lef=-1,top=1,bot=-1;
    double z_front_limit=-1;

    if (config.is_open()) {
        config>>scrWidth>>scrHeight;
        config.close();  
    }
    
    for(int i=0; i<results.size(); i++){
        results[i].setColor(random(),random(),random());
    }
   
    dx=(rig-lef)/scrWidth;
    dy=(top-bot)/scrHeight;
    Topy=top-dy/2;
    Leftx=lef+dx/2;
    Boty=Topy-(scrHeight-1)*dy;
    Rigx=Leftx+(scrWidth-1)*dx;

    //z-bufffer
    double z_max = 1.0;
    double** zBuffer = createZBuffer(scrWidth, scrHeight, z_max);

    bitmap_image image(scrWidth,scrHeight);
    //init with black
    for (unsigned int x = 0; x < scrWidth; ++x){
        for (unsigned int y = 0; y < scrHeight; ++y){
            image.set_pixel(x,y,0,0,0);
        }
    }

    //main algo
    double top_scan,bottom_scan,left_scan,rig_scan;
    int top_scanline,bot_scanline,left_scanline,rig_scanline;

    double y_co,x_co;
    double x_a[2],z_a[2];
    double zdepth;

    //for each triangle
    for(int t=0; t<results.size(); t++){
        results[t].sort();

        if(results[t].get(1,0)==results[t].get(1,1) && results[t].get(1,0)==results[t].get(1,2))
            continue;
       
        top_scan=results[t].get(1,0);
        bottom_scan=results[t].get(1,2);
       
        // clip
        if(top_scan>Topy)
            top_scan=Topy;
        if(bottom_scan<Boty)
            bottom_scan=Boty;
        
        double temp,row;
        //finding index r
        top_scanline = static_cast<int>(ceil((Topy - top_scan) / dy));
        bot_scanline = static_cast<int>(floor((Topy - bottom_scan) / dy));

        for(int ind_i=top_scanline; ind_i<=bot_scanline; ind_i++){
            row=Topy-ind_i*dy;
            if((results[t].get(1,0)==results[t].get(1,1)) || (row < results[t].get(1,1))){   //a-c, b-c ; ay=by
                x_a[0]=findx(row,results[t].get(0,0),results[t].get(1,0),results[t].get(0,2),results[t].get(1,2));
                x_a[1]=findx(row,results[t].get(0,1),results[t].get(1,1),results[t].get(0,2),results[t].get(1,2));

                z_a[0]=findx(row,results[t].get(2,0),results[t].get(1,0),results[t].get(2,2),results[t].get(1,2));
                z_a[1]=findx(row,results[t].get(2,1),results[t].get(1,1),results[t].get(2,2),results[t].get(1,2));
            }
            else { //a-b, a-c; by=cy
                x_a[0]=findx(row,results[t].get(0,0),results[t].get(1,0),results[t].get(0,1),results[t].get(1,1));
                x_a[1]=findx(row,results[t].get(0,0),results[t].get(1,0),results[t].get(0,2),results[t].get(1,2));

                z_a[0]=findx(row,results[t].get(2,0),results[t].get(1,0),results[t].get(2,1),results[t].get(1,1));
                z_a[1]=findx(row,results[t].get(2,0),results[t].get(1,0),results[t].get(2,2),results[t].get(1,2));
            }
            

            if(abs(x_a[0]-x_a[1])<1e-7)
                continue;
            if(x_a[0]>x_a[1]){
                temp=x_a[0];
                x_a[0]=x_a[1];
                x_a[1]=temp;

                temp=z_a[0];
                z_a[0]=z_a[1];
                z_a[1]=temp;
            }

            //clip
            left_scan=x_a[0];
            rig_scan=x_a[1];
            if(left_scan<Leftx)
                left_scan=Leftx;
            if(rig_scan>Rigx)
                rig_scan=Rigx;

            // finding index
            //inner ind_i=row, ind_j=col
            left_scanline = static_cast<int>(ceil((left_scan - Leftx) / dx));
            rig_scanline = static_cast<int>(round((rig_scan - Leftx) / dx));
            color = results[t].color();

            for (ind_j = left_scanline; ind_j <= rig_scanline; ind_j++) {
                temp=Leftx+ind_j*dx;
                zdepth = findx(temp, z_a[0], x_a[0], z_a[1], x_a[1]);

                if (zdepth < z_front_limit)
                    continue;
                // cout<<zdepth<<endl;
                if (zBuffer[ind_i][ind_j] > zdepth) {
                    zBuffer[ind_i][ind_j] = zdepth;
                    image.set_pixel(ind_j, ind_i, std::get<0>(color), std::get<1>(color), std::get<2>(color));
                }
            }
        }

        

    }
    cout<<fixed<<setprecision(6);
    image.save_image("out.bmp");
    freopen("z_buffer.txt", "w", stdout);
    for (int i = 0; i < scrWidth; i++) {
        for (int j = 0; j < scrHeight; j++) {
            if (zBuffer[i][j] < z_max) {
                cout << zBuffer[i][j] << "\t";
            }
        }
        cout << endl;
    }
    deleteZBuffer(zBuffer, scrHeight);
}


int main(){
    freopen("scene.txt","r",stdin);
    freopen("stage1.txt","w",stdout);
    freopen("stage2.txt","w",stdout);
    freopen("stage3.txt","w",stdout);
    
    double eyeX,eyeY,eyeZ,lx,ly,lz,upx,upy,upz,fovY,aspect,near,far;
    string cmd;
    matrix triangle(4,3);
    matrix result(4,3);
    vector<matrix> results;
    double a;

    double tx,ty,tz,angle;
    matrix trans(4,4);
    matrix scal(4,4);
    matrix rot(4,4);

    stk stack;
    matrix M(4,4);
    M.identity();

    cin>>eyeX>>eyeY>>eyeZ;
    cin>>lx>>ly>>lz;
    cin>>upx>>upy>>upz;
    cin>>fovY>>aspect>>near>>far;

    // stage2
    point up(upx,upy,upz);
    point l(lx-eyeX,ly-eyeY,lz-eyeZ);
    l.normalize();
    point r;
    r=l.cross(up);
    r.normalize();
    point u;
    u=r.cross(l);

    matrix T(4,4);
    T.genTrans(-eyeX,-eyeY,-eyeZ);
    matrix R(4,4);
    R.identity();
    R.insert(0,0,r.x);
    R.insert(0,1,r.y);
    R.insert(0,2,r.z);

    R.insert(1,0,u.x);
    R.insert(1,1,u.y);
    R.insert(1,2,u.z);

    R.insert(2,0,l.x*(-1));
    R.insert(2,1,l.y*(-1));
    R.insert(2,2,l.z*(-1));

    matrix V;
    V=R.product(T);
    //print V*...

    //stage3
    double fovX=fovY*aspect;
    double tt=near*tan(fovY*2*PI/(2*360.0));
    double rr=near*tan(fovX*2*PI/(2*360.0));

    matrix P(4,4);
    P.insert(0,0,near/rr);
    P.insert(1,1,near/tt);
    P.insert(2,2,(far+near)*(-1)/(far-near));
    P.insert(2,3,(far*near)*(-2)/(far-near));
    P.insert(3,2,-1);
    //print P*...


    //stage1
    while (true)
    {
        cin>>cmd;
        if(cmd=="triangle"){
            for(int j=0; j<3; j++){
                for(int i=0; i<3; i++){
                    cin>>a;
                    triangle.insert(i,j,a);
                }
            }
            triangle.insert(3,0,1);
            triangle.insert(3,1,1);
            triangle.insert(3,2,1);

            //stage1
            freopen("stage1.txt","a",stdout);
            result=M.product(triangle);
            printTraingle(result);

            //stage2
            freopen("stage2.txt","a",stdout);
            result=V.product(result);
            printTraingle(result);

            //stage3
            freopen("stage3.txt","a",stdout);
            result=P.product(result);
            result.proj();
            printTraingle(result);

            results.push_back(result);
        } 
        else if(cmd=="translate"){
            cin>>tx>>ty>>tz;
            trans.genTrans(tx,ty,tz);
            M=M.product(trans);
        }
        else if(cmd=="scale"){
            cin>>tx>>ty>>tz;
            scal.genScaling(tx,ty,tz);
            M=M.product(scal);
        }
        else if(cmd=="rotate"){
            cin>>angle>>tx>>ty>>tz;
            rot.genRot(angle,tx,ty,tz);
            M=M.product(rot);
        }
        else if(cmd=="push"){
            stack.push(M);
        }
        else if(cmd=="pop"){
            M=stack.pop();
        }
        else if(cmd=="end"){
            break;
        }  
    }


    //stage4
    stage4(results);         
}