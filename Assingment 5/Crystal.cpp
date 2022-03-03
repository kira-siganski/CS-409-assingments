//
//  Crystal.cpp
//

#include "Crystal.h"

#include <cassert>
#include <algorithm>  // for min/max

#include "GetGlut.h"
#include "ObjLibrary/Vector3.h"
#include "ObjLibrary/DisplayList.h"

#include "CoordinateSystem.h"
#include "Entity.h"

using namespace ObjLibrary;
namespace
{
	const double RADIUS = 2.0;
	const double MASS   = 1.0;
	const double ROTATION_RATE_MAX = 6.0;  // radians / second

	double random01 ()
	{
		return rand() / (RAND_MAX + 1.0);
	}

}  // end of anonymous namespace



Crystal :: Crystal ()
		: Entity()
		, m_rotation_axis(Vector3(1.0, 0.0, 0.0))
		, m_rotation_rate(0.0)
		, m_is_gone(false)
{
	assert(!isInitialized());
	assert(invariant());
}

Crystal :: Crystal (const ObjLibrary::Vector3& position,
                    const ObjLibrary::Vector3& velocity,
                    const ObjLibrary::DisplayList& display_list)
		: Entity(position,
		         velocity,
		         MASS,
		         RADIUS,
		         display_list,
		         RADIUS / 0.7)
		, m_rotation_axis(Vector3::getRandomUnitVector())
		, m_rotation_rate(std::min(random01(), random01()) * ROTATION_RATE_MAX)  // mostly rotate slowly
		, m_is_gone(false)
{
	assert(display_list.isReady());

	assert(isInitialized());
	assert(invariant());
}



void Crystal :: markGone ()
{
	assert(isInitialized());

	m_is_gone++;

	assert(invariant());
}

void Crystal :: updatePhysics (double delta_time,
                               const Entity& black_hole)
{
	assert(isInitialized());
	assert(delta_time > 0.0);

	Entity::updatePhysics(delta_time, black_hole);

	double rotation_radians = m_rotation_rate * delta_time;
	m_coords.rotateAroundArbitrary(m_rotation_axis, rotation_radians);

	assert(invariant());
}



bool Crystal :: invariant () const
{
	if(!m_rotation_axis.isUnit()) return false;
	if(m_rotation_rate < 0.0) return false;
	return true;
}
