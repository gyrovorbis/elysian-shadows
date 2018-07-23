#include <math.h>

#include "../api/gyro_kinetic_physics.h"
#include "../api/gyro_kinetic_contact.h"
#include "../api/gyro_kinetic_collider.h"
#include "../api/gyro_kinetic_kinetic.h"

static GYVector3 _gravityForce = { 0.0f, 0.0f, 0.0f };

void gyGravityForceSet(GYVector3* force) {
    _gravityForce = *force;
}

//void gyIntegrate(void *collider, struct _GYKinetic *kin, const struct _GYStatic *stat);

void gyIntegrate(struct _GYKinetic *kin,
         const struct _GYStatic *attrib,
         const float deltaTime,
         GYVector3* pos,
         float* orient)
{

  //Calculate linear acceleration with force input
  kin->prevAccel.x = kin->accel.x;
    kin->prevAccel.y = kin->accel.y;// - kin->accel.z;
  kin->prevAccel.z = kin->accel.z;

    GYVector3 newForce;
    gyVec3Scale(&newForce, &kin->forceAccum, attrib->inverseMass);

    //Don't apply gravity this frame if body is just sitting here chilling... for stability.
   // if(!(kin->resting && newForce.x == _gravityForce.x && newForce.y == _gravityForce.y && newForce.z == _gravityForce.z)) {

        gyVec3AddScaled(&kin->prevAccel, &kin->forceAccum, attrib->inverseMass);

        //Calculate angular acceleration from torque input
        kin->angAccel = kin->torqueAccum * attrib->invMomOfInertia;

        //Update linear velocity from linear acceleration (and impulse?)
        gyVec3AddScaled(&kin->vel, &kin->prevAccel, deltaTime);
        //Update rotation from angular acceleration (and impulsive torque?)
        kin->rot += kin->angAccel * deltaTime;

  //  } else {
   //     printf("TROLOLOLO");
   // }

     GYVector3 dampFactors;
     dampFactors.x = powf(attrib->damping.x, deltaTime);
     dampFactors.y = (attrib->damping.y == attrib->damping.x)? dampFactors.x :
                                                                powf(attrib->damping.y, deltaTime);
     dampFactors.z = (attrib->damping.z == attrib->damping.x)? dampFactors.x :
                                                                powf(attrib->damping.z, deltaTime);
     const float angDampFactor = powf(attrib->angDamping, deltaTime);



  //Damp (impose drag) DO NOT DAMP THE Z AXIS!!!
  //gyVec3ScaleAccum(&kin->vel, attrib->damping);
    gyVec3CompProdAccum(&kin->vel, &dampFactors);
    kin->rot *= angDampFactor;

  //Update position from linear velocity
    GYVector3 vel = { kin->vel.x, kin->vel.y/*-kin->vel.z*/, kin->vel.z };
    gyVec3AddScaled(pos, &vel, deltaTime);

  //Update orientation from rotation
  *orient += kin->rot * deltaTime;

    gyVec3CompProdAccum(&kin->vel, &dampFactors);
    kin->rot *= angDampFactor;

  //IMPOSE DRAG AGAIN?
  //Calculate Derived Data

  //Clear force accumulators
  kin->forceAccum.x = kin->forceAccum.y = kin->forceAccum.z = 0.0f;
  kin->torqueAccum = 0.0f;

}


//Optimize for nonrigid bodies, so rotational math isn't performed
//Make support only one body for terrain
void gyResolvePenetration(struct _GYContact *con, GYVector2 posDelta[], GYVector2 rotDir[], float orientDelta[]/*, float deltaTime*/) {
//	(void)deltaTime;
  GYVector2 velPerImp;
  float linearMove[2], angularMove[2];
  float linearInertia[2], angularInertia[2];

  float angularLimit = 0.01f;
  float totalInertia = 0;

  float rotPerImp;

  //Angular Inertia (in the direction of the contact normal)
  rotPerImp = gyVec2ScalarCrossProd(&con->objPos[0], &con->normal);
  rotPerImp *= con->staticAttrib[0]->invMomOfInertia;
  gyVec2VectorCrossProd(&velPerImp, &con->objPos[0], rotPerImp);
  angularInertia[0] = gyVec2DotProd(&velPerImp, &con->normal);

  if(con->staticAttrib[1]) {
    rotPerImp = gyVec2ScalarCrossProd(&con->objPos[1], &con->normal);
    rotPerImp *= con->staticAttrib[1]->invMomOfInertia;
    gyVec2VectorCrossProd(&velPerImp, &con->objPos[1], rotPerImp);
    angularInertia[1] = gyVec2DotProd(&velPerImp, &con->normal);
  }
  else angularInertia[1] = 0.0f;

  //Linear Inertia
  linearInertia[0] = con->staticAttrib[0]->inverseMass;
  linearInertia[1] = (con->staticAttrib[1])? con->staticAttrib[1]->inverseMass : 0.0f;

#if 0
  printf("linearInertia[0] - %f\n", linearInertia[0]);
  printf("angularInertia[0] - %f\n", angularInertia[0]);
  printf("linearInertia[1] - %f\n", linearInertia[1]);
  printf("angularInertia[1] - %f\n", angularInertia[1]);
#endif

  //Total Inertia
  totalInertia = linearInertia[0] + angularInertia[0] + linearInertia[1] + angularInertia[1];

  if(totalInertia == 0.0f) return; //nothing here can move
  totalInertia = 1.0f/totalInertia;

  //printf("totalInertia - %f\n", totalInertia);

  //Components of total movement
  //Note that teh signs of the angularMotion are reversed!
  linearMove[0] = con->depth * linearInertia[0] * totalInertia;
  angularMove[0] = -con->depth * angularInertia[0] * totalInertia;
  linearMove[1] = -con->depth * linearInertia[1] * totalInertia;
  angularMove[1] = con->depth * angularInertia[1] * totalInertia;

  //Excessive rotation check
  for(unsigned i = 0; i < 2; ++i) {
    if(con->staticAttrib[i]) {
      float max = angularLimit * gyVec2Magnitude(&con->objPos[i]);
      //float max = angularLimit * gyVec2DotProd
            if(fabs(angularMove[i]) > max) {
        float totalMotion = angularMove[i] + linearMove[i];
        angularMove[i] = (angularMove[i] > 0)? max : -max;
        linearMove[i] = totalMotion - angularMove[i];
      }
    }
  }

#if 0
  _gyLog(GY_DEBUG_LOG_VERBOSE, "linearMove[0] - %f\n", linearMove[0]);
  _gyLog(GY_DEBUG_LOG_VERBOSE, "angularMove[0] - %f\n", angularMove[0]);
  _gyLog(GY_DEBUG_LOG_VERBOSE, "linearMove[1] - %f\n", linearMove[1]);
  _gyLog(GY_DEBUG_LOG_VERBOSE, "angularMove[1] - %f\n", angularMove[1]);
#endif


  //Calculate orientation change to give us the desired linear delta from angular movement
  rotPerImp = gyVec2ScalarCrossProd(&con->objPos[0], &con->normal);
  rotPerImp *= con->staticAttrib[0]->invMomOfInertia;
  //printf("rotPerImp[0] - %f\n", rotPerImp);
  orientDelta[0] = (angularInertia[0] != 0.0f && angularMove[0] != 0.0f)? rotPerImp / angularInertia[0] * angularMove[0] : 0.0f;

  if(con->staticAttrib[1]) {
    rotPerImp = gyVec2ScalarCrossProd(&con->objPos[1], &con->normal);
    rotPerImp *= con->staticAttrib[1]->invMomOfInertia;
    //printf("rotPerImp[1] - %f\n", rotPerImp);
    orientDelta[1] = (angularInertia[1] != 0.0f && angularMove[1] != 0.0f)? rotPerImp / angularInertia[1] * angularMove[1] : 0.0f;
  }
  else orientDelta[1] = 0.0f;

  //Apply movements
  gyVec2Scale(&posDelta[0], &con->normal, linearMove[0]);
  //Can't apply them here, because that's just the collider coordinates!
  //gyVec2AddAccum(&con->collider[0]->transform._pos, &posDelta[0]);
  //con->collider[0]->transform._orientZ += orientDelta[0];

  if(con->staticAttrib[1]) {
    gyVec2Scale(&posDelta[1], &con->normal, linearMove[1]);
    //gyVec2AddAccum(&con->collider[1]->transform._pos, &posDelta[1]);
    //con->collider[1]->transform._orientZ += orientDelta[1];
  }
  else gyVec2Clear(&posDelta[1]);

  //printf("posDelta[0] - <%f, %f>\n", posDelta[0].x, posDelta[0].y);
  //printf("orientDelta[0] - %f\n", orientDelta[0]);
  //printf("posDelta[1] - <%f, %f>\n", posDelta[1].x, posDelta[1].y);
  //printf("orientDelta[1] - %f\n", orientDelta[1]);

  //Calculate rotation directions for penetration depth adjustment
  //this is never even fucking used... not relevant in 3D?
  rotDir[0].x = con->objPos[0].y * con->staticAttrib[0]->invMomOfInertia;
  rotDir[0].y = -con->objPos[0].x * con->staticAttrib[0]->invMomOfInertia;

  if(con->staticAttrib[1]) {
    rotDir[1].x = con->objPos[1].y * con->staticAttrib[1]->invMomOfInertia;
    rotDir[1].y = -con->objPos[1].x * con->staticAttrib[1]->invMomOfInertia;
  }
  else {
    rotDir[1].x = 0.0f;
    rotDir[1].y = 0.0f;
  }
}



void gyContactResolveVelocity(struct _GYContact *con, GYVector2 velDelta[], float rotDelta[]/*, const float deltaTime*/) {
//	(void)deltaTime;
  GYVector2 velPerImp;
  GYVector2 impulse;
  float rotPerImp;
  float impulseMag;
  float totalDeltaVelPerImp; //overall change in velocity of both objects per unit impulse

//BODY 1 FACTORS

  //totalDeltaVelPerImp = _attribPool[_partPool[_colPool[_conPool[index]._colIndex[0]].partIndex]._attribIndex]._inverseMass;
  totalDeltaVelPerImp = con->staticAttrib[0]->inverseMass;

  /*
  rotPerImp = gyroVector2ScalarCrossProd(&_conPool[index]._relContactPos[0], &_conPool[index]._normal);
  rotPerImp *= _attribPool[_partPool[_colPool[_conPool[index]._colIndex[0]].partIndex]._attribIndex]._invMomOfInertia;
  gyroVector2VectorCrossProd(&velPerImp, &_conPool[index]._relContactPos[0], rotPerImp);
  totalDeltaVelPerImp += gyroVector2DotProd(&velPerImp, &_conPool[index]._normal);
  */

  rotPerImp = gyVec2ScalarCrossProd(&con->objPos[0], &con->normal);
  rotPerImp *= con->staticAttrib[0]->invMomOfInertia;
  gyVec2VectorCrossProd(&velPerImp, &con->objPos[0], rotPerImp);
  totalDeltaVelPerImp += gyVec2DotProd(&velPerImp, &con->normal);

//BODY 2 FACTORS

  if(con->staticAttrib[1]) {
    totalDeltaVelPerImp += con->staticAttrib[1]->inverseMass;

    rotPerImp = gyVec2ScalarCrossProd(&con->objPos[1], &con->normal);
    rotPerImp *= con->staticAttrib[1]->invMomOfInertia;
    gyVec2VectorCrossProd(&velPerImp, &con->objPos[1], rotPerImp);
    totalDeltaVelPerImp += gyVec2DotProd(&velPerImp, &con->normal);

  }

//	printf("TotalDeltaVelPerImp - %f\n", totalDeltaVelPerImp);

  if(totalDeltaVelPerImp == 0.0f) return; //nothing can move

  //IMPULSE CALCULATION
  //impulseMag = _conPool[index]._deltaVelocity / totalDeltaVelPerImp;
  //gyroVector2Scale(&impulse, &_conPool[index]._normal, impulseMag);
  impulseMag = con->deltaVel / totalDeltaVelPerImp;
  gyVec2Scale(&impulse, &con->normal, impulseMag);


//	printf("impulseMag - %f\n", impulseMag);
//	printf("impulse - <%f, %f>\n", impulse.x, impulse.y);

  //CALCULATE RESULTING VELOCITY DELTAS BODY 1
/*	gyroVector2Scale(&velDelta[0], &impulse, _attribPool[_partPool[_colPool[_conPool[index]._colIndex[0]].partIndex]._attribIndex]._inverseMass);
  rotDelta[0] = gyroVector2ScalarCrossProd(&impulse, &_conPool[index]._relContactPos[0]);
  rotDelta[0] *= _attribPool[_partPool[_colPool[_conPool[index]._colIndex[0]].partIndex]._attribIndex]._invMomOfInertia;*/

  gyVec2Scale(&velDelta[0], &impulse, con->staticAttrib[0]->inverseMass);
  rotDelta[0] = gyVec2ScalarCrossProd(&impulse, &con->objPos[0]);
  rotDelta[0] *= con->staticAttrib[0]->invMomOfInertia;

  if(con->staticAttrib[1]) {
    gyVec2ScaleAccum(&impulse, -1.0f);

  //CALCULATE RESULTING VELOCITY DELTAS BODY 2
    gyVec2Scale(&velDelta[1], &impulse, con->staticAttrib[1]->inverseMass);
    rotDelta[1] = gyVec2ScalarCrossProd(&impulse, &con->objPos[1]);
    rotDelta[1] *= con->staticAttrib[1]->invMomOfInertia;
  }


  //APPLY RESULTING VELOCITY DELTAS
  //gyroVector2AddTo(&_partPool[_colPool[_conPool[index]._colIndex[0]].partIndex]._velocity, &velDelta[0]);
  //_partPool[_colPool[_conPool[index]._colIndex[0]].partIndex]._rotation += rotDelta[0];
  //gyroVector2AddTo(&_partPool[_colPool[_conPool[index]._colIndex[1]].partIndex]._velocity, &velDelta[1]);
  //_partPool[_colPool[_conPool[index]._colIndex[1]].partIndex]._rotation += rotDelta[1];

}


//needs to know:
//worldPos[2];
//velocity[0]
//rotation

void gyContactCalculatePenetrationInternals(GYContact*const contact, const GYVector2*const pos1, const GYVector2*const pos2) {

  //printf("normal - <%f, %f>\n", _conPool[index]._normal.x, _conPool[index]._normal.y);
  //printf("pen depth - %f\n", _conPool[index]._depth);

  gyVec2Sub(&contact->objPos[0], &contact->worldPos, pos1);
  if(pos2) gyVec2Sub(&contact->objPos[1], &contact->worldPos, pos2);

  //printf("relContactPos[0] - <%f, %f>\n", _conPool[index]._relContactPos[0].x, _conPool[index]._relContactPos[0].y);
  //printf("relContactPos[1] - <%f, %f>\n", _conPool[index]._relContactPos[1].x, _conPool[index]._relContactPos[1].y);

  //printf("Coeff of Rest - %f\n", _conPool[index]._coeffOfRestitution);
}


void gyContactCalculateVelocityInternals(GYContact*const contact, GYKinetic*const rb1, GYKinetic*const rb2, const float deltaTime) {
  GYVector2 totalVelocity[2];

  gyVec2VectorCrossProd(&totalVelocity[0], &contact->objPos[0], rb1->rot);
  //printf("body1->rotVel - <%f, %f>\n", totalVelocity[0].x, totalVelocity[0].y);
  gyVec2AddAccum(&totalVelocity[0], (GYVector2*)&rb1->vel);

  //WEIRD CORRECTIVE SHIT
  //Add velocity from acceleration NOT IN THE DIRECTION OF THE NORMAL
  //GYVector2 accVelocity;
  //GYVector2 normalVel;
//	gyVec2Scale(&accVelocity, &rb1->prevAccel, deltaTime);
  //gyVec2Scale(&normalVel, &contact->normal, gyVec2DotProd(&accVelocity, &contact->normal));
  //gyVec2SubFrom(&accVelocity, &normalVel);
  //gyVec2AddAccum(&totalVelocity[0], &accVelocity);

  if(rb2) {
    gyVec2VectorCrossProd(&totalVelocity[1], &contact->objPos[1], rb2->rot);
    //printf("body2->rotVel - <%f, %f>\n", totalVelocity[1].x, totalVelocity[1].y);
    gyVec2AddAccum(&totalVelocity[1], (GYVector2*)&rb2->vel);

    //gyVec2Scale(&accVelocity, &rb2->prevAccel, deltaTime);
    //gyVec2Scale(&normalVel, &contact->normal, gyVec2DotProd(&accVelocity, &contact->normal));
    //gyVec2SubFrom(&accVelocity, &normalVel);
    //gyVec2AddAccum(&totalVelocity[1], &accVelocity);


  } else {
    totalVelocity[1].x = totalVelocity[1].y = 0.0f;
  }

  //printf("Body1->vel = <%f, %f>\n", totalVelocity[0].x, totalVelocity[0].y);
  //printf("Body2->vel = <%f, %f>\n", totalVelocity[1].x, totalVelocity[1].y);

  gyVec2SubFrom(&totalVelocity[0], &totalVelocity[1]);
  //gyVec2AddAccum(&totalVelocity[0], &totalVelocity[1]);
  //printf("Relative vel - <%f, %f>\n", totalVelocity[0].x, totalVelocity[0].y);
  contact->closingVel = gyVec2DotProd(&totalVelocity[0], &contact->normal);

  //printf("Separating vel - %f\n", _conPool[index]._closingVelocity);

  contact->coeffOfRest = contact->staticAttrib[0]->coeffOfRest;
  if(rb2) contact->coeffOfRest *= contact->staticAttrib[1]->coeffOfRest;

  gyContactUpdateDeltaVelocity(contact, rb1, rb2, deltaTime);
}

//Do actual ADJUSTMENT of closing velocity in update loop
//This could just as easily not be its own function anymore and be done in velocityResolution as with the linear case
void gyContactUpdateDeltaVelocity(GYContact*const contact, GYKinetic*const rb1, GYKinetic*const rb2, const float deltaTime) {
  float velocityFromAccel;
  //float newRestitution = contact->coeffOfRest;

  velocityFromAccel = gyVec2DotProd((GYVector2*)&rb1->prevAccel, &contact->normal) * deltaTime;

  if(rb2) {
    velocityFromAccel -= gyVec2DotProd((GYVector2*)&rb2->prevAccel, &contact->normal) * deltaTime;
  }

  //===========NEED AN EPSILON IN THE FUTURE ====
    contact->coeffOfRest = (fabs(contact->closingVel) < 0.1f)? 0.0f : contact->coeffOfRest;

  contact->deltaVel = -contact->closingVel -
    contact->coeffOfRest * (contact->closingVel - velocityFromAccel);


  //contact->deltaVel = -contact->closingVel * (1.0f + newRestitution);

  //contact->deltaVel = -contact->closingVel*newRestitution -contact->closingVel;
  //printf("DeltaVel - %f\n", _conPool[index]._deltaVelocity);

}
