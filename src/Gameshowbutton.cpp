#include <Arduino.h>
#include <ESP8266WiFi.h>
#include "AudioFileSourceLittleFS.h"
#include "AudioGeneratorMP3.h"
#include "AudioOutputI2S.h"
#include <EEPROM.h>

#define EEPROM_SIZE 1 // Save selected mp3 state

AudioGeneratorMP3 *mp3;
AudioFileSourceLittleFS *file;
AudioOutputI2S *out;

int playMP3;                // the selected mp3
int firstRun = 1;

char *filename;
char* getfileName(int playMP3);
void light_sleep();
void wakeup();

int previewMP3 = 0;
unsigned long previewTime = 0;
unsigned long previewLength = 1000;

// Set how many MP3s are in the filesystem
const int numMp3 = 13;        // must equal actual number. If more than 20, the function getfileName must be modified.

// Button debounce
// constants won't change. They're used here to set pin numbers:
const int button1Pin = D1;    // the number of the pushbutton pin
const int button2Pin = D2;    // the number of the pushbutton pin
const int wakeupPin = D5;
const int ledPin = D6;

// Variables will change:
int button1State;             // the current reading from the input pin
int lastButton1State = HIGH;   // the previous reading from the input pin
int button2State;             // the current reading from the input pin
int lastButton2State = HIGH;   // the previous reading from the input pin

// the following variables are unsigned longs because the time, measured in
// milliseconds, will quickly become a bigger number than can be stored in an int.
unsigned long lastDebounce1Time = 0;  // the last time the output pin was toggled
unsigned long lastDebounce2Time = 0;  // the last time the output pin was toggled
unsigned long debounceDelay = 50;    // the debounce time; increase if the output flickers

void setup()
{
  EEPROM.begin(EEPROM_SIZE);
  WiFi.mode(WIFI_OFF);
  WiFi.forceSleepBegin(); 
  Serial.begin(115200);
  digitalWrite(ledPin, HIGH);
  delay(1000);
  LittleFS.begin();

  //audioLogger = &Serial;
  out = new AudioOutputI2S();
  mp3 = new AudioGeneratorMP3();

  pinMode(button1Pin, INPUT_PULLUP);
  pinMode(button2Pin, INPUT_PULLUP);
  pinMode(wakeupPin, INPUT_PULLUP);
  pinMode(ledPin, OUTPUT);

  playMP3 = EEPROM.read(0);
  Serial.printf("\nBoot\nSelecting %d.mp3 from EEPROM\n",playMP3);

  wifi_status_led_uninstall();
  
}

void loop()
{
  // read the state of the switch into a local variable:
  int reading1 = digitalRead(button1Pin);
  int reading2 = digitalRead(button2Pin);
  // check to see if you just pressed the button
  // (i.e. the input went from LOW to HIGH), and you've waited long enough
  // since the last press to ignore any noise:

  // If the switch changed, due to noise or pressing:
  if (reading1 != lastButton1State) {
    // reset the debouncing timer
    lastDebounce1Time = millis();
  }
  if (reading2 != lastButton2State) {
    // reset the debouncing timer
    lastDebounce2Time = millis();
  }

  if ((millis() - lastDebounce1Time) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading1 != button1State) {
      button1State = reading1;

      // only to action if the new button state is HIGH
      if (button1State == LOW) {
        Serial.printf("Action button pressed\n");
          filename = getfileName(playMP3);
          Serial.printf("%s\n",filename);
          file = new AudioFileSourceLittleFS(filename);
          if (mp3->isRunning()) {
            mp3->stop();
          }
          mp3->begin(file, out);
    //    }
      }
    }
  }

  if ((millis() - lastDebounce2Time) > debounceDelay) {
    // whatever the reading is at, it's been there for longer than the debounce
    // delay, so take it as the actual current state:

    // if the button state has changed:
    if (reading2 != button2State) {
      button2State = reading2;

      // only to action if the new button state is HIGH
      if (button2State == LOW) {
        Serial.printf("Config button pressed\n");
        playMP3++;
        if (playMP3 > numMp3){
          playMP3 = 1;
        }
        EEPROM.write(0, playMP3);
        EEPROM.commit();
        previewMP3 = 1;
        previewTime = millis();
        filename = getfileName(playMP3);
        Serial.printf("%s\n",filename);
        file = new AudioFileSourceLittleFS(filename);
        if (mp3->isRunning()) {
          mp3->stop();
        }
        mp3->begin(file, out);
      }
    }
  }

  // save the reading. Next time through the loop, it'll be the lastButtonState:
  lastButton1State = reading1;
  lastButton2State = reading2;

 // preview MP3 on config button press
  if (previewMP3) {
    if ((millis() - previewTime) > previewLength) {
      if (mp3->isRunning()) {
        mp3->stop();
        light_sleep(); // Go to sleep
      }
      previewMP3 = 0;
    }
  }

  // MP3. Stop when finished.
  if (mp3->isRunning()) {
    if (!mp3->loop()) {
      mp3->stop();
      light_sleep(); // Go to sleep
    }
  }

  if (firstRun) {
    firstRun = 0;
    light_sleep();
  }
}

char* getfileName(int playMP3)
{
  switch (playMP3)
    {
    case 1:
      filename = "1.mp3";
      break;
    case 2:
      filename = "2.mp3";
      break;
    case 3:
      filename = "3.mp3";
      break;
    case 4:
      filename = "4.mp3";
      break;
    case 5:
      filename = "5.mp3";
      break;
    case 6:
      filename = "6.mp3";
      break;
    case 7:
      filename = "7.mp3";
      break;
    case 8:
      filename = "8.mp3";
      break;
    case 9:
      filename = "9.mp3";
      break;
    case 10:
      filename = "10.mp3";
      break;
    case 11:
      filename = "11.mp3";
      break;
    case 12:
      filename = "12.mp3";
      break;
    case 13:
      filename = "13.mp3";
      break;
    case 14:
      filename = "14.mp3";
      break;
    case 15:
      filename = "15.mp3";
      break;
    case 16:
      filename = "16.mp3";
      break;
    case 17:
      filename = "17.mp3";
      break;
    case 18:
      filename = "18.mp3";
      break;
    case 19:
      filename = "19.mp3";
      break;
    case 20:
      filename = "20.mp3";
      break;
    default:
      break;
    }
  return filename;
}

void light_sleep() {
  Serial.println("Going to light sleep...");
  wifi_station_disconnect();
  wifi_set_opmode(NULL_MODE);
  wifi_fpm_set_sleep_type(LIGHT_SLEEP_T); //light sleep mode
  digitalWrite(ledPin, LOW);
  gpio_pin_wakeup_enable(GPIO_ID_PIN(wakeupPin), GPIO_PIN_INTR_LOLEVEL); //set the interrupt to look for LOW pulses.
  wifi_fpm_open();
  delay(100);
  wifi_fpm_set_wakeup_cb(wakeup); //wakeup callback
  
  wifi_fpm_do_sleep(0xFFFFFFF); 
  delay(100);
}

void wakeup() {
  Serial.println("Woke from light sleep...");
  digitalWrite(ledPin, HIGH);
  wifi_fpm_close();
  loop();
  //wifi_set_opmode(STATION_MODE);
  //wifi_station_connect();
}