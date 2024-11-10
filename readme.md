### ESP8266 0.96inch OLED MQTT display

This is a MQTT client app to display text.It is for a module sold on eBay: "ESP8266 0.96inch OLED Clock Weather PM2.5 Calendar ESP-12F Development Board"

Accepted payload looks like this:
[{"c":"C"},{"c":"F","v":2},{"c":"G","x":0,"y":0},{"c":"P","v":"line 1"},{"c":"P","v":"line"},{"c":"D"}]

C = Clear display
F = Set font size
G = Goto x,y position
P = Print text
D = Update display.

