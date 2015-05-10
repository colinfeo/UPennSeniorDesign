/*
* serialapi.cpp - Defines the entry point for the console application - version 6
* 				  Computes x,y location of cursor, forces for each motor, given a net x-y force
* Created by: Colin Feo
*         on: 03/11/15
* All code available at https://github.com/colinfeo/UPennSeniorDesign
*  - Last backed up 05/10/15
* Revisions:
*  - see github repo for full list of commits
*  - Revised most recently by: Colin Feo
*  - Revised most recently on: 04/18/15
*  - Description:
*     Added decoding of button clicks which are embedded (as negative signs) in the encoder lengths.
*/


//

#include <zmq.hpp>
#include <zmq_utils.h>

#include <stdlib.h>
#include "stdafx.h"
#include "SerialClass.h"
#include <math.h>
#include <vector>

#include<iostream>

#include <Eigen\Core>

#include <Windows.h>
#include <windowsx.h>
#include <lp_lib.h>


using namespace std;

float A = 64.60f; //y distance from left pulley to right pulley
float B = 45.24f; //x distance from bottom pulley to top pulley

int screenWidth = 1920;
int screenHeight = 1080;

//x,y postiions of top aqnd bottom end effector points
float botX;
float botY;
float topX;
float topY;

//theta offset of end effector
float theta;

//x,y position of center of end effector
float X;
float Y;

//minimum force on motor
float minForce = 2.36f;
float maxForce = 10.0f;


void calculateCoordinates(float L0, float L1, float L2, float L3);
//Eigen::Matrix<float,3,4> calculateJacobian(float L0, float L1, float L2, float L3);
void testCalculateCoordinates();
void testCalculateForce();
float vectorLength(float x_0, float y_0);
vector<float> getMotorForces(float x, float y, float moment);


int _tmain(int argc, _TCHAR* argv[])
{
	//testCalculateForce();
	//testCalculateCoordinates();
	//while(true) {
	//	Sleep(5);
	//}

	//read in L1, L2, L3, L4 position from teensy
	float L0 = 0; //bottom right cord length
	float L1 = 0; //upper right cord length
	float L2 = 0; //upper left cord length
	float L3 = 0; //bottom left cord length
	//initialize serial connection
	Serial* mySerial = new Serial("COM6");
	char serialBuffTest[16];


	//int wireLengths[5] = {0};
	//initialize tcp server connection and open socket: for Unity
	zmq::context_t context (1);
	zmq::socket_t socket (context, ZMQ_SUB);
	socket.bind("tcp://*:11000");
	int hwm = 2;
	int rcvto = 0;
	socket.setsockopt(ZMQ_SUBSCRIBE,"",strlen(""));
	socket.setsockopt(ZMQ_RCVHWM,&hwm,sizeof(hwm));
	socket.setsockopt(ZMQ_RCVTIMEO,&rcvto,sizeof(rcvto));
	//socket.setsockopt(ZMQ_RCVHWM,1);

	//declare send and recieve timer values
	float sendTime = (float)timeGetTime();
	float recieveTime = (float)timeGetTime();
	int* motorForcesToSerial = new int[4];
	int oldClick = 0;
	/////////////////////
	//MAIN POLLING LOOP//
	/////////////////////
	while(true) {

		/////////////////////////
		//RECIEVING FROM TEENSY//
		/////////////////////////
		if( (float)timeGetTime() > recieveTime ) {			
			int click = 0;
			//int oldClick = 0;
			//update next time we should run this loop
			recieveTime = (float)timeGetTime() + 10.0f;
			//read in over serial
			int f = mySerial->ReadData(serialBuffTest,16);
			//printf("%i \n",f);
			//cast to array of ints
			int* wireLengths = (int*)(serialBuffTest);
			//printf("%i, %i, %i, %i \n",wireLengths[0],wireLengths[1],wireLengths[2],wireLengths[3]);
			

			//left click
			if(wireLengths[0] < 0 ){
				click = 1;
				wireLengths[0] *= -1;
			}
			//right click
			if(wireLengths[1] < 0 ){
				click = 2;
				wireLengths[1] *= -1;
			}
			//both click
			if(wireLengths[2] < 0 ){
				click = 3;
				wireLengths[2] *= -1;
			}

			//printf("%i \n", click);
			//lengths of wires, in centimeters
			float conversionRate = (2.0f * 3.14159f) / 512.0f;
			L0 = (float)wireLengths[0]*conversionRate; // bottom right
			L1 = (float)wireLengths[1]*conversionRate; // top right
			L2 = (float)wireLengths[2]*conversionRate; // top left
			L3 = (float)wireLengths[3]*conversionRate; // bottom left
			
			//calculate mouse position based on lengths
			calculateCoordinates(L0,L1,L2,L3);	
			//printf("l0: %f || l1: %f || l2: %f || l3: %f \n", L0,L1,L2,L3);
			//printf("X: %f, Y: %f \n", X,Y);
			//transform to generalized screen coordinates (between 0 and 1.0f)
			float generalizedX = X/A;
			float generalizedY = Y/B;

			//transformations to adjust for working area of device
			generalizedX = (generalizedX - 0.15727) / (0.65);
			generalizedY = (generalizedY - 0.22458) / (0.5);

			
			//change the coordinate frame from our frame (origin bottom left) to windows frame (origin upper left)

			//set the windows mouse position
			SetCursorPos( (int) (generalizedX * screenWidth) , (int) ((1.0f-generalizedY) * screenHeight) );
 
			//NO CLICK
			// process mouse clicks into no click state		
			if(click == 0 && oldClick == 1) {
				mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);	
			}
			if(click == 0 && oldClick == 2) {
				mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);				
			}
			if(click == 0 && oldClick == 3) {
				mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
				mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);				
			}

			//LEFT CLICK//
			// process mouse clicks into left click state 
			if(click == 1 && oldClick == 0) {
				mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
				//mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
			}
			// process mouse clicks into left click state 
			if(click == 1 && oldClick == 2) {
				mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
				mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
			}
			// process mouse clicks into left click state 
			if(click == 1 && oldClick == 3) {
				mouse_event(MOUSEEVENTF_RIGHTUP, 0, 0, 0, 0);
			}

			//RIGHT CLICK//
			// process mouse clicks into left click state 
			if(click == 2 && oldClick == 0) {
				//mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
				mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
			}
			// process mouse clicks into left click state 
			if(click == 2 && oldClick == 1) {
				mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
				mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
			}
			// process mouse clicks into left click state 
			if(click == 2 && oldClick == 3) {
				mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
			}

			//BOTH CLICK//
			// process mouse clicks into left click state 
			if(click == 3 && oldClick == 0) {
				mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
				mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
			}
			// process mouse clicks into left click state 
			if(click == 3 && oldClick == 1) {
				//mouse_event(MOUSEEVENTF_LEFTUP, 0, 0, 0, 0);
				mouse_event(MOUSEEVENTF_RIGHTDOWN, 0, 0, 0, 0);
			}
			// process mouse clicks into left click state 
			if(click == 3 && oldClick == 2) {
				mouse_event(MOUSEEVENTF_LEFTDOWN, 0, 0, 0, 0);
			}
			oldClick = click;
		}

		/////////////////////
		//SENDING TO TEENSY//
		/////////////////////
		if( (float)timeGetTime() > sendTime ) {
			//put in delay
			sendTime = (float)timeGetTime() + 10.0f;
			//request data from socket: for Unity, returned in array of size 2: unityForces
			zmq::message_t request = zmq::message_t();
			
			while(socket.recv(&request))
			{
			}

			float* unityForces = static_cast<float*>(request.data());
			float rootSquareValue = sqrt(pow(unityForces[0],2) + pow(unityForces[1],2));
			if(rootSquareValue > 5.0f)
			{
				unityForces[0] = (unityForces[0]/rootSquareValue) * 5.0f;
				unityForces[1] = (unityForces[1]/rootSquareValue) * 5.0f;
			}
			//printf("%f, %f \n",unityForces[0],unityForces[1]);
			Eigen::Vector3f desiredControl;
			//desiredControl[0] = 0.0f;
			//desiredControl[1] = 0.0f; 
			//desiredControl[2] = 0.0f;
			desiredControl[0] = unityForces[0];
			desiredControl[1] = unityForces[1];
			desiredControl[2] = 0.0f;
			//vector<float> forces(4);// = vector<float>(0.0f,0.0f,0.0f);// = (0.0f,0.0f,0.0f);
		
			vector<float> forces = getMotorForces(desiredControl[0],desiredControl[1],desiredControl[2]);

			motorForcesToSerial[0] = (int)((forces[0] / maxForce) * -255);
			motorForcesToSerial[1] = (int)((forces[1] / maxForce) * -255);
			motorForcesToSerial[2] = (int)((forces[2] / maxForce) * -255);
			motorForcesToSerial[3] = (int)((forces[3] / maxForce) * -255);
			//printf("M1: %f \n M2:%f \n M3:%f \n M4:%f \n",forces[0],forces[1],forces[2],forces[3]);
			//printf("%i, %i, %i, %i \n",(int)motorForcesToSerial[0],(int)motorForcesToSerial[1],(int)motorForcesToSerial[2],(int)motorForcesToSerial[3]);
			
			//motorForcesToSerial[0] = 80; 
			//motorForcesToSerial[1] = 80; 
			//motorForcesToSerial[2] = 80; 
			//motorForcesToSerial[3] = 80; 

			//send data to teensy
			mySerial->WriteData((char*)motorForcesToSerial, 16);	
			//printf("not skip \n");
		} else {
			//printf("skip \n");
		}

		//clear the usb buffer really quickly
		mySerial->ReadData(serialBuffTest,16);
	}

}

//takes in the 4 lengths of the wires and returns a vector containing [x,y,theta]
void calculateCoordinates(float L0, float L1, float L2, float L3) {
	//our calculation for the bottom x and y positions from wire lengths
	botX = (pow(L0,2) - pow(A,2) - pow(L3,2))/(-2.0f*A);
	botY = sqrtf( (pow(L3,2) - pow(botX,2))  );
	
	//our calculation for the top x and y positions from wire lengths
	topY = B - sqrtf( pow(L2,2) - pow( ( pow(L1,2) - pow(A,2) -pow(L2,2) ) / (-2.0f*A)  ,2) );
	topX = sqrtf( pow(L2,2) - pow((B-topY),2) );

	X = (topX + botX)/2.0f;
	Y = (topY + botY)/2.0f;
	//our calculation for the top x and y positions from wire lengths

	theta = atan( (topX - botX) / (topY - botY) );
}

vector<float> getMotorForces(float desired_f_x, float desired_f_y, float desired_moment)
{

	vector<float> retVec(4);
	//retArray = float[4];

	//calculate unit vectors from corners to bottom and top junction points
	Eigen::Vector2f l0_unit;
	float l0_Length = vectorLength( botX-A,botY );
	l0_unit << ( (botX - A)/l0_Length ),( botY/l0_Length );
	
	Eigen::Vector2f l1_unit;
	float l1_Length = vectorLength(topX-A,topY-B);
	l1_unit << ( (topX - A)/l1_Length ),( (topY - B) / l1_Length );

	Eigen::Vector2f l2_unit;
	float l2_Length = vectorLength(topX,topY - B);
	l2_unit << (topX/l2_Length),((topY - B)/l2_Length);

	Eigen::Vector2f l3_unit;
	float l3_Length = vectorLength(botX,botY);
	l3_unit << (botX/l3_Length),(botY/l3_Length);
	
	//calculate x and y length of the seperation gap between two wire junctions
	float y_arm = (botY - topY)/2.0f;
	float x_arm = (botX - topX)/2.0f;

	//calculate coefficients for moment solving
	float m0 = -1.0f * l0_unit[0] * y_arm + l0_unit[1] * x_arm;
	float m1 = -1.0f * l1_unit[1] * x_arm + l1_unit[0] * y_arm;
	float m2 = -1.0f * l2_unit[1] * x_arm + l2_unit[0] * y_arm;
	float m3 = -1.0f * l3_unit[0] * y_arm + l3_unit[1] * x_arm;

	int Ncol = 4; //number of variables we are solving for
	int* colno = NULL; //int array to store column indicators
	REAL *row = NULL; //REAL (double) array to store coefficients for constraints

	//allocate memory for colno
	colno = (int *) malloc(Ncol * sizeof(*colno));
	//allocate memory for row
    row = (REAL *) malloc(Ncol * sizeof(*row));

	//initialize solver for 4 variables
	lprec *lp;
	lp = make_lp(0,Ncol);
	set_verbose(lp,IMPORTANT); //turn off loud reporting
	
	//label solver variables
	set_col_name(lp,1,"c0");
	set_col_name(lp,2,"c1");
	set_col_name(lp,3,"c2");
	set_col_name(lp,4,"c3");

	//set bounds on output torques must keep them between -1.0 and -10.0
	set_bounds(lp,1,-1.0f * maxForce, -1.0f * minForce);
	set_bounds(lp,2,-1.0f * maxForce, -1.0f * minForce);
	set_bounds(lp,3,-1.0f * maxForce, -1.0f * minForce);
	set_bounds(lp,4,-1.0f * maxForce, -1.0f * minForce);

	//add rowmode speeds up the process of creating a model in memory
	set_add_rowmode(lp, TRUE);

	//build model here
	//////////////////////////////////////////////
	//CONSTRAINT 1: Force sum of x == desired_x///
	//////////////////////////////////////////////
	colno[0] = 1; colno[1] = 2; colno[2] = 3; colno[3] = 4;
	row[0] = l0_unit[0];
	row[1] = l1_unit[0];
	row[2] = l2_unit[0];
	row[3] = l3_unit[0];
	add_constraintex(lp,4,row,colno,EQ,	desired_f_x);

	//////////////////////////////////////////////
	//CONSTRAINT 2: Force sum of y == desired_y///
	//////////////////////////////////////////////
	colno[0] = 1; colno[1] = 2; colno[2] = 3; colno[3] = 4;
	row[0] = l0_unit[1];
	row[1] = l1_unit[1];
	row[2] = l2_unit[1];
	row[3] = l3_unit[1];	
	add_constraintex(lp,4,row,colno,EQ,	desired_f_y);
	

	/////////////////////////////////////////////////////////
	//CONSTRAINT 3: Force sum of moments == desired_moment///
	/////////////////////////////////////////////////////////
	colno[0] = 1; colno[1] = 2; colno[2] = 3; colno[3] = 4;
	row[0] = m0;
	row[1] = m1;
	row[2] = m2;
	row[3] = m3;	
	//add_constraintex(lp,4,row,colno,EQ,	desired_moment);

	//turn off add_rowmode once model is completed
	set_add_rowmode(lp, FALSE);
	
	/////////////////////////////////////////
	//OBJ FUNCTION: set objective function///
	/////////////////////////////////////////
	colno[0] = 1; colno[1] = 2; colno[2] = 3; colno[3] = 4;
	row[0] = 1.0f;
	row[1] = 1.0f;
	row[2] = 1.0f;
	row[3] = 1.0f;
	set_obj_fnex(lp,4,row,colno);
	set_maxim(lp);

	//solve the system of equations
	solve(lp);

	//copy answers into row
	get_variables(lp, row);
	
	//copy all values into return array
    for(int j = 0; j < Ncol; j++) {
		//printf("%s: %f\n", get_col_name(lp, j + 1), row[j]);
		retVec[j] = (float)row[j];
	}
	
	//clean up memory after all solving
	free(row);
	free(colno);
	delete_lp(lp);
	
	return retVec;
}

void testCalculateCoordinates()
{
	//pass
	float l0 = 39.43f;
	float l1 = 39.43f;
	float l2 = 39.43f;
	float l3 = 39.43f;
	calculateCoordinates(l0,l1,l2,l3);
	printf("Should be 32.3, 22.62, 0:  %f, %f, %f\n", X,Y,theta);

	
	//pass
	l0 = 53.47f;
	l1 = 57.59f;
	l2 = 13.702f;
	l3 = 27.800f;
	calculateCoordinates(l0,l1,l2,l3);
	printf("Should be 12, 27, -0.609:  %f, %f, %f\n", X,Y,theta);
	/*
	//pass
	l0 = 186.81;
	l1 = 194.164;
	l2 = 41.231;
	l3 = 53.851;
	test = calculateCoordinates(l0,l1,l2,l3);
	printf("Should be 55, 185, 0.785:  %f, %f, %f\n", test[0],test[1],test[2]);
	*/
}

void testCalculateForce()
{
	float l0 = 30.5;
	float l1 = 45.4;
	float l2 = 48.7;
	float l3 = 35.2;
	calculateCoordinates(l0,l1,l2,l3);

	float initialTime = (float)timeGetTime();
	vector<float> motorforces = getMotorForces(3.0f,4.0f,-0.3f);
	float timeElapsed = (float)timeGetTime() - initialTime;
	printf("%f",timeElapsed);
	//Eigen::Matrix<float,3,4> jacobian = calculateJacobian(l0,l1,l2,l3);
	//Eigen::Matrix<float,4,3> jacobianT = jacobian.transpose();		
	//Eigen::Matrix<float,3,1> desiredControl;
	//desiredControl[0] = 0.0f; //flip the inputs from unity because of a change of frame of reference
	//desiredControl[1] = 10.0f; 
	//desiredControl[2] = 0.0f;


	//Eigen::Matrix<float,4,1> motorForces = jacobianT * desiredControl;	
	
	/*	
	Eigen::Vector2f directionl0;
	directionl0[0] = 0.0f; directionl0[2] = 0.0f;
	Eigen::Vector2f directionl1;
	directionl1[0] = 0.0f; directionl1[2] = 0.0f;
	Eigen::Vector2f directionl2;
	directionl2[0] = 0.0f; directionl2[2] = 0.0f;
	Eigen:: Vector2f directionl3;
	directionl3[0] = 0.0f; directionl3[2] = 0.0f;
	directionl0*=motorForces[0];
	directionl1*=motorForces[1];
	directionl2*=motorForces[2];
	directionl3*=motorForces[3];
	
	Eigen::Vector2f forceOut = directionl0 + directionl1 + directionl2 + directionl3; 
	printf("%f, %f \n", forceOut[0], forceOut[1]);
	*/
	//printf("%f, %f, %f, %f \n",motorForces[0], motorForces[1], motorForces[2], motorForces[3]);
	
	//should be centered
	//l0 = 130;
	//l1 = 130;
	//l2 = 120;
	//l3 = 170;
	//jacobian = calculateJacobian(l0,l1,l2,l3);
	//jacobianT = jacobian.transpose();		
	//desiredControl[0] = 0.0f; //flip the inputs from unity because of a change of frame of reference
	//desiredControl[1] = 0.0f; 
	//desiredControl[2] = 0.0f;

	//motorForces = jacobianT * desiredControl;	
	
	//printf("%f, %f, %f, %f \n",motorForces[0], motorForces[1], motorForces[2], motorForces[3]);	
	
}

float vectorLength(float x_0, float y_0)
{
	return sqrtf(pow(x_0,2) + pow(y_0,2));
};