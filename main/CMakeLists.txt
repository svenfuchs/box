idf_component_register(
  SRCS
    "app.cpp"
    "display.cpp"
    "events.cpp"
    "heating.cpp"
    "helper.cpp"
    "logger.cpp"
    "main.cpp"
    "metrics.cpp"
    "ota.cpp"
    "relay.cpp"
    "sensor.cpp"
    "settings.cpp"
    "sntp.cpp"
    "wifi.cpp"
    "u8g2_hal.cpp"
  INCLUDE_DIRS  ""
  # EMBED_TXTFILES certs/cert.pem
)
