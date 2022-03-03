//
//  Spaceship.h
//
//  A module to represent a spaceship.
//

#include "Spaceship.h"

#include <cassert>
#include <cmath>

#include "GetGlut.h"
#include "ObjLibrary/Vector3.h"
#include "ObjLibrary/DisplayList.h"

#include "CoordinateSystem.h"
#include "Entity.h"

using namespace ObjLibrary;



Spaceship :: Spaceship ()
		: Entity()
		, m_is_alive(false)
		, m_acceleration_main(1.0)
		, m_acceleration_manoeuver(1.0)
		, m_rotation_rate_radians(1.0)
{
	assert(!isInitialized());
	assert(invariant());
}

Spaceship :: Spaceship (const ObjLibrary::Vector3& position,
                        const ObjLibrary::Vector3& velocity,
                        double mass,
                        double radius,
                        double acceleration_main,
                        double acceleration_manoeuver,
                        double rotation_rate_radians,
                        const ObjLibrary::DisplayList& display_list)
		: Entity(position, velocity, mass, radius, display_list, radius)
		, m_is_alive(true)
		, m_acceleration_main(acceleration_main)
		, m_acceleration_manoeuver(acceleration_manoeuver)
		, m_rotation_rate_radians(rotation_rate_radians)
{
	assert(mass                   >  0.0);
	assert(radius                 >= 0.0);
	assert(acceleration_main      >  0.0);
	assert(acceleration_manoeuver >  0.0);
	assert(rotation_rate_radians  >  0.0);
	assert(display_list.isReady());

	assert(isInitialized());
	assert(invariant());
}



Vector3 Spaceship :: getFollowCameraPosition (double back_distance,
                                              double up_distance) const
{
	assert(isInitialized());

	CoordinateSystem camera = m_coords;
	camera.addPosition(camera.getForward() * -back_distance);
	camera.addPosition(camera.getUp()      *  up_distance);
	return camera.getPosition();
}

void Spaceship :: setupFollowCamera (double back_distance,
                                     double up_distance) const
{
	assert(isInitialized());

	CoordinateSystem camera = m_coords;
	camera.addPosition(camera.getForward() * -back_distance);
	camera.addPosition(camera.getUp() * up_distance);
	camera.setupCamera();
}

void Spaceship :: drawPath (const Entity& black_hole,
                            unsigned int point_count,
                            const ObjLibrary::Vector3& colour) const
{
	assert(isInitialized());

	Spaceship future = *this;

	glBegin(GL_LINE_STRIP);
		glColor3d(colour.x, colour.y, colour.z);
		glVertex3d(future.getPosition().x, future.getPosition().y, future.getPosition().z);

		for(unsigned int i = 1; i < point_count; i++)
		{
			double distance   = black_hole.getPosition().getDistance(getPosition());
			double delta_time = sqrt(distance) / 25.0;

			future.updatePhysics(delta_time, black_hole);

			double fraction = sqrt(1.0 - (double)(i) / point_count);
			glColor3d(colour.x * fraction, colour.y * fraction, colour.z * fraction);
			glVertex3d(future.getPosition().x, future.getPosition().y, future.getPosition().z);
		}
	glEnd();
}



void Spaceship :: markDead ()
{
	m_is_alive = false;

	assert(invariant());
}

void Spaceship :: thrustMainEngine (double delta_time)
{
	assert(isInitialized());
	assert(delta_time >= 0.0);

	assert(m_coords.getForward().isUnit());
	m_velocity += m_coords.getForward() * m_acceleration_main * delta_time;

	assert(invariant());
}

void Spaceship :: thrustManoeuver (double delta_time,
                                   const Vector3& direction_world)
{
	assert(isInitialized());
	assert(delta_time >= 0.0);
	assert(direction_world.isUnit());

	m_velocity += direction_world * m_acceleration_manoeuver * delta_time;

	assert(invariant());
}

void Spaceship :: rotateAroundForward (double delta_time,
                                       bool is_backwards)
{
	assert(delta_time >= 0.0);

	double max_radians = m_rotation_rate_radians * delta_time;
	if(is_backwards)
		m_coords.rotateAroundForward(-max_radians);
	else
		m_coords.rotateAroundForward(max_radians);

	assert(invariant());
}

void Spaceship :: rotateAroundUp (double delta_time,
                                  bool is_backwards)
{
	assert(delta_time >= 0.0);

	double max_radians = m_rotation_rate_radians * delta_time;
	if(is_backwards)
		m_coords.rotateAroundUp(-max_radians);
	else
		m_coords.rotateAroundUp(max_radians);

	assert(invariant());
}

void Spaceship :: rotateAroundRight (double delta_time,
                                     bool is_backwards)
{
	assert(delta_time >= 0.0);

	double max_radians = m_rotation_rate_radians * delta_time;
	if(is_backwards)
		m_coords.rotateAroundRight(-max_radians);
	else
		m_coords.rotateAroundRight(max_radians);

	assert(invariant());
}



bool Spaceship :: invariant () const
{
	if(m_acceleration_main      <= 0.0) return false;
	if(m_acceleration_manoeuver <= 0.0) return false;
	if(m_rotation_rate_radians  <= 0.0) return false;
	return true;
}

