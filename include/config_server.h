#include <ESP8266WebServer.h>
#include <DNSServer.h>

const char INDEX_HTML[] PROGMEM = R"=====(
<!doctype html>
<html lang="en">
<head>
  <meta charset="utf-8">
  <title>People Counter Configuration</title>
</head>
<body>
  <form method="post">
  <label style="display:block; width:100%; text-align:center;" for="maxPersons">Max Persons:</label><br>
  <div style="width:100px; margin-right:auto; margin-left:auto;text-align: center;">
	<input style="width:100%;" type="number" id="maxPersons" name="maxPersons" value="%d"><br>
	<input style="margin-top:20px;" type="submit" value="Save">
  </div>
  </form>
</body>
</html>
)=====";

class ConfigServer
{

public:
    ConfigServer(int maxPeople,std::function<void(int)> maxPersionsChangedCallback);
    void startServer();
    void stopServer();
    void handleClient();
    void setMaxPeople(int newMaxPeople);
private:
    IPAddress apIp;
    IPAddress subnet;
    ESP8266WebServer server;
    DNSServer dnsServer;
    int maxPeople;
    std::function<void(int)> callback;
    void handleRequest();
};