#include <common.h>
#include <GrinderController.h>
#include <TelegramHandler.h>
#include <WifiServerHandler.h>

QueueHandle_t g_xQueue;
TaskHandle_t g_GrinderTask;
TaskHandle_t g_TelegramTask;

void command_task(void *p)
{
	/*
	Task that handle all communications from Telegram and UART
	The task will analyze the commands and
	will add to the queue 'g_xQueue' the update grinder state
	*/
	Serial.print("command_task running on core: ");
	Serial.println(xPortGetCoreID());
	handle_new_messages(g_xQueue);
	vTaskDelete(NULL);
}

void grinder_task(void *p)
{
	/*
	Task that handle all the grinder hardware following the grinder state
	retrieved in the queue 'g_xQueue'
	*/
	Serial.print("grinder_task running on core: ");
	Serial.println(xPortGetCoreID());
	handle_queue_commands(g_xQueue);
	vTaskDelete(NULL);
}

void setup()
{
	/*
	Set the Motor's GPIO to LOW
	Set the wifi
	Launch the two task on each core.
	*/
	Serial.begin(115200);
	digitalWrite(GRINDER_PIN_EXT, HIGH);

	wifi_handler();

	// Launch tasks
	g_xQueue = xQueueCreate(5, sizeof(struct GrinderState));
	if (g_xQueue == NULL)
		Serial.println("Failed to create queue");

	xTaskCreate(
		command_task,
		"command_task",
		5000,
		NULL,
		2,
		&g_TelegramTask);
	xTaskCreate(
		grinder_task,
		"grinder_task",
		50000,
		NULL,
		3,
		&g_GrinderTask);
	Serial.println("Setup done");
}

void loop()
{
	// we got tasks, no need loop
}
