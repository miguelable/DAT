/**
 * @file secrets.h
 * @author Miguel Ferrer (mferrer@inbiot.es)
 * @brief Secret credentials and configuration settings.
 * @version 0.1
 * @date 2025-01-14
 *
 * This file contains secret credentials and configuration settings that should not be shared publicly.
 * It includes WiFi credentials, NTP server settings, and server certificates for secure communication.
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef SECRETS_H
#define SECRETS_H

// NTP
const char* ntpServer     = "pool.ntp.org"; /*!< NTP server address. */
const long  gmtOffset_sec = 3600;           /*!< GMT offset in seconds. */

// WiFi credentials
const char* ssid     = "GL-MT300N-V2-0bb"; /*!< WiFi SSID. */
const char* password = "goodlife";         /*!< WiFi password. */
// const char* ssid     = "Pixel_3021";
// const char* password = "miguelferrer";

const char* server_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIID9zCCAt+gAwIBAgIUZKPYCVsl3aqE4EZDs/1agNCfFWcwDQYJKoZIhvcNAQEL
BQAwgYoxCzAJBgNVBAYTAkFVMQswCQYDVQQIDAJFUzERMA8GA1UEBwwIUGFtcGxv
bmExEDAOBgNVBAoMB0dydXBvMDMxDDAKBgNVBAsMA0RBVDEPMA0GA1UEAwwGTWln
dWVsMSowKAYJKoZIhvcNAQkBFhtmZXJyZXIuMTIxMjQyQGUudW5hdmFycmEuZXMw
HhcNMjQxMTIwMTczNDI0WhcNMjUxMTIwMTczNDI0WjCBijELMAkGA1UEBhMCQVUx
CzAJBgNVBAgMAkVTMREwDwYDVQQHDAhQYW1wbG9uYTEQMA4GA1UECgwHR3J1cG8w
MzEMMAoGA1UECwwDREFUMQ8wDQYDVQQDDAZNaWd1ZWwxKjAoBgkqhkiG9w0BCQEW
G2ZlcnJlci4xMjEyNDJAZS51bmF2YXJyYS5lczCCASIwDQYJKoZIhvcNAQEBBQAD
ggEPADCCAQoCggEBALyFjQ5SjSVnuOkT7ATSalnqldf7dsFmzPmA4raNJYCnrV5V
kw1pePUrWYUrJIaBeyiL9bl94NOT+NdzDujj/lHiEINiqEApesLYzQ9nDFDS36FC
yJ4kSvg3vDf4qy11J+FNrkZQQM40RePNpEGis3jDfXmJNHplmfiaT+dVC04GRlYh
8ejt+L0Ms2Po6SeePG4qofCr6PtRYeYOtQAFKSMut6ihE4oH6FDSWb4pqhaL3/Gi
8x7I6+2NxCEdgifbJrPcHFXtL4oLk6NJUbxw9Z/+BhaIC760IxjFlF5WfwggwGs3
2IzJxu8DhGkW1Ob4zcj0yVbXDmDfw0KiSl6j7lkCAwEAAaNTMFEwHQYDVR0OBBYE
FJf3xjP6AHuRnNzMmlHL6ZlD6uUQMB8GA1UdIwQYMBaAFJf3xjP6AHuRnNzMmlHL
6ZlD6uUQMA8GA1UdEwEB/wQFMAMBAf8wDQYJKoZIhvcNAQELBQADggEBAGt9T6Ck
XVrrfbo4oMfIqV6KcVvLzowrdGQxft4wKm/MOxC1Jp1griqg8SnMmFupePOxgY0R
Y6MSqGpBrH3Sz435WI+3+mPZunvKJYpqz07gNZA1xh7TgiPn7M14fP0HXPHwUpTb
f/CGQHAPY6MrQ++56Ej4ciHyAP9klPfB16XX7wYmtLpIC8ds+C4msl3TgPRl0SvT
ea75O+JV+uaM5xdA8D7QFaTGsGj8e1Ps+XqclVEsmUT2RYP5cpCka4xVV15zyTYJ
DfsG1AMLXq0xX6D66OzZyjpJygOApCpThPkYz3QEbBF4aJthmZ5vQr67L/A7fPYP
FnAxRrpGfjne7Ks=
-----END CERTIFICATE-----
)EOF"; /*!< Server certificate for secure communication. */

#endif // SECRETS_H