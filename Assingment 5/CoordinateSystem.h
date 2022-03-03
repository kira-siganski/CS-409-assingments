//
//  CoordinateSystem.h
//
//  A module to represent a coordinate system in 3D space.
//

#pragma once

#include "ObjLibrary/Vector3.h"



//
//  CoordinateSystem.h
//
//  A class to represent a coordinate system in 3D space.
//
//  Class Invariant:
//    <1> m_forward.isNormal()
//    <2> m_up.isNormal()
//    <3> m_right.isNormal()
//
class CoordinateSystem
{
public:
	CoordinateSystem ();
	CoordinateSystem (const ObjLibrary::Vector3& position);
	CoordinateSystem (const ObjLibrary::Vector3& position,
	                  const ObjLibrary::Vector3& forward,
	                  const ObjLibrary::Vector3& up);
	CoordinateSystem (const CoordinateSystem& to_copy) = default;
	~CoordinateSystem () = default;
	CoordinateSystem& operator= (const CoordinateSystem& to_copy) = default;

	const ObjLibrary::Vector3& getPosition () const
	{	return m_position;	}
	const ObjLibrary::Vector3& getForward () const
	{	return m_forward;	}
	const ObjLibrary::Vector3& getUp () const
	{	return m_up;	}
	const ObjLibrary::Vector3& getRight () const
	{	return m_right;	}

	ObjLibrary::Vector3 localToWorld (const ObjLibrary::Vector3& local) const;
	ObjLibrary::Vector3 worldToLocal (const ObjLibrary::Vector3& world) const;
	void calculateOrientationMatrix (double a_matrix[]) const;
	void applyDrawTransformations () const;
	void setupCamera () const;

	void setPosition (const ObjLibrary::Vector3& position);
	void addPosition (const ObjLibrary::Vector3& delta_position);
	void setOrientation (const ObjLibrary::Vector3& forward,
	                     const ObjLibrary::Vector3& up);
	void moveForward (double distance);
	void moveUp (double distance);
	void moveRight (double distance);
	void rotateAroundForward (double radians);
	void rotateAroundUp (double radians);
	void rotateAroundRight (double radians);
	void rotateAroundArbitrary (const ObjLibrary::Vector3& axis,
	                            double radians);
	void rotateToVector (const ObjLibrary::Vector3& target_forward,
	                     double max_radians);

private:
	bool invariant () const;

private:
	ObjLibrary::Vector3 m_position;
	ObjLibrary::Vector3 m_forward; 
	ObjLibrary::Vector3 m_up; 
	ObjLibrary::Vector3 m_right;
};
