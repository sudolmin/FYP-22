const mqtt = require("mqtt");
const { writeFile, readFile } = require("./filehandler");
const { sendMailFromNode } = require("./mailer");
const { sendMessage } = require("./sms");
const client = mqtt.connect("ws://3.108.52.230:9001");

var jsonObj;

const configTopic = "mq2/config";
const configFile = "./config.json";

const limitJson = { lpg: 11000, co: 25000, smoke: 25000, analog_data: 500 };

// subscribe to config topic inorder to get config data
client.subscribe(configTopic);

client.on("connect", function () {
    client.subscribe("mq2/calc_data", function (err) {
        if (!err) {
            client.publish("mq2/state", "Hello mqtt");
        }
    });
});

client.on("message", function (topic, message) {
    if (topic === configTopic) {
        // convert JSON object to a string
        const data = message.toString();

        // write file to disk
        writeFile(configFile, data);

        // readFile(configFile);
    } else {
        // message is Buffer
        jsonObj = JSON.parse(message.toString());
        var configData = JSON.parse(readFile(configFile))
        var phn_no = configData.phn_no
        var email_id = configData.email_id
        var raw_limit = parseInt(configData.raw_limit)

        

        console.log(
            jsonObj
        );

        if (jsonObj.raw_analog.value>raw_limit) {
            console.log("Limit for raw data surpassed");
            sendMessage(phn_no, "Limit for raw data surpassed");
            sendMailFromNode("Limit for raw data surpassed"+message.toString());
        }
    }
});
