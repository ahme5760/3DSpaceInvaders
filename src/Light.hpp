
#ifndef LIGHT_HPP
#define LIGHT_HPP
#include <GL/glut.h>
#include "Shape.hpp"



class Light : public Shape {
	public:
		//light and shading. use MC origin as the light position
		GLfloat I, Rd, Ia, Ra;
		GLboolean On;
		GLfloat size;

		Light();
		void Reset();
		void draw();
		void set_colour(GLfloat x, GLfloat y, GLfloat z);
		//void LightReset();
		void Increment(GLfloat, GLfloat, GLfloat);


};
#endif /* LIGHT_HPP */
