#ifndef SECRETS_H
#define SECRETS_H

// Ajuste com as credenciais reais da rede Wi-Fi.
constexpr char WIFI_SSID[] = "Wokwi-GUEST";
constexpr char WIFI_PASSWORD[] = "";

// Endpoint HTTP que receberá os dados em JSON.
constexpr char API_URL[] = "http://192.168.0.100:3000/api/telemetry";

// Identificador lógico do dispositivo no backend.
constexpr char DEVICE_ID[] = "esp32-lab-001";

#endif
