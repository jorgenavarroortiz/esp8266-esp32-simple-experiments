# Simple experiments with LoRa and LoRaWAN

This repository is intended to collect a number of simple implementations with different nodes that support LoRa or LoRaWAN.

## LoRa

### TTGO-LoRa32-SendReceiver 

From https://github.com/cubapp/LilyGO-TTGO-LoRa32-SenderReceiver, included in https://github.com/jorgenavarroortiz/lora-lorawan-simple-experiments/tree/main/LoRa/TTGO-LoRa32-SenderReceiver. It simply sends a counter and a random number from a LoRa sender to a LoRa receiver, and show the information in the display.

![image](https://github.com/jorgenavarroortiz/lora-lorawan-simple-experiments/assets/17797704/cfa9f4e9-688d-4a51-83df-58e8a91ad4e4)

### Sending images over LoRa

TO BE WRITEN, using Pycom nodes.

## LoRaWAN

### The Things Uno

The [LoRaWAN/TheThingsUno/TTNUnoSendABP](https://github.com/jorgenavarroortiz/lora-lorawan-simple-experiments/tree/main/LoRaWAN/TheThingsUno/TTNUnoSendABP) directory contains a sample code that sends information in CayenneLPP (Low Power Payload) format. It includes the code for a flame sensor detector KY-026.

![image](https://github.com/jorgenavarroortiz/lora-lorawan-simple-experiments/assets/17797704/65772651-8909-4085-bf38-cf3e96396dc0)

### TTGO T-beam

From [TTGO T-Beam Tracker for The Things Network](https://github.com/kizniche/ttgo-tbeam-ttn-tracker).

The [lora-lorawan-simple-experiments/LoRaWAN/ttgo-tbeam-ttn-tracker-sx1262](https://github.com/jorgenavarroortiz/lora-lorawan-simple-experiments/tree/main/LoRaWAN/ttgo-tbeam-ttn-tracker-sx1262) directory contains a sample code that sends the GPS information to TTN.

Steps to configugre TTN mapper:

1) Add the code to a TTGO T-Beam. The node shall be outdoor to get information from GPS satellites.

![image](https://github.com/jorgenavarroortiz/lora-lorawan-simple-experiments/assets/17797704/a840d753-c488-4848-801b-42fd3819d317)

2) In TTN, create a new application for this experiment (`wimunet-ttnmapper` in the example). Configure the nodes to use CayenneLPP decoder. You can export the received information in JSON format (`Export as JSON` button).

![image](https://github.com/jorgenavarroortiz/lora-lorawan-simple-experiments/assets/17797704/648ce1a5-e804-4e0d-8980-2e36dbace55c)

3) In the TTN application, go to `integrations` -> `webhooks` with webhook format `protocol buffers`, base URL `https://integrations.ttnmapper.org/tts/v3`, and TTNMAPPERORG-USER `your mail address`.

![image](https://github.com/jorgenavarroortiz/lora-lorawan-simple-experiments/assets/17797704/4ff1e277-61b5-413b-ad8b-be66c79cdb1c)

4) You should see the node in `https://ttnmapper.org/heatmap/`.

![image](https://github.com/jorgenavarroortiz/lora-lorawan-simple-experiments/assets/17797704/4cb820de-5dbc-4433-af39-bc55bbb9145f)

### LILYGO T-Higrow ESP32 Soil Tester BEM280

Based on https://github.com/Xinyuan-LilyGO/LilyGo-HiGrow.

Check the repository [t-higrow-lorawan](https://github.com/jorgenavarroortiz/t-higrow-lorawan).

![image](https://github.com/jorgenavarroortiz/lora-lorawan-simple-experiments/assets/17797704/a731ec1d-cb19-4198-b863-f3bbbbd03034)

### WeMos D1 Mini with a Hallard LoRaWAN board (RN2483)

TO BE DONE

![image](https://github.com/jorgenavarroortiz/lora-lorawan-simple-experiments/assets/17797704/598c5fe6-4ae3-4ee6-92b0-f55404a9049d)


