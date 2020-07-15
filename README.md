# Blynk-Energy-monitor-Tank-Water-Level-Monitor
A Blynk based IOT project that contains one main module with PZEM00T v3 to monitor energy in AC Grid and one sensor module to monitor water level in an overhead Tank. They are connected via Blynk Bridge. 

<h2> Features</h2>
1) Calculates Percentage of water in tank(I used Cylindrical)<br>
2) Automatically turns off Pump connected to relay when Tank is full
3) Wifi signal strength indicator<br>

<h2>Requirements</h2>
1) <a href="https://play.google.com/store/apps/details?id=cc.blynk" target="_blank">Blynk App</a><br>
2) <a href="http://s.click.aliexpress.com/e/nlefJ4PI" target="_blank">NodeMCU</a> <br>
3) <a href="http://s.click.aliexpress.com/e/e6xt0wEy" target="_blank">Ultrasonic sensor (I used HCSRO4)</a><br>
4) <a href="https://robu.in/product/pzem-004t-multi-function-ac-power-monitor-module"> PZEM-004t Energy monitor</a>
<h2> Installation </h2>
Open settings.h and change Bynk Auth code, Wifi settings, server settings and flow calibration.

<h2>Blynk App </h2>
<img src="/images/Blynk_app.jpg" width="" height=" ">

<h2>Sensor Module Connection </h2>
<img src="/images/nodemcu-to-ultrasonic-sensor-connection.png" width="400" height=" " alt="NodeMCU to Ultrasonic Sensor Connection" title="NodeMCU to Ultrasonic Sensor Connection">

