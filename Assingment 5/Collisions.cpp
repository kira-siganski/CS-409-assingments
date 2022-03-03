//
//  Collisions.h
//
//  A module to handle collisions between Entities.
//

#include "Collisions.h"

#include <cassert>

#include "ObjLibrary/Vector3.h"

#include "CoordinateSystem.h"
#include "Entity.h"
#include "Asteroid.h"

using namespace ObjLibrary;



bool Collisions :: isCollision (const Entity& entity1,
                                const Entity& entity2)
{
	double radius_sum = entity1.getRadius() + entity2.getRadius();
	return entity1.getPosition().isDistanceLessThan(entity2.getPosition(), radius_sum);
}

bool Collisions :: isCollision (const Asteroid& asteroid,
                                const Entity& entity)
{
	bool is_spheres_collide = isCollision((const Entity&)(asteroid), entity);
	if(!is_spheres_collide)
		return false;

	Vector3 difference = entity.getPosition() - asteroid.getPosition();
	assert(!difference.isZero());
	Vector3 direction = difference.getNormalized();

	double asteroid_radius = asteroid.getRadiusForDirection(direction);
	assert(asteroid_radius >= 0.0);
	double radius_sum = asteroid_radius + entity.getRadius();
	assert(radius_sum >= 0.0);
	return difference.isNormLessThan(radius_sum);
}

bool Collisions :: isCollision (const Entity& entity,
                                const Asteroid& asteroid)
{
	return isCollision(asteroid, entity);
}

bool Collisions :: isCollision (const Asteroid& asteroid1,
                                const Asteroid& asteroid2)
{
	bool is_spheres_collide = isCollision((const Entity&)(asteroid1),
	                                      (const Entity&)(asteroid2));
	if(!is_spheres_collide)
		return false;

	Vector3 difference = asteroid1.getPosition() - asteroid2.getPosition();
	assert(!difference.isZero());
	Vector3 a2_to_a1 = difference.getNormalized();

	double asteroid1_radius = asteroid1.getRadiusForDirection(-a2_to_a1);
	assert(asteroid1_radius >= 0.0);
	double asteroid2_radius = asteroid2.getRadiusForDirection(a2_to_a1);
	assert(asteroid2_radius >= 0.0);
	double radius_sum = asteroid1_radius + asteroid2_radius;
	assert(radius_sum >= 0.0);
	return difference.isNormLessThan(radius_sum);
}



void Collisions :: bounceOff (Entity& bounced,
                              Entity& unaffected)
{
	Vector3 unaffected_to_bounced = bounced.getPosition() - unaffected.getPosition();
	if(unaffected_to_bounced.isZero())
		return;  // cannot resolve, centers are at same position

	Vector3 old_velocity      = bounced.getVelocity();
	Vector3 relative_velocity = old_velocity - unaffected.getVelocity();
	Vector3 normal_velocity   = relative_velocity.getProjection(unaffected_to_bounced);

	if(unaffected_to_bounced.isSameHemisphere(normal_velocity))
		return;  // already moving apart

	Vector3 new_velocity = old_velocity - normal_velocity * 2.0;
	bounced.setVelocity(new_velocity);
}

void Collisions :: elastic (Entity& entity1,
                            Entity& entity2)
{
	Vector3 e1_to_e2 = entity2.getPosition() - entity1.getPosition();
	if(e1_to_e2.isZero())
		return;  // cannot resolve, centers are at same position

	assert(!e1_to_e2.isZero());
	Vector3 collision_normal = e1_to_e2.getNormalized();

	double mass1 = entity1.getMass();
	double mass2 = entity2.getMass();
	double mass_sum = mass1 + mass2;

	Vector3 velocity1 = entity1.getVelocity();
	Vector3 velocity2 = entity2.getVelocity();
	Vector3 momentum1 = velocity1 * mass1;
	Vector3 momentum2 = velocity2 * mass2;
	Vector3 momentum_sum = momentum1 + momentum2;

	assert(mass_sum > 0.0);
	Vector3 average_velocity = momentum_sum / mass_sum;

	Vector3 extra_velocity1 = velocity1 - average_velocity;
	Vector3 extra_velocity2 = velocity2 - average_velocity;
	Vector3 velocity_to_move1 = extra_velocity1.getProjectionNormal(collision_normal);
	Vector3 velocity_to_move2 = extra_velocity2.getProjectionNormal(collision_normal);
	Vector3 momentum_to_move1 = velocity_to_move1 * mass1;
	Vector3 momentum_to_move2 = velocity_to_move2 * mass2;

	if(momentum_to_move1.dotProduct(e1_to_e2) <= 0.0)
		return;  // don't change velocities (already moving apart)

	assert(mass1 > 0.0);
	assert(mass2 > 0.0);
	Vector3 new_velocity1 = average_velocity + momentum_to_move2 / mass1;
	Vector3 new_velocity2 = average_velocity + momentum_to_move1 / mass2;
	entity1.setVelocity(new_velocity1);
	entity2.setVelocity(new_velocity2);
}
