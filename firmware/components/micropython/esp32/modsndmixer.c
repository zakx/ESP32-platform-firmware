#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "py/mperrno.h"
#include "py/mphal.h"
#include "py/runtime.h"

#include "sndmixer.h"

static mp_obj_t modsndmixer_start() {
	sndmixer_init(2, 22050);
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_0(modsndmixer_start_obj, modsndmixer_start);

static mp_obj_t modsndmixer_wav(mp_obj_t _data) {
	mp_uint_t len;
	if (!MP_OBJ_IS_TYPE(_data, &mp_type_bytes)) {
		mp_raise_ValueError("Expected a bytestring like object.");
		return mp_const_none;
	}
	uint8_t *data = (uint8_t *)mp_obj_str_get_data(_data, &len);

	int id=sndmixer_queue_wav(data, data+len-1, 1);
	sndmixer_play(id);
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(modsndmixer_wav_obj, modsndmixer_wav);

static mp_obj_t modsndmixer_mod(mp_obj_t _data) {
	mp_uint_t len;
	if (!MP_OBJ_IS_TYPE(_data, &mp_type_bytes)) {
		mp_raise_ValueError("Expected a bytestring like object.");
		return mp_const_none;
	}
	uint8_t *data = (uint8_t *)mp_obj_str_get_data(_data, &len);

	int id=sndmixer_queue_mod(data, data+len-1);
	sndmixer_play(id);
	return mp_const_none;
}
static MP_DEFINE_CONST_FUN_OBJ_1(modsndmixer_mod_obj, modsndmixer_mod);

static const mp_rom_map_elem_t sndmixer_module_globals_table[] = {
	{MP_ROM_QSTR(MP_QSTR_start), MP_ROM_PTR(&modsndmixer_start_obj)},
	{MP_ROM_QSTR(MP_QSTR_wav), MP_ROM_PTR(&modsndmixer_wav_obj)},
	{MP_ROM_QSTR(MP_QSTR_mod), MP_ROM_PTR(&modsndmixer_mod_obj)},
};

static MP_DEFINE_CONST_DICT(sndmixer_module_globals, sndmixer_module_globals_table);

const mp_obj_module_t sndmixer_module = {
	.base = {&mp_type_module},
	.globals = (mp_obj_dict_t *)&sndmixer_module_globals,
};
