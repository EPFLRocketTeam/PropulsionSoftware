/*
 * storage.c
 *
 * author: Iacopo Sprenger
 */



#include <main.h>
#include <storage.h>
#include <flash.h>
#include <rocket_fs.h>
#include <maxon_comm.h>

//#define UNSTABLE
#ifdef UNSTABLE

static FileSystem fs = { 0 };

static File * test;



void storage_init() {
	flash_init();

	rocket_fs_device(&fs, "NOR Flash", 4096 * 4096, 4096);
	rocket_fs_bind(&fs, &flash_read, &flash_write, &flash_erase_subsector);
	rocket_fs_mount(&fs);

	test = rocket_fs_newfile(&fs, "test", RAW);
}


void test_write(int32_t data) {

	Stream stream;
	rocket_fs_stream(&stream, &fs, test, OVERWRITE);
	stream.write32(data);
	stream.close();
}

int32_t test_read(void) {

	Stream stream;
	static int32_t buffer;
	rocket_fs_stream(&stream, &fs, test, OVERWRITE);
	stream.read32(&buffer);
	stream.close();
	return buffer;
}

#else

void test_write(int32_t data) {

}
int32_t test_read(void) {
	return 0;
}
#endif
