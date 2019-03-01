/*
 * POSIX Timer demo code.
 * Demonstrates the use of the 1 shot POSIX timer expiry calling thread_handler. 
 * Also, if uncommented as described below, will demonstrate interval timing feature too.
 *
 * Based on code from https://riptutorial.com/posix/example/16306/posix-timer-with-sigev-thread-notification
 */
#include <stdio.h>  /* for puts() */
#include <string.h> /* for memset() */
#include <unistd.h> /* for sleep() */
#include <stdlib.h> /* for EXIT_SUCCESS */

#include <signal.h> /* for `struct sigevent` and SIGEV_THREAD */
#include <time.h>   /* for timer_create(), `struct itimerspec`,
                     * timer_t and CLOCK_REALTIME 
                     */

static timer_t timerid;
struct itimerspec trigger;

void thread_handler(union sigval sv) {
        char *s = sv.sival_ptr;
		  struct timespec thTimeSpec;	

		  /* print out the posix clock to see that time has incremented
		   * by 2 seconds.
			*/
		  clock_gettime(CLOCK_REALTIME, &thTimeSpec);
		  printf("Clock_getttime: %ld - ",thTimeSpec.tv_sec);

        /* Will print "2 seconds elapsed." from stored sv data */
        puts(s);

}

int main(void) {
        char info[] = "2 seconds elapsed.";
        struct sigevent sev;
		  struct timespec mainTimeSpec;	

		  printf("Hi: Process ID: %d\n",getpid());

        /* Set all `sev` and `trigger` memory to 0 */
        memset(&sev, 0, sizeof(struct sigevent));
        memset(&trigger, 0, sizeof(struct itimerspec));

        /* 
         * Set the notification method as SIGEV_THREAD:
         *
         * Upon timer expiration, `sigev_notify_function` (thread_handler()),
         * will be invoked as if it were the start function of a new thread.
         *
         */
        sev.sigev_notify = SIGEV_THREAD;
        sev.sigev_notify_function = &thread_handler;
        sev.sigev_value.sival_ptr = &info;

        /* Create the timer. In this example, CLOCK_REALTIME is used as the
         * clock, meaning that we're using a system-wide real-time clock for
         * this timer.
         */
        timer_create(CLOCK_REALTIME, &sev, &timerid);

        /* Timer expiration will occur withing 2 seconds after being armed
         * by timer_settime(). Then the interval timer will takeover 
         */
        trigger.it_value.tv_sec = 2;

		  /* Uncomment the following line to set the interval timer and
			* and see the threadhandler() execute periodically.
		   */
        /* trigger.it_interval.tv_sec = 2;*/

        /* Arm the timer. No flags are set and no old_value will be retrieved.
         */
        timer_settime(timerid, 0, &trigger, NULL);

		  clock_gettime(CLOCK_REALTIME, &mainTimeSpec);
		  printf("Clock_getttime: %ld (initial) waiting for 2 seconds\n",mainTimeSpec.tv_sec);
        /* Wait 21 seconds under the main thread. In 2 seconds (when the
         * timer expires), a message will be printed to the standard output
         * by the newly created notification thread handler.
         */
        sleep(21);

        /* Delete (destroy) the timer */
        timer_delete(timerid);

		  printf("Bye: Process ID: %d\n",getpid());
        return EXIT_SUCCESS;
}
