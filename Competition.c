#pragma config(I2C_Usage, I2C1, i2cSensors)
#pragma config(Sensor, dgtl1,  liftDistance,   sensorSONAR_cm)
#pragma config(Sensor, dgtl3,  conveyorQuad,   sensorQuadEncoder)
#pragma config(Sensor, I2C_1,  flQuad,         sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Sensor, I2C_2,  frQuad,         sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Sensor, I2C_3,  brQuad,         sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Sensor, I2C_4,  blQuad,         sensorQuadEncoderOnI2CPort,    , AutoAssign)
#pragma config(Motor,  port1,           scissorLeft2,  tmotorVex393_HBridge, openLoop)
#pragma config(Motor,  port2,           frontLeft,     tmotorVex393_MC29, openLoop, encoderPort, I2C_1)
#pragma config(Motor,  port3,           frontRight,    tmotorVex393_MC29, openLoop, encoderPort, I2C_2)
#pragma config(Motor,  port4,           backLeft,      tmotorVex393_MC29, openLoop, reversed, encoderPort, I2C_4)
#pragma config(Motor,  port5,           backRight,     tmotorVex393_MC29, openLoop, reversed, encoderPort, I2C_3)
#pragma config(Motor,  port6,           scissorLeft,   tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port7,           scissorRight,  tmotorVex393_MC29, openLoop)
#pragma config(Motor,  port8,           conveyor,      tmotorVex393_MC29, openLoop, encoderPort, dgtl3)
#pragma config(Motor,  port9,           scissorRight2, tmotorVex393_MC29, openLoop, reversed)
//*!!Code automatically generated by 'ROBOTC' configuration wizard               !!*//

#pragma platform(VEX)

//Competition Control and Duration Settings
#pragma competitionControl(Competition)
#pragma autonomousDuration(15)
#pragma userControlDuration(105)

#include "Vex_Competition_Includes.c"	//Main competition background code...do not modify!

int autoProgram = 1;

int Lift(int liftTarget, int threshold, int speed);
void SetDriveMotors(int fl, int fr, int bl, int br);
void ResetMotorEncoders();
void SetLiftSpeed(int speed);
task MoveConveyor();

void autonomous1();
void autonomous2();
void autonomous3();
void autonomous4();

//drive wheels 3.25" * pi * sqrt(2) = 14.43 inches forward per rotation, 627.2 ticks per rotation, ===43.5 TICKS PER INCH===

void pre_auton()
{
	ResetMotorEncoders();
	bLCDBacklight = true;
	clearLCDLine(0);
	clearLCDLine(1);

	displayLCDCenteredString(0, "Position conveyor");
	displayLCDCenteredString(1, "Done");
	while(nLCDButtons==0){}
	SensorValue[conveyorQuad] = 0;
	wait1Msec(500);

	displayLCDString(0, 0, "Prog select:");
	while(nLCDButtons != 2) { //as long as the "OK" button isn't pressed
		if(nLCDButtons == 1) { //if the select button is pressed
			if(autoProgram == 2) {
				autoProgram = 1;
				} else {
				autoProgram++;
			}
			while(nLCDButtons != 0){} //wait for button release
		}
		displayLCDNumber(0, 14, autoProgram);
	}

	// Set bStopTasksBetweenModes to false if you want to keep user created tasks running between
	// Autonomous and Tele-Op modes. You will need to manage all user created tasks if set to false.
	bStopTasksBetweenModes = true;
}

task autonomous()
{
	clearLCDLine(0);
	clearLCDLine(1);
	string auto = "Autonomous: ";
	char num[1]; //to hold int
	sprintf(num, "%d", autoProgram); //int to string
	strcat(auto, num); //combine
	displayLCDCenteredString(0, auto);

	switch (autoProgram) { //trigger the program selected in pre_auton
	case 1:
		autonomous1();
		break;
	case 2:
		autonomous2();
		break;
	case 3:
		autonomous3();
		break;
	case 4:
		autonomous4();
		break;
	}
}

task usercontrol()
{
	//Create "deadzone" variables. Adjust threshold value to increase/decrease deadzone
	int X2 = 0, Y1 = 0, X1 = 0, threshold = 10;

	//Loop Forever
	while(1)
	{
		clearLCDLine(0);
		clearLCDLine(1);
		string flquadv = SensorValue(liftDistance);
		displayLCDCenteredString(0, flquadv);
		ResetMotorEncoders();
		//Create "deadzone" for Y1/Ch3
		if(abs(vexRT[Ch3]) > threshold)
			Y1 = vexRT[Ch3];
		else
			Y1 = 0;
		//Create "deadzone" for X1/Ch4
		if(abs(vexRT[Ch4]) > threshold)
			X1 = vexRT[Ch4];
		else
			X1 = 0;
		//Create "deadzone" for X2/Ch1
		if(abs(vexRT[Ch1]) > threshold)
			X2 = vexRT[Ch1];
		else
			X2 = 0;

		//Remote Control Commands
		motor[frontRight] = Y1 - X2 - X1;
		motor[backRight] =  Y1 - X2 + X1;
		motor[frontLeft] = Y1 + X2 + X1;
		motor[backLeft] =  Y1 + X2 - X1;

		if (vexRT(Btn5U)) {	//scissor lift up/down
			motor[scissorLeft] = -127;
			motor[scissorRight] = -127;
			motor[scissorLeft2] = -127;
			motor[scissorRight2] = -127;
		}
		else {
			if (vexRT(Btn5D)){
				motor[scissorLeft] = 127;
				motor[scissorRight] = 127;
				motor[scissorLeft2] = 127;
				motor[scissorRight2] = 127;
			}
			else {
				motor[scissorLeft] = 0;
				motor[scissorRight] = 0;
				motor[scissorLeft2] = 0;
				motor[scissorRight2] = 0;
			}
		}

		if (vexRT(Btn6U)){ //conveyor forward/back
			motor[conveyor] = -127;
		}
		else {
			if (vexRT(Btn6D)) {
				motor[conveyor] = 127;
			}
			else {
				motor[conveyor] = 0;
			}
		}
	}
}

void SetDriveMotors(int fl, int fr, int bl, int br) //set speeds of all 4 motors
{
	motor[frontLeft] = fl;
	motor[frontRight] = fr;
	motor[backLeft] = bl;
	motor[backRight] = br;
}

void SetLiftSpeed(int speed) //consolidate all lift motor commands to one function
{
	motor[scissorLeft] = speed;
	motor[scissorRight] = speed;
	motor[scissorLeft2] = speed;
	motor[scissorRight2] = speed;
}

void ResetMotorEncoders() //reset drive motor encoders
{
	nMotorEncoder[frontLeft] = 0;
	nMotorEncoder[frontRight] = 0;
	nMotorEncoder[backLeft] = 0;
	nMotorEncoder[backRight] = 0;
}

task MoveConveyor() //autonomous conveyor handler
{
	SensorValue[conveyorQuad] = 0;
	motor[conveyor] = -127;
	while(SensorValue(conveyorQuad)<160){}
	motor[conveyor] = 127;
	while(SensorValue(conveyorQuad)>-600){}
	motor[conveyor] = 0;
}

void autonomous1(){
	ResetMotorEncoders();
	nMotorEncoder[conveyor] = 0;

	SetDriveMotors(-75,75,75,-75);
	while(nMotorEncoder[frontLeft]>(-5*14)){}
	SetDriveMotors(75,-75,-75,75);
	ResetMotorEncoders();
	while(nMotorEncoder[frontLeft]<(5*14)){}
	SetDriveMotors(-10,10,10,-10);
	wait1Msec(100);
	SetDriveMotors(75,75,75,75);
	ResetMotorEncoders();
	while(nMotorEncoder[frontLeft]<(45.5*14)){}
	SetDriveMotors(-10,-10,-10,-10);
	wait1Msec(100);
	SetDriveMotors(0,0,0,0);
	motor[conveyor] = -127;
	while(nMotorEncoder[conveyor]<300){}
	motor[conveyor] = 0;
	SetDriveMotors(-75,75,-75,75);
	ResetMotorEncoders();
	while(nMotorEncoder[frontLeft]>-1300){}
	SetDriveMotors(10,-10,10,-10);
	wait1Msec(100);
	SetDriveMotors(75,75,75,75);
	ResetMotorEncoders();
	while(nMotorEncoder[frontLeft]<(20*14)){}
	SetDriveMotors(-10,-10,-10,-10);
	wait1Msec(100);
	SetDriveMotors(0,0,0,0);
	motor[conveyor] = 127;
	while(nMotorEncoder[conveyor]>0){}
	motor[conveyor] = 0;
}

void autonomous2(){
	ResetMotorEncoders();
	nMotorEncoder[conveyor] = 0;

	SetDriveMotors(75,-75,-75,75);
	while(nMotorEncoder[frontLeft]>(5*14)){}
	SetDriveMotors(-75,75,75,-75);
	ResetMotorEncoders();
	while(nMotorEncoder[frontLeft]<(-5*14)){}
	SetDriveMotors(10,-10,-10,10);
	wait1Msec(100);
	SetDriveMotors(75,75,75,75);
	ResetMotorEncoders();
	while(nMotorEncoder[frontLeft]<(45.5*14)){}
	SetDriveMotors(-10,-10,-10,-10);
	wait1Msec(100);
	SetDriveMotors(0,0,0,0);
	motor[conveyor] = -127;
	while(nMotorEncoder[conveyor]<300){}
	motor[conveyor] = 0;
	SetDriveMotors(-75,75,-75,75);
	ResetMotorEncoders();
	while(nMotorEncoder[frontLeft]>-1300){}
	SetDriveMotors(10,-10,10,-10);
	wait1Msec(100);
	SetDriveMotors(75,75,75,75);
	ResetMotorEncoders();
	while(nMotorEncoder[frontLeft]<(20*14)){}
	SetDriveMotors(-10,-10,-10,-10);
	wait1Msec(100);
	SetDriveMotors(0,0,0,0);
	motor[conveyor] = 127;
	while(nMotorEncoder[conveyor]>0){}
	motor[conveyor] = 0;
}

void autonomous3(){
}

void autonomous4(){
}
