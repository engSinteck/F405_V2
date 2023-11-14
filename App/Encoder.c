/*
 * Encoder.c
 *
 *  Created on: Oct 24, 2023
 *      Author: rinaldo.santos
 */
#include "main.h"
#include "tim.h"

extern volatile uint32_t enc1_cnt, enc2_cnt, enc3_cnt;
extern volatile uint32_t enc1_last, enc2_last, enc3_last;

void Read_Encoder(void)
{
	// Encoder 1
	enc1_cnt = htim3.Instance->CNT >> 1;
	if(enc1_cnt != enc1_last) {
		enc1_last  = enc1_cnt;
	}
	// Encoder 2
	enc2_cnt = htim2.Instance->CNT >> 1;
	if(enc2_cnt != enc2_last) {
		enc2_last = enc2_cnt;
	}
	// Encoder 3
	enc3_cnt = htim4.Instance->CNT >> 1;
	if(enc3_cnt != enc3_last) {
		enc3_last = enc3_cnt;
	}
}
