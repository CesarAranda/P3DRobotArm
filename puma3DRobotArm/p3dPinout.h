#ifndef PINOUT_H_
#define PINOUT_H_
/*
 * Pinout of RAMPS 1.4
 * source: http://reprap.org/wiki/RAMPS_1.4
 */

#define HIG_STEP_PIN         54
#define HIG_DIR_PIN          55
#define HIG_ENABLE_PIN       38
#define HIG_MIN_PIN           3
#define HIG_MAX_PIN           2
 
#define LOW_STEP_PIN         60
#define LOW_DIR_PIN          61
#define LOW_ENABLE_PIN       56
#define LOW_MIN_PIN          14
#define LOW_MAX_PIN          15

#define ROT_STEP_PIN         46
#define ROT_DIR_PIN          48
#define ROT_ENABLE_PIN       62
#define ROT_MIN_PIN          18
#define ROT_MAX_PIN          19

#define RAI_STEP_PIN        26
#define RAI_DIR_PIN         28
#define RAI_ENABLE_PIN      24
#define RAI_MIN_PIN         20

#define GRI_STEP_PIN        36
#define GRI_DIR_PIN         34
#define GRI_ENABLE_PIN      30
#define GRI_MIN_PIN         32

#define BYJ_PIN_0          40
#define BYJ_PIN_1          63
#define BYJ_PIN_2          59
#define BYJ_PIN_3          64

#define SERVO_PIN           4

#define LED_PIN            13  // MODIF
#define FAN_PIN             9
#define PUMP_PIN            8
#define LASER_PIN          10

#define SDPOWER            -1
#define SDSS               53

#define PS_ON_PIN          12
#define KILL_PIN           -1

#define TEMP_0_PIN         13   // ANALOG NUMBERING
#define TEMP_1_PIN         14   // ANALOG NUMBERING

//RAMPS AUX-2

#endif
