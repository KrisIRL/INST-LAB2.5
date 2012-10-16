// example to test the mbed Lab Board lcd
// Pot1 change the contrast
// Pot2 change the speed of the sin wave


#include "mbed.h"
#include "rtos.h"
#include "Small_6.h"
#include "Small_7.h"
#include "Arial_9.h"
#include "stdio.h"
#include "C12832_lcd.h"

Serial pc(USBTX, USBRX);


// LCD object
C12832_LCD LCD("LCD");

AnalogIn Pot1(p19);
AnalogIn Pot2(p20);

// defines to make lib thread save
#define _lock lcd_mutex.lock();
#define _unlock lcd_mutex.unlock();
Mutex lcd_mutex;

// print data into first line and wait for 1s
void thread1(void const *args)
{
    int i;
    while(true) {       // thread loop
        _lock
        LCD.locate(0,0);
        LCD.set_font((unsigned char*) Small_6);
        printf("Thread1 count: %d",i);
        LCD.copy_to_lcd();
        _unlock
        i++;
        Thread::wait(1000);
    }
}

// print data into third line and wait for 0,5s
void thread2(void const *args)
{
    int k;
    while(true) {       // thread loop
        _lock
        LCD.locate(0,20);
        LCD.set_font((unsigned char*) Arial_9);
        printf("Thread 2 count : %d",k);
        LCD.copy_to_lcd();
        _unlock
        k++;
        Thread::wait(500);
    }
}


void thread4(void const *args)
{
    int i,k,v;
    double s,a;
    k = 1;
    _lock
    LCD.rect(89,0,127,17,1);
    _unlock
    while(true) {       // thread loop
        _lock
        v = Pot1.read_u16();  // get value of pot 1
        for (i=90; i<127; i++) {           
            s = 8 * sin((long double)(i+k) /5);
            a = 8 * sin((long double)(i+k-1) /5);
            LCD.pixel(i,9 + (int)a ,0);
            LCD.pixel(i,9 + (int)s ,1);
        }
        LCD.copy_to_lcd();
        _unlock
        k++;
        Thread::wait(v/100);
    }
}


void thread5(void const *args)
{
    int k;
    while(true) {         // thread loop
    k = Pot2.read_u16();  // get the value of poti 2
    k = k >> 10;          // we need only 6 bit
    _lock
    LCD.set_contrast(k);
    _unlock
    Thread::wait(500);  
}
}

int main()
{
    int j;

    LCD.claim(stdout);      // send stdout to the LCD display
    LCD.cls();
    //start thread1
    Thread t1(thread1);
    //start thread2
    Thread t2(thread2);
    //start thread4
    Thread t4(thread4);
    Thread t5(thread5);

    while(true) {   // this is the third thread
        _lock
        LCD.locate(0,9);
        LCD.set_font((unsigned char*) Small_7);
        j = LCD.get_contrast();
        printf("contrast : %d",j);
        LCD.copy_to_lcd();
        _unlock
        Thread::wait(500);
    }

}
