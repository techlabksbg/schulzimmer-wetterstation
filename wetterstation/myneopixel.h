#include <NeoPixelBus.h>

// See https://github.com/Makuna/NeoPixelBus/wiki

const uint8_t PixelPin = 25;
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(1, PixelPin);

// ppm, r, g, b
int colormap[][4] = {
  {0,0, 0,255},
  {400,0,0,255},
  {600,0,255,0},
  {800,0,255,0}, 
  {1200,255,255,0},
  {1500,255,255,0},
  {2000,255,0,0},
  {10000,255,0,0}  
};

RgbColor mycolor(0,0,0);

void setColor(uint8_t r, uint8_t g, uint8_t b) {
  strip.SetPixelColor(0, RgbColor(r,g,b));
}
                 
// i ranges from 0 to 250 for animation when ppm>2000
void neopixel_update(int ppm, int i) {
  i=0;
  while (ppm<colormap[i+1][0]) i++;
  float t = ((float)(ppm-colormap[i][0]))/(colormap[i+1][0]-colormap[i][0]);
  float mul=1.0;
  if (ppm>2000) {
    mul = 1.0-abs(125-i)/125.0;
  }
  uint8_t rgb[3];
  for (int i=0; i<3; i++) {
    rgb[i] = (uint8_t)(mul*(t*colormap[i+1][i+1]+(1-t)*colormap[i][i+1]));
  }
  mycolor.R=rgb[0];
  mycolor.G=rgb[1];
  mycolor.B=rgb[2];
  strip.SetPixelColor(0, mycolor);
  strip.Show();
}


void neopixelAnimate(int waitforit) {
  long int ms = millis();
  static int hue=0;
  while (millis()-ms<waitforit) {
    strip.SetPixelColor(0, HslColor(hue,255,255));
    strip.Show();
    hue=(hue+1)%255;
    delay(10);
  }
}

void neopixel_setup() {
  strip.Begin();
  strip.Show();
}


