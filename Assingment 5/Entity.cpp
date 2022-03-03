//
//  Entity.cpp
//

#include "Entity.h"

#include <cassert>

#include "GetGlut.h"
#include "ObjLibrary/Vector3.h"
#include "ObjLibrary/DisplayList.h"

#include "Gravity.h"
#include "CoordinateSystem.h"

using namespace ObjLibrary;



Entity :: Entity ()
		: m_coords()
		, m_velocity()
		, m_mass(1.0)
		, m_radius(0.0)
		, m_display_list()
		, m_scaling_factor(1.0)
{
	assert(!isInitialized());
	assert(invariant());
}

Entity :: Entity (const ObjLibrary::Vector3& position,
                  const ObjLibrary::Vector3& velocity,
                  double mass,
                  double radius,
                  const ObjLibrary::DisplayList& display_list,
                  double scaling_factor)
		: m_coords(position)
		, m_velocity(velocity)
		, m_mass(mass)
		, m_radius(radius)
		, m_display_list(display_list)
		, m_scaling_factor(scaling_factor)
{
	assert(mass   >= 0.0);
	assert(radius >= 0.0);
	assert(display_list.isReady());
	assert(scaling_factor >= 0.0);

	assert(isInitialized());
	assert(invariant());
}



void Entity :: draw () const
{
	assert(isInitialized());

	glPushMatrix();
		m_coords.applyDrawTransformations();
		glScaled(m_scaling_factor, m_scaling_factor, m_scaling_factor);
		assert(m_display_list.isReady());
		m_display_list.draw();
	glPopMatrix();
}



void Entity :: setVelocity (const ObjLibrary::Vector3& velocity)
{
	assert(isInitialized());

	m_velocity = velocity;

	assert(invariant());
}

void Entity :: addVelocity (const ObjLibrary::Vector3& delta)
{
	assert(isInitialized());

	m_velocity += delta;

	assert(invariant());
}

void Entity :: updatePhysics (double delta_time,
                              const Entity& black_hole)
{
	assert(isInitialized());
	assert(delta_time > 0.0);

	Vector3 old_position = m_coords.getPosition();

	// apply black hole gravity
	const Vector3& black_hole_position = black_hole.getPosition();
	Vector3 vector_to_black_hole = black_hole_position - old_position;
	if(!vector_to_black_hole.isZero())
	{
		double distance_squared = old_position.getDistanceSquared(black_hole_position);
		assert(distance_squared > 0.0);

		double magnitude = GRAVITY * black_hole.getMass() / distance_squared;
		Vector3 acceleration = vector_to_black_hole.getCopyWithNorm(magnitude);
		m_velocity += acceleration * delta_time;
	}

	// move according to velocity
	Vector3 new_position = old_position + m_velocity * delta_time;
	m_coords.setPosition(new_position);
/*
	// rotate based on motion around black hole
	Vector3 new_vector_to_black_hole = black_hole_position - new_position;
	Vector3 axis = vector_to_black_hole.crossProduct(new_vector_to_black_hole);
	if(!axis.isZero())
	{
		double radians = vector_to_black_hole.getAngleSafe(new_vector_to_black_hole);
		m_coords.rotateAroundArbitrary(axis, radians);
	}
*/
	assert(invariant());
}



bool Entity :: invariant () const
{
	if(m_mass <= 0.0) return false;
	if(m_radius < 0.0) return false;
	if(m_display_list.isPartial()) return false;
	if(m_scaling_factor <= 0.0) return false;
	return true;
}
