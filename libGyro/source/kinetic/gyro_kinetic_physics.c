#include "kinetic/gyro_kinetic_physics.h"
#include "kinetic/gyro_kinetic_contact.h"
#include "kinetic/gyro_kinetic_collider.h"

//Optimize for nonrigid bodies, so rotational math isn't performed
//Make support only one body for terrain
void gyResolvePenetration(struct _GYContact *con, GYVector2 posDelta[], GYVector2 rotDir[], float orientDelta[], float deltaTime) {
	GYVector2 velPerImp;
	float linearMove[2], angularMove[2];
	float linearInertia[2], angularInertia[2];
	
	float angularLimit = 1000.0f;
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
	else angularInertia[1] = 0;

	//Linear Inertia
	linearInertia[0] = con->staticAttrib[0]->inverseMass;
	if(con->staticAttrib[1])
		linearInertia[1] = con->staticAttrib[1]->inverseMass;
	else linearInertia[1] = 0.0f;

#if 0
	printf("linearInertia[0] - %f\n", linearInertia[0]);
	printf("angularInertia[0] - %f\n", angularInertia[0]);
	printf("linearInertia[1] - %f\n", linearInertia[1]);
	printf("angularInertia[1] - %f\n", angularInertia[1]);
#endif 

	//Total Inertia
	totalInertia = linearInertia[0] + angularInertia[0] + linearInertia[1] + angularInertia[1];
	totalInertia = 1.0f/totalInertia;
	
	if(totalInertia == 0.0f) return; //nothing here can move
	
	//printf("totalInertia - %f\n", totalInertia);
	
	//Components of total movement
	//Note that teh signs of the angularMotion are reversed!
	linearMove[0] = con->depth * linearInertia[0] * totalInertia;
	angularMove[0] = -con->depth * angularInertia[0] * totalInertia;
	linearMove[1] = -con->depth * linearInertia[1] * totalInertia;
	angularMove[1] = con->depth * angularInertia[1] * totalInertia;
	
	//printf("linearMove[0] - %f\n", linearMove[0]);
	//printf("angularMove[0] - %f\n", angularMove[0]);
	//printf("linearMove[1] - %f\n", linearMove[1]);
	//printf("angularMove[1] - %f\n", angularMove[1]);
	
	//Excessive rotation check
	
	//Calculate orientation change to give us the desired linear delta from angular movement
	rotPerImp = gyVec2ScalarCrossProd(&con->objPos[0], &con->normal);
	rotPerImp *= con->staticAttrib[0]->invMomOfInertia;
	//printf("rotPerImp[0] - %f\n", rotPerImp);
	orientDelta[0] = (angularInertia[0] != 0.0f)? rotPerImp / angularInertia[0] * angularMove[0] : 0.0f;
	
	if(con->staticAttrib[1]) {
		rotPerImp = gyVec2ScalarCrossProd(&con->objPos[1], &con->normal);
		rotPerImp *= con->staticAttrib[1]->invMomOfInertia;
		//printf("rotPerImp[1] - %f\n", rotPerImp);
		orientDelta[1] = (angularInertia[1] != 0.0f)? rotPerImp / angularInertia[1] * angularMove[1] : 0.0f;
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
