/*
 * DYNAMIXEL OBJECTS DEFINITIONS
 */

#ifndef SERVO_DEF_H
#define SERVO_DEF_H


//DSV2 INSTRUCTIONS
#define MAX_WRITE_LEN	(64)
#define WRITE_INST		(0x03)

#define MAX_READ_LEN	(4)
#define READ_INST		(0x02)

#define PING_INST		(0x01)




//DYNAMIXEL OBJECTS ADDRESSES


//EEPROM OBJECTS ONLY ACCESSIBLE WHEN TORQUE DISABLED
#define SERVO_MODEL_NUMBER				0	//2 bytes	RO
#define SERVO_MODEL_INFORMATION			2	//4 bytes	RO
#define SERVO_FIRMWARE_VERSION			6	//1 byte	RO
#define SERVO_ID						7 	//1 byte	RW
#define SERVO_BAUD_RATE					8	//1 byte	RW
#define SERVO_RETURN_DELAY_TIME			9	//1 byte	RW
#define SERVO_DRIVE_MODE				10 	//1 byte	RW
#define SERVO_OPERATING_MODE			11	//1 byte	RW
#define SERVO_SECONDARY_ID				12	//1 byte	RW
#define SERVO_PROTOCOL_TYPE				13	//1 byte	RW
#define SERVO_HOMING_OFFSET				20	//4 bytes	RW
#define SERVO_MOVING_THRESHOLD			24	//4 bytes	RW
#define SERVO_TEMPERATURE_LIMIT			31	//1 byte	RW
#define SERVO_MAX_VOLTAGE_LIMIT			32	//2 bytes	RW
#define SERVO_MIN_VOLTAGE_LIMIT			34	//2 bytes	RW
#define SERVO_PWM_LIMIT					36	//2 bytes	RW
#define SERVO_CURRENT_LIMIT				38	//2 bytes	RW
#define SERVO_VELOCITY_LIMIT			44	//4 bytes	RW
#define SERVO_MAX_POSITION_LIMIT		48	//4 bytes	RW
#define SERVO_MIN_POSITION_LIMIT		52	//4 bytes	RW
#define SERVO_PWM_SLOPE					62	//1 byte	RW
#define SERVO_SHUTDOWN					63	//1 byte	RW


//RAM OBJECTS ALWAYS ACCESSIBLE
#define SERVO_TORQUE_ENABLE 			64  //1 byte	RW
#define SERVO_LED 						65  //1 byte	RW
#define SERVO_STATUS_RETURN_LEVEL 		68  //1 byte	RW
#define SERVO_REGISTERED_INSTRUCTION 	69  //1 byte	RW
#define SERVO_HARDWARE_ERROR_STATUS 	70  //1 byte	RW
#define SERVO_VELOCITY_I 				76  //2 bytes	RW
#define SERVO_VELOCITY_P 				78  //2 bytes	RW
#define SERVO_POSITION_D 				80  //2 bytes	RW
#define SERVO_POSITION_I 				82  //2 bytes	RW
#define SERVO_POSITION_P 				84  //2 bytes	RW
#define SERVO_FEEDFORWARD_2 			88  //2 bytes	RW
#define SERVO_FEEDFORWARD_1				90  //2 bytes	RW
#define SERVO_BUS_WATCHDOG 				98  //1 byte	RW
#define SERVO_GOAL_PWM 					100 //2 bytes	RW
#define SERVO_GOAL_CURRENT 				102 //2 bytes	RW
#define SERVO_GOAL_VELOCITY 			104 //4 bytes	RW
#define SERVO_PROFILE_ACCELERATION 		108 //4 bytes	RW
#define SERVO_PROFILE_VELOCITY 			112 //4 bytes	RW
#define SERVO_GOAL_POSITION 			116 //4 bytes	RW
#define SERVO_REALTIME_TICK 			120 //2 bytes	RO
#define SERVO_MOVING 					122 //1 byte	RO
#define SERVO_MOVING_STATUS 			123 //1 byte	RO
#define SERVO_PRESENT_PWM 				124 //2 bytes	RO
#define SERVO_PRESENT_CURRENT 			126 //2 bytes	RO
#define SERVO_PRESENT_VELOCITY 			128 //4 bytes	RO
#define SERVO_PRESENT_POSITION 			132 //4 bytes	RO
#define SERVO_VELOCITY_TRAJECTORY 		136 //4 bytes	RO
#define SERVO_POSITION_TRAJECTORY 		140 //4 bytes	RO
#define SERVO_PRESENT_INPUT_VOLTAGE 	144 //2 bytes	RO
#define SERVO_PRESENT_TEMPERATURE 		146 //1 byte	RO











#endif	/* SERVO_DEF_H */
