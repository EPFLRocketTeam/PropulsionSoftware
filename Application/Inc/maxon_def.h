#ifndef MAXON_DEF_H
#define MAXON_DEF_H


#define MAXON_DEVICE_TYPE		0x1000, 0x00 //U32
#define MAXON_ERROR_REGISTER 	0x1001, 0x00 //U8

#define MAXON_MODE_PPM			1
#define MAXON_MODE_HOMING		6


#define ACTUAL_POSITION			37
//CONTROL

#define MAXON_PSU_VOLTAGE			0x2200, 0x01 //u16

#define MAXON_MOTOR_TYPE			0x6402, 0x00 //u16
#define MAXON_MODE_OF_OPERATION		0x6060, 0x00 //I8
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
#define MAXON_ERROR_WORD			0x603F, 0x00 //u16
#define MAXON_POWER_SUPPLY_VOLTAGE	0x2200, 0x01 //U16
#define MAXON_ABORT_CONNECTION_OPTION_CODE 0x6007, 0x00 //I16
#define MAXON_ELECTRICAL_RESISTANCE			0x3002, 0x01 //u32
#define MAXON_ELECTRICAL_INDUCTANCE			0x3002, 0x02 //u16
#define MAXON_ACTUAL_POSITION				0x6064, 0x00 //i32
#define MAXON_ACTUAL_TORQUE					0x6077, 0x00 //i16
#define MAXON_FOLLOW_ERROR_WINDOW			0x6065, 0x00 //u32
#define MAXON_HOME_POSITION					0x30B0, 0x00 //i32
#define MAXON_HOME_OFFSET					0x30B1, 0x00 //i32
#define MAXON_MOTOR_RATED_TORQUE			0x6076, 0x00 //u32


#define MAXON_MOTOR_NOMINAL_CURRENT	0x3001, 0x01 //U32
#define MAXON_MOTOR_CURRENT_LIMIT	0x3001, 0x02 //U32
#define MAXON_MOTOR_POLE_PAIRS		0x3001, 0x03 //U8
#define MAXON_MOTOR_THERMAL_CST		0x3001, 0x04 //U16
#define MAXON_MOTOR_TORQUE_CST		0x3001, 0x05 //U32

#define MAXON_ENC1_NB_PULSES		0x3010, 0x01 //u32
#define MAXON_ENC1_TYPE				0x3010, 0x02 //u16
#define MAXON_ENC1_INDEX			0x3010, 0x03 //i32
#define MAXON_HALL_TYPE				0x301A, 0x01 //u16
#define MAXON_HALL_PATTERN			0x301A, 0x02 //u16

#define MAXON_CUR_CTRL_P			0x30A0, 0x01 //u32
#define MAXON_CUR_CTRL_I			0x30A0, 0x02 //u32
#define MAXON_PPM_CTRL_P			0x30A1, 0x01 //u32
#define MAXON_PPM_CTRL_I			0x30A1, 0x02 //u32
#define MAXON_PPM_CTRL_D			0x30A1, 0x03 //u32
#define MAXON_PPM_CTRL_FFV			0x30A1, 0x04 //u32
#define MAXON_PPM_CTRL_FFA			0x30A1, 0x05 //u32

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
#define MAXON_CLR_REL			0b00000000
#define MAXON_SET_NEW_POS		0b00010000
#define MAXON_CLR_NEW_POS		0b00000000
#define MAXON_SET_IMM			0b00100000
#define MAXON_CLR_IMM			0b00000000
#define MAXON_SET_HALT			0b100000000
#define MAXON_CLR_HALT			0b000000000
#define MAXON_SET_UNHALT		0b000000000
#define MAXON_CLR_UNHALT		0b100000000

//fixed cw commands
#define MAXON_SWITCH_ON_ENABLE 0x000f
#define MAXON_SHUTDOWN	0x0006
#define MAXON_DISABLE	0x0000



//STATUSWORD BIT TESTS
#define SW_READY_TO_SWITCH_ON(sw) 	((sw & 0x0001)?1:0)
#define SW_SWITCHED_ON(sw) 			((sw & 0x0002)?1:0)
#define SW_ENABLED(sw) 				((sw & 0x0004)?1:0)
#define SW_FAULT(sw)			 	((sw & 0x0008)?1:0)
#define SW_VOLTAGE_ENABLED(sw) 		((sw & 0x0010)?1:0)
#define SW_QUICKSTOP(sw)		 	((sw & 0x0020)?1:0)
#define SW_SWITCH_ON_DISABLED(sw) 	((sw & 0x0040)?1:0)
#define SW_WARNING(sw) 				((sw & 0x0080)?1:0)
#define SW_REMOTE(sw) 				((sw & 0x0200)?1:0)
#define SW_TARGET_REACHED(sw) 		((sw & 0x0400)?1:0)
#define SW_LIMIT_ACTIVE(sw) 		((sw & 0x0800)?1:0)
#define SW_SET_ACK(sw) 				((sw & 0x1000)?1:0)
#define SW_HOMING_ATTAINED(sw) 		((sw & 0x1000)?1:0)
#define SW_SPECIFIC_ERROR(sw) 		((sw & 0x2000)?1:0)
#define SW_POSITION_REF_TO_HOME(sw) ((sw & 0x8000)?1:0)

#endif
