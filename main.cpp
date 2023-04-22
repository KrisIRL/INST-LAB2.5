// This is a sample program to demonstrate the use of the mbed Lab Board LCD library with the mbed RTOS.
// The program creates 5 threads that perform different tasks:
// 1. Thread 1: Prints a counter in the first line of the LCD and increments it every 1 second.
// 2. Thread 2: Prints another counter in the third line of the LCD and increments it every 0.5 seconds.
// 3. Thread 3: Displays a sine wave on a small window on the LCD, with the sine wave's speed controlled by Pot1.
// 4. Thread 4: Reads the value of Pot2 and sets the LCD contrast accordingly.
// 5. Thread 5: Reads the state of the joystick and the fire button and sets the corresponding LEDs.
// The program uses mutexes to ensure that the LCD is accessed by only one thread at a time, preventing any conflicts or data corruption.
// Pot1 change the contrast
// Pot2 change the speed of the sin wave

#include "mbed.h"
#include "rtos.h"
#include "Small_6.h"
#include "Small_7.h"
#include "Arial_9.h"
#include "stdio.h"
#include "C12832_lcd.h"

// joystick
BusIn joy(p15,p12,p13,p16);
DigitalIn fire(p14);

BusOut leds(LED1,LED2,LED3,LED4);

// LCD object
C12832_LCD LCD;

AnalogIn Pot1(p19);
AnalogIn Pot2(p20);

// mutex to make the lcd lib thread save
Mutex lcd_mutex;

// Thread 1
// print counter into first line and wait for 1 s
void thread1(void const *args)
{
    int i;
    while(true) {       // thread loop
        lcd_mutex.lock();
        LCD.locate(0,0);
        LCD.set_font((unsigned char*) Small_6);
        LCD.printf("Thread1 count: %d",i);
        lcd_mutex.unlock();
        i++;
        Thread::wait(1000);
    }
}

// Thread 2
// print counter into third line and wait for 0,5s
void thread2(void const *args)
{
    int k;
    while(true) {       // thread loop
        lcd_mutex.lock();
        LCD.locate(0,20);
        LCD.set_font((unsigned char*) Arial_9);
        LCD.printf("Thread 2 count : %d",k);
        lcd_mutex.unlock();
        k++;
        Thread::wait(500); // wait 0.5s
    }
}

// Thread 3
// print a sin function in a small window
// the value of pot 1 change the speed of the sinwave
void thread3(void const *args)
{
    int i,k,v;
    double s,a;
    k = 1;
    lcd_mutex.lock();
    LCD.rect(89,0,127,17,1);
    lcd_mutex.unlock();
    while(true) {       // thread loop
        v = Pot1.read_u16();  // get value of pot 1
        lcd_mutex.lock();
        for (i=90; i<127; i++) {           
            s = 8 * sin((long double)(i+k) /5);   // pixel to print  
            a = 8 * sin((long double)(i+k-1) /5); // old pixel to erase
            LCD.pixel(i,9 + (int)a ,0);           // erase pixel  
            LCD.pixel(i,9 + (int)s ,1);           // print pixel  
        }
        LCD.copy_to_lcd();  // LCD.pixel do not update the lcd 
        lcd_mutex.unlock();
        k++;
        Thread::wait(v/100);   // value of pot1 / 100
    }
}

// Thread 4
// input pot 2 and change the contrast of lcd
void thread4(void const *args)
{
    int k;
    while(true) {         // thread loop
    k = Pot2.read_u16();  // get the value of pot 2
    k = k >> 10;          // we need only 6 bit for contrast
     lcd_mutex.lock();
    LCD.set_contrast(k);
     lcd_mutex.unlock();
    Thread::wait(500);    // wait 0.5s
}
}

// Thread 5
// joystick function
void thread5(void const *args){
       while(1) {
        if (fire) {
            leds=0xf;
        } else {
            leds=joy;
        }
        wait(0.1);
    }
    
    }


// print the actual contrast 
int main(){
    
    
    Thread t5(thread5); //start thread5
    
    int j;
    LCD.cls();
  
    Thread t1(thread1); //start thread1
    Thread t2(thread2); //start thread2
    Thread t3(thread3); //start thread3
    Thread t4(thread4); //start thread4

    while(true) {       // main is the next thread
        lcd_mutex.lock();
        LCD.locate(0,9);
        LCD.set_font((unsigned char*) Small_7);
        j = LCD.get_contrast();    // read the actual contrast
        LCD.printf("contrast : %d",j);
        lcd_mutex.unlock();
        Thread::wait(500);   // wait 0.5s
    }

}
