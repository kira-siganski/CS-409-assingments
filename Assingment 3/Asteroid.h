//
//  Asteroid.h
//
//  A module to represent an asteroid.
//

#pragma once

#include "ObjLibrary/Vector3.h"
#include "ObjLibrary/ObjModel.h"
#include "ObjLibrary/DisplayList.h"

#include "CoordinateSystem.h"
//for entity as parent class
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
//    <2> m_inner_radius <= m_outer_radius
//    <3> m_rotation_axis.isUnit()
//    <4> m_rotation_rate >= 0.0
//    <5> !m_display_list.isPartial()
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
//    <2> inner_radius: The inner asteroid radius
//    <3> outer_radius: The outer asteroid radius
//    <4> base_model: The base ObjModel that wil be modified to
//                    produce the asteroid
//  Preconditions: N/A
//    <1> inner_radius >= 0.0
//    <2> inner_radius <= outer_radius
//    <3> isUnitSphere(base_model)
//  Returns: N/A
//  Side Effect: A new Asteroid is created at position position.
//               It has a random mesh based on model base_model
//               and with its surface radius always in the
//               interval [inner_radius, outer_radius].  The new
//               Asteroid has a random orientation and
//               rotational velocity.
//
	Asteroid (const ObjLibrary::Vector3& position,
	          double inner_radius,
	          double outer_radius,
	          const ObjLibrary::ObjModel& base_model);

	Asteroid (const Asteroid& to_copy) = default;
	~Asteroid () = default;
	Asteroid& operator= (const Asteroid& to_copy) = default;

//
//  isInitialized
//
//  Purpose: To determine whether this Asteroid has been
//           intialized.
//  Parameter(s): N/A
//  Preconditions: N/A
//  Returns: Whether this Asteroid has been initialized.
//  Side Effect: N/A
//
	bool isInitialized () const
	{
		return m_display_list.isReady();
	}

//
//  getPosition
//
//  Purpose: To determine the original position of this
//           Asteroid.
//  Parameter(s): N/A
//  Preconditions:
//    <1> isInitialized()
//  Returns: The position of this Asteroid's origin.
//  Side Effect: N/A
//
	const ObjLibrary::Vector3& getPosition () const
	{
		return m_coords.getPosition();
	}

//
//  getRadius
//
//  Purpose: To determine the outer radius of this Asteroid.
//  Parameter(s): N/A
//  Preconditions:
//    <1> isInitialized()
//  Returns: The outer radius of this Asteroid.
//  Side Effect: N/A
//
	double getRadius () const
	{
		return m_outer_radius;
	}


	//to calculate the mass of the asteroid given 2 radius values
	double calcMass(double inner_radius, double outer_radius);
//
//  draw
//
//  Purpose: To display this Asteroid.
//  Parameter(s): N/A
//  Preconditions:
//    <1> isInitialized()
//  Returns: N/A
//  Side Effect: This Asteroid is displayed at its current
//               position with its current rotation.
//
	void draw () const;

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
//  update
//
//  Purpose: To update this Asteroid for one time step.
//  Parameter(s): N/A
//  Preconditions:
//    <1> isInitialized()
//  Returns: N/A
//  Side Effect: This Asteroid is updated for one time step.
//                This includes rotation.
//

	void update ();

private:
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
	double m_outer_radius;
	ObjLibrary::Vector3 m_rotation_axis;
	double m_rotation_rate;
	ObjLibrary::Vector3 m_random_noise_offset;
	ObjLibrary::DisplayList m_display_list;
};


