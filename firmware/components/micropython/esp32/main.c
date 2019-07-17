/*
 * This file is part of the MicroPython ESP32 project, https://github.com/loboris/MicroPython_ESP32_psRAM_LoBo
 *
 * The MIT License (MIT)
 *
 * Copyright (c) 2018 LoBo (https://github.com/loboris)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stdio.h>
#include <string.h>
#include <time.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_task_wdt.h"
#include "esp_heap_caps.h"
#include "esp_system.h"
#include "nvs_flash.h"
#include "esp_task.h"
#include "soc/cpu.h"
#include "esp_log.h"
#include "driver/periph_ctrl.h"
#include "esp_pm.h"

#include "py/stackctrl.h"
#include "py/nlr.h"
#include "py/compile.h"
#include "py/runtime.h"
#include "py/repl.h"
#include "py/gc.h"
#include "py/mphal.h"
#include "extmod/vfs.h"
#include "extmod/vfs_native.h"
#include "lib/mp-readline/readline.h"
#include "lib/utils/pyexec.h"
#include "uart.h"
#include "modmachine.h"
#include "mpthreadport.h"
#include "mpsleep.h"
#include "machine_rtc.h"
#ifdef CONFIG_MICROPY_USE_FTPSERVER
#include "libs/ftp.h"
#endif

#include "driver/uart.h"
#include "rom/uart.h"
#include "sdkconfig.h"


#define NVS_NAMESPACE       "system"
#define MP_TASK_STACK_LEN	4096

#if CONFIG_SPIRAM_IGNORE_NOTFOUND
extern bool s_spiram_okay;
#endif

static StaticTask_t DRAM_ATTR mp_task_tcb;
static StackType_t *mp_task_stack;
static StackType_t *mp_task_stack_end;
static int mp_task_stack_len = 4096;
static uint8_t *mp_task_heap = NULL;

void nlr_jump_fail(void *val) {
    printf("RESET: NLR jump failed, val=%p\n", val);
    //prepareSleepReset(1, NULL);
    esp_restart();
}

int MainTaskCore = 0;

bool renze_micropython_app_start;
char* renze_micropython_app_name;

void mp_task_renze(void *pvParameter)
{
	while (true) {
		volatile uint32_t sp = (uint32_t)get_sp();
		uart_init();
		mpsleep_init0();
		rtc_init0();
		mp_thread_preinit(mp_task_stack, mp_task_stack_len);

		// Initialize the stack pointer for the main thread
		mp_stack_set_top((void *)sp);
		mp_stack_set_limit(mp_task_stack_len - 1024);

		// Initialize the MicroPython heap
		gc_init(mp_task_heap, mp_task_heap + mpy_heap_size);

		// Initialize MicroPython environment
		mp_init();
		mp_obj_list_init(mp_sys_path, 0);
		mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR_));
		mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_lib));
		mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_apps));
		mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_sd__slash_lib));
		mp_obj_list_append(mp_sys_path, MP_OBJ_NEW_QSTR(MP_QSTR__slash_sd__slash_apps));
		mp_obj_list_init(mp_sys_argv, 0);

		readline_init0();
		machine_pins_init();
		int res = mount_vfs(VFS_NATIVE_TYPE_SPIFLASH, VFS_NATIVE_INTERNAL_MP);
		if (res != 0) ESP_LOGE("MicroPython", "Error mounting Flash file system");
		
		//pyexec_frozen_module("_boot.py");
		//if (pyexec_mode_kind == PYEXEC_MODE_FRIENDLY_REPL) pyexec_frozen_module("boot.py");
		
		gc_info_t info;
		gc_info(&info);
		#ifdef CONFIG_MICROPY_GC_SET_THRESHOLD
		MP_STATE_MEM(gc_alloc_threshold) = ((info.total * 100) / CONFIG_MICROPY_GC_THRESHOLD_VALUE) / MICROPY_BYTES_PER_GC_BLOCK;
		#endif

		MP_THREAD_GIL_EXIT();

		if (renze_micropython_app_start) {//(strlen(renze_micropython_app_name) > 0) {
			renze_micropython_app_start = false;
			printf("Starting app %s!\n", renze_micropython_app_name);
			/*FILE *fd;
			fd = fopen(renze_micropython_app_name, "rb");
            if (fd) {
            	fclose(fd);*/
            	pyexec_file(renze_micropython_app_name);
           /* } else {
				printf("File not found.");
			}*/
			//renze_micropython_app_name[0] = 0; //Clear string so that repl boots next.
		} else {
			printf("Starting repl / normal boot!\n");
			pyexec_frozen_module("_boot.py");
			pyexec_frozen_module("boot.py");
			ReplTaskHandle = MainTaskHandle;
			pyexec_friendly_repl();
		}
		
		//ReplTaskHandle = MainTaskHandle;
		//pyexec_friendly_repl();
	}
}

void micropython_renze(void)
{
	MPY_DEFAULT_STACK_SIZE = CONFIG_MICROPY_STACK_SIZE * 1024;
	MPY_DEFAULT_HEAP_SIZE = CONFIG_MICROPY_HEAP_SIZE * 1024;
	
	renze_micropython_app_name = malloc(1024);
	if (renze_micropython_app_name == NULL) {printf("MALLOC FAIL"); return;}
	renze_micropython_app_name[0] = 0;
	renze_micropython_app_start = false;

	//Configuration
	mp_task_stack_len =MPY_DEFAULT_STACK_SIZE; //64*1024;
	mpy_heap_size     = MPY_DEFAULT_HEAP_SIZE; //3584*1024;

	//Stack
	mp_task_stack_len &= 0x7FFFFFF8;
	mp_task_stack_len = mp_task_stack_len / sizeof(StackType_t);
	mp_task_stack = heap_caps_malloc((mp_task_stack_len * sizeof(StackType_t))+8, MALLOC_CAP_8BIT | MALLOC_CAP_INTERNAL);
	if (mp_task_stack == NULL) { ESP_LOGE("MicroPython", "Error allocating stack, HALTED."); return; }
	mp_task_stack_end = mp_task_stack + ((mp_task_stack_len * sizeof(StackType_t)) + 8);
	//ESP_LOGE("MicroPython", "MPy stack: %p - %p (%d)", mp_task_stack, mp_task_stack_end, mp_task_stack_len+8);
	
    //Heap
    mpy_heap_size &= 0x7FFFFFF0;	
	mp_task_heap = malloc(mpy_heap_size+16);
    if (mp_task_heap == NULL) { ESP_LOGE("MicroPython", "Error allocating heap, HALTED."); return; }
	//ESP_LOGE("MicroPython", "MPy heap: %p - %p (%d)", mp_task_heap, mp_task_heap+mpy_heap_size+64, mpy_heap_size);

	//Creating the microPython task
	MainTaskHandle = xTaskCreateStaticPinnedToCore(&mp_task_renze, "mp_task_renze", mp_task_stack_len, NULL, CONFIG_MICROPY_TASK_PRIORITY, mp_task_stack, &mp_task_tcb, 0);

	//After task exits
	if (!MainTaskHandle) ESP_LOGE("MicroPython", "Error creating MicroPython task, HALTED.");
	//ESP_LOGE("MicroPython", "Main task exit, stack used: %d", CONFIG_MAIN_TASK_STACK_SIZE - uxTaskGetStackHighWaterMark(NULL));
}

