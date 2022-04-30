require('dotenv').config();

const nodemailer = require("nodemailer");

// async..await is not allowed in global scope, must use a wrapper
async function sendMailFromNode(curr_val) {
  // create reusable transporter object using the default SMTP transport
    let transporter = nodemailer.createTransport({
        host: "smtp.gmail.com",
        port: 587,
        secure: false, // true for 465, false for other ports
        auth: {
        user: process.env.SENDER_EMAIL_ID, // generated ethereal user
        pass: process.env.EMAIL_PASSWORD, // generated ethereal password
        },
    });

  // send mail with defined transport object
    let info = await transporter.sendMail({
        from: `"${process.env.SENDER} 👻" <${process.env.SENDER_EMAIL_ID}>`, // sender address
        to: process.env.RECIEVER_EMAIL_ID, // list of receivers
        subject: "Alert sent from MQ-2 sensor", // Subject line
        html: curr_val // html body
    });

    console.log("Message sent: %s", info.messageId);

    console.log("Preview URL: %s", nodemailer.getTestMessageUrl(info));
}

module.exports.sendMailFromNode = sendMailFromNode;