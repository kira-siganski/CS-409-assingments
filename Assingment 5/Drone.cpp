#include "Drone.h"
#include "Entity.h"
#include "Spaceship.h"
#include "Asteroid.h"
#include "CoordinateSystem.h"
//include namespace objlibrary

#include "GetGlut.h"
#include "ObjLibrary/Vector3.h"
#include "ObjLibrary/DisplayList.h"

#include <iostream>
using namespace std;
using namespace ObjLibrary;

//constructor
Drone::Drone(
	const Vector3& position,
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
) : Entity(position, velocity, mass, radius, display_list, radius / 2) {
	is_alive = true;
	ai_state = 0;
	target_crystal = -1;
	target_asteroid = -1;
	drone_color = color;

	escort_coords = player_entity->getCoordinateSystem();
	player = player_entity;
	relative_escort_coords = escort_position;

	main_engine_acceleration_rate = acceleration_main;
	manouvering_engine_acceleration_rate = acceleration_manoeuver;
	rotation_rate = rotation_rate_radians;
}

//determine arrival time
double Drone::calculateArrivalTime(double sf, double distance, double acceleration) {
	//tarrival = (sqrt(sf^2 + 2*a*d) - sf) / a
	double arrival_time = (sqrt(pow(sf, 2) + 2*acceleration*distance) - sf) / acceleration;

	return arrival_time;
}
//calculate minimum safe distance
double Drone::calculateMinSafeDistance(double radius, Vector3 agent_velocity, Vector3 target_velocity, double acceleration) {
	//dsafe = r2 + 10 * ||agentv - targetv|| / a
	double safe_distance;
	safe_distance = radius + 10 * (agent_velocity - target_velocity).getNorm() / acceleration;

	return safe_distance;
}
//calculate safe speed
double Drone::calculateSafeSpeed(double tarrival, double distance, double sf, double acceleration) {
	//safe speed = sf + a*tarrival
	double safe_speed = sf + acceleration * tarrival;

	return safe_speed;
}
//calculate desired relative velocity
Vector3 Drone::calculateDesiredRelativeVelocity(Vector3 target, Vector3 agent, double distance, double sf, double acceleration) {
	//calculate safe speed for distance
	double arrival_time = calculateArrivalTime(sf, distance, acceleration);
	double safe_speed = calculateSafeSpeed(arrival_time, distance, sf, acceleration);
	
	// unit vec direction = P2 - P1 / ||P2 - P1||
	Vector3 direction = target - agent;
	Vector3 direction_unit_vec = direction / (target - agent).getNorm();
	Vector3 desiredVelocity = direction_unit_vec * safe_speed;
	return desiredVelocity;
}
//calculate overall desired velocity
Vector3 Drone::calculateOverallDesiredVelocity(Entity* target, double delta_time) {
	ObjLibrary::Vector3 desiredVelocity = m_velocity;
	
	switch (ai_state) {
	case 0: //escort state
		desiredVelocity = EscortSteering(target, manouvering_engine_acceleration_rate, delta_time);
		desiredVelocity = desiredVelocity + target->getVelocity();
		break;
	case 1: //pursuit state - chasing crystals
		//placeholder
		//desiredVelocity = EscortSteering(target, manouvering_engine_acceleration_rate, delta_time);
		desiredVelocity = pursuitSteering(target, main_engine_acceleration_rate, delta_time);
		desiredVelocity = desiredVelocity + target->getVelocity();
		break;
	case 2: //avoid state - avoid asteroids
		//placeholder
		//desiredVelocity = EscortSteering(target, manouvering_engine_acceleration_rate, delta_time);
		desiredVelocity = avoidSteering(target, main_engine_acceleration_rate, delta_time);
		//addition was moved inside function to avoid altering course if no action needed to avoid asteroid
		
		break;
	}

	return desiredVelocity;
}
//use engines to adjust towards desired velocity - steering
void Drone::steerToDesiredVelocity(Vector3 desired_velocity, double delta_time) {
	if (desired_velocity != m_velocity) {
		//magnitude of the difference in velocities not difference of magnitudes/speed
		double difference = (desired_velocity - m_velocity).getNorm();

		//need to calculate steering vector
		Vector3 steering_vector = desired_velocity - m_velocity;

		if (steering_vector.getNorm() < 50) {
			//use manouvering engines
			//m_coords.rotateToVector(desired_velocity, rotation_rate);
			m_velocity += steering_vector.getNormalized() * manouvering_engine_acceleration_rate * delta_time;
		}
		else {
			//rotate to face direction of desired velocity 
			m_coords.rotateToVector(steering_vector, rotation_rate * delta_time);
			
			if (m_coords.getForward().isSameDirection(steering_vector)) {
				m_velocity += steering_vector.getNormalized() * main_engine_acceleration_rate * delta_time;
			}
		}
	}
	
}
//escort steering behaviour
Vector3 Drone::EscortSteering(Entity* space_ship, double acceleration, double delta_time)
{
	//find current escort position coordinates
	Vector3 current_escort_position = findWorldEscortPosition(space_ship);

	Vector3 escort_velocity = space_ship->getVelocity();
	Vector3 drone_velocity = m_velocity;

	double distance_current = (current_escort_position - m_coords.getPosition()).getNorm();
	double tfuture = calculateArrivalTime(0.0, distance_current, acceleration);
	//cout << "Drone: " << drone_color << endl;
	//cout << "tfuture: " << tfuture << endl;
	Vector3 drone_future = m_coords.getPosition() + drone_velocity * tfuture;
	Vector3 future_escort_position = current_escort_position + escort_velocity * tfuture;

	
	double future_distance = (future_escort_position - drone_future).getNorm();
	//cout << "future distance: " << future_distance << endl;

	Vector3 relative_velocity;
	relative_velocity = calculateDesiredRelativeVelocity(future_escort_position,
							drone_future, future_distance, 
							0.0, acceleration);
	//cout << "relative velocity: " << relative_velocity << endl;
	return relative_velocity;
}

Vector3 Drone::findWorldEscortPosition(Entity* space_ship) {
	Entity ship = *space_ship;
	ObjLibrary::Vector3 position;
	ship.getCoordinateSystem().addPosition(ship.getForward() * relative_escort_coords.x);
	ship.getCoordinateSystem().addPosition(ship.getUp() * relative_escort_coords.y);
	ship.getCoordinateSystem().addPosition(ship.getRight() * relative_escort_coords.z);
	position = ship.getPosition();

	return position;
}
//pursue steering behaviour
Vector3 Drone::pursuitSteering(Entity* crystal, double acceleration, double delta_time) {
	//going to be similar to escort but no offset and final speed is based on crystal's current speed
	
	//find current crystal position coordinates
	Vector3 current_position = crystal->getPosition();

	Vector3 crystal_velocity = crystal->getVelocity();
	double crystal_speed = crystal_velocity.getNorm();
	double final_speed = crystal_speed + (crystal_speed * 0.1);
	Vector3 drone_velocity = m_velocity;

	double distance_current = (current_position - m_coords.getPosition()).getNorm();
	double tfuture = calculateArrivalTime(final_speed, distance_current, acceleration);
	//cout << "Drone: " << drone_color << endl;
	//cout << "tfuture: " << tfuture << endl;
	Vector3 drone_future = m_coords.getPosition() + drone_velocity * tfuture;
	Vector3 future_position = current_position + crystal_velocity * tfuture;


	double future_distance = (future_position - drone_future).getNorm();
	//cout << "future distance: " << future_distance << endl;

	Vector3 relative_velocity;
	relative_velocity = calculateDesiredRelativeVelocity(future_position,
		drone_future, future_distance,
		0.0, acceleration);
	//cout << "relative velocity: " << relative_velocity << endl;
	return relative_velocity;
}
//avoid steering behaviour
Vector3 Drone::avoidSteering(Entity* asteroid, double acceleration, double delta_time) {
	Vector3 desired_velocity;
	Vector3 asteroid_position = asteroid->getPosition();
	Vector3 drone_position = m_coords.getPosition();
	double safe_distance = calculateMinSafeDistance(asteroid->getRadius(), m_velocity, asteroid->getVelocity(), acceleration);
	//TODO: check distance against asteroid vs ship not drone
	double drone_distance = (asteroid_position - drone_position).getNorm();
	double ship_distance = (player->getPosition() - drone_position).getNorm();

	//check if asteroid is too close
	if (ship_distance < safe_distance) {
		//find steering vector to avoid asteroid
		Vector3 direction = (asteroid_position - drone_position) / drone_distance;

		Vector3 steering_vector = -direction * acceleration * delta_time;

		desired_velocity = m_velocity + steering_vector;
	}
	else {
		//if not return current velocity and maintane previous ai state
		if (target_crystal != -1) {
			ai_state = 1;
		}
		else {
			ai_state = 0;
		}
		desired_velocity = m_velocity;
	}
	
	return desired_velocity;
}
//handle drone AI
void Drone::handleAI(Entity* target, double delta_time) {
	//calculate desired velocity
	Vector3 desired_velocity = calculateOverallDesiredVelocity(target, delta_time);
	//fire engines if needed
	steerToDesiredVelocity(desired_velocity, delta_time);
}

void Drone::displayAI(Entity* target, double delta_time) {
	//if in escort mode
	if (ai_state == 0) {
		//display current escort position
		Vector3 position;
		position = findWorldEscortPosition(target);

		displayPoint(position);

		//display future escort position
		Vector3 future_escort_position = target->getVelocity();
		double distance_current = (position - m_coords.getPosition()).getNorm();
		double tfuture = calculateArrivalTime(0.0, distance_current, manouvering_engine_acceleration_rate);

		future_escort_position = position + future_escort_position * tfuture;

		displayPoint(future_escort_position);
	}
	// if pursuing a crystal
	else if (ai_state == 1) {
		//display marker on current crystal location
		Vector3 position = target->getPosition();
		displayPoint(position);

		//find future crystal position
		double distance_current = (position - m_coords.getPosition()).getNorm();
		//setting chosen final speed to current speed of the crystal
		double tfuture = calculateArrivalTime(target->getVelocity().getNorm(), distance_current, manouvering_engine_acceleration_rate);
		
		//display marker on future crystal location
		Vector3 future_postion = position + target->getVelocity() * tfuture;
		displayPoint(future_postion);
	}	
	else if(ai_state == 2){

		Vector3 position = target->getPosition();
		double safe_distance = calculateMinSafeDistance(target->getRadius(), m_velocity, target->getVelocity(), main_engine_acceleration_rate);
		//display radius of too close to asteroid
		drawMinSafeDistance(position, safe_distance);
	}
}

void Drone::drawMinSafeDistance(Vector3 asteroid_position, double safe_distance) {
	static const unsigned int MARKERS_PER_ARC = 10;
	const double PI = 3.1415926535897932384626433832795;
	const double TWO_PI = PI * 2.0;
	const double HALF_PI = PI * 0.5;

	displayPoint(asteroid_position, Vector3::UNIT_X_PLUS, safe_distance);
	displayPoint(asteroid_position, Vector3::UNIT_X_MINUS, safe_distance);
	displayPoint(asteroid_position, Vector3::UNIT_Y_PLUS, safe_distance);
	displayPoint(asteroid_position, Vector3::UNIT_Y_MINUS, safe_distance);
	displayPoint(asteroid_position, Vector3::UNIT_Z_PLUS, safe_distance);
	displayPoint(asteroid_position, Vector3::UNIT_Z_MINUS, safe_distance);

	for (unsigned int m = 1; m < MARKERS_PER_ARC; m++)
	{
		double radians1 = m * HALF_PI / MARKERS_PER_ARC;
		double radians2 = radians1 + HALF_PI;
		double radians3 = radians2 + HALF_PI;
		double radians4 = radians3 + HALF_PI;
		
		displayPoint(asteroid_position, Vector3::UNIT_X_PLUS.getRotatedY(radians1), safe_distance);
		displayPoint(asteroid_position, Vector3::UNIT_X_PLUS.getRotatedY(radians2), safe_distance);
		displayPoint(asteroid_position, Vector3::UNIT_X_PLUS.getRotatedY(radians3), safe_distance);
		displayPoint(asteroid_position, Vector3::UNIT_X_PLUS.getRotatedY(radians4), safe_distance);

		displayPoint(asteroid_position, Vector3::UNIT_Y_PLUS.getRotatedZ(radians1), safe_distance);
		displayPoint(asteroid_position, Vector3::UNIT_Y_PLUS.getRotatedZ(radians2), safe_distance);
		displayPoint(asteroid_position, Vector3::UNIT_Y_PLUS.getRotatedZ(radians3), safe_distance);
		displayPoint(asteroid_position, Vector3::UNIT_Y_PLUS.getRotatedZ(radians4), safe_distance);

		displayPoint(asteroid_position, Vector3::UNIT_Z_PLUS.getRotatedX(radians1), safe_distance);
		displayPoint(asteroid_position, Vector3::UNIT_Z_PLUS.getRotatedX(radians2), safe_distance);
		displayPoint(asteroid_position, Vector3::UNIT_Z_PLUS.getRotatedX(radians3), safe_distance);
		displayPoint(asteroid_position, Vector3::UNIT_Z_PLUS.getRotatedX(radians4), safe_distance);
	}
}
void Drone::displayPoint(const Vector3& position, const Vector3& direction, double safe_distance) {
	glPushMatrix();
	glColor3d(drone_color.x, drone_color.y, drone_color.z);
	glTranslated(position.x, position.y, position.z);
	glTranslated(direction.x * safe_distance, direction.y * safe_distance, direction.z * safe_distance);
	glScaled(2.5, 2.5, 2.5);
	glutWireOctahedron();
	glPopMatrix();
}

void Drone::displayPoint(const Vector3& position) {
	glPushMatrix();
		glColor3d(drone_color.x, drone_color.y, drone_color.z);
		glTranslated(position.x, position.y, position.z);
		glScaled(2.5, 2.5, 2.5);
		glutWireOctahedron();
	glPopMatrix();
}

void Drone::setAI(int state) {
	ai_state = state;
}

int Drone::getAIState() {
	return ai_state;
}

void Drone::setTargetCrystal(int crystal) {
	target_crystal = crystal;
}
int Drone::getTargetCrystal() {
	return target_crystal;
}

void Drone::setTargetAsteroid(int asteroid) {
	target_asteroid = asteroid;
}
int Drone::getTargetAsteroid() {
	return target_asteroid;
}
//check if alive
bool Drone::isAlive() {
	return is_alive;
}

void Drone::markDead() {
	is_alive = false;
}

void Drone::drawPath(const Entity& black_hole,
	unsigned int point_count) const {

	Drone future = *this;

	glBegin(GL_LINE_STRIP);
	glColor3d(drone_color.x, drone_color.y, drone_color.z);
	glVertex3d(future.getPosition().x, future.getPosition().y, future.getPosition().z);

	for (unsigned int i = 1; i < point_count; i++)
	{
		double distance = black_hole.getPosition().getDistance(getPosition());
		double delta_time = sqrt(distance) / 25.0;

		future.updatePhysics(delta_time, black_hole);

		double fraction = sqrt(1.0 - (double)(i) / point_count);
		glColor3d(drone_color.x * fraction, drone_color.y * fraction, drone_color.z * fraction);
		glVertex3d(future.getPosition().x, future.getPosition().y, future.getPosition().z);
	}
	glEnd();
}

ObjLibrary::Vector3 Drone::getColor() {
	return drone_color;
}