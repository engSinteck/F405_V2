/*
 * key.h
 *
 *  Created on: Aug 15, 2023
 *      Author: rinaldo.santos
 */

#ifndef KEY_H_
#define KEY_H_

#include <stdint.h>
#include <stdbool.h>

// Prototipos
void KeyboardInit(uint8_t mask);
void ClearLog(uint8_t key, uint8_t index);
void Enter_ClearLog(uint8_t index);
void Enter_SetMode(uint8_t mode, bool flag);
void KeyEnter_Routine(uint8_t key);
uint8_t PushButton_Read(uint8_t key);
void KeyboardSetMode(uint8_t key, uint8_t mode, bool flag);
void KeyboardRead(uint8_t key);
void Key_Read(void);
void ButtonEvent(void);
void KeyboardEvent(void);
void state_encoder_button(uint8_t key, uint8_t state);

// Eventos
/// Register a new event
bool Evt_EnQueue(uint8_t *event);
/// Checkout the oldest event
bool Evt_DeQueue(uint8_t *event);
/// Initialize the event queue
void Evt_InitQueue(void);

#define DEBOUNCE_KEY			2
#define KEY_DN					0
#define	KEY_UP					1
#define KEY_ENTER				2
#define KEY_ESC					3

/// PushButton_Routine timer period in msec
#define PUSHBTN_TMR_PERIOD		50
/// Criteria for determination of short click and long click
#define PUSHBTN_TO_SHORT		3		// 3 * PUSHBTN_TMR_PERIOD  150
#define PUSHBTN_TO_LONG			20		// 10 * PUSHBTN_TMR_PERIOD 500
#define PUSHBTN_TO_MAX			255		// maximum duration count

#define PUSHBTN_MODE_CLICK		0x00	// click mode: detect change
#define PUSHBTN_MODE_UDOWN		0x01	// up down mode: detect level

// Eventos
#define EVT_PBTN_INPUT			0x10		///< event code for pushbutton input
#define PBTN_SCLK				0x01		///< single click
#define PBTN_LCLK				0x02		///< long click
#define PBTN_DCLK				0x03		///< double click
#define PBTN_TCLK				0x04		///< triple click
#define PBTN_DOWN				0x05		///< button state down
#define PBTN_ENDN				0x06		///< button state changed to up

/** Maximum number of events the queue can hold.  This number should be less
 * than 256.
 */
#define EVT_QDEPTH				(8)
/** The maximum size of the event data. It consists of one byte of event code
 * with variable length of data bytes.
 */
#define EVT_QWIDTH				(16)

//typedef struct
//{
//	uint8_t old_state;		///< button state old
//	uint8_t new_state;		///< button state new
//	uint8_t click[8];		///< number of button click
//	uint8_t duration[8];	///< time passed since last click
//	uint8_t mask;			///< mask for buttons to be ignored
//	uint8_t mode;			///< click mode / updown mode
//	int flag;				///< internal flag to prevent false detection
//} pushbtn_param;

typedef struct
{
	uint8_t old_state;		///< button state old
	uint8_t new_state;		///< button state new
	uint8_t click[8];		///< number of button click
	uint8_t duration[8];	///< time passed since last click
	uint8_t mask;			///< mask for buttons to be ignored
	uint8_t mode;			///< click mode / updown mode
	int flag;				///< internal flag to prevent false detection
} pushbtn[4];


#endif /* KEY_H_ */
