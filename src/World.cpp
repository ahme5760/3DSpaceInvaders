/**
 * File: World.hpp
 */

#include "World.hpp"
#include <stdio.h>



extern bool isSolar;
extern int planet_num;
extern bool hero;
extern int num_aliens;
extern bool defender;
extern bool background;

World::World() {
	for (int k = 0; k <=num_aliens; k++){
		list[k] = new Cube();
		// object list
		ObjectList.push_back(list[0]);
	}
	list[num_aliens+1] = new Cube();		// create 3 defender ships, after creating aliens
	ObjectList.push_back(list[0]);
	list[num_aliens+2] = new Cube();
	ObjectList.push_back(list[0]);
	list[num_aliens+3] = new Cube();
	ObjectList.push_back(list[0]);
	list[num_aliens+4] = new Cube();          // background image (it is a textured cube turned inside out)
	ObjectList.push_back(list[0]);
	// object vector
	ObjectVector = new std::vector<Shape*>(15);
	std::vector<Shape*>& ObjectVectorRef = *ObjectVector;
	for (int k = 0; k <=num_aliens+4; k++){ // +3 defender ships + background image
		ObjectVectorRef[k] = list[k];
	}
}

World::~World(){
	for (int k = 0; k <=num_aliens+4; k++){ // +3 defender ships + background image
		delete list[k];
	}
}

void World::draw_world() {
	hero=true;
	list[0]->draw();
	hero=false;
	defender=true; // defender ships
	list[num_aliens+1]->draw();
	list[num_aliens+2]->draw();
	list[num_aliens+3]->draw();
	defender=false;
	background=true;
	list[num_aliens+4]->draw(); // background image
	background=false;
	for (int k = 1; k <=num_aliens+4; k++){ // +3 defender ships + background image
		if(!list[k]->mark_delete){
			list[k]->draw();
		}

	}
}

void World::reset(){
	for (int k = 0; k <=10+4; k++){ // +3 defender ships + background image
		list[k]->reset();
		list[k]->mark_delete=false;
	}
}


