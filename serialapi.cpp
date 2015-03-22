// serialapi.cpp : Defines the entry point for the console application.
//

#include <zmq.hpp>
#include <zmq_utils.h>

#include <stdlib.h>
#include "stdafx.h"
#include "SerialClass.h"
#include <math.h>
#include <vector>

#include <Eigen\Core>

using namespace std;

float A = 0; //y distance from bottom pulley to top pulley
float B = 0; //x distance from right pulley to left pulley
	
vector<float> calculateCoordinates(float L0, float L1, float L2, float L3);

int _tmain(int argc, _TCHAR* argv[])
{
	//initialize serial connection
	Serial* mySerial = new Serial("COM4");

	zmq::context_t context (1);
	zmq::socket_t socket (context, ZMQ_PAIR);
	socket.bind("tcp://*:11000");
	
	while(true) {
		zmq::message_t request = zmq::message_t();
		//char buf[32];
        //  Wait for next request from client
        //printf("Received Hello");
		socket.recv (&request);
		stringbuf sstream();
		socket.send (&request);
		char string[7];
		memcpy(string,request.data(),6);
		string[6] = '\0';
		//std::string rpl = std::string(static_cast<char*>(request.data()),request.size());
		std::string rpl = std::string( string );
		//std::string(
		//printf( ());
		printf("%s \n",rpl);
		//free(string);
	}

	//read in L1, L2, L3, L4 position from teensy
	float L0 = 0; //bottom right cord length
	float L1 = 0; //upper right cord length
	float L2 = 0; //upper left cord length
	float L3 = 0; //bottom left cord length
	
	//object state contains the 
	vector<float> objectState = calculateCoordinates(L0,L1,L2,L3);
	float x = objectState[0];
	float y = objectState[1];
	float theta = objectState[2];

	int i = 0;
	while (i == 0)
	{

		//set mouse position
		//SetCursorPos(x,y);
		
		//check required force vector
		
		//calculate four direction vectors from mouse pos

		//calculate jacobian

		//serial send torques to teensy		
		char* outputbuff = "hey dude";//[8];
		char inputbuff[8];
		//buff = "hey dude";
		//int buffInt;
		
		mySerial->WriteData(outputbuff,8);
		Sleep(50);
		mySerial->ReadData(inputbuff,8);
		
		//printf("Check out this input %s \n",inputbuff);
	}
	
}

	
//takes in the 4 lengths of the wires and returns a vector containing [x,y,theta]
vector<float> calculateCoordinates(float L0, float L1, float L2, float L3) {
	std::vector<float> returnVector(3);
	float x1 = sqrtf( (-1*(pow(B,4))) + (2*(pow(B,2)*pow(L0,2))) + (2*(pow(B,2)*pow(L3,2))) - (pow(L0,4)) + (2*(pow(L0,2)*pow(L3,2))) - (pow(L3,4)) ) /(2*B); //lower wire convergence point
	float x2 = sqrtf( (-1*(pow(B,4))) + (2*(pow(B,2)*pow(L1,2))) + (2*(pow(B,2)*pow(L2,2))) - (pow(L1,4)) + (2*(pow(L1,2)*pow(L2,2))) - (pow(L2,4)) - 2*A*B ) /(2*B); //upper wire convergence point

	float y1 = ( pow(B,2) + pow(L0,2) - pow(L3,2) ) / (2*B);
	float y2 = ( pow(B,2) + pow(L1,2) - pow(L2,2) ) / (2*B);

	float x = (x1 + x2)/2; //mouse x position
	float y = (y1 + y2)/2; // mouse y position

	float theta = atan2(y2 - y1, x2 - x1); //rotation offset from vertical, positive values are CCW, negative are CW
	
	returnVector[0] = x;
	returnVector[1] = y;
	returnVector[2] = theta;
	return returnVector;
}

