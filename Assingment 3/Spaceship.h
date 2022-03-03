#pragma once
#include "Entity.h"
#include "ObjLibrary/Vector3.h"
#include "ObjLibrary/ObjModel.h"
#include "ObjLibrary/DisplayList.h"
#include "CoordinateSystem.h"

class Spaceship : public Entity {
public:
	Spaceship();
	Spaceship(ObjLibrary::DisplayList display_list);
	void setPosition(ObjLibrary::Vector3& position);
	void setVelocity(ObjLibrary::Vector3& new_velocity);
	//rotate around up
	void rotateAroundUp(double radians);
	//rotate around right
	void rotateAroundRight(double radians);
	//rotate around forward
	void rotateAroundForward(double radians);
	//accellerate forward
	void accelerateForward(double distance);
	void accelerateUp(double distance);
	void accelerateRight(double distance);
	//accelerate to arbitrary direction
	void accelerateArbitrary(ObjLibrary::Vector3& direction);

	void draw();
	void setUpCamera();
	void setModel(ObjLibrary::DisplayList display_list);
	
	ObjLibrary::Vector3 locateCamera();
	ObjLibrary::Vector3 getVelocity();
private:
	ObjLibrary::DisplayList m_display_list;
};