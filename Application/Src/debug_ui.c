/*
 *
 * debug_ui.c
 * autheur: Iacopo
 */


/*
 * The ui should be like a terminal. the user enters text commands with arguments
 * (numerical and potentially multiple)
 *
 * examples: enable, disable, quickstop, setup_ppm var1 var2 var3
 *
 */

#include <debug_ui.h>
#include <stdio.h>
#include "usart.h"
#include <comm.h>
#include <string.h>
#include <maxon_comm.h>
#include <maxon_def.h>
#include <sensor.h>
#include <control.h>



#define TEST_BOARD 0

#if TEST_BOARD == 1

#include <can_comm.h>

#endif


#define CHAR_CR	0x0D
#define CHAR_LF	0x0A
#define CHAR_TAB 0x09


typedef struct {
	uint8_t name[DUI_NAME_LEN];
	uint8_t nb_args;
	void (*func)(uint16_t nb, int32_t * in, uint8_t * out);

}DUI_ITEM_t;


//local menu functions prototypes
void ui_help(uint16_t nb, int32_t * in, uint8_t * out);
void ui_echo(uint16_t nb, int32_t * in, uint8_t * out);
void ui_status(uint16_t nb, int32_t * in, uint8_t * out);
void ui_setup(uint16_t nb, int32_t * in, uint8_t * out);
void ui_enable(uint16_t nb, int32_t * in, uint8_t * out);
void ui_disable(uint16_t nb, int32_t * in, uint8_t * out);
void ui_startup(uint16_t nb, int32_t * in, uint8_t * out);
void ui_shutdown(uint16_t nb, int32_t * in, uint8_t * out);
void ui_homing(uint16_t nb, int32_t * in, uint8_t * out);
void ui_reg_ppm_profile(uint16_t nb, int32_t * in, uint8_t * out);
void ui_reg_op_profile(uint16_t nb, int32_t * in, uint8_t * out);
void ui_operation(uint16_t nb, int32_t * in, uint8_t * out);
void ui_abort(uint16_t nb, int32_t * in, uint8_t * out);
void ui_move_abs(uint16_t nb, int32_t * in, uint8_t * out);
void ui_move_rel(uint16_t nb, int32_t * in, uint8_t * out);
void ui_short_stat(uint16_t nb, int32_t * in, uint8_t * out);
void ui_short_ppm(uint16_t nb, int32_t * in, uint8_t * out);
void ui_short_op(uint16_t nb, int32_t * in, uint8_t * out);
void ui_short_sensors(uint16_t nb, int32_t * in, uint8_t * out);
void ui_solenoid(uint16_t nb, int32_t * in, uint8_t * out);
void ui_get_object(uint16_t nb, int32_t * in, uint8_t * out);
void ui_set_object(uint16_t nb, int32_t * in, uint8_t * out);



const char help_text[] = 	"debug ui help:\n"
							"help		: this page\n"
							"echo num	: test function\n"
							"status		: maxon motor status\n"
							"setup		: maxon motor setup\n"
							"enable		: enable motor\n"
							"disable	: disable motor\n"
							"startup	: start motor\n"
							"shutdown	: shutdown motor\n"
							"move_abs target\n"
							"			: move to target\n"
							"move_rel target\n"
							"			: move +/- target\n"
							"homing 	: start homing\n"
							"ppm_profile speed acc dec\n"
							"			: setup ppm profile\n"
							"op_profile half wait1 full wait2\n"
							"			: setup operation profile\n"
							"operation	: start operation\n";

static DUI_ITEM_t ui_items[] = {
		{"help", 0, ui_help},
		{"echo", 1, ui_echo},
		{"status", 0, ui_status},
		{"setup", 0, ui_setup},
		{"enable", 0, ui_enable},
		{"disable", 0, ui_disable},
		{"startup", 0, ui_startup},
		{"shutdown", 0, ui_shutdown},
		{"move_abs", 1, ui_move_abs},
		{"move_rel", 1, ui_move_rel},
		{"homing", 0, ui_homing},
		{"ppm_profile", 3, ui_reg_ppm_profile},
		{"op_profile", 5, ui_reg_op_profile},
		{"operation", 0, ui_operation},
		{"abort", 0, ui_abort},
		{"short_stat", 0, ui_short_stat},
		{"short_ppm", 0, ui_short_ppm},
		{"short_op", 0, ui_short_op},
		{"short_sensors", 0, ui_short_sensors},
		{"solenoid", 0, ui_solenoid},
		{"get_object", 2, ui_get_object},
		{"set_object", 3, ui_set_object}
};




int8_t	debug_ui_decode(uint8_t d, uint8_t * entry) {
	static uint16_t counter = 0;

	if(!(counter < DUI_ENTRY_LEN)) {
		counter = 0;
		return -1;
	}
	if((d >= ' ') && (d <= '}')) {
		entry[counter++] = d;
		return -1;
	} else if (d == CHAR_LF || d == CHAR_CR) {
		entry[counter] = '\0';
		counter = 0;
		return 1;
	} else if(d == CHAR_TAB) {
		entry[counter++] = ' ';
		return -1;
	}
	return -1;

}

uint8_t str_cmp_to_space(uint8_t * a, uint8_t * b, uint16_t la, uint16_t lb) {
	uint16_t i = 0;
	while((a[i] > ' ') || (b[i] > ' ')) {
		if(a[i] != b[i]) {
			return 0;
		}
		i += 1;
		if((i == la) || (i == lb)) {
			return 0;
		}
	}
	return 1;
}

uint16_t str_len(uint8_t * a, uint16_t la) {
	uint16_t i = 0;
	while(a[i] != '\0') {
		if(i >= la) {
			return i;
		}
		i++;
	}
	return i;
}

uint8_t * next_space(uint8_t * str) {
	uint16_t i = 0;
	while (str[i] != '\0') {
		if(str[i++] == ' ') {
			return str+i*sizeof(uint8_t);
		}
	}
	return NULL;
}


int32_t power(int32_t base, int32_t exp) {
	int32_t res = 1;
	for(uint16_t i = 0; i < exp; i++) {
		res *= base;
	}
	return res;
}

uint8_t read_int32(uint8_t * str, int32_t * val) {
	uint16_t i = 0;
	int8_t sign = 1;
	uint8_t started = 0;
	int32_t res = 0;
	uint16_t digits = 0;
	//count digits
	while(str[i] != '\0') {
		if(str[i] == '-' && !started) {
			started = 1;
		} else if(str[i] <= '9' && str[i] >= '0') {
			started = 1;
			digits++;
		} else if(started == 1 && str[i] == ' ') {
			break;
		} else if (started == 1){
			return 0;
		}
		i++;
	}

	i = 0;
	started = 0;
	//decode
	while(str[i] != '\0') {
		if(str[i] == '-' && !started) {
			sign = -1;
			started = 1;
		} else if(str[i] <= '9' && str[i] >= '0') {
			started = 1;
			res += power(10, --digits)*(str[i]-'0');

		} else if(started == 1 && str[i] == ' ') {
			break;
		} else if (started == 1){
			return 0;
		}
		i++;
	}
	*(val) = sign*res;
	return 1;
}


void debug_ui_receive(uint8_t recvBuffer) {
	static uint8_t entry[DUI_ENTRY_LEN];
	static uint8_t resp[DUI_RESP_LEN];
	static int32_t args[DUI_MAX_ARGS];
	//some uin32_t  vals wont be accessible but
	//not a prob because we dont use such big vals anyway!
	int8_t res  = debug_ui_decode(recvBuffer, entry);
	if(res == 1) {
		for(uint16_t i = 0; i < sizeof(ui_items)/sizeof(DUI_ITEM_t); i++) {
			if(str_cmp_to_space(entry, ui_items[i].name, DUI_ENTRY_LEN, DUI_NAME_LEN)) {
				uint8_t * pstr = next_space(entry);
				uint8_t args_read = 0;
				//read arguments
				for(uint8_t j = 0; j < ui_items[i].nb_args; j++) {
					if(!pstr) {
						break;
					}
					if(read_int32(pstr, &(args[j])) == 1) {
						pstr = next_space(pstr);
						args_read++;
					} else {
						break;
					}
				}
				//invoke function
				resp[0] = '\0'; //clear response
				ui_items[i].func(args_read, args, resp);

				HAL_UART_Transmit(&UI_UART, resp, str_len(resp, DUI_RESP_LEN), 500);
				return;
			}
		}
	}
}


//display help message
void ui_help(uint16_t nb, int32_t * in, uint8_t * out) {
	sprintf((char *) out, help_text);
}

void ui_echo(uint16_t nb, int32_t * in, uint8_t * out) {
	if(nb > 0) {
		sprintf((char *) out, "number entered: %ld\n", in[0]);
	} else {
		sprintf((char *) out, "number entered: none\n");
	}
}

void ui_status(uint16_t nb, int32_t * in, uint8_t * out) {
	uint16_t status = motor_get_status();
	sprintf((char *) out, "status:\nready: %d\nswitched on: %d\nenabled: %d\n"
						"fault: %d\nvoltage enabled: %d\nquickstop: %d\ntarget reached: %d\n"
						"set ack/homing attained: %d\nlimit active: %d\nerror: %x\nposition: %ld\n",
						SW_READY_TO_SWITCH_ON(status), SW_SWITCHED_ON(status), SW_ENABLED(status),
						SW_FAULT(status), SW_VOLTAGE_ENABLED(status), SW_QUICKSTOP(status),
						SW_TARGET_REACHED(status), SW_SET_ACK(status), SW_LIMIT_ACTIVE(status),
						motor_get_error(), motor_get_position());

}

void ui_setup(uint16_t nb, int32_t * in, uint8_t * out) {
	motor_def_config();
}

void ui_startup(uint16_t nb, int32_t * in, uint8_t * out) {
	motor_def_startup();
}

void ui_shutdown(uint16_t nb, int32_t * in, uint8_t * out) {
	motor_def_shutdown();
}

void ui_enable(uint16_t nb, int32_t * in, uint8_t * out) {
	motor_def_enable();
}

void ui_disable(uint16_t nb, int32_t * in, uint8_t * out) {
	motor_def_disable();
}

void ui_reg_ppm_profile(uint16_t nb, int32_t * in, uint8_t * out) {
	if(nb >= 3) { //check that the accelerations are positive!
		motor_register_speed(in[0]);
		motor_register_acceleration(in[1]);
		motor_register_deceleration(in[2]);
		sprintf((char *) out, "speed: %ld [rmp]\nacc: %ld [rmp/s]\ndec: %ld [rpm/s]\n", in[0], in[1], in[2]);
	}else{
		sprintf((char *) out, "speed: %ld [rmp]\nacc: %ld [rmp/s]\ndec: %ld [rpm/s]\n", motor_get_ppm_speed(), motor_get_ppm_acceleration(), motor_get_ppm_deceleration());
	}

}
void ui_reg_op_profile(uint16_t nb, int32_t * in, uint8_t * out) {
	if(nb >= 5) {
		motor_register_pre_wait(in[0]);
		motor_register_half_target(DDEG2INC(in[1]));
		motor_register_half_wait(in[2]);
		motor_register_target(DDEG2INC(in[3]));
		motor_register_end_wait(in[4]);
		sprintf((char *) out, "pre wait: %ld\nhalf target: %ld [0.1deg]\nwait 1: %ld [ms]\ntarget: %ld  [0.1deg]\nwait 2: %ld [ms]\n", in[0], in[1], in[2], in[3], in[4]);
	} else{
		sprintf((char *) out, "pre wait: %ld\nhalf target: %ld [0.1deg]\nwait 1: %ld [ms]\ntarget: %ld  [0.1deg]\nwait 2: %ld [ms]\n",
				motor_get_pre_wait(), INC2DDEG(motor_get_half_target()), motor_get_half_wait(), INC2DDEG(motor_get_target()), motor_get_end_wait());
	}
}
void ui_operation(uint16_t nb, int32_t * in, uint8_t * out) {
	motor_def_start_operation();

#if TEST_BOARD == 1
	can_setFrame(0, DATA_ID_START_OPERATION, 0);
#endif
}

void ui_abort(uint16_t nb, int32_t * in, uint8_t * out) {
	motor_def_abort();

#if TEST_BOARD == 1
	can_setFrame(0, DATA_ID_ABORT, 0);
#endif

}

void ui_move_abs(uint16_t nb, int32_t * in, uint8_t * out) {
	if(nb > 0) {
		motor_def_set_ppm();
		motor_register_tmp_target(DDEG2INC(in[0]));
		motor_register_absolute();
		motor_def_start_ppm_operation();
		sprintf((char *) out, "started %ld\n", DDEG2INC(in[0]));
	}
}

void ui_move_rel(uint16_t nb, int32_t * in, uint8_t * out) {
	if(nb > 0) {
		motor_def_set_ppm();
		motor_register_tmp_target(DDEG2INC(in[0]));
		motor_register_relative();
		motor_def_start_ppm_operation();
		sprintf((char *) out, "started %ld\n", DDEG2INC(in[0]));
	}
}

void ui_homing(uint16_t nb, int32_t * in, uint8_t * out) {
	motor_def_start_homing_operation();
}

void ui_short_stat(uint16_t nb, int32_t * in, uint8_t * out) {
	uint16_t status = motor_get_status();
	sprintf((char *) out, "%d %d %d %d %x %ld %d %d %lx\n",SW_SWITCHED_ON(status), SW_ENABLED(status), SW_FAULT(status),
			SW_TARGET_REACHED(status), motor_get_error(), INC2DDEG(motor_get_position()),
			motor_get_psu_voltage(), motor_get_torque(), motor_get_custom_object());
}

void ui_short_ppm(uint16_t nb, int32_t * in, uint8_t * out) {
	sprintf((char *) out, "%ld %ld %ld\n", motor_get_ppm_speed(), motor_get_ppm_acceleration(), motor_get_ppm_deceleration());
}

void ui_short_op(uint16_t nb, int32_t * in, uint8_t * out) {
	sprintf((char *) out, "%ld %ld %ld %ld %ld\n", motor_get_pre_wait(), INC2DDEG(motor_get_half_target()), motor_get_half_wait(), INC2DDEG(motor_get_target()), motor_get_end_wait());
}

void ui_short_sensors(uint16_t nb, int32_t * in, uint8_t * out) {
	sprintf((char *) out, "%d %d %d %d %d %lu\n", 	sensor_get_data(PP_PRESSURE_1), sensor_get_data(PP_PRESSURE_2), sensor_get_data(PP_TEMPERATURE_1),
												sensor_get_data(PP_TEMPERATURE_2), sensor_get_data(PP_TEMPERATURE_3), sensor_get_time());
}

void ui_solenoid(uint16_t nb, int32_t * in, uint8_t * out) {
	sprintf((char *) out, "%d \n", toggle_solenoid());
}

void ui_get_object(uint16_t nb, int32_t * in, uint8_t * out) {
	if(nb >= 2) {
		motor_register_custom_index(in[0]);
		motor_register_custom_subindex(in[1]);
	}
}

void ui_set_object(uint16_t nb, int32_t * in, uint8_t * out) {
	if(nb >= 3) {
		motor_register_custom_index(in[0]);
		motor_register_custom_subindex(in[1]);
		motor_register_custom_write(in[2]);
		motor_def_custom_write();
	}
}






