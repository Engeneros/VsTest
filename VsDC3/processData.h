#ifndef PROCESSDATA_H_INCLUDED
#define PROCESSDATA_H_INCLUDED

double Mean(float * data, unsigned int Nstart, unsigned int Nstop);
double Rms(double mean, float * data,  unsigned int Nstart,  unsigned int Nstop);
#endif // PROCESSDATA_H_INCLUDED
