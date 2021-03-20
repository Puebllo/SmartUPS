#include "machines.h"

WiFiUDP UDP;
boolean udpStarted = false;

void sendWOL(IPAddress addr, WiFiUDP udp, byte *mac, size_t size_of_mac) {
    byte preamble[] = {0xFF, 0xFF, 0xFF, 0xFF, 0xFF, 0xFF};
    byte i;

    udp.beginPacket(addr, 9);  // sending packet at 9,

    udp.write(preamble, sizeof preamble);

    for (i = 0; i < 16; i++) {
        udp.write(mac, size_of_mac);
    }
    udp.endPacket();
}

void turnOn(String serverName) {
    if (!udpStarted) {
        UDP.begin(9);  // start UDP client, not sure if really necessary.
        udpStarted = true;
    }

    managedServer server = getServerByName(serverName);

    Serial.print("server: ");
    Serial.print(server.serverName);
    Serial.print("ip: ");
    Serial.print(server.ipAddress);
    Serial.print("mac: ");
    Serial.println(server.macAddress);

    String ipAddress = "";

    int idx = server.ipAddress.lastIndexOf('.');
    if (idx != -1) {
        ipAddress = server.ipAddress.substring(0, idx);
        ipAddress += ".255";

        IPAddress computer_ip;
        computer_ip.fromString(ipAddress);

        Serial.print("IP address: ");
        Serial.println(computer_ip.toString());

        byte mac[6];

        String ma = server.macAddress;
        ma.replace(":", "");
        const char *maa = ma.c_str();

        // solution from johnwasser
        // https://forum.arduino.cc/index.php?topic=586895.msg3992797#msg3992797
        hexCharacterStringToBytes(mac, maa);

        sendWOL(computer_ip, UDP, mac, sizeof mac);
    }
}