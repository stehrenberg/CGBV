#ifndef TIMER_H_
#define TIMER_H_

#include <stdint.h>


/**
	 * High performance timer
	 * @author Andreas Klein
	 * @date 06.11.08
	 */
	class  Timer
	{
	public:
		Timer();
		/**
		 * Starts the timer
		 */
		void start();
		/**
		 * Stops the timer
		 */
		float stop();
		/**
		 * Gets the elapsed time in seconds
		 * @return the elapsed time in seconds
		 */
		float getElapsedSeconds();
		/**
		 * Gets the total time since start
		 * @return the total time
		 */
		float getTotalTime();

		float restart();
	private:

		 int64_t   frequency;
		 int64_t startTime;
		 int64_t  lastTime;

		/**
		 * Gets the current time
		 * @return the current tume
		 */
		int64_t getCurrentTime();
	};


#endif
