//
//  main.cpp
// Kira: using sln for A3 for the sake of time

#include <iostream>
#include <cassert>
#include <cctype>  // for toupper
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>  // for min/max
#include <chrono>
#include <stack>

#include "GetGlut.h"
#include "Sleep.h"

#include "ObjLibrary/Vector3.h"
#include "ObjLibrary/ObjModel.h"
#include "ObjLibrary/DisplayList.h"
#include "ObjLibrary/SpriteFont.h"

#include "Gravity.h"
#include "CoordinateSystem.h"
#include "PerlinNoiseField3.h"
#include "Entity.h"
#include "BlackHole.h"
#include "Asteroid.h"
#include "Spaceship.h"
#include "Collision.h"

using namespace std;
using namespace chrono;
using namespace ObjLibrary;

void initDisplay ();
void loadModels ();
void initEntities ();
void initAsteroids ();
void initPlayer ();
double getCircularOrbitSpeed (double distance);
void initTime ();

unsigned char fixShift (unsigned char key);
void keyboardDown (unsigned char key, int x, int y);
void keyboardUp (unsigned char key, int x, int y);
void specialDown (int special_key, int x, int y);
void specialUp (int special_key, int x, int y);

void update ();
void handleInput (double delta_time);
void updatePhysics (double delta_time);

void reshape (int w, int h);
void display ();
void drawSkybox ();
void drawEntities (bool is_show_debug);
void drawOverlays ();

namespace
{
	int window_width  = 640;
	int window_height = 480;
	SpriteFont font;

	const unsigned int KEY_PRESSED_COUNT = 0x100 + 5;
	const unsigned int KEY_PRESSED_RIGHT = 0x100 + 0;
	const unsigned int KEY_PRESSED_LEFT  = 0x100 + 1;
	const unsigned int KEY_PRESSED_UP    = 0x100 + 2;
	const unsigned int KEY_PRESSED_DOWN  = 0x100 + 3;
	const unsigned int KEY_PRESSED_END   = 0x100 + 4;
	bool key_pressed[KEY_PRESSED_COUNT];

	const int PHYSICS_PER_SECOND = 60;
	const double SECONDS_PER_PHYSICS = 1.0 / PHYSICS_PER_SECOND;
	const microseconds PHYSICS_MICROSECONDS(1000000 / PHYSICS_PER_SECOND);
	const unsigned int MAXIMUM_UPDATES_PER_FRAME = 10;
	const unsigned int FAST_PHYSICS_FACTOR = 10;
	const double SIMULATE_SLOW_SECONDS = 0.05;

	system_clock::time_point next_update_time;
	const unsigned int SMOOTH_RATE_COUNT = MAXIMUM_UPDATES_PER_FRAME * 2 + 2;
	system_clock::time_point old_frame_times [SMOOTH_RATE_COUNT];
	system_clock::time_point old_update_times[SMOOTH_RATE_COUNT];
	unsigned int next_old_update_index = 0;
	unsigned int next_old_frame_index  = 0;

	bool g_is_paused     = false;
	bool g_is_show_debug = false;

	const unsigned int ASTEROID_COUNT = 100;

	const double BLACK_HOLE_RADIUS  =    50.0;
	const double DISK_RADIUS        = 10000.0;
	const double PLAYER_RADIUS      =     4.0;

	static const double BLACK_HOLE_MASS = 5.0e16;  // kg
	static const double PLAYER_MASS     = 1000.0;  // kg

	DisplayList g_skybox_display_list;
	DisplayList g_disk_display_list;
	DisplayList g_player_display_list;

	BlackHole g_black_hole;

	static const unsigned int ASTEROID_MODEL_COUNT = 25;
	ObjModel ga_asteroid_models[ASTEROID_MODEL_COUNT];

	vector<Asteroid> gv_asteroids;

	const double  CAMERA_BACK_DISTANCE  =   20.0;
	const double  CAMERA_UP_DISTANCE    =    5.0;
	const double  PLAYER_START_DISTANCE = 1000.0;
	const Vector3 PLAYER_START_FORWARD(1.0, 0.0, 0.0);
	Spaceship g_player;

	BoundsList x_bounds;
	BoundsList y_bounds;
	BoundsList z_bounds;

	double random01 ()
	{
		return rand() / (RAND_MAX + 1.0);
	}

	double random2 (double min_value, double max_value)
	{
		assert(min_value <= max_value);

		return min_value + random01() * (max_value - min_value);
	}

	vector<int> collisions;
}  // end of anonymous namespace



int main (int argc, char* argv[])
{
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(0, 0);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutCreateWindow("CS 409 Assignment 3 Solution");
	glutKeyboardFunc(keyboardDown);
	glutKeyboardUpFunc(keyboardUp);
	glutSpecialFunc(specialDown);
	glutSpecialUpFunc(specialUp);
	glutIdleFunc(update);
	glutReshapeFunc(reshape);
	glutDisplayFunc(display);

	//PerlinNoiseField3 pnf;
	//pnf.printPerlin(40, 60, 0.1f);

	initDisplay();
	loadModels();
	initEntities();
	initTime();  // should be last

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
	// change this to an absolute path on Mac computers
	string path = "Models/";

	g_skybox_display_list  = ObjModel(path + "Skybox.obj")     .getDisplayList();
	g_disk_display_list    = ObjModel(path + "Disk.obj")       .getDisplayList();
	g_player_display_list  = ObjModel(path + "Sagittarius.obj").getDisplayList();

	assert(ASTEROID_MODEL_COUNT <= 26);  // only 26 letters to use
	for(unsigned m = 0; m < ASTEROID_MODEL_COUNT; m++)
	{
		string filename = "AsteroidA.obj";
		assert(filename[8] == 'A');
		filename[8] = 'A' + m;
		ga_asteroid_models[m].load(path + filename);
	}

	font.load(path + "Font.bmp");
}

void initEntities ()
{
	// remove existing entities (if any)
	gv_asteroids.clear();

	// create new entities
	g_black_hole = BlackHole(Vector3::ZERO, BLACK_HOLE_MASS,
	                         BLACK_HOLE_RADIUS, DISK_RADIUS, g_disk_display_list);
	initAsteroids();
	initPlayer();
	//sort the lists
	x_bounds.mergeSort();
	y_bounds.mergeSort();
	z_bounds.mergeSort();
}

void initAsteroids ()
{
	static const double DISTANCE_MIN = DISK_RADIUS * 0.2;
	static const double DISTANCE_MAX = DISK_RADIUS * 0.8;

	static const double SPEED_FACTOR_MIN = 0.5;
	static const double SPEED_FACTOR_MAX = 1.5;

	static const double OUTER_RADIUS_MIN =  50.0;
	static const double OUTER_RADIUS_MAX = 400.0;
	static const double INNER_FRACTION_MIN = 0.1;
	static const double INNER_FRACTION_MAX = 0.5;

	stringstream asteroidname;
	for(unsigned a = 0; a < ASTEROID_COUNT; a++)
	{
		// choose a random position in a thick shell around the black hole
		double distance = random2(DISTANCE_MIN, DISTANCE_MAX);
		Vector3 position = Vector3::getRandomUnitVector() * distance;

		// choose starting velocity
		double speed_circle = getCircularOrbitSpeed(distance);
		double speed_factor = random2(SPEED_FACTOR_MIN, SPEED_FACTOR_MAX);
		double speed = speed_circle * speed_factor;
		Vector3 velocity = Vector3::getRandomUnitVector().getRejection(position);  // tangent to gravity
		assert(!velocity.isZero());
		velocity.setNorm(speed);

		// mostly smaller asteroids
		double outer_radius = min(random2(OUTER_RADIUS_MIN, OUTER_RADIUS_MAX),
		                          random2(OUTER_RADIUS_MIN, OUTER_RADIUS_MAX));

		double inner_fraction = random2(INNER_FRACTION_MIN, INNER_FRACTION_MAX);
		double inner_radius   = outer_radius * inner_fraction;

		unsigned int model_index = a % ASTEROID_MODEL_COUNT;
		assert(model_index < ASTEROID_MODEL_COUNT);
		assert(!ga_asteroid_models[model_index].isEmpty());

		gv_asteroids.push_back(Asteroid(position, velocity,
		                                inner_radius, outer_radius,
		                                ga_asteroid_models[model_index]));
		//setting number of asteroids and setting their collision check to false where a = a
		collisions.push_back(a);
		//grabbing their initial positions for the bound list here and using the a to name each one
		asteroidname << "asteroid" << a;
		//create code to insertobject into list after finding its bounds
		double* bounds = gv_asteroids[a].getBounds();

		x_bounds.insertFront(bounds[0], 0, a);
		x_bounds.insertFront(bounds[1], 1, a);

		y_bounds.insertFront(bounds[2], 0, a);
		y_bounds.insertFront(bounds[3], 1, a);

		z_bounds.insertFront(bounds[4], 0, a);
		z_bounds.insertFront(bounds[5], 1, a);

		asteroidname.str("");
	}
	assert(gv_asteroids.size() == ASTEROID_COUNT);
}

void initPlayer ()
{
	const double PLAYER_FORWARD_POWER  = 500.0;  // m/s^2
	const double PLAYER_MANEUVER_POWER =  50.0;  // m/s^2
	const double PLAYER_ROTATION_RATE  =   3.0;  // radians / second

	double  player_speed    = getCircularOrbitSpeed(PLAYER_START_DISTANCE);
	Vector3 player_position(0.0, PLAYER_START_DISTANCE, 0.0);
	Vector3 player_velocity = PLAYER_START_FORWARD * player_speed;

	assert(g_player_display_list.isReady());
	g_player = Spaceship(player_position, player_velocity,
	                     PLAYER_MASS, PLAYER_RADIUS,
	                     PLAYER_FORWARD_POWER, PLAYER_MANEUVER_POWER, PLAYER_ROTATION_RATE,
	                     g_player_display_list);
	double* bounds = g_player.getBounds();
	x_bounds.insertFront(bounds[0], 0, ASTEROID_COUNT);
	x_bounds.insertFront(bounds[1], 1, ASTEROID_COUNT);

	y_bounds.insertFront(bounds[2], 0, ASTEROID_COUNT);
	y_bounds.insertFront(bounds[3], 1, ASTEROID_COUNT);

	z_bounds.insertFront(bounds[4], 0, ASTEROID_COUNT);
	z_bounds.insertFront(bounds[5], 1, ASTEROID_COUNT);

	collisions.push_back(ASTEROID_COUNT);
	//TODO:
	//now to make two asteroids to crash in front of the player
	//could use velocity to get forward
	//just change the position and velocity of two already initialized asteroids
	//also need to make one much larger than the other
}

double getCircularOrbitSpeed (double distance)
{
	assert(distance > 0.0);

	return sqrt(GRAVITY * g_black_hole.getMass() / distance);
}

void initTime ()
{
	system_clock::time_point start_time = system_clock::now();
	next_update_time = start_time;

	for(unsigned int i = 1; i < SMOOTH_RATE_COUNT; i++)
	{
		unsigned int steps_back = SMOOTH_RATE_COUNT - i;
		old_update_times[i] = start_time - PHYSICS_MICROSECONDS * steps_back;
		old_frame_times [i] = start_time - PHYSICS_MICROSECONDS * steps_back;
	}
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
	system_clock::time_point current_time = system_clock::now();
	for(unsigned int i = 0; i < MAXIMUM_UPDATES_PER_FRAME &&
	                        next_update_time < current_time; i++)
	{
		double delta_time = SECONDS_PER_PHYSICS;
		if(g_is_paused)
			delta_time = 0.0;
		else if(key_pressed['g'])
			delta_time *= FAST_PHYSICS_FACTOR;

		handleInput(delta_time);
		if(delta_time > 0.0)
		{
			updatePhysics(delta_time);

			old_update_times[next_old_update_index % SMOOTH_RATE_COUNT] = current_time;
			next_old_update_index++;

			if(key_pressed['u'])
				sleep(SIMULATE_SLOW_SECONDS);
		}

		next_update_time += PHYSICS_MICROSECONDS;
		current_time = system_clock::now();
	}

	if(current_time < next_update_time)
	{
		system_clock::duration sleep_time = next_update_time - current_time;
		sleep(duration<double>(sleep_time).count());
	}

	glutPostRedisplay();
}

void handleInput (double delta_time)
{
	//
	//  Accelerate player - depends on physics rate
	//

	if(key_pressed[' '])
		g_player.thrustMainEngine(delta_time);
	if(key_pressed[';'] || key_pressed['\''])  // either key
		g_player.thrustManoeuver(delta_time,  g_player.getForward());
	if(key_pressed['/'])
		g_player.thrustManoeuver(delta_time, -g_player.getForward());
	if(key_pressed['w'] || key_pressed['e'])  // either key
		g_player.thrustManoeuver(delta_time,  g_player.getUp());
	if(key_pressed['s'])
		g_player.thrustManoeuver(delta_time, -g_player.getUp());
	if(key_pressed['d'])
		g_player.thrustManoeuver(delta_time,  g_player.getRight());
	if(key_pressed['a'])
		g_player.thrustManoeuver(delta_time, -g_player.getRight());
	//TODO:
	if (key_pressed['k']){
		//crystal blasting stuff
		}

	//
	//  Rotate player - independant of physics rate
	//

	if(key_pressed['.'])
		g_player.rotateAroundForward(SECONDS_PER_PHYSICS, true);
	if(key_pressed[','])
		g_player.rotateAroundForward(SECONDS_PER_PHYSICS, false);
	if(key_pressed[KEY_PRESSED_UP])
		g_player.rotateAroundRight(SECONDS_PER_PHYSICS, false);
	if(key_pressed[KEY_PRESSED_DOWN])
		g_player.rotateAroundRight(SECONDS_PER_PHYSICS, true);
	if(key_pressed[KEY_PRESSED_LEFT])
		g_player.rotateAroundUp(SECONDS_PER_PHYSICS, false);
	if(key_pressed[KEY_PRESSED_RIGHT])
		g_player.rotateAroundUp(SECONDS_PER_PHYSICS, true);

	//
	//  Other
	//

	// 'g' is handled in update
	if(key_pressed['p'])
	{
		g_is_paused = !g_is_paused;
		key_pressed['p'] = false;  // only once per keypress
	}
	if(key_pressed['t'])
	{
		g_is_show_debug = !g_is_show_debug;
		key_pressed['t'] = false;  // only once per keypress
	}
	// 'u' is handled in update
	// 'y' is handled in draw
	if(key_pressed[KEY_PRESSED_END])
	{
		initEntities();
		key_pressed[KEY_PRESSED_END] = false;  // only once per keypress
	}
}

void updatePhysics (double delta_time)
{	
	//check for collisions
	double* bounds;
	//could update the physics func to take the plane sweep list as an input
	for (unsigned a = 0; a < gv_asteroids.size(); a++) {
		gv_asteroids[a].updatePhysics(delta_time, g_black_hole);
		bounds = gv_asteroids[a].getBounds();
		//update lists
		x_bounds.updateObject(bounds[0], 0, a);
		x_bounds.updateObject(bounds[1], 1, a);

		y_bounds.updateObject(bounds[2], 0, a);
		y_bounds.updateObject(bounds[3], 1, a);

		z_bounds.updateObject(bounds[4], 0, a);
		z_bounds.updateObject(bounds[5], 1, a);
	}
		


	//alive check for player already built in this solution and it won't move the ship anymore if dead
	if (g_player.isAlive()) {
		g_player.updatePhysics(delta_time, g_black_hole);

		bounds = g_player.getBounds();
		//update lists
		x_bounds.updateObject(bounds[0], 0, ASTEROID_COUNT);
		x_bounds.updateObject(bounds[1], 1, ASTEROID_COUNT);

		y_bounds.updateObject(bounds[2], 0, ASTEROID_COUNT);
		y_bounds.updateObject(bounds[3], 1, ASTEROID_COUNT);

		z_bounds.updateObject(bounds[4], 0, ASTEROID_COUNT);
		z_bounds.updateObject(bounds[5], 1, ASTEROID_COUNT);

	}
	//sort bounds lists with bubble sort after all locations updated
	x_bounds.bubbleSort();
	y_bounds.bubbleSort();
	z_bounds.bubbleSort();

	//check if collision on all dimensions
	//vector<int> x = *x_bounds.findCollisions(); //Xmin Ymin Zmin Ymin Zmax Xmax 
	//vector<int> y = *y_bounds.findCollisions(); 
	//vector<int> z = *z_bounds.findCollisions();
	x_bounds.findPairs();
	cout << "All pairs found" << endl;
}



void reshape (int w, int h)
{
	glViewport (0, 0, w, h);

	window_width  = w;
	window_height = h;

	glMatrixMode(GL_PROJECTION);
	glLoadIdentity();
	gluPerspective(60, (GLdouble)w / (GLdouble)h, 1.0, 100000.0);
	glMatrixMode(GL_MODELVIEW);

	glutPostRedisplay();
}

void display ()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	// clear the screen - any drawing before here will not display

	glLoadIdentity();
	g_player.setupFollowCamera(CAMERA_BACK_DISTANCE, CAMERA_UP_DISTANCE);
	// camera is set up - any drawing before here will display incorrectly

	drawSkybox();  // has to be first
	drawEntities(g_is_show_debug);
	drawOverlays();

	glBegin(GL_LINES);
		glColor3d(1.0, 0.0, 0.0);
		glVertex3d(0.0, 1.0, 0.0);
		glVertex3d(1000.0, 1.0, 0.0);
		glColor3d(0.0, 0.0, 0.0);
		glVertex3d(0.0, 1.0, 0.0);
		glVertex3d(0.0, 1000.0, 0.0);
		glColor3d(0.0, 0.0, 1.0);
		glVertex3d(0.0, 1.0, 0.0);
		glVertex3d(0.0, 1.0, 1000.0);
	glEnd();

	if(key_pressed['y'])
		sleep(SIMULATE_SLOW_SECONDS);  // simulate slow drawing

	// send the current image to the screen - any drawing after here will not display
	glutSwapBuffers();
}

void drawSkybox ()
{
	glPushMatrix();
		Vector3 camera = g_player.getFollowCameraPosition(CAMERA_BACK_DISTANCE, CAMERA_UP_DISTANCE);
		glTranslated(camera.x, camera.y, camera.z);
		glRotated(90.0, 0.0, 0.0, 1.0);  // line band of clouds on skybox up with accretion disk

		glDepthMask(GL_FALSE);
		g_skybox_display_list.draw();
		glDepthMask(GL_TRUE);
	glPopMatrix();
}

void drawEntities (bool is_show_debug)
{
	static const Vector3 PLAYER_COLOUR(0.0, 0.0, 1.0);

	const Vector3& player_position = g_player.getPosition();
	for(unsigned a = 0; a < gv_asteroids.size(); a++)
	{
		const Asteroid& asteroid = gv_asteroids[a];
		asteroid.draw();

		if(is_show_debug)
			asteroid.drawAxes(asteroid.getRadius() + 50.0);
	}

	if(g_player.isAlive())
	{
		g_player.draw();
		g_player.drawPath(g_black_hole, 1000, PLAYER_COLOUR);
	}

	g_black_hole.draw();  // must be last
}

void drawOverlays ()
{
	SpriteFont::setUp2dView(window_width, window_height);

	system_clock::time_point current_time = system_clock::now();

	// display frame rate

	unsigned int oldest_frame_index = (next_old_frame_index + 1) % SMOOTH_RATE_COUNT;
	duration<float> total_frame_duration = current_time - old_frame_times[oldest_frame_index];
	float average_frame_duration = total_frame_duration.count() / (SMOOTH_RATE_COUNT - 1);
	float average_frame_rate = 1.0f / average_frame_duration;

	stringstream smoothed_frame_rate_ss;
	smoothed_frame_rate_ss << "Frame rate:\t" << setprecision(3) << average_frame_rate;
	font.draw(smoothed_frame_rate_ss.str(), 16, 16);

	// update frame rate values

	old_frame_times[next_old_frame_index % SMOOTH_RATE_COUNT] = current_time;
	next_old_frame_index++;

	// display physics rate

	unsigned int oldest_update_index = (next_old_update_index + 1) % SMOOTH_RATE_COUNT;
	duration<float> total_update_duration = current_time - old_update_times[oldest_update_index];
	float average_update_duration = total_update_duration.count() / (SMOOTH_RATE_COUNT - 1);
	float average_update_rate = 1.0f / average_update_duration;

	stringstream smoothed_update_rate_ss;
	smoothed_update_rate_ss << "Update rate:\t" << setprecision(3) << average_update_rate;
	font.draw(smoothed_update_rate_ss.str(), 16, 40);

	// display control keys

	unsigned char byte_g = key_pressed['g'] ? 0x00 : 0xFF;
	unsigned char byte_t = g_is_show_debug  ? 0x00 : 0xFF;
	unsigned char byte_y = key_pressed['y'] ? 0x00 : 0xFF;
	unsigned char byte_u = key_pressed['u'] ? 0x00 : 0xFF;

	//TODO: display 'Game Over message' if player is dead
	if (!g_player.isAlive()) {
		font.draw("Game Over", window_width / 2, window_height / 2);
	}



	font.draw("[G]:\tAccelerate time",  window_width - 256,  16, byte_g, 0xFF, byte_g);
	font.draw("[T]:\tToggle debugging", window_width - 256,  48, byte_t, 0xFF, byte_t);
	font.draw("[Y]:\tSlow display",     window_width - 256,  80, byte_y, 0xFF, byte_y);
	font.draw("[U]:\tSlow physics",     window_width - 256, 112, byte_u, 0xFF, byte_u);

	SpriteFont::unsetUp2dView();
}

