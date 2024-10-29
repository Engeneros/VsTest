#include "vsdcbase.h"

vsdcBase::vsdcBase()
{

}

VsDCinRam::VsDCinRam() : vsdcBase()
{

}

 void VsDCinRam::pstartAll(void){}
 void VsDCinRam::pstart(uint32_t ch){}
 void VsDCinRam::pstopAll(void){}
 void VsDCinRam::pstop(uint32_t ch){}
 void VsDCinRam::calibAll(void){}
 void VsDCinRam::calib(uint32_t ch){}
 float VsDCinRam::readIntegral(uint32_t ch)
 {
     return (ch+0.1)*0.5;
 }
 void VsDCinRam::setInputMux(){}
 float VsDCinRam::getTimeQuant()
 {
     return (3.2e-6);
 }
 float VsDCinRam::getPrTimerQuant()
 {
     return (1.0e-8);
 }
 void VsDCinRam::setTimer(uint32_t count){}
 void VsDCinRam::setTimer(uint32_t count, uint32_t ch){}
 void VsDCinRam::setPrecisionTimer(uint32_t count){}
 void VsDCinRam::setPrecisionTimer(uint32_t count, uint32_t ch){}
 uint32_t VsDCinRam::chanCount(void){return 2;}
 uint32_t VsDCinRam::GetUid(){}
