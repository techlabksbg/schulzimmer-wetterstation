#ifndef MYSTEPPER_INCLUDED
#define MYSTEPPER_INCLUDED



class MyStepper {  // pin 16 is labeled RX2

private:
  int motorpins[4] = {15,32,16,5};//{5,16, 32, 15}; //{5,16,32,15}; 
  int motorstep=0;
  int fullpos=2048;
  int zeropos=-40;
  int motorposition=0;
  int sollposition=0;
  int limitpin = 34;

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
 


  void makestep();
  void off();
  void forward();
  void backward();
  void setPixel(int ppm);


public:
  
  void home();
  void gotoPPM(int ppm);
  void gotoPos(int pos);
  void demo();
  
  MyStepper();
  
  
  
};



#endif
