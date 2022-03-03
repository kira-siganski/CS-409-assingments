#pragma once
#include "Spaceship.h"

#include <cmath>

#include "GetGlut.h"
#include "ObjLibrary/Vector3.h"
#include "ObjLibrary/ObjModel.h"
#include "ObjLibrary/DisplayList.h"

#include "CoordinateSystem.h"
#include "Entity.h"

using namespace ObjLibrary;

Spaceship::Spaceship() {
	mass = 1000;
}

Spaceship::Spaceship(ObjLibrary::DisplayList display_list) {
	mass = 1000;
	m_display_list = display_list;
}

void Spaceship::setPosition(ObjLibrary::Vector3& position) {
	m_coords.setPosition(position);
}

void Spaceship::setVelocity(ObjLibrary::Vector3& new_velocity) {
	velocity = new_velocity;
}

//rotate around up using coordinate system
void Spaceship::rotateAroundUp(double radians) {
	m_coords.rotateAroundUp(radians);
}
//rotate around right using coordinate system
void Spaceship::rotateAroundRight(double radians) {
	m_coords.rotateAroundRight(radians);
}
//rotate around forward using coordinate system
void Spaceship::rotateAroundForward(double radians) {
	m_coords.rotateAroundForward(radians);
}

void Spaceship::accelerateForward(double distance) {
	//we want to to find the acceleration vector
	//forward * distance
	double delta_time = 1.0 / 60.0;
	//distance is magnitude, direction is forward vector
	ObjLibrary::Vector3 acceleration(m_coords.getForward() * distance);

	velocity = velocity + acceleration * delta_time;

	//now we move
	m_coords.moveForward(distance);
	//m_coords.setPosition(m_coords.getPosition() + velocity);
}

void Spaceship::accelerateUp(double distance) {
	//we want to to find the acceleration vector
	//forward * distance
	double delta_time = 1.0 / 60.0;
	//distance is magnitude, direction is forward vector
	ObjLibrary::Vector3 acceleration(m_coords.getUp() * distance);

	velocity = velocity + acceleration * delta_time;

	//now we move
	m_coords.moveUp(distance);
	//m_coords.setPosition(m_coords.getPosition() + velocity);
}

void Spaceship::accelerateRight(double distance) {
	//we want to to find the acceleration vector
	//forward * distance
	double delta_time = 1.0 / 60.0;
	//distance is magnitude, direction is forward vector
	ObjLibrary::Vector3 acceleration(m_coords.getRight() * distance);

	velocity = velocity + acceleration * delta_time;

	//now we move
	m_coords.moveRight(distance);
	//m_coords.setPosition(m_coords.getPosition() + velocity);
}

void Spaceship::accelerateArbitrary(ObjLibrary::Vector3& direction) {
	double delta_time = 1.0 / 60.0;

	velocity = velocity + direction * delta_time;

	m_coords.setPosition(m_coords.getPosition() + velocity);
}


void Spaceship::draw() {
	//if our position is the camera, then always move the spaceship then draw it
	glPushMatrix();
		m_coords.applyDrawTransformations();
		glScaled(4.0, 4.0, 4.0);
		m_display_list.draw();
	glPopMatrix();
}

void Spaceship::setUpCamera() {
	//camera needs to be 20m behind ship and 5m above it
	Vector3 look_at = m_coords.getPosition() + m_coords.getForward();
	Vector3 pos = locateCamera();
	gluLookAt(pos.x, pos.y, pos.z,
		look_at.x, look_at.y, look_at.z,
		m_coords.getUp().x, m_coords.getUp().y, m_coords.getUp().z);
}

void Spaceship::setModel(ObjLibrary::DisplayList display_list) {
	m_display_list = display_list;
}

ObjLibrary::Vector3 Spaceship::locateCamera() {
	Vector3 pos = m_coords.getPosition() + m_coords.getForward() * -20;
	pos = pos + m_coords.getUp() * 5;

	return pos;
}

ObjLibrary::Vector3 Spaceship::getVelocity() {
	return velocity;
}