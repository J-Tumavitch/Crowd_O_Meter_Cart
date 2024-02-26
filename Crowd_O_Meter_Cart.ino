#include <Adafruit_NeoPixel.h>

#define STRANDMAX 50
#define NEOPIN    5
#define AINPIN    A0
#define ARANGE    2048
#define PPMAX     775
#define WINDOW    50*4
#define TCNUM     1
#define TCDEN     10

unsigned int strandLevel  = 0; 
unsigned int strandLevelF = 0;

const int sampleWindow = 50; // 50 mS Sample window width (20Hz)

unsigned int sample;

Adafruit_NeoPixel pixels(STRANDMAX, NEOPIN, NEO_GRB + NEO_KHZ800);

void setup() {
  Serial.begin(9600);
  pixels.begin(); // INITIALIZE NeoPixel strip object (REQUIRED)
}

void loop() {

  unsigned long startMillis = millis(); // Sample Window Start
  unsigned int peakToPeak = 0;   // Pk to Pk Level
  unsigned int signalMax = 0;
  unsigned int signalMin = ARANGE;

  while ( millis() - startMillis < WINDOW ) {
    sample = analogRead(AINPIN);

    if (sample < ARANGE) { // Cast out spurious readings

      if (sample > signalMax) {
        signalMax = sample;  // Save just the MAX levels
      }
      else if (sample < signalMin) {
        signalMin = sample;  // Save just the MIN levels
      }
    }
  }

  peakToPeak = signalMax - signalMin;

  unsigned int strandLevelF_Old = strandLevelF;
  strandLevel  = peakToPeak / ( PPMAX / STRANDMAX );
  strandLevelF = ( strandLevel * TCNUM) / TCDEN  + ( strandLevelF * (TCDEN - TCNUM) ) / TCDEN;


  Serial.print(signalMin);
  Serial.print(", ");
  Serial.print(signalMax);
  Serial.print(", ");
  Serial.print(peakToPeak);
  Serial.print(", ");
  Serial.print(strandLevel);
  Serial.print(", ");
  Serial.println(strandLevelF);

  pixels.clear();
  for(int i=0; i<strandLevelF; i++) { 
    pixels.setPixelColor(i, pixels.Color( 5*i, 5*(STRANDMAX-i), 0));
  }
  for(int i=strandLevelF; i<STRANDMAX; i++) {
    pixels.setPixelColor(i, pixels.Color( 0, 0, 0));  
  }

  unsigned int maxpos = max(strandLevelF, strandLevelF_Old);

  pixels.setPixelColor(maxpos, pixels.Color( 255, 255, 255));
  
  pixels.show();   
}