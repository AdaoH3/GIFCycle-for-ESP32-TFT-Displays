#include <Arduino.h>
#include <SPI.h>
#include <FS.h>

#include <TFT_eSPI.h>
#include <AnimatedGIF.h>

#define button 1
//Button Pin
#define TFT_BACKLIGHT_PIN 45
//Backlight Pin (Optional) Disable if your tft monitor does not have a backlight

// TFT_eSPI_memory
//
// Example sketch which shows how to display an
// animated GIF image stored in FLASH memory
//
// written by Larry Bank
// bitbank@pobox.com
//
// Adapted by Bodmer for the TFT_eSPI Arduino library:
// https://github.com/Bodmer/TFT_eSPI
//
// To display a GIF from memory, a single callback function
// must be provided - GIFDRAW
// This function is called after each scan line is decoded
// and is passed the 8-bit pixels, RGB565 palette and info
// about how and where to display the line. The palette entries
// can be in little-endian or big-endian order; this is specified
// in the begin() method.
//
// The AnimatedGIF class doesn't allocate or free any memory, but the
// instance data occupies about 22.5K of RAM.

//#define USE_DMA       // ESP32 ~1.25x single frame rendering performance boost for badgers.h
                        // Note: Do not use SPI DMA if reading GIF images from SPI SD card on same bus as TFT
#define NORMAL_SPEED  // Comment out for rame rate for render speed test

// Load GIF library
#include <AnimatedGIF.h>
AnimatedGIF gif;

// Example AnimatedGIF library images
#include "out.h"
#include "jetre.h"
#include "yoru.h"

#define GIF_IMAGE reynaa   
#define GIF_IMAGE1 jettre
#define GIF_IMAGE2 yoruuu
// Refers to the actual gif image

TFT_eSPI tft = TFT_eSPI();

int counter = 0;
int previousCounter = 0;
bool buttonPressed = false;
// counter variables for button
bool gifPlaying = false;
// Checks if gif is playing or not to know when to clear the screen

int checkCounter();
// Refers to checkCounter function to know when button is pressed
void backgroundLoop();
// Background Loop function
void displayGIF(uint8_t* gifData, size_t gifSize);
// Function to display GIF

unsigned long loopInterval = 10; // Loop interval in milliseconds
unsigned long previousLoopTime = 0; 
// Background loop intervals for checking button

void setup() {
  Serial.begin(115200);
  tft.begin();
  // enabled baud rate and starts the tft

  pinMode(TFT_BACKLIGHT_PIN, OUTPUT);
  digitalWrite(TFT_BACKLIGHT_PIN, HIGH);
  pinMode(button, INPUT);
  // gives all the pins value
 
#ifdef USE_DMA
  tft.initDMA();
#endif
  tft.setRotation(1);
  tft.fillScreen(TFT_BLACK);

  gif.begin(BIG_ENDIAN_PIXELS);
  // Sets up TFT
}

#ifdef NORMAL_SPEED // Render at rate that is GIF controlled
void loop()
{
  Serial.println(counter);
  // prints button check
  
  unsigned long currentMillis = millis();
  if (currentMillis - previousLoopTime >= loopInterval) {
    previousLoopTime = currentMillis; // Update the previous loop time
    backgroundLoop(); // Call the background loop function
  }

  switch(counter) {
    case 0:
      displayGIF((uint8_t*)GIF_IMAGE, sizeof(GIF_IMAGE));
      break;
    case 1:
      displayGIF((uint8_t*)GIF_IMAGE1, sizeof(GIF_IMAGE1));
      break;
    case 2:
      displayGIF((uint8_t*)GIF_IMAGE2, sizeof(GIF_IMAGE2));
      break;
    default:
      counter = 0;
      break;
  }
  // Basically checking what the counter is on... uses that for deciding which gif to play
}

#endif

int checkCounter() {
  if (digitalRead(button) == HIGH && !buttonPressed) {
    buttonPressed = true;
    counter++;
    /*
      Checks if the button is pressed
      The !buttonPressed is to avoid registering 2 presses when you only intended for one (sorta like a delay)
      Adds one to the counter
    */

    if (previousCounter != counter) {
      gifPlaying = false;
      previousCounter = counter;
    }
    /*
      The purpose fo this is to register if the counter changed, 
      indicating that the GIF has changed, so I can clear the TFT screen
    */
  }

  if (digitalRead(button) == LOW) {
    buttonPressed = false;
    // when the button goes low the buttonPressed is false so the first if statement can run
  }

  return counter;
  // returns the value of counter
}

void displayGIF(uint8_t* gifData, size_t gifSize)
{
  if (!gifPlaying) {
    tft.fillScreen(TFT_BLACK);
    // clears screen when GIF is switched
  }
  
  if (gif.open(gifData, gifSize, GIFDraw)) {
    gifPlaying = true;
    Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
    tft.startWrite(); // The TFT chip select is locked low
    while (gif.playFrame(true, NULL)) {
      yield();
    }
    gif.close();
    tft.endWrite(); // Release TFT chip select for other SPI devices
    // Display GIF function
  }
}

void backgroundLoop() {
  checkCounter();
  Serial.println("Background loop");
}

// to use the function above displayGIF((uint8_t*)GIF_IMAGE, sizeof(GIF_IMAGE))

// Code here is just for reference to show how much I shortened the loop up by creating a function and using switch case statements

/*
if (gif.open((uint8_t *)GIF_IMAGE, sizeof(GIF_IMAGE), GIFDraw))
{
  Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
  tft.startWrite(); // The TFT chip select is locked low
  while (gif.playFrame(true, NULL))
  {
    yield();
  }
  checkCounter();
}
gif.close();
tft.endWrite(); // Release TFT chip select for other SPI devices

} else {
// reset TFT Screen
tft.fillScreen(TFT_BLACK);
// Badger GIF Display
if (counter == 1) {
  if (gif.open((uint8_t *)GIF_IMAGE1, sizeof(GIF_IMAGE1), GIFDraw))
  {
    Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
    tft.startWrite(); // The TFT chip select is locked low
    while (gif.playFrame(true, NULL))
    {
      yield();
    }
    checkCounter();
    gif.close();
    tft.endWrite(); // Release TFT chip select for other SPI devices
  }
}
} else {
// reset TFT Screen
tft.fillScreen(TFT_BLACK);
// Badger GIF Display
if (counter == 2) {
  if (gif.open((uint8_t *)GIF_IMAGE1, sizeof(GIF_IMAGE2), GIFDraw))
  {
    Serial.printf("Successfully opened GIF; Canvas size = %d x %d\n", gif.getCanvasWidth(), gif.getCanvasHeight());
    tft.startWrite(); // The TFT chip select is locked low
    while (gif.playFrame(true, NULL))
    {
      yield();
    }
    checkCounter();
    gif.close();
    tft.endWrite(); // Release TFT chip select for other SPI devices
  }
}
} else {
  counter = 0;
}
#else // Test maximum rendering speed
void loop()
{
  long lTime = micros();
  int iFrames = 0;

  if (gif.open((uint8_t *)GIF_IMAGE, sizeof(GIF_IMAGE), GIFDraw))
  {
    tft.startWrite(); // For DMA the TFT chip select is locked low
    while (gif.playFrame(false, NULL))
    {
      // Each loop renders one frame
      iFrames++;
      yield();
    }
    gif.close();
    tft.endWrite(); // Release TFT chip select for other SPI devices
    lTime = micros() - lTime;
    Serial.print(iFrames / (lTime / 1000000.0));
    Serial.println(" fps");
  }
} 
*/
