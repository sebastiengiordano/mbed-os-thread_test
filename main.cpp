/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"

Thread thread;

Timer t;
EventQueue* queue = mbed_event_queue();

// Since we're printing from multiple threads, we need a mutex
Mutex print_lock;

// Parameters
uint16_t osPriorityLow_sleep_timer =          100;
uint16_t osPriorityBelowNormal_sleep_timer =  200;
uint16_t osPriorityNormal1_sleep_timer =      300;
uint16_t osPriorityNormal6_sleep_timer =      400;
uint16_t osPriorityAboveNormal_sleep_timer =  500;
uint16_t osPriorityHigh3_sleep_timer =        600;
uint16_t osPriorityHigh5_sleep_timer =        700;
uint16_t osPriorityRealtime_sleep_timer =     800;
uint16_t osPriorityRealtime7_sleep_timer =   1000;
uint16_t wait_timer =                         500;
uint16_t queue_timer =                        100;


void queue_call(void){
        print_lock.lock();
        printf(
            "\t %s \t id: %d \t queue call at %u ms.\n",
            ThisThread::get_name(),
            (int)ThisThread::get_id(),
            (unsigned int)chrono::duration_cast<chrono::milliseconds>(t.elapsed_time()).count());
        print_lock.unlock();
}

void handler(uint16_t* sleep_for_timer)
{
    // Safe to print
    print_lock.lock();
    printf("  Starting %s \t id: %d \t sleep_for: %d ms\t at %u ms.\n",
            ThisThread::get_name(),
            (int)ThisThread::get_id(),
            *sleep_for_timer,
            (unsigned int)chrono::duration_cast<chrono::milliseconds>(t.elapsed_time()).count());
    print_lock.unlock();

    while (true) {
        print_lock.lock();
        printf(
            "\t %s \t id: %d \t sleep_for: %d ms\t at %u ms.\n",
            ThisThread::get_name(),
            (int)ThisThread::get_id(),
            *sleep_for_timer,
            (unsigned int)chrono::duration_cast<chrono::milliseconds>(t.elapsed_time()).count());
        print_lock.unlock();
        ThisThread::sleep_for(chrono::milliseconds(*sleep_for_timer));


        print_lock.lock();
        printf(
            "\t %s \t id: %d \t wait_for: %d ms\t at %u ms.\n",
            ThisThread::get_name(),
            (int)ThisThread::get_id(),
            wait_timer,
            (unsigned int)chrono::duration_cast<chrono::milliseconds>(t.elapsed_time()).count());
        print_lock.unlock();
        wait_us(wait_timer * 1000);


        print_lock.lock();
        printf(
            "\t %s \t id: %d \t queue call in: %d ms\t at %u ms.\n",
            ThisThread::get_name(),
            (int)ThisThread::get_id(),
            queue_timer,
            (unsigned int)chrono::duration_cast<chrono::milliseconds>(t.elapsed_time()).count());
        print_lock.unlock();

        // queue->call_in(chrono::milliseconds(queue_timer), queue_call);
    }
}


int main()
{
    printf( "\n\n"
            "\t\t********\n"
            "\t\t* Main *\n"
            "\t\t********\n"
            "\n"
    );
    wait_us(1 * 1000);

    t.start();

    /******************/
    /* Initialization */
    /******************/
    Thread *ThreadPriorityLow = new Thread(osPriorityLow, 512, NULL, "PriorityLow        ");
    Thread *ThreadPriorityBelowNormal = new Thread(osPriorityBelowNormal, 512, NULL, "PriorityBelowNormal");
    Thread *ThreadPriorityNormal1 = new Thread(osPriorityNormal1, 512, NULL, "PriorityNormal1    ");
    Thread *ThreadPriorityNormal6 = new Thread(osPriorityNormal6, 512, NULL, "PriorityNormal6    ");
    Thread *ThreadPriorityAboveNormal = new Thread(osPriorityAboveNormal, 512, NULL, "PriorityAboveNormal");
    Thread *ThreadPriorityHigh3 = new Thread(osPriorityHigh3, 512, NULL, "PriorityHigh3      ");
    Thread *ThreadPriorityHigh5 = new Thread(osPriorityHigh5, 512, NULL, "PriorityHigh5      ");
    Thread *ThreadPriorityRealtime = new Thread(osPriorityRealtime, 512, NULL, "PriorityRealtime   ");
    Thread *ThreadPriorityRealtime7 = new Thread(osPriorityRealtime7, 512, NULL, "PriorityRealtime7  ");


    /*****************/
    /* Threads start */
    /*****************/

    // Start the user thread
    ThreadPriorityLow->start(callback(handler, &osPriorityLow_sleep_timer));
    // wait_us(0.01);
    // ThreadPriorityBelowNormal->start(callback(handler, &osPriorityBelowNormal_sleep_timer));
    // wait_us(0.01);
    // ThreadPriorityNormal1->start(callback(handler, &osPriorityNormal1_sleep_timer));
    // wait_us(0.01);
    ThreadPriorityNormal6->start(callback(handler, &osPriorityNormal6_sleep_timer));
    // wait_us(0.01);
    // ThreadPriorityAboveNormal->start(callback(handler, &osPriorityAboveNormal_sleep_timer));
    // wait_us(0.01);
    // ThreadPriorityHigh3->start(callback(handler, &osPriorityHigh3_sleep_timer));
    // wait_us(0.01);
    // ThreadPriorityHigh5->start(callback(handler, &osPriorityHigh5_sleep_timer));
    // wait_us(0.01);
    ThreadPriorityRealtime->start(callback(&queue, &EventQueue::dispatch_forever));
    // ThreadPriorityRealtime->start(callback(handler, &osPriorityRealtime_sleep_timer));
    // wait_us(0.01);
    // ThreadPriorityRealtime7->start(callback(&queue, &EventQueue::dispatch_forever));
    ThreadPriorityRealtime7->start(callback(handler, &osPriorityRealtime7_sleep_timer));
    // wait_us(0.01);


    ThisThread::sleep_for(5s);
    // Wait for the thread to terminate
    thread.join();
}
