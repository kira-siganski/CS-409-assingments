//
//  BlackHole.cpp
//

#include "BlackHole.h"

#include <cassert>

#include "GetGlut.h"

#include "ObjLibrary/Vector3.h"

#include "CoordinateSystem.h"
#include "Entity.h"

using namespace ObjLibrary;



BlackHole :: BlackHole ()
		: Entity()
		, m_disk_radius(0.0)
{
	assert(!isInitialized());
}

BlackHole :: BlackHole (const ObjLibrary::Vector3& position,
                        double mass,
                        double sphere_radius,
                        double disk_radius,
                        const ObjLibrary::DisplayList& disk_display_list)
		: Entity(position, Vector3(0, 0, 0), mass, sphere_radius, disk_display_list, disk_radius)
		, m_disk_radius(disk_radius)
{
	assert(mass > 0.0);
	assert(sphere_radius >= 0.0);
	assert(disk_radius   >= 0.0);
	assert(disk_display_list.isReady());

	assert(isInitialized());
}



void BlackHole :: draw () const
{
	assert(isInitialized());

	glPushMatrix();
		m_coords.applyDrawTransformations();
		glColor3f(0.0f, 0.0f, 0.0f);
		glutSolidSphere(getRadius(), 40, 30);
	glPopMatrix();

	// draw accretion disk - has to be last because of transparency
	Entity::draw();
}

