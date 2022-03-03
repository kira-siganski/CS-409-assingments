//
//  main4.cpp
//

#include <cassert>
#include <cmath>
#include <ctime>
#include <string>
#include <iostream>

#include "GetGlut.h"
#include "Sleep.h"
#include "ObjLibrary/ObjModel.h"
#include "ObjLibrary/DisplayList.h"
#include "ObjLibrary/Vector3.h"

using namespace std;
using namespace ObjLibrary;

const unsigned int KEY_UP_ARROW = 256;
const unsigned int KEY_DOWN_ARROW = 257;
const unsigned int KEY_LEFT_ARROW = 258;
const unsigned int KEY_RIGHT_ARROW = 259;
const unsigned int KEY_END = 260;
const unsigned int KEY_COUNT = 261;
bool key_pressed[KEY_COUNT];

void init ();
void initAsteroids();
void initDisplay ();
void keyboard(unsigned char key, int x, int y);
void keyboardUp(unsigned char key, int x, int y);
void special(int special_key, int x, int y);
void specialUp(int special_key, int x, int y);
void update ();
void reshape (int w, int h);
void display ();
void displayNoise();
void displayNoiseToConsole(float noise_at);
//borrowed from tut4
void drawAxes();

//tutorial 11 functions - altered to be 3D
float perlinNoise3D(float x, float y, float z);
unsigned int pseudorandom3D(int x, int y, int z);
float interpolate(float v0, float v1, float fraction);
float fade(float n);
Vector3 lattice(int x, int y, int z);

//tut 11 vars
const float GRID_SIZE = 6.0f;
//tut 11 seeds
const unsigned int SEED_X1 = 1273472206;
const unsigned int SEED_X2 = 4278162623;
const unsigned int SEED_Y1 = 1440014778;
const unsigned int SEED_Y2 = 524485263;
const unsigned int SEED_Z1 = 2813546167;
const unsigned int SEED_Z2 = 3305132234;
const unsigned int SEED_Q0 = 1498573726;
const unsigned int SEED_Q1 = 3476519523;
const unsigned int SEED_Q2 = 3905844518;


// add your global variables here
ObjModel skybox;
ObjModel asteroid;
ObjModel disk;

DisplayList asteroid_list;

//create camera class for easier use - based on tutorial 6
class CoordinateSystem {
public:
	//constructors
	//default
	CoordinateSystem() {
		forward.addComponents(0, 0, -1);
		up.addComponents(0, 1, 0);
		right = forward.crossProduct(up);
		position.addComponents(0, 0, 0);
	}
	//custom
	CoordinateSystem(Vector3& forward_coords, Vector3& up_coords, Vector3& position_coords) {
		forward = forward_coords;
		up = up_coords;
		right = forward.crossProduct(up);
		position = position_coords;
	}

	//return forward vector
	const Vector3& getForward() {
		return forward;
	}
	//return up vector
	const Vector3& getUp() {
		return up;
	}
	//return right vector
	const Vector3& getRight() {
		return right;
	}
	//return position vector
	const Vector3& getPosition() {
		return position;
	}

	void setUpCamera() {
		Vector3 look_at = position + forward;
		// set up the camera here
		gluLookAt(position.x, position.y, position.z,
			look_at.x, look_at.y, look_at.z,
			up.x, up.y, up.z);
	}

	void setPosition(const Vector3& position_new) {
		position = position_new;
	}

	void setOrientation(const Vector3& forward_new, const Vector3& up_new) {
		forward = forward_new;
		up = up_new;
		right = forward.crossProduct(up);
	}

	void moveForward(double distance) {
		position += forward * distance;
	}

	void moveUp(double distance) {
		position += up * distance;
	}

	void moveRight(double distance) {
		position += right * distance;
	}

	void rotateAroundForward(double distance) {
		up.rotateArbitrary(forward, distance);
		right.rotateArbitrary(forward, distance);
	}
	void rotateAroundUp(double distance) {
		forward.rotateArbitrary(up, distance);
		right.rotateArbitrary(up, distance);
	}
	void rotateAroundRight(double distance) {
		up.rotateArbitrary(right, distance);
		forward.rotateArbitrary(right, distance);
	}
	void rotateAroundArbitrary(Vector3 axis, double radians) {
		forward.rotateArbitrary(axis, radians);
		right.rotateArbitrary(axis, radians);
		up.rotateArbitrary(axis, radians);
	}

	void rotateToVector(const Vector3& desired_forward, double max_radians) {
		if (desired_forward.isZero()) {
			return;
		}
		Vector3 axis = forward.crossProduct(desired_forward);
		if (axis.isZero()) {
			axis = up;
		}
		else {
			axis.normalize();
		}

		double radians = forward.getAngleSafe(desired_forward);

		if (radians > max_radians) {
			radians = max_radians;
		}


		forward.rotateArbitrary(axis, radians);
		up.rotateArbitrary(axis, radians);
		right.rotateArbitrary(axis, radians);

	}

private:
	//vector variables
	Vector3 forward;
	Vector3 up;
	Vector3 right;
	Vector3 position;
};

//create meteor class
class Asteroid {
public:
	//constructors
	// default constructor
	Asteroid() {
		forward.addComponents(0, 0, -1);
		up.addComponents(0, 1, 0);
		right = forward.crossProduct(up);
		position.addComponents(0, 0, 0);
		//set size - 50 <= x <= 400
		Vector3 size(1.0, 1.0, 1.0);
		asteroid_size = size;
		//set rotation
		Vector3 spin(0.0, 0.0, 0.0);
		rotation_axis = spin;
		//set spin rate - set between -10.0 and 10.0
		spinRate = 0.0;

		Vector3 start_position(0.0, 0.0, 0.0);
		position = start_position;

		draw = true;
	}

	//return forward vector
	const Vector3& getForward() {
		return forward;
	}
	//return up vector
	const Vector3& getUp() {
		return up;
	}
	//return right vector
	const Vector3& getRight() {
		return right;
	}
	//return position vector
	const Vector3& getPosition() {
		return position;
	}

	void setPosition(const Vector3& position_new) {
		position = position_new;
	}

	void setOrientation(const Vector3& forward_new, const Vector3& up_new) {
		forward = forward_new;
		up = up_new;
		right = forward.crossProduct(up);
	}

	void moveForward(double distance) {
		position += forward * distance;
	}

	void moveUp(double distance) {
		position += up * distance;
	}

	void moveRight(double distance) {
		position += right * distance;
	}

	void rotateAroundForward(double distance) {
		up.rotateArbitrary(forward, distance);
		right.rotateArbitrary(forward, distance);
	}
	void rotateAroundUp(double distance) {
		forward.rotateArbitrary(up, distance);
		right.rotateArbitrary(up, distance);
	}
	void rotateAroundRight(double distance) {
		up.rotateArbitrary(right, distance);
		forward.rotateArbitrary(right, distance);
	}
	void rotateAroundArbitrary() {
		forward.rotateArbitrary(rotation_axis, spinRate);
		right.rotateArbitrary(rotation_axis, spinRate);
		up.rotateArbitrary(rotation_axis, spinRate);
	}

	void applyDrawTransformations() const {

		//code for translation
		glTranslated(position.x, position.y, position.z);
		//rotate the asteroids
		double matrix[16] =
		{ forward.x, forward.y, forward.z, 0.0,
		   up.x,      up.y,      up.z,      0.0,
		   right.x,   right.y,   right.z,   0.0,
		   0.0,       0.0,       0.0,       1.0, };

		glMultMatrixd(matrix);
		//scale the asteroids
		glScaled(asteroid_size.x, asteroid_size.y, asteroid_size.z);

		if (draw == true) {
				glBegin(GL_LINES);
				glColor3d(1.0, 0.0, 0.0);
				glVertex3d(0.0, 0.0, 0.0);
				glVertex3d(2.0, 0.0, 0.0);
				glColor3d(0.0, 1.0, 0.0);
				glVertex3d(0.0, 0.0, 0.0);
				glVertex3d(0.0, 2.0, 0.0);
				glColor3d(0.0, 0.0, 1.0);
				glVertex3d(0.0, 0.0, 0.0);
				glVertex3d(0.0, 0.0, 2.0);
				glEnd();
		}
	}
	//change size
	void setSize(Vector3& new_size) {
		asteroid_size = new_size;
	}
	//return size
	const Vector3& getSize() {
		return asteroid_size;
	}

	//change rotation
	void setRotation(Vector3& rotation) {
	rotation_axis = rotation;
	}
	//get rotation
	const Vector3& getRotation() {
		return rotation_axis;
	}
	//change spin rate
	void setSpinRate(double spin_rate) {
		spinRate = spin_rate;
	}
	//get spin rate
	double getSpinRate() {
		return spinRate;
	}

	void setDraw() {
		draw = true;
	}

	void unsetDraw() {
		draw = false;
	}

	bool checkDraw() {
		return draw;
	}

private:
	//size of asteroid
	Vector3 asteroid_size;
	//asteroid rotation
	Vector3 rotation_axis;
	//speed of spin
	double spinRate;

	Vector3 forward;

	Vector3 up;

	Vector3 right;

	Vector3 position;

	bool draw;
};

//create camera object
CoordinateSystem camera;

Asteroid asteroids[100];
double move_speed = 1.0;
double turn_speed = 0.1;

Asteroid test;

int main (int argc, char* argv[])
{
	glutInitWindowSize(1024, 768);
	glutInitWindowPosition(0, 0);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutCreateWindow("Loading OBJ Models");
	glutKeyboardFunc(keyboard);
	glutKeyboardUpFunc(keyboardUp);
	glutSpecialFunc(special);
	glutSpecialUpFunc(specialUp);
	glutIdleFunc(update);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);

	initAsteroids();
	displayNoise();
	init();

	glutMainLoop();  // contains an infinite loop, so it never returns
	return 1;
}

void init ()
{
	initDisplay();
	//initialize models
	skybox.load("Models/Skybox.obj");
	disk.load("Models/Disk.obj");

	asteroid.load("Models/AsteroidA.obj");
	asteroid_list = asteroid.getDisplayList();

	//initialize camera positions
	//set camera position
	Vector3 startposition(0.0, 100.0, 1000.0);
	camera.setPosition(startposition);
}

void initAsteroids() {
	srand((unsigned)time(NULL));
	for (int i = 0; i < 100; i++) {
		//set size
		Vector3 size = Vector3::getRandomInRangeInclusive(
			Vector3(25.0, 25.0, 25.0),
			Vector3(300.0, 300.0, 300.0));
		asteroids[i].setSize(size);

		//set rotaion axis
		Vector3 spin = Vector3::getRandomUnitVector();
		asteroids[i].setRotation(spin);

		//set spin rate - set between 0 and 9 then divided by 10 to be between 1.0 and 0.0
		asteroids[i].setSpinRate(fmod(rand(), 10)/100);

		//set position
		Vector3 position = Vector3::getRandomInRangeInclusive(
			Vector3(-4000.0, -4000.0, -4000.0),
			Vector3(4000.0, 4000.0, 4000.0));

		//we need to make sure no part of the position is near the black hole itself
		if (position.x >= -50.0 && position.x <= 50.0) {
			if (position.x <= 0) {
				position.x -= 50.0;
			}
			else {
				position.x += 50.0;
			}
		}
		if (position.y >= -50.0 && position.y <= 50.0) {
			if (position.y <= 0) {
				position.y -= 50.0;
			}
			else {
				position.y += 50.0;
			}
		}
		if (position.z >= -50.0 && position.z <= 50.0) {
			if (position.z <= 0) {
				position.z -= 50.0;
			}
			else {
				position.z += 50.0;
			}
		}
		asteroids[i].setPosition(position);
	}
}

void initDisplay ()
{
	glClearColor(0.5, 0.5, 0.5, 0.0);
	glColor3f(0.0, 0.0, 0.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);

	glutPostRedisplay();
}

void keyboard (unsigned char key, int x, int y)
{


	switch (key)
	{
	case 27: // on [ESC]
		exit(0); // normal exit
		break;
	}
	//switched around so lowercase is prefered. Holding down shift all the time isn't fun
	if (key >= 'A' && key <= 'Z') {
		key = key + 'a' - 'A';
	}
	else if (key == ':') {
		key = ';';
	}
	else if (key == '?') {
		key = '/';
	}
	else if (key == '<') {
		key = ',';
	}
	else if (key == '>') {
		key = '.';
	}

	key_pressed[key] = true;
}

void keyboardUp(unsigned char key, int x, int y) {
	if (key >= 'A' && key <= 'Z') {
		key = key + 'a' - 'A';
	}
	else if (key == ':') {
		key = ';';
	}
	else if (key == '?') {
		key = '/';
	}
	else if (key == '<') {
		key = ',';
	}
	else if (key == '>') {
		key = '.';
	}

	key_pressed[key] = false;
}

void special(int special_key, int x, int y) {
	switch (special_key) {
	case GLUT_KEY_RIGHT: //turn camera right
		key_pressed[KEY_RIGHT_ARROW] = true;
		break;
	case GLUT_KEY_LEFT://turn camera left
		key_pressed[KEY_LEFT_ARROW] = true;
		break;
	case GLUT_KEY_UP://turn camera up
		key_pressed[KEY_UP_ARROW] = true;
		break;
	case GLUT_KEY_DOWN: //turn camera down
		key_pressed[KEY_DOWN_ARROW] = true;
		break;
	}
}

void specialUp(int special_key, int x, int y) {
	switch (special_key) {
	case GLUT_KEY_RIGHT: //turn camera right
		key_pressed[KEY_RIGHT_ARROW] = false;
		break;
	case GLUT_KEY_LEFT://turn camera left
		key_pressed[KEY_LEFT_ARROW] = false;
		break;
	case GLUT_KEY_UP://turn camera up
		key_pressed[KEY_UP_ARROW] = false;
		break;
	case GLUT_KEY_DOWN: //turn camera down
		key_pressed[KEY_DOWN_ARROW] = false;
		break;
	}
}

void update ()
{
	// update your variables here
	if (key_pressed[KEY_RIGHT_ARROW]) {
		camera.rotateAroundUp(-turn_speed);
	}
	if (key_pressed[KEY_LEFT_ARROW]) {
		camera.rotateAroundUp(turn_speed);
	}
	if (key_pressed[KEY_UP_ARROW]) {
		camera.rotateAroundRight(turn_speed);
	}
	if (key_pressed[KEY_DOWN_ARROW]) {
		camera.rotateAroundRight(-turn_speed);
	}

	if (key_pressed[' ']) { //move forward fast
		camera.setPosition(camera.getPosition() + camera.getForward() * 100);
	}
	if (key_pressed[';']) { //move forward
		camera.setPosition(camera.getPosition() + camera.getForward() * move_speed);
	}
	if (key_pressed['/']) {//move backward
		camera.setPosition(camera.getPosition() - camera.getForward() * move_speed);
	}
	if (key_pressed['a']) {//strafe left
		camera.setPosition(camera.getPosition() - camera.getRight() * move_speed);
	}
	if (key_pressed['d']) {//strafe right
		camera.setPosition(camera.getPosition() + camera.getRight() * move_speed);
	}
	if (key_pressed['w']) {//strafe up
		camera.setPosition(camera.getPosition() + camera.getUp() * move_speed);
	}
	if (key_pressed['s']) {//strafe down
		camera.setPosition(camera.getPosition() - camera.getUp() * move_speed);
	}
	if (key_pressed[',']) {//roll left
		camera.rotateAroundForward(-turn_speed);
	}
	if (key_pressed['.']) {//roll right
		camera.rotateAroundForward(turn_speed);
	}
	if (key_pressed['h']) {//rotate to face origin
		Vector3 origin = Vector3::ZERO;
		Vector3 direction_to_origin = origin - camera.getPosition();
		camera.rotateToVector(direction_to_origin, turn_speed);
	}
	if (key_pressed['t'] && asteroids[0].checkDraw() == false) {
		//added in check draw so it doesn't try to iterate more than once if the key is held down
		for (int i = 0; i < 100; i++) {
			asteroids[i].setDraw();
		}
	} 
	else if (!key_pressed['t'] && asteroids[0].checkDraw() == true) {
		for (int i = 0; i < 100; i++) {
			asteroids[i].unsetDraw();
		}
	}

	for (int i = 0; i < 100; i++) {
		asteroids[i].rotateAroundArbitrary();
	}
	sleep(1.0 / 60.0);
	glutPostRedisplay();
}

void reshape (int w, int h)
{
	glViewport (0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60.0, (GLdouble)w / (GLdouble)h, 1.0, 100000.0);
	glMatrixMode(GL_MODELVIEW);

	glutPostRedisplay();
}

void display ()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// clear the screen - any drawing before here will not display at all

	glLoadIdentity();
	camera.setUpCamera();
	
	// camera is now set up - any drawing before here will display incorrectly
	//draw skybox
	glPushMatrix();
		glTranslated(camera.getPosition().x, camera.getPosition().y, camera.getPosition().z);
		glDepthMask(GL_FALSE);
		skybox.draw();
		glDepthMask(GL_TRUE);
	glPopMatrix();

	//draw black hole
	glPushMatrix();
		glTranslated(0.0, 0.0, 0.0);
		glColor3d(0.0, 0.0, 0.0);
		glutSolidSphere(50, 15, 15);
	glPopMatrix();

	//draw asteroids
	for (int i = 0; i < 100; i++) {
		glPushMatrix();
			asteroids[i].applyDrawTransformations();
			asteroid_list.draw();
		glPopMatrix();
	}


	//draw accretion disk
	glPushMatrix();
		glScaled(5000.0, 0.0, 5000.0);
		disk.draw();
	glPopMatrix();

	// send the current image to the screen - any drawing after here will not display
	glutSwapBuffers();
}

void displayNoise() {
	//we want to display symbols in a 5 x 3 grid to start
	//our values are from 0 - 4 for x and 0 - 2 y
	//that test worked, went to larger values and noticed we only hit max and min when starting with -numbers
	cout << "Perlin noise: XY plane, z = 0" << endl << endl;
	for (int y = 0; y < 30; y++) {
		for (int x = 0; x < 40; x++) {
			// change to Perlin noise here
			float noise_at = perlinNoise3D(x, y, 0);
			displayNoiseToConsole(noise_at);
		}
		cout << endl << endl;
	}

	cout << "Perlin noise: XZ plane, y = 0" << endl << endl;
	for (int x = 0; x < 30; x++) {
		for (int z = 0; z < 40; z++) {
			// change to Perlin noise here
			float noise_at = perlinNoise3D(x, 0, z);
			displayNoiseToConsole(noise_at);
		}
		cout << endl << endl;
	}

	cout << "Perlin noise: YZ plane, x = 0" << endl << endl;
	for (int z = 0; z < 30; z++) {
		for (int y = 0; y < 40; y++) {
			// change to Perlin noise here
			float noise_at = perlinNoise3D(0, y, z);
			displayNoiseToConsole(noise_at);
		}
		cout << endl;
	}
}

void displayNoiseToConsole(float noise_at) {
	//symbols are ' ', '.', '-', '=', 'o', 'O', 'H', 'M', '@'
	// lowest = ' ', mid = 'o', highest = '@'
	if (noise_at <= -1.0) { //min
		cout << ' ';
	}
	else if (noise_at < -0.75) {
		cout << '.';
	}
	else if (noise_at < -0.5) {
		cout << '-';
	}
	else if (noise_at < -0.25) {
		cout << '=';
	}
	else if (noise_at <= 0.0) { // midpoint
		cout << 'o';
	}
	else if (noise_at < 0.25) {
		cout << 'O';
	}
	else if (noise_at < 0.5) {
		cout << 'H';
	}
	else if (noise_at < 0.75) {
		cout << 'M';
	}
	else {//max
		cout << '@';
	}
}

float perlinNoise3D(float x, float y, float z)
{
	// calculate noise here
	int x0 = (int)(float(x / GRID_SIZE));
	int y0 = (int)(float(y / GRID_SIZE));
	int z0 = (int)(float(z / GRID_SIZE));

	float x_frac = x / GRID_SIZE - x0;
	float y_frac = y / GRID_SIZE - y0;
	float z_frac = z / GRID_SIZE - z0;

	float x_fade = fade(x_frac);
	float y_fade = fade(y_frac);
	float z_fade = fade(z_frac);

	int x1 = x0 + 1;
	int y1 = y0 + 1;
	int z1 = z0 + 1;

	Vector3 lattice000 = lattice(x0, y0, z0);
	Vector3 lattice001 = lattice(x0, y0, z1);
	Vector3 lattice010 = lattice(x0, y1, z0);
	Vector3 lattice011 = lattice(x0, y1, z1);
	Vector3 lattice100 = lattice(x1, y0, z0);
	Vector3 lattice101 = lattice(x1, y0, z1);
	Vector3 lattice110 = lattice(x1, y1, z0);
	Vector3 lattice111 = lattice(x1, y1, z1);

	Vector3 direction000(-x_frac, -y_frac, -z_frac);
	Vector3 direction001(-x_frac, -y_frac, 1.0f - z_frac);
	Vector3 direction010(-x_frac, 1.0f - y_frac, -z_frac);
	Vector3 direction011(-x_frac, 1.0f - y_frac, 1.0f - z_frac);
	Vector3 direction100(1.0f - x_frac, -y_frac, -z_frac);
	Vector3 direction101(1.0f - x_frac, -y_frac, 1.0f - z_frac);
	Vector3 direction110(1.0f - x_frac, 1.0f - y_frac, -z_frac);
	Vector3 direction111(1.0f - x_frac, 1.0f - y_frac, 1.0f - z_frac);

	float value000 = (float)(lattice000.dotProduct(direction000));
	float value001 = (float)(lattice001.dotProduct(direction001));
	float value010 = (float)(lattice010.dotProduct(direction010));
	float value011 = (float)(lattice011.dotProduct(direction011));
	float value100 = (float)(lattice100.dotProduct(direction100));
	float value101 = (float)(lattice101.dotProduct(direction101));
	float value110 = (float)(lattice110.dotProduct(direction110));
	float value111 = (float)(lattice111.dotProduct(direction111));

	float value00 = interpolate(value000, value001, z_fade);
	float value01 = interpolate(value010, value011, z_fade);
	float value10 = interpolate(value100, value101, z_fade);
	float value11 = interpolate(value110, value111, z_fade);

	float value0 = interpolate(value00, value01, y_fade);
	float value1 = interpolate(value10, value11, y_fade);
	float value = interpolate(value0, value1, x_fade);

	return value;
}

unsigned int pseudorandom3D(int x, int y, int z) {
	unsigned int n = (SEED_X1 * x) + (SEED_Y1 * y) + (SEED_Z1 * z);
	unsigned int quad_term = SEED_Q2 * n * n + SEED_Q1 * n + SEED_Q0;
	return quad_term + (SEED_X2 * x) + (SEED_Y2 * y) + (SEED_Z2 * z);
}

float interpolate(float v0, float v1, float fraction)
{
	return (v0 * (1.0f - fraction)) + (v1 * fraction);
	//return v0 + fraction * (v1 - v0);
}

float fade(float n)
{
	return (1 - cos(n * 3.14159265f)) * 0.5f;
}

Vector3 lattice(int x, int y, int z)
{
	unsigned int value = pseudorandom3D(x , y, z);
	unsigned int value2 = pseudorandom3D(x + 1, y + 1, z + 1);
	float radians = (float)(sin(value));  // very random
	float radians2 = (float)(cos(value2));

	if (radians < 0.0) {
		radians = -radians;
	}
	if (radians2 < 0.0) {
		radians2 = -radians2;
	}
	//apparently this function needs a seed between 0.0 and 1.0
	return Vector3::getPseudorandomUnitVector(radians, radians2);
}

