#include<GL/glut.h>
#include"Renderer_BattleField.h"
#include"EngineGLES.h"
#include"Texture.h"
#include<stdio.h>

Texture tex;

void timerFunc(int timerID){
	//glutTimerFunc(500,&timerFunc,0);
}
void idleFunc(){}

void keyboardFunc(unsigned char key, int x,int y){}
void keyboardUpFunc(unsigned char key, int x,int y){}
void specialFunc(int key,int x,int y){}
void specialUpFunc(int key,int x,int y){}
void joystickFunc(unsigned int buttonMask,int x,int y,int z){}
void reshapeFunc(int w,int h){}
void visibilityFunc(int state){
	switch(state){
		case GLUT_VISIBLE:break;
		case GLUT_NOT_VISIBLE:break;
	}
}
void mouseFunc(int button,int state,int x,int y){
	switch(button){
		case GLUT_LEFT_BUTTON:break;
		case GLUT_MIDDLE_BUTTON:break;
		case GLUT_RIGHT_BUTTON:break;
	}
	switch(state){
		case GLUT_UP:break;
		case GLUT_DOWN:break;
	}
}
void motionFunc(int x,int y){}
void passiveMotionFunc(int x,int y){}
void entryFunc(int state){
	switch(state){
		case GLUT_LEFT:break;
		case GLUT_ENTERED:break;
	}
}

void displayFunc(){
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);

	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	//glColor3f(0,0,1);
	/*for(int x=0;x<25;++x){
		for(int y=0;y<25;++y){
			GLfloat vertex[]={x*16,y*16 , (x+1)*16-1,y*16 , (x+1)*16-1,(y+1)*16-1 , x*16,(y+1)*16-1};
			glVertexPointer(2,GL_FLOAT,0,vertex);
			
			GLfloat texCoord[]={0,0 , 1,0 , 1,1 , 0,1};
			glTexCoordPointer(2,GL_FLOAT,0,texCoord);
			
			glDrawArrays(GL_TRIANGLE_FAN,0,4);
		}
	}*/
	tex.draw(Point2D<GLfloat>(0,0));
	glFlush();
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}

int main(int argc,char* argv[]){
	//init
	glutInit(&argc,argv);
	glutInitDisplayMode(GLUT_RGB|GLUT_SINGLE);
	glutInitWindowPosition(100,100);
	int w=400,h=400;
	glutInitWindowSize(w,h);
	auto window=glutCreateWindow("AdvanceWars");
	//callback
	glutTimerFunc(500,timerFunc,0);
	glutIdleFunc(idleFunc);

	glutKeyboardFunc(keyboardFunc);
	glutKeyboardUpFunc(keyboardUpFunc);
	glutSpecialFunc(specialFunc);
	glutSpecialUpFunc(specialUpFunc);
	glutJoystickFunc(joystickFunc,1);
	glutReshapeFunc(reshapeFunc);
	glutVisibilityFunc(visibilityFunc);
	glutDisplayFunc(displayFunc);
	glutMouseFunc(mouseFunc);
	glutMotionFunc(motionFunc);
	glutPassiveMotionFunc(passiveMotionFunc);
	glutEntryFunc(entryFunc);
	EngineGLES engine;
	engine.initial(w,h);
	//load texture
	if(argc==2){
		FileBMP fileBmp;
		fileBmp.loadFile(argv[1]);
		fileBmp.parseData();
		tex.texImage2D(fileBmp);
	}
	//main loop
	glutMainLoop();
	glutDestroyWindow(window);
	return 0;
}