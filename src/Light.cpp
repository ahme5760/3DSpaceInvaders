/*
 *	Name: 
 *	ID: 
 *	Class: CP411
 *	Assignment: 3
 */
#include "Light.hpp"
#include "Matrix.hpp"
#include <stdio.h>


Light::Light(){
	I = 1.0;
	Rd = 0.5;
	Ia = 1.0;
	Ra = 0.5;
	On = true;
	size = 30;


}

/*
 * Reset
 * 	a method to reset the light
 * Parameters:
 * 	none
 * Returns
 * 	none
 */
void Light::Reset(){
	I = 1.0;
	Rd = 0.5;
	Ia = 1.0;
	Ra = 0.5;
	On = true;
	size = 30;

}


/*
 * Increment
 * a function to increment the light source
 * Parameters:
 * 	I: the increment to point light intensity
 * 	Ra: the increment to the background ambient reflection coefficient
 * 	Rd: the increment to the point light diffusion reflection coefficient
 * Returns:
 * 	none
 */
void Light::Increment(GLfloat p, GLfloat ra, GLfloat rd){
// add change  to  I, Ra, Rd
	I=I+p;
	Ia=Ia+p;
	Ra=Ra+ra;
	Rd=Rd+rd;
	if (Ia < 0)
		Ia = 0;
	if (Ia > 1 )
		Ia = 1;
	if (I < 0)
		I = 0;
	if (I > 1 )
		I = 1;
	if (Ra < 0)
		Ra = 0;
	if (Ra > 1)
		Ra = 1;
	if (Rd < 0)
		Rd = 0;
	if (Rd > 1)
		Rd = 1;


}

/*
 * draw
 * 	a method that draws the light
 * Parameters:
 * 	none
 * Returns
 * 	none
 */
void Light::draw(){
	glDisable(GL_TEXTURE_2D);
	if(On == true){
		glPushMatrix();
		this->ctm_multiply();

		glColor3f(I, I, I);
		glEnable(GL_POINT_SMOOTH);
		glPointSize(size);
		glBegin(GL_POINTS);
			glVertex3f(getMC().mat[0][3], getMC().mat[1][3], getMC().mat[2][3]);
		glEnd();
		glPopMatrix();
	}
}

void Light::set_colour(GLfloat x, GLfloat y, GLfloat z){

}




