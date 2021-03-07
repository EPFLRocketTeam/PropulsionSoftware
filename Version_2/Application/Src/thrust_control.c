/*  Title		: Thrust control
 *  Filename	: thrust_control.c
 *	Author		: iacopo sprenger
 *	Date		: 03.03.2021
 *	Version		: 0.1
 *	Description	: thrust control algorithm implementation
 *				  The algorithm was developped by Antoine Faltz and
 *				  Luca Rezzonico
 *
 *				  The algorithm will take as input the CC pressure and thrust command value
 *				  and output the desired valve opening angle
 *
 *				  controller: c = tf([1 28.6 484], [1 200 0])
 *				  discretization: d = c2d(c, 0.01)
 *
 *				  -->
 *				    z^2 - 1.863 z + 0.8835
 *	  	  	  	  	----------------------
 *	  	  	  	  	z^2 - 1.135 z + 0.1353
 *
 *
 *	  	  	  	  	en ss:
 *
 *	  	  	  	  	  A =
								x1       x2
					   x1    1.135  -0.2707
					   x2      0.5        0

					  B =
						   u1
					   x1   2
					   x2   0

					  C =
								x1       x2
					   y1  -0.3636   0.7482

					  D =
						   u1
					   y1   1


	u1 = error
	y1 = gen control sig

 */


/**********************
 *	INCLUDES
 **********************/

#include "thrust_control.h"
#include "util.h"

/**********************
 *	CONSTANTS
 **********************/


#define TC_A11 util_double_2_fix(1.35)
#define TC_A12 util_double_2_fix(-0.2707)
#define TC_A21 util_double_2_fix(0.5)
#define TC_A22 util_double_2_fix(0)

#define TC_B11 util_double_2_fix(2)
#define TC_B21 util_double_2_fix(0)

#define TC_C11 util_double_2_fix(-0.3636)
#define TC_C12 util_double_2_fix(0.7482)

#define TC_D11 util_int_2_fix(1)



/**********************
 *	MACROS
 **********************/




/**********************
 *	TYPEDEFS
 **********************/


/**********************
 *	VARIABLES
 **********************/

static UTIL_MAT21_t state;

static UTIL_MAT22_t TC_A = {
		TC_A11, TC_A12,
		TC_A21, TC_A22
};

static UTIL_MAT21_t TC_B = {
		TC_B11,
		TC_B21
};

static UTIL_MAT12_t TC_C = {
		TC_C11,
		TC_C12
};

static int32_t TC_D = TC_D11;





/**********************
 *	PROTOTYPES
 **********************/

static int32_t tc_eval_ss(int32_t error);





/**********************
 *	DECLARATIONS
 **********************/

void tc_init() {
	state.x11 = 0;
	state.x21 = 0;
}


static int32_t tc_eval_ss(int32_t error) {
	int32_t command;
	state = util_fix_mat21_add_mat21(util_fix_mat22_mul_mat21(TC_A, state), util_fix_fix_mul_mat21(error, TC_B));
	command = util_fix_mat12_mul_mat21(TC_C, state) + util_fix_mul(error, TC_D);
	return command;
}



//CC_pressure in mbar, desired thrust in N time in
//This function should be called every 10ms
//Returns the motor angle
int32_t tc_compute(int32_t cc_pressure, int32_t desired_thrust) {
	int32_t angle;
	int32_t error = util_int_2_fix(desired_thrust - TC_PRESSURE_2_THRUST(TC_MBAR_2_PASCAL(cc_pressure)));
	angle = tc_eval_ss(error);
	return angle;
}



/* END */


