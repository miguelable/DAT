/**
 * @file secrets.h
 * @author Miguel Ferrer
 * @brief Credenciales secretas y configuraciones de seguridad.
 * @version 0.1
 * @date 2025-01-14
 *
 * Este archivo contiene credenciales secretas y configuraciones de seguridad que no deben compartirse públicamente.
 * Incluye credenciales de WiFi, configuraciones del servidor NTP y certificados del servidor para comunicación segura.
 *
 * @copyright Copyright (c) 2025
 *
 */
#ifndef SECRETS_H
#define SECRETS_H

// NTP
const char* const ntpServer     = "pool.ntp.org"; /*!< Dirección del servidor NTP. */
const long        gmtOffset_sec = 3600;           /*!< Desfase GMT en segundos. */

// Credenciales de WiFi
const char* const ssid     = "GL-MT300N-V2-0bb"; /*!< SSID de WiFi. */
const char* const password = "goodlife";         /*!< Contraseña de WiFi. */
// const char* ssid     = "Pixel_3021";
// const char* password = "miguelferrer";

// Certificado del servidor
const char* const server_cert = R"EOF(
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
)EOF"; /*!< Certificado del servidor para comunicación segura https */

// Certificado CA para comunicación segura
const char* const ca_cert = R"EOF(
-----BEGIN CERTIFICATE-----
MIIDpzCCAo8CFHhZVxXafcgMu5vPsHLZYBpoj8uqMA0GCSqGSIb3DQEBCwUAMIGP
MQswCQYDVQQGEwJFUzEQMA4GA1UECAwHTmF2YXJyYTERMA8GA1UEBwwIUGFtcGxv
bmExDTALBgNVBAoMBFVQTkExDDAKBgNVBAsMA0RBVDESMBAGA1UEAwwJbG9jYWxo
b3N0MSowKAYJKoZIhvcNAQkBFhtmZXJyZXIuMTIxMjQyQGUudW5hdmFycmEuZXMw
HhcNMjQxMjEyMTcyNTIzWhcNMjUxMjEyMTcyNTIzWjCBjzELMAkGA1UEBhMCRVMx
EDAOBgNVBAgMB05hdmFycmExETAPBgNVBAcMCFBhbXBsb25hMQ0wCwYDVQQKDARV
UE5BMQwwCgYDVQQLDANEQVQxEjAQBgNVBAMMCWxvY2FsaG9zdDEqMCgGCSqGSIb3
DQEJARYbZmVycmVyLjEyMTI0MkBlLnVuYXZhcnJhLmVzMIIBIjANBgkqhkiG9w0B
AQEFAAOCAQ8AMIIBCgKCAQEAtqonOOWPfm8oxMTxRdAkGupXdj7iThpm7hCYCdaC
Uv7787Yv7wO8DMNXYu79xXUd4G7iVjDx+p5RWojBUkU26qWve9gbaIJa9J56cBeP
naEjQ6xRTJq6D7u6TKD8S18XFP/4NEsZuPUp3+cL5K+s/CDeMTF5woDPtEiYg9cz
Cyf7py3N3qQ57MIfxI3aJNf/xA/c+n3Neqo25xGHxoPDVsDvNLr0s2EVWbJXhaIh
NmXtHAs50ftBFJxYHRoZLjr4JoOPFxwh4PIsuZadPrajTNyKb7Q3KLLGGB8Ea/m1
TI+wUxwHx5np45OdwiC9QfZweWmSu/FB/3mgLD51+eojOQIDAQABMA0GCSqGSIb3
DQEBCwUAA4IBAQAtEDgbI77onhbQzyQ1GVpN7K5USky1gBTPZoaywg2GSEZbYmUk
4yJ5J+EimbSDtMEPg5Db+gEVJG/HYBiqsYur7pmEgyR1Z2U0+TZgiVX9GyhomlSg
u7KntSGJrKCLkIzIYa4WmbVF3YcX54PlLLiJqK+u2omZTx355RJllmbl+zh2Q5/i
MEabp2xaIZhU4VdU0X4UpPNNGh+J4yqugRNs1FsIPzwNT6rLf7EGq9HJb4TLzXwh
8nRlkTA5WvARLNR4kHjvlApgkZF+aCgXF3rKys+fJCssoZOB1FJ7dsd1BCeD/0Fi
a6Tzmg6mrfc/Z8KGf6Tjl1R5KFY/FGrqJnRd
-----END CERTIFICATE-----
)EOF"; /*!< Certificado CA para comunicación segura con mqtt */

#endif // SECRETS_H