#include <common.h>
#include <TelegramHandler.h>

WiFiClientSecure g_WIFIClient;
UniversalTelegramBot g_bot(BOT_TOKEN, g_WIFIClient);

bool handle_commands(String user_text, GrinderState *grinder_state)
{
	if (grinder_state->status >= 3 && user_text != "/maintenance")
	{
		g_bot.sendMessage(CHAT_ID, MAINTENANCE_ERROR);
		return (false);
	}

	if (user_text == "/on")
		grinder_state->status = 1;
	if (user_text == "/add")
		grinder_state->status = 2;
	else if (user_text == "/maintenance" || user_text == "/off")
		grinder_state->status = 0;
	else if (user_text.startsWith("/set_timer", 0))
	{
		String b = user_text.substring(10);
		if (b.length() > 0)
		{
			int b_int = b.toInt();
			// no more than a minute
			if (b_int > 0 && b_int <= 60)
			{
				grinder_state->status = 0;
				grinder_state->seconds = b_int;
			}
			else
			{
				g_bot.sendMessage(CHAT_ID, TIMER_ERROR);
				return (false);
			}
		}
		else
		{
			g_bot.sendMessage(CHAT_ID, TIMER_ERROR);
			return (false);
		}
	}
	else
		return (false);
	return (true);
}

void process_new_messages(int nu_new, GrinderState *grinder_state, QueueHandle_t g_xQueue)
{
	for (int i = 0; i < nu_new; i++)
	{
		String chat_id = String(g_bot.messages[i].chat_id);
		if (chat_id == CHAT_ID)
		{
			String user_text = g_bot.messages[i].text;
			Serial.println(user_text);
			if (user_text == "/start" || user_text == "/help" || user_text == "/commands")
				g_bot.sendMessage(chat_id, WELCOME, "");
			else
			{
				if (handle_commands(user_text, grinder_state))
					// Send grinder_state->update to Task2 if changed
					Serial.println("Command sended");
					xQueueSend(g_xQueue,
							   (void *)grinder_state,
							   (TickType_t)10);
			}
		}
	}
}

void handle_new_messages(QueueHandle_t g_xQueue)
{
	struct GrinderState grinder_state;
	unsigned long last_time_bot = 0;

	// Init grinder_state use in queue
	grinder_state.status = 0;
	grinder_state.seconds = 10;
	g_WIFIClient.setCACert(TELEGRAM_CERTIFICATE_ROOT);

	g_bot.sendMessage(CHAT_ID, WELCOME, "");
	while (1)
	{
		if (millis() > last_time_bot + 1000)
		{
			int nu_new = g_bot.getUpdates(-1);
			while (nu_new)
			{
				process_new_messages(nu_new, &grinder_state, g_xQueue);
				nu_new = g_bot.getUpdates(g_bot.last_message_received + 1);
			}
			last_time_bot = millis();
		}
	}
}
