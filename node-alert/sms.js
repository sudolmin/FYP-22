require('dotenv').config();
const accountSid = process.env.TWILIO_ACCOUNT_SID;
const authToken = process.env.TWILIO_AUTH_TOKEN;
const fromNumber = process.env.FROM_NUMBER;

const client = require("twilio")(accountSid, authToken);

async function sendMessage(number, messageBody){
    client.messages
    .create({
        body: messageBody,
        from: fromNumber,
        to: number
    })
    .then((message) => console.log(message));
}

module.exports = {
    "sendMessage":sendMessage,
}