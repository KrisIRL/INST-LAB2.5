// example to test the mbed Lab Board lcd lib with the mbed rtos
// Pot1 change the contrast
// Pot2 change the speed of the sin wave

#include "mbed.h"
#include "rtos.h"
#include "Small_6.h"
#include "Small_7.h"
#include "Arial_9.h"
#include "stdio.h"
#include "C12832_lcd.h"

// LCD object
C12832_LCD LCD("LCD");

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
        printf("Thread1 count: %d",i);
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
        printf("Thread 2 count : %d",k);
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
    k = Pot2.read_u16();  // get the value of poti 2
    k = k >> 10;          // we need only 6 bit for contrast
     lcd_mutex.lock();
    LCD.set_contrast(k);
     lcd_mutex.unlock();
    Thread::wait(500);    // wait 0.5s
}
}


// print the actual contrast 
int main()
{
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
        printf("contrast : %d",j);
        lcd_mutex.unlock();
        Thread::wait(500);   // wait 0.5s
    }

}
