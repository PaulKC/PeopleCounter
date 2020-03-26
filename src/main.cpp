#include <Arduino.h>
#include <hcsr04.h>
#include "FastLED.h"
#include <ESP_EEPROM.h>
#include "config_server.h"


#define TRIG_PIN_IN 13
#define ECHO_PIN_IN 15

#define TRIG_PIN_OUT 14
#define ECHO_PIN_OUT 12

#define BUZZER_PIN 5

#define TRIGGER_DISTANCE 200

#define LED_PIN 4
#define NB_LEDS 12

CRGB leds[NB_LEDS];

int maxPeople = 8;
int counter = 0;

boolean inTriggered = false;
boolean outTriggered = false;

int inResetCounter = 0;
int outResetCounter = 0;

HCSR04 sensorIn(TRIG_PIN_IN, ECHO_PIN_IN, 10, 4000);
HCSR04 sensorOut(TRIG_PIN_OUT, ECHO_PIN_OUT, 10, 4000);

void updateSavedState()
{
  EEPROM.put(0, maxPeople);
  EEPROM.commit();
}

void maxPersonsChanged(int newMaxPersons)
{
  maxPeople = newMaxPersons;
  updateSavedState();
}

ConfigServer server(maxPeople,maxPersonsChanged);

void setLedColor(boolean full)
{
  for (int i = 0; i < NB_LEDS / 2; i++)
  {
    if (full)
    {
      leds[i] = CRGB::Black;
    }
    else
    {
      leds[i] = CRGB::Green;
    }
  }
  for (int i = NB_LEDS / 2; i < NB_LEDS; i++)
  {
    if (full)
    {
      leds[i] = CRGB::Red;
    }
    else
    {
      leds[i] = CRGB::Black;
    }
  }
  FastLED.show();
}

void setup()
{
  Serial.begin(9600);

  //EEPROM
  EEPROM.begin(16);
  if (EEPROM.percentUsed() >= 0)
  {
    EEPROM.get(0, maxPeople);
  }
  server.setMaxPeople(maxPeople);

  //LED
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NB_LEDS);
  setLedColor(false);

  server.startServer();
}

void loop()
{
  server.handleClient();
  
  int distanceIn = sensorIn.distanceInMillimeters();
  int distanceOut = sensorOut.distanceInMillimeters();

  // Trigger counter
  if (distanceIn > 0 && distanceIn < TRIGGER_DISTANCE && !inTriggered)
  {
    inTriggered = true;
    counter++;
    tone(BUZZER_PIN, 880, 500);
    Serial.println(counter);
    if (counter == maxPeople)
    {
      setLedColor(true);
    }
  }
  if (distanceOut > 0 && distanceOut < TRIGGER_DISTANCE && !outTriggered)
  {
    outTriggered = true;
    if (counter == maxPeople)
    {
      setLedColor(false);
    }
    counter--;
    tone(BUZZER_PIN, 440, 500);
    Serial.println(counter);
  }

  //Reset trigger
  if ((distanceIn < 0 || distanceIn > TRIGGER_DISTANCE) && inTriggered)
  {
    inResetCounter++;
  }
  if (inResetCounter > 8)
  {
    inTriggered = false;
    inResetCounter = 0;
  }
  if ((distanceOut < 0 || distanceOut > TRIGGER_DISTANCE) && outTriggered)
  {
    outResetCounter++;
  }
  if (outResetCounter > 8)
  {
    outTriggered = false;
    outResetCounter = 0;
  }
}