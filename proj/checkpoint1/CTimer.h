//provide a timer 
// SetTimer() set the timer_interval.
//starttimer() enable timer
//canceltimer() stop the time

#include<pthread.h>
#include<sys/time.h>

class CTimer
{
	private:
		pthread_t thread _timer;
		long m_second;
}
