// 2020
// PROJECT MOVED TO https://github.com/nikoschalikias/firm_n-DAP_MODBUS_FEEDER_ADC
// USING mbed-cli
// --------------------------------------
// 13/12/2022
    // made a new os2 project in keil-studio
    // Updated main.cpp from  this https://ide.mbed.com/compiler/#nav:/n-DAP_MODBUS_FEEDER_ADC;
    // Updated mbed.bld with this content: https://os.mbed.com/users/mbed_official/code/mbed/builds/176b8275d35d
    // Updated  the SDK with CTRL+SHIFT+P: Refresh mbed2 SDK
    // Imported the USBSerial from version 71:53949e6131f6 at https://os.mbed.com/users/mbed_official/code/USBDevice/
    // ...then had a single error
    // I then  removed line 24 from USBHAL.h and 
    // Compiled
    // To be tested if it works
    // printf() over USB works :-)
    // All updates form  main.cpp from: https://github.com/nikoschalikias/firm_n-DAP_MODBUS_FEEDER_ADC/blob/master/main.cpp ,  copied here
    // USB serial works, red LED blinks

//--------------N-DAP BOARD PINOUT----------------------------
// RX       P0.19           P0.22(ADC0)     PRESSURE SENSOR 
// TX       P0.18           P0.8            RC-SERVO
// DIR      P0.2            P0.7            Vacuum
//          VBUS            P0.12(ADC1)
//          3.3V            P0.5(SDA)
//          GND             P0.4(SCL)
//------------------------------------------------------------    

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
DigitalOut dir_RS485(P0_2);
PwmOut rcservo(P0_8);
DigitalOut Vacuum(P0_7);

int main() {
    modbus.baud(9600);
    uint8_t buf[32];
    uint8_t i = 0;
    uint16_t counter = 0;
    uint16_t angle = 0;

    dir_RS485 = 1;
    wait(0.5); // wait the usb to be recognized from the PC
    modbus.printf(" 0x%04u\r",  adc1.read_u16());
    serial.printf("ADC =  0x%04u\n\r",  adc1.read_u16());   //adc readout to PC  
    //serial.printf("HI FERNANDO");   //adc readout to PC  
    dir_RS485 = 0;
    green = 1;
    blue = 1;
    rcservo.period_ms(20);      
    rcservo.pulsewidth_us(1500) ;
       
    while(1) {
        counter ++;
        if (counter > 20000) {
            red = !red;
            counter = 0; 
        }  


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
        if (buf[9] == 0x05) { Vacuum = 1; }
        if (buf[9] == 0x06) { Vacuum = 0; }
        if (buf[9] >  0x07) {
            angle = buf[9]*4;
            angle = 1000 + angle;
            buf[9] = 0;
            rcservo.period_ms(20);      
            rcservo.pulsewidth_us(angle) ;
            }
    }
}

