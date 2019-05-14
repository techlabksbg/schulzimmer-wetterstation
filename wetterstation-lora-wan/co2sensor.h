#ifndef CO2SENSOR_INCLUDED
#define CO2SENSOR_INCLUDED



class CO2Sensor {

  public:

  void begin(int co2tx, int co2rx);
  void begin();
  CO2Sensor();

  unsigned int readCO2UART();

  unsigned int ppmCO2;
  int temp;
  int status = 0;

  void measure();
  void compute();
  void clearSums();
  

  private:
  void clearSerialBuffer();
  unsigned char getCheckSum(unsigned char *packet);
  void enableABC();

  double co2s=0.0;
  int n = 0;

};

#endif

