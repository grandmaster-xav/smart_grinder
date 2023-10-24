#include <WifiServerHandler.h>

WiFiServer connexion_server(80);

void set_creds_nvs(char *ssid, char *password)
{
	set_str_value_nvs("WIFI", "ssid", ssid);
	set_str_value_nvs("WIFI", "password", password);
}

bool get_creds_nvs(char *ssid, char *password)
{
	bool r = true;

	r &= get_str_value_nvs("WIFI", "ssid", ssid, SSID_LENGTH);
	r &= get_str_value_nvs("WIFI", "password", password, SSID_LENGTH);
	return (r);
}

int wifi_connect(char *ssid, char *password)
{
	WiFi.begin(ssid, password);
	Serial.println(ssid);
	Serial.println(password);
	byte connect_count = 0;
	while (WiFi.status() != WL_CONNECTED)
	{
		delay(500);
		Serial.print(".");
		connect_count++;
		if (connect_count > 10)
		{
			Serial.println("Error connection to WiFi");
			return (false);
		}
	}
	Serial.println("WiFi connected");
	return (true);
}

bool get_creds_from_url(String get_url)
{
	// get_url from "http://192.168.4.1/set_over?ssid=SSID&password=PWD"
	int ssid_add = 0;
	int pwd_add = 0;
	int end_add = 0;
	String ssid = "";
	String password = "";

	pwd_add = get_url.indexOf('&');
	ssid = get_url.substring(get_url.indexOf('?') + 6, pwd_add);
	ssid.replace("+", " ");
	password = get_url.substring(pwd_add + 10, get_url.indexOf(' '));
	password.replace("+", " ");
	password.replace("%40", "@");
	set_creds_nvs((char *)ssid.c_str(), (char *)password.c_str());
	Serial.println("credentials:");
	Serial.println((char *)ssid.c_str());
	Serial.println((char *)password.c_str());
	return (wifi_connect((char *)ssid.c_str(), (char *)password.c_str()));
}

int wifi_config_server()
{
	WiFiClient client = connexion_server.available();

	if (client)
	{
		String currentLine = "";
		while (client.connected())
		{
			if (client.available())
			{
				Serial.println("Wifi client available");

				char c = client.read();
				if (c == '\n')
				{
					if (currentLine.length() == 0)
					{
						client.println("HTTP/1.1 200 OK");
						client.println("Content-type:text/html");
						client.println();
						client.println(HOME_HTML);
						break;
					}
					else
						currentLine = "";
				}
				else if (c != '\r')
					currentLine += c;
				if (currentLine.endsWith("GET /set_over"))
				{
					String get_request = "";
					while (1)
					{
						char c_get = client.read();
						Serial.write(c_get);
						if (c_get == '\n')
							break;
						else
							get_request += c_get;
					}
					if (get_creds_from_url(get_request))
					{
						client.println("HTTP/1.1 200 OK");
						client.println("Content-type:text/html");
						client.println();
						client.println(WIFI_SUCCESS_HTML);
						client.stop();
						return (true);
					}
					else
					{
						client.println("HTTP/1.1 200 OK");
						client.println("Content-type:text/html");
						client.println();
						client.println(WIFI_FAIL_HTML);
						currentLine = "";
						break;
					}
				}
			}
			Serial.print(".");
		}
		client.stop();
	}
	return (false);
}

void ap_init()
{
	// WiFi.softAP(ssid, password);
	WiFi.softAP("ALEX_XAVOU_SMART_GRINDER");
	IPAddress myIP = WiFi.softAPIP();
	Serial.print("ALEX_XAVOU_SMART_GRINDER IP address: ");
	Serial.println(myIP);
	connexion_server.begin();
}

void wifi_handler()
{
	char ssid[SSID_LENGTH];
	char password[SSID_LENGTH];
	esp_err_t init_nvs();

	Serial.println("Connecting to wifi....");
	if (!get_creds_nvs(ssid, password) || !wifi_connect(ssid, password))
	{
		ap_init();
		while (!wifi_config_server())
			;
	}
	connexion_server.end();
	Serial.println("Connect WIFI SUCCESS");
}
