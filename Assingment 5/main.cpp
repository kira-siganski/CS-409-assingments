//	Kira: Starting with soln for A4 as the base
//  main.cpp
//

#include <cassert>
#include <climits>
#include <cctype>  // for toupper
#include <sstream>
#include <iomanip>
#include <vector>
#include <algorithm>  // for min/max
#include <chrono>

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
#include "Crystal.h"
#include "Spaceship.h"
#include "Drone.h"
#include "Collisions.h"

using namespace std;
using namespace chrono;
using namespace ObjLibrary;

void initDisplay ();
void loadModels ();
void initEntities ();
void initAsteroids ();
void initPlayer ();
void initDrones();
double getCircularOrbitSpeed (double distance);
void initTime ();

unsigned char fixShift (unsigned char key);
void keyboardDown (unsigned char key, int x, int y);
void keyboardUp (unsigned char key, int x, int y);
void specialDown (int special_key, int x, int y);
void specialUp (int special_key, int x, int y);

void update ();
void handleInput (double delta_time);
void knockOffCrystals ();
void addCrystal (const ObjLibrary::Vector3& position,
                 const ObjLibrary::Vector3& asteroid_velocity);
void updatePhysics (double delta_time);
void handleCollisions ();

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
	const double DEBUG_MAX_DISTANCE =  2000.0;

	static const double BLACK_HOLE_MASS = 5.0e16;  // kg
	static const double PLAYER_MASS     = 1000.0;  // kg

	DisplayList g_skybox_display_list;
	DisplayList g_disk_display_list;
	DisplayList g_crystal_display_list;
	DisplayList g_player_display_list;
	DisplayList g_drone_display_list;

	BlackHole g_black_hole;

	static const unsigned int ASTEROID_MODEL_COUNT = 25;
	ObjModel ga_asteroid_models[ASTEROID_MODEL_COUNT];

	vector<Asteroid> gv_asteroids;

	const double CRYSTAL_KNOCK_OFF_RANGE = 200.0;
	const unsigned int CRYSTAL_KNOCK_OFF_COUNT = 10;
	const double CRYSTAL_KNOCK_OFF_SPEED = 10.0;
	vector<Crystal> gv_crystals;

	const double  CAMERA_BACK_DISTANCE  =   20.0;
	const double  CAMERA_UP_DISTANCE    =    5.0;
	const double  PLAYER_START_DISTANCE = 1000.0;
	const Vector3 PLAYER_START_FORWARD(1.0, 0.0, 0.0);
	Spaceship g_player;
	unsigned int g_crystals_collected = 0;

	const int DRONE_COUNT = 5;
	vector<Drone> gv_drones;
	DisplayList dl_drone_models[DRONE_COUNT];
	unsigned int g_drones_alive = 5;


	double random01 ()
	{
		return rand() / (RAND_MAX + 1.0);
	}

	double random2 (double min_value, double max_value)
	{
		assert(min_value <= max_value);

		return min_value + random01() * (max_value - min_value);
	}

}  // end of anonymous namespace



int main (int argc, char* argv[])
{
	glutInitWindowSize(640, 480);
	glutInitWindowPosition(0, 0);

	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_DEPTH | GLUT_RGB);
	glutCreateWindow("CS 409 Assignment 4 Solution");
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
	g_crystal_display_list = ObjModel(path + "Crystal.obj")    .getDisplayList();
	g_player_display_list  = ObjModel(path + "Sagittarius.obj").getDisplayList();
	//g_drone_display_list = ObjModel(path + "Grapple.obj").getDisplayList();

	assert(ASTEROID_MODEL_COUNT <= 26);  // only 26 letters to use
	for(unsigned m = 0; m < ASTEROID_MODEL_COUNT; m++)
	{
		string filename = "AsteroidA.obj";
		assert(filename[8] == 'A');
		filename[8] = 'A' + m;
		ga_asteroid_models[m].load(path + filename);
	}
	ObjModel drone(path + "Grapple.obj");
	vector<string> colors = { "grapple_body_red", "grapple_body_orange", "grapple_body_yellow", "grapple_body_green", "grapple_body_cyan" };
	for (int i = 0; i < 5; i++) {
		dl_drone_models[i] = drone.getDisplayListMaterial(colors[i]);
	}

	font.load(path + "Font.bmp");
}

void initEntities ()
{
	// remove existing entities (if any)
	gv_asteroids.clear();
	gv_crystals.clear();
	gv_drones.clear();
	g_crystals_collected = 0;
	g_drones_alive = 5;

	// create new entities
	g_black_hole = BlackHole(Vector3::ZERO, BLACK_HOLE_MASS,
	                         BLACK_HOLE_RADIUS, DISK_RADIUS, g_disk_display_list);
	initAsteroids();
	initPlayer();
	initDrones();
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

	static const double  COLLISION_AHEAD_DISTANCE  = 1500.0;
	static const double  COLLISION_HALF_SEPERATION =  500.0;
	static const Vector3 COLLISION_POSITION_1(COLLISION_AHEAD_DISTANCE, PLAYER_START_DISTANCE,  COLLISION_HALF_SEPERATION);
	static const Vector3 COLLISION_POSITION_2(COLLISION_AHEAD_DISTANCE, PLAYER_START_DISTANCE, -COLLISION_HALF_SEPERATION);

	// create 2 asteroids to collide in front of player
	double collider_speed1 = getCircularOrbitSpeed(COLLISION_POSITION_1.getNorm()) * 0.9;
	double collider_speed2 = getCircularOrbitSpeed(COLLISION_POSITION_2.getNorm()) * 1.1;
	Vector3 collider_velocity1 = Vector3(0.0, 0.0, -collider_speed1);
	Vector3 collider_velocity2 = Vector3(0.0, 0.0,  collider_speed2);
	double collider_inner_radius1 = OUTER_RADIUS_MAX * INNER_FRACTION_MIN;
	double collider_inner_radius2 = OUTER_RADIUS_MIN * INNER_FRACTION_MAX;

	assert(1 < ASTEROID_MODEL_COUNT);
	assert(!ga_asteroid_models[0].isEmpty());
	assert(!ga_asteroid_models[1].isEmpty());
	gv_asteroids.push_back(Asteroid(COLLISION_POSITION_1, collider_velocity1,
	                                collider_inner_radius1, OUTER_RADIUS_MAX,
	                                ga_asteroid_models[0]));
	gv_asteroids.push_back(Asteroid(COLLISION_POSITION_2, collider_velocity2,
	                                collider_inner_radius2, OUTER_RADIUS_MIN,
	                                ga_asteroid_models[1]));

	// create remaining asteroids
	for(unsigned a = 2; a < ASTEROID_COUNT; a++)
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

}

void initDrones() {
	const double DRONE_FORWARD_POWER = 250.0;  // m/s^2
	const double DRONE_MANEUVER_POWER = 25.0;  // m/s^2
	const double DRONE_ROTATION_RATE = 1.0;  // radians / secon
	const double DRONE_MASS = 100;
	const double DRONE_RADIUS = 2;
	double drone_speed = getCircularOrbitSpeed(PLAYER_START_DISTANCE);
	Vector3 velocity = PLAYER_START_FORWARD * drone_speed;
	Vector3 escort_position_initial(0.0, 10.0, 0.0);
	Vector3 position;
	vector<Vector3> color_vec = { Vector3(1.0,0.0,0.0), Vector3(1.0,0.5,0.0) , Vector3(1.0,1.0,0.0) , Vector3(0.0,1.0,0.0) , Vector3(0.5,1.0,1.0) };
	
	Vector3 escort_position;
	for (int i = 0; i < DRONE_COUNT; i++) {
		CoordinateSystem escort_coords = g_player.getCoordinateSystem();

		escort_position = escort_position_initial.getRotatedX(-2.0 + i);

		escort_coords.addPosition(escort_position);

		position = escort_coords.localToWorld(escort_coords.getPosition());

		gv_drones.push_back(Drone(position, velocity, escort_position, &g_player, DRONE_MASS, DRONE_RADIUS, DRONE_FORWARD_POWER, DRONE_MANEUVER_POWER, DRONE_ROTATION_RATE, 
			dl_drone_models[i], color_vec[i]));
	}

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
			handleCollisions();

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
	if(key_pressed['k'])
	{
		knockOffCrystals();
		key_pressed['k'] = false;  // only once per keypress
	}
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

void knockOffCrystals ()
{
	const Vector3& player_position = g_player.getPosition();

	for(unsigned a = 0; a < gv_asteroids.size(); a++)
	{
		Asteroid& asteroid = gv_asteroids[a];
		if(asteroid.isCrystals())
		{
			Vector3 asteroid_position  = asteroid.getPosition();
			Vector3 asteroid_to_player = player_position - asteroid_position;
			double asteroid_radius = asteroid.getRadiusForDirection(asteroid_to_player.getNormalized());
			double maximum_distance = asteroid_radius + CRYSTAL_KNOCK_OFF_RANGE;

			if(asteroid_to_player.isNormLessThan(maximum_distance))
			{
				Vector3 knock_off_position = asteroid_position + asteroid_to_player.getCopyWithNorm(asteroid_radius);
				for(unsigned c = 0; c < CRYSTAL_KNOCK_OFF_COUNT; c++)
					addCrystal(knock_off_position, asteroid.getVelocity());
				asteroid.removeCrystals();
			}
		}
	}
}

void addCrystal (const ObjLibrary::Vector3& position,
                 const ObjLibrary::Vector3& asteroid_velocity)
{
	static const unsigned int EXPAND_VECTOR = UINT_MAX;

	Vector3 crystal_velocity = asteroid_velocity + Vector3::getRandomUnitVector() * CRYSTAL_KNOCK_OFF_SPEED;

	// look for an existing already-gone crystal to replace
	//  -> this will speed up searching the list elsewhere
	unsigned int index = EXPAND_VECTOR;
	for(unsigned c = 0; c < gv_crystals.size(); c++)
		if(gv_crystals[c].isGone())
		{
			index = c;
			break;
		}

	assert(g_crystal_display_list.isReady());
	if(index == EXPAND_VECTOR)
		gv_crystals.push_back(Crystal(position, crystal_velocity, g_crystal_display_list));
	else
		gv_crystals[index] =  Crystal(position, crystal_velocity, g_crystal_display_list);
}

void updatePhysics (double delta_time)
{
	for(unsigned a = 0; a < gv_asteroids.size(); a++)
		gv_asteroids[a].updatePhysics(delta_time, g_black_hole);

	for(unsigned c = 0; c < gv_crystals.size(); c++)
		if(!gv_crystals[c].isGone())
			gv_crystals[c].updatePhysics(delta_time, g_black_hole);

	if(g_player.isAlive())
		g_player.updatePhysics(delta_time, g_black_hole);

	for (int i = 0; i < DRONE_COUNT; i++) {
		if (gv_drones[i].isAlive()) {
			gv_drones[i].updatePhysics(delta_time, g_black_hole);
			int target;
			switch (gv_drones[i].getAIState()) {
			case 0: //escort
				gv_drones[i].handleAI(&g_player, delta_time);
				break;
			case 1: //chase crystals
				target = gv_drones[i].getTargetCrystal();
				gv_drones[i].handleAI(&gv_crystals[target], delta_time);
				break;
			case 2: // if an asteroid is too close
				target = gv_drones[i].getTargetAsteroid();
				gv_drones[i].handleAI(&gv_asteroids[target], delta_time);
				break;
			}	
		}
	}

}

void handleCollisions ()
{
/*
	if(Collisions::isCollision(g_player, g_black_hole))
		g_player.markDead();
*/
	for(unsigned c = 0; c < gv_crystals.size(); c++)
	{
		Crystal& crystal = gv_crystals[c];
		if(!crystal.isGone())
		{
			//if(Collisions::isCollision(g_black_hole, crystal))
			//	crystal.markGone();
			//else
			if(Collisions::isCollision(g_player, crystal))
			{
				assert(!crystal.isGone());
				crystal.markGone();
				g_crystals_collected++;

				//quick loop to tell any other drones after that same crystal it's gone
				for (int i = 0; i < DRONE_COUNT; i++) {
					if (&gv_crystals[gv_drones[i].getTargetCrystal()] == &crystal) {
						gv_drones[i].setTargetCrystal(gv_crystals.size());
						gv_drones[i].setAI(0);
					}
				}
			}
			for (int d = 0; d < DRONE_COUNT; d++) {
				if (gv_drones[d].isAlive()) {
					if (Collisions::isCollision(gv_drones[d], crystal)) {
						//assert(!crystal.isGone());
						crystal.markGone();
						//quick loop to tell any other drones after that same crystal it's gone
						for (int i = 0; i < DRONE_COUNT; i++) {
							if (&gv_crystals[gv_drones[i].getTargetCrystal()] == &crystal) {
								gv_drones[i].setTargetCrystal(-1);
								gv_drones[i].setAI(0);
							}
						}
						g_crystals_collected++;
						gv_drones[d].setAI(0); //0 means escort state
					}
					if (crystal.getPosition().isDistanceLessThan(gv_drones[d].getPosition(), 405) && gv_drones[d].getTargetCrystal() == -1) {
						gv_drones[d].setTargetCrystal(c);
						gv_drones[d].setAI(1); //int 1 means pursuit state
					}
				}
			}
				
			
		}
	}

	for(unsigned a = 0; a < gv_asteroids.size(); a++)
	{
		Asteroid& asteroid = gv_asteroids[a];

		for(unsigned a2 = a + 1; a2 < gv_asteroids.size(); a2++)
		{
			Asteroid& asteroid2 = gv_asteroids[a2];
			if(Collisions::isCollision(asteroid, asteroid2))
				Collisions::elastic(asteroid, asteroid2);
		}

		for(unsigned c = 0; c < gv_crystals.size(); c++)
		{
			Crystal& crystal = gv_crystals[c];
			if(!crystal.isGone())
				if(Collisions::isCollision(crystal, asteroid))
				{
					Collisions::elastic(crystal, asteroid);
					//Collisions::bounceOff(crystal, asteroid);  // does about the same thing
				}
		}

		if(Collisions::isCollision(g_player, asteroid))
			g_player.markDead();

		for (int d = 0; d < DRONE_COUNT; d++) {
			if (gv_drones[d].isAlive()) {
				if (Collisions::isCollision(gv_drones[d], asteroid)) {
					gv_drones[d].markDead();
					g_drones_alive--;
				}

				//calculate if need to avoid asteroid, doing the check here to make it easier to switch states
				//since this happens after the crystal check it'll avoid the asteroids and only chase 
				//crystals again if its safe based on ship position
				if (asteroid.getPosition().isDistanceLessThan(g_player.getPosition(), 500)) {
					gv_drones[d].setTargetAsteroid(a);
					gv_drones[d].setAI(2);
				}
			}	
		}
	}
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
		{
			asteroid.drawAxes(asteroid.getRadius() + 50.0);
			if(asteroid.getPosition().isDistanceLessThan(player_position, DEBUG_MAX_DISTANCE))
				asteroid.drawSurfaceEquators();
		}
	}

	for(unsigned c = 0; c < gv_crystals.size(); c++)
	{
		const Crystal& crystal = gv_crystals[c];
		if(!crystal.isGone())
			crystal.draw();
	}

	if(g_player.isAlive())
	{
		g_player.draw();
		g_player.drawPath(g_black_hole, 1000, PLAYER_COLOUR);
	}

	for (int i = 0; i < DRONE_COUNT; i++) {
		if (gv_drones[i].isAlive()) {
			gv_drones[i].draw();
			gv_drones[i].drawPath(g_black_hole, 500);

			if (is_show_debug) {
				int target;
				double delta_time = 1.0 / 60.0;
				switch (gv_drones[i].getAIState()) {
				case 0: //escorting
					gv_drones[i].displayAI(&g_player, delta_time);
					break;
				case 1: //chasing crystals
					target = gv_drones[i].getTargetCrystal();
					gv_drones[i].displayAI(&gv_crystals[target], delta_time);
					break;
				case 2: //avoiding asteroids
					target = gv_drones[i].getTargetAsteroid();
					gv_drones[i].displayAI(&gv_asteroids[target], delta_time);
					break; 
				}
				
			}
		}	
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
	if(g_is_paused)
		smoothed_update_rate_ss << "Update rate:\t-----";
	else
		smoothed_update_rate_ss << "Update rate:\t" << setprecision(3) << average_update_rate;
	font.draw(smoothed_update_rate_ss.str(), 16, 40);

	// display crystal information

	unsigned int crystal_count = 0;
	for(unsigned c = 0; c < gv_crystals.size(); c++)
		if(!gv_crystals[c].isGone())
			crystal_count++;
	stringstream crystals_ss;
	crystals_ss << "Drifting crystals:\t" << crystal_count;
	font.draw(crystals_ss.str(), 16, 64);

	stringstream collected_ss;
	collected_ss << "Collected crystals:\t" << g_crystals_collected;
	font.draw(collected_ss.str(), 16, 88);

	stringstream drones_ss;
	drones_ss << "Drones: " << g_drones_alive;
	font.draw(drones_ss.str(), 16, 110);

	// display control keys

	unsigned char byte_g = key_pressed['g'] ? 0x00 : 0xFF;
	unsigned char byte_t = g_is_show_debug  ? 0x00 : 0xFF;
	unsigned char byte_y = key_pressed['y'] ? 0x00 : 0xFF;
	unsigned char byte_u = key_pressed['u'] ? 0x00 : 0xFF;

	font.draw("[G]:\tAccelerate time",  window_width - 256,  16, byte_g, 0xFF, byte_g);
	font.draw("[T]:\tToggle debugging", window_width - 256,  48, byte_t, 0xFF, byte_t);
	font.draw("[Y]:\tSlow display",     window_width - 256,  80, byte_y, 0xFF, byte_y);
	font.draw("[U]:\tSlow physics",     window_width - 256, 112, byte_u, 0xFF, byte_u);

	// display "GAME OVER" if appropriate

	if(!g_player.isAlive())
		font.draw("GAME OVER", window_width / 2, window_height / 2);

	SpriteFont::unsetUp2dView();
}

