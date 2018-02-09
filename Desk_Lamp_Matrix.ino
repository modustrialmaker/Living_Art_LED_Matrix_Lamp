#include <FastLED.h>
//#include <Adafruit_NeoPixel.h>

/** BASIC CONFIGURATION  **/
#define BRIGHTNESS          100
#define FRAMES_PER_SECOND  60
 
//The pin that controls the LEDs
#define LED_PIN 3
#define switchPin 6

uint8_t led_volts = 5;
uint32_t max_milliamps = 2000;


////////////////////////////////////////////////////////////////////////////////
////////////////MATRIX PATTERN SETUP

const uint8_t kMatrixWidth  = 16;
const uint8_t kMatrixHeight = 16;
const bool    kMatrixSerpentineLayout = true;

//The amount of LEDs in the setup
#define NUM_LEDS (kMatrixWidth * kMatrixHeight)
#define MAX_DIMENSION ((kMatrixWidth>kMatrixHeight) ? kMatrixWidth : kMatrixHeight)


// The 16 bit version of our coordinates
static uint16_t x;
static uint16_t y;
static uint16_t z;

// We're using the x/y dimensions to map to the x/y pixels on the matrix.  We'll
// use the z-axis for "time".  speed determines how fast time moves forward.  Try
// 1 for a very slow moving effect, or 60 for something that ends up looking like
// water.
uint16_t speed = 6; // speed is set dynamically once we've started up

// Scale determines how far apart the pixels in our noise matrix are.  Try
// changing these values around to see how it affects the motion of the display.  The
// higher the value of scale, the more "zoomed out" the noise iwll be.  A value
// of 1 will be so zoomed in, you'll mostly see solid colors.
uint16_t scale = 30; // scale is set dynamically once we've started up

// This is the array that we keep our computed noise values in
uint8_t noise[MAX_DIMENSION][MAX_DIMENSION];


CRGBPalette16 currentPalette( PartyColors_p );
uint8_t       colorLoop = 1;

// x,y, & time values
uint32_t v_time,hue_time,hxy;

// how many octaves to use for the brightness and hue functions
uint8_t octaves=1;
uint8_t hue_octaves=3;

// the 'distance' between x/y points for the hue noise
int hue_scale=1;

// how fast we move through time & hue noise
int time_speed=100;
int hue_speed=10;

// adjust these values to move along the x or y axis between frames
int x_speed=331;
int y_speed=1111;

// the 'distance' between points on the x and y axis
int xscale=57771;
int yscale=57771;



void led_off();
int oldMode = 0;  // assume switch closed because of pull-down resistor

//How many LEDs to we display
//int height = NUM_LEDS;

/*Not really used yet. Thought to be able to switch between sound reactive
mode, and general gradient pulsing/static color*/
//int mode = 0;


//Led array
CRGB leds[NUM_LEDS];



// FOR SYLON ETC
//uint8_t thisbeat =  23;
//uint8_t thatbeat =  28;
//uint8_t thisfade =   2;                                     // How quickly does it fade? Lower = slower fade rate.
//uint8_t thissat = 255;                                     // The saturation, where 255 = brilliant colours.
//uint8_t thisbri = 255; 
//int    myhue =   0;

//FOR JUGGLE
//uint8_t numdots = 4;                                          // Number of dots in use.
//uint8_t faderate = 2;                                         // How long should the trails be. Very low value = longer trails.
//uint8_t hueinc = 16;                                          // Incremental change in hue between each dot.
//uint8_t thishue = 0;                                          // Starting hue.
//uint8_t curhue = 0; 
//uint8_t thisbright = 255;                                     // How bright should the LED/display be.
//uint8_t basebeat = 5; 
//uint8_t max_bright = 255;

// Twinkle
float redStates[NUM_LEDS];
float blueStates[NUM_LEDS];
float greenStates[NUM_LEDS];
float Fade = 0.96;

//For Fireplace
//CRGBPalette16 gPal;
//#define FPS 48
//#define FPS_DELAY 1000/FPS
//#define COOLING 20  
//#define HOT 100
//#define MAXHOT HOT*kMatrixHeight


uint8_t gHue = 0; // rotating "base color" used by many of the patterns



///////Setup Function run when turned on

void setup() {
    analogReference(EXTERNAL);                                  // Connect 3.3V to AREF pin for any microphones using 3.3V

  //Serial.begin(9600);
  pinMode (switchPin, INPUT);
  //Set all lights to make sure all are working as expected
  FastLED.addLeds<NEOPIXEL, LED_PIN>(leds, NUM_LEDS);
  set_max_power_in_volts_and_milliamps (led_volts, max_milliamps);
  
//  for (int i = 0; i < NUM_LEDS; i++) 
//    leds[i] = CRGB(70, 70, 70);
//  FastLED.show(); 
  delay(200);  
  

   // Initialize our coordinates to some random values
  x = random16();
  y = random16();
  z = random16();


 

}

// Lists of patterns to cycle through.  Each is defined as a separate function below.
typedef void (*SimplePatternList[])();
SimplePatternList gPatterns = { blobby_flow, blobby_flow, blobby_flow, XYmatrix, rainbow, rainbowWithGlitter, confetti, blobby_flow_gaps, blobby_flow, blobby_flow, blobby_flow, blobby_flow, sinelon, matrix_code, blobby_flow };  //non-reactive patterns


uint8_t gCurrentPatternNumber = 0; // Index number of which pattern is current

void loop ()
  {
  // see if switch is open or closed
  int switchState = digitalRead (switchPin);
 
  // has it changed since last time?
  if (switchState == 0)
    {
      delay(300);
 
    if (oldMode == 6)
       {
        oldMode = 0;

       }  // end if switchState is LOW
      else {
        oldMode++;

      }
    }  // end of state change

  switch(oldMode) {
    case 0:
      rotate_all_patterns();
      break;
 
    case 1:
      warm_white();
      break;
     
     case 2:
      max_white();
      //all2();
      break;

    case 3:
      nightlight();
      //Fireplace();
      break;

//    case 4:
//      Fireplace();
//      break;

    case 4:
      matrix_code();
      break;

    case 5:
      led_off();
      break;    
      
    default:
      break;
  }
//    delay(DELAY);       // delay in between reads for stability
}


//////////////////////////////////////////////
//All red nightlight mode

void nightlight() {
      //Serial.println("running led_mode3");

   for (int i =0; i < NUM_LEDS; i++) 
    leds[i] = CHSV (0, 250, 100);
  

FastLED.show();
delay(5);
}

void warm_white() {
     // Serial.println("running led_mode3");

   for (int i =0; i < NUM_LEDS; i++) 
    leds[i] = CRGB(255, 172, 68);

FastLED.show();
delay(5);
}



void led_off() {
     // Serial.println("turning off LEDs");

   for (int i =0; i < NUM_LEDS; i++) 
    leds[i] = CRGB(0, 0, 0);
  

FastLED.show();
delay(5);
}

void max_white() {
    //  Serial.println("turning off LEDs");

   for (int i =0; i < NUM_LEDS; i++) 
    leds[i] = CRGB(255, 255, 255);
  

FastLED.show();
delay(5);
}




//////////////////////////////////////////////////////////////////////////
//FASTLED MATRIX FUNCTIONS
//////////////////////////////////////////////////////////////////////////

void blobby_flow_gaps() {
  // fill the led array 2/16-bit noise values
  fill_2dnoise16(LEDS.leds(), kMatrixWidth, kMatrixHeight, kMatrixSerpentineLayout,
                octaves,x,xscale,y,yscale,v_time,
                hue_octaves,hxy,hue_scale,hxy,hue_scale,hue_time, false);

  LEDS.show();

  // adjust the intra-frame time values
  x += x_speed;
  y += y_speed;
  v_time += time_speed;
  hue_time += hue_speed;
   delay(50);
}


void blobby_flow() {
  
  
  ChangePaletteAndSettingsPeriodically();
  
  // generate noise data
  fillnoise8();
  
  // convert the noise data to colors in the LED array
  // using the current palette
  mapNoiseToLEDsUsingPalette();

  LEDS.show();
  // delay(10);
}



// Fill the x/y array of 8-bit noise values using the inoise8 function.
void fillnoise8() {
  // If we're runing at a low "speed", some 8-bit artifacts become visible
  // from frame-to-frame.  In order to reduce this, we can do some fast data-smoothing.
  // The amount of data smoothing we're doing depends on "speed".
  uint8_t dataSmoothing = 0;
  if( speed < 50) {
    dataSmoothing = 200 - (speed * 4);
  }
  
  for(int i = 0; i < MAX_DIMENSION; i++) {
    int ioffset = scale * i;
    for(int j = 0; j < MAX_DIMENSION; j++) {
      int joffset = scale * j;
      
      uint8_t data = inoise8(x + ioffset,y + joffset,z);

      // The range of the inoise8 function is roughly 16-238.
      // These two operations expand those values out to roughly 0..255
      // You can comment them out if you want the raw noise data.
      data = qsub8(data,16);
      data = qadd8(data,scale8(data,39));

      if( dataSmoothing ) {
        uint8_t olddata = noise[i][j];
        uint8_t newdata = scale8( olddata, dataSmoothing) + scale8( data, 256 - dataSmoothing);
        data = newdata;
      }
      
      noise[i][j] = data;
    }
  }
  
  z += speed;
  
  // apply slow drift to X and Y, just for visual variation.
  x += speed / 8;
  y -= speed / 16;
}

void mapNoiseToLEDsUsingPalette()
{
  static uint8_t ihue=0;
  
  for(int i = 0; i < kMatrixWidth; i++) {
    for(int j = 0; j < kMatrixHeight; j++) {
      // We use the value at the (i,j) coordinate in the noise
      // array for our brightness, and the flipped value from (j,i)
      // for our pixel's index into the color palette.

      uint8_t index = noise[j][i];
      uint8_t bri =   noise[i][j];

      // if this palette is a 'loop', add a slowly-changing base value
      if( colorLoop) { 
        index += ihue;
      }

      // brighten up, as the color palette itself often contains the 
      // light/dark dynamic range desired
      if( bri > 127 ) {
        bri = 255;
      } else {
        bri = dim8_raw( bri * 2);
      }

      CRGB color = ColorFromPalette( currentPalette, index, bri);
      leds[XY(i,j)] = color;
    }
  }
  
  ihue+=1;
}



// There are several different palettes of colors demonstrated here.
//
// FastLED provides several 'preset' palettes: RainbowColors_p, RainbowStripeColors_p,
// OceanColors_p, CloudColors_p, LavaColors_p, ForestColors_p, and PartyColors_p.
//
// Additionally, you can manually define your own color palettes, or you can write
// code that creates color palettes on the fly.

// 1 = 5 sec per palette
// 2 = 10 sec per palette
// etc
#define HOLD_PALETTES_X_TIMES_AS_LONG 1

void ChangePaletteAndSettingsPeriodically()
{
  EVERY_N_MILLISECONDS(100) {
  uint8_t secondHand = ((millis() / 1000) / HOLD_PALETTES_X_TIMES_AS_LONG) % 60;
  static uint8_t lastSecond = 99;
  
  if( lastSecond != secondHand) {
    lastSecond = secondHand;
    if( secondHand ==  0)  { 
      currentPalette = RainbowColors_p;  
      speed = 6;       
//      speed = 20; 
      scale = 30; 
      colorLoop = 1; }
    if( secondHand ==  5)  { 
      SetupPurpleAndGreenPalette();             
     speed = 4; 
//     speed = 10; 
      scale = 50; 
      colorLoop = 1; }
    if( secondHand == 10)  { 
      SetupRandomPalette();          
      speed = 4; 
//      speed = 20;  
      scale = 30; 
      colorLoop = 1; }
    if( secondHand == 15)  { 
      currentPalette = ForestColors_p;          
      speed =  12; 
      scale =120; 
      colorLoop = 0; }
    if( secondHand == 20)  { 
      SetupRandomPalette();          
      speed =  6; 
      scale = 30; 
      colorLoop = 1; }
    if( secondHand == 25)  { 
      currentPalette = LavaColors_p;            
      speed =  8; 
      scale = 50; 
      colorLoop = 0; }
    if( secondHand == 30)  { 
       SetupRandomPalette();        
      speed = 4; 
//      speed = 20; 
      scale = 40; 
      colorLoop = 0; }
    if( secondHand == 35)  { 
      currentPalette = PartyColors_p;           
      speed = 8; 
//      speed = 20; 
      scale = 30; 
      colorLoop = 1; }
    if( secondHand == 40)  { 
      SetupBlackAndWhiteStripedPalette();                   
     speed = 8; 
//      speed = 20; 
      scale = 20; 
      colorLoop = 1; }
    if( secondHand == 45)  { 
      SetupRandomPalette();                     
     speed = 5; 
//     speed = 50; 
      scale = 25; 
      colorLoop = 1; }
    if( secondHand == 50)  { 
      currentPalette = CloudColors_p;                       
     speed = 8; 
//     speed = 90; 
      scale = 90; 
      colorLoop = 1; }
    if( secondHand == 55)  { 
      currentPalette = OceanColors_p;   
     speed = 8; 
//     speed = 30; 
      scale = 20; 
      colorLoop = 1; }
  }
  }
}

void ChangeSettingsPeriodically()
{
  EVERY_N_MILLISECONDS(100) {
  uint8_t secondHand = ((millis() / 1000) / HOLD_PALETTES_X_TIMES_AS_LONG) % 60;
  static uint8_t lastSecond = 99;
  
  if( lastSecond != secondHand) {
    lastSecond = secondHand;
    if( secondHand ==  0)  { 
      speed = 3;       
//      speed = 20; 
      scale = 30; 
      colorLoop = 1; }
    if( secondHand ==  5)  { 
     speed = 2; 
//     speed = 10; 
      scale = 50; 
      colorLoop = 1; }
    if( secondHand == 10)  { 
      speed = 2; 
//      speed = 20;  
      scale = 30; 
      colorLoop = 1; }
    if( secondHand == 15)  { 
      speed =  6; 
      scale =120; 
      colorLoop = 0; }
    if( secondHand == 20)  { 
      speed =  3; 
      scale = 30; 
      colorLoop = 0; }
    if( secondHand == 25)  { 
      currentPalette = LavaColors_p;            
      speed =  4; 
      scale = 50; 
      colorLoop = 0; }
    if( secondHand == 30)  { 
      speed = 4; 
//      speed = 20; 
      scale = 90; 
      colorLoop = 0; }
    if( secondHand == 35)  { 
      speed = 4; 
//      speed = 20; 
      scale = 30; 
      colorLoop = 1; }
    if( secondHand == 40)  { 
     speed = 4; 
//      speed = 20; 
      scale = 20; 
      colorLoop = 1; }
    if( secondHand == 45)  { 
     speed = 4; 
//     speed = 50; 
      scale = 50; 
      colorLoop = 1; }
    if( secondHand == 50)  { 
     speed = 8; 
//     speed = 90; 
      scale = 90; 
      colorLoop = 1; }
    if( secondHand == 55)  { 
     speed = 4; 
//     speed = 30; 
      scale = 20; 
      colorLoop = 1; }
  }
  }
}

// This function generates a random palette that's a gradient
// between four different colors.  The first is a dim hue, the second is 
// a bright hue, the third is a bright pastel, and the last is 
// another bright hue.  This gives some visual bright/dark variation
// which is more interesting than just a gradient of different hues.
void SetupRandomPalette()
{
  currentPalette = CRGBPalette16( 
                      CHSV( random8(), 255, 32), 
                      CHSV( random8(), 255, 255), 
                      CHSV( random8(), 128, 255), 
                      CHSV( random8(), 255, 255)); 
}

// This function sets up a palette of black and white stripes,
// using code.  Since the palette is effectively an array of
// sixteen CRGB colors, the various fill_* functions can be used
// to set them up.
void SetupBlackAndWhiteStripedPalette()
{
  // 'black out' all 16 palette entries...
  fill_solid( currentPalette, 16, CRGB::Black);
  // and set every fourth one to white.
  currentPalette[0] = CRGB::White;
  currentPalette[4] = CRGB::White;
  currentPalette[8] = CRGB::White;
  currentPalette[12] = CRGB::White;

}

// This function sets up a palette of purple and green stripes.
void SetupPurpleAndGreenPalette()
{
  CRGB purple = CHSV( HUE_PURPLE, 255, 255);
  CRGB green  = CHSV( HUE_GREEN, 255, 255);
  CRGB black  = CRGB::Black;
  
  currentPalette = CRGBPalette16( 
    green,  green,  black,  black,
    purple, purple, black,  black,
    green,  green,  black,  black,
    purple, purple, black,  black );
}


//
// Mark's xy coordinate mapping code.  See the XYMatrix for more information on it.
//
uint16_t XY( uint8_t x, uint8_t y)
{
  uint16_t i;
  if( kMatrixSerpentineLayout == false) {
    i = (y * kMatrixWidth) + x;
  }
  if( kMatrixSerpentineLayout == true) {
    if( y & 0x01) {
      // Odd rows run backwards
      uint8_t reverseX = (kMatrixWidth - 1) - x;
      i = (y * kMatrixWidth) + reverseX;
    } else {
      // Even rows run forwards
      i = (y * kMatrixWidth) + x;
    }
  }
  return i;
}



////////////////////////////////////
///Blur Visualize

void blur() {


  EVERY_N_MILLISECONDS(40) {
  uint8_t blurAmount = dim8_raw( beatsin8(3,64, 192) );       // A sinewave at 3 Hz with values ranging from 64 to 192.
  blur1d( leds, NUM_LEDS, blurAmount);                        // Apply some blurring to whatever's already on the strip, which will eventually go black.
  
  uint8_t  i = beatsin8(  9, 0, NUM_LEDS-1);
  uint8_t  j = beatsin8( 7, 0, NUM_LEDS-1);
  uint8_t  k = beatsin8(  5, 0, NUM_LEDS-1);

  
  // The color of each point shifts over time, each at a different speed.
  uint16_t ms = millis();  
  leds[(i+j)/2] = CHSV( ms / 29, 200, 255);
  leds[(j+k)/2] = CHSV( ms / 41, 200, 255);
  leds[(k+i)/2] = CHSV( ms / 73, 200, 255);
  leds[(k+i+j)/3] = CHSV( ms / 53, 200, 255);
 
  FastLED.show();
  }
 
} // loop()



//////Matrix Sinelon
void matrix_sinelon() {
    fadeToBlackBy( leds, NUM_LEDS, 25);
    int pos = beatsin16( 19, 0, kMatrixWidth-1 );

    for(int i = 0; i < kMatrixWidth; i++) {
      leds[XY(i, pos)] += CHSV( gHue, 240, 140);
    }

    for(int i = 0; i < kMatrixHeight; i++) {
      leds[XY(pos, i)] += CHSV( gHue+100, 240, 140);
    }
    FastLED.show();
  EVERY_N_MILLISECONDS( 50 ) { gHue++; } // slowly cycle the "base color" through the rainbow

}



//////////////////////////////////////////////////////
//////////////////Rotate Non-Reactive Visualizers

void rotate_all_patterns() {

  gPatterns[gCurrentPatternNumber]();

  // send the 'leds' array out to the actual LED strip
  FastLED.show();  
  // insert a delay to keep the framerate modest
  FastLED.delay(1000/FRAMES_PER_SECOND); 

  // do some periodic updates
  EVERY_N_MILLISECONDS( 20 ) { gHue++; } // slowly cycle the "base color" through the rainbow
  EVERY_N_SECONDS( 20 ) { nextPattern(); } // change patterns periodically

}

#define ARRAY_SIZE(A) (sizeof(A) / sizeof((A)[0]))

void nextPattern()
{
  // add one to the current pattern number, and wrap around at the end
  gCurrentPatternNumber = (gCurrentPatternNumber + 1) % ARRAY_SIZE( gPatterns);
}

void rainbow() 
{
  // FastLED's built-in rainbow generator
  fill_rainbow( leds, NUM_LEDS, gHue, 7);
}

void rainbowWithGlitter() 
{
  // built-in FastLED rainbow, plus some random sparkly glitter
  rainbow();
  addGlitter(80);
}

void addGlitter( fract8 chanceOfGlitter) 
{
  if( random8() < chanceOfGlitter) {
    leds[ random16(NUM_LEDS) ] += CRGB::White;
  }
}

void confetti() 
{
  // random colored speckles that blink in and fade smoothly
  fadeToBlackBy( leds, NUM_LEDS, 10);
  int pos = random16(NUM_LEDS-1);
  leds[pos] += CHSV( gHue + random8(64), 200, 255);
}

void sinelon()
{
  // a colored dot sweeping back and forth, with fading trails
  fadeToBlackBy( leds, NUM_LEDS, 20);
  int pos = beatsin16( 13, 0, NUM_LEDS-1 );
  leds[pos] += CHSV( gHue, 255, 192);
}

//void bpm()
//{
//  // colored stripes pulsing at a defined Beats-Per-Minute (BPM)
//  uint8_t BeatsPerMinute = 62;
//  CRGBPalette16 palette = PartyColors_p;
//  uint8_t beat = beatsin8( BeatsPerMinute, 64, 255);
//  for( int i = 0; i < NUM_LEDS; i++) { //9948
//    leds[i] = ColorFromPalette(palette, gHue+(i*2), beat-gHue+(i*10));
//  }
//}

//void juggle() {
//  // eight colored dots, weaving in and out of sync with each other
//  fadeToBlackBy( leds, NUM_LEDS, 20);
//  byte dothue = 0;
//  for( int i = 0; i < 8; i++) {
//    leds[beatsin16( i+7, 0, NUM_LEDS-1 )] |= CHSV(dothue, 200, 255);
//    dothue += 32;
//  }
//}

///////////////////////////////////////////////////////////////////
//FASTLED XYMATRIX PATTERN EXAMPLE

void XYmatrix()
{
    uint32_t ms = millis();
    int32_t yHueDelta32 = ((int32_t)cos16( ms * (27/1) ) * (350 / kMatrixWidth));
    int32_t xHueDelta32 = ((int32_t)cos16( ms * (39/1) ) * (310 / kMatrixHeight));
    DrawOneFrame( ms / 65536, yHueDelta32 / 32768, xHueDelta32 / 32768);
    if( ms < 5000 ) {
      FastLED.setBrightness( scale8( BRIGHTNESS, (ms * 256) / 5000));
    } else {
      FastLED.setBrightness(BRIGHTNESS);
    }
    FastLED.show();
}

void DrawOneFrame( byte startHue8, int8_t yHueDelta8, int8_t xHueDelta8)
{
  byte lineStartHue = startHue8;
  for( byte y = 0; y < kMatrixHeight; y++) {
    lineStartHue += yHueDelta8;
    byte pixelHue = lineStartHue;      
    for( byte x = 0; x < kMatrixWidth; x++) {
      pixelHue += xHueDelta8;
      leds[ XY(x, y)]  = CHSV( pixelHue, 255, 255);
    }
  }
}



/////////////////////////////////////////////////
/////////////////////////////////////////////////
//Fireplace visualizer
//
//void Fireplace () {
//  
//  random16_add_entropy( random() ); // We chew a lot of entropy
//
//  static unsigned int spark[kMatrixHeight]; // base heat
//  CRGB stack[kMatrixHeight][kMatrixWidth];        // stacks that are cooler
// 
//  // 1. Generate sparks to re-heat
//  for( int i = 0; i < kMatrixHeight; i++) {
//    if (spark[i] < HOT ) {
//      int base = HOT * 2;
//      spark[i] = random16( base, MAXHOT );
//    }
//  }
//  
//  // 2. Cool all the sparks
//  for( int i = 0; i < kMatrixHeight; i++) {
//    spark[i] = qsub8( spark[i], random8(0, COOLING) );
//  }
//  
//  // 3. Build the stack
//  /*    This works on the idea that pixels are "cooler"
//        as they get further from the spark at the bottom */
//  for( int i = 0; i < kMatrixHeight; i++) {
//    unsigned int heat = constrain(spark[i], HOT, MAXHOT);
//    for( int j = kMatrixWidth-1; j >= 0; j--) {
//      /* Calculate the color on the palette from how hot this
//         pixel is */
//      byte index = constrain(heat, 0, HOT);
//      stack[i][j] = ColorFromPalette( gPal, index );
//      
//      /* The next higher pixel will be "cooler", so calculate
//         the drop */
//      unsigned int drop = random8(0,HOT);
//      if (drop > heat) heat = 0; // avoid wrap-arounds from going "negative"
//      else heat -= drop;
// 
//      heat = constrain(heat, 0, MAXHOT);
//    }
//  }
//  
//  // 4. map stacks to led array
//  for( int i = 0; i < kMatrixHeight; i++) {
//    for( int j = 0; j < kMatrixWidth; j++) {
//     leds[ XY(j, i) ] = stack[i][j];
//    }
//  }
//  FastLED.show();
//  FastLED.delay(FPS_DELAY); 
//}

////////////////////////////////////////////////////
//FastLED Matrix Code Pattern

void matrix_code() {
  EVERY_N_MILLIS(75) // falling speed
  {
    // move code downward
    // start with lowest row to allow proper overlapping on each column
    for (int8_t row=kMatrixHeight-1; row>=0; row--)
    {
      for (int8_t col=0; col<kMatrixWidth; col++)
      {
        if (leds[getIndex(row, col)] == CRGB(175,255,175))
        {
          leds[getIndex(row, col)] = CRGB(27,130,39); // create trail
          if (row < kMatrixHeight-1) leds[getIndex(row+1, col)] = CRGB(175,255,175);
        }
      }
    }

    // fade all leds
    for(int i = 0; i < NUM_LEDS; i++) {
      if (leds[i].g != 255) leds[i].nscale8(192); // only fade trail
    }

    // check for empty screen to ensure code spawn
    bool emptyScreen = true;
    for(int i = 0; i < NUM_LEDS; i++) {
      if (leds[i])
      {
        emptyScreen = false;
        break;
      }
    }

    // spawn new falling code
    if (random8(3) == 0 || emptyScreen) // lower number == more frequent spawns
    {
      int8_t spawnX = random8(kMatrixWidth);
      leds[getIndex(0, spawnX)] = CRGB(175,255,175 );
    }

    FastLED.show();
  }
}

// convert x/y cordinates to LED index on zig-zag grid
uint16_t getIndex(uint16_t x, uint16_t y)
{
  uint16_t index;
  if (y == 0)
  {
    index = x;
  }
  else if (y % 2 == 0)
  {
    index = y * kMatrixWidth + x;
  }
  else
  {
    index = ((y * kMatrixWidth) + (kMatrixWidth-1)) - x;
  }
  return index;
}



