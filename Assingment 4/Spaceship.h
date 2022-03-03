//
//  Spaceship.h
//
//  A module to represent a spaceship.
//

#pragma once

#include <cassert>

#include "ObjLibrary/Vector3.h"
#include "ObjLibrary/DisplayList.h"

#include "CoordinateSystem.h"
#include "Entity.h"



//
//  Spaceship
//
//  A class to represent a spaceship.  This can be the player
//    ship or a drone.
//
//  Class Invariant:
//    <1> m_acceleration_main      > 0.0
//    <2> m_acceleration_manoeuver > 0.0
//    <3> m_rotation_rate_radians  > 0.0
//
class Spaceship : public Entity
{
public:
//
//  Constructor
//
//  Purpose: To create an Spaceship without initializing it.
//  Parameter(s): N/A
//  Preconditions: N/A
//  Returns: N/A
//  Side Effect: A new Spaceship is created.  It is not
//               initialized.
//
	Spaceship ();

//
//  Constructor
//
//  Purpose: To create an Spaceship with the specified position,
//           velocity, mass, and radius.  It will be displayed
//           with the specified DisplayList.
//  Parameter(s):
//    <1> position: The starting position
//    <2> velocity: The starting velocity
//    <3> mass: The mass when the fuel tanks are empty
//    <4> radius: The outer collision radius
//    <5> acceleration_main: The acceleration provided by the
//                           main engine
//    <6> acceleration_manoeuver: The acceleration provided by
//                                the manoeuvering engine
//    <7> rotation_rate_radians: The rotation rate in radians
//                               per second
//    <8> display_list: The DisplayList for this spaceship
//  Preconditions:
//    <1> mass                   >  0.0
//    <2> radius                 >= 0.0
//    <3> acceleration_main      >  0.0
//    <4> acceleration_manoeuver >  0.0
//    <5> rotation_rate_radians  >  0.0
//    <6> display_list.isReady()
//  Returns: N/A
//  Side Effect: A new Spaceship is created at position position
//               with velocity velocity.  It has a mass of mass
//               and a radius of radius.
//
	Spaceship (const ObjLibrary::Vector3& position,
	           const ObjLibrary::Vector3& velocity,
	           double mass,
	           double radius,
	           double acceleration_main,
	           double acceleration_manoeuver,
	           double rotation_rate_radians,
	           const ObjLibrary::DisplayList& display_list);

	Spaceship (const Spaceship& to_copy) = default;
	~Spaceship () = default;
	Spaceship& operator= (const Spaceship& to_copy) = default;

//
//  isAlive
//
//  Purpose: To determine whether this Spaceship is still alive.
//  Parameter(s): N/A
//  Preconditions:
//    <1> isInitialized()
//  Returns: Whether this Spaceship is alive.
//  Side Effect: N/A
//
	bool isAlive () const
	{
		assert(isInitialized());

		return m_is_alive;
	}

//
//  getFollowCameraPosition
//
//  Purpose: To determine the camera position to look past this
//           Spaceship from the specified relative offset.
//  Parameter(s):
//    <1> back_distance: How far behind the spaceship the camera
//                       should be
//    <2> up_distance: How far above the spaceship the camera
//                     should be
//  Preconditions:
//    <1> isInitialized()
//  Returns: The camera position.  The camera is assumed to have
//           the same orientation as this Spaceship.
//  Side Effect: N/A
//
	ObjLibrary::Vector3 getFollowCameraPosition (
	                                  double back_distance,
	                                  double up_distance) const;

//
//  setupFollowCamera
//
//  Purpose: To set up the camera position to look past this
//           Spaceship from the specified relative offset.
//  Parameter(s):
//    <1> back_distance: How far behind the spaceship the camera
//                       should be
//    <2> up_distance: How far above the spaceship the camera
//                     should be
//  Preconditions:
//    <1> isInitialized()
//  Returns: N/A
//  Side Effect: The camera is set up to look in the forward
//               direction of this spaceship from behind and
//               above it.
//
	void setupFollowCamera (double back_distance,
	                        double up_distance) const;

//
//  drawPath
//
//  Purpose: To display the path that this Spaceship will follow
//           if it is only affected by gravity from the
//           specified black hole.
//  Parameter(s):
//    <1> black_hole: The black hole
//    <2> point_count: How many points ahead to display
//    <3> colour: How colour of the path
//  Preconditions:
//    <1> isInitialized()
//  Returns: N/A
//  Side Effect: A path of point_count vertexes is displayed for
//               this Spaceship is displayed.  It will start
//               with a colour of colour and then fade to black
//               at the end.
//
	void drawPath (const Entity& black_hole,
	               unsigned int point_count,
	               const ObjLibrary::Vector3& colour) const;

//
//  markDead
//
//  Purpose: To destroy this Spaceship.  This likely happens as
//           the result of a collision.
//  Parameter(s): N/A
//  Preconditions:
//    <1> isInitialized()
//  Returns: N/A
//  Side Effect: This Spaceship is marked as dead.
//
	void markDead ();

//
//  thrustMainEngine
//
//  Purpose: To fire the main engines of this spaceship, causing
//           it to accelerate quickly forward.
//  Parameter(s):
//    <1> delta_time: The length of the time step in seconds
//  Preconditions:
//    <1> isInitialized()
//    <2> delta_time >= 0.0
//  Returns: N/A
//  Side Effect: This spaceship accelerates forward.
//
	void thrustMainEngine (double delta_time);

//
//  thrustManoeuver
//
//  Purpose: To fire the main engines of this spaceship, causing
//           it to accelerate slowly in the specified direction.
//  Parameter(s):
//    <1> delta_time: The length of the time step in seconds
//    <2> direction_world: The direction to accelerate in world
//                         coordinates
//  Preconditions:
//    <1> isInitialized()
//    <2> delta_time >= 0.0
//    <3> direction_world.isUnit()
//  Returns: N/A
//  Side Effect: This spaceship accelerates in direction
//               direction_world.
//
	void thrustManoeuver (
	                double delta_time,
	                const ObjLibrary::Vector3& direction_world);

//
//  rotateAroundForward
//  rotateAroundUp
//  rotateAroundRight
//
//  Purpose: To rotate this spaceship around the indicated axis.
//  Parameter(s):
//    <1> delta_time: The length of the time step in seconds
//    <2> is_backwards: Whether to rotate backwards
//  Preconditions:
//    <1> isInitialized()
//    <2> delta_time >= 0.0
//  Returns: N/A
//  Side Effect: This spaceship is rotated.
//
	void rotateAroundForward (double delta_time,
	                          bool is_backwards);
	void rotateAroundUp (double delta_time,
	                     bool is_backwards);
	void rotateAroundRight (double delta_time,
	                        bool is_backwards);

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
	bool m_is_alive;
	double m_acceleration_main;
	double m_acceleration_manoeuver;
	double m_rotation_rate_radians;
};


