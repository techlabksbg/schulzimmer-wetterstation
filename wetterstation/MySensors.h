#ifndef MYSENSORS_INCLUDED
#define MYSNSORS_INCLUDED



class MySensors {

  public:

  void begin(int co2tx, int co2rx, int sda, int scl);
  void begin();
  MySensors();

  unsigned int readCO2UART();
  void setZeroPoint();

  double temp[3]={0.0, 0.0, 0.0};
  double humidity;
  unsigned int ppmCO2;
  unsigned int approxppmCO2;
  unsigned int ppbTVOC;
  int status = 0;

  void measure();
  void compute();
  

  private:
  void clearSerialBuffer();
  unsigned char getCheckSum(unsigned char *packet);
  void enableABC();

  double temps[3]={0.0, 0.0, 0.0};
  double humids=0.0;
  double co2s=0.0;
  double aco2s=0.0;
  double tvocs=0.0;
  int n = 0;
  void clearSums();

};

#endif

