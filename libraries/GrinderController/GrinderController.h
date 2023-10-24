#ifndef _GRINDER_CONTROLLER_H_
# define _GRINDER_CONTROLLER_H_

# include "time.h"
# include <vector>
# include <Arduino.h>
# include <common.h>

# define GRINDER_PIN_EXT 26

class GrinderController
{
	// typedef void (GrinderController::*pfunc_t)();
	// typedef std::vector<pfunc_t> f_func_vec_t;

	private:
		bool on;
		unsigned long timer;
		unsigned long start_time;

		// Utils
		unsigned long seconds_to_millis(unsigned int seconds);
		bool check_time(struct tm timeinfo);

	public:
		// f_func_vec_t gv;
		GrinderController();
		// Setters
		void run();
		void start_motor();
		void stop_motor();
		void set_timer(unsigned int seconds);
};

void handle_queue_commands(QueueHandle_t g_xQueue);

#endif
