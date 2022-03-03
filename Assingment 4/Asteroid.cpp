//
//  Asteroid.cpp
//

#include "Asteroid.h"

#include <cassert>
#include <cmath>
#include <algorithm>  // for min/max

#include "GetGlut.h"
#include "ObjLibrary/Vector3.h"
#include "ObjLibrary/ObjModel.h"
#include "ObjLibrary/DisplayList.h"

#include "CoordinateSystem.h"
#include "PerlinNoiseField3.h"
#include "Entity.h"

using namespace ObjLibrary;
namespace
{
	const double PI     = 3.1415926535897932384626433832795;
	const double TWO_PI = PI * 2.0;
	const double DENSITY = 2710.0;  // kg / m^3 (type S asteroid)
	const double ROTATION_RATE_MAX = 0.25;  // radians / second

	const double NOISE_AMPLITUDE  = 1.0;
	const double NOISE_OFFSET_MAX = 1.0e4;
	const PerlinNoiseField3 NOISE(0.6f, (float)(NOISE_AMPLITUDE));



	double random01 ()
	{
		return rand() / (RAND_MAX + 1.0);
	}

}  // end of anonymous namespace



bool Asteroid :: isUnitSphere (const ObjLibrary::ObjModel& base_model)
{
	static const double TOLERANCE = 1.0e-3;

	for(unsigned int v = 0; v < base_model.getVertexCount(); v++)
	{
		const Vector3& vertex = base_model.getVertexPosition(v);

		if(fabs(vertex.getNormSquared() - 1.0) > TOLERANCE)
			return false;
		assert(!vertex.isZero());
	}
	return true;
}

double Asteroid :: calculateMass (double inner_radius,
                                  double outer_radius)
{
	assert(inner_radius >= 0.0);
	assert(inner_radius <= outer_radius);

	return PI * outer_radius * outer_radius * inner_radius * DENSITY / 6.0;
}

ObjLibrary::DisplayList Asteroid :: createDisplayList (const ObjLibrary::ObjModel& base_model,
                                                       double inner_radius,
                                                       double outer_radius,
                                                       ObjLibrary::Vector3 random_noise_offset)
{
	assert(isUnitSphere(base_model));

	ObjModel model = base_model;
	double radius_average    = (outer_radius + inner_radius) * 0.5;
	double radius_half_range = (outer_radius - inner_radius) * 0.5;

	for(unsigned int v = 0; v < model.getVertexCount(); v++)
	{
		const Vector3& old_vertex = model.getVertexPosition(v);
		assert(!old_vertex.isZero());
		//assert(old_vertex.isUnit());  // tolerances are too tight, so skip

		Vector3 offset_vertex = old_vertex + random_noise_offset;
		double noise = NOISE.perlinNoise((float)(offset_vertex.x),
		                                 (float)(offset_vertex.y),
		                                 (float)(offset_vertex.z));
		assert(noise >= -1.0);
		assert(noise <=  1.0);

		double new_radius = radius_average + noise * radius_half_range;
		Vector3 new_vertex = old_vertex.getCopyWithNorm(new_radius);
		model.setVertexPosition(v, new_vertex);
	}

	// don't check invariant in helper function
	return model.getDisplayList();
}



Asteroid :: Asteroid ()
		: Entity()
		, m_inner_radius(0.0)
		, m_random_noise_offset()
		, m_rotation_axis(Vector3(1.0, 0.0, 0.0))
		, m_rotation_rate(0.0)
{
	assert(!isInitialized());
	assert(invariant());
}

static Vector3 g_noise_offset;  // to copy value out of parameter into member field initialized after
Asteroid :: Asteroid (const ObjLibrary::Vector3& position,
                      const ObjLibrary::Vector3& velocity,
                      double inner_radius,
                      double outer_radius,
                      const ObjLibrary::ObjModel& base_model)
		: Entity(position,
		         velocity,
		         calculateMass(inner_radius, outer_radius),
		         outer_radius,
		         createDisplayList(base_model,
		                           inner_radius,
		                           outer_radius,
		                           g_noise_offset = Vector3::getRandomSphereVector() * NOISE_OFFSET_MAX),
		         1.0)
		, m_inner_radius(inner_radius)
		, m_random_noise_offset(g_noise_offset)  // copy from value set above
		, m_rotation_axis(Vector3::getRandomUnitVector())
		, m_rotation_rate(std::min(random01(), random01()) * ROTATION_RATE_MAX)  // mostly rotate slowly
{
	assert(inner_radius >= 0.0);
	assert(inner_radius <= outer_radius);
	assert(isUnitSphere(base_model));

	// rotate randomly
	m_coords.rotateAroundForward(random01() * TWO_PI);
	m_coords.rotateAroundUp     (random01() * TWO_PI);
	m_coords.rotateAroundRight  (random01() * TWO_PI);
	m_coords.rotateAroundForward(random01() * TWO_PI);
	m_coords.rotateAroundUp     (random01() * TWO_PI);
	m_coords.rotateAroundRight  (random01() * TWO_PI);

	assert(isInitialized());
	assert(invariant());
}



void Asteroid :: drawAxes (double length) const
{
	assert(isInitialized());

	glPushMatrix();
		m_coords.applyDrawTransformations();

		glBegin(GL_LINES);
			glColor3d(1.0, 0.0, 0.0);
			glVertex3d(0.0, 0.0, 0.0);
			glVertex3d(length, 0.0, 0.0);
			glColor3d(0.0, 1.0, 0.0);
			glVertex3d(0.0, 0.0, 0.0);
			glVertex3d(0.0, length, 0.0);
			glColor3d(0.0, 0.0, 1.0);
			glVertex3d(0.0, 0.0, 0.0);
			glVertex3d(0.0, 0.0, length);
		glEnd();
	glPopMatrix();
}



void Asteroid :: updatePhysics (double delta_time,
                                const Entity& black_hole)
{
	assert(isInitialized());
	assert(delta_time > 0.0);

	Entity::updatePhysics(delta_time, black_hole);

	double rotation_radians = m_rotation_rate * delta_time;
	m_coords.rotateAroundArbitrary(m_rotation_axis, rotation_radians);

	assert(invariant());
}



bool Asteroid :: invariant () const
{
	if(m_inner_radius < 0.0) return false;
	if(m_inner_radius > getRadius()) return false;
	if(!m_rotation_axis.isUnit()) return false;
	if(m_rotation_rate < 0.0) return false;
	return true;
}
