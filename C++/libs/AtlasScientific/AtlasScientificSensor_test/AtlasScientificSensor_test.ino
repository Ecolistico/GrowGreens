/* Pendiente:
 *  Arreglar funciones: 
 *    changeI2CAddress
 *    
 *  Revisar funciones:
 *    calibration
 */

#include <EZO.h>

EZO phMeter(EZO_PH);
EZO ecMeter(EZO_EC);

int count = 0;

void setup() {
  Serial.begin(115200);

  delay(2000);
  phMeter.init();
  ecMeter.init(); 
}

void loop() {
  phMeter.run();
  ecMeter.run();

  if(phMeter.isEnable() && ecMeter.isEnable()){
    switch(count){
      case 0:
        ecMeter.exportCal();
        break;
      case 1:
        phMeter.exportCal(); 
        break;
      case 2:
        phMeter.importCalibration("0000006646C3");
        break;
      case 3:
        phMeter.importCalibration("6646C3000202");
        break;
      case 4:
        phMeter.importCalibration("52804000E040");
        break;
      case 5:
        phMeter.importCalibration("29204118AD");
        break;
      case 6:
        delay(10000);
        ecMeter.infoStatus();
        phMeter.infoStatus();
        break;
      case 7:
        ecMeter.information();
        phMeter.information();
        break;
      case 8:
        ecMeter.led(0);
        phMeter.led(0);
        break;
      case 9:
        ecMeter.led(2);
        phMeter.led(2);
        break;
      case 10:
        ecMeter.led(1);
        phMeter.led(1);
        break;
      case 11:
        ecMeter.led(2);
        phMeter.led(2);
        break;
      case 12:
        ecMeter.plock(0);
        phMeter.plock(0);
        break;
      case 13:
        ecMeter.plock(2);
        phMeter.plock(2);
        break;
      case 14:
        ecMeter.plock(1);
        phMeter.plock(1);
        break;
      case 15:
        ecMeter.plock(2);
        phMeter.plock(2);
        break;
      case 16:
        ecMeter.read();
        phMeter.read();
        break;
      case 17:
        ecMeter.readWithTempCompensation(20.5);
        phMeter.readWithTempCompensation(20.5);
        break;
      case 18:
        ecMeter.tempCompensation(true,20.5);
        phMeter.tempCompensation(true,20.5);
        break;
      case 19:
        ecMeter.tempCompensation(false,20.5);
        phMeter.tempCompensation(false,20.5);
        break;
      case 20:
        ecMeter.enableParameters(1,0);
        break;
      case 21:
        ecMeter.enableParameters(0,1);
        break;
      case 22:
        ecMeter.enableParameters(0,2);
        break;
      case 23:
        ecMeter.enableParameters(0,3);
        break;
      case 24:
        ecMeter.enableParameters(0,4);
        break;
      case 25:
        phMeter.slope();
        break;
      /* changeI2Caddress has to be fixed it does not working */
      case 26:
        //ecMeter.changeI2Caddress(90);
        //phMeter.changeI2Caddress(91);
        //delay(50000);
        break;
      case 27:
        //ecMeter.changeI2Caddress(EC_I2C_ADDRESS);
        //phMeter.changeI2Caddress(PH_I2C_ADDRESS);
        //delay(5000);
        break;
      case 28:
        ecMeter.setProbe(false, 0);
        break;
      case 29:
        ecMeter.setProbe(true, 0.1);
        break;
      case 30:
        ecMeter.setProbe(false, 0);
        break;
      case 31:
        ecMeter.setProbe(true, 10);
        break;
      case 32:
        ecMeter.setProbe(false, 0);
        break;
      case 33:
        ecMeter.setProbe(true, 1);
        break;
      case 34:
        ecMeter.setProbe(false, 0);
        break;
      case 35:
        ecMeter.reboot();
        phMeter.reboot();
        delay(10000);
        break;
      case 36:
        ecMeter.sleep();
        phMeter.sleep();
        delay(10000);
        break;
      case 37:
        ecMeter.find();
        phMeter.find();
        break;
      default:
        break;
    }
    if(count<38) count++;
  }
}
