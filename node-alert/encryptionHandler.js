var aesjs = require('aes-js');
require('dotenv').config();

var key = [0, 1, 2, 3, 4, 25, 26, 20, 21, 22, 23, 24, 27, 28,
    29, 30, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15,
    16, 17, 18, 19, 31];


function encryptdata(data) {    
    var textBytes = aesjs.utils.utf8.toBytes(JSON.stringify(data));
    
    var aesCtr = new aesjs.ModeOfOperation.ctr(key, new aesjs.Counter(6));
    var encryptedBytes = aesCtr.encrypt(textBytes);
    
    var encryptedHex = aesjs.utils.hex.fromBytes(encryptedBytes);
    return encryptedHex;
}

function decryptdata(encData) {
    var encryptedBytes = aesjs.utils.hex.toBytes(encData);
    // The counter mode of operation maintains internal state, so to
    // decrypt a new instance must be instantiated.
    var aesCtr = new aesjs.ModeOfOperation.ctr(key, new aesjs.Counter(6));
    var decryptedBytes = aesCtr.decrypt(encryptedBytes);
    
    // Convert our bytes back into text
    var decryptedText = aesjs.utils.utf8.fromBytes(decryptedBytes);
    return decryptedText;
}

module.exports = {
    "decryptdata":decryptdata,
    "encryptdata":encryptdata
}