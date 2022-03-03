//
//  Collisions.h
//
//  A module to handle collisions between Entities.
//

#pragma once

class Entity;
class Asteroid;



//
//  Collisions
//
//  A namespace to handle collisions between Entities.  Checks
//    are posssible between any combination of generic Entities
//    and Asteroids.  Entities are treated a spheres, but
//    Asteroids use their true shape.
//
namespace Collisions
{
//
//  isCollision
//
//  Purpose: To determine if the specified two Entitys collide.
//           This function variant just checks whether the
//           bounding spheres intersect.
//  Parameter(s):
//    <1> entity1: The first Entity
//    <2> entity2: The second Entity
//  Preconditions: N/A
//  Returns: Whether entity1 and entity2 are currently
//           colliding.
//  Side Effect: N/A
//
bool isCollision (const Entity& entity1,
                  const Entity& entity2);

//
//  isCollision
//
//  Purpose: To determine if the specified two Entitys collide.
//           These function variants check the true shape of
//           Asteroids.
//  Parameter(s):
//    <1> entity1: The first Entity
//    <2> entity2: The second Entity
//  Preconditions: N/A
//  Returns: Whether entity1 and entity2 are currently
//           colliding.
//  Side Effect: N/A
//
bool isCollision (const Asteroid& asteroid,
                  const Entity& entity);
bool isCollision (const Entity& entity,
                  const Asteroid& asteroid);
bool isCollision (const Asteroid& asteroid1,
                  const Asteroid& asteroid2);

//
//  bounceOff
//
//  Purpose: To cause the specifed small Entity to bounce off
//           the specified large Entity.  The large Entity's
//           velocity is not affected.
//  Parameter(s):
//    <1> bounced: The small Entity
//    <2> unaffected: The large Entity
//  Preconditions: N/A
//  Returns: N/A
//  Side Effect: Entity bounced bounces off Entity unaffected,
//               taking into account unaffected's velocity.  If
//               bounced was already heading away from
//               unaffected, its velocity is unchanged. Entity
//               unaffected is always unchanged.
//               
//
void bounceOff (Entity& bounced,
                Entity& unaffected);

//
//  elastic
//
//  Purpose: To handle an elastic collision between the
//           specified Entitys.
//  Parameter(s):
//    <1> entity1: The large Entity
//    <2> entity2: The small Entity
//  Preconditions: N/A
//  Returns: N/A
//  Side Effect: If entity1 and entity2 are moving towards each
//               other, an elastic collision occurs between
//               them.
//               
//
void elastic (Entity& entity1,
              Entity& entity2);

}  // end of namespace Collisions
