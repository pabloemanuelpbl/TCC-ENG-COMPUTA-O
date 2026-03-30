#include "http_sender.h"

#include <HTTPClient.h>
#include <WiFi.h>

HttpSender::HttpSender(const char* ssid, const char* password, const char* endpoint)
    : ssid_(ssid), password_(password), endpoint_(endpoint) {}

void HttpSender::begin() {
  // Inicia conexão Wi-Fi no boot.
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid_, password_);
}

bool HttpSender::ensureWifiConnected() {
  if (WiFi.status() == WL_CONNECTED) {
    return true;
  }

  // Tenta reconectar em caso de queda de rede.
  WiFi.disconnect();
  WiFi.begin(ssid_, password_);

  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 10000) {
    delay(250);
  }

  return WiFi.status() == WL_CONNECTED;
}

bool HttpSender::postTelemetry(const TelemetryPayload& payload) {
  if (!ensureWifiConnected()) {
    Serial.println("[HTTP] Wi-Fi indisponível.");
    return false;
  }

  WiFiClientSecure secureClient;
  secureClient.setInsecure(); // <- corrige erro SSL

  HTTPClient client;
  client.begin(endpoint_);
  client.addHeader("Content-Type", "application/json");

  String json = buildJson(payload);
  int code = client.POST(json);
  Serial.print("[HTTP] POST payload: ");
  Serial.println(json);

  if (code > 0) {
    Serial.print("[HTTP] POST enviado. Status: ");
    Serial.println(code);
  } else {
    Serial.print("[HTTP] Falha no POST. Erro: ");
    Serial.println(client.errorToString(code));
  }

  client.end();
  return code > 0 && code < 300;
}

String HttpSender::buildJson(const TelemetryPayload& payload) const {
  // JSON montado manualmente para reduzir dependências no firmware.
  String json = "{";
  json += "\"device_id\":\"" + payload.deviceId + "\",";
  json += "\"uptimeMs\":" + String(payload.uptimeMs) + ",";
  json += "\"temperature\":" + String(payload.ds18b20TemperatureC, 2) + ",";
  json += "\"temperatureDht22C\":" + String(payload.dht22TemperatureC, 2) + ",";
  json += "\"humidity\":" + String(payload.dht22HumidityPct, 2) + ",";
  json += "\"current\":" + String(payload.simulatedCurrentA, 2);
  json += ",\"created_at\":\"" + String(__DATE__) + " " + String(__TIME__) + "\"";
  json += "}";

  return json;
}
