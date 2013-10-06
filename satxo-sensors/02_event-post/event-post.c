/**
 * \file
 *         Event posting between processes example application.
 * \author
 *         Clément Burin des Roziers <clement.burin-des-roziers@inria.fr>
 */

#include "contiki.h"
#include "dev/leds.h"

#include <stdio.h> /* For printf() */

/* Driver Include */
#include "dev/temperature-sensor.h"
#include "dev/pwr_ctrl.h"

/* Variables: the application specific event value */
static process_event_t event_data_ready;

/*---------------------------------------------------------------------------*/
/* We declare the two processes */
PROCESS(temp_process, "Temperature process");
PROCESS(print_process, "Print process");

/* We require the processes to be started automatically */
AUTOSTART_PROCESSES(&temp_process, &print_process);
/*---------------------------------------------------------------------------*/
/* Implementation of the first process */
PROCESS_THREAD(temp_process, ev, data)
{
    // variables are declared static to ensure their values are kept
    // between kernel calls.
    static struct etimer timer;
    static int count = 0;
    static int average, valid_measure;
    
    // those 3 variables are recomputed at every run, therefore it is not
    // necessary to declare them static.
    int tmp75_t_raw;
    int measure;
    
    // any process mustt start with this.
    PROCESS_BEGIN();
    
    /* allocate the required event */
    event_data_ready = process_alloc_event();
    
    /* Initialize the temperature sensor */
    pwr_ctrl_init();
  
    /* turn on sensors power */
    pwr_ctrl(1);

    SENSORS_ACTIVATE(temperature_sensor);
    
    average = 0;
    
    // set the etimer module to generate an event in one second.
    etimer_set(&timer, CLOCK_SECOND);
    
    while (1)
    {
        // wait here for the timer to expire
        PROCESS_WAIT_EVENT_UNTIL(ev == PROCESS_EVENT_TIMER);
        
        leds_toggle(LEDS_YELLOW);
        
        // do the process work
        // TMP75
        tmp75_t_raw = temperature_sensor.value(0);
        measure =  (int)(tmp75_t_raw >> 4);

        average += measure;
        count ++;
        
        if (count == 4)
        {
            // average the sum and store
            valid_measure = average >> 2;
            
            // reset variables
            average = 0;
            count = 0;
            
            // post an event to the print process
            // and pass a pointer to the last measure as data
            process_post(&print_process, event_data_ready, &valid_measure);
        }
        
        // reset the timer so it will generate another event
        etimer_reset(&timer);
    }
    // any process must end with this, even if it is never reached.
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
/* Implementation of the second process */
PROCESS_THREAD(print_process, ev, data)
{
    PROCESS_BEGIN();
    
    while (1)
    {
        // wait until we get a data_ready event
        PROCESS_WAIT_EVENT_UNTIL(ev == event_data_ready);
        
        // display it
        printf("temperature = %u.%u\n", (*(int*)data)>>2, ((*(int*)data)&0x3)*25);
        
    }
    PROCESS_END();
}
/*---------------------------------------------------------------------------*/
