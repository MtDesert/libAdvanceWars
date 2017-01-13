#include<GL/glut.h>
#include"Renderer_BattleField.h"
#include"Texture.h"
#include<stdio.h>

Texture *tex;
void displayFunc(){
	glClearColor(0,0,1,1);
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
	tex->draw();
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
	glutDisplayFunc(displayFunc);
	//make 0,0 to left
	//glScalef(2.0/w,2.0/h,1);
	//glTranslatef(-w/2+1,-h/2+1,0);
	//gl function
	glEnable(GL_TEXTURE_2D);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	//load texture
	tex=new Texture();
	FileBMP fileBmp;
	fileBmp.loadFile("bmp.bmp");
	fileBmp.parseData();
	tex->setTexImage2D(fileBmp);
	//main loop
	glutMainLoop();
	glutDestroyWindow(window);
	return 0;
}