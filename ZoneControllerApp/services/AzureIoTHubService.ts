// var clientFromConnectionString = require('azure-iot-device-mqtt').clientFromConnectionString;
// var Message = require('azure-iot-device').Message;

// // Device connection string from Azure Portal
// var client = clientFromConnectionString(connectionString);


// interface Payload {
//   newValue?: number;
//   [key: string]: any;
// }

// export const sendCommandToDevice = (command: string, payload: Payload = {}): void => {
//     const messageContent: Payload = { command, ...payload };
//     const message: Message = new Message(JSON.stringify(messageContent));
//     client.sendEvent(message, (err) => {
//         if (err) console.error('Failed to send command:', err);
//     });
// }
