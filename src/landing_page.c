#include <stdio.h>
#include <string.h>
#include "esp_wifi.h"
#include "esp_common.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "lwip/sys.h"
#include "lwip/netdb.h"
#include "lwip/api.h"
#include "c_types.h"

void http_server_netconn_serve(struct netconn *conn)
{
    struct netbuf *inbuf;
    char *buf;
    u16_t buflen;
    const char *html_response =
        "HTTP/1.1 200 OK\r\n"
        "Content-Type: text/html\r\n"
        "Connection: close\r\n"
        "\r\n"
        "<!DOCTYPE HTML>"
        "<html><head><title>Login Page</title></head><body>"
        "<h2>Login</h2>"
        "<form action=\"/login\" method=\"post\">"
        "<label for=\"username\">Username:</label>"
        "<input type=\"text\" id=\"username\" name=\"username\"><br><br>"
        "<label for=\"password\">Password:</label>"
        "<input type=\"password\" id=\"password\" name=\"password\"><br><br>"
        "<input type=\"submit\" value=\"Login\">"
        "</form>"
        "</body></html>";

    if (netconn_recv(conn, &inbuf) == ERR_OK)
    {
        netbuf_data(inbuf, (void **)&buf, &buflen);

        // Respond to any GET request with the HTML page
        if (buflen >= 5 && strncmp(buf, "GET /", 5) == 0)
        {
            netconn_write(conn, html_response, strlen(html_response), NETCONN_COPY);
        }

        netbuf_delete(inbuf);
    }

    netconn_close(conn);
    netconn_delete(conn);
}

void http_server_task(void)
{
    struct netconn *conn, *newconn;
    err_t err;

    conn = netconn_new(NETCONN_TCP);
    netconn_bind(conn, NULL, 80);
    netconn_listen(conn);

    while (1)
    {
        err = netconn_accept(conn, &newconn);
        if (err == ERR_OK)
        {
            http_server_netconn_serve(newconn);
        }
    }

    netconn_close(conn);
    netconn_delete(conn);
}

// void user_init(void)
// {
//     printf("SDK version:%s\n", system_get_sdk_version());
//     wifi_init();
//     xTaskCreate(http_server_task, "http_server", 256, NULL, 2, NULL);
// }
