//
//  Asteroid.h
//
//  A module to represent an asteroid.
//

#pragma once

#include <cassert>

#include "ObjLibrary/Vector3.h"
#include "ObjLibrary/ObjModel.h"
#include "ObjLibrary/DisplayList.h"

#include "CoordinateSystem.h"
#include "Entity.h"



//
//  Asteroid
//
//  A class to represent an asteroid.  All asteroids are
//    irregularly shaped, with a size defined by the (outer)
//    collision radius and an inner radius.  At all points, the
//    distance from the asteroid surface to its origin falls
//    between these two values.  Note that it is normal for no
//    part of the surface to ever reach either radii.
//
//  A base ObjModel is used to produce the asteroid model.  The
//    base model must be a unit sphere and should have materials
//    set.  The vertexes of the model will be moved, but the
//    materials and the mesh structure will be unchanged.  Using
//    a higher-polygon sphere for the base model will produce a
//    higher-polygon asteroid.
//
//  Class Invariant:
//    <1> m_inner_radius >= 0.0
//    <2> m_inner_radius <= getRadius()
//    <3> m_rotation_axis.isUnit()
//    <4> m_rotation_rate >= 0.0
//
class Asteroid : public Entity
{
public:
//
//  Class Function: isUnitSphere
//
//  Purpose: To determine if the specified ObjModel is a unit
//           sphere.
//  Parameter(s): N/A
//    <1> base_model: The ObjModel to check
//  Preconditions: N/A
//  Returns: Whether the position of every vertex in model
//           base_model is a unit vector.  If base_mode contains
//           no vertexes, this funciton returns true.
//  Side Effect: N/A
//
	static bool isUnitSphere (const ObjLibrary::ObjModel& base_model);

//
//  Class Function: calculateMass
//
//  Purpose: To determine the mass for an Asteroid with the
//           specified inner and outer radii.
//  Parameter(s):
//    <1> inner_radius: The inner asteroid radius
//    <2> outer_radius: The outer asteroid radius
//  Preconditions:
//    <1> inner_radius >= 0.0
//    <2> inner_radius <= outer_radius
//  Returns: The radius of the asteroid.
//  Side Effect: N/A
//
	static double calculateMass (double inner_radius,
	                             double outer_radius);

//
//  Class Function: createDisplayList
//
//  Purpose: To create the DisplayList for this Asteroid.
//  Parameter(s):
//    <1> base_model: The base ObjModel that wil be modified to
//                    produce the asteroid
//    <2> inner_radius: The inner asteroid radius
//    <3> outer_radius: The outer asteroid radius
//    <4> random_noise_offset: The offset for the Perlin noise
//  Preconditions:
//    <1> isUnitSphere(base_model)
//  Returns: A DisplayList based on base_model.  The vertexes
//           are positioned based on Perlin noise and the inner
//           and outer radii.
//  Side Effect: N/A
//
	static ObjLibrary::DisplayList createDisplayList (
	                   const ObjLibrary::ObjModel& base_model,
	                   double inner_radius,
	                   double outer_radius,
	                   ObjLibrary::Vector3 random_noise_offset);

public:
//
//  Default Constructor
//
//  Purpose: To create an Asteroid without initializing it.
//  Parameter(s): N/A
//  Preconditions: N/A
//  Returns: N/A
//  Side Effect: A new Asteroid is created.  It is not
//               initialized.
//
	Asteroid ();

//
//  Constructor
//
//  Purpose: To create a random asteroid with the specified
//           position, inner and out radii, and base model file.
//  Parameter(s):
//    <1> position: The position of the asteroid origin
//    <2> velocity: The velocity of the asteroid
//    <3> inner_radius: The inner asteroid radius
//    <4> outer_radius: The outer asteroid radius
//    <5> base_model: The base ObjModel that wil be modified to
//                    produce the asteroid
//  Preconditions: N/A
//    <1> inner_radius >= 0.0
//    <2> inner_radius <= outer_radius
//    <3> isUnitSphere(base_model)
//  Returns: N/A
//  Side Effect: A new Asteroid is created at position position
//               with velocity velocity.  It has a random mesh
//               based on model base_model and with its surface
//               radius always in the interval
//               [inner_radius, outer_radius].  The new Asteroid
//               has a random orientation and rotational
//               velocity.
//
	Asteroid (const ObjLibrary::Vector3& position,
	          const ObjLibrary::Vector3& velocity,
	          double inner_radius,
	          double outer_radius,
	          const ObjLibrary::ObjModel& base_model);

	Asteroid (const Asteroid& to_copy) = default;
	~Asteroid () = default;
	Asteroid& operator= (const Asteroid& to_copy) = default;

//
//  getRadiusForDirection
//
//  Purpose: To determine the surface radius of this Asteroid in
//           the specified direction.  The direction is
//           specified in world space coordinates from the
//           Asteroid origin.
//  Parameter(s):
//    <1> direction: The direction to measure the surface in
//  Preconditions:
//    <1> isInitialized()
//    <2> direction.isUnit()
//  Returns: The distance from the Asteroid origin to its
//           surface in direction direction.
//  Side Effect: N/A
//
	double getRadiusForDirection (
	                const ObjLibrary::Vector3& direction) const;

//
//  isCrystals
//
//  Purpose: To determine if there are crystals on this Asteroid
//           available to knock off.
//  Parameter(s): N/A
//  Preconditions:
//    <1> isInitialized()
//  Returns: Whether it is possible for the player to knock
//           crystals off this Asteroid.
//  Side Effect: N/A
//
	bool isCrystals () const
	{
		assert(isInitialized());

		return m_is_crystals;
	}

//
//  drawAxes
//
//  Purpose: To display the XYZ axes of the local coordinate
//           system for this Asteroid.
//  Parameter(s):
//    <1> length: The length of the axes
//  Preconditions:
//    <1> isInitialized()
//    <2> length >= 0.0
//  Returns: N/A
//  Side Effect: The current orientation of this Asteroid is
//               displayed.
//
	void drawAxes (double length) const;

//
//  drawSurfaceEquators
//
//  Purpose: To display circles of markers showing the distance
//           to the surface of this Asteroid along the XY, YZ,
//           and ZX planes.  The [planes are in world
//           coordinates.
//  Parameter(s): N/A
//  Preconditions:
//    <1> isInitialized()
//  Returns: N/A
//  Side Effect: Markers are displayed showing the collision
//               surface of this Asteroid.
//
	void drawSurfaceEquators () const;

//
//  removeCrystals
//
//  Purpose: To mark this Asteroid as not having any crystals to
//           knock off.
//  Parameter(s): N/A
//  Preconditions: N/A
//  Returns: N/A
//  Side Effect: The Asteroid is marked as having already had
//               all its crystals knocked off.
//
	void removeCrystals ();

//
//  updatePhysics
//
//  Purpose: To perform the physics updates for this Entity
//           for one time step.
//  Parameter(s):
//    <1> delta_time: The length of the time step in seconds
//    <2> black_hole: The black hole
//  Preconditions:
//    <1> isInitialized()
//    <2> delta_time > 0.0
//  Returns: N/A
//  Side Effect: This Entity is updated for one time step.  This
//               includes rotation.
//
	virtual void updatePhysics (double delta_time,
	                            const Entity& black_hole);

private:
//
//  drawSurfaceMarker
//
//  Purpose: To display a markers showing the distance to the
//           surface of this Asteroid in the specified
//           direction.
//  Parameter(s):
//    <1> direction: The direction to the marker in world
//                   coordinates
//    <2> colour: The marker colour
//  Preconditions:
//    <1> isInitialized()
//    <2> direction.isUnit()
//  Returns: N/A
//  Side Effect: A marker is displayed showing the distance to
//               the collision surface of this Asteroid in
//               direction direction.
//
	void drawSurfaceMarker (
	                   const ObjLibrary::Vector3& direction,
	                   const ObjLibrary::Vector3& colour) const;

//
//  invariant
//
//  Purpose: To determine whether the class invariant is true.
//  Parameter(s): N/A
//  Preconditions: N/A
//  Returns: Whether the class invariant is true.
//  Side Effect: N/A
//
	bool invariant () const;

private:
	double m_inner_radius;
	ObjLibrary::Vector3 m_random_noise_offset;
	ObjLibrary::Vector3 m_rotation_axis;
	double m_rotation_rate;
	bool m_is_crystals;
};


