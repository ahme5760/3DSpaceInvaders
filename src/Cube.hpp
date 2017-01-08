#ifndef CCUBE_H
#define CCUBE_H

#include <GL/glut.h>
#include "Shape.hpp"
#include "Camera.hpp"
#include "Light.hpp"

class Cube: public Shape {
protected:
	GLfloat vertex[8][3];
	GLint face[6][4];
    GLfloat faceColor[6][3];   // RGB color for each face
    GLfloat cube_face_norm_mc[6][3];  // normal for each face in MC


public:
	Cube();
	void draw_face(int);
	void draw();

    bool isBackface(int faceindex);  // test if a face is backface
    GLfloat getFaceShade(int faceindex);  // compute and return shading of a given face
    void set_colour(GLfloat x, GLfloat y, GLfloat z);
};

#endif
