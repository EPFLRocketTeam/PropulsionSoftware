/*
 * io_driver.c
 *
 *  Created on: 1 Oct 2019
 *      Author: Arion
 */

#include "io_driver.h"
#include "MT25QL128ABA.h"


/*
 * Reads the flag status register and returns the value of the 8-bits register
 */
uint8_t __read_flags() {
	Command cmd = get_default_command();
	with_data(&cmd, 1);

	if(!qspi_run(&cmd, READ_FLAG_STATUS_REGISTER)) {

	}

	uint8_t flags;

	if(!qspi_receive(&flags)) {

	}

	return flags;
}



/*
 * Enables the write latch.
 * This function must be called before each PROGRAM or ERASE operation.
 */
bool __write_enable_latch() {
	Command cmd = get_default_command();

	if(qspi_run(&cmd, WRITE_ENABLE_LATCH)) {
		cmd = get_default_command();

		with_data(&cmd, 1);

		if(qspi_poll(&cmd, READ_STATUS_REGISTER, 1, true)) {
			return true;
		}
	}

	return false;
}

/*
 * Call this function to prevent data corruption when a hardware fault (e.g. protection fault) occurs.
 * Please refer to the documentation for details.
 */
bool __write_disable_latch() {
	Command cmd = get_default_command();
	return qspi_run(&cmd, WRITE_ENABLE_LATCH);
}

/*
 * Initialises the flash driver
 */
void flash_init() {
	uint8_t configuration = 0b00011011; // 1 Dummy cycle
	Command cmd = get_default_command();
	with_data(&cmd, 1);

	__write_enable_latch();

	if(!qspi_run(&cmd, 0x81)) { // Write volatile configuration register

	}

	if(!qspi_transmit(&configuration)) {

	}

	if(!qspi_poll(&cmd, READ_FLAG_STATUS_REGISTER, 7, true)) {

	}
}

/*
 *
 * --- Read operations ---
 *
 * Test providers:
 * 	 - read_ut.c
 *
 */

void flash_read(uint32_t address, uint8_t* buffer, uint32_t length) {
	while(QUADSPI->SR & QUADSPI_SR_BUSY);
	QUADSPI->CCR = (uint32_t) (FREAD_SINGLE) | (0b00000001 << 24) | (0b00000100 << 16) | (0b00100101 << 8);
	while(QUADSPI->SR & QUADSPI_SR_BUSY);
	QUADSPI->AR = address;
	while(QUADSPI->SR & QUADSPI_SR_BUSY);
	QUADSPI->DLR = length;

	if(!qspi_receive(buffer)) {

	}
}



/*
 *
 * --- Write operations ---
 *
 * Test providers:
 * 	 - write_ut.c
 *
 */

void __flash_write_page(uint32_t address, uint8_t* buffer, uint32_t length) {
	__write_enable_latch();

	Command cmd = get_default_command();

	with_address(&cmd, address);
	with_data(&cmd, length);

	if(!qspi_run(&cmd, WRITE_SINGLE)) {

	}

	if(!qspi_transmit(buffer)) {

	}

	/*
	 * Checks if the controller is ready to proceed to the next command
	 */

	cmd = get_default_command();
	with_data(&cmd, 1);

	if(!qspi_poll(&cmd, READ_FLAG_STATUS_REGISTER, 7, true)) {

	}

	uint8_t flags = __read_flags();

	// Checks if the protection fault flag is set
	if(flags & (1 << 4)) {
		__write_disable_latch(); // Manually reset the latch


	}
}

void flash_write(uint32_t address, uint8_t* buffer, uint32_t length) {
	uint32_t internal_address = address % PAGE_SIZE;

	while(internal_address + length > PAGE_SIZE) {
		uint32_t write_length = PAGE_SIZE - internal_address;

		__flash_write_page(address, buffer, write_length);
		buffer += write_length;
		address += write_length;
		length -= write_length;

		internal_address = 0;
	}

	__flash_write_page(address, buffer, length);
}

/*
 *
 * --- Erase operations ---
 *
 * Test providers:
 * 	 - erase_ut.c
 *
 */
void flash_erase_all() {
   __write_enable_latch();

   Command cmd = get_default_command();

   if(!qspi_run(&cmd, ERASE_ALL)) {

   }

   /*
    * Checks if the controller is ready to proceed to the next command
    */
   cmd = get_default_command();
   with_data(&cmd, 1);

   if(!qspi_poll(&cmd, READ_FLAG_STATUS_REGISTER, 7, true)) {

   }

   /*
    * Checks if the protection fault flag is set
    */
   uint8_t flags = __read_flags();

   if(flags & (1 << 5)) {
      __write_disable_latch(); // Manually reset the latch


   }
}

void __flash_erase(uint32_t instruction, uint32_t address) {

	__write_enable_latch();


	Command cmd = get_default_command();
	with_address(&cmd, address);


	if(!qspi_run(&cmd, instruction)) {

	}

	/*
	 * Checks if the controller is ready to proceed to the next command
	 */
	cmd = get_default_command();
	with_data(&cmd, 1);

	if(!qspi_poll(&cmd, READ_FLAG_STATUS_REGISTER, 7, true)) {

	}

	/*
	 * Checks if the protection fault flag is set
	 */
	uint8_t flags = __read_flags();

	if(flags & (1 << 5)) {
		__write_disable_latch(); // Manually reset the latch


	}
}

/*
 * Erases the whole sector represented by the provided address.
 * The address may be any of those within the sector.
 */
void flash_erase_sector(uint32_t address) {
	__flash_erase(ERASE_SECTOR, address);
}


/*
 * Erases the whole sub-sector represented by the provided address.
 * The address may be any of those within the sub-sector.
 */
void flash_erase_subsector(uint32_t address) {
	__flash_erase(ERASE_SUBSECTOR, address);
}
