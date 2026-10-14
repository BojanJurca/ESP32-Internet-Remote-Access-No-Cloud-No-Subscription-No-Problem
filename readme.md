# ESP32 Internet Remote Access: No Cloud, No Subscription, No Problem

There are many ways to control an ESP32 remotely. This tutorial focuses on a solution that provides good functionality while keeping the required technologies reasonably simple.

The final result is a web application running directly on the ESP32. A smartphone connects to the ESP32 through a web browser and can control its outputs. In this example, the built-in LED is used, but the same approach can easily be applied to relays, lights, blinds, sensors, and other devices.

## Skills Required

- Basic C++ programming
- Basic understanding of networking concepts
- Basic HTML knowledge
- Ability to configure a home WiFi router

## Things Needed

### ESP32 Board

An ESP32 development board with WiFi and onboard flash memory.

Examples:

- ESP32-WROOM
- ESP32-S2
- ESP32-S3
- ESP32-C2
- ESP32-C3
- ESP32-C6

Most ESP32 boards include sufficient flash memory to store both firmware and web content.

In this tutorial, the ESP32:

- controls the built-in LED,
- serves a web application,
- provides a REST API,
- runs an FTP server for file uploads.

### Arduino IDE

Download and install Arduino IDE:

https://support.arduino.cc/hc/en-us/articles/360019833020-Download-and-install-Arduino-IDE

### ESP32 Multitasking Network Suite

Several web server libraries are available for ESP32, including:

- WiFiServer
- ESPAsyncWebServer
- ESP32 Multitasking Network Suite

This tutorial uses ESP32 Multitasking Network Suite because it provides:

- HTTP server
- HTTPS server
- FTP server
- additional networking services

The library is available through Arduino Library Manager or on GitHub:

https://github.com/BojanJurca/Multitasking-Http-Ftp-Telnet-Ntp-Smtp-Servers-and-clients-for-ESP32-Arduino-Library

### Home WiFi Router

A standard home WiFi router with DHCP enabled.

### Smartphone or Computer

Any modern web browser should work: Edge, Chrome, Safari, Firefox, ...

### OpenSSL

Required only for the HTTPS/Internet use case.

https://openssl-library.org/source/

---

# Use Case 1: Local WiFi Network

**Control your home from the comfort of your favorite chair. Because walking to the light switch is so 20th century.**

This is the simplest setup. The smartphone and ESP32 are connected to the same local WiFi network.

We'll create:

- Application icons
- An HTML page
- An Arduino sketch
- A smartphone shortcut
- Fixed IP address for ESP32

##  App Icons

Modern browsers allow a web page to be added to a smartphone's home screen.

Prepare:

- `appIcon192x192.png` (192 × 192 pixels, for Android)
- `appIcon180x180.png` (180 × 180 pixels, for iOS)

These icons will be displayed when the page is installed as a shortcut.

## Create index.html

The ESP32 HTTP server serves static files from its web root directory.

Copy `index.html`, `appIcon192x192.png`, and `appIcon180x180.png` to: /var/www/html dirctory on your ESP32.

The example page provides:

- LED control
- REST API communication


```html
<!DOCTYPE html>
<html lang='en'>
	<head>
        <meta http-equiv='content-type' content='text/html; charset=utf-8' />            
		<meta name='viewport' content='width=device-width, initial-scale=1.0'>

        <link rel='shortcut icon' type='image/x-icon' sizes='192x192' href='/appIcon192x192.png'>
        <link rel='icon' type='image/png' sizes='192x192' href='/appIcon192x192.png'>
        <link rel='apple-touch-icon' sizes='180x180' href='/appIcon180x180.png'>

		<title>Control ESP32</title>

		<script type='text/javascript'>

			// -------------------------------------------------------------
			// HTTP REQUEST HELPER
			// -------------------------------------------------------------
			async function httpRequest(url, method = 'GET') {
				try {
					const response = await fetch(url, { method });
					if (!response.ok) {
						throw new Error(`Server error: ${response.status}`);
					}
					return await response.text();
				} catch (error) {
					throw new Error(`Network error: ${error.message}`);
				}
			}

			// -------------------------------------------------------------
			// LED CONTROL VIA ESP32 REST API
			// -------------------------------------------------------------

			// Query the ESP32 for the current LED state and update the UI.
			async function getLED() {
				try {
					const json = await httpRequest('/builtInLed');
					const obj = JSON.parse(json);
					ledSwitch.disabled = false;
					ledSwitch.checked = (obj.builtInLed === 'on');
				} catch (_) {
                    // Error already reported by httpRequest()
                }
			}

			// Send the requested LED state to the ESP32 and synchronize
			// the switch position with the state reported by the server.
			async function turnLED(On) {
				try {
					const json = await httpRequest(On ? '/builtInLed/on' : '/builtInLed/off', 'PUT');
					const obj = JSON.parse(json);
					ledSwitch.disabled = false;
					ledSwitch.checked = (obj.builtInLed === 'on');
				} catch (_) {
					// Error already reported by httpRequest()
				}
			}

			// Synchronize the switch state with the ESP32 when the page loads.
			getLED();

		</script>

	</head>

	<body>

		<div class='container'>
			<h1>Control ESP32</h1>
			
			<p>Built-in LED</p>

			<label class='switch' for='ledSwitch'><input type='checkbox' disabled id='ledSwitch' onClick='turnLED (this.checked)'><div class='slider round'></div></label>
		</div>

	</body>
</html>
```

### Optional CSS Styling

Default browser controls may not provide the desired appearance. If you don't want to see the default checkbox square ☑ and would prefer a toggle switch 
![switch](switch.png)
instead, add custom CSS to your HTML page.

Custom CSS can improve:

- control layout
- switch appearance
- typography
- overall user experience


```css
		<style>

			/* Background */
			body {
				font-family: 'Segoe UI', Tahoma, Geneva, Verdana, sans-serif;
				background-color: #f4f7f6;
				color: #333333;
				display: flex;
				justify-content: center;
				align-items: center;
				height: 100vh;
				margin: 0;
			}

			/* Card */
			.container {
				background-color: #ffffff;
				padding: 30px;
				border-radius: 12px;
				box-shadow: 0 4px 15px rgba(0, 0, 0, 0.05);
				text-align: center;
				max-width: 350px;
				width: 100%;
			}

			/* Heading */
			h1 {
				color: #1a365d;
				font-size: 24px;
				margin-bottom: 20px;
			}

			/* Text */
			p {
				font-size: 16px;
				color: #4a5568;
				margin-bottom: 15px;
			}

			/* Switch */
			.switch {
				position: relative;
				display: inline-block;
				width: 86px;
				height: 46px
			}

			.switch input {
				display: none;
			}

			/* switch off */
			.slider {
				position: absolute;
				cursor: pointer;
				top: 0;
				left: 0;
				right: 0;
				bottom: 0;
				background-color: #cbd5e0;
				transition: .4s
			}

			/* switch bullet */
			.slider:before {
				position: absolute;
				content: '';
				height: 38px;
				width: 38px;
				left: 4px;
				bottom: 4px;
				background-color: white;
				transition: .4s
			}

			/* switch on */
			input:checked+.slider {
				background-color: #1a365d;
			}
			input:focus+.slider {
				box-shadow: 0 0 1px #1a365d;
			}

			input:checked+.slider:before {
				transform: translateX(40px)
			}

			/* switch disabled */
			input:disabled + .slider {
				background-color: #718096 !important;
				cursor: not-allowed;
				opacity: 1;
			}

			/* Round edges */
			.slider.round {
				border-radius: 30px;
			}
			.slider.round:before {
				border-radius: 50%;
			}
		</style>
```


## Create the Arduino Sketch

The ESP32 firmware:

- connects to WiFi,
- mounts the file system,
- starts HTTP and FTP servers,
- implements a simple REST API.

The REST API exposes:

```text
GET  /builtInLed
PUT  /builtInLed/on
PUT  /builtInLed/off
```


```cpp
#include <WiFi.h>

#include <LittleFS.h>           // Select file system; LittleFS.h or SPIFFS.h or FFat.h or SD.h ...
#include <threadSafeFS.h>       // Thread-safe wrapper around the underlying file system
                                // because native FS implementations are generally not task-safe
#include <httpServer.h>         // Embedded HTTP server used for REST API requests
                                // and serving static web content
#include <ftpServer.h>          // FTP server used for uploading files to the web root 
                                // directory /var/www/html

// define the GPIO that built-in LED uses (different GPIO for different ESP32 boards)
#undef LED_BUILTIN
#define LED_BUILTIN 2


// Implement a simple REST API for controlling the built-in LED
String httpRequestHandlerCallback(const char *httpRequest, httpServer_t::httpConnection_t *hcn) {

    // Must be reentrant !!!

    #define httpRequestIs(X)(strstr(httpRequest,X)==httpRequest)

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

  static httpServer_t httpsServer(tsfs, httpRequestHandlerCallback);    // Start HTTP server
  static ftpServer_t ftpServer(tsfs);                                   // Start FTP server

  // Wait until a valid IP address is assigned by the WiFi network
  while(WiFi.localIP() == IPAddress(0, 0, 0, 0)) {
      delay(1000);
      Serial.println("   .");
  }
  Serial.print("Use this IP addess to connect to ESP32: "); Serial.println(WiFi.localIP());

  pinMode(LED_BUILTIN, OUTPUT); // Configure the built-in LED pin as an output
}

void loop() {

}
```


## Upload and Test

1. Create a project folder containing the sketch.
2. Ensure the sketch filename matches the folder name.
3. Replace: YOUR_SSID and YOUR_PASSWORD with your WiFi credentials.
4. Compile and uppoad the sketch.
5. Open Serial Monitor.
6. Note the IP address assigned to the ESP32.
7. Upload the web files to: /var/www/html on your ESP32 using FTP.
8. Open the ESP32 IP address in a browser, for example: **http://192.168.1.xxx**
9. Add the page to your smartphone's home screen.

## Assign a Permanent IP Address

By default, DHCP may assign a different IP address every time the ESP32 reconnects.

Since the smartphone shortcut stores a specific address, this may break the connection.

There are two common solutions.

### Option 1: DHCP Reservation

Configure your router to always assign the same IP address to the ESP32.

Look for settings such as:

- DHCP Reservation
- Address Reservation
- Static Lease

The exact procedure depends on the router model.

### Option 2: Static IP Address

Configure the ESP32 to use a fixed IP address.

This approach works equally well but is outside the scope of this tutorial.

### Option 3: mDNS

Multicast DNS (mDNS) provides name resolution for devices on a local network, similar to how DNS resolves domain names on the Internet. Although the underlying mechanisms are fundamentally different, the result is similar: you can access your ESP32 by name without knowing its IP address.

mDNS works well within a local network, but it cannot be extended to support Use Case 2 (Internet access). Therefore, it is not covered in this tutorial.

---

# Use Case 2: Internet Access

**Because controlling your blinds from the couch is nice. Controlling them while sitting on a beach is even better.**

The local-network solution works well, but exposing the ESP32 to the Internet introduces additional requirements.

We must solve three problems:

- Make the ESP32 reachable from the Internet.
- Restrict access to authorized users.
- Encrypt network traffic.

Plain HTTP is no longer appropriate for Internet-facing devices because all traffic is transmitted unencrypted.

The solution is:
- HTTPS
- Authentication
- Router configuration

## Making the ESP32 reachable from the Internet

The ESP32 is located behind a router using private IP addressing.

Two things must be configured.

### Obtain a Permanent Public Address

Ask your Internet Service Provider (ISP) for one of the following:

#### Static Public IP Address

Example:

```text
193.77.159.208
```

#### Dynamic DNS (DDNS)

Example:

```text
jurca.dyn.ts.si
```

A DDNS hostname allows your device to remain reachable even if the public IP address changes.

### Configure Port Forwarding

The router must forward incoming HTTPS traffic to the ESP32.

Typical configuration:

```text
External Port: 443
Internal Port: 443
Target IP: ESP32 local address
```

Port 443 is the standard HTTPS port and is generally preferred.

### Configure NAT Loopback (Hairpin NAT)

NAT Loopback allows the same public hostname to be used:

- from inside your home network,
- from the Internet.

Without NAT Loopback, you may need different addresses depending on where you connect from.

Not all routers support this feature.

---

# Switching from HTTP to HTTPS

The library provides HTTP and HTTPS server implementations with a very similar programming interface.

In many cases changing: httpServer to: httpsServer is sufficient.

However, HTTPS additionally requires:

- WolfSSL
- a server certificate
- a server private key

## Install WolfSSL

WolfSSL can be installed through Arduino Library Manager or from GitHub:

https://github.com/wolfSSL/wolfssl

WolfSSL is highly configurable, making it suitable for embedded systems with limited resources.

## Recommended WolfSSL Configuration

This tutorial uses:

- TLS 1.3
- ECC cryptography
- AES-GCM

The example configuration has been successfully tested on:

- ESP32-WROOM
- ESP32-S2

both as:

- HTTPS server
- HTTPS client


```cpp
#ifndef WOLFSSL_USER_SETTINGS_H
#define WOLFSSL_USER_SETTINGS_H


/* --------------------------------------------------------------------------
   DEBUGGING
   -------------------------------------------------------------------------- */

// #define DEBUG_WOLFSSL
// Besides #defining DEBUG_WOLFSSL the wolfSSL_Debugging_ON (); function needs to be called 


/* ==========================================================================
   UNIVERSAL WolfSSL user_settings.h for ESP32-WROOM and ESP32-S2
   Compatible in BOTH directions (client ↔ server)
   ECC-only (secp256r1), TLS 1.2 + TLS 1.3, AES-GCM, no RSA.
   Stable handshake, no -125, no -313.
   ========================================================================== */

/* --------------------------------------------------------------------------
   PLATFORM
   -------------------------------------------------------------------------- */
#define WOLFSSL_ARDUINO
#define WOLFSSL_ESP32
#define FREERTOS


/* Disable hardware crypto everywhere (WROOM and S2 do not support it) */
#define NO_WOLFSSL_ESP32_CRYPT_HASH


/* --------------------------------------------------------------------------
   TLS VERSIONS
   -------------------------------------------------------------------------- */
#define WOLFSSL_TLS13
#define NO_OLD_TLS
/* DO NOT disable TLS 1.2 — ESP32-S2 needs fallback */


/* --------------------------------------------------------------------------
   TLS EXTENSIONS (REQUIRED for TLS 1.3)
   -------------------------------------------------------------------------- */
#define HAVE_TLS_EXTENSIONS
#define HAVE_SUPPORTED_CURVES
#define HAVE_HKDF
// #define HAVE_SNI        /* allow SNI from browsers */


/* --------------------------------------------------------------------------
   ECC CONFIGURATION
   -------------------------------------------------------------------------- */
#define HAVE_ECC
#define ECC_USER_CURVES
#undef NO_ECC256
#define HAVE_ECDH
#define HAVE_ECC256
#define ECC_SHAMIR
#define ECC_TIMING_RESISTANT


/* --------------------------------------------------------------------------
   SYMMETRIC CRYPTO
   -------------------------------------------------------------------------- */
#define HAVE_AESGCM
#define HAVE_HMAC
/* SHA256 required */


/* --------------------------------------------------------------------------
   RSA DISABLED (ECC-only)
   -------------------------------------------------------------------------- */
#define NO_RSA


/* --------------------------------------------------------------------------
   MEMORY CONFIGURATION
   -------------------------------------------------------------------------- */
#define USE_FAST_MATH
#define TFM_TIMING_RESISTANT

/* TLS 1.3 handshake requires >2 KB record size */
#undef RECORD_SIZE
#define RECORD_SIZE 4096
#define WOLFSSL_MAX_FRAGMENT_SIZE 4096


/* ECC big integer limit — required for DER parsing */
#undef FP_MAX_BITS
#define FP_MAX_BITS 4096


/* Static memory disabled */
// #define WOLFSSL_STATIC_MEMORY


/* --------------------------------------------------------------------------
   OPTIONAL MEMORY SAVING (SAFE)
   -------------------------------------------------------------------------- */
#define NO_SESSION_CACHE
#define NO_PSK
#define NO_MD4
#define NO_PWDBASED
#define NO_ERROR_STRINGS   /* optional */


/* --------------------------------------------------------------------------
   UNSAFE / INCOMPATIBLE FEATURES (MUST BE DISABLED)
   -------------------------------------------------------------------------- */
#undef SINGLE_THREADED
#undef WOLFSSL_DYNAMIC_BUFFERS
#undef ALT_ECC_SIZE


/* --------------------------------------------------------------------------
   END
   -------------------------------------------------------------------------- */

#endif /* WOLFSSL_USER_SETTINGS_H */
```


## ECC Cryptography

This tutorial uses the NIST P-256 elliptic curve: **prime256v1** also known as **secp256r1**.

This provides strong security while keeping CPU and memory requirements within the capabilities of ESP32 devices.

## Generating Certificates

To keep costs low, we'll create our own certificate authority (CA) and sign the server certificate ourselves.

The generated certificates use **ECC P-256 (prime256v1)**

The process generates:

- Root CA certificate
- Server certificate
- Server private key

The example below generates certificates for **jurca.dyn.ts.si**.


```cmd
@echo off
setlocal EnableExtensions

rem ==============================================================
rem Generate an EC root CA and TLS server certificate for:
rem   jurca.dyn.ts.si
rem Requires OpenSSL 1.1.1 or 3.x.
rem Run this CMD in an empty working directory.
rem ==============================================================

set "HOST=jurca.dyn.ts.si"
set "COUNTRY=SI"
set "STATE=Slovenia"
set "CITY=Ljubljana"
set "ORG=ESP32"
set "CA_CN=ESP32-CA"
set "CA_DAYS=3650"
set "SERVER_DAYS=397"

set OPENSSL="C:\Program Files\OpenSSL-Win64\bin\openssl"


REM ===========================================================================
REM 1) Generate CA key + self-signed CA certificate (CA:TRUE)
REM ===========================================================================

%OPENSSL% ecparam -name prime256v1 -genkey -noout -out ca-key.pem
if errorlevel 1 goto :fail

%OPENSSL% req -x509 -new -sha256 -key ca-key.pem -days %CA_DAYS% ^
  -subj "/C=%COUNTRY%/ST=%STATE%/L=%CITY%/O=%ORG%/CN=%CA_CN%" ^
  -addext "basicConstraints=critical,CA:TRUE,pathlen:0" ^
  -addext "keyUsage=critical,keyCertSign,cRLSign" ^
  -addext "subjectKeyIdentifier=hash" ^
  -out ca-cert.pem
if errorlevel 1 goto :fail

REM ===========================================================================
REM DER-encoded .crt is convenient for installing on Android and iOS.
REM ===========================================================================

%OPENSSL% x509 -in ca-cert.pem -outform DER -out ca-cert.crt
if errorlevel 1 goto :fail


REM ===========================================================================
REM 2) Create the server private key and certificate request.
REM ===========================================================================

%OPENSSL% ecparam -name prime256v1 -genkey -noout -out server-key.pem
if errorlevel 1 goto :fail

%OPENSSL% req -new -sha256 -key server-key.pem ^
  -subj "/C=%COUNTRY%/ST=%STATE%/L=%CITY%/O=%ORG%/CN=%HOST%" ^
  -out server.csr
if errorlevel 1 goto :fail

REM ===========================================================================
REM Extensions required by modern Android/iOS browsers.
REM ===========================================================================

>server-ext.cnf echo basicConstraints=critical,CA:FALSE
>>server-ext.cnf echo keyUsage=critical,digitalSignature,keyAgreement
>>server-ext.cnf echo extendedKeyUsage=serverAuth
>>server-ext.cnf echo subjectAltName=DNS:%HOST%
>>server-ext.cnf echo subjectKeyIdentifier=hash
>>server-ext.cnf echo authorityKeyIdentifier=keyid,issuer

%OPENSSL% x509 -req -sha256 -in server.csr -CA ca-cert.pem -CAkey ca-key.pem ^
  -CAcreateserial -days %SERVER_DAYS% -extfile server-ext.cnf ^
  -out server-cert.pem
if errorlevel 1 goto :fail

REM ===========================================================================
REM DER certificate and unencrypted PKCS#8 DER private key.
REM ===========================================================================

%OPENSSL% x509 -in server-cert.pem -outform DER -out server-cert.der
if errorlevel 1 goto :fail

%OPENSSL% pkcs8 -topk8 -nocrypt -in server-key.pem -outform DER -out server-key.der
if errorlevel 1 goto :fail

REM ===========================================================================
REM Verify signature, hostname, SAN and key pairing.
REM ===========================================================================

%OPENSSL% verify -CAfile ca-cert.pem server-cert.pem
if errorlevel 1 goto :fail

%OPENSSL% verify -CAfile ca-cert.pem -verify_hostname %HOST% server-cert.pem
if errorlevel 1 goto :fail

%OPENSSL% x509 -in server-cert.pem -noout -subject -issuer -dates -ext subjectAltName -ext extendedKeyUsage
if errorlevel 1 goto :fail

%OPENSSL% pkey -in server-key.pem -pubout -out server-public.pem
if errorlevel 1 goto :fail

%OPENSSL% x509 -in server-cert.pem -pubkey -noout -out cert-public.pem
if errorlevel 1 goto :fail

fc /b server-public.pem cert-public.pem >nul
if errorlevel 1 (
  echo ERROR: The private key does not match the server certificate.
  goto :fail
)

del /q server.csr server-ext.cnf server-public.pem cert-public.pem 2>nul

echo.
echo SUCCESS. Generated:
echo   ca-cert.pem       Root CA certificate in PEM format (for certificate veritication) - keep offline and secret
echo   ca-cert.crt       Root CA certificate in DER format (for certificate veritication) - install it on the client Android or iOS phone
echo   ca-key.pem        Root CA private key 											  - keep offline and secret
echo   server-cert.pem   Server certificate in PEM format
echo   server-cert.der   Server certificate in DER format                                 - copy it to /etc/ssl/certs on the SSL server
echo   server-key.pem    Server private key in PEM/SEC1 format
echo   server-key.der    Server private key in unencrypted PKCS#8 DER format              - copy it to /etc/ssl/certs on the SSL server
echo.
echo Install only ca-cert.crt on Android/iOS. Never install or copy ca-key.pem.
echo Configure the server with server-cert.der and server-key.der.
exit /b 0

:fail
echo.
echo ERROR: Certificate generation failed.
exit /b 1
```


## Certificate Installation

Copy server key and certificate to the ESP32's /etc/ssl/certs folder:

```text
server-cert.der
server-key.der
```


To eliminate browser security warnings like 
![Not safe](not_safe.png)
and display a trusted HTTPS connection, also install the CA certificate:

```text
ca-cert.crt
```

on:

- Android
- iOS
- another /etc/ssl/ca-trust folder if you want to use it as HTTPS client


Do not distribute:

```text
ca-key.pem
```

The CA private key must remain secret.

## Login to ESP32

We still don't want just anyone on the Internet to access the ESP32.

Instead of implementing a full login/logout mechanism, we'll simplify things for the purpose of this tutorial. The user must first access the ESP32 from the local network using its public IP address or dynamic DNS hostname.

The HTTPS server determines whether the request originates from the local network by checking the client's IP address. If the request is considered local, the server creates a cookie that the browser can later use to identify itself when connecting from anywhere on the Internet.

Please niote that this is just a simplified authentication scheme. In a real-world application, a proper authentication mechanism should be used. IP address verification is shown here for educational purposes only.

A better web login-logout example can be found here:

https://github.com/BojanJurca/Multitasking-Http-Ftp-Telnet-Ntp-Smtp-Servers-and-clients-for-ESP32-Arduino-Library/tree/main/examples/HTTPS_server

An UNIX-like user management can be found here:

https://github.com/BojanJurca/Multitasking-Http-Ftp-Telnet-Ntp-Smtp-Servers-and-clients-for-ESP32-Arduino-Library/tree/main/examples/Telnet_server/Unix_like_user_management


## Knowing the Current Time Is Important

Accurate time is essential when using certificates and cookies because the browser must be able to determine whether they are valid or have already expired.

To keep the system clock synchronized, the ESP32 uses an NTP client to obtain the current time from one or more NTP servers.

## Create new Arduino Sketch that uses HTTPS server

```cpp
#include <WiFi.h>

#include <LittleFS.h>           // Select file system; LittleFS.h or SPIFFS.h or FFat.h or SD.h ...
#include <threadSafeFS.h>       // Thread-safe wrapper around the underlying file system
                                // because native FS implementations are generally not task-safe

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


    // Check if HTTP request carries the "secretKey" cookie 
    if (hcn->getHttpRequestCookie("secretKey") != "qjRb6suCU48sdl2r") {

        // If the browser accesses the ESP32 through the router's
        // public IP address or DDNS hostname from the home network,
        // many routers performing NAT loopback (hairpin NAT) will
        // present their own LAN address as the client address.
        // In this case the server issues a persistent authentication cookie.
        // Please note that not all routers support Loopback/Hairpin NAT functionality
        if (strcmp(hcn->getClientIP(), WiFi.gatewayIP().toString().c_str()) == 0) {
            hcn->setHttpReplyCookie("secretKey", "qjRb6suCU48sdl2r", time (NULL) + 7776000, "/", true,  true,   "Lax");
                          // cookie Name         Value               Expire in 90 days      Path Secure HttpOnly SameSite
            // continue processing the request
        } else {
            // Inform the browser that access is forbidden
            hcn->setHttpReplyStatus("403");
            return "Forbidden";
        }
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

  static httpServer_t httpsServer(tsfs, httpRequestHandlerCallback);    // Start HTTPS server
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
```

---

# Final Result

After completing these steps:

- The ESP32 hosts its own web application.
- A smartphone can control it directly.
- HTTPS provides encrypted communication.
- The same solution works both locally and over the Internet.

The same architecture can easily be extended to:

- relay control,
- smart lighting,
- blinds,
- heating,
- irrigation,
- sensor monitoring,
- home automation systems.

## Notes

The examples in this tutorial intentionally prioritize simplicity and understandability over advanced production features such as:

- user authentication,
- authorization levels,
- certificate rotation,
- intrusion protection.

These features can be added later using the same architectural foundation.