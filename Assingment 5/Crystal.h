//
//  Crystal.h
//
//  A module to represent a mineral crystal.
//

#pragma once

#include <cassert>

#include "ObjLibrary/Vector3.h"
#include "ObjLibrary/DisplayList.h"

#include "CoordinateSystem.h"
#include "Entity.h"



//
//  Crystal
//
//  A class to represent a mineral crystal.
//
//  Class Invariant:
//    <1> m_rotation_axis.isUnit()
//    <2> m_rotation_rate >= 0.0
//
class Crystal : public Entity
{
public:
//
//  Constructor
//
//  Purpose: To create an Crystal without initializing it.
//  Parameter(s): N/A
//  Preconditions: N/A
//  Returns: N/A
//  Side Effect: A new Crystal is created.  It is not
//               initialized.
//
	Crystal ();

//
//  Constructor
//
//  Purpose: To create an Crystal with the specified position,
//           velocity, and DisplayList.
//  Parameter(s):
//    <1> position: The starting position
//    <2> velocity: The starting velocity
//    <3> display_list: The DisplayList for this crystal
//  Preconditions:
//    <1> display_list.isReady()
//  Returns: N/A
//  Side Effect: A new Crystal is created at position position
//               with velocity velocity and a random orientation
//               and rotation.  It will be displayed with 
//               DisplayList display_list.
//
	Crystal (const ObjLibrary::Vector3& position,
	         const ObjLibrary::Vector3& velocity,
	         const ObjLibrary::DisplayList& display_list);

	Crystal (const Crystal& to_copy) = default;
	~Crystal () = default;
	Crystal& operator= (const Crystal& to_copy) = default;

//
//  isGone
//
//  Purpose: To determine whether this Crystal has been
//           destroyed.
//  Parameter(s): N/A
//  Preconditions:
//    <1> isInitialized()
//  Returns: Whether this Crystal has been removed from play.
//  Side Effect: N/A
//
	bool isGone () const
	{
		assert(isInitialized());

		return m_is_gone;
	}

//
//  markGone
//
//  Purpose: To mark this Crystal as having been destroyed.
//  Parameter(s): N/A
//  Preconditions:
//    <1> isInitialized()
//  Returns: N/A
//  Side Effect: This crystal is marked as having been
//               destroyed.
//
	void markGone ();

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
	ObjLibrary::Vector3 m_rotation_axis;
	double m_rotation_rate;
	bool m_is_gone;
};


