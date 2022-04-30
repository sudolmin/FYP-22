const fs = require("fs");
const { decryptdata } = require("./encryptionHandler");

function writeFile(filename, data, encrypted=false) {

    // decrypting data i.e coming in encrypted form
    data = decryptdata(data);

    // stringify the decrypted JSON object.
    data = JSON.stringify(data);

    fs.writeFileSync(filename, data, "utf8");
}

function readFile(filename) {
    const data = fs.readFileSync(filename, 'utf8');

    // parse JSON string to JSON object
    const jsonData = JSON.parse(data.toString());

    return jsonData
}

module.exports = {
    "writeFile": writeFile,
    "readFile": readFile
}