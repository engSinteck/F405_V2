/*
 * screen.c
 *
 *  Created on: Aug 14, 2023
 *      Author: rinaldo.santos
 */

#include "main.h"
#include "screen.h"
#include "../lvgl/lvgl.h"

LV_FONT_DECLARE(lv_font_7Seg_B72);
LV_FONT_DECLARE(lv_font_7Seg_B16);
LV_IMG_DECLARE(iron);
LV_IMG_DECLARE(airflow);

extern volatile uint32_t enc1_cnt, enc1_dir, enc1_btn, enc1_last;
extern volatile uint32_t enc2_cnt, enc2_dir, enc2_btn, enc2_last;
extern volatile uint32_t enc3_cnt, enc3_dir, enc3_btn, enc3_last;
extern volatile uint16_t enc1_val, enc2_val, enc3_val;
extern volatile uint8_t sw_air, sw_iron;
extern uint32_t target_speed;
extern float temp_iron, temp_gun;
extern float target_iron, target_air;
extern uint16_t pwm_iron;
extern float vdda, vref, temp_stm;
extern RTC_TimeTypeDef RTC_Time;
extern uint32_t dimmer_Counter[];
extern uint32_t dimmer_value[];

extern float temperature_K, temperature_air_K;

extern encoder rot1;
extern encoder rot2;
extern encoder rot3;

static lv_obj_t * Tela_Yaxun;
static lv_obj_t * img_iron;
static lv_obj_t * img_air;
static lv_obj_t * iron_temperature;
static lv_obj_t * air_temperature;
static lv_obj_t * preset_iron;
static lv_obj_t * preset_air;
static lv_obj_t * preset_speed;
static lv_obj_t * txt_pct;
static lv_obj_t * txt_pwm;
static lv_obj_t * bar_iron;
static lv_obj_t * bar_air;
static lv_obj_t * frame_iron;
static lv_obj_t * frame_air;
static lv_timer_t * task_yaxun;

// Debug
static lv_obj_t * Tela_Debug;
static lv_obj_t * adc_iron;
static lv_obj_t * adc_air;
static lv_obj_t * enc_1;
static lv_obj_t * enc_2;
static lv_obj_t * enc_3;
static lv_obj_t * label_sw_iron;
static lv_obj_t * label_sw_air;
static lv_obj_t * label_temp_iron;
static lv_obj_t * label_temp_air;
static lv_obj_t * label_power;
static lv_obj_t * label_clock;
static lv_timer_t * task_debug;

void create_iron(void);
void create_air(void);
void update_yaxun_screen(lv_timer_t * timer);
void update_debug_screen(lv_timer_t * timer);

void screen_main(void)
{
	Tela_Yaxun = lv_obj_create(NULL);
	lv_obj_clear_flag(Tela_Yaxun, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_bg_color(Tela_Yaxun, lv_color_hex(0x0000FF), 0);
	lv_obj_set_style_bg_grad_color(Tela_Yaxun, lv_color_hex(0x0000FF), 0);

	create_iron();
	create_air();

    static uint32_t user_data = 10;
    task_yaxun = lv_timer_create(update_yaxun_screen, 200,  &user_data);
}

void create_iron(void)
{
	// Desenha Frame Iron
	frame_iron = lv_obj_create(Tela_Yaxun);
    lv_obj_set_size(frame_iron, 239, 88);
    lv_obj_set_style_radius(frame_iron, 2, 0);
    lv_obj_set_style_bg_color(frame_iron, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_grad_color(frame_iron, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_color(frame_iron, lv_color_hex(0xAAA9AD), 0);
    lv_obj_set_style_bg_opa(frame_iron, LV_OPA_COVER, 0);
    lv_obj_clear_flag(frame_iron, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_pos(frame_iron, 0, 28);
    // Image IRON
	img_iron = lv_img_create(frame_iron);
	lv_img_set_src(img_iron, &iron);
	lv_obj_align_to(img_iron, frame_iron, LV_ALIGN_TOP_LEFT, 0, 8);	// Align
	// Label Temperature IRON
    iron_temperature = lv_label_create(frame_iron);
    lv_obj_set_style_text_font(iron_temperature, &lv_font_7Seg_B72, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(iron_temperature, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(iron_temperature, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(iron_temperature, 1, 0);
    lv_obj_set_style_text_line_space(iron_temperature, 1, 0);
    lv_label_set_long_mode(iron_temperature, LV_LABEL_LONG_WRAP);          	// Break the long lines
    lv_label_set_recolor(iron_temperature, true);                         	// Enable re-coloring by commands in the text
	lv_label_set_text_fmt(iron_temperature, "%0.0f", temp_iron);
    lv_obj_align_to(iron_temperature, frame_iron, LV_ALIGN_CENTER, -32, 0);	// Align
    // Label Preset IRON
    preset_iron = lv_label_create(frame_iron);
    lv_obj_set_style_text_font(preset_iron, &lv_font_7Seg_B16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(preset_iron, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(preset_iron, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(preset_iron, 1, 0);
    lv_obj_set_style_text_line_space(preset_iron, 1, 0);
    lv_label_set_long_mode(preset_iron, LV_LABEL_LONG_WRAP);          	// Break the long lines
    lv_label_set_recolor(preset_iron, true);                         	// Enable re-coloring by commands in the text
	lv_label_set_text_fmt(preset_iron, "%0.0f", target_iron);
    lv_obj_align_to(preset_iron, frame_iron, LV_ALIGN_TOP_LEFT, -10, -10);	// Align
    // Label PWM
    txt_pwm = lv_label_create(frame_iron);
    lv_obj_set_style_text_font(txt_pwm, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(txt_pwm, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(txt_pwm, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(txt_pwm, 1, 0);
    lv_obj_set_style_text_line_space(txt_pwm, 1, 0);
    lv_label_set_long_mode(txt_pwm, LV_LABEL_LONG_WRAP);          	// Break the long lines
    lv_label_set_recolor(txt_pwm, true);                         	// Enable re-coloring by commands in the text
	lv_label_set_text_fmt(txt_pwm, "%d", pwm_iron);
    lv_obj_align_to(txt_pwm, frame_iron, LV_ALIGN_TOP_LEFT, -10, 40);	// Align
    // Barra IRON
    bar_iron = lv_bar_create(frame_iron);
    lv_obj_set_style_radius(bar_iron, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(bar_iron, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(bar_iron, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_radius(bar_iron, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(bar_iron, lv_color_hex(0x00FF00), LV_PART_INDICATOR );
    lv_obj_set_style_bg_grad_color(bar_iron, lv_color_hex(0x00FF00), LV_PART_INDICATOR);
    lv_obj_set_size(bar_iron, 8, 70);
    lv_bar_set_range(bar_iron, 0, 4095);
    lv_bar_set_value(bar_iron, 0, LV_ANIM_OFF);
    lv_obj_align_to(bar_iron, frame_iron, LV_ALIGN_TOP_RIGHT, 10, -10);	// Align
}

void create_air(void)
{
	// Desenha Frame Air
	frame_air = lv_obj_create(Tela_Yaxun);
    lv_obj_set_size(frame_air, 239, 88);
    lv_obj_set_style_radius(frame_air, 2, 0);
    lv_obj_set_style_bg_color(frame_air, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_grad_color(frame_air, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_color(frame_air, lv_color_hex(0xAAA9AD), 0);
    lv_obj_set_style_bg_opa(frame_air, LV_OPA_COVER, 0);
    lv_obj_clear_flag(frame_air, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_pos(frame_air, 0, 231);
    // Image AIR
	img_air = lv_img_create(frame_air);
	lv_img_set_src(img_air, &airflow);
	lv_obj_align_to(img_air, frame_air, LV_ALIGN_TOP_LEFT, 0, 6);	// Align
    // Label Temperature Air
    air_temperature = lv_label_create(frame_air);
    lv_obj_set_style_text_font(air_temperature, &lv_font_7Seg_B72, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(air_temperature, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(air_temperature, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(air_temperature, 1, 0);
    lv_obj_set_style_text_line_space(air_temperature, 1, 0);
    lv_label_set_long_mode(air_temperature, LV_LABEL_LONG_WRAP);          	// Break the long lines
    lv_label_set_recolor(air_temperature, true);                         	// Enable re-coloring by commands in the text
	lv_label_set_text_fmt(air_temperature, "%0.0f", temp_gun);
    lv_obj_align_to(air_temperature, frame_air, LV_ALIGN_CENTER, -32, 0);		// Align
    // Label Preset Air
    preset_air = lv_label_create(frame_air);
    lv_obj_set_style_text_font(preset_air, &lv_font_7Seg_B16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(preset_air, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(preset_air, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(preset_air, 1, 0);
    lv_obj_set_style_text_line_space(preset_air, 1, 0);
    lv_label_set_long_mode(preset_air, LV_LABEL_LONG_WRAP);          	// Break the long lines
    lv_label_set_recolor(preset_air, true);                         	// Enable re-coloring by commands in the text
	lv_label_set_text_fmt(preset_air, "%0.0f", target_air);
    lv_obj_align_to(preset_air, frame_air, LV_ALIGN_TOP_LEFT, -10, -10);	// Align
    // Label Speed Air
    preset_speed = lv_label_create(frame_air);
    lv_obj_set_style_text_font(preset_speed, &lv_font_7Seg_B16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(preset_speed, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(preset_speed, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(preset_speed, 1, 0);
    lv_obj_set_style_text_line_space(preset_speed, 1, 0);
    lv_label_set_long_mode(preset_speed, LV_LABEL_LONG_WRAP);          	// Break the long lines
    lv_label_set_recolor(preset_speed, true);                         	// Enable re-coloring by commands in the text
	lv_label_set_text_fmt(preset_speed, "%ld", target_speed);
    lv_obj_align_to(preset_speed, frame_air, LV_ALIGN_TOP_LEFT, -10, 40);	// Align
    //
    txt_pct = lv_label_create(preset_speed);
    lv_obj_set_style_text_font(txt_pct, &lv_font_montserrat_18, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(txt_pct, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(txt_pct, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(txt_pct, 1, 0);
    lv_obj_set_style_text_line_space(txt_pct, 1, 0);
    lv_label_set_long_mode(txt_pct, LV_LABEL_LONG_WRAP);          	// Break the long lines
    lv_label_set_recolor(txt_pct, true);                         	// Enable re-coloring by commands in the text
	lv_label_set_text(txt_pct, "%");
    lv_obj_align_to(txt_pct, preset_speed, LV_ALIGN_TOP_LEFT, 44, 0);	// Align

    // Barra Air
    bar_air = lv_bar_create(frame_air);
    lv_obj_set_style_radius(bar_air, 0, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(bar_air, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_grad_color(bar_air, lv_color_hex(0x808080), LV_PART_MAIN | LV_STATE_DEFAULT);

    lv_obj_set_style_radius(bar_air, 0, LV_PART_INDICATOR | LV_STATE_DEFAULT);
    lv_obj_set_style_bg_color(bar_air, lv_color_hex(0x00FF00), LV_PART_INDICATOR );
    lv_obj_set_style_bg_grad_color(bar_air, lv_color_hex(0x00FF00), LV_PART_INDICATOR);
    lv_obj_set_size(bar_air, 8, 70);
    lv_bar_set_range(bar_air, 0, 100);
    lv_bar_set_value(bar_air, 0, LV_ANIM_OFF);
    lv_obj_align_to(bar_air, frame_air, LV_ALIGN_TOP_RIGHT, 10, -10);	// Align
}

void update_yaxun_screen(lv_timer_t * timer)
{
	// IRON
	lv_label_set_text_fmt(iron_temperature, "%0.0f", temp_iron);
	lv_label_set_text_fmt(preset_iron, "%0.0f", target_iron);
	lv_bar_set_value(bar_iron, (int32_t)pwm_iron, LV_ANIM_OFF);
	lv_label_set_text_fmt(txt_pwm, "%d", pwm_iron);

	// AIR
	lv_label_set_text_fmt(air_temperature, "%0.0f", temp_gun);
	lv_label_set_text_fmt(preset_air, "%0.0f", target_air);
	lv_label_set_text_fmt(preset_speed, "%ld", target_speed);
	lv_bar_set_value(bar_air, (int32_t)target_speed, LV_ANIM_OFF);

	if(target_speed == 100)
		lv_obj_align_to(txt_pct, preset_speed, LV_ALIGN_TOP_LEFT, 44, 0);	// Align
	else if(target_speed >= 10)
		lv_obj_align_to(txt_pct, preset_speed, LV_ALIGN_TOP_LEFT, 30, 0);	// Align
	else
		lv_obj_align_to(txt_pct, preset_speed, LV_ALIGN_TOP_LEFT, 20, 0);	// Align
}


void screen_debug(void)
{
	Tela_Debug = lv_obj_create(NULL);
	lv_obj_clear_flag(Tela_Debug, LV_OBJ_FLAG_CLICKABLE | LV_OBJ_FLAG_SCROLLABLE);
	lv_obj_set_style_bg_color(Tela_Debug, lv_color_hex(0x000000), 0);
	lv_obj_set_style_bg_grad_color(Tela_Debug, lv_color_hex(0x000000), 0);
	// Desenha Frame Debug
	lv_obj_t * frame_debug = lv_obj_create(Tela_Debug);
    lv_obj_set_size(frame_debug, 239, 319);
    lv_obj_set_style_radius(frame_debug, 2, 0);
    lv_obj_set_style_bg_color(frame_debug, lv_color_hex(0x000000), 0);
    lv_obj_set_style_bg_grad_color(frame_debug, lv_color_hex(0x000000), 0);
    lv_obj_set_style_border_color(frame_debug, lv_color_hex(0xAAA9AD), 0);
    lv_obj_set_style_bg_opa(frame_debug, LV_OPA_COVER, 0);
    lv_obj_clear_flag(frame_debug, LV_OBJ_FLAG_SCROLLABLE);
    lv_obj_set_pos(frame_debug, 0, 0);
	//
    adc_iron = lv_label_create(Tela_Debug);
    lv_obj_set_style_text_font(adc_iron, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(adc_iron, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(adc_iron, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(adc_iron, 1, 0);
    lv_obj_set_style_text_line_space(adc_iron, 1, 0);
    lv_label_set_long_mode(adc_iron, LV_LABEL_LONG_WRAP);          	// Break the long lines
    lv_label_set_recolor(adc_iron, true);                         	// Enable re-coloring by commands in the text
	lv_label_set_text_fmt(adc_iron, "D0: %ld - %ld", dimmer_value[0], dimmer_Counter[0] );
	lv_obj_set_pos(adc_iron, 10, 8);
	//
    adc_air = lv_label_create(Tela_Debug);
    lv_obj_set_style_text_font(adc_air, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(adc_air, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(adc_air, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(adc_air, 1, 0);
    lv_obj_set_style_text_line_space(adc_air, 1, 0);
    lv_label_set_long_mode(adc_air, LV_LABEL_LONG_WRAP);          	// Break the long lines
    lv_label_set_recolor(adc_air, true);                         	// Enable re-coloring by commands in the text
	lv_label_set_text_fmt(adc_air, "D1: %ld - %ld", dimmer_value[1], dimmer_Counter[1] );
	lv_obj_set_pos(adc_air, 10, 30);
	//
    enc_1 = lv_label_create(Tela_Debug);
    lv_obj_set_style_text_font(enc_1, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(enc_1, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(enc_1, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(enc_1, 1, 0);
    lv_obj_set_style_text_line_space(enc_1, 1, 0);
    lv_label_set_long_mode(enc_1, LV_LABEL_LONG_WRAP);          // Break the long lines
    lv_label_set_recolor(enc_1, true);                         	// Enable re-coloring by commands in the text
	lv_label_set_text_fmt(enc_1, "E1 - %ld  D: %ld B: %ld V: %ld", enc1_cnt, enc1_dir, enc1_btn, enc1_last);
	lv_obj_set_pos(enc_1, 10, 52);
	//
    enc_2 = lv_label_create(Tela_Debug);
    lv_obj_set_style_text_font(enc_2, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(enc_2, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(enc_2, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(enc_2, 1, 0);
    lv_obj_set_style_text_line_space(enc_2, 1, 0);
    lv_label_set_long_mode(enc_2, LV_LABEL_LONG_WRAP);          // Break the long lines
    lv_label_set_recolor(enc_2, true);                         	// Enable re-coloring by commands in the text
	lv_label_set_text_fmt(enc_2, "E2 - %ld  D: %ld B: %ld V: %ld", enc2_cnt, enc2_dir, enc2_btn, enc2_last);
	lv_obj_set_pos(enc_2, 10, 74);
	//
    enc_3 = lv_label_create(Tela_Debug);
    lv_obj_set_style_text_font(enc_3, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(enc_3, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(enc_3, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(enc_3, 1, 0);
    lv_obj_set_style_text_line_space(enc_3, 1, 0);
    lv_label_set_long_mode(enc_3, LV_LABEL_LONG_WRAP);          // Break the long lines
    lv_label_set_recolor(enc_3, true);                         	// Enable re-coloring by commands in the text
	lv_label_set_text_fmt(enc_3, "E3 - %ld  D: %ld B: %ld V: %ld", enc3_cnt, enc3_dir, enc3_btn, enc3_last);
	lv_obj_set_pos(enc_3, 10, 96);

	label_temp_iron = lv_label_create(Tela_Debug);
    lv_obj_set_style_text_font(label_temp_iron, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label_temp_iron, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(label_temp_iron, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(label_temp_iron, 1, 0);
    lv_obj_set_style_text_line_space(label_temp_iron, 1, 0);
    lv_label_set_long_mode(label_temp_iron, LV_LABEL_LONG_WRAP);          	// Break the long lines
    lv_label_set_recolor(label_temp_iron, true);                         	// Enable re-coloring by commands in the text
	lv_label_set_text_fmt(label_temp_iron, "IRON: %0.2f °C", temp_iron);
	lv_obj_set_pos(label_temp_iron, 10, 118);

	label_temp_air = lv_label_create(Tela_Debug);
	lv_obj_set_style_text_font(label_temp_air, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(label_temp_air, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(label_temp_air, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(label_temp_air, 1, 0);
	lv_obj_set_style_text_line_space(label_temp_air, 1, 0);
	lv_label_set_long_mode(label_temp_air, LV_LABEL_LONG_WRAP);          	// Break the long lines
	lv_label_set_recolor(label_temp_air, true);                         	// Enable re-coloring by commands in the text
    lv_label_set_text_fmt(label_temp_air, "AIR: %0.2f °C", temp_gun);
	lv_obj_set_pos(label_temp_air, 10, 140);

	label_sw_iron = lv_label_create(Tela_Debug);
    lv_obj_set_style_text_font(label_sw_iron, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label_sw_iron, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(label_sw_iron, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(label_sw_iron, 1, 0);
    lv_obj_set_style_text_line_space(label_sw_iron, 1, 0);
    lv_label_set_long_mode(label_sw_iron, LV_LABEL_LONG_WRAP);          	// Break the long lines
    lv_label_set_recolor(label_sw_iron, true);                         		// Enable re-coloring by commands in the text
	lv_label_set_text_fmt(label_sw_iron, "SW_IRON: %d PWM: %d", sw_iron, pwm_iron);
	lv_obj_set_pos(label_sw_iron, 10, 162);

	label_sw_air = lv_label_create(Tela_Debug);
    lv_obj_set_style_text_font(label_sw_air, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_color(label_sw_air, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_opa(label_sw_air, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
    lv_obj_set_style_text_letter_space(label_sw_air, 1, 0);
    lv_obj_set_style_text_line_space(label_sw_air, 1, 0);
    lv_label_set_long_mode(label_sw_air, LV_LABEL_LONG_WRAP);          	// Break the long lines
    lv_label_set_recolor(label_sw_air, true);                         	// Enable re-coloring by commands in the text
	lv_label_set_text_fmt(label_sw_air, "SW_AIR: %d", sw_air);
	lv_obj_set_pos(label_sw_air, 10, 184);

	label_power = lv_label_create(Tela_Debug);
	lv_obj_set_style_text_font(label_power, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(label_power, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(label_power, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(label_power, 1, 0);
	lv_obj_set_style_text_line_space(label_power, 1, 0);
	lv_label_set_long_mode(label_power, LV_LABEL_LONG_WRAP);          	// Break the long lines
	lv_label_set_recolor(label_power, true);                         	// Enable re-coloring by commands in the text
	lv_label_set_text_fmt(label_power, "uC V:%0.2f  VR:%0.2f T:%0.1f", vdda/1000.0f, vref/1000.0f, temp_stm);
	lv_obj_set_pos(label_power, 10, 206);

	label_clock = lv_label_create(Tela_Debug);
	lv_obj_set_style_text_font(label_clock, &lv_font_montserrat_16, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_color(label_clock, lv_color_hex(0xFFFFFF), LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_opa(label_clock, LV_OPA_COVER, LV_PART_MAIN | LV_STATE_DEFAULT);
	lv_obj_set_style_text_letter_space(label_clock, 1, 0);
	lv_obj_set_style_text_line_space(label_clock, 1, 0);
	lv_label_set_long_mode(label_clock, LV_LABEL_LONG_WRAP);          	// Break the long lines
	lv_label_set_recolor(label_clock, true);                         	// Enable re-coloring by commands in the text
	lv_label_set_text_fmt(label_clock, "Time: %02d:%02d:%02d",RTC_Time.Hours,RTC_Time.Minutes,RTC_Time.Seconds);
	lv_obj_set_pos(label_clock, 10, 228);

    static uint32_t user_data = 10;
    task_debug = lv_timer_create(update_debug_screen, 250,  &user_data);
}

void update_debug_screen(lv_timer_t * timer)
{
	lv_label_set_text_fmt(adc_iron, "D0: %ld - %ld", dimmer_value[0], dimmer_Counter[0] );
	lv_label_set_text_fmt(adc_air , "D1: %ld - %ld", dimmer_value[1], dimmer_Counter[1] );

	lv_label_set_text_fmt(label_temp_iron, "IRON: %0.2f °C", temp_iron);
	lv_label_set_text_fmt(label_temp_air , "AIR : %0.2f °C", temp_gun);

	lv_label_set_text_fmt(enc_1, "E1 - %ld  D: %ld B: %ld V: %ld", enc1_cnt, enc1_dir, enc1_btn, enc1_last);
	lv_label_set_text_fmt(enc_2, "E2 - %ld  D: %ld B: %ld V: %ld", enc2_cnt, enc2_dir, enc2_btn, enc2_last);
	lv_label_set_text_fmt(enc_3, "E3 - %ld  D: %ld B: %ld V: %ld", enc3_cnt, enc3_dir, enc3_btn, enc3_last);

	lv_label_set_text_fmt(label_sw_iron, "SW_IRON: %d PWM: %d", sw_iron, pwm_iron);
	lv_label_set_text_fmt(label_sw_air , "SW_AIR : %d", sw_air);

	lv_label_set_text_fmt(label_power, "uC V:%0.2f  VR:%0.2f T:%0.1f", vdda/1000.0f, vref/1000.0f, temp_stm);
	lv_label_set_text_fmt(label_clock, "Time: %02d:%02d:%02d",RTC_Time.Hours,RTC_Time.Minutes,RTC_Time.Seconds);
}

void load_screen(uint8_t value)
{
	if(value == 0) {
		lv_scr_load(Tela_Debug);
	}
	else {
		lv_scr_load(Tela_Yaxun);
	}
}

void delete_screen_main(void)
{
	lv_timer_del(task_yaxun);
	lv_obj_del(Tela_Yaxun);
	screen_debug();
}

void delete_screen_debug(void)
{
	lv_timer_del(task_debug);
	lv_obj_del(Tela_Debug);
	screen_main();
}
