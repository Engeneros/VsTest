#include "math.h"
#include "processData.h"

double Mean(float * data, unsigned int Nstart,  unsigned int Nstop){
double mean=0.0;
 unsigned int N;
N = Nstop-Nstart;
data += Nstart;
for(int ii=0;ii<N;ii++){
  mean += *data++;
}
mean /= N;
return mean;
}

double Rms(double mean, float * data,  unsigned int Nstart,  unsigned int Nstop){
  double rms=0.0;
   unsigned int N;
  N = Nstop-Nstart;
  data += Nstart;
  for(int ii=0;ii<N;ii++){
    volatile double tmp = (*data++ - mean);
    rms += tmp*tmp;
  }
  rms /= N;
  rms = sqrt(rms);
  return rms;
}
