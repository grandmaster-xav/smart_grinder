#include <GrinderController.h>

GrinderController::GrinderController()
{
	Serial.println("GrinderController init");
	// init function pointers
	// this->gv.push_back(&GrinderController::start_motor);
	// this->gv.push_back(&GrinderController::stop_motor);
	// this->gv.push_back(&GrinderController::set_timer);

	// init class state
	this->on = false;
	this->timer = 10000; // 10s
	pinMode(GRINDER_PIN_EXT, OUTPUT);
	digitalWrite(GRINDER_PIN_EXT, HIGH);
  	
	configTime(0, 3600, "pool.ntp.org");

	Serial.print("Default grinder timer: ");
	Serial.println(this->timer);
}

unsigned long GrinderController::seconds_to_millis(unsigned int seconds)
{
	return (unsigned long)(seconds * 1000);
}

bool GrinderController::check_time(struct tm timeinfo)
{
	char time_hour[3];
  	char time_day[10];
	strftime(time_hour,3, "%H", &timeinfo);
  	strftime(time_day,10, "%A", &timeinfo);

	int hour = String(time_hour).toInt();

	Serial.println("Day: ");
	Serial.print(time_day);
	Serial.print(" | time: ");
	Serial.println(hour);
	if ((hour < 8 && hour > 18) || time_day == "Saturday" || time_day == "Sunday")
		return (false);
	return (true);
}

void GrinderController::start_motor()
{
	struct tm timeinfo;
	if (!getLocalTime(&timeinfo) && check_time(timeinfo))
	{
		Serial.println("Failed to obtain time or out of business day");
		return;
	}

	this->on = true;
	// create end time allow less calculation while running
	this->start_time = millis();
	digitalWrite(GRINDER_PIN_EXT, HIGH);
	Serial.print("Start grinder for: ");
	Serial.print(this->timer);
	Serial.println(" milliseconds.");
}

void GrinderController::stop_motor()
{
	this->on = false;
	this->start_time = 0;
	digitalWrite(GRINDER_PIN_EXT, LOW);
	Serial.println("Stop grinder");
}

void GrinderController::set_timer(unsigned int seconds)
{
	this->timer = this->seconds_to_millis(seconds);
	Serial.print("Grinder Timer set to: ");
	Serial.print(this->timer);
	Serial.println(" ms.");
}

void GrinderController::run()
{
	uint32_t tick_delay = pdMS_TO_TICKS(500);

	if (millis() - this->start_time > this->timer || this->on == false)
		this->stop_motor();

	if (this->on == false)
		// 2s sleep for this task as an idle when the motor is off
		vTaskDelay(tick_delay * 4);
	vTaskDelay(tick_delay);
}

void handle_queue_commands(QueueHandle_t g_xQueue)
{
	unsigned int timer = 0;
	BaseType_t queue_signal;
	struct GrinderState grinder_state;
	GrinderController grinder_controller;

	while (1)
	{
		// Receive update from `command_task` with pointer to grinder_state
		if (g_xQueue != NULL)
		{
			queue_signal = xQueueReceive(g_xQueue,
										 &grinder_state,
										 (TickType_t)0);
			if (queue_signal == pdPASS)
			{
				// set timer when a signal arrived.
				timer = grinder_state.seconds;
				if (grinder_state.status == 2)
					timer = 1;
				grinder_controller.set_timer(timer);

				// set the grinder state
				if (grinder_state.status == 1 || grinder_state.status == 2)
					grinder_controller.start_motor();
				else
					grinder_controller.stop_motor();
			}
			grinder_controller.run();
		}
	}
}
