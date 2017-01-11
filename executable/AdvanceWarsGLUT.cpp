#include<GL/glut.h>

void displayFunc();

int main(int argc,char* argv[]){
	//init
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_SINGLE);
	glutInitWindowPosition(100,100);
	int w=400,h=400;
	glutInitWindowSize(w,h);
	auto window=glutCreateWindow("hahaha");
	//callback
	glutDisplayFunc(displayFunc);
	//make 0,0 to left
	glTranslatef(-1,-1,0);
	glScalef(2.0/w,2.0/h,1);
	//main loop
	glutMainLoop();
	glutDestroyWindow(window);
	return 0;
}