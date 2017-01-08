#ifndef CWORLD_HPP
#define CWORLD_HPP

#include "Cube.hpp"
#include "Light.hpp"
#include <iostream>
#include <list>
#include <vector>


class World {
public:
	Shape* list[19];

	std::list<Shape*> ObjectList;
	std::vector<Shape*>* ObjectVector;

	World();
	~World();
	void draw_world(); // draw all objects in the world
	void delete_alien(Shape* const &_value);
	void reset();
};

#endif
