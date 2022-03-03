//
//  BlackHole.h
//
//  A module to represent the black hole.
//

#pragma once

#include "ObjLibrary/Vector3.h"
#include "ObjLibrary/DisplayList.h"

#include "CoordinateSystem.h"
#include "Entity.h"



//
//  BlackHole
//
//  A class to represent the black hole.
//
class BlackHole : public Entity
{
public:
//
//  Default Constructor
//
//  Purpose: To create an BlackHole without initializing it.
//  Parameter(s): N/A
//  Preconditions: N/A
//  Returns: N/A
//  Side Effect: A new BlackHole is created.  It is not
//               initialized.
//
	BlackHole ();

//
//  Constructor
//
//  Purpose: To create a black hole with the specified position,
//           mass, and radius.
//  Parameter(s):
//    <1> position: The position
//    <2> mass: The mass
//    <3> sphere_radius: The radius of the black hole itself
//    <4> disk_radius: The radius of the accretion disk
//    <5> disk_display_list: The DisplayList for the accretion
//                           disk
//  Preconditions: N/A
//    <1> mass > 0.0
//    <2> sphere_radius >= 0.0
//    <3> disk_radius   >= 0.0
//    <4> disk_display_list.isReady()
//  Returns: N/A
//  Side Effect: A new BlackHole is created at position position
//               with mas mas and radius radius.
//
	BlackHole (const ObjLibrary::Vector3& position,
	           double mass,
	           double sphere_radius,
	           double disk_radius,
	           const ObjLibrary::DisplayList& disk_display_list);

	BlackHole (const BlackHole& to_copy) = default;
	~BlackHole () = default;
	BlackHole& operator= (const BlackHole& to_copy) = default;

//
//  draw
//
//  Purpose: To display this BlackHole.
//  Parameter(s): N/A
//  Preconditions: N/A
//  Returns: N/A
//  Side Effect: This BlackHole is displayed at its current
//               position with its current rotation.
//
	virtual void draw () const;

private:
	double m_disk_radius;
	ObjLibrary::DisplayList m_disk_display_list;
};


