#include "I2C.h"
#include "Arduino.h"
#include <Wire.h>

static byte cmdBuff[MAX_PACKET_LENGTH];

static byte checkHeader;
static byte cmdIndex;
static byte receiveLength;
static byte receiveDtype;
static byte receiveCtype;
static byte dataBuff[30];



cMotor_t cMotor;
cPatLite_t cPatLite;
cServo_t cServo = {0xff, 0, 0};


static void I2C_Receive();

void I2C_begin(byte adress)
{
	Wire.begin(adress);
	Wire.onReceive(I2C_Receive);
//	Serial.begin(9600);
}

void I2C_Receive()
{
	if (Wire.available() > 0)
	{
		
		int input = Wire.read();	
		cmdBuff[cmdIndex++] = (char)input;

		if (cmdIndex < MAX_PACKET_LENGTH)
		{
			if (cmdIndex == 1)
			{
				if (cmdBuff[0] == START1){
					checkHeader = 1;
//					Serial.print("START1: ");
//					Serial.println(cmdBuff[0]);
				}
				else
				{
					checkHeader = 0;
					cmdIndex = 0;
				}
			}
			else if (checkHeader == 1)
			{
				if (cmdBuff[1] == START2){
					checkHeader = 2;
//					Serial.print("START2: ");
//					Serial.println(cmdBuff[1]);
				}
				else
				{
					checkHeader = 0;
					cmdIndex = 0;
				}
			}

			else if (checkHeader == 2)
			{
				if (cmdIndex == 3)
				{
					receiveDtype = cmdBuff[2];
//					Serial.print("receiveDtype: ");
//					Serial.println(receiveDtype);
					
					dataBuff[cmdIndex - 3] = cmdBuff[cmdIndex - 1];
				}
				else if (receiveDtype != dType_StringMessage)
				{
					if (cmdIndex == 4 && cmdIndex)
					{
						receiveLength = cmdBuff[3];
//						Serial.print("receiveLength: ");
//						Serial.println(receiveLength);

						dataBuff[cmdIndex - 3] = cmdBuff[cmdIndex - 1];
					}
					else if (cmdIndex > 4)
					{
						
						if (receiveLength + 3 > cmdIndex){
							dataBuff[cmdIndex - 3] = cmdBuff[cmdIndex - 1];
						}else if (receiveLength + 4 <= cmdIndex){
								
							if (receiveDtype == dType_Command)
							{
								receiveCtype = cmdBuff[4];
//								Serial.print("receiveCtype: ");
								Serial.println(receiveCtype);
								
								if (receiveCtype == cType_Motor)
								{
									cMotor.Number = cmdBuff[5];
									cMotor.Direction = cmdBuff[6];
									cMotor.Speed = cmdBuff[7];
								}								
								else if (receiveCtype == cType_Servo)
								{
									cServo.Number = cmdBuff[5];
									cServo.Angle = cmdBuff[6];
									cServo.Speed = cmdBuff[7];
									
//									Serial.print(cServo.Number);
//									Serial.print(cServo.Angle);
//									Serial.print(cServo.Speed);

								}else if (receiveCtype == cType_PatLite)
								{
									cPatLite.Lamp = cmdBuff[5];
									cPatLite.Speed = cmdBuff[6];
								}
							}
							checkHeader = 0;
							cmdIndex = 0;
						}
					}
				}
				else
				{
					checkHeader = 0;
					cmdIndex = 0;
				}
			}
		}
		else
		{
			checkHeader = 0;
			cmdIndex = 0;
		}

	}
}