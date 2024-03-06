# ESP01S/BME280 Weather Station
Simple ESP01S/BME280 weather station designed to provide environment readings every 10 minutes and run for about 6 months on 2 AA batteries.

# Prerequisites
- ESP01S module with [deepsleep mod](https://www.instructables.com/Enable-DeepSleep-on-an-ESP8266-01/).
- [ESP01 breakout board](/img-esp01bo.png) (recommended, not required).
- [BME280 module](/img-bme280.png). In theory, one should be able to use the BMP280 as well, albeit without the humidity component. For that reason, I will refer to the Bosch module as BMx280.
- Web server data can be sent to. Raspberry Pi with lighttpd works well. Data can be parsed using PHP and forwarded onto Weather Underground or other weather reporting services.

# Wiring diagram
- BMx280 SDA to ESP GPIO0
- BMx280 SDC to ESP GPIO2
- BMx280 GND to ESP GND
- BMx280 VCC to ESP TXD

![ESP01S with breakout board and BME280](/img-esp01.jpg)

# Notes
Data sent from the ESP01S module is also providing voltage information, which should be monitored and batteries replaced when V1 voltage starts approaching 2.7V.

# Reference links
- Powering BMx280 through GPIO to prevent various reset/reading issues with the BMx280 modules.
  - https://community.particle.io/t/bme280-sensor-problem/49627/6
  - https://forum.arduino.cc/t/troubleshooting-a-bmp280-sensor-with-wrong-readings/666298/5
