// Include knihoven
#include <Arduino.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Adafruit_I2CDevice.h>
#include <U8glib.h>
#include "avdweb_AnalogReadFast.h"

// Display nastaveni
#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels
#define OLED_RESET 4
Adafruit_SSD1306 OLED(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#define BUZZER 7

int frekvence = 700;

// flagy
int S_flag;
int D_flag;
int H_flag = 1;

bool vypis;

char zprava;

unsigned long startMillis;  //some global variables available anywhere in the program
unsigned long currentMillis;
const unsigned long period = 1000;  //the value is a number of milliseconds

const byte numChars = 32;
char receivedChars[numChars]; // an array to store the received data
boolean newData = false;
int dataNumber = 0; // new for this version
int usb_message;
int flag;
int stop_flag;

void OLEDtext(String text, int x, int y, int size, boolean d);
void OLEDclear();
void OLEDstartup_menu();
void beep_on(int pwm);
void beep_off();
void peep(int frekvence, int delka_tonu);

void peep(int frekvence, int delka_tonu)
{
	pinMode(BUZZER, OUTPUT);
	tone(BUZZER, frekvence);
	delay(delka_tonu);
	noTone(BUZZER);
	pinMode(BUZZER, INPUT);
}

// 'EKG_startup', 127x64px
const unsigned char BMP_EMG_startup [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0xff, 0x81, 0xe0, 0x00, 0x07, 0x80, 0x0f, 0xfc, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0xff, 0x81, 0xf0, 0x00, 0x0f, 0x80, 0x3f, 0xff, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0xff, 0x81, 0xf0, 0x00, 0x0f, 0x80, 0x7f, 0xff, 0xc0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0x00, 0x01, 0xf8, 0x00, 0x1f, 0x80, 0xfe, 0x1f, 0xe0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0x00, 0x01, 0xf8, 0x00, 0x1f, 0x81, 0xf8, 0x03, 0xe0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0x00, 0x01, 0xfc, 0x00, 0x3f, 0x83, 0xe0, 0x01, 0xf0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0x00, 0x01, 0xfc, 0x00, 0x3f, 0x83, 0xc0, 0x00, 0xf0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0x00, 0x01, 0xfc, 0x00, 0x7f, 0x87, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0x00, 0x01, 0xfe, 0x00, 0x7f, 0x87, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0x00, 0x01, 0xee, 0x00, 0xf7, 0x87, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0x00, 0x01, 0xef, 0x00, 0xf7, 0x87, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0xff, 0x01, 0xe7, 0x00, 0xe7, 0x8f, 0x80, 0xff, 0xf8, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0xff, 0x01, 0xe7, 0x81, 0xe7, 0x8f, 0x00, 0xff, 0xf8, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0xff, 0x01, 0xe7, 0x81, 0xc7, 0x8f, 0x80, 0xff, 0xf8, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0x00, 0x01, 0xe3, 0xc3, 0xc7, 0x87, 0x80, 0x00, 0x78, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0x00, 0x01, 0xe3, 0xc3, 0xc7, 0x87, 0x80, 0x00, 0x78, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0x00, 0x01, 0xe1, 0xe7, 0x87, 0x87, 0x80, 0x00, 0x78, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0x00, 0x01, 0xe1, 0xe7, 0x87, 0x87, 0xc0, 0x00, 0xf8, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0x00, 0x01, 0xe0, 0xef, 0x07, 0x83, 0xc0, 0x00, 0xf0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0x00, 0x01, 0xe0, 0xff, 0x07, 0x83, 0xe0, 0x01, 0xf0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0x00, 0x01, 0xe0, 0x7e, 0x07, 0x81, 0xf8, 0x07, 0xe0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0x00, 0x01, 0xe0, 0x7e, 0x07, 0x80, 0xff, 0x3f, 0xc0, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0xff, 0x81, 0xe0, 0x7c, 0x07, 0x80, 0x7f, 0xff, 0x80, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0xff, 0x81, 0xe0, 0x3c, 0x07, 0x80, 0x1f, 0xfe, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0xff, 0x81, 0xe0, 0x3c, 0x07, 0x80, 0x07, 0xf8, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x24, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x1c, 0x7e, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0x00, 0x60, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0x00, 0x60, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0x34, 0x60, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0x38, 0x60, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x30, 0x7e, 
	0x3c, 0xf3, 0xcf, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x30, 0x60, 
	0x45, 0x16, 0x51, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x30, 0x60, 
	0x05, 0x10, 0x41, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x30, 0x60, 
	0x09, 0x10, 0xc2, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x30, 0x60, 
	0x19, 0x10, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc0, 0x30, 0x60, 
	0x21, 0x93, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x7c, 0xe7, 0xdf, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xe0, 0x00, 0x00, 
	0x60, 0xf2, 0x40, 0x40, 0x78, 0x46, 0x02, 0x00, 0x00, 0x30, 0x43, 0x04, 0x10, 0xf0, 0x82, 0x00, 
	0x60, 0x88, 0x4c, 0x48, 0x44, 0x46, 0x62, 0x34, 0x60, 0x18, 0x21, 0x82, 0x08, 0x38, 0x41, 0x00, 
	0x60, 0x9a, 0x5b, 0x58, 0x44, 0x46, 0xda, 0x4d, 0x90, 0x0c, 0x10, 0x41, 0x04, 0x1c, 0x20, 0x80, 
	0x60, 0xf2, 0x51, 0x70, 0x78, 0x7e, 0x8a, 0xc5, 0x98, 0x02, 0x08, 0x20, 0x82, 0x0e, 0x10, 0x40, 
	0x60, 0x8a, 0x5f, 0x60, 0x40, 0x46, 0x8a, 0x85, 0x98, 0x01, 0x04, 0x10, 0x41, 0x07, 0x08, 0x20, 
	0x60, 0x8a, 0x51, 0x50, 0x40, 0x46, 0x8a, 0x45, 0x98, 0x00, 0x82, 0x08, 0x20, 0x83, 0xc4, 0x10, 
	0x7a, 0xf2, 0x4e, 0x48, 0x41, 0x46, 0x72, 0x7d, 0x98, 0x00, 0xc1, 0x04, 0x10, 0x41, 0xc2, 0x08, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

// 'connect', 128x64px
const unsigned char BMP_connect[] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x78, 0xf8, 0x00, 0x06, 0x80, 0x02, 0x33, 0xcf, 0x00, 0x00,
	0x00, 0x00, 0x0e, 0x00, 0x00, 0x00, 0x7c, 0x40, 0x00, 0x00, 0xc0, 0x02, 0x37, 0xef, 0x80, 0x00,
	0x00, 0x00, 0x1f, 0x00, 0x00, 0x00, 0x47, 0xe9, 0xf1, 0xe7, 0xe7, 0x82, 0x36, 0x08, 0xc0, 0x00,
	0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x6d, 0xc9, 0x9b, 0x36, 0xcc, 0xc2, 0x37, 0x8f, 0x80, 0x00,
	0x00, 0x00, 0x7f, 0x80, 0x00, 0x00, 0x7d, 0x89, 0x0a, 0x16, 0xcf, 0xc2, 0x31, 0xcf, 0x80, 0x00,
	0x00, 0x00, 0x7f, 0xc7, 0xf0, 0x00, 0x61, 0x89, 0x1a, 0x16, 0xcf, 0xc2, 0x30, 0x68, 0xc0, 0x00,
	0x00, 0x00, 0xff, 0xe7, 0xf8, 0x00, 0x41, 0x89, 0x9b, 0x36, 0xcc, 0xc3, 0x36, 0x6c, 0xc0, 0x00,
	0x00, 0x00, 0x1e, 0x07, 0xf8, 0x00, 0x41, 0x89, 0xf1, 0xe6, 0xe7, 0x81, 0xe3, 0xcf, 0x80, 0x00,
	0x00, 0x00, 0x1e, 0x07, 0xf8, 0x00, 0x00, 0x01, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x1e, 0x07, 0xf8, 0x00, 0x00, 0x01, 0x00, 0x06, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x1e, 0x07, 0xf8, 0x00, 0x00, 0x01, 0x00, 0x0c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0xf8, 0x1e, 0x07, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x01, 0xf8, 0x1e, 0x07, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x03, 0xfc, 0x1e, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x80, 0x00, 0x00, 0x00, 0x00,
	0x03, 0xfc, 0x1e, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0xe0, 0x00, 0x00, 0x00, 0x00,
	0x03, 0xfc, 0x1e, 0x03, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xb0, 0x00, 0x00, 0x00, 0x00,
	0x01, 0xf8, 0x1e, 0x7f, 0xc0, 0x00, 0x7e, 0x1e, 0x7e, 0x44, 0xf7, 0xfb, 0xe0, 0x00, 0x00, 0x00,
	0x01, 0xf8, 0x1f, 0xff, 0xc0, 0x00, 0x66, 0x1b, 0x66, 0x45, 0x93, 0x33, 0x30, 0x00, 0x00, 0x00,
	0x00, 0xf0, 0x1f, 0xff, 0x80, 0x00, 0x46, 0x1e, 0x63, 0x44, 0xe2, 0x23, 0xf0, 0x00, 0x00, 0x00,
	0x00, 0xfc, 0x1f, 0xfe, 0x00, 0x00, 0x46, 0x07, 0x63, 0x44, 0x72, 0x23, 0xe0, 0x00, 0x00, 0x00,
	0x00, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x66, 0x1b, 0x66, 0x6d, 0x93, 0x33, 0x20, 0x00, 0x00, 0x00,
	0x00, 0x7f, 0xff, 0x00, 0x00, 0x00, 0x3e, 0x1f, 0x7e, 0x7c, 0xf3, 0xb9, 0xe0, 0x00, 0x00, 0x00,
	0x00, 0x3f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x0f, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x7e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x60, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x1e, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x7f, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x04, 0x00, 0x00, 0x08, 0x00, 0x00,
	0x00, 0x00, 0xff, 0xc0, 0x00, 0x00, 0x78, 0x78, 0x00, 0x00, 0x24, 0x80, 0x00, 0x08, 0x00, 0x00,
	0x00, 0x00, 0xff, 0xe0, 0x00, 0x00, 0x7c, 0xfc, 0x00, 0x00, 0x20, 0x80, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0xff, 0xe0, 0x00, 0x00, 0x47, 0x84, 0x1f, 0xbf, 0x24, 0x9b, 0xf3, 0xc8, 0x00, 0x00,
	0x00, 0x01, 0xff, 0xe0, 0x00, 0x00, 0x6d, 0x80, 0x19, 0xb9, 0xa4, 0xb2, 0x36, 0x68, 0x00, 0x00,
	0x00, 0x00, 0xff, 0xe0, 0x00, 0x00, 0x7d, 0x80, 0x11, 0xb1, 0xa4, 0xe6, 0x34, 0x08, 0x00, 0x00,
	0x00, 0x00, 0xff, 0xe0, 0x00, 0x00, 0x61, 0x80, 0x11, 0xb1, 0xa4, 0xe6, 0x34, 0x08, 0x00, 0x00,
	0x00, 0x00, 0xff, 0xc0, 0x00, 0x00, 0x40, 0xcc, 0x19, 0xb9, 0xa4, 0xb3, 0x36, 0x68, 0x00, 0x00,
	0x00, 0x00, 0x7f, 0x80, 0x00, 0x00, 0x40, 0x7c, 0x0f, 0xbf, 0x24, 0x9b, 0xf3, 0xc8, 0x00, 0x00,
	0x00, 0x00, 0x3f, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x30, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00};

// 'mereni', 128x64px
const unsigned char BMP_EMG_mereni [] PROGMEM = {
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x03, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x0f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x1f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x3f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x7f, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x01, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x03, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x07, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x07, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x0f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x1f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x1f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x3f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02, 0x01, 0x90, 0x03, 0x00, 0x00, 
	0x00, 0x00, 0x3f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0xfc, 0x00, 0x03, 0x03, 0x30, 0x06, 0x00, 0x00, 
	0x00, 0x00, 0x3f, 0xf0, 0x00, 0x00, 0x00, 0x00, 0xfe, 0x00, 0x03, 0x00, 0x30, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x3f, 0xf0, 0xf8, 0x07, 0x80, 0x00, 0xc6, 0xf3, 0xe3, 0xf1, 0xbf, 0x0f, 0xc0, 0x00, 
	0x00, 0x00, 0x7f, 0xf3, 0xfe, 0x1f, 0xc0, 0x00, 0xc6, 0xf7, 0x73, 0xb9, 0xb9, 0x99, 0xc0, 0x00, 
	0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xc0, 0x00, 0xfc, 0xc6, 0x33, 0x19, 0xb1, 0xb8, 0xc0, 0x00, 
	0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xe0, 0x00, 0xf8, 0xc6, 0x33, 0x19, 0xb1, 0xb0, 0xc0, 0x00, 
	0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xe0, 0x00, 0xc0, 0xc6, 0x33, 0x19, 0xb1, 0x98, 0xc0, 0x00, 
	0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0xc0, 0xc7, 0xf3, 0xf9, 0xb1, 0x9f, 0xc0, 0x00, 
	0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0xc0, 0xc3, 0xe3, 0xf1, 0xb1, 0x8f, 0xc0, 0x00, 
	0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x01, 0x22, 0x40, 0x00, 0x0c, 0x00, 0x00, 
	0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0xe1, 0xc0, 0x00, 0x18, 0x00, 0x00, 
	0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0x80, 0x00, 0xfd, 0xe1, 0xe3, 0xcf, 0x9f, 0x8c, 0x00, 0x00, 
	0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0x00, 0x00, 0xe7, 0x33, 0x33, 0xd9, 0x9d, 0xcc, 0x00, 0x00, 
	0x00, 0x00, 0x03, 0xff, 0xff, 0xfc, 0x00, 0x00, 0xc6, 0x37, 0x3b, 0x18, 0xd8, 0xcc, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x3f, 0xfc, 0x00, 0x00, 0x00, 0xc6, 0x37, 0xfb, 0x1f, 0xd8, 0xcc, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0x32, 0x03, 0x18, 0x18, 0xcc, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0x33, 0xf3, 0x1f, 0x98, 0xcc, 0x33, 0x60, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xc6, 0x31, 0xe3, 0x0f, 0x18, 0xcc, 0x33, 0x60, 
	0x00, 0x06, 0x00, 0x03, 0x00, 0x00, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x0e, 0x00, 0x07, 0x00, 0x01, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x1e, 0x60, 0x0f, 0x30, 0x01, 0xc4, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x36, 0xe0, 0x09, 0x30, 0x03, 0x4c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x0f, 0xe2, 0xe7, 0xf9, 0x7b, 0xfe, 0x7e, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x03, 0xbc, 0x01, 0xde, 0x00, 0x73, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x03, 0x18, 0x01, 0xcc, 0x00, 0x73, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x03, 0x10, 0x00, 0x88, 0x00, 0x21, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x08, 0x01, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x18, 0x03, 0x00, 0x18, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x3b, 0x03, 0xa0, 0x3b, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x07, 0xef, 0x7f, 0xff, 0xef, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x0d, 0xf8, 0xdf, 0xcd, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x0d, 0x80, 0x90, 0x09, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
	0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void OLEDtext(String text, int x, int y, int size, boolean d)
{
  //Výpis textu na OLED, Využívá knihovny Adafruit pro mikrořadič SSD 1306
  OLED.setTextSize(size);
  OLED.setTextColor(WHITE);
  OLED.setCursor(x, y);
  OLED.println(text);
  if (d)
  {
    OLED.display();
  }

  delay(100);
}

void OLEDclear()
{
	// Vyčíštění OLED
	OLED.clearDisplay();
	OLEDtext("", 0, 0, 1, true);
}

void OLEDstartup_menu()
{

	OLEDclear();
	OLED.drawBitmap(0, 0, BMP_EMG_startup, 128, 64, WHITE);
	OLED.display();

	//originálně 5000
	delay(1000);
}

void beep_on(int pwm)
{
	pinMode(BUZZER, OUTPUT);
	tone(BUZZER, pwm);
}

void beep_off()
{
	noTone(BUZZER);
	pinMode(BUZZER, INPUT);
}

// Čtení USB komunikace
void USB_read()
{
	static byte ndx = 0;
	char endMarker = '\n';
	char rc;

	if (Serial.available() > 0)
	{
		rc = Serial.read();

		if (rc != endMarker)
		{
			receivedChars[ndx] = rc;
			ndx++;
			if (ndx >= numChars)
			{
				ndx = numChars - 1;
			}
		}
		else
		{
			receivedChars[ndx] = '\0'; // terminate the string
			ndx = 0;
			newData = true;
		}
	}

	if (newData == true)
	{
		dataNumber = 0;					  // new for this version
		dataNumber = atoi(receivedChars); // new for this version
		// Serial.println(receivedChars)
		usb_message = dataNumber;
		newData = false;
	}
}

void setup()
{
	// put your setup code here, to run once:

	Serial.begin(500000);
	OLED.begin(SSD1306_SWITCHCAPVCC, 0x3C);

	OLED.dim(true); // Snížení jasu OLED

	vypis = true;


	OLEDclear();
	OLED.drawBitmap(0, 0, BMP_EMG_startup, 128, 64, WHITE);
	OLED.display();

	peep(500,150);
	peep(800,150);
	peep(1000,150);

	//originálně 5000, změna kvůli vývoji
	delay(1000);
}

void loop()
{
	if (vypis)
	{
		OLEDclear();
		OLED.drawBitmap(0, 0, BMP_connect, 128, 64, WHITE);
		OLED.display();

		peep(1000,150);
		peep(1000,150);

		vypis = false;
	}

	// čtení bufferu
	if (Serial.available() > 0)
	{
		usb_message = 0;
		USB_read();
		zprava = usb_message;
		// delay(1);

		if (zprava == 2) //Setting
		{
			//implement period change protocol
		}
		if(zprava == 3)
		{
			if ((digitalRead(10) == 1) || (digitalRead(11) == 1)) //Data threshold detection
			{
				// send zero then time 
				Serial.println('0');
			}
			else
			{
				// send data then time 
				Serial.println(analogReadFast(A0));    //this mby saves time idk anymore (100us to 20us)
				// //Serial.write('\r'); -- use to get it to work faster	
			}
		}
		if (zprava == 4) //Stop sending
		{
			S_flag = 0;
			vypis = true;
			Serial.println('P');
		}
		if (zprava == 8) //Handshake
		{
			Serial.println('C');
		}
		if (zprava == 5) //Start sending
		{
			S_flag = 1;
			vypis = true;

			// ACK
			Serial.println('S');
			
			
			if (vypis)
			{
				OLEDclear();
				OLED.drawBitmap(0, 0, BMP_EMG_mereni, 128, 64, WHITE);
				OLED.display();

				peep(1000,150);
				peep(1200,150);

				vypis = false;
			}
			delay(1);
		}
	}
	while (S_flag) //NOTE: might change this while to if for cleaner code, for now it stays
	{
		
		if (Serial.available() > 0) //quick check 
		{
			break;
		}
		currentMillis = micros();  //get the current "time" (actually the number of milliseconds since the program started)
		if (!(currentMillis - startMillis >= period))  //test whether the period has elapsed
		{
			continue;
		}
		startMillis = currentMillis;  //reseting loop (havent found a way to make it perfectly time period)
		if ((digitalRead(10) == 1) || (digitalRead(11) == 1)) //Data threshold detection
		{
			// send zero then time 
			Serial.print('0');
			Serial.print(',');
			Serial.println(currentMillis);
		}
		else
		{
			// send data then time 
			Serial.print(analogReadFast(A0));    //this mby saves time idk anymore (100us to 20us)
			Serial.print(',');					 //FIX- making the print on one line
			Serial.println(currentMillis);
			 // //Serial.write('\r'); -- use to get it to work faster								
		}
	}
}