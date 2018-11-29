#include <NeoPixelBus.h>

// See https://github.com/Makuna/NeoPixelBus/wiki

const uint8_t PixelPin = 25;
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(1, PixelPin);

NeoGamma<NeoGammaTableMethod> colorGamma;

// ppm, r, g, b
int colormap[8][4] = {
  {0,0, 0,255},
  {400,0,0,255},
  {600,0,255,0},
  {800,0,255,0}, 
  {1200,255,200,0},
  {1500,255,100,0},
  {2000,255,0,0},
  {10000,255,0,0}  
};

RgbColor mycolor(0,0,0);

void setColor(uint8_t r, uint8_t g, uint8_t b) {
  strip.SetPixelColor(0, colorGamma.Correct(RgbColor(r,g,b)));
}
                 
// i ranges from 0 to 250 for animation when ppm>2000
void neopixel_update(int ppm, int iter) {
  int i=0;
  while (ppm>=colormap[i+1][0]) {
    i++;
  }
  //Serial.printf("ppm=%d, i=%d ", ppm,i);
  float t = ((float)(ppm-colormap[i][0]))/(colormap[i+1][0]-colormap[i][0]);
  float mul=1.0;
  if (ppm>2000) {
    mul = 1.0-abs(125-iter)/125.0;
  }
  //Serial.printf("ppm=%d, i=%d, t=%f, mul=%f\r\n", ppm,i,t,mul);
  uint8_t rgb[3];
  for (int c=0; c<3; c++) {
    //Serial.printf("  [%d]   %d -> %d\r\n",i,colormap[i][c+1], colormap[i][c+1]);
    rgb[c] = (uint8_t)(mul*(t*colormap[i+1][c+1]+(1-t)*colormap[i][c+1]));
  }
  if (ppm<2500) {
    mycolor.R=rgb[0];
    mycolor.G=rgb[1];
    mycolor.B=rgb[2];
  } else { // red/blue flashing after 2500
    mycolor.R = (iter%50<25) ? 255 : 0;
    mycolor.B = 0;
    mycolor.R = (iter%50<25) ? 0 : 255;
  }
//  Serial.printf("r=%d, g=%d, b=%d\r\n", rgb[0], rgb[1], rgb[2]);
  strip.SetPixelColor(0, colorGamma.Correct(mycolor));
  strip.Show();
}


void neopixelAnimate(int waitforit) {
  long int ms = millis();
  static float hue=0.0;
  while (millis()-ms<waitforit) {
    strip.SetPixelColor(0, colorGamma.Correct(RgbColor(HslColor(hue,1.0,0.5))));
    strip.Show();
    hue+=0.01;
    if (hue>1.0) hue=0.0;
    delay(10);
  }
}

void neopixel_setup() {
  strip.Begin();
  strip.Show();
}


