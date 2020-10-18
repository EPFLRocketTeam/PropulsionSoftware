/*
 * storage.c
 *
 * author: Iacopo Sprenger
 */



#include <main.h>
#include <storage.h>
#include <flash.h>
#include <maxon_comm.h>



void storage_init() {
	flash_init();
}


void test_write(int32_t data) {

}

int32_t test_read(void) {
	return 0;
}
