
#include <cmath>
#include <iostream>
#include "ObjLibrary/Vector3.h"

#include "Entity.h"
#include "CoordinateSystem.h"

using namespace std;
using namespace ObjLibrary;

//borrowed from Asteroid.cpp
double random01()
{
	return rand() / (RAND_MAX + 1.0);
}

Entity :: Entity()
{
	m_coords;
	mass = 0.0;
	velocity.set(0.0, 0.0, 0.0);
}

Entity::Entity(const ObjLibrary::Vector3& position,
	const ObjLibrary::Vector3& forward,
	const ObjLibrary::Vector3& up)
{
	m_coords.addPosition(position);
	m_coords.setOrientation(forward, up);
	mass = 0.0;
	velocity.set(0.0, 0.0, 0.0);
}

void Entity::setMass(double newMass) {
	double mass = newMass;
}

void Entity::setInitialVelocity() {
	ObjLibrary::Vector3 random_vec(ObjLibrary::Vector3::getPseudorandomUnitVector(random01(), random01()));
	random_vec = random_vec.getRejectionSafe(Vector3(0.0, 0.0, 0.0));
	random_vec.normalizeSafe();

	velocity = random_vec * calcSpeed();
}

void Entity::update() {
	float delta_time = 1.0 / 60.0;
	double blackhole_mass = pow(5.0, 16.0);
	double gravity = pow(6.67408, -11.0);
	ObjLibrary::Vector3 blackhole_pos(0.0, 0.0, 0.0);
	ObjLibrary::Vector3 position(m_coords.getPosition());
	double distance_to_blackhole = position.getDistance(blackhole_pos);
	
	//calculate acceleration magnitude G * m / d^2 where g = gravity, m = black hole mass, d = distance
	float acceleration_magnitude = gravity * blackhole_mass / pow(distance_to_blackhole, 2.0);

	//calculate acceleration direction
	ObjLibrary::Vector3 direction_to_blackhole(blackhole_pos - position);
	direction_to_blackhole.normalize();

	//calculate acceleration vector = direction * magnitude
	ObjLibrary::Vector3 acceleration(direction_to_blackhole * acceleration_magnitude);

	//update velocity where new velocity is v(t + dt) = v(t) + a*dt where v(t) is original velocity and a is acceleration
	velocity = velocity + (acceleration * delta_time);

	//update position
	ObjLibrary::Vector3 new_position;

	new_position = position + velocity;
	m_coords.setPosition(new_position);
}

double Entity::calcSpeed() {
	double blackhole_mass = pow(5.0, 16);
	ObjLibrary::Vector3 blackhole_pos(0.0, 0.0, 0.0);
	ObjLibrary::Vector3 position(m_coords.getPosition());
	double distance_to_blackhole = position.getDistance(blackhole_pos);
	double gravity = pow(6.67408, -11);
	//speed is equal to root(Gravity * Mass / distance)
	double speed = sqrt(gravity * blackhole_mass / distance_to_blackhole);

	return speed;
}

