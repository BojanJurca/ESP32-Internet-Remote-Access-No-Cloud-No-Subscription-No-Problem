#include <WiFi.h>

#include <LittleFS.h>           // Select file system; LittleFS.h or SPIFFS.h or FFat.h or SD.h ...
#include <threadSafeFS.h>       // Thread-safe wrapper around the underlying file system
                                // because native FS implementations are generally not task-safe

#include <httpServer.h>         // Embedded HTTP server used for REST API requests
                                // and serving static web content
#include <httpsServer.h>        // Embedded HTTPS server used for REST API requests
                                // and serving static web content                                
#include <ftpServer.h>          // FTP server used for uploading files to the web root 
                                // directory /var/www/html
#include <ntpClient.h>          // NTP client used to get current time from internet                                

// define the GPIO that built-in LED uses (different GPIO for different ESP32 boards)
#undef LED_BUILTIN
#define LED_BUILTIN 2


// Implement a simple REST API for controlling the built-in LED
String httpRequestHandlerCallback(const char *httpRequest, httpServer_t::httpConnection_t *hcn) {

    // Must be reentrant !!!

    #define httpRequestIs(X)(strstr(httpRequest,X)==httpRequest)


    // For simplicity, only protect HTTPS access.
    // Plain HTTP access from the local network remains unrestricted.
    if (strcmp (hcn->cipherName (), "None") != 0) {

        // DEBUG:
        Serial.printf ("HTTPS request from %s: %s\n", hcn->getClientIP(), httpRequest);

        // Check if HTTP request carries the "secretKey" cookie 
        if (hcn->getHttpRequestCookie("secretKey") != "qjRb6suCU48sdl2r") {

            // If the browser accesses the ESP32 through the router's
            // public IP address or DDNS hostname from the home network,
            // many routers performing NAT loopback (hairpin NAT) will
            // present their own LAN address as the client address.
            // In this case the server issues a persistent authentication cookie.
            // Please note that not all routers support Loopback/Hairpin NAT functionality
            if (strcmp(hcn->getClientIP(), WiFi.gatewayIP().toString().c_str()) != 0) {
                // Inform the browser that access is forbidden
                hcn->setHttpReplyStatus("403");
                return "Forbidden";
            }
        }
        // refresh secretKey cookie
        hcn->setHttpReplyCookie("secretKey", "qjRb6suCU48sdl2r", time (NULL) + 7776000, "/", true,  true,   "Lax");
                      // cookie Name         Value               Expire in 90 days      Path Secure HttpOnly SameSite
    }


    if(httpRequestIs("GET /builtInLed "))               {   // Return the current LED state
                                                        getBuiltInLedState:
                                                            // Return JSON because it can be consumed directly by browser-based JavaScript
                                                            return "{\"builtInLed\":\"" + String(digitalRead(LED_BUILTIN) ? "on" : "off") + "\"}\r\n";
                                                        }                                                                    
    else if(httpRequestIs("PUT /builtInLed/on "))       {   // Turn the built-in LED on
                                                            digitalWrite(LED_BUILTIN, HIGH); 
                                                            goto getBuiltInLedState;
                                                        }
    else if(httpRequestIs("PUT /builtInLed/off "))      {   // Turn the built-in LED off
                                                            digitalWrite(LED_BUILTIN, LOW);  
                                                            goto getBuiltInLedState;
                                                        }

    // Request not handled by the REST API. Let the HTTP server process it normally (serve files, return 404, ...)
    return "";
}

void setup() {
  Serial.begin(115200);

  tsfs.begin(true);  // Mount the selected file system through the thread-safe wrapper

  WiFi.begin("YOUR_SSID", "YOUR_PASSWORD");  // Connect to WiFi

  static httpServer_t httpServer(tsfs, httpRequestHandlerCallback);     // Start HTTP server
  static httpsServer_t httpsServer(tsfs, httpRequestHandlerCallback);   // Start HTTPS server
  static ftpServer_t ftpServer(tsfs);                                   // Start FTP server

  // Wait until a valid IP address is assigned by the WiFi network
  while(WiFi.localIP() == IPAddress(0, 0, 0, 0)) {
      delay(1000);
      Serial.println("   .");
  }
  Serial.print("Use this IP addess to connect to ESP32: "); Serial.println(WiFi.localIP());
  // Get current time from NTP servers
  ntpClient_t ntpClient ("1.si.pool.ntp.org", "2.si.pool.ntp.org", "3.si.pool.ntp.org");
  ntpClient.syncTime ();

  pinMode(LED_BUILTIN, OUTPUT); // Configure the built-in LED pin as an output
}

void loop() {

}
