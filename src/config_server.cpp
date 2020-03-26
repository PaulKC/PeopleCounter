#include "config_server.h"

void ConfigServer::handleRequest()
{
    if (server.hasArg("maxPersons") && server.method()==HTTP_POST)
    {
        int newMaxPeople = server.arg("maxPersons").toInt();
        maxPeople = newMaxPeople;
        callback(newMaxPeople);
    }
    String path = server.uri();
    char index_html[600];
    sprintf(index_html,INDEX_HTML,maxPeople);
    server.send(200, "text/html", index_html);
}

ConfigServer::ConfigServer(int initialMaxPeople,std::function<void(int)> maxPersionsChangedCallback) : apIp(192, 168, 4, 22), subnet(255, 255, 255, 0), server(80)
{   
    maxPeople = initialMaxPeople;
    callback = maxPersionsChangedCallback;
    server.onNotFound(std::bind(&ConfigServer::handleRequest, this));
}

void ConfigServer::handleClient()
{
    dnsServer.processNextRequest();
    server.handleClient();
}

void ConfigServer::startServer()
{
    WiFi.mode(WIFI_AP);
    WiFi.softAPConfig(apIp, apIp, subnet);
    dnsServer.start(53, "*", apIp);
    Serial.println((WiFi.softAP("PeopleCounter", "password")) ? "AP created" : "AP creation failed");
    server.begin();
}

void ConfigServer::stopServer()
{
    server.stop();
    dnsServer.stop();
    Serial.println((WiFi.softAPdisconnect(true)) ? "AP disconnected" : "AP disconnect failed");
}

void ConfigServer::setMaxPeople(int newMaxPeople)
{
    maxPeople = newMaxPeople;
}