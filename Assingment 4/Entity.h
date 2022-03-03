//
//  Entity.h
//
//  A module to represent an entity in the world.
//

#pragma once

#include <cassert>

#include "ObjLibrary/Vector3.h"
#include "ObjLibrary/DisplayList.h"

#include "CoordinateSystem.h"



//
//  Entity
//
//  An abstract superclass for all entities in the world.  An
//    Entity has a coordinate system, a velocity, a mass, and a
//    collision radius.  The Entity must fit entirely inside the
//    collision radius, although it is not required to occupy
//    that whole volume.
//
//  Class Invariant:
//    <1> m_mass > 0.0
//    <2> m_radius >= 0.0
//    <3> !m_display_list.isPartial();
//    <4> m_scaling_factor > 0.0
//
class Entity
{
public:
//
//  Default Constructor
//
//  Purpose: To create an Entity without initializing it.
//  Parameter(s): N/A
//  Preconditions: N/A
//  Returns: N/A
//  Side Effect: A new Entity is created.  It is not
//               initialized.
//
	Entity ();

//
//  Constructor
//
//  Purpose: To create an Entity with the specified values.
//  Parameter(s):
//    <1> position: The starting position
//    <2> velocity: The starting velocity
//    <3> mass: The mass
//    <4> radius: The collision radius
//    <5> display_list: The DisplayList for this spaceship
//    <6> scaling_factor: The scaling factor for display_list
//  Preconditions: N/A
//    <1> mass   >  0.0
//    <2> radius >= 0.0
//    <3> display_list.isReady()
//    <4> scaling_factor > 0.0
//  Returns: N/A
//  Side Effect: A new Entity is created at position position
//               with velocity velocity.  It has a mass of mass
//               and a collision radius of radius.  It will be
//               displayed with DisplayList display_list,
//               uniformly scaled by scaling_factor.
//
	Entity (const ObjLibrary::Vector3& position,
	        const ObjLibrary::Vector3& velocity,
	        double mass,
	        double radius,
	        const ObjLibrary::DisplayList& display_list,
	        double scaling_factor);

	Entity (const Entity& to_copy) = default;
	~Entity () = default;
	Entity& operator= (const Entity& to_copy) = default;

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
//  Purpose: To determine the origin position of this Entity.
//  Parameter(s): N/A
//  Preconditions:
//    <1> isInitialized()
//  Returns: The position of this Entity's origin.
//  Side Effect: N/A
//
	const ObjLibrary::Vector3& getPosition () const
	{
		assert(isInitialized());

		return m_coords.getPosition();
	}

//
//  getForward
//  getUp
//  getRight
//
//  Purpose: To determine the local forward/up/right vector for
//           this Entity.
//  Parameter(s): N/A
//  Preconditions:
//    <1> isInitialized()
//  Returns: The position of this Entity's origin.
//  Side Effect: N/A
//
	const ObjLibrary::Vector3& getForward () const
	{
		assert(isInitialized());

		return m_coords.getForward();
	}
	const ObjLibrary::Vector3& getUp () const
	{
		assert(isInitialized());

		return m_coords.getUp();
	}
	const ObjLibrary::Vector3& getRight () const
	{
		assert(isInitialized());

		return m_coords.getRight();
	}

//
//  getCoordinateSystem
//
//  Purpose: To retreive a refernce to the coordinate system for
//           this Entity.  The coordinate system is const IFF
//           this Entity is.
//  Parameter(s): N/A
//  Preconditions:
//    <1> isInitialized()
//  Returns: The local coordinate system for this Entity.
//  Side Effect: N/A
//
	const CoordinateSystem& getCoordinateSystem () const
	{
		assert(isInitialized());

		return m_coords;
	}
	CoordinateSystem& getCoordinateSystem ()
	{
		assert(isInitialized());

		return m_coords;
	}

//
//  getVelocity
//
//  Purpose: To determine the velocity of this Entity.
//  Parameter(s): N/A
//  Preconditions:
//    <1> isInitialized()
//  Returns: The current velocity of this Entity.
//  Side Effect: N/A
//
	const ObjLibrary::Vector3& getVelocity () const
	{
		assert(isInitialized());

		return m_velocity;
	}

//
//  getMass
//
//  Purpose: To determine the current mass of this Entity.
//  Parameter(s): N/A
//  Preconditions:
//    <1> isInitialized()
//  Returns: The current mass of this Entity.
//  Side Effect: N/A
//
	double getMass () const
	{
		assert(isInitialized());

		return m_mass;
	}

//
//  getRadius
//
//  Purpose: To determine the collision radius of this Entity.
//  Parameter(s): N/A
//  Preconditions:
//    <1> isInitialized()
//  Returns: The collision radius of this Entity.  If the
//           collision radii of two Entities do not overlap,
//           there is not a collision.  The reverse is not
//           necessarily true.
//  Side Effect: N/A
//
	double getRadius () const
	{
		assert(isInitialized());

		return m_radius;
	}

//
//  draw
//
//  Purpose: To display this Entity.
//  Parameter(s): N/A
//  Preconditions:
//    <1> isInitialized()
//  Returns: N/A
//  Side Effect: This Entity is displayed.
//
	virtual void draw () const;

//
//  setVelocity
//
//  Purpose: To change the velocity of this Entity.
//  Parameter(s):
//    <1> velocity: The new velocity
//  Preconditions:
//    <1> isInitialized()
//  Returns: N/A
//  Side Effect: This Entity is changed to be moving at velocity
//               velocity.
//
	void setVelocity (const ObjLibrary::Vector3& velocity);

//
//  addVelocity
//
//  Purpose: To increase the velocity of this Entity by the
//           specified amount.
//  Parameter(s):
//    <1> delta: The change in velocity
//  Preconditions:
//    <1> isInitialized()
//  Returns: N/A
//  Side Effect: This velocity of this Entity is increased by
//               delta.
//
	void addVelocity (const ObjLibrary::Vector3& delta);

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
//  Side Effect: This Entity is updated for one time step.  The
//               default implementation accelerates this Entity
//               according to the gravity of black_hole and then
//               moves this Entity based on its updated
//               velocity.
//
	virtual void updatePhysics (double delta_time,
	                            const Entity& black_hole);
	//function to return extreme bounds of sphere
	double* getBounds();
	//TODO: function to handle collision
	//void handleCollision()

protected:
//
//  setMass
//
//  Purpose: To change the mass of this Entity.
//  Parameter(s):
//    <1> mass: The new mass
//  Preconditions:
//    <1> isInitialized()
//    <2> mass > 0.0
//  Returns: N/A
//  Side Effect: The mass of this Entity is changed to mass.
//
	void setMass (double mass)
	{
		assert(isInitialized());
		assert(mass > 0.0);

		m_mass = mass;

		assert(invariant());
	}

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

protected:
	CoordinateSystem m_coords;
	ObjLibrary::Vector3 m_velocity;

private:
	double m_mass;
	double m_radius;
	ObjLibrary::DisplayList m_display_list;
	double m_scaling_factor;
	double bounds[6]; //0 xmin, 1, xmax, 2, ymin, 3 ymax, 4, zmin, 5 zmax
};


