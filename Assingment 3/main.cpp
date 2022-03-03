//
//  main.cpp 
//  Kira: using solution from assingment 2 as a base
//didn't have time for path part so oh well

#include <cassert>
#include <cctype>  // for toupper
#include <vector>
#include <algorithm>  // for min/max
//adding includes from tutorial 17
#include <cmath>
#include <string>
#include <sstream>
#include <chrono>

#include "GetGlut.h"
#include "Sleep.h"

#include "ObjLibrary/Vector3.h"
#include "ObjLibrary/ObjModel.h"
#include "ObjLibrary/DisplayList.h"
#include "ObjLibrary/SpriteFont.h"

#include "CoordinateSystem.h"
#include "PerlinNoiseField3.h"
#include "Entity.h"
#include "Asteroid.h"
#include "Spaceship.h"

using namespace std;
using namespace ObjLibrary;
using namespace chrono;

void initDisplay ();
void loadModels ();
void initAsteroids ();
void initCamera ();
void initShip();
void initFont();
void initFrames();

unsigned char fixShift (unsigned char key);
void keyboardDown (unsigned char key, int x, int y);
void keyboardUp (unsigned char key, int x, int y);
void specialDown (int special_key, int x, int y);
void specialUp (int special_key, int x, int y);

void update ();
void handleInput ();
void updateAsteroids ();
void updateShip();

void reshape (int w, int h);
void display ();
void drawSkybox ();
void drawAsteroids (bool is_show_debug);
void drawBlackHole ();
void drawSpaceShip();
void drawOverlays();

namespace
{
	const unsigned int KEY_PRESSED_COUNT = 0x100 + 5;
	const unsigned int KEY_PRESSED_RIGHT = 0x100 + 0;
	const unsigned int KEY_PRESSED_LEFT  = 0x100 + 1;
	const unsigned int KEY_PRESSED_UP    = 0x100 + 2;
	const unsigned int KEY_PRESSED_DOWN  = 0x100 + 3;
	const unsigned int KEY_PRESSED_END   = 0x100 + 4;
	bool key_pressed[KEY_PRESSED_COUNT];

	bool g_is_show_debug = false;

	const unsigned int ASTEROID_COUNT = 100;

	const double BLACK_HOLE_RADIUS  =    50.0;
	const double DISK_RADIUS        = 10000.0;

	DisplayList g_skybox_display_list;
	DisplayList g_disk_display_list;
	DisplayList g_spaceship_display_list;

	static const unsigned int ASTEROID_MODEL_COUNT = 25;
	ObjModel ga_asteroid_models[ASTEROID_MODEL_COUNT];

	vector<Asteroid> gv_asteroids;

	const double MOVE_FAST_RATE = 100.0;
	const double MANOEUVER_RATE = 1.0;
	const double TURN_RATE = 0.02;
	CoordinateSystem g_camera;



	double random01 ()
	{
		return rand() / (RAND_MAX + 1.0);
	}

	double random2 (double min_value, double max_value)
	{
		assert(min_value <= max_value);

		return min_value + random01() * (max_value - min_value);
	}

	//adding in stuff for A3
	int window_width  = 640;
	int window_height = 480;
	//tutorial 17 vars
	SpriteFont font;


	float smoothed_frames_per_second = 60;
	float smoothed_updates_per_second = 60;
	system_clock::time_point start_time;
	system_clock::time_point last_frame_time;
	system_clock::time_point last_update_time;
	system_clock::time_point next_update_time;

	const unsigned int MAXIMUM_UPDATES_PER_FRAME = 10;
	const int UPDATES_PER_SECOND = 60;
	const milliseconds FRAME_TIME_MIN(5);
	const microseconds DELTA_TIME(1000000 / UPDATES_PER_SECOND);

	Spaceship ship;
}  // end of anonymous namespace



int main (int argc, char* argv[])
{
	glutInitWindowSize(window_width, window_height);
	glutInitWindowPosition(0, 0);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutCreateWindow("CS 409 Assignment 2 Solution");
	glutKeyboardFunc(keyboardDown);
	glutKeyboardUpFunc(keyboardUp);
	glutSpecialFunc(specialDown);
	glutSpecialUpFunc(specialUp);
	glutIdleFunc(update);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);

	PerlinNoiseField3 pnf;
	pnf.printPerlin(40, 60, 0.1f);

	initDisplay();
	loadModels();
	initAsteroids();
	//initCamera();
	initShip();
	initFont();
	initFrames();

	glutMainLoop();

	return 1;
}

void initDisplay ()
{
	glClearColor(0.0, 0.0, 0.0, 0.0);
	glColor3f(0.0, 0.0, 0.0);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_CULL_FACE);
	//glEnable(GL_BLEND);
	//glBlendFunc(GL_SRC_ALPHA, GL_ONE);  // additive blending

	glutPostRedisplay();
}

void loadModels ()
{
	g_skybox_display_list = ObjModel("Models/Skybox.obj").getDisplayList();
	g_disk_display_list   = ObjModel("Models/Disk.obj")  .getDisplayList();

	assert(ASTEROID_MODEL_COUNT <= 26);  // only 26 letters to use
	for(unsigned m = 0; m < ASTEROID_MODEL_COUNT; m++)
	{
		string filename = "Models/AsteroidA.obj";
		assert(filename[15] == 'A');
		filename[15] = 'A' + m;
		ga_asteroid_models[m].load(filename);
	}

	//load space ship
	g_spaceship_display_list = ObjModel("Models/Sagittarius.obj").getDisplayList();
}

void initAsteroids ()
{
	//pressing end breaks this since it keeps making more asteroids since there's a limit on them 
	gv_asteroids.clear();
	static const double DISTANCE_MIN = DISK_RADIUS * 0.2;
	static const double DISTANCE_MAX = DISK_RADIUS * 0.8;

	static const double SPEED_FACTOR_MIN = 0.5;
	static const double SPEED_FACTOR_MAX = 1.5;

	static const double OUTER_RADIUS_MIN =  50.0;
	static const double OUTER_RADIUS_MAX = 400.0;
	static const double INNER_FRACTION_MIN = 0.1;
	static const double INNER_FRACTION_MAX = 0.5;

	for(unsigned a = 0; a < ASTEROID_COUNT; a++)
	{
		// choose a random position in a thick shell around the black hole
		double distance = random2(DISTANCE_MIN, DISTANCE_MAX);
		Vector3 position = Vector3::getRandomUnitVector() * distance;

		// mostly smaller asteroids
		double outer_radius = min(random2(OUTER_RADIUS_MIN, OUTER_RADIUS_MAX),
		                          random2(OUTER_RADIUS_MIN, OUTER_RADIUS_MAX));

		double inner_fraction = random2(INNER_FRACTION_MIN, INNER_FRACTION_MAX);
		double inner_radius   = outer_radius * inner_fraction;

		unsigned int model_index = a % ASTEROID_MODEL_COUNT;
		assert(model_index < ASTEROID_MODEL_COUNT);
		assert(!ga_asteroid_models[model_index].isEmpty());

		gv_asteroids.push_back(Asteroid(position,
		                                inner_radius, outer_radius,
		                                ga_asteroid_models[model_index]));
	}
	assert(gv_asteroids.size() == ASTEROID_COUNT);
}

void initCamera ()
{
	g_camera = CoordinateSystem(Vector3(0, 0, 0),  // position
	                            Vector3(1, 0, 0),  // forward vector
	                            Vector3(0, 1, 0)); // up vector
}

void initShip() {
	ship.setModel(g_spaceship_display_list);
	Vector3 start(150.0, 120.0, 150.0);
	Vector3 init_velocity(0.0, 0.0, 0.0);
	ship.setPosition(start);
	ship.setVelocity(init_velocity);
}

void initFont() {
	font.load("Font.bmp");
}

void initFrames() {
	start_time = system_clock::now();
	last_frame_time = start_time;
	last_update_time = start_time;
	next_update_time = start_time;
}

unsigned char fixShift (unsigned char key)
{
	switch(key)
	{
	case '<':  return ',';
	case '>':  return '.';
	case '?':  return '/';
	case ':':  return ';';
	case '"':  return '\'';
	default:
		return tolower(key);
	}
}

void keyboardDown (unsigned char key, int x, int y)
{
	key = fixShift(key);

	// mark key as pressed
	key_pressed[key] = true;

	switch (key)
	{
	case 27: // on [ESC]
		exit(0); // normal exit
		break;
	}
}

void keyboardUp (unsigned char key, int x, int y)
{
	key = fixShift(key);
	key_pressed[key] = false;
}

void specialDown (int special_key, int x, int y)
{
	switch(special_key)
	{
	case GLUT_KEY_RIGHT:
		key_pressed[KEY_PRESSED_RIGHT] = true;
		break;
	case GLUT_KEY_LEFT:
		key_pressed[KEY_PRESSED_LEFT] = true;
		break;
	case GLUT_KEY_UP:
		key_pressed[KEY_PRESSED_UP] = true;
		break;
	case GLUT_KEY_DOWN:
		key_pressed[KEY_PRESSED_DOWN] = true;
		break;
	case GLUT_KEY_END:
		key_pressed[KEY_PRESSED_END] = true;
		break;
	}
}

void specialUp (int special_key, int x, int y)
{
	switch(special_key)
	{
	case GLUT_KEY_RIGHT:
		key_pressed[KEY_PRESSED_RIGHT] = false;
		break;
	case GLUT_KEY_LEFT:
		key_pressed[KEY_PRESSED_LEFT] = false;
		break;
	case GLUT_KEY_UP:
		key_pressed[KEY_PRESSED_UP] = false;
		break;
	case GLUT_KEY_DOWN:
		key_pressed[KEY_PRESSED_DOWN] = false;
		break;
	case GLUT_KEY_END:
		key_pressed[KEY_PRESSED_END] = false;
		break;
	}
}



void update ()
{
	handleInput();
	updateAsteroids();
	updateShip();
	last_update_time = system_clock::now();
	if (key_pressed['g']) {
		next_update_time += DELTA_TIME/10;
	}
	else
		next_update_time += DELTA_TIME;

	//slow update rate when U is pressed
	if (key_pressed['u']) {
		sleep(1.0 / 10.0);
	}
	else {
		sleep(duration<double>(DELTA_TIME).count());
	}
	
	system_clock::time_point current_time = system_clock::now();

	//physics update
	for (unsigned int i = 0; i < MAXIMUM_UPDATES_PER_FRAME &&
		next_update_time < current_time; i++) {
		handleInput();
		updateAsteroids(); 
		updateShip();

		if (key_pressed['g']) {
			next_update_time += DELTA_TIME / 10;
		}
		else
			next_update_time += DELTA_TIME;

		current_time = system_clock::now();
		last_update_time = current_time;

		if (key_pressed['u']) {
			sleep(1.0 / 60.0);
		}
		
	}

	glutPostRedisplay();
}

void handleInput ()
{
	//
	//  Move camera
	//

	if(key_pressed[' '])
		ship.accelerateForward(MOVE_FAST_RATE);
	if(key_pressed[';'] || key_pressed['\''])  // either key
		ship.accelerateForward(MANOEUVER_RATE);
	if(key_pressed['/'])
		ship.accelerateForward(-MANOEUVER_RATE);
	if(key_pressed['w'] || key_pressed['e'])  // either key
		ship.accelerateUp(MANOEUVER_RATE); //move up
	if(key_pressed['s'])
		ship.accelerateUp(-MANOEUVER_RATE); //move down
	if(key_pressed['d'])
		ship.accelerateRight(MANOEUVER_RATE); //move right
	if(key_pressed['a'])
		ship.accelerateRight(-MANOEUVER_RATE); //move left

	//
	//  Rotate camera
	//

	if(key_pressed['.'])
		ship.rotateAroundForward(TURN_RATE);
	if(key_pressed[','])
		ship.rotateAroundForward(-TURN_RATE);
	if(key_pressed[KEY_PRESSED_UP])
		ship.rotateAroundRight(TURN_RATE);
	if(key_pressed[KEY_PRESSED_DOWN])
		ship.rotateAroundRight(-TURN_RATE);
	if(key_pressed[KEY_PRESSED_LEFT])
		ship.rotateAroundUp(TURN_RATE);
	if(key_pressed[KEY_PRESSED_RIGHT])
		ship.rotateAroundUp(-TURN_RATE);

	//
	//  Other
	//

	if(key_pressed['t'])
	{
		g_is_show_debug = !g_is_show_debug;
		key_pressed['t'] = false;  // only once per keypress
	}
	if(key_pressed[KEY_PRESSED_END])
	{
		initAsteroids();
		initShip();
		key_pressed[KEY_PRESSED_END] = false;  // only once per keypress
	}
}

//need to alter this to also use the entity update
void updateAsteroids ()
{
	for (unsigned a = 0; a < gv_asteroids.size(); a++) {
		gv_asteroids[a].update();
	}
		
}

void updateShip() {
	ship.update();
}

void reshape (int w, int h)
{
	window_width = w;
	window_height = h;

	glViewport (0, 0, w, h);

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLdouble)w / (GLdouble)h, 1.0, 100000.0);
	glMatrixMode(GL_MODELVIEW);

	glutPostRedisplay();
}

void display ()
{
	if (key_pressed['y']) {
		sleep(1.0 / 10.0);
	}
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// clear the screen - any drawing before here will not display

	glLoadIdentity();
	ship.setUpCamera();
	// camera is set up - any drawing before here will display incorrectly

	drawSkybox();  // has to be first
	drawAsteroids(g_is_show_debug);
	drawSpaceShip();
	drawBlackHole();

	drawOverlays();

	// send the current image to the screen - any drawing after here will not display
	glutSwapBuffers();
}

void drawSkybox ()
{
	glPushMatrix();
		glTranslated(ship.locateCamera().x,
					 ship.locateCamera().y,
					 ship.locateCamera().z);
		glRotated(90.0, 0.0, 0.0, 1.0);  // line band of clouds on skybox up with accretion disk

		glDepthMask(GL_FALSE);
		g_skybox_display_list.draw();
		glDepthMask(GL_TRUE);
	glPopMatrix();
}

void drawAsteroids (bool is_show_debug)
{
	for(unsigned a = 0; a < gv_asteroids.size(); a++)
	{
		const Asteroid& asteroid = gv_asteroids[a];
		asteroid.draw();

		if(is_show_debug)
			asteroid.drawAxes(asteroid.getRadius() + 50.0);
	}
}

void drawBlackHole ()
{
	// draw the black hole
	glPushMatrix();
		// don't move from origin
		glColor3f(0.0f, 0.0f, 0.0f);
		glutSolidSphere(BLACK_HOLE_RADIUS, 40, 30);
	glPopMatrix();

	// draw the accretion disk
	glPushMatrix();
		// don't move from origin
		glScaled(DISK_RADIUS, DISK_RADIUS, DISK_RADIUS);
		g_disk_display_list.draw();  // has to be last because of transparency
	glPopMatrix();
}

void drawSpaceShip() {
	//display spaceship
	ship.draw();
}

void drawOverlays() {
	//pulling in work from tutorial 17
	system_clock::time_point current_time = system_clock::now();
	float game_duration = duration<float>(current_time - start_time).count();

	//frame rates
	system_clock::duration frame_duration_1 = current_time - last_frame_time;
	if (frame_duration_1 < FRAME_TIME_MIN)
		frame_duration_1 = FRAME_TIME_MIN;
	float frame_duration = duration<float>(frame_duration_1).count();
	float instantaneous_frames_per_second = 1.0f / frame_duration;
	

	smoothed_frames_per_second = 0.95f * smoothed_frames_per_second +
		0.05f * instantaneous_frames_per_second;
	stringstream smoothed_frame_ss;
	smoothed_frame_ss << "Frame rate: " << smoothed_frames_per_second;

	//physics update rate
	system_clock::duration update_duration_1 = current_time - last_update_time;
	if (update_duration_1 < FRAME_TIME_MIN)
		update_duration_1 = FRAME_TIME_MIN;
	float update_duration = duration<float>(update_duration_1).count();
	float instantaneous_updates_per_second = 1.0f / update_duration;

	smoothed_updates_per_second = 0.95f * smoothed_updates_per_second +
		0.05f * instantaneous_updates_per_second;
	stringstream smoothed_update_ss;
	smoothed_update_ss << "Physics rate: " << smoothed_updates_per_second;

	//trying to figure out why ship is moving in only one direction with the gravity
	stringstream ship_velocity_ss;
	ship_velocity_ss << "Ship velocity: " << ship.getVelocity();

	//display text
	SpriteFont::setUp2dView(window_width, window_height);

	font.draw(smoothed_frame_ss.str(), 16, 12);
	font.draw(smoothed_update_ss.str(), 16, 48);
	font.draw(ship_velocity_ss.str(), 16, 72);


	SpriteFont::unsetUp2dView();
	last_frame_time = current_time;


}