import WebSocket from 'ws';
import MongoClient from 'mongodb';
import fs from 'fs';
import dotenv from 'dotenv';

if (fs.existsSync('./.env')) {
    dotenv.config();
}

const port = process.env.BOCS_PORT || 2492;

// Figure out MongoDB stuff
const username = process.env.BOCS_MONGO_USERNAME;
const password = process.env.BOCS_MONGO_PASSWORD;
const mongoPort = process.env.BOCS_MONGO_PORT;
const cluster1 = process.env.BOCS_MONGO_CLUSTER1;
const cluster2 = process.env.BOCS_MONGO_CLUSTER2;
const cluster3 = process.env.BOCS_MONGO_CLUSTER3;
const dbName = process.env.BOCS_MONGO_DB_NAME;
const replicaSet = process.env.BOCS_MONGO_REPLICA_SET;
const authSource = process.env.BOCS_MONGO_AUTH_SOURCE || 'admin';

if (!(username && password && cluster1 && cluster2 && cluster3 && dbName && replicaSet)) {
    console.log('Please check your environment variables to make sure the following are defined:');
    console.log('\t- BOCS_MONGO_USERNAME');
    console.log('\t- BOCS_MONGO_PASSWORD');
    console.log('\t- BOCS_MONGO_PORT');
    console.log('\t- BOCS_MONGO_CLUSTER1');
    console.log('\t- BOCS_MONGO_CLUSTER2');
    console.log('\t- BOCS_MONGO_CLUSTER3');
    console.log('\t- BOCS_MONGO_DB_NAME');
    console.log('\t- BOCS_MONGO_REPLICA_SET');
    console.log('Exiting...');
    process.exit(1); // Exit with error
}

const mongoUri = `mongodb://${username}:${password}@${cluster1}:${mongoPort},${cluster2}:${mongoPort},${cluster3}:${mongoPort}/${dbName}?ssl=true&replicaSet=${replicaSet}&authSource=${authSource}`;

let db = null;

// Connect to the MongoDB database
MongoClient.connect(mongoUri, (err, ourDb) => {
    if (!err) {
        console.log('Successfully connected to MongoDB instance.');
        db = ourDb;
    } else {
        console.log(err);
        console.log('Error connecting to MongoDB instance.');
        console.log('Exiting...');
        process.exit(1); // Exit with error
    }
});

// Start the server
const server = new WebSocket.Server({ port });
server.on('connection', (ws) => {
    console.log('Client connected');
    ws.on('message', (message) => {
        console.log(message);
        try {
            const msg = JSON.parse(message);

            if (msg.msg_type === 'attempt') {
                // Store the attempt in our database
                delete msg['attempt']; // Don't save that in the db
                msg.datettime = new Date(); // Do save the time though
                db.collection('attempts').insertOne(msg)
                    .then((response) => {
                        if (response.result.ok == 1) {
                            console.log(`Successfully saved attempt to db (ObjectID: ${response.insertedId})`);
                        } else {
                            console.log(`Error saving attempt to db:\n${response}`)
                        }
                    });
            }

        } catch (e) {
            console.log(`WARNING: Bad message type received:\n${message}`)
        }
    });
});



//
// const express = require('express')
// const app = express()
//
// app.get('/', (req, res) => res.send(
// `<html>
// <head><title>Hello World</title></head>
// <body><h1>Hello World!</h1></body>
// </html>`))
//
// app.get('/stat', (req, res) => {
//
// 	const puzzleId = req.param('puzzleId')
//
// 	res.send(
// `<html>
// <head><title>Statistics</title></head>
// <body>You sent me something!<br>You completed puzzle #${puzzleId}</body>
// </html>`)
// })
//
//
// app.listen(3000, () => console.log('BOCS server listening on port 3000...'))
