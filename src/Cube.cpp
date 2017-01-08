#include "Cube.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include <stdlib.h>
#include <stdio.h>


extern Camera myCamera; // get camera object
extern Light myLight;  // get light object
extern GLuint texture[5];
extern bool textures;
extern bool hero;
extern bool defender;
extern bool background;
Cube::Cube()
{  
    vertex[0][0] = -1;vertex[0][1] = -1;vertex[0][2] = -1;
    vertex[1][0] = -1;vertex[1][1] = 1; vertex[1][2] = -1;
    vertex[2][0] = 1;vertex[2][1] = -1; vertex[2][2] = -1;
    vertex[3][0] = 1;vertex[3][1] = 1;vertex[3][2] = -1;
    vertex[4][0] = -1;vertex[4][1] = -1;vertex[4][2] = 1;
    vertex[5][0] = -1;vertex[5][1] = 1; vertex[5][2] = 1;
    vertex[6][0] = 1;vertex[6][1] = -1;vertex[6][2] = 1;
    vertex[7][0] = 1;vertex[7][1] = 1;vertex[7][2] = 1;

    face[0][0] = 0;face[0][1] = 1;face[0][2] = 3;face[0][3] = 2;
    face[1][0] = 3;face[1][1] = 7;face[1][2] = 6;face[1][3] = 2;
    face[2][0] = 7;face[2][1] = 5;face[2][2] = 4;face[2][3] = 6;
    face[3][0] = 4;face[3][1] = 5;face[3][2] = 1;face[3][3] = 0;
    face[4][0] = 5;face[4][1] = 7;face[4][2] = 3;face[4][3] = 1;
    face[5][0] = 6;face[5][1] = 4;face[5][2] = 0;face[5][3] = 2;

    // the face color setting
	faceColor[0][0] = 1.0, faceColor[0][1] = 0.0; faceColor[0][2] = 0.0;
	faceColor[1][0] = 0.0, faceColor[1][1] = 1.0; faceColor[1][2] = 0.0;
	faceColor[2][0] = 0.0, faceColor[2][1] = 0.0; faceColor[2][2] = 1.0;
	faceColor[3][0] = 1.0, faceColor[3][1] = 1.0; faceColor[3][2] = 0.0;
	faceColor[4][0] = 1.0, faceColor[4][1] = 0.0; faceColor[4][2] = 1.0;
	faceColor[5][0] = 0.0, faceColor[5][1] = 1.0; faceColor[5][2] = 1.0;

	// face normal setting
	cube_face_norm_mc[0][0] = 0.0,cube_face_norm_mc[0][1] = 0.0,cube_face_norm_mc[0][2] = -1.0,
	cube_face_norm_mc[1][0] = 1.0, cube_face_norm_mc[1][1] = 0.0, cube_face_norm_mc[1][2] = 0.0;
	cube_face_norm_mc[2][0] = 0.0, cube_face_norm_mc[2][1] = 0.0, cube_face_norm_mc[2][2] = 1.0;
	cube_face_norm_mc[3][0] = -1.0, cube_face_norm_mc[3][1] = 0.0, cube_face_norm_mc[3][2] = 0.0;
	cube_face_norm_mc[4][0] = 0.0, cube_face_norm_mc[4][1] = 1.0, cube_face_norm_mc[4][2] = 0.0;
	cube_face_norm_mc[5][0] = 0.0, cube_face_norm_mc[5][1] = -1.0, cube_face_norm_mc[5][2] = 0.0;



}

void Cube::draw_face(int i)
{
    if(hero){
    	glEnable(GL_TEXTURE_2D);
    	glBindTexture(GL_TEXTURE_2D, texture[0]);
    }else if (defender){
    	glEnable(GL_TEXTURE_2D);
    	glBindTexture(GL_TEXTURE_2D, texture[2]);
    }else if (background){
    	glEnable(GL_TEXTURE_2D);
    	glBindTexture(GL_TEXTURE_2D, texture[3]); // background image
    }else{
    	glEnable(GL_TEXTURE_2D);
    	glBindTexture(GL_TEXTURE_2D, texture[1]);
    }
	glBegin(GL_POLYGON);
		glTexCoord2f(0.0, 0.0); glVertex3fv(&vertex[face[i][0]][0]);
		glTexCoord2f(1.0, 0.0); glVertex3fv(&vertex[face[i][1]][0]);
		glTexCoord2f(1.0, 1.0); glVertex3fv(&vertex[face[i][2]][0]);
		glTexCoord2f(0.0, 1.0); glVertex3fv(&vertex[face[i][3]][0]);
	glEnd();

}

void Cube::draw()
{
	GLfloat shade;
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);

    glPushMatrix();
    this->ctm_multiply();
	glScalef(s, s, s);
    for (int i = 0; i < 6; i++) {

    	// add your code for drawing color and shaded face

    	if (!isBackface(i))
    	{

    	     shade = getFaceShade(i);
    	     //glColor3f(faceColor[i][0]*shade, faceColor[i][1]*shade, faceColor[i][2]*shade);
    	     if(hero==true && i==4){
    	    	 glDisable(GL_TEXTURE_2D);
    	     }else{
    	    	 draw_face(i);
    	     }

    	}

    }
    glPopMatrix();
}

bool Cube::isBackface(int faceindex) {
	GLfloat v[4];
    v[0] = cube_face_norm_mc[faceindex][0];
    v[1] = cube_face_norm_mc[faceindex][1];
    v[2] = cube_face_norm_mc[faceindex][2];
    v[3] = 0.0;
    this->MC.multiply_vector(v);

   // use normal v and ref-eye to computing the backgace

    // return true if faceface false otherwise
    return (myCamera.ref.x-myCamera.eye.x)*v[0] + (myCamera.ref.y - myCamera.eye.y)*v[1] + (myCamera.ref.z - myCamera.eye.z)*v[2] > 0;
}

GLfloat Cube::getFaceShade(int faceindex) {

    GLfloat v[4],s[3],t, faceShade;
    s[0] = myLight.getMC().mat[0][3] - vertex[face[faceindex][0]][0];
    s[1] = myLight.getMC().mat[1][3] - vertex[face[faceindex][0]][1];
    s[2] = myLight.getMC().mat[2][3] - vertex[face[faceindex][0]][2];

    t = sqrt(s[0]*s[0]+s[1]*s[1]+s[2]*s[2]);

    if(t==0){//stop division by 0
    	t=1;
    }
    s[0] = s[0]/t;
    s[1] = s[1]/t;
    s[2] = s[2]/t;

    v[0] = cube_face_norm_mc[faceindex][0];
    v[1] = cube_face_norm_mc[faceindex][1];
    v[2] = cube_face_norm_mc[faceindex][2];
    v[3] = 1.0;
    this->MC.multiply_vector(v);

    t = sqrt(v[0]*v[0]+v[1]*v[1]+v[2]*v[2]);
    if(t==0){ // stop division by 0
    	t=1;
    }
    v[0] = v[0]/t;
    v[1] = v[1]/t;
    v[2] = v[2]/t;

    t = v[0]*s[0]+v[1]*s[1]+v[2]*s[2];

    faceShade = myLight.I*myLight.Rd*t + myLight.Ia*myLight.Ra;

    if (faceShade > 1)
    	faceShade = 1;
    if (faceShade < 0)
    	faceShade = 0;

    return faceShade;
}
void Cube::set_colour(GLfloat x, GLfloat y, GLfloat z){
}

