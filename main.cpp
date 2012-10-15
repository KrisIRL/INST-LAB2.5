// example to test the mbed Lab Board lcd

#include "mbed.h"
#include "rtos.h"
#include "Small_6.h"
#include "Small_7.h"
#include "Arial_9.h"
#include "stdio.h"


Serial pc(USBTX, USBRX);

#include "C12832_lcd.h"

// LCD object
C12832_LCD LCD("LCD");



// print data into first line and wait for 1s
void thread1(void const *args)
{
    int i;
    while(true) {       // thread loop
        _lock
        LCD.locate(0,0);
        LCD.set_font((unsigned char*) Small_6);
        printf("Thread 1 count : %d",i);
        LCD.copy_to_lcd();
        _unlock
        i++;
        Thread::wait(1000);
    }
}

// print data into third line and wait for 0,5s
void thread2(void const *args)
{
    int i;
    while(true) {       // thread loop
        _lock
        LCD.locate(0,20);
        LCD.set_font((unsigned char*) Arial_9);
        printf("Thread 2 count : %d",i);
        LCD.copy_to_lcd();
        _unlock
        i++;
        Thread::wait(500);
    }
}




int main()
{
    int j;
    pc.printf("Test LCD \n\r");

    LCD.set_orientation(1);
    LCD.claim(stdout);      // send stdout to the LCD display
 
    // start thread1
    Thread t1(thread1);
    // start thread2
    Thread t2(thread2);

    while(true) {   // this is the third thread
        _lock
        LCD.locate(0,10);
        LCD.set_font((unsigned char*) Small_7);
        printf("Thread3 count : %d",j);
        LCD.copy_to_lcd();
        _unlock
        j++;
        Thread::wait(1500);
    }
   
}
