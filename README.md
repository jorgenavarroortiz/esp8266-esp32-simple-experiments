# Simple experiments with LoRa and LoRaWAN

This repository is intended to collect a number of simple implementations with different nodes that support LoRa or LoRaWAN.

## LoRa

### TTGO-LoRa32-SendReceiver 

From https://github.com/cubapp/LilyGO-TTGO-LoRa32-SenderReceiver, included in the https://github.com/jorgenavarroortiz/lora-lorawan-simple-experiments/tree/main/LoRa/TTGO-LoRa32-SenderReceiver directory. It simply sends a counter and a random number from a LoRa sender to a LoRa receiver, and show the information in the display.

<!--![image](https://github.com/jorgenavarroortiz/lora-lorawan-simple-experiments/assets/17797704/cfa9f4e9-688d-4a51-83df-58e8a91ad4e4)-->
<img src="https://raw.githubusercontent.com/jorgenavarroortiz/lora-lorawan-simple-experiments/main/img/ttgo-lora32-senderreceiver.png" alt="ttgo-lora32-senderreceiver" width="400"/>

### Sending images over LoRa

Private repository at https://github.com/jorgenavarroortiz/LoRa-multimedia. Using Pycom FiPy nodes.

<!--![image](https://github.com/jorgenavarroortiz/lora-lorawan-simple-experiments/assets/17797704/64d1f8d6-a3ee-41d1-a670-0c886bcaa3ed)-->
<img src="https://raw.githubusercontent.com/jorgenavarroortiz/lora-lorawan-simple-experiments/main/img/lora-multimedia.png" alt="lora-multimedia" width="800"/>

## LoRaWAN

### The Things Uno

The [LoRaWAN/TheThingsUno/TTNUnoSendABP](https://github.com/jorgenavarroortiz/lora-lorawan-simple-experiments/tree/main/LoRaWAN/TheThingsUno/TTNUnoSendABP) directory contains a sample code that sends information in CayenneLPP (Low Power Payload) format. It includes the code for a flame sensor detector KY-026.

<!--![image](https://github.com/jorgenavarroortiz/lora-lorawan-simple-experiments/assets/17797704/65772651-8909-4085-bf38-cf3e96396dc0)-->
<img src="https://raw.githubusercontent.com/jorgenavarroortiz/lora-lorawan-simple-experiments/main/img/ttuno-sendabp.png" alt="ttuno-sendabp" width="400"/>

### TTGO T-beam

From [TTGO T-Beam Tracker for The Things Network](https://github.com/kizniche/ttgo-tbeam-ttn-tracker).

The [lora-lorawan-simple-experiments/LoRaWAN/ttgo-tbeam-ttn-tracker-sx1262](https://github.com/jorgenavarroortiz/lora-lorawan-simple-experiments/tree/main/LoRaWAN/ttgo-tbeam-ttn-tracker-sx1262) directory contains a sample code that sends the GPS information to TTN.

Steps to configugre TTN mapper:

1) Add the code to a TTGO T-Beam. The node shall be outdoor to get information from GPS satellites.

<!--![image](https://github.com/jorgenavarroortiz/lora-lorawan-simple-experiments/assets/17797704/a840d753-c488-4848-801b-42fd3819d317)-->
<img src="https://raw.githubusercontent.com/jorgenavarroortiz/lora-lorawan-simple-experiments/main/img/ttgo-tbeam-01.png" alt="ttgo-tbeam-01.png" width="600"/>

2) In TTN, create a new application for this experiment (`wimunet-ttnmapper` in the example). Configure the nodes to use CayenneLPP decoder. You can export the received information in JSON format (`Export as JSON` button).

<!--![image](https://github.com/jorgenavarroortiz/lora-lorawan-simple-experiments/assets/17797704/648ce1a5-e804-4e0d-8980-2e36dbace55c)-->
<img src="https://raw.githubusercontent.com/jorgenavarroortiz/lora-lorawan-simple-experiments/main/img/ttgo-tbeam-02.png" alt="ttgo-tbeam-02" width="800"/>

3) In the TTN application, go to `integrations` -> `webhooks` with webhook format `protocol buffers`, base URL `https://integrations.ttnmapper.org/tts/v3`, and TTNMAPPERORG-USER `your mail address`.

<!--![image](https://github.com/jorgenavarroortiz/lora-lorawan-simple-experiments/assets/17797704/4ff1e277-61b5-413b-ad8b-be66c79cdb1c)-->
<img src="https://raw.githubusercontent.com/jorgenavarroortiz/lora-lorawan-simple-experiments/main/img/ttgo-tbeam-03.png" alt="ttgo-tbeam-03" width="800"/>

4) You should see the node in [TTN Mapper](https://ttnmapper.org/heatmap/).

<!--![image](https://github.com/jorgenavarroortiz/lora-lorawan-simple-experiments/assets/17797704/4cb820de-5dbc-4433-af39-bc55bbb9145f)-->
<!--img src="https://raw.githubusercontent.com/jorgenavarroortiz/lora-lorawan-simple-experiments/main/img/ttgo-tbeam-04.png" alt="ttgo-tbeam-04" width="600"/-->
<img src="https://raw.githubusercontent.com/jorgenavarroortiz/lora-lorawan-simple-experiments/main/img/ttgo-tbeam-05.png" alt="ttgo-tbeam-05" width="800"/>

### LILYGO T-Higrow ESP32 Soil Tester BEM280

Based on https://github.com/Xinyuan-LilyGO/LilyGo-HiGrow.

Check the repository [t-higrow-lorawan](https://github.com/jorgenavarroortiz/t-higrow-lorawan).

<!--![image](https://github.com/jorgenavarroortiz/lora-lorawan-simple-experiments/assets/17797704/a731ec1d-cb19-4198-b863-f3bbbbd03034)-->
<img src="https://raw.githubusercontent.com/jorgenavarroortiz/lora-lorawan-simple-experiments/main/img/t-higrow-lorawan.png" alt="t-higrow-lorawan" width="800"/>

### WeMos D1 Mini with a Hallard LoRaWAN board (RN2483)

Chech the https://github.com/jorgenavarroortiz/lora-lorawan-simple-experiments/tree/main/LoRaWAN/ESP8266_RN2483_basic_DHT_BMP180 directory. It includes the source code for a WeMos D1 mini with a RN2483 LoRaWAN chip (using [Hallard's shield](https://github.com/hallard/WeMos-RN2483)), with a DHT22 sensor (temperature and humidity) and a BME180 sensor (temperature, pressure, altitude).

<!--![image](https://github.com/jorgenavarroortiz/lora-lorawan-simple-experiments/assets/17797704/598c5fe6-4ae3-4ee6-92b0-f55404a9049d)-->
<img src="https://raw.githubusercontent.com/jorgenavarroortiz/lora-lorawan-simple-experiments/main/img/wemos-rn2483-sensors.png" alt="wemos-rn2483-sensors" width="400"/>


