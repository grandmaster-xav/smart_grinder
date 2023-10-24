#ifndef _TELEGRAM_HANDLER_H_
#define _TELEGRAM_HANDLER_H_

#include <WiFiClientSecure.h>
#include <UniversalTelegramBot.h>
#include <ArduinoJson.h>

// Bot Token
#define CHAT_ID "XXX"
#define BOT_TOKEN "XXX"
#define ID_SIZE 10
#define TOKEN_SIZE 46

// Telegram usage
#define WELCOME "Use the following commands to control your grinder.\nSend /on or /off to start\nSend /set_timer [0-60]\nSend /add to run the grinder for 1s"
#define TIMER_ERROR "Error: Timer must be between 0 and 60 seconds"
#define MAINTENANCE_ERROR "Error: Smart Grinder in maintenance. Check the beans and the container then send /maintenance to unlock it."

void handle_new_messages(QueueHandle_t g_xQueue);

#endif
