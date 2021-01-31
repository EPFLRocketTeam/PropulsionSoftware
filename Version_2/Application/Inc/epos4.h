/*  Title       : Maxon EPOS4 driver
 *  Filename    : epos4.h
 *  Author      : iacopo sprenger
 *  Date        : 25.01.2021
 *  Version     : 0.1
 *  Description : Maxon epos 4 board driver
 */

#ifndef EPOS4_H
#define EPOS4_H

/**********************
 *  INCLUDES
 **********************/

#include <stdint.h>

/**********************
 *  CONSTANTS
 **********************/



/**********************
 *  MACROS
 **********************/


/**********************
 *  TYPEDEFS
 **********************/

typedef struct EPOS4_INST EPOS4_INST_t;

typedef enum EPOS4_STATE {
	EPOS4_PPM,
	EPOS4_HOM,
	EPOS4_CSP
}EPOS4_STATE_t;

typedef enum EPOS4_ERROR {
	EPOS4_SUCCESS = 0,
	EPOS4_TIMEOUT,
	EPOS4_REMOTE_ERROR,
	EPOS4_ERROR
}EPOS4_ERROR_t;

typedef struct EPOS4_PPM_CONFIG {

}EPOS4_PPM_CONFIG_t;

typedef struct EPOS4_CSP_CONFIG {

}EPOS4_CSP_CONFIG_t;

typedef struct EPOS4_HOM_CONFIG {

}EPOS4_HOM_CONFIG_t;


/**********************
 *  VARIABLES
 **********************/


/**********************
 *  PROTOTYPES
 **********************/

#ifdef __cplusplus
extern "C"{
#endif

//MISC
void epos4_global_init();
void epos4_init(EPOS4_INST_t * epos4, uint8_t id);

//LOW LEVEL
EPOS4_ERROR_t epos4_readobject(EPOS4_INST_t * epos4, uint16_t index, uint8_t subindex, uint8_t * data, uint32_t * err);
EPOS4_ERROR_t epos4_writeobject(EPOS4_INST_t * epos4, uint16_t index, uint8_t subindex, uint8_t * data, uint32_t * err);


//MEDIUM LEVEL
EPOS4_ERROR_t epos4_get_status(EPOS4_INST_t * epos4, uint16_t * status);
EPOS4_ERROR_t epos4_startup(EPOS4_INST_t * epos4 );
EPOS4_ERROR_t epos4_enable(EPOS4_INST_t * epos4);
EPOS4_ERROR_t epos4_disable(EPOS4_INST_t * epos4);

//HIGH LEVEL
EPOS4_ERROR_t epos4_setmode_ppm(EPOS4_INST_t * epos4);
EPOS4_ERROR_t epos4_setmode_hom(EPOS4_INST_t * epos4);
EPOS4_ERROR_t epos4_setmode_csp(EPOS4_INST_t * epos4);


EPOS4_ERROR_t epos4_ppm_move(EPOS4_INST_t * epos4, int32_t target);
EPOS4_ERROR_t epos4_ppm_config(EPOS4_INST_t * epos4, EPOS4_PPM_CONFIG_t config);

EPOS4_ERROR_t epos4_csp_move(EPOS4_INST_t * epos4, int32_t target);
EPOS4_ERROR_t epos4_csp_config(EPOS4_INST_t * epos4, EPOS4_CSP_CONFIG_t config);

EPOS4_ERROR_t epos4_hom_move(EPOS4_INST_t * epos4);
EPOS4_ERROR_t epos4_hom_config(EPOS4_INST_t * epos4, EPOS4_HOM_CONFIG_t config);


#ifdef __cplusplus
} // extern "C"
#endif /* __cplusplus */

#endif /* EPOS4_H */

/* END */
