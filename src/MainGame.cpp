/**
 *	CP411 Final Project.
 *	Khaja Ali Ahmed
 *	110425760
 */

#include <windows.h>
#include <GL/glew.h>
#include <list>
#include <string>
#include <sstream>
#include <iomanip>
#include <locale>
#include <ctime>
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/glut.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <time.h>
#include "World.hpp"
#include "Camera.hpp"
#include "Light.hpp"
#include "Sphere.hpp"

GLint winWidth = 800, winHeight = 800;

GLfloat red = 1.0, green = 1.0, blue = 1.0;  //color
bool shift = false;
bool hero = false;
bool defender = false;
bool background = false;
bool bulletColor = false;
float maxMove = 0;
int theta;
bool resetGame = false;
bool gameOver = false;
std::list<Sphere> bulletList;
std::list<Sphere> alienBulletList;
int num_aliens = 10;
int score=0;
GLint moving = 0, xBegin = 0,ycamera =0;
GLuint texture[5];
bool textures = false;
//Declare a world containing all objects to draw.
Light myLight; // not using
World myWorld;
Camera myCamera;

struct Image {
	unsigned long sizeX;
	unsigned long sizeY;
	char *data;
};
typedef struct Image Image;

int ImageLoad(char *filename, Image *image) {
	FILE *file;
	unsigned long size; // size of the image in bytes.
	unsigned long i; // standard counter.
	unsigned short int planes; // number of planes in image (must be 1)
	unsigned short int bpp; // number of bits per pixel (must be 24)

	char temp; // temporary color storage for bgr-rgb conversion.

	// make sure the file is there.
	if ((file = fopen(filename, "rb")) == NULL) {
		printf("File Not Found : %s\n", filename);
		return 0;
	}

	// seek through the bmp header, up to the width/height:

	fseek(file, 18, SEEK_CUR);

	// read the width

	if ((i = fread(&image->sizeX, 4, 1, file)) != 1) {
		printf("Error reading width from %s.\n", filename);
		return 0;
	}

	// read the height
	if ((i = fread(&image->sizeY, 4, 1, file)) != 1) {
		printf("Error reading height from %s.\n", filename);
		return 0;
	}

	size = image->sizeX * image->sizeY * 3;

	if ((fread(&planes, 2, 1, file)) != 1) {
		printf("Error reading planes from %s.\n", filename);
		return 0;
	}

	if (planes != 1) {
		printf("Planes from %s is not 1: %u\n", filename, planes);
		return 0;
	}

	// read the bitsperpixel
	if ((i = fread(&bpp, 2, 1, file)) != 1) {
		printf("Error reading bpp from %s.\n", filename);
		return 0;
	}

	if (bpp != 24) {
		printf("Bpp from %s is not 24: %u\n", filename, bpp);
		return 0;
	}

	// seek past the rest of the bitmap header.
	fseek(file, 24, SEEK_CUR);

	image->data = (char *) malloc(size);
	if (image->data == NULL) {
		printf("Error allocating memory for color-corrected image data");
		return 0;
	}

	if ((i = fread(image->data, size, 1, file)) != 1) {
		printf("Error reading image data from %s.\n", filename);
		return 0;
	}

	for (i = 0; i < size; i += 3) { // reverse all of the colors. (bgr -> rgb)
		temp = image->data[i];
		image->data[i] = image->data[i + 2];
		image->data[i + 2] = temp;
	}
	return 1;
}

Image* loadTexture(char *filename) {
	Image *image1;
	image1 = (Image *) malloc(sizeof(Image));
	if (image1 == NULL) {
		printf("Error allocating space for image");
		exit(0);
	}
	if (!ImageLoad(filename, image1)) {
		exit(1);
	}
	return image1;
}
void deleteBullet(int i){
	unsigned N = i;
	if (bulletList.size() > N)
	{
	    std::list<Sphere>::iterator it = bulletList.begin();
	    std::advance(it, N);
	    bulletList.erase(it);
	}
}
void alienDeleteBullet(int i){
	unsigned N = i;
	if (alienBulletList.size() > N)
	{
	    std::list<Sphere>::iterator it = alienBulletList.begin();
	    std::advance(it, N);
	    alienBulletList.erase(it);
	}
}

void bulletColorTimer(int x) {
GLfloat speed = 0.04;
static int oldTime = clock(), newTime;
newTime = clock();
theta = (newTime - oldTime) * speed;
oldTime = newTime;

if (x){
	if(bulletColor){
		bulletColor=false;
		glColor3f(1,0,0);
		glutPostRedisplay();
		glutTimerFunc(1000, bulletColorTimer, x);
	}else{
		bulletColor=true;
		glColor3f(1,1,1);
		glutPostRedisplay();
		glutTimerFunc(1000, bulletColorTimer, x);
	}

}


}
void alienCreateBullet(int k){
	if(alienBulletList.size() < 5 && myWorld.list[k]->mark_delete==false){ // limit number of bullets on screen to 5 for added challenge
		Sphere bullet;
		bullet.scale_change(0.1);
		bullet.translate(myWorld.list[k]->getMC().mat[0][3], myWorld.list[k]->getMC().mat[1][3], 0);
		alienBulletList.push_back(bullet);
	}
}
void alienBulletTimer(int x) {
GLfloat speed = 0.04;
static int oldTime = clock(), newTime;
newTime = clock();
theta = (newTime - oldTime) * speed;
oldTime = newTime;
if (x){
	// create random alien bullet
	int v3 = rand() % num_aliens + 1;
	alienCreateBullet(v3);
	glutPostRedisplay();
	glutTimerFunc(5000, alienBulletTimer, x);
}
}
void checkGameOver(){
	for (int k=1; k<=num_aliens; k++){ // use collision algorithm to find out if alien that is not deleted hits defender
		if(!myWorld.list[k]->mark_delete){
	    	GLfloat alienDefenderDistance1 = sqrt(pow(myWorld.list[k]->MC.mat[0][3] - myWorld.list[num_aliens+1]->MC.mat[0][3],2) + pow(myWorld.list[k]->MC.mat[1][3] - myWorld.list[num_aliens+1]->MC.mat[1][3],2) + pow(myWorld.list[k]->MC.mat[2][3] - myWorld.list[num_aliens+1]->MC.mat[2][3],2));
	    	GLfloat alienDefenderDistance2 = sqrt(pow(myWorld.list[k]->MC.mat[0][3] - myWorld.list[num_aliens+2]->MC.mat[0][3],2) + pow(myWorld.list[k]->MC.mat[1][3] - myWorld.list[num_aliens+2]->MC.mat[1][3],2) + pow(myWorld.list[k]->MC.mat[2][3] - myWorld.list[num_aliens+2]->MC.mat[2][3],2));
	    	GLfloat alienDefenderDistance3 = sqrt(pow(myWorld.list[k]->MC.mat[0][3] - myWorld.list[num_aliens+3]->MC.mat[0][3],2) + pow(myWorld.list[k]->MC.mat[1][3] - myWorld.list[num_aliens+3]->MC.mat[1][3],2) + pow(myWorld.list[k]->MC.mat[2][3] - myWorld.list[num_aliens+3]->MC.mat[2][3],2));
	    	if ((alienDefenderDistance1 < (0.6)) || (myWorld.list[k]->MC.mat[1][3] <= myWorld.list[num_aliens+1]->MC.mat[1][3])
	    			|| (alienDefenderDistance2 < (0.6)) || (myWorld.list[k]->MC.mat[1][3] <= myWorld.list[num_aliens+2]->MC.mat[1][3])
					|| (alienDefenderDistance3 < (0.6)) || (myWorld.list[k]->MC.mat[1][3] <= myWorld.list[num_aliens+3]->MC.mat[1][3])){ // if alien touches hero, or alien sneaks past hero, end game.
	    		Camera setDefaultView;
	    		myCamera = setDefaultView;
	    		myCamera.setProjectionMatrix();
	    		gameOver = true;
	    		break;
	    	}
		}
	}
}
void loseScreen(){ // player lost the game, display background image and text saying player has lost

	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[3]); // background image
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2f(-6, -6);
	glTexCoord2f(0, 1); glVertex2f(-6, 6);
	glTexCoord2f(1, 1); glVertex2f(6, 6);
	glTexCoord2f(1, 0); glVertex2f(6, -6);
	glEnd();

	glDisable(GL_TEXTURE_2D);
	char loser[50] = "YOU LOSE!!!! PLAY AGAIN?";
	  glRasterPos2f(-1.5, 0);
	  int length, j;
	  length = (int)strlen(loser);
	  for (j = 0; j < length; j++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, loser[j]);
	  }
}
void winScreen(){// player won the game, display background image and text saying player has won
	Camera setDefaultView;
	myCamera = setDefaultView;
	myCamera.setProjectionMatrix();
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, texture[3]);//background image
	glBegin(GL_QUADS);
	glTexCoord2f(0, 0); glVertex2f(-6, -6);
	glTexCoord2f(0, 1); glVertex2f(-6, 6);
	glTexCoord2f(1, 1); glVertex2f(6, 6);
	glTexCoord2f(1, 0); glVertex2f(6, -6);
	glEnd();

	glDisable(GL_TEXTURE_2D);

	char winner[100] = "YOU WON!!!! PLAY AGAIN?";
	  glRasterPos2f(-1.5, 0);
	  int len, i;
	  len = (int)strlen(winner);
	  for (i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, winner[i]);
	  }
}
void printScore(){
	char scoreboard[32];												// dynamically print score
	sprintf(scoreboard, "%d", score);
	char other_string[64] = "SCORE: ";
	strcat(other_string, scoreboard);
	  glRasterPos2f(2, 2);
	  int len, i;
	  len = (int)strlen(other_string);
	  for (i = 0; i < len; i++) {
		glutBitmapCharacter(GLUT_BITMAP_TIMES_ROMAN_24, other_string[i]);
	  }
}
void alienMovementAlgorithm(){
	if (myWorld.list[1]->getMC().mat[0][3] > 0){    // alien move algorithm, using primitive "mutex locks" (ie boolean value to make sure aliens shift only when its time to shift.)
		shift = true;
		for (int k = 1; k <=num_aliens; k++){
			myWorld.list[k]->translate(0, -0.2, 0);
		}
	}
	if (myWorld.list[1]->getMC().mat[0][3] < -2){
		shift = false;
		for (int k = 1; k <=num_aliens; k++){
			myWorld.list[k]->translate(0, -0.2, 0);
		}
	}
	if(shift == false){

		for (int k = 1; k <=num_aliens; k++){
			myWorld.list[k]->translate(0.03, 0, 0);
		}
	}
	if(shift == true){

		for (int k = 1; k <=num_aliens; k++){
			myWorld.list[k]->translate(-0.03, 0, 0);
		}
	}
}
void bulletMovement(){
	for (unsigned k = 0; k< bulletList.size(); k++) {           // player bullets movement up
			std::list<Sphere>::iterator it = bulletList.begin();
			std::advance(it, k);
			Sphere temp = *it;
			temp.translate(0,0.02,0);
			glDisable(GL_TEXTURE_2D);

			temp.draw();
			*it = temp;
		}
		for (unsigned k = 0; k< alienBulletList.size(); k++) {		// alien bullets movement up
			std::list<Sphere>::iterator it = alienBulletList.begin();
			std::advance(it, k);
			Sphere temp = *it;
			temp.translate(0,-0.02,0);
			glDisable(GL_TEXTURE_2D);

			temp.draw();
			*it = temp;
		}
}
void playing(){ // player has not died yet, game in play.
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	myCamera.setProjectionMatrix();

	alienMovementAlgorithm();

	bulletMovement();
	for (unsigned p = 0; p< bulletList.size(); p++) {		// check if player bullets hit alien
		std::list<Sphere>::iterator it = bulletList.begin();
		std::advance(it, p);
		Sphere temp = *it;
		for (int k = 1; k <=num_aliens; k++){				// iterate through all aliens
			if(!myWorld.list[k]->mark_delete){
				GLfloat distance = sqrt(pow(myWorld.list[k]->MC.mat[0][3] - temp.MC.mat[0][3],2) + pow(myWorld.list[k]->MC.mat[1][3] - temp.MC.mat[1][3],2) + pow(myWorld.list[k]->MC.mat[2][3] - temp.MC.mat[2][3],2));
				if (distance < (temp.radius + 0.2)){
					deleteBullet(p);
					myWorld.list[k]->mark_delete=true;			// if player bullet hit alien (using collision algorithm), kill alien, destroy bullet
					score++;
					p--;
				}
				if(temp.MC.mat[1][3] > 3){				// destroy bullet if it goes off screen
					deleteBullet(p);
					p--;
				}
				GLfloat defenderDistance1 = sqrt(pow(myWorld.list[num_aliens+1]->MC.mat[0][3] - temp.MC.mat[0][3],2) + pow(myWorld.list[num_aliens+1]->MC.mat[1][3] - temp.MC.mat[1][3],2) + pow(myWorld.list[num_aliens+1]->MC.mat[2][3] - temp.MC.mat[2][3],2));
				GLfloat defenderDistance2 = sqrt(pow(myWorld.list[num_aliens+2]->MC.mat[0][3] - temp.MC.mat[0][3],2) + pow(myWorld.list[num_aliens+2]->MC.mat[1][3] - temp.MC.mat[1][3],2) + pow(myWorld.list[num_aliens+2]->MC.mat[2][3] - temp.MC.mat[2][3],2));
				GLfloat defenderDistance3 = sqrt(pow(myWorld.list[num_aliens+3]->MC.mat[0][3] - temp.MC.mat[0][3],2) + pow(myWorld.list[num_aliens+3]->MC.mat[1][3] - temp.MC.mat[1][3],2) + pow(myWorld.list[num_aliens+3]->MC.mat[2][3] - temp.MC.mat[2][3],2));
				if(defenderDistance1 < (temp.radius + 0.35)
						|| defenderDistance2 < (temp.radius + 0.35)
						|| defenderDistance3 < (temp.radius + 0.35)){		// if bullet hits defender, delete bullet
					deleteBullet(p);
					p--;
				}
			}
		}
		for (unsigned h = 0; h< alienBulletList.size(); h++) {			// check if alien bullet hits hero/player's bullet
			std::list<Sphere>::iterator it2 = alienBulletList.begin();
			std::advance(it2, h);
			Sphere temp2 = *it2;
			GLfloat distance = sqrt(pow(temp2.MC.mat[0][3] - temp.MC.mat[0][3],2) + pow(temp2.MC.mat[1][3] - temp.MC.mat[1][3],2) + pow(temp2.MC.mat[2][3] - temp.MC.mat[2][3],2));
			if(distance < (temp.radius)){
				deleteBullet(p);
				p--;
				alienDeleteBullet(h);
				h--;
			}
		}

	}

	for (unsigned p = 0; p< alienBulletList.size(); p++) {                // check if alien bullet hits hero
		std::list<Sphere>::iterator it = alienBulletList.begin();
		std::advance(it, p);
		Sphere temp = *it;


		GLfloat distance = sqrt(pow(myWorld.list[0]->MC.mat[0][3] - temp.MC.mat[0][3],2) + pow(myWorld.list[0]->MC.mat[1][3] - temp.MC.mat[1][3],2) + pow(myWorld.list[0]->MC.mat[2][3] - temp.MC.mat[2][3],2));
		if (distance < (temp.radius + 0.2)){ 			// alien bullet hits player game over
			Camera setDefaultView;
			myCamera = setDefaultView;
			myCamera.setProjectionMatrix();
			gameOver=true;
			break;
		}
		if(temp.MC.mat[1][3] < -3){						// alien bullet goes off screen, delete itself
			alienDeleteBullet(p);
			p--;
		}
		GLfloat defenderDistance1 = sqrt(pow(myWorld.list[num_aliens+1]->MC.mat[0][3] - temp.MC.mat[0][3],2) + pow(myWorld.list[num_aliens+1]->MC.mat[1][3] - temp.MC.mat[1][3],2) + pow(myWorld.list[num_aliens+1]->MC.mat[2][3] - temp.MC.mat[2][3],2));
		GLfloat defenderDistance2 = sqrt(pow(myWorld.list[num_aliens+2]->MC.mat[0][3] - temp.MC.mat[0][3],2) + pow(myWorld.list[num_aliens+2]->MC.mat[1][3] - temp.MC.mat[1][3],2) + pow(myWorld.list[num_aliens+2]->MC.mat[2][3] - temp.MC.mat[2][3],2));
		GLfloat defenderDistance3 = sqrt(pow(myWorld.list[num_aliens+3]->MC.mat[0][3] - temp.MC.mat[0][3],2) + pow(myWorld.list[num_aliens+3]->MC.mat[1][3] - temp.MC.mat[1][3],2) + pow(myWorld.list[num_aliens+3]->MC.mat[2][3] - temp.MC.mat[2][3],2));
		if(defenderDistance1 < (temp.radius + 0.35)
				|| defenderDistance2 < (temp.radius + 0.35)
				|| defenderDistance3 < (temp.radius + 0.35)){		// check if alien bullet hits defender, delete alien bullet if it does
			alienDeleteBullet(p);
			p--;
		}


	}
	printScore();
	myWorld.draw_world(); // draw all objects in the world
}
void display(void) {
	glDisable(GL_TEXTURE_2D);
	if(!gameOver){
		checkGameOver();
	}
	if(gameOver){
		loseScreen();
		}
	else if(score==num_aliens){
		winScreen();
	}
	else{
		playing();
	}
	glColorMaterial ( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
	glEnable ( GL_COLOR_MATERIAL );
	glFlush();
	glutSwapBuffers();
}

void createBullet(){
	if(bulletList.size() < 5){ // limit number of bullets on screen to 5 for added challenge
		Sphere bullet;
		bullet.scale_change(0.1);
		bullet.translate(myWorld.list[0]->getMC().mat[0][3], myWorld.list[0]->getMC().mat[1][3], 0);
		bulletList.push_back(bullet);
	}
}



void winReshapeFcn(GLint newWidth, GLint newHeight) {
	glViewport(0, 0, newWidth, newHeight);
	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	winWidth = newWidth;
	winHeight = newHeight;
}

void mouseAction(int button, int state, int x, int y) {
	if (button == GLUT_LEFT_BUTTON && state == GLUT_DOWN) {
		moving = 1;
		xBegin = x;
	}
	if (button == GLUT_LEFT_BUTTON && state == GLUT_UP) {
		moving = 0;
	}

	glutPostRedisplay();
}
void SpecialInput(int key, GLint x, GLint y) // hero movement algorithm
{
	switch(key)
	{
	case GLUT_KEY_UP:
		createBullet();
	break;
	case GLUT_KEY_LEFT:
		if(maxMove >= -2.5){
			maxMove=maxMove - 0.1;
			myWorld.list[0]->translate(-0.1, 0, 0);
		}
	break;
	case GLUT_KEY_RIGHT:
		if(maxMove <= 2.5){
			maxMove= maxMove+ 0.1;
			myWorld.list[0]->translate(0.1, 0, 0);
		}
	break;
}
glutPostRedisplay();
}
void mouseMotion(GLint x, GLint y) {
	GLfloat rx, ry, rz, theta;
	if (moving) {

		theta = (xBegin - x < 0) ? 1 : -1;
		rx=0;
		ry=1;
		rz=0;
		myCamera.rotate(rx,ry,rz, theta);

		glutPostRedisplay();
	}

}
void init(void) {
	if(!resetGame){
		bulletColorTimer(1);
	}

	
	//hero initial position
	myWorld.list[0]->scale_change(-0.8);
	myWorld.list[0]->translate(0, -2.6, 0);

	//barracks initial position
	myWorld.list[num_aliens+1]->scale_change(-0.6);
	myWorld.list[num_aliens+1]->translate(-1.5, -1.7, 0);

	myWorld.list[num_aliens+2]->scale_change(-0.6);
	myWorld.list[num_aliens+2]->translate(1.5, -1.7, 0);

	myWorld.list[num_aliens+3]->scale_change(-0.6);
	myWorld.list[num_aliens+3]->translate(0, -1.7, 0);

	myWorld.list[num_aliens+4]->scale_change(-10); // background image, turn cube inside out, give it texture, camera inside cube

	// alien initial positions
	for (int k = 1; k <=10; k++){
		myWorld.list[k]->scale_change(-0.8);
	}
	GLfloat posn = -2;
	for (int k = 1; k <=5; k++){
		myWorld.list[k]->translate(posn, 2.5, 0);
		posn=posn+0.5;
	}
	// second row
	posn = -2;
	for (int k = 6; k <=10; k++){
		myWorld.list[k]->translate(posn, 2, 0);
		posn=posn+0.5;
	}
	glewInit();

	glClearColor(0.0, 0.0, 0.0, 1.0);  // Set display-window color to black
	glColor3f(red, green, blue);


	glEnable(GL_LINE_SMOOTH);
	glHint(GL_LINE_SMOOTH_HINT, GL_NICEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glShadeModel(GL_SMOOTH);


	myCamera.setProjectionMatrix();

	glutPostRedisplay();


	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// Create texture
	glGenTextures(5, texture);



	// second texture configure and loading
	glBindTexture(GL_TEXTURE_2D, texture[0]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	Image* image1 = loadTexture("hero_converted.bmp");
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image1->sizeX, image1->sizeY, 0, GL_RGB,
			GL_UNSIGNED_BYTE, image1->data);

	glBindTexture(GL_TEXTURE_2D, texture[1]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	Image* image2 = loadTexture("invader_converted.bmp");
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image2->sizeX, image2->sizeY, 0, GL_RGB,
			GL_UNSIGNED_BYTE, image2->data);

	glBindTexture(GL_TEXTURE_2D, texture[2]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	Image* image3 = loadTexture("defenderShips.bmp");
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image3->sizeX, image3->sizeY, 0, GL_RGB,
			GL_UNSIGNED_BYTE, image3->data);

	glBindTexture(GL_TEXTURE_2D, texture[3]);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL);

	Image* image4 = loadTexture("stars-018.bmp");
	glTexImage2D(GL_TEXTURE_2D, 0, 3, image4->sizeX, image4->sizeY, 0, GL_RGB,
			GL_UNSIGNED_BYTE, image4->data);


	//glEnable(GL_TEXTURE_2D);
	glDisable(GL_TEXTURE_2D);

	glShadeModel(GL_FLAT);

	textures=false;
	glUseProgram(0);
	glutIdleFunc(NULL);


	glDisable(GL_DEPTH_TEST);
	glDisable(GL_CULL_FACE);
	glDisable(GL_NORMALIZE);
	glDisable(GL_COLOR_MATERIAL);

	glDisable(GL_LIGHTING);
	glDisable(GL_LIGHT0);
	glFlush(); glutPostRedisplay();
	 if(!resetGame){
		 alienBulletTimer(1);
	 }
}

void reset(void) {
	glutIdleFunc(NULL);
	glDisable(GL_TEXTURE_2D);
	score=0;
	num_aliens=10;
	gameOver = false;
	resetGame = true;
	shift = false;
	hero = false;
	defender = false;
	bulletColor = false;
	moving = 0;
	xBegin = 0;
	ycamera =0;
	maxMove = 0;
	glDisable(GL_CULL_FACE);
	glDisable(GL_DEPTH_TEST);
	glDisable(GL_NORMALIZE);
	glDisable(GL_COLOR_MATERIAL);
	glDisable( GL_LIGHTING );
	glDisable(GL_LIGHT0);
	myWorld.reset();
	Camera newCamera;
	myCamera=newCamera;
	init();
	alienBulletList.clear();
	bulletList.clear();
	glCullFace(GL_BACK);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glDisable(GL_BLEND);


	glFlush();
	glutPostRedisplay();
}

void timer(int value)
{
    display();
    glutTimerFunc(20, timer, 0);

}
void musicTimer(int value){
	PlaySound((LPCSTR) "one_piece_music.wav", NULL, SND_FILENAME | SND_ASYNC);
	glutTimerFunc(63000, musicTimer, 0);
}
void mainMenu(GLint option) {
	switch(option) {
	case 1:
		reset();
		break;
	case 2:
		exit(1);
		break;
	}
	glutPostRedisplay();
}
void menu() {
	glutCreateMenu(mainMenu);      // Create main pop-up menu.
	glutAddMenuEntry(" Restart Game ", 1);
	glutAddMenuEntry(" Quit", 2);
}

int main(int argc, char** argv) {

	PlaySound((LPCSTR) "one_piece_music.wav", NULL, SND_FILENAME | SND_ASYNC);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitWindowPosition(100, 100);


	glutInitWindowSize(winWidth, winHeight);
	glutCreateWindow("3D Space Invaders - By Ali Ahmed and Qamar Farooq");
	init();
	menu();


	glutDisplayFunc(display);
	glutMotionFunc(mouseMotion);
	glutSpecialFunc(SpecialInput);
	glutMouseFunc(mouseAction);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutTimerFunc(20, timer, 0);
	glutTimerFunc(63000, musicTimer, 0);
	glutSetCursor(GLUT_CURSOR_DESTROY);
	glutAttachMenu(GLUT_RIGHT_BUTTON);
	glutMainLoop();


	return 0;
}

