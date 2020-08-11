#ifndef MAXON_DEF_H
#define MAXON_DEF_H

#define MAXON_DEVICE_TYPE		0x1000, 0x00 //U32
#define MAXON_ERROR_REGISTER 	0x1001, 0x00 //U8

#define MAXON_MODE_PPM			1
//CONTROL


#define MAXON_MODE_OF_OPERATION		0x6060, 0x00
#define MAXON_MAX_PROFILE_VELOCITY	0x607F, 0x00 //U32
#define MAXON_MOTOR_MAX_SPEED		0x6080, 0x00 //U32
#define MAXON_GEAR_NUMERATOR		0x3003, 0x01 //U32
#define MAXON_GEAR_DENOMINATOR		0x3003, 0x02 //U32
#define MAXON_GEAR_MAX_INPUT_SPEED	0x3003, 0x03 //U32
#define MAXON_PROFILE_VELOCITY		0x6081, 0x00 //U32
#define MAXON_PROFILE_ACCELERATION	0x6083, 0x00 //U32
#define MAXON_PROFILE_DECELERATION	0x6084, 0x00 //U32
#define MAXON_MOTION_PROFILE_TYPE	0x6086, 0x00 //I16
#define MAXON_QUICKSTOP_DECELERATION	0x6085, 0x00 //U32
#define MAXON_MAX_ACCELERATION		0x60C5, 0x00 //U32
#define MAXON_TARGET_POSITION				0x607A, 0x00 //I32
#define MAXON_SOFTWARE_MIN_POSITION			0x607D, 0x01 //I32
#define MAXON_SOFTWARE_MAX_POSITION			0x607D, 0x02 //I32
#define MAXON_HOMING_METHOD			0x6098, 0x00 //I8
#define MAXON_CONTROL_WORD			0x6040, 0x00 //U16
#define MAXON_STATUS_WORD			0x6041, 0x00 //U16
#define MAXON_POWER_SUPPLY_VOLTAGE	0x2200, 0x01 //U16
#define MAXON_ABORT_CONNECTION_OPTION_CODE 0x6007, 0x00 //I16

#define MAXON_MOTOR_NOMINAL_CURRENT	0x3001, 0x01 //U32
#define MAXON_MOTOR_CURRENT_LIMIT	0x3001, 0x02 //U32
#define MAXON_MOTOR_POLE_PAIRS		0x3001, 0x03 //U8
#define MAXON_MOTOR_THERMAL_CST		0x3001, 0x04 //U16
#define MAXON_MOTOR_TORQUE_CST		0x3001, 0x05 //U32



//CONTROL WORD COMMANDS
#define MAXON_SET_SHUTDOWN		0b00000110
#define MAXON_SET_SWITCH_ON		0b00000111
#define MAXON_SET_ENABLE		0b00001111
#define MAXON_SET_DISABLE		0b00000111
#define MAXON_SET_DISABLE_V		0b00000000
#define MAXON_SET_QUICKSTOP		0b00000010
#define MAXON_SET_FAULT_RST		0b10000000
#define MAXON_CLR_SHUTDOWN		0b10000001
#define MAXON_CLR_SWITCH_ON		0b10000000
#define MAXON_CLR_ENABLE		0b10000000
#define MAXON_CLR_DISABLE		0b10001000
#define MAXON_CLR_DISABLE_V		0b10000010
#define MAXON_CLR_QUICKSTOP		0b10000100
#define MAXON_CLR_FAULT_RST		0b00000000
//PPM SPECIFIC SETTINGS
#define MAXON_SET_ABS			0b00000000
#define MAXON_CLR_ABS			0b01000000
#define MAXON_SET_REL			0b01000000
#define MAXON_CLR_REL			0b01000000
#define MAXON_SET_NEW_POS		0b00110000
#define MAXON_CLR_NEW_POS		0b00000000



#endif
