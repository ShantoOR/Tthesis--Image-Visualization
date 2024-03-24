#include<iostream>
#include<algorithm>
#include<cmath>
#include<map>
#include<vector>
#include"spline.h"
#include<fstream>

#include <GL/glut.h>

#define PI (2*acos(0.0))

using namespace std;

double cameraDistance;
double cameraHeight;
double cameraAngle;
int drawgrid;
int drawaxes;
double angle;

struct point
{
	double x,y,z;

	point() = default;

	point(double x,double y,double z){
		this->x = x;
		this->y = y;
		this->z = z;
	}

	point operator +(point p){
		return point(this->x + p.x,this->y + p.y,this->z + p.z);
	}

	point operator -(point p){
		return point(this->x - p.x,this->y - p.y,this->z - p.z);
	}

	point operator /(double x){
		return point(this->x / x,this->y / x,this->z / x);
	}

	point operator *(double x){
		return point(this->x * x,this->y * x,this->z * x);
	}

	friend std::ostream& operator<<(std::ostream& os, point p){
		os << p.x << ", " << p.y << ", " << p.z << std::endl;
		return os;
	}

	friend istream& operator>>(istream& is, point &p){
		is >> p.x  >> p.y  >> p.z;
		return is;
	}
};

void drawAxes()
{
	if(drawaxes==1)
	{
		glColor3f(1.0, 0.0, 0.0);
		glBegin(GL_LINES);{
			glVertex3f( 100,0,0);
			glVertex3f(-100,0,0);
		}glEnd();


		glColor3f(0.0, 1.0, 0.0);
		glBegin(GL_LINES);{
			glVertex3f(0,-100,0);
			glVertex3f(0, 100,0);
		}glEnd();

		glColor3f(0.0, 0.0, 1.0);
		glBegin(GL_LINES);{
			glVertex3f(0,0, 100);
			glVertex3f(0,0,-100);
		}glEnd();
	}
}

void drawSquare(double a)
{
    //glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( a, a,0);
		glVertex3f( a,-a,0);
		glVertex3f(-a,-a,0);
		glVertex3f(-a, a,0);
	}glEnd();
}

void drawRect(double x, double y)
{
    //glColor3f(1.0,0.0,0.0);
	glBegin(GL_QUADS);{
		glVertex3f( x/2, y/2,0);
		glVertex3f( x/2,-y/2,0);
		glVertex3f(-x/2,-y/2,0);
		glVertex3f(-x/2, y/2,0);
	}glEnd();
}

void draw2Square(double a)
{
	double thickness = 1.5;
	glBegin(GL_QUADS);{
		glVertex3f( a, a,-thickness);
		glVertex3f( a,-a,-thickness);
		glVertex3f(-a,-a,-thickness);
		glVertex3f(-a, a,-thickness);
	}glEnd();

	glBegin(GL_QUADS);{
		glVertex3f( a, a,-thickness);
		glVertex3f( a,-a,-thickness);
		glVertex3f( a,a,thickness);
		glVertex3f( a, -a,thickness);
	}glEnd();

	for(int i = 0; i < 4; i++){
		glPushMatrix();
		glRotatef(i*90, 0, 0, 1);
		glBegin(GL_QUADS);{
			glVertex3f( -a, -a,-thickness);
			glVertex3f( -a,a,-thickness);
			glVertex3f( -a, -a,thickness);
			glVertex3f( -a, a,thickness);
		}glEnd();
		glPopMatrix();
	}
}


void drawGrid()
{
	int i;
	if(drawgrid==1)
	{
		glColor3f(0.6, 0.6, 0.6);	//grey
		glBegin(GL_LINES);{
			for(i=-8;i<=8;i++){

				if(i==0)
					continue;	//SKIP the MAIN axes

				//lines parallel to Y-axis
				glVertex3f(i*10, -90, 0);
				glVertex3f(i*10,  90, 0);

				//lines parallel to X-axis
				glVertex3f(-90, i*10, 0);
				glVertex3f( 90, i*10, 0);
			}
		}glEnd();
	}
}



void drawCircle(double radius,int segments)
{
    int i;
    struct point points[100];
    //generate points
    for(i=0;i<=segments;i++)
    {
        points[i].x=radius*cos(((double)i/(double)segments)*2*PI);
        points[i].y=radius*sin(((double)i/(double)segments)*2*PI);
    }
    //draw segments using generated points
    for(i=0;i<segments;i++)
    {
        glBegin(GL_LINES);
        {
			glVertex3f(points[i].x,points[i].y,0);
			glVertex3f(points[i+1].x,points[i+1].y,0);
        }
        glEnd();
    }
}

void keyboardListener(unsigned char key, int x,int y){
	switch(key){

		case '1':
			drawgrid=1-drawgrid;
			break;
        
		case '+':
			cameraDistance -= 2;
			break;
		
		case '-':
			cameraDistance += 2;
			break;
		
		default:
			break;
	}
}


void specialKeyListener(int key, int x,int y){
	switch(key){
		case GLUT_KEY_DOWN:
			cameraHeight -= 3.0;
			break;
		case GLUT_KEY_UP:	
			cameraHeight += 3.0;
			break;

		case GLUT_KEY_RIGHT:
			cameraAngle += 0.03;
			break;
		case GLUT_KEY_LEFT:
			cameraAngle -= 0.03;
			break;

		case GLUT_KEY_PAGE_UP:
			break;
		case GLUT_KEY_PAGE_DOWN:
			break;

		case GLUT_KEY_INSERT:
			break;

		case GLUT_KEY_HOME:
			break;
		case GLUT_KEY_END:
			break;

		default:
			break;
	}
}


void mouseListener(int button, int state, int x, int y){	//x, y is the x-y of the screen (2D)
	switch(button){
		case GLUT_LEFT_BUTTON:
			if(state == GLUT_DOWN){		// 2 times?? in ONE click? -- solution is checking DOWN or UP
				drawaxes=1-drawaxes;
			}
			break;

		case GLUT_RIGHT_BUTTON:
			//........
			break;

		case GLUT_MIDDLE_BUTTON:
			//........
			break;

		default:
			break;
	}
}

void perpQuad(double x, double y, double z, bool isThick=true, double thickness=2) {
    double lenXY = sqrt(x*x + z*z);
    double lon   = atan2(x, z);
    double lat   = atan2(y, -lenXY);
    
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(lon * 180 / M_PI , 0, 1, 0);
    glRotatef(lat * 180 / M_PI , 1, 0, 0);

	if(isThick)
    	draw2Square(thickness);
	else drawSquare(thickness);

    glPopMatrix();
}

void perpRect(double x, double y, double z, double rect_x, double rect_y) {

    double lenXY = sqrt(x*x + z*z);
    double lon   = atan2(x, z);
    double lat   = atan2(y, -lenXY);
    
    glPushMatrix();
    glTranslatef(x, y, z);
    // glRotatef(lon * 180 / M_PI , 0, 1, 0);
    glRotatef(lat * 180 / M_PI , 1, 0, 0);
	drawRect(rect_x, rect_y);

    glPopMatrix();
}

void perpCircle(double x, double y, double z, double r) {
    double lenXY = sqrt(x*x + z*z);
    double lon   = atan2(x, z);
    double lat   = atan2(y, -lenXY);
    
    glPushMatrix();
    glTranslatef(x, y, z);
    glRotatef(lon * 180 / M_PI , 0, 1, 0);
    glRotatef(lat * 180 / M_PI , 1, 0, 0);

	drawCircle(r, 20);

    glPopMatrix();
}

void drawHelix() {
    float radius = 20.0f;

    // glBegin(GL_POINTS);
    for (float angle = -180; angle <= 180; angle += 1)
    {
        double x = cos(angle * M_PI / 180) * radius;
        double y = sin(angle * M_PI / 180) * radius;
        double z = angle * M_PI / 180 * 2;
        // radius += 0.1f;
        // glVertex3f(x, y, z);
        perpQuad(x,y,z, true, 1000);
    }
    // glEnd();

}

point getBazierPoints(double t, point p0, point p1, point p2, point p3){
	point p = p0 * (1-t) * (1-t) * (1-t) 
				+ p1 * 3 * (1-t) * (1-t) * t
				+ p2 * 3 * (1-t) * t * t
				+ p3 * t * t * t;
	return p;
}

void drawHelix2(point p0, point p1, point p2, point p3) {
	// glBegin(GL_LINES);{
	// 	for(double t = 0; t < 1; t += .001){
	// 		point p = getBazierPoints(t, p0, p1, p2, p3);
	// 		glVertex3f(p.x, p.y, p.z);
	// 	}
	// }glEnd();

    float radius = 10.0f;
    // glBegin(GL_POINTS);
    for (double t = 0; t < 1; t += .001)
    {
		double angle = t * 1800 - 900;
        double x = cos(angle * M_PI / 180) * radius;
        double y = sin(angle * M_PI / 180) * radius;
        double z = angle * M_PI / 180 * 2;

		point p = getBazierPoints(t, p0, p1, p2, p3);

        // radius += 0.1f;
        // glVertex3f(x, y, z);
		glPushMatrix();
		glTranslatef(x +  p.x, y, z);
        perpQuad(x,y,z);
		glPopMatrix();
    }
    glEnd();

}

void drawBazierCurve(point p0, point p1, point p2, point p3) {
	glBegin(GL_POINTS);{
	for(double t = 0; t <= 1; t+=.0001){
		point p = getBazierPoints(t, p0, p1, p2, p3);

		glVertex3f(p.x, p.y, p.z);
		double rect_y = 8;
		double rect_x = 3.1;

		if (t < .1) {
			rect_x = .1;
			rect_y = 160*t;
		}
		
		perpRect(p.x, p.y, p.z, rect_y, rect_x);
	}
	}glEnd();
}

void drawBetaSheet(point p0, point p1, point p2, point p3){
	// bazier curve
	drawBazierCurve(p0, p1, p2, p3);
}

double ribbonAxisHelper(int axis, double val1, double val2){
	if(axis == 0) return 0;
	if(axis == 1) return val1;
	if(axis == 2) return val2;
	if(axis == -1) return -val1;
	if(axis == -2) return -val2;
	return 0;
}

void drawRibbon(vector<double> x, vector<double> y, double from, double to, point axis){
	tk::spline s(x,y);	

	for (double xi = from; xi <= to; xi += 0.01) {
		double t = s(xi) ;
		double x = ribbonAxisHelper(axis.x, xi, t);
		double y = ribbonAxisHelper(axis.y, xi, t);
		double z = ribbonAxisHelper(axis.z, xi, t);
		perpCircle(x, y, z, 1);
	}
}

void drawBazierRibbon(point p0, point p1, point p2, point p3){
	for(double t = 0; t <= 1; t+=.0001){
		point p = getBazierPoints(t, p0, p1, p2, p3);

		perpCircle(p.x, p.y, p.z, 2);
	}

}

void drawSphere(double radius,int slices,int stacks){
	point points[stacks+1][slices+1];
	
	//generate points
	for(int i=0;i<=stacks;i++)
	{
		double h=radius*sin(((double)i/(double)stacks)*(PI/2));
		double r=radius*cos(((double)i/(double)stacks)*(PI/2));

		for(int j=0;j<=slices;j++)
		{
			points[i][j].x=r*cos(((double)j/(double)slices)*2*PI);
			points[i][j].y=r*sin(((double)j/(double)slices)*2*PI);
			points[i][j].z=h;
		}
	}

	//draw quads using generated points
	for(int i=0;i<stacks;i++)
	{
		// glColor3f((double)i/(double)stacks,(double)i/(double)stacks,(double)i/(double)stacks);
		for(int j=0;j<slices;j++)
		{
			glBegin(GL_QUADS);{
				//upper hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,points[i+1][j].z);
			}glEnd();

			glBegin(GL_QUADS);{
				//lower hemisphere
				glVertex3f(points[i][j].x,points[i][j].y,-points[i][j].z);
				glVertex3f(points[i][j+1].x,points[i][j+1].y,-points[i][j+1].z);
				glVertex3f(points[i+1][j+1].x,points[i+1][j+1].y,-points[i+1][j+1].z);
				glVertex3f(points[i+1][j].x,points[i+1][j].y,-points[i+1][j].z);
			}glEnd();
		}
	}
}

void drawSpheres(point p, double radius){
	glPushMatrix();
	glTranslatef(p.x,p.y,p.z);
	drawSphere(radius, 30, 20);
	glPopMatrix();
}

bool parse(istream& is){
	// cout << "==================================" << endl;

	string input;
	string objectType;
	string colon;

	string sequenceNumber, residueName;
	double radius;
	int noControlPoint;
	point position, color;
	vector<double> x, y;
	int start, end;
	point axis;
	string isTranslate;
	point tranlate, rotate;
	double angle;

	double orientation_x, orientation_y;

	point startPosition, endPosition, control1, control2;
	string shape;
	
	
	if( !(is >> objectType)) return false;
	// cout << objectType << endl;

	
	while (is >> input && input != "}"){
		if(input == "{") continue;
		is >> colon ;

		// cout << input << endl;
		if(input == "sequenceNumber"){
			is >> sequenceNumber;
		}else if(input == "residueName"){
			is >> residueName;
		}else if(input == "radius"){
			is >> radius ;
		} else if(input == "position"){
			is >> position;
		} else if(input == "color"){
			is >> color;
		} else if(input == "noControlPoint"){
			is >> noControlPoint;
		} else if(input == "x"){
			for(int i = 0; i < noControlPoint; i++){
				double _x;
				is >> _x;
				x.push_back(_x);
			}
		} else if(input == "y"){
			for(int i = 0; i < noControlPoint; i++){
				double _x;
				is >> _x;
				y.push_back(_x);
			}
		} else if(input == "start"){
			is >> start;
		} else if(input == "end"){
			is >> end;
		} else if(input == "axis"){
			is >> axis;
		} else if(input == "isTranslate"){
			is >> isTranslate;
		} else if(input == "translate"){
			is >> tranlate;
		} else if(input == "rotate"){
			is >> angle >> rotate;
		} else if(input == "startPosition"){
			is >> startPosition;
		} else if(input == "endPosition"){
			is >> endPosition;
		} else if(input == "controls"){
			is >> control1 >> control2;
		} else if(input == "orientation"){
			is >> orientation_x >> orientation_y;
		} else if(input == "shape"){
			is >> shape;
		}
	}

	// cout << "==================================" << endl;

	if(objectType == "sphere"){
		glColor3f(color.x, color.y, color.z);
		drawSpheres(position, radius);
	}
	else if(objectType == "fiber"){
		if(shape == "spline"){
			glPushMatrix();
			glColor3f(color.x, color.y, color.z);
			if(isTranslate == "t"){
				glTranslatef(tranlate.x, tranlate.y, tranlate.z);
				glRotatef(angle, rotate.x, rotate.y, rotate.z);
			}else{
				glRotatef(angle, rotate.x, rotate.y, rotate.z);
				glTranslatef(tranlate.x, tranlate.y, tranlate.z);
			}
			drawRibbon(x, y, start, end, axis);
			glPopMatrix();
		}else if(shape == "bazier") {
			glPushMatrix();
			glColor3f(color.x, color.y, color.z);
			drawBazierRibbon(startPosition, control1, control2, endPosition);
			glPopMatrix();
		}
	}
	else if(objectType == "sheet"){
		glPushMatrix();
		glColor3f(color.x, color.y, color.z);
		drawBetaSheet(startPosition, control1, control2, endPosition);
		glPopMatrix();
	
	}
	else if(objectType == "helix"){
		glPushMatrix();
		glColor3f(color.x, color.y, color.z);
		glTranslatef(position.x, position.y, position.z);
		glRotatef(orientation_x, 0, 1, 0);
		glRotatef(orientation_y, 0, 0, 1);
		drawHelix2(startPosition, control1, control2, endPosition);
		glPopMatrix();
	}

	return true;
}

void display(){
	ifstream fin("map.txt");


	//clear the display
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(1,1,1,0);	//color black
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	/********************
	/ set-up camera here
	********************/
	//load the correct matrix -- MODEL-VIEW matrix
	glMatrixMode(GL_MODELVIEW);

	//initialize the matrix
	glLoadIdentity();

	//now give three info
	//1. where is the camera (viewer)?
	//2. where is the camera looking?
	//3. Which direction is the camera's UP direction?

	//gluLookAt(100,100,100,	0,0,0,	0,0,1);
	gluLookAt(cameraDistance*cos(cameraAngle), cameraDistance*sin(cameraAngle), cameraHeight,		0,0,0,		0,0,1);
	// gluLookAt(0,0,200,	0,0,0,	0,1,0);
	// camera.look();

	//again select MODEL-VIEW
	glMatrixMode(GL_MODELVIEW);

	
	while(parse(fin))
		;

	//ADD this line in the end --- if you use double buffer (i.e. GL_DOUBLE)
	glutSwapBuffers();
}


void animate(){

	angle+=0.05;
	//codes for any changes in Models, Camera
	glutPostRedisplay();
}

void init(){
	//codes for initialization
	drawgrid=0;
	drawaxes=0;
	cameraHeight=150.0;
	cameraAngle=1.0;
	cameraDistance = 200;
	angle=0;

	//clear the screen
	glClearColor(0,0,0,0);

	/************************
	/ set-up projection here
	************************/
	//load the PROJECTION matrix
	glMatrixMode(GL_PROJECTION);

	//initialize the matrix
	glLoadIdentity();

	//give PERSPECTIVE parameters
	gluPerspective(80,	1,	1,	1000.0);
	//field of view in the Y (vertically)
	//aspect ratio that determines the field of view in the X direction (horizontally)
	//near distance
	//far distance
}

int main(int argc, char **argv){
	glutInit(&argc,argv);
	glutInitWindowSize(650, 650);
	glutInitWindowPosition(0, 0);
	glutInitDisplayMode(GLUT_DEPTH | GLUT_DOUBLE | GLUT_RGB);	//Depth, Double buffer, RGB color

	glutCreateWindow("My OpenGL Program");

	init();

	glEnable(GL_DEPTH_TEST);	//enable Depth Testing

	glutDisplayFunc(display);	//display callback function
	glutIdleFunc(animate);		//what you want to do in the idle time (when no drawing is occuring)

	glutKeyboardFunc(keyboardListener);
	glutSpecialFunc(specialKeyListener);
	glutMouseFunc(mouseListener);

	glutMainLoop();		//The main loop of OpenGL

	return 0;
}