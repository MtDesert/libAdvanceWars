#include"gl.h"

void displayFunc(){
	glClearColor(0,0,0,1);
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT|GL_STENCIL_BUFFER_BIT);
	
	glEnableClientState(GL_VERTEX_ARRAY);
	glEnableClientState(GL_NORMAL_ARRAY);
	glEnableClientState(GL_COLOR_ARRAY);
	glEnableClientState(GL_TEXTURE_COORD_ARRAY);
	
	//glBindTexture(GL_TEXTURE_2D,texture);
	GLfloat vertex[]={16,16 , 32,16 , 32,32 , 16,32};
	glVertexPointer(2,GL_FLOAT,0,vertex);
	
	GLfloat texCoord[]={0,0 , 1,0 , 1,1 , 0,1};
	glTexCoordPointer(2,GL_FLOAT,0,texCoord);
	
	glDrawArrays(GL_TRIANGLE_FAN,0,4);
	glFlush();
	
	glDisableClientState(GL_VERTEX_ARRAY);
	glDisableClientState(GL_NORMAL_ARRAY);
	glDisableClientState(GL_COLOR_ARRAY);
	glDisableClientState(GL_TEXTURE_COORD_ARRAY);
}
