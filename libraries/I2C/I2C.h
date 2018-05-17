#ifndef I2C_h
#define I2C_h

#include "Arduino.h"

#define START1    			0x0A
#define START2   			0x55
#define MAX_PACKET_LENGTH 	200


/*
0:START1
1:START2
2:cType
3:len
4:dType
5-:Option
*/


enum DataType
{
	dType_None = 0,
	dType_Command,
	dType_StringMessage,
	dType_EndOfType
};


enum CommandType
{
	cType_None = 0,
	cType_Motor,
	cType_Servo,
	cType_PatLite,
	cType_EndOfType
};

enum Request
{	
	Req_PsPad = 0x30,
	Req_RC522,
	Req_Switch,
};

enum MotorDirectionType
{
	mdType_Stop = 0,
	mdType_Forward,
	mdType_Backward,
};


typedef struct {
    byte Number;
    byte Direction;
	byte Speed;
} cMotor_t;


typedef struct {
    byte Lamp;
    byte Speed;
} cPatLite_t;


typedef struct {
    byte Number;
    byte Angle;
	byte Speed;
} cServo_t;


extern cMotor_t cMotor;
extern cPatLite_t cPatLite;
extern cServo_t cServo;


extern	void I2C_begin(byte adress);	
extern	void I2C_RequestCbk(byte reg, byte* p_data, byte* p_size);

#endif   