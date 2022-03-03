//
//  CoordinateSystem.cpp
//

#include <cassert>

#include "GetGlut.h"
#include "ObjLibrary/Vector3.h"

#include "CoordinateSystem.h"

using namespace ObjLibrary;



CoordinateSystem :: CoordinateSystem ()
		: m_position(0, 0, 0)
		, m_forward(1, 0, 0)
		, m_up     (0, 1, 0)
		, m_right  (0, 0, 1)
{
	assert(invariant());
}

CoordinateSystem :: CoordinateSystem (const ObjLibrary::Vector3& position)
		: m_position(position)
		, m_forward(1, 0, 0)
		, m_up     (0, 1, 0)
		, m_right  (0, 0, 1)
{
	assert(invariant());
}

CoordinateSystem :: CoordinateSystem (const ObjLibrary::Vector3& position,
                                      const ObjLibrary::Vector3& forward,
                                      const ObjLibrary::Vector3& up)
		: m_position(position)
		, m_forward(forward)
		, m_up     (up)
		, m_right  (forward.crossProduct(up))
{
	assert(forward.isNormal());
	assert(up     .isNormal());
	assert(forward.isOrthogonalNormal(up));

	assert(invariant());
}



ObjLibrary::Vector3 CoordinateSystem :: localToWorld (const ObjLibrary::Vector3& local) const
{
	return m_forward * local.x +
	       m_up      * local.y +
	       m_right   * local.z;
}

ObjLibrary::Vector3 CoordinateSystem :: worldToLocal (const ObjLibrary::Vector3& world) const
{
	//
	//  https://math.stackexchange.com/q/1897915
	//
	//  The usual thing to undo a matrix multiplication is you multiply with
	//    the inverse matrix. But if a,b,c are orthonormal, then the matrix
	//    is called orthogonal and the inverse of that matrix is just its
	//    transpose: a matrix whose columns are a,b,c.
	//

	Vector3 row1(m_forward.x, m_up.x, m_right.x);
	Vector3 row2(m_forward.y, m_up.y, m_right.y);
	Vector3 row3(m_forward.z, m_up.z, m_right.z);

	return row1 * world.x +
	       row2 * world.y +
	       row3 * world.z;
}

void CoordinateSystem :: calculateOrientationMatrix (double a_matrix[]) const
{
	assert(a_matrix != nullptr);

	a_matrix[0]  = m_forward.x;
	a_matrix[1]  = m_forward.y;
	a_matrix[2]  = m_forward.z;
	a_matrix[3]  = 0.0;
	a_matrix[4]  = m_up.x;
	a_matrix[5]  = m_up.y;
	a_matrix[6]  = m_up.z;
	a_matrix[7]  = 0.0;
	a_matrix[8]  = m_right.x;
	a_matrix[9]  = m_right.y;
	a_matrix[10] = m_right.z;
	a_matrix[11] = 0.0;
	a_matrix[12] = 0.0;
	a_matrix[13] = 0.0;
	a_matrix[14] = 0.0;
	a_matrix[15] = 1.0;
}

void CoordinateSystem :: applyDrawTransformations () const
{
	glTranslated(m_position.x, m_position.y, m_position.z);

	double a_matrix[16];
	calculateOrientationMatrix(a_matrix);
	glMultMatrixd(a_matrix);
}

void CoordinateSystem :: setupCamera () const
{
	Vector3 look_at = m_position + m_forward;
	gluLookAt(m_position.x, m_position.y, m_position.z,
	             look_at.x,    look_at.y,    look_at.z,
	                m_up.x,       m_up.y,       m_up.z);
}



void CoordinateSystem :: setPosition (const Vector3& position)
{
	m_position = position;

	assert(invariant());
}

void CoordinateSystem :: addPosition (const ObjLibrary::Vector3& delta_position)
{
	m_position += delta_position;

	assert(invariant());
}

void CoordinateSystem :: setOrientation (const Vector3& forward,
                                         const Vector3& up)
{
	assert(forward.isNormal());
	assert(up     .isNormal());
	assert(forward.isOrthogonalNormal(up));

	m_forward = forward;
	m_up      = up;
	m_right   = forward.crossProduct(up);

	assert(invariant());
}

void CoordinateSystem :: moveForward (double distance)
{
	m_position += m_forward * distance;

	assert(invariant());
}

void CoordinateSystem :: moveUp (double distance)
{
	m_position += m_up * distance;

	assert(invariant());
}

void CoordinateSystem :: moveRight (double distance)
{
	m_position += m_right * distance;

	assert(invariant());
}

void CoordinateSystem :: rotateAroundForward (double radians)
{
	m_up   .rotateArbitrary(m_forward, radians);
	m_right.rotateArbitrary(m_forward, radians);

	assert(invariant());
}

void CoordinateSystem :: rotateAroundUp (double radians)
{
	m_forward.rotateArbitrary(m_up, radians);
	m_right  .rotateArbitrary(m_up, radians);

	assert(invariant());
}

void CoordinateSystem :: rotateAroundRight (double radians)
{
	m_forward.rotateArbitrary(m_right, radians);
	m_up     .rotateArbitrary(m_right, radians);

	assert(invariant());
}

void CoordinateSystem :: rotateAroundArbitrary (const ObjLibrary::Vector3& axis,
                                                double radians)
{
	if(axis.isZero())
		return;

	Vector3 axis_normal = axis.getNormalized();
	assert(axis_normal.isNormal());

	m_forward.rotateArbitraryNormal(axis_normal, radians);
	m_up     .rotateArbitraryNormal(axis_normal, radians);
	m_right  .rotateArbitraryNormal(axis_normal, radians);

	assert(invariant());
}

void CoordinateSystem :: rotateToVector (const Vector3& target_forward,
                                         double max_radians)
{
	if(target_forward.isZero())
		return;

	Vector3 axis = m_forward.crossProduct(target_forward);
	if(axis.isZero())
		axis = m_up;
	else
		axis.normalize();
	assert(axis.isNormal());

	double radians = m_forward.getAngleSafe(target_forward);
	if(radians > max_radians)
		radians = max_radians;

	m_forward.rotateArbitraryNormal(axis, radians);
	m_up     .rotateArbitraryNormal(axis, radians);
	m_right  .rotateArbitraryNormal(axis, radians);

	assert(invariant());
}



bool CoordinateSystem :: invariant () const
{
	if(!m_forward.isNormal()) return false;
	if(!m_up     .isNormal()) return false;
	if(!m_right  .isNormal()) return false;
	return true;
}
