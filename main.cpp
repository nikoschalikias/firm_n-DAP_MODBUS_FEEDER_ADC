#include "mbed.h"
#include "LPC11Uxx.h"
#include "USBSerial.h"
//#include "Servo.h"
 


DigitalOut red(P0_20);
DigitalOut green(P0_21);
DigitalOut blue(P0_11);

AnalogIn adc1(P0_22); //n-DAP pin 12
USBSerial serial(0x1f00,0x2012,0x0001,false);// serial over native USB on board (false is non blocking)
Serial modbus(P0_19, P0_18); // tx, rx
DigitalOut dir(P0_2);
PwmOut rcservo(P0_8);
DigitalOut Vacuum(P0_7);


int main() {
    modbus.baud(9600);
    uint8_t buf[32];
    uint8_t i = 0;
    uint16_t angle = 0;

    dir = 1;
    wait(4); // wait the usb to be recognized from the PC
    modbus.printf(" 0x%04u\r",  adc1.read_u16());
    serial.printf(" 0x%04u\r",  adc1.read_u16());   //adc readout to PC  
    dir = 0;
    green = 1;
    blue = 1;
            rcservo.period_ms(20);      
            rcservo.pulsewidth_us(1500) ;
       
    while(1) {
        red = !red;           
        if(modbus.readable()) {                                                 
            if (i > 31) {                                
                i = 0;
                }
            buf[i] = modbus.getc();
            if ( buf[i] == 0x20){
                  serial.printf("\n");    //change line and reset index
                  i = 0; buf[i] = 0x20;
                  }
            modbus.putc(buf[i]);                            //debug only
            serial.printf("0x%02X ", buf[i]);
            i++; 
        }
        if (buf[9] == 0x01) { blue = 0;}
        if (buf[9] == 0x02) { blue = 1;}
        if (buf[9] == 0x03) { green = 0; }
        if (buf[9] == 0x04) { green = 1; }
        if (buf[9] == 0x04) { Vacuum = 1; }
        if (buf[9] >  0x05)  {
            angle = buf[9]*4;
            angle = 1000 + angle;
            buf[9] =0;
            rcservo.period_ms(20);      
            rcservo.pulsewidth_us(angle) ;
            }

    }
}
