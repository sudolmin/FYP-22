#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <WiFiManager.h>
#include <SFE_BMP180.h>
#include <Wire.h>

#define         MQ_PIN                       (0)     //define which analog input channel you are going to use
#define         RL_VALUE                     (5)     //define the load resistance on the board, in kilo ohms
#define         RO_CLEAN_AIR_FACTOR          (9.83)  //RO_CLEAR_AIR_FACTOR=(Sensor resistance in clean air)/RO,
//which is derived from the chart in datasheet

/***********************Software Related Macros************************************/
#define         CALIBARAION_SAMPLE_TIMES     (50)    //define how many samples you are going to take in the calibration phase
#define         CALIBRATION_SAMPLE_INTERVAL  (500)   //define the time interal(in milisecond) between each samples in the
//cablibration phase
#define         READ_SAMPLE_INTERVAL         (50)    //define how many samples you are going to take in normal operation
#define         READ_SAMPLE_TIMES            (5)     //define the time interal(in milisecond) between each samples in 
//normal operation

/**********************Application Related Macros**********************************/
#define         GAS_LPG                      (0)
#define         GAS_CO                       (1)
#define         GAS_SMOKE                    (2)

/*****************************Globals***********************************************/
float           LPGCurve[3]  =  {2.3, 0.21, -0.47}; //two points are taken from the curve.
//with these two points, a line is formed which is "approximately equivalent"
//to the original curve.
//data format:{ x, y, slope}; point1: (lg200, 0.21), point2: (lg10000, -0.59)
float           COCurve[3]  =  {2.3, 0.72, -0.34};  //two points are taken from the curve.
//with these two points, a line is formed which is "approximately equivalent"
//to the original curve.
//data format:{ x, y, slope}; point1: (lg200, 0.72), point2: (lg10000,  0.15)
float           SmokeCurve[3] = {2.3, 0.53, -0.44}; //two points are taken from the curve.
//with these two points, a line is formed which is "approximately equivalent"
//to the original curve.
//data format:{ x, y, slope}; point1: (lg200, 0.53), point2: (lg10000,  -0.22)
float           Ro           = 2.5;                //Ro is initialized to 10 kilo ohms

float           lpgval;
float           coval;
float           smokeval;

const char* mqttServer = "";
int mqttPort;
const char* mqttPortStr = "1883";

//delay in sending out data
char* data_delay_str = "500";
int data_delay_int;

int smokeA0 = MQ_PIN;
float sensorValue;

char senstrval[7];


// You will need to create an SFE_BMP180 object, here called "pressure":

SFE_BMP180 pressure;
StaticJsonDocument<1024> doc;
#define ALTITUDE 50.0 // Altitude of ASSAM. in meters

char            out[1024];
JsonObject tempObj  = doc.createNestedObject("temp");
//JsonObject absolutePressureObj  = doc.createNestedObject("abs_pres");
//JsonObject relativePressureObj  = doc.createNestedObject("rel_pres");
JsonObject originalaltitudeObj  = doc.createNestedObject("orig_alt");
//JsonObject computedaltitudeObj  = doc.createNestedObject("computed_alt");
JsonObject lpgObj  = doc.createNestedObject("lpg");
JsonObject coObj  = doc.createNestedObject("co");
JsonObject smokeObj  = doc.createNestedObject("smoke");
JsonObject analogObj  = doc.createNestedObject("raw_analog");

WiFiClient espClient;
PubSubClient client(espClient);

void setup() {

  WiFi.mode(WIFI_STA);
  Serial.begin(115200);

  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wm;

  // Automatically connect using saved credentials,
  // if connection fails, it starts an access point with the specified name ( "AutoConnectAP"),
  // if empty will auto generate SSID, if password is blank it will be anonymous AP (wm.autoConnect())
  // then goes into a blocking loop awaiting configuration and will return success result

  // Remove any previous network settings
//  wm.resetSettings();

  // Supress Debug information
  wm.setDebugOutput(false);

  // Define a text box, 50 characters maximum
  WiFiManagerParameter mqttserver_addr_box("mqtt_addr", "Enter mqtt host address", mqttServer, 50);
  WiFiManagerParameter mqttserver_port_box("mqtt_port", "Enter mqtt server port", mqttPortStr, 6);
  WiFiManagerParameter datadelay_box("data_delay", "Enter data delay [in ms]", data_delay_str, 7);

  // Add custom parameter
  wm.addParameter(&mqttserver_addr_box);
  wm.addParameter(&mqttserver_port_box);
  wm.addParameter(&datadelay_box);

  bool res;
  // res = wm.autoConnect(); // auto generated AP name from chipid
  // res = wm.autoConnect("AutoConnectAP"); // anonymous ap
  res = wm.autoConnect("MQ2-AP", "password"); // password protected ap

  mqttServer =  mqttserver_addr_box.getValue();
  mqttPort = atoi(mqttserver_port_box.getValue());
  data_delay_int = atoi(datadelay_box.getValue());
  
  if (!res) {
    Serial.println("Failed to connect");
    // ESP.restart();
  }
  else {
    //if you get here you have connected to the WiFi
    Serial.println("Connected to the WiFi network");
    client.setServer(mqttServer, mqttPort);
    client.setCallback(callback);

    while (!client.connected()) {
      Serial.println("Connecting to MQTT...");
      Serial.print("mqtt://");
      Serial.print(mqttServer);
      Serial.print(":");
      Serial.print(mqttPort);

      if (client.connect("ESP8266Client")) {

        Serial.println("connected");

      } else {

        Serial.print("failed with state ");
        Serial.println(client.state());
        delay(2000);

      }
      Serial.println("Gas sensor warming up!");
      Serial.print("Calibration is done...\n");
      Serial.print("Ro=");
      Serial.print(Ro);
      Serial.print("kohm");
      Serial.print("\n");
    }

    client.publish("mq2/state", "started"); //Topic name
  }

  if (pressure.begin())
    Serial.println("BMP180 init success");
  else
  {
    // Oops, something went wrong, this is usually a connection problem,
    // see the comments at the top of this sketch for the proper connections.

    Serial.println("BMP180 init fail\n\n");
    while (1); // Pause forever.
  }

}

void callback(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message arrived in topic: ");
  Serial.println(topic);

  Serial.print("Message:");
  for (int i = 0; i < length; i++) {
    Serial.print((char)payload[i]);
  }

  Serial.println();
  Serial.println("-----------------------");

}

void loop() {
  //  client.loop();
  bmploop();

  sensorValue = analogRead(smokeA0);

  lpgval = MQGetGasPercentage(MQRead(MQ_PIN) / Ro, GAS_LPG);

  coval = MQGetGasPercentage(MQRead(MQ_PIN) / Ro, GAS_CO);

  smokeval = MQGetGasPercentage(MQRead(MQ_PIN) / Ro, GAS_SMOKE);

  lpgObj["value"] = lpgval;
  lpgObj["unit"] = "ppm";
  coObj["value"] = coval;
  coObj["unit"] = "ppm";
  smokeObj["value"] = smokeval;
  smokeObj["unit"] = "ppm";
  analogObj["value"] = sensorValue;
  analogObj["unit"] = "units";

  serializeJson(doc, out);
  Serial.println(out);

  client.publish("mq2/calc_data", out);

  delay(500);
}

void bmploop()
{
  char status;
  double T, P, p0, a;

  char TempString[10];  //  Hold The Convert Data

  // Loop here getting pressure readings every 10 seconds.

  // If you want sea-level-compensated pressure, as used in weather reports,
  // you will need to know the altitude at which your measurements are taken.
  // We're using a constant called ALTITUDE in this sketch:

  originalaltitudeObj["value"] = ALTITUDE;
  originalaltitudeObj["unit"] = "meters";
  // If you want to measure altitude, and not pressure, you will instead need
  // to provide a known baseline pressure. This is shown at the end of the sketch.

  // You must first get a temperature measurement to perform a pressure reading.

  // Start a temperature measurement:
  // If request is successful, the number of ms to wait is returned.
  // If request is unsuccessful, 0 is returned.

  status = pressure.startTemperature();
  delay(status);
  status = pressure.getTemperature(T);

  //  dtostrf(T,2,2,TempString);
  tempObj["value"] = T;
  tempObj["unit"] = "celsius";

  delay(1000);  // Pause for 1 seconds.
}

/****************** MQResistanceCalculation ****************************************
  Input:   raw_adc - raw value read from adc, which represents the voltage
  Output:  the calculated sensor resistance
  Remarks: The sensor and the load resistor forms a voltage divider. Given the voltage
         across the load resistor and its resistance, the resistance of the sensor
         could be derived.
************************************************************************************/
float MQResistanceCalculation(int raw_adc)
{
  return ( ((float)RL_VALUE * (1023 - raw_adc) / raw_adc));
}

/***************************** MQCalibration ****************************************
  Input:   mq_pin - analog channel
  Output:  Ro of the sensor
  Remarks: This function assumes that the sensor is in clean air. It use
         MQResistanceCalculation to calculates the sensor resistance in clean air
         and then divides it with RO_CLEAN_AIR_FACTOR. RO_CLEAN_AIR_FACTOR is about
         10, which differs slightly between different sensors.
************************************************************************************/
float MQCalibration(int mq_pin)
{
  int i;
  float val = 0;

  for (i = 0; i < CALIBARAION_SAMPLE_TIMES; i++) {      //take multiple samples
    val += MQResistanceCalculation(analogRead(mq_pin));
    delay(CALIBRATION_SAMPLE_INTERVAL);
  }
  val = val / CALIBARAION_SAMPLE_TIMES;                 //calculate the average value

  val = val / RO_CLEAN_AIR_FACTOR;                      //divided by RO_CLEAN_AIR_FACTOR yields the Ro
  //according to the chart in the datasheet

  return val;
}
/*****************************  MQRead *********************************************
  Input:   mq_pin - analog channel
  Output:  Rs of the sensor
  Remarks: This function use MQResistanceCalculation to caculate the sensor resistenc (Rs).
         The Rs changes as the sensor is in the different consentration of the target
         gas. The sample times and the time interval between samples could be configured
         by changing the definition of the macros.
************************************************************************************/
float MQRead(int mq_pin)
{
  int i;
  float rs = 0;

  for (i = 0; i < READ_SAMPLE_TIMES; i++) {
    rs += MQResistanceCalculation(analogRead(mq_pin));
    delay(READ_SAMPLE_INTERVAL);
  }

  rs = rs / READ_SAMPLE_TIMES;

  return rs;
}

/*****************************  MQGetGasPercentage **********************************
  Input:   rs_ro_ratio - Rs divided by Ro
         gas_id      - target gas type
  Output:  ppm of the target gas
  Remarks: This function passes different curves to the MQGetPercentage function which
         calculates the ppm (parts per million) of the target gas.
************************************************************************************/
int MQGetGasPercentage(float rs_ro_ratio, int gas_id)
{
  if ( gas_id == GAS_LPG ) {
    return MQGetPercentage(rs_ro_ratio, LPGCurve);
  } else if ( gas_id == GAS_CO ) {
    return MQGetPercentage(rs_ro_ratio, COCurve);
  } else if ( gas_id == GAS_SMOKE ) {
    return MQGetPercentage(rs_ro_ratio, SmokeCurve);
  }

  return 0;
}

/*****************************  MQGetPercentage **********************************
  Input:   rs_ro_ratio - Rs divided by Ro
         pcurve      - pointer to the curve of the target gas
  Output:  ppm of the target gas
  Remarks: By using the slope and a point of the line. The x(logarithmic value of ppm)
         of the line could be derived if y(rs_ro_ratio) is provided. As it is a
         logarithmic coordinate, power of 10 is used to convert the result to non-logarithmic
         value.
************************************************************************************/
int  MQGetPercentage(float rs_ro_ratio, float *pcurve)
{
  return (pow(10, ( ((log(rs_ro_ratio) - pcurve[1]) / pcurve[2]) + pcurve[0])));
}
