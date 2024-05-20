

#include "espressif/esp_common.h"
#include "wifi.h"

#define WIFI_SSID "TP-Link_5568"
#define WIFI_PASS "311eqpor!"

void wifi_connect()
{
    struct station_config stationConf;

    wifi_set_opmode(STATION_MODE);
    strcpy((char *)stationConf.ssid, WIFI_SSID);
    strcpy((char *)stationConf.password, WIFI_PASS);
    wifi_station_set_config(&stationConf);
    wifi_station_connect();
}

void check_wifi_status()
{
    STATION_STATUS status = wifi_station_get_connect_status();
    switch (status)
    {
    case STATION_IDLE:
        printf("WiFi Status: IDLE\n");
        break;
    case STATION_CONNECTING:
        printf("WiFi Status: CONNECTING\n");
        break;
    case STATION_WRONG_PASSWORD:
        printf("WiFi Status: WRONG PASSWORD\n");
        break;
    case STATION_NO_AP_FOUND:
        printf("WiFi Status: NO AP FOUND\n");
        break;
    case STATION_CONNECT_FAIL:
        printf("WiFi Status: CONNECT FAIL\n");
        break;
    case STATION_GOT_IP:
        printf("WiFi Status: GOT IP\n");
        break;
    default:
        printf("WiFi Status: UNKNOWN\n");
        break;
    }
}