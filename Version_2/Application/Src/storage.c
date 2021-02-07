/*  Title		: Storage
 *  Filename	: storage.c
 *	Author		: iacopo sprenger
 *	Date		: 07.02.2021
 *	Version		: 0.1
 *	Description	: storage on the onboard flash memory
 */

/**********************
 *	INCLUDES
 **********************/

#include "storage.h"
#include <flash.h>

/**********************
 *	CONSTANTS
 **********************/

#define MAGIC_NUMBER	0xCBE0C5E6
#define MAGIC_ADDR		0x00000000
#define USED_SS_ADDR	0x00000004
#define STATE_ADDR		0x00000008

#define SUBSECTOR_SIZE	4096
#define SAMPLES_PER_SS	(SUBSECTOR_SIZE/sizeof(DATA_t))

#define NEXT_SUBSECTOR (SUBSECTOR_SIZE*used_subsectors)
#define DATA_START		SUBSECTOR_SIZE
#define NB_SUBSECTOR	4096


/**********************
 *	MACROS
 **********************/

#define ADDRESS(i)		(DATA_START + (i)*sizeof(DATA_t))
#define SUBSECTOR(i)	(ADDRESS(i)>>12)

/**********************
 *	TYPEDEFS
 **********************/


/**********************
 *	VARIABLES
 **********************/


/**********************
 *	PROTOTYPES
 **********************/


/**********************
 *	DECLARATIONS
 **********************/

/* END */


