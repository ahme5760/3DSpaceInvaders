#ifndef SPHERE_H
#define SPHERE_H

#include <windows.h>
#include <GL/glut.h>
#include "Shape.hpp"

class Sphere: public Shape {
public:

	GLfloat radius;
	GLint slices, stacks;
	GLuint textureID;
	GLUquadric *quad;
	Sphere();
	~Sphere();
	void draw();
	void set_colour(GLfloat x, GLfloat y, GLfloat z);
	void bulletColor(int value);
};

#endif
