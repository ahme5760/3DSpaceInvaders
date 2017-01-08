#include "Sphere.hpp"

extern bool change_color;
Sphere::Sphere() {
	radius = 0.1, slices = 50, stacks = 50;
	textureID = 0;
	quad = gluNewQuadric();

	gluQuadricTexture(quad, GL_TRUE);
	gluQuadricOrientation(quad, GLU_OUTSIDE);
	gluQuadricNormals(quad, GLU_SMOOTH);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
}

Sphere::~Sphere() {
}
void Sphere::draw() {
	glDisable(GL_TEXTURE_2D);
	glPushMatrix();
	this->ctm_multiply();
	gluSphere(quad, radius, slices, stacks);
	glPopMatrix();


}
void Sphere::set_colour(GLfloat x, GLfloat y, GLfloat z){
	glColor3f(x, y, z);
}
