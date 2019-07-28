#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <ibxm/ibxm.h>
#include "snd_source_mod.h"

typedef struct {
	struct module *module;
	struct replay *replay;
	int sample_rate;
} mod_ctx_t;

int mod_init_source(const void *data_start, const void *data_end, int req_sample_rate, void **ctx) {
	mod_ctx_t *mod=calloc(sizeof(mod_ctx_t), 1);
	if (!mod) return -1;
	char error[64];
	struct data data={
		.buffer=(char*)data_start,
		.length=(char*)data_end-(char*)data_start
	};
	mod->module=module_load(&data, error);
	if (!mod->module) {
		printf("Failed loading mod: %s\n", error);
		goto err;
	}
	mod->replay=new_replay(mod->module, req_sample_rate, 0);
	if (!mod->replay) goto err;
	mod->sample_rate=req_sample_rate;
	*ctx=(void*)mod;
	return calculate_mix_buf_len(req_sample_rate)*4; //ToDo: check this!
err:
	if (mod->module) dispose_module(mod->module);
	if (mod->replay) dispose_replay(mod->replay);
	free(mod);
	return -1;
}

int mod_get_sample_rate(void *ctx) {
	mod_ctx_t *mod=(mod_ctx_t*)ctx;
	return mod->sample_rate;
}

int mod_fill_buffer(void *ctx, int8_t *buffer) {
	mod_ctx_t *mod=(mod_ctx_t*)ctx;
	int *samps=(int*)buffer;
	int r=replay_get_audio(mod->replay, samps);
//	printf("Got %d samps from ibxm.\n", r);
	for (int i=0; i<r; i++) {
		int s=samps[i+1];
		if (s>32767) s=32767;
		if (s<-32767) s=-32767;
		buffer[i]=s>>8;
//		if (i<16) buffer[i]=0;
	}
	return r;
//	return r-10;
}

void mod_deinit_source(void *ctx) {
	mod_ctx_t *mod=(mod_ctx_t*)ctx;
	if (mod->module) dispose_module(mod->module);
	if (mod->replay) dispose_replay(mod->replay);
	free(mod);
}

const sndmixer_source_t sndmixer_source_mod={
	.init_source=mod_init_source,
	.get_sample_rate=mod_get_sample_rate,
	.fill_buffer=mod_fill_buffer,
	.deinit_source=mod_deinit_source
};