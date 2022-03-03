#pragma once

#include "ObjLibrary/Vector3.h"
#include "CoordinateSystem.h"
//making this a header file to make reading these easier
class Entity {
public:
	//default constructor
	Entity();
	//borrowing from coordinate system
	Entity(const ObjLibrary::Vector3& n_position,
		const ObjLibrary::Vector3& n_forward,
		const ObjLibrary::Vector3& n_up);
	//set mass
	void setMass(double newMass);
	//set initial velocity
	void setInitialVelocity();
	//update function
	void update();
	//calculate speed
	double calcSpeed();
	//update velocity
	
protected:
	//local coordinate system
	CoordinateSystem m_coords;
	//velocity
	ObjLibrary::Vector3 velocity;
	//mass
	double mass;
};