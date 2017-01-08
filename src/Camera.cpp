#include <stdio.h>
#include "Camera.hpp"
#include "Matrix.hpp"
#include <math.h>


bool DEBUG = false;

Camera::Camera(){
	eye.x = 3.0, eye.y = 3.0, eye.z = 7.0;
	ref.x = 0.0, ref.y = 0.0, ref.z = 0.0;
	viewup.x = 0.0,viewup.y = 1.0, viewup.z = 0.0;
	aspect = 1.0, viewAngle = 40.0, nearDist = 1.0, farDist = 40.0;
}


void Camera::setDefaultCamera(void) { // make default camera
	eye.x = 3.0, eye.y = 3.0, eye.z = 7.0;
	ref.x = 0.0, ref.y = 0.0, ref.z = 0.0;
	viewup.x = 0.0,viewup.y = 1.0, viewup.z = 0.0;
	aspect = 1.0, viewAngle = 40.0, nearDist = 1.0, farDist = 40.0;
}


void Camera::set(Point Eye, Point Look, Vector Up) {
	eye.set(Eye);
	ref.set(Look);
	viewup.set(Up);
	aspect = 1.0, viewAngle = 40.0, nearDist = 1.0, farDist = 10.0;
    //setModelViewMatrix();
}

void Camera::rotate(GLfloat rx, GLfloat ry, GLfloat rz, GLfloat angle){ //w.r.p.t WC
	Matrix* m = new Matrix();
	m->rotate(rx, ry, rz, angle);
	GLfloat v[4];
	v[0] = this->eye.x;
	v[1] = this->eye.y;
	v[2] = this->eye.z;

	m->multiply_vector(v);
	this->eye.x = v[0];
	this->eye.y = v[1];
	this->eye.z = v[2];

	delete m;

}

void Camera::translate(GLfloat tx, GLfloat ty, GLfloat tz){ //w.r.p.t WC
	this->eye.x += tx;
	this->eye.y += ty;
	this->eye.z += tz;
}

void Camera::setProjectionMatrix() {
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(viewAngle, aspect, nearDist, farDist);
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	gluLookAt(eye.x, eye.y, eye.z, ref.x, ref.y, ref.z,
			viewup.x,viewup.y,viewup.z);

}
