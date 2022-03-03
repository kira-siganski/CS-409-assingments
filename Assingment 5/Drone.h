#pragma once
#include "Entity.h"
#include "Spaceship.h"
#include "CoordinateSystem.h"

#include "ObjLibrary/Vector3.h"
#include "ObjLibrary/ObjModel.h"
#include "ObjLibrary/DisplayList.h"

using namespace ObjLibrary;

/*
AI states:
0 - escort
1 - pursuit
2 - avoid
*/
class Drone : public Entity {
public:
	//constructor
	Drone(const Vector3& position,
		const Vector3& velocity,
		const Vector3& escort_position,
		Entity *player_entity,
		double mass,
		double radius,
		double acceleration_main,
		double acceleration_manoeuver,
		double rotation_rate_radians,
		const DisplayList& display_list,
		const Vector3 color
		);

	//determine arrival time
	double calculateArrivalTime(double sf, double distance, double acceleration);
	//calculate minimum safe distance
	double calculateMinSafeDistance(double radius, Vector3 agent_velocity, Vector3 target_velocity, double acceleration);
	//calculate safe speed
	double calculateSafeSpeed(double tarrival, double distance, double sf, double acceleration);
	//calculate desired relative velocity
	Vector3 calculateDesiredRelativeVelocity(Vector3 target, Vector3 agent,  double distance, double sf, double acceleration);
	//calculate overall desired velocity
	Vector3 calculateOverallDesiredVelocity(Entity* target, double delta_time);
	//use engines to adjust towards desired velocity - steering
	void steerToDesiredVelocity(Vector3 desired_velocity, double delta_time);
	//escort steering behaviour
	Vector3 EscortSteering(Entity* space_ship, double acceleration, double delta_time);
	Vector3 findWorldEscortPosition(Entity* space_ship);
	//pursue steering behaviour
	Vector3 pursuitSteering(Entity* crystal, double acceleration, double delta_time);
	//avoid steering behaviour
	Vector3 avoidSteering(Entity* asteroid, double acceleration, double delta_time);
	//handle drone AI
	void handleAI(Entity* target, double delta_time);
	void displayAI(Entity* target, double delta_time);
	void drawMinSafeDistance(Vector3 asteroid_position, double safe_distance);
	void displayPoint(const Vector3& position, const Vector3& direction, double safe_distance);
	void displayPoint(const Vector3& position);
	void setAI(int state);
	int getAIState();
	
	void setTargetCrystal(int crystal);
	int getTargetCrystal();
	void setTargetAsteroid(int asteroid);
	int getTargetAsteroid();
	//check if alive
	bool isAlive();
	void markDead();

	//borrowing from spaceship drawpath
	void drawPath(const Entity& black_hole,
		unsigned int point_count) const;
	Vector3 getColor();


private:
	CoordinateSystem escort_coords;
	Vector3 relative_escort_coords;
	//color
	Vector3 drone_color;
	//is alive
	bool is_alive;
	//main engine acceleration
	double main_engine_acceleration_rate;
	//maneuvering engine accleration
	double manouvering_engine_acceleration_rate;
	double rotation_rate;
	//ai state
	int ai_state;
	//target crystal
	int target_crystal;
	int target_asteroid;

	//giving this a player pointer to help with asteroid func, might overhaul to only need this instead of passing everytime for player
	const Entity* player;
};
