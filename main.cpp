/* mbed Microcontroller Library
 * Copyright (c) 2019 ARM Limited
 * SPDX-License-Identifier: Apache-2.0
 */

#include "mbed.h"

Thread thread;

Timer current_timer;
EventQueue* queue = mbed_event_queue();
EventQueue* highprio_queue = mbed_highprio_event_queue();

// Since we're printing from multiple threads, we need a mutex
Mutex print_lock;

// Parameters
uint16_t osPriorityLow_sleep_timer =          100;
uint16_t osPriorityBelowNormal_sleep_timer =  200;
uint16_t osPriorityNormal1_sleep_timer =      300;
uint16_t osPriorityNormal6_sleep_timer =      400;
uint16_t osPriorityAboveNormal_sleep_timer = 1500;
uint16_t osPriorityHigh3_sleep_timer =        600;
uint16_t osPriorityHigh5_sleep_timer =        700;
uint16_t osPriorityRealtime_sleep_timer =     800;
uint16_t osPriorityRealtime7_sleep_timer =   1000;
uint16_t sleep_for_timer =                    500;
uint16_t wait_timer =                         500;
uint16_t queue_timer =                        100;

void loop_for(uint16_t delay){
    unsigned int start_time = (unsigned int)chrono::duration_cast<chrono::milliseconds>(current_timer.elapsed_time()).count();
    unsigned int current_time = start_time;
    unsigned int delta_time = 0;

    print_lock.lock();
    printf(
        "\t %s \t id: %x \t loop_for %d ms: start\t at %u ms.\n",
        ThisThread::get_name(),
        (int)ThisThread::get_id(),
        delay,
        (unsigned int)chrono::duration_cast<chrono::milliseconds>(current_timer.elapsed_time()).count());
    print_lock.unlock();

    while(delta_time < delay){
        current_time = (unsigned int)chrono::duration_cast<chrono::milliseconds>(current_timer.elapsed_time()).count();
        delta_time = current_time - start_time;
    }

    print_lock.lock();
    printf(
        "\t %s \t id: %x \t loop_for %d ms: end\t at %u ms.\n",
        ThisThread::get_name(),
        (int)ThisThread::get_id(),
        delay,
        (unsigned int)chrono::duration_cast<chrono::milliseconds>(current_timer.elapsed_time()).count());
    print_lock.unlock();
}

void queue_call(void){
    print_lock.lock();
    printf(
        "\t %s \t id: %x \t queue call at %u ms.\n",
        ThisThread::get_name(),
        (int)ThisThread::get_id(),
        (unsigned int)chrono::duration_cast<chrono::milliseconds>(current_timer.elapsed_time()).count());
    print_lock.unlock();

    print_lock.lock();
    printf(
        "\t %s \t id: %x \t wait_for: %d ms\t at %u ms.\n",
        ThisThread::get_name(),
        (int)ThisThread::get_id(),
        500,
        (unsigned int)chrono::duration_cast<chrono::milliseconds>(current_timer.elapsed_time()).count());
    print_lock.unlock();
    wait_us(500 * 1000);

    print_lock.lock();
    printf(
        "\t %s \t id: %x \t wait_for: end\t at %u ms.\n",
        ThisThread::get_name(),
        (int)ThisThread::get_id(),
        (unsigned int)chrono::duration_cast<chrono::milliseconds>(current_timer.elapsed_time()).count());
    print_lock.unlock();
}

void highprio_queue_call(void){

    print_lock.lock();
    printf(
        "\t %s \t id: %x \t highprio_queue call at %u ms.\n",
        ThisThread::get_name(),
        (int)ThisThread::get_id(),
        (unsigned int)chrono::duration_cast<chrono::milliseconds>(current_timer.elapsed_time()).count());
    print_lock.unlock();

    // print_lock.lock();
    // printf(
    //     "\t %s \t id: %x \t wait_for: %d ms\t at %u ms.\n",
    //     ThisThread::get_name(),
    //     (int)ThisThread::get_id(),
    //     500,
    //     (unsigned int)chrono::duration_cast<chrono::milliseconds>(current_timer.elapsed_time()).count());
    // print_lock.unlock();
    // wait_us(500 * 1000);

    // print_lock.lock();
    // printf(
    //     "\t %s \t id: %x \t wait_for: end\t at %u ms.\n",
    //     ThisThread::get_name(),
    //     (int)ThisThread::get_id(),
    //     (unsigned int)chrono::duration_cast<chrono::milliseconds>(current_timer.elapsed_time()).count());
    // print_lock.unlock();

    loop_for(1000);
}

void handler(uint16_t* wait_for_timer)
{
    // Safe to print
    print_lock.lock();
    printf("Starting %s \t id: %x \t at %u ms.\n",
            ThisThread::get_name(),
            (int)ThisThread::get_id(),
            (unsigned int)chrono::duration_cast<chrono::milliseconds>(current_timer.elapsed_time()).count());
    print_lock.unlock();

    while (true) {
        print_lock.lock();
        printf(
        "\t %s \t id: %x \t sleep_for: %d ms\t at %u ms.\n",
            ThisThread::get_name(),
            (int)ThisThread::get_id(),
            sleep_for_timer,
            (unsigned int)chrono::duration_cast<chrono::milliseconds>(current_timer.elapsed_time()).count());
        print_lock.unlock();
        ThisThread::sleep_for(chrono::milliseconds(sleep_for_timer));

        print_lock.lock();
        printf(
            "\t %s \t id: %x \t wait_for: %d ms\t at %u ms.\n",
            ThisThread::get_name(),
            (int)ThisThread::get_id(),
            *wait_for_timer,
            (unsigned int)chrono::duration_cast<chrono::milliseconds>(current_timer.elapsed_time()).count());
        print_lock.unlock();
        wait_us(*wait_for_timer * 1000);


        print_lock.lock();
        printf(
            "\t %s \t id: %x \t highprio_queue call in: %d ms\t at %u ms.\n",
            ThisThread::get_name(),
            (int)ThisThread::get_id(),
            queue_timer,
            (unsigned int)chrono::duration_cast<chrono::milliseconds>(current_timer.elapsed_time()).count());
        print_lock.unlock();

        highprio_queue->call_in(chrono::milliseconds(queue_timer), highprio_queue_call);


        print_lock.lock();
        printf(
            "\t %s \t id: %x \t queue call in: %d ms\t at %u ms.\n",
            ThisThread::get_name(),
            (int)ThisThread::get_id(),
            queue_timer,
            (unsigned int)chrono::duration_cast<chrono::milliseconds>(current_timer.elapsed_time()).count());
        print_lock.unlock();

        queue->call_in(chrono::milliseconds(queue_timer), queue_call);


        // print_lock.lock();
        // printf(
        //     "\t %s \t id: %x \t wait_for: %d ms\t at %u ms.\n",
        //     ThisThread::get_name(),
        //     (int)ThisThread::get_id(),
        //     *wait_for_timer,
        //     (unsigned int)chrono::duration_cast<chrono::milliseconds>(current_timer.elapsed_time()).count());
        // print_lock.unlock();
        // wait_us(*wait_for_timer * 1000);


        print_lock.lock();
        printf(
            "\t %s \t id: %x \t wait_for: %d ms\t at %u ms.\n",
            ThisThread::get_name(),
            (int)ThisThread::get_id(),
            *wait_for_timer,
            (unsigned int)chrono::duration_cast<chrono::milliseconds>(current_timer.elapsed_time()).count());
        print_lock.unlock();
        wait_us(*wait_for_timer * 1000);
    }
}

void queue_dispatch_forever(void){
    /************************/
    /* Dispatch queue event */
    /************************/
    print_lock.lock();
    printf(
        "Starting %s \t id: %x \t queue_dispatch_forever at %u ms.\n",
        ThisThread::get_name(),
        (int)ThisThread::get_id(),
        (unsigned int)chrono::duration_cast<chrono::milliseconds>(current_timer.elapsed_time()).count());
    print_lock.unlock();
    queue->dispatch_forever();
}

void highprio_queue_dispatch_forever(void){
    /************************/
    /* Dispatch queue event */
    /************************/
    print_lock.lock();
    printf(
        "Starting %s \t id: %x \t highprio_queue_dispatch_forever at %u ms.\n",
        ThisThread::get_name(),
        (int)ThisThread::get_id(),
        (unsigned int)chrono::duration_cast<chrono::milliseconds>(current_timer.elapsed_time()).count());
    print_lock.unlock();
    highprio_queue->dispatch_forever();
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

    current_timer.start();

    /******************/
    /* Initialization */
    /******************/
    // Thread *ThreadPriorityLow = new Thread(osPriorityLow, 512, NULL, "PriorityLow        ");
    // Thread *ThreadPriorityBelowNormal = new Thread(osPriorityBelowNormal, 512, NULL, "PriorityBelowNormal");
    // Thread *ThreadPriorityNormal1 = new Thread(osPriorityNormal1, 512, NULL, "PriorityNormal1    ");
    // Thread *ThreadPriorityNormal6 = new Thread(osPriorityNormal6, 512, NULL, "PriorityNormal6    ");
    Thread *ThreadPriorityAboveNormal = new Thread(osPriorityAboveNormal, 512, NULL, "PriorityAboveNormal");
    Thread *ThreadPriorityHigh3 = new Thread(osPriorityHigh3, 512, NULL, "PriorityHigh3      ");
    // Thread *ThreadPriorityHigh5 = new Thread(osPriorityHigh5, 512, NULL, "PriorityHigh5      ");
    Thread *ThreadPriorityRealtime = new Thread(osPriorityRealtime, 512, NULL, "PriorityRealtime   ");
    Thread *ThreadPriorityRealtime7 = new Thread(osPriorityRealtime7, 512, NULL, "PriorityRealtime7  ");


    /*****************/
    /* Threads start */
    /*****************/

    // ThreadPriorityLow->start(callback(handler, &osPriorityLow_sleep_timer));
    // wait_us(1000);

    // ThreadPriorityBelowNormal->start(callback(handler, &osPriorityBelowNormal_sleep_timer));
    // wait_us(1000);

    // ThreadPriorityNormal1->start(callback(handler, &osPriorityNormal1_sleep_timer));
    // wait_us(1000);

    // ThreadPriorityNormal6->start(callback(handler, &osPriorityNormal6_sleep_timer));
    // wait_us(1000);

    ThreadPriorityAboveNormal->start(callback(handler, &osPriorityAboveNormal_sleep_timer));
    wait_us(1000);

    // ThreadPriorityHigh3->start(callback(handler, &osPriorityHigh3_sleep_timer));
    ThreadPriorityHigh3->start(callback(highprio_queue_dispatch_forever));
    wait_us(1000);

    // ThreadPriorityHigh5->start(callback(handler, &osPriorityHigh5_sleep_timer));
    // wait_us(1000);

    ThreadPriorityRealtime->start(callback(queue_dispatch_forever));
    // ThreadPriorityRealtime->start(callback(handler, &osPriorityRealtime_sleep_timer));
    wait_us(1000);

    // ThreadPriorityRealtime7->start(callback(&queue, &EventQueue::dispatch_forever));
    ThreadPriorityRealtime7->start(callback(handler, &osPriorityRealtime7_sleep_timer));
    // wait_us(1000);




    ThisThread::sleep_for(5s);
    // Wait for the thread to terminate
    thread.join();
}
