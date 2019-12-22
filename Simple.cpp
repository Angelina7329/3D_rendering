#include <GL/glut.h>
#include <stdlib.h>
#include <math.h>
#include <stdio.h>
#include <GL/glaux.h>
#include <vector>
#include <iostream>  
#include <math.h>
#include <Windows.h>
#include <string>
#include <fstream>
#include <sstream>
using namespace std;
static GLuint texName;

int vertexnum, facenum, edgenum;

vector<double>* vx;//顶点x坐标
vector<double>* vy;//顶点y坐标
vector<double>* vz;//顶点z坐标
vector<double*>* normal;//面法向量
vector<vector<int>*>* e;//面

						//鼠标交互
int mousetate = 0; //鼠标当前的状态
GLfloat Oldx = 0.0; // 点击之前的位置
GLfloat Oldy = 0.0;

float xRotate = 0.0f;
float yRotate = 0.0f;
float ty = 0.0f;
float tx = 0.0f;
float tz = 0.0f;
float scale = 1.5;

void readfile(string file) {
	string t = file.substr(file.length() - 3, 3);
	if (!t.compare("off")) {
		fstream f(file, ios::in);
		string type;
		f >> type;
		if (type.compare("OFF")) {
			cout << "文件类型有误" << endl;
			exit;
		}

		f >> vertexnum >> facenum >> edgenum;
		vx = new vector<double>();
		vy = new vector<double>();
		vz = new vector<double>();
		for (int i = 0; i < vertexnum; i++) {
			double temp1, temp2, temp3;
			f >> temp1; f >> temp2; f >> temp3;
			vx->push_back(temp1); vy->push_back(temp2); vz->push_back(temp3);
		}

		int num;
		e = new vector<vector<int>*>();
		normal = new vector<double*>();

		for (int i = 0; i < facenum; i++) {
			f >> num;
			normal->push_back(new double[3]);
			e->push_back(new vector<int>());

			for (int j = 0; j < num; j++) {
				int temp;
				f >> temp;
				e->at(i)->push_back(temp);
			}
			double u1 = vx->at(e->at(i)->at(0)) - vx->at(e->at(i)->at(1));
			double u2 = vy->at(e->at(i)->at(0)) - vy->at(e->at(i)->at(1));
			double u3 = vz->at(e->at(i)->at(0)) - vz->at(e->at(i)->at(1));
			double v1 = vx->at(e->at(i)->at(0)) - vx->at(e->at(i)->at(2));
			double v2 = vy->at(e->at(i)->at(0)) - vy->at(e->at(i)->at(2));
			double v3 = vz->at(e->at(i)->at(0)) - vz->at(e->at(i)->at(2));
			double c1 = u2*v3 - v2*u3;
			double c2 = u3*v1 - v3*u1;
			double c3 = u1*v2 - v1*u2;
			double c = sqrt(c1*c1 + c2*c2 + c3*c3);
			normal->at(i)[0] = c1 / c;
			normal->at(i)[1] = c2 / c;
			normal->at(i)[2] = c3 / c;
		}

	}
	if (!t.compare("obj")) {
		fstream f(file, ios::in);
		char line[1024] = { 0 };
		vx = new vector<double>();
		vy = new vector<double>();
		vz = new vector<double>();
		e = new vector<vector<int>*>();
		normal = new vector<double*>();
		vertexnum = 0; facenum = 0;
		while (f.getline(line, sizeof(line)))
		{
			stringstream word(line);
			string str;
			word >> str;
			if (!str.compare("v")) {
				double temp1, temp2, temp3;
				word >> temp1; word >> temp2; word >> temp3;
				vx->push_back(temp1); vy->push_back(temp2); vz->push_back(temp3);
				vertexnum++;

			}
			else if (!str.compare("f")) {
				normal->push_back(new double[3]);
				e->push_back(new vector<int>());
				while (!word.eof()) {
					double temp;
					word >> temp;
					e->at(facenum)->push_back(temp - 1);
				}
				double u1 = vx->at(e->at(facenum)->at(0)) - vx->at(e->at(facenum)->at(1));
				double u2 = vy->at(e->at(facenum)->at(0)) - vy->at(e->at(facenum)->at(1));
				double u3 = vz->at(e->at(facenum)->at(0)) - vz->at(e->at(facenum)->at(1));
				double v1 = vx->at(e->at(facenum)->at(0)) - vx->at(e->at(facenum)->at(2));
				double v2 = vy->at(e->at(facenum)->at(0)) - vy->at(e->at(facenum)->at(2));
				double v3 = vz->at(e->at(facenum)->at(0)) - vz->at(e->at(facenum)->at(2));
				double c1 = u2*v3 - v2*u3;
				double c2 = u3*v1 - v3*u1;
				double c3 = u1*v2 - v1*u2;
				double c = sqrt(c1*c1 + c2*c2 + c3*c3);
				normal->at(facenum)[0] = c1 / c;
				normal->at(facenum)[1] = c2 / c;
				normal->at(facenum)[2] = c3 / c;
				facenum++;
			}
		}

	}
	if (!t.compare("ply")) {
		fstream f(file, ios::in);
		char line[1024] = { 0 };
		vx = new vector<double>();
		vy = new vector<double>();
		vz = new vector<double>();
		e = new vector<vector<int>*>();
		normal = new vector<double*>();
		vertexnum = 0; facenum = 0;
		while (f.getline(line, sizeof(line))) {
			stringstream word(line);
			string str;
			word >> str;
			if (!str.compare("end_header"))
				break;
			if (!str.compare("element")) {
				word >> str;
				if (!str.compare("vertex")) {
					word >> vertexnum;
				}
				else if (!str.compare("face")) {
					word >> facenum;
				}
			}
		}
		for (int i = 0; i < vertexnum; i++) {
			double temp1, temp2, temp3;
			f >> temp1; f >> temp2; f >> temp3;
			vx->push_back(temp1); vy->push_back(temp2); vz->push_back(temp3);
		}
		int num;
		for (int i = 0; i < facenum; i++) {
			f >> num;
			normal->push_back(new double[3]);
			e->push_back(new vector<int>());

			for (int j = 0; j < num; j++) {
				int temp;
				f >> temp;
				e->at(i)->push_back(temp);
			}
			double u1 = vx->at(e->at(i)->at(0)) - vx->at(e->at(i)->at(1));
			double u2 = vy->at(e->at(i)->at(0)) - vy->at(e->at(i)->at(1));
			double u3 = vz->at(e->at(i)->at(0)) - vz->at(e->at(i)->at(1));
			double v1 = vx->at(e->at(i)->at(0)) - vx->at(e->at(i)->at(2));
			double v2 = vy->at(e->at(i)->at(0)) - vy->at(e->at(i)->at(2));
			double v3 = vz->at(e->at(i)->at(0)) - vz->at(e->at(i)->at(2));
			double c1 = u2*v3 - v2*u3;
			double c2 = u3*v1 - v3*u1;
			double c3 = u1*v2 - v1*u2;
			double c = sqrt(c1*c1 + c2*c2 + c3*c3);
			normal->at(i)[0] = c1 / c;
			normal->at(i)[1] = c2 / c;
			normal->at(i)[2] = c3 / c;
		}
	}
}

AUX_RGBImageRec *LoadBMP(char *Filename)				
{
	FILE *File=NULL;								
	if(!Filename){
		return NULL;									
	}

	File=fopen(Filename,"r");						

	if (File){
		fclose(File);								
		return auxDIBImageLoad(Filename);				
	}
	return NULL;									
}

int LoadGLTextures()								
{
	int Status = FALSE;								
	AUX_RGBImageRec *Image[1];				
	memset(Image,0,sizeof(void *)*1);           

	if (Image[0]=LoadBMP("Data/box.bmp"))
	{
		Status=TRUE;									
		glGenTextures(1, &texName);	
		glBindTexture(GL_TEXTURE_2D, texName);

		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, Image[0]->sizeX, Image[0]->sizeY, 0, GL_RGB, GL_UNSIGNED_BYTE, Image[0]->data);
	}

	if (Image[0]){
		if (Image[0]->data){
			free(Image[0]->data);			
		}
		free(Image[0]);						
	}
	return Status;									
}

void display(void)
{

	glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
//	glEnable(GL_TEXTURE_2D);
//	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE);

	glLoadIdentity();
	gluLookAt (0.0, 0.0, 5.0, 0.0, 0.0, 0.0, 0.0, 1.0, 0.0);
	glRotatef(xRotate, 1.0f, 0.0f, 0.0f);
	glRotatef(yRotate, 0.0f, 1.0f, 0.0f);
	glTranslatef(tx, ty, tz);
	glScalef(scale, scale, scale);


	glBindTexture(GL_TEXTURE_2D, texName);
    //glBegin (GL_TRIANGLES);
	
	for (int i = 0; i <facenum; i++) {
		glBegin(GL_POLYGON);
		glNormal3f(normal->at(i)[0], normal->at(i)[1], normal->at(i)[2]);
		if (e->at(i)->size() == 4) {
			glTexCoord2f(0.0, 0.0);
			glVertex3f(vx->at(e->at(i)->at(0)), vy->at(e->at(i)->at(0)), vz->at(e->at(i)->at(0)));
			glTexCoord2f(0.0, 1.0);
			glVertex3f(vx->at(e->at(i)->at(1)), vy->at(e->at(i)->at(1)), vz->at(e->at(i)->at(1)));
			glTexCoord2f(1.0, 1.0);
			glVertex3f(vx->at(e->at(i)->at(2)), vy->at(e->at(i)->at(2)), vz->at(e->at(i)->at(2)));
			glTexCoord2f(1.0, 0.0);
			glVertex3f(vx->at(e->at(i)->at(3)), vy->at(e->at(i)->at(3)), vz->at(e->at(i)->at(3)));
		}
		else if (e->at(i)->size() == 3) {
			glTexCoord2f(0.0, 0.0);
			glVertex3f(vx->at(e->at(i)->at(0)), vy->at(e->at(i)->at(0)), vz->at(e->at(i)->at(0)));
			glTexCoord2f(0.0, 1.0);
			glVertex3f(vx->at(e->at(i)->at(1)), vy->at(e->at(i)->at(1)), vz->at(e->at(i)->at(1)));
			glTexCoord2f(1.0, 0.0);
			glVertex3f(vx->at(e->at(i)->at(2)), vy->at(e->at(i)->at(2)), vz->at(e->at(i)->at(2)));
		}
		else {
			cout << "error!" << endl;
		}
		glEnd();
	}
    

	glutSwapBuffers();
	glFlush();
}



void reshape(int w, int h)
{
	// Prevent a divide by zero, when window is too short
	// (you cant make a window of zero width).
	if(h == 0)
		h = 1;

	float ratio = 1.0f * w / h;
	// Set the viewport to be the entire window
    glViewport(0, 0, w, h);
	
	//下面给出你的投影矩阵
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, ratio, 1.0, 255.0);	
	
	//下面给出你的模型视点变换矩阵
	glMatrixMode(GL_MODELVIEW);

}

int init (void) 
{
	glClearColor (0.0, 0.0, 0.0, 0.0);
    glShadeModel(GL_SMOOTH);

	GLfloat ambient[]={2.5,1.5,0.5,1.0};
	GLfloat diffuse[]={2.0,1.0,1.0};
	GLfloat specular[]={1.0,1.5,0.1};
	GLfloat position[]={1.0,0.0,2.0,1.0};
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	glLightfv(GL_LIGHT0, GL_AMBIENT, ambient);
	glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuse);
	glLightfv(GL_LIGHT0, GL_SPECULAR, specular);

//打开光源0
	glEnable( GL_LIGHT0 );

//打开光照
	glEnable( GL_LIGHTING );
//////////////////////////////////////////////////////////////////////////////////
	
	GLfloat l_specular[]={0.33,0.33,0.52,1.0};
	GLfloat shininess[]={10.0};
	glMaterialfv(GL_FRONT_AND_BACK,GL_SPECULAR,l_specular);
	glMaterialfv(GL_FRONT_AND_BACK,GL_SHININESS,shininess);
	glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);

//深度测试
	glClearDepth(1.0);
    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

//设置纹理
	if (!LoadGLTextures()){
			return FALSE;
		}
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);
	glEnable(GL_TEXTURE_2D);
	return TRUE;
}


void myKeyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case '=':
		scale += 0.01;
		glutPostRedisplay();
		break;
	case '-':
		scale -= 0.01;
		glutPostRedisplay();
		break;
	}
}

//  键盘交互 
void mySpecial(int key, int x, int y) {
	switch (key) {

	case GLUT_KEY_UP:
		ty += 0.02;
		break;
	case GLUT_KEY_DOWN:
		ty -= 0.02;
		break;
	case GLUT_KEY_LEFT:
		tx -= 0.02;
		break;
	case GLUT_KEY_RIGHT:
		tz -= 0.02;
		break;
	case GLUT_KEY_PAGE_UP:
		tx += 0.02;
		break;
	case GLUT_KEY_PAGE_DOWN:
		tz += 0.02;;
		break;

	default:
		break;
	}
	glutPostRedisplay();
}

// 鼠标交互
void myMouse(int button, int state, int x, int y)
{
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		mousetate = 1;
		Oldx = x;
		Oldy = y;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP)
		mousetate = 0;
	glutPostRedisplay();
}

// 鼠标运动时
void onMouseMove(int x, int y) {
	if (mousetate) {
		//x对应y是因为对应的是法向量
		yRotate += x - Oldx;
		glutPostRedisplay();
		Oldx = x;
		xRotate += y - Oldy;
		glutPostRedisplay();
		Oldy = y;
	}
}


//下面是主程序
int main(int argc, char** argv)
{
   glutInit(&argc, argv);
   glutInitDisplayMode (GLUT_DOUBLE | GLUT_RGB);
   //设置窗口大小，以像素为单位
   glutInitWindowSize (800, 600); 
   glutInitWindowPosition (100, 100);
   glutCreateWindow ("ex4");
   readfile("cube.off");
   init();
   glutDisplayFunc(display); 
   glutMouseFunc(myMouse);
   glutMotionFunc(onMouseMove); // 鼠标移动的时候的函数
   glutSpecialFunc(mySpecial);
   glutKeyboardFunc(myKeyboard);
   glutReshapeFunc( reshape );
   glutMainLoop();
   return 0;   /* ANSI C requires main to return int. */
}
