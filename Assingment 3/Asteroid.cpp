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

#include "Entity.h"
#include "CoordinateSystem.h"
#include "PerlinNoiseField3.h"

using namespace ObjLibrary;
namespace
{
	const double PI     = 3.1415926535897932384626433832795;
	const double TWO_PI = PI * 2.0;
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
	static const double TOLERANCE  = 1.0e-3;

	for(unsigned int v = 0; v < base_model.getVertexCount(); v++)
	{
		const Vector3& vertex = base_model.getVertexPosition(v);

		if(fabs(vertex.getNormSquared() - 1.0) > TOLERANCE)
			return false;
		assert(!vertex.isZero());
	}
	return true;
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
		: m_inner_radius(0.0)
		, m_outer_radius(0.0)
		, m_rotation_axis(Vector3(1.0, 0.0, 0.0))
		, m_rotation_rate(0.0)
		, m_random_noise_offset()
		, m_display_list()
{
	assert(invariant());
}

Asteroid :: Asteroid (const ObjLibrary::Vector3& position,
                      double inner_radius,
                      double outer_radius,
                      const ObjLibrary::ObjModel& base_model)
		:   // rotated randomly below
		 m_inner_radius(inner_radius)
		, m_outer_radius(outer_radius)
		, m_rotation_axis(Vector3::getRandomUnitVector())
		, m_rotation_rate(std::min(random01(), random01()) * ROTATION_RATE_MAX)  // mostly rotate slowly
		, m_random_noise_offset(Vector3::getRandomSphereVector() * NOISE_OFFSET_MAX)
		, m_display_list(createDisplayList(base_model,
		                                   inner_radius,
		                                   outer_radius,
		                                   m_random_noise_offset))
{
	m_coords.setOrientation(Vector3::UNIT_X_PLUS, Vector3::UNIT_Y_PLUS);
	m_coords.setPosition(position);
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

	assert(invariant());

	//calculate mass
	mass = calcMass(inner_radius, outer_radius);

	setInitialVelocity();
}



void Asteroid :: draw () const
{
	assert(isInitialized());

	glPushMatrix();
		m_coords.applyDrawTransformations();
		assert(m_display_list.isReady());
		m_display_list.draw();
	glPopMatrix();
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


void Asteroid :: update()
{
	assert(isInitialized());
	Entity::update();
	double rotation_radians = m_rotation_rate / 50.0;  // fix when we have frame rate control
	m_coords.rotateAroundArbitrary(m_rotation_axis, rotation_radians);

	
	assert(invariant());
}





bool Asteroid :: invariant () const
{
	if(m_inner_radius < 0.0) return false;
	if(m_inner_radius > m_outer_radius) return false;
	if(!m_rotation_axis.isUnit()) return false;
	if(m_rotation_rate < 0.0) return false;
	if(m_display_list.isPartial()) return false;
	return true;
}

double Asteroid::calcMass(double inner_radius, double outer_radius) {
	//using formula m = (pi/6)*ro^2*ri*p, where ro is outer, ri is inner and p = 2710 kg/m^3  
	double mass = (PI / 6) * pow(outer_radius, 2) * inner_radius * 2710;

	return mass;
}