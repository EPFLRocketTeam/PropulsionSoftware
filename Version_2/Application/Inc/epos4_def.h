#ifndef EPOS4_DEF_H
#define EPOS4_DEF_H



#define EPOS4_DEVICE_TYPE		0x1000, 0x00 //U32
#define EPOS4_ERROR_REGISTER 	0x1001, 0x00 //U8

#define EPOS4_MODE_PPM			0X01
#define EPOS4_MODE_HOMING		0X06
#define EPOS4_MODE_CSP			0x08


#define EPOS4_POSITION			37
//CONTROL

#define EPOS4_CONTROL_WORD			0x6040, 0x00 //U16
#define EPOS4_STATUS_WORD			0x6041, 0x00 //U16

#define EPOS4_PSU_VOLTAGE			0x2200, 0x01 //u16

#define EPOS4_MOTOR_TYPE			0x6402, 0x00 //u16
#define EPOS4_MODE_OF_OPERATION		0x6060, 0x00 //I8
#define EPOS4_MAX_PROFILE_VELOCITY	0x607F, 0x00 //U32
#define EPOS4_MOTOR_MAX_SPEED		0x6080, 0x00 //U32
#define EPOS4_GEAR_NUMERATOR		0x3003, 0x01 //U32
#define EPOS4_GEAR_DENOMINATOR		0x3003, 0x02 //U32
#define EPOS4_GEAR_MAX_INPUT_SPEED	0x3003, 0x03 //U32
#define EPOS4_PROFILE_VELOCITY		0x6081, 0x00 //U32
#define EPOS4_PROFILE_ACCELERATION	0x6083, 0x00 //U32
#define EPOS4_PROFILE_DECELERATION	0x6084, 0x00 //U32
#define EPOS4_MOTION_PROFILE_TYPE	0x6086, 0x00 //I16
#define EPOS4_QUICKSTOP_DECELERATION	0x6085, 0x00 //U32
#define EPOS4_MAX_ACCELERATION		0x60C5, 0x00 //U32
#define EPOS4_TARGET_POSITION				0x607A, 0x00 //I32
#define EPOS4_SOFTWARE_MIN_POSITION			0x607D, 0x01 //I32
#define EPOS4_SOFTWARE_MAX_POSITION			0x607D, 0x02 //I32
#define EPOS4_HOMING_METHOD			0x6098, 0x00 //I8
#define EPOS4_ERROR_WORD			0x603F, 0x00 //u16
#define EPOS4_POWER_SUPPLY_VOLTAGE	0x2200, 0x01 //U16
#define EPOS4_ABORT_CONNECTION_OPTION_CODE 0x6007, 0x00 //I16
#define EPOS4_ELECTRICAL_RESISTANCE			0x3002, 0x01 //u32
#define EPOS4_ELECTRICAL_INDUCTANCE			0x3002, 0x02 //u16
#define EPOS4_ACTUAL_POSITION				0x6064, 0x00 //i32
#define EPOS4_ACTUAL_TORQUE					0x6077, 0x00 //i16
#define EPOS4_ACTUAL_VELOCITY				0x606C, 0x00 //i32
#define EPOS4_DEMAND_POSITION				0x6062, 0x00 //i32
#define EPOS4_FOLLOW_ERROR_WINDOW			0x6065, 0x00 //u32
#define EPOS4_HOME_POSITION					0x30B0, 0x00 //i32
#define EPOS4_HOME_OFFSET					0x30B1, 0x00 //i32
#define EPOS4_MOTOR_RATED_TORQUE			0x6076, 0x00 //u32
#define EPOS4_ACTUAL_CURRENT				0x30D1, 0x02 //i32
#define EPOS4_DEMAND_CURRENT				0x30D0, 0x00 //i32
#define EPOS4_ACTUAL_VELOCITY_AVG			0x30D3, 0x01 //i32
#define EPOS4_HOMING_SPEED_SEARCH			0x6099, 0x01 //u32
#define EPOS4_HOMING_SPEED_ZERO				0x6099, 0x02 //u32
#define EPOS4_HOMING_ACC					0x609A, 0x00

#define EPOS4_INTERP_TIME					0x60C2, 0x01 // u8
#define EPOS4_INTERP_EXP					0x60C2, 0x02 // i8
#define EPOS4_POSITION_OFFSET				0x60B0, 0x00 //i32
#define EPOS4_VELOCITY_OFFSET				0x60B1, 0x00 //i32
#define EPOS4_TORQUE_OFFSET					0x60B2, 0x00 //i32

#define EPOS4_MOTOR_NOMINAL_CURRENT	0x3001, 0x01 //U32
#define EPOS4_MOTOR_CURRENT_LIMIT	0x3001, 0x02 //U32
#define EPOS4_MOTOR_POLE_PAIRS		0x3001, 0x03 //U8
#define EPOS4_MOTOR_THERMAL_CST		0x3001, 0x04 //U16
#define EPOS4_MOTOR_TORQUE_CST		0x3001, 0x05 //U32

#define EPOS4_ENC1_NB_PULSES		0x3010, 0x01 //u32
#define EPOS4_ENC1_TYPE				0x3010, 0x02 //u16
#define EPOS4_ENC1_INDEX			0x3010, 0x03 //i32
#define EPOS4_HALL_TYPE				0x301A, 0x01 //u16
#define EPOS4_HALL_PATTERN			0x301A, 0x02 //u16

#define EPOS4_CUR_CTRL_P			0x30A0, 0x01 //u32
#define EPOS4_CUR_CTRL_I			0x30A0, 0x02 //u32
#define EPOS4_PPM_CTRL_P			0x30A1, 0x01 //u32
#define EPOS4_PPM_CTRL_I			0x30A1, 0x02 //u32
#define EPOS4_PPM_CTRL_D			0x30A1, 0x03 //u32
#define EPOS4_PPM_CTRL_FFV			0x30A1, 0x04 //u32
#define EPOS4_PPM_CTRL_FFA			0x30A1, 0x05 //u32


//CONTROLWORD_COMMANDS

#define EPOS4_CW_SHUTDOWN			0x0006
#define EPOS4_CW_SOENABLE			0x000F
#define EPOS4_CW_DISABLE			0x0007

//PPM SPECIFIC START
#define EPOS4_CW_PPM_ABSOLUTE		0x001F	//start absolute
#define EPOS4_CW_PPM_RELATIVE		0x007F	//start relative
#define EPOS4_CW_PPM_ABSOLUTE_I		0x003F	//start absolute immediately
#define EPOS4_CW_PPM_RELATIVE_I		0x005F  //start relative immediately

#define EPOS4_CW_HOM_START			0x001F	//start homing



//STATUSWORD BIT TESTS
#define EPOS4_SW_READY_TO_SWITCH_ON(sw) 	((sw & 0x0001)?1:0)
#define EPOS4_SW_SWITCHED_ON(sw) 			((sw & 0x0002)?1:0)
#define EPOS4_SW_ENABLED(sw) 				((sw & 0x0004)?1:0)
#define EPOS4_SW_FAULT(sw)			 		((sw & 0x0008)?1:0)
#define EPOS4_SW_VOLTAGE_ENABLED(sw) 		((sw & 0x0010)?1:0)
#define EPOS4_SW_QUICKSTOP(sw)		 		((sw & 0x0020)?1:0)
#define EPOS4_SW_SWITCH_ON_DISABLED(sw) 	((sw & 0x0040)?1:0)
#define EPOS4_SW_WARNING(sw) 				((sw & 0x0080)?1:0)
#define EPOS4_SW_REMOTE(sw) 				((sw & 0x0200)?1:0)
#define EPOS4_SW_TARGET_REACHED(sw) 		((sw & 0x0400)?1:0)
#define EPOS4_SW_LIMIT_ACTIVE(sw) 			((sw & 0x0800)?1:0)
#define EPOS4_SW_SET_ACK(sw) 				((sw & 0x1000)?1:0)
#define EPOS4_SW_HOMING_ATTAINED(sw) 		((sw & 0x1000)?1:0)
#define EPOS4_SW_SPECIFIC_ERROR(sw) 		((sw & 0x2000)?1:0)
#define EPOS4_SW_POSITION_REF_TO_HOME(sw) 	((sw & 0x8000)?1:0)

#endif /* EPOS4_DEF_H */
