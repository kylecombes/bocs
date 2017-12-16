import express from 'express';
import WebSocket from 'ws';
import MongoClient from 'mongodb';
import fs from 'fs';
import dotenv from 'dotenv';
import expose from './expose';

if (fs.existsSync('./.env')) {
    dotenv.config();
}

const port = process.env.PORT || 2492;

// Current hack with Node and experimental modules
const fullPath = expose.__dirname.split('/');
fullPath.pop();
const rootDir = fullPath.join('/');

// Start the HTTP server
const httpServer = express()
    .use((req,res) => res.sendFile(rootDir + '/webapp/public/index.html'))
    .listen(port, () => console.log(`HTTP server listening on port ${port}`));

// Once the HTTP server is started, figure out MongoDB stuff
const username = process.env.BOCS_MONGO_USERNAME;
const password = process.env.BOCS_MONGO_PASSWORD;
const mongoPort = process.env.BOCS_MONGO_PORT;
const cluster1 = process.env.BOCS_MONGO_CLUSTER1;
const cluster2 = process.env.BOCS_MONGO_CLUSTER2;
const cluster3 = process.env.BOCS_MONGO_CLUSTER3;
const dbName = process.env.BOCS_MONGO_DB_NAME;
const replicaSet = process.env.BOCS_MONGO_REPLICA_SET;
const authSource = process.env.BOCS_MONGO_AUTH_SOURCE || 'admin';

function getLatestCompletionStats(db, count) {
    let completions = [];
    const queryRes = db.collection('attempts').find({ 'guess': { '$exists': false } }).sort({datettime: -1}).limit(count);
    queryRes.forEach((completion) => {
        completions.push({
            puzzle_id: completion.puzzle_id,
            time: completion.elapsed_time,
            datettime: completion.datettime,
        })
    });
    return { 'leaderboardStats': completions };
}

if (!(username && password && cluster1 && cluster2 && cluster3 && dbName && replicaSet)) {
    // Couldn't find environment variables
    console.error('Please check your environment variables to make sure the following are defined:');
    console.error('\t- BOCS_MONGO_USERNAME');
    console.error('\t- BOCS_MONGO_PASSWORD');
    console.error('\t- BOCS_MONGO_PORT');
    console.error('\t- BOCS_MONGO_CLUSTER1');
    console.error('\t- BOCS_MONGO_CLUSTER2');
    console.error('\t- BOCS_MONGO_CLUSTER3');
    console.error('\t- BOCS_MONGO_DB_NAME');
    console.error('\t- BOCS_MONGO_REPLICA_SET');
    console.warn('Connection to MongoDB failed, so not starting WebSocket server. HTTP server still running.');
} else {

    const mongoUri = `mongodb://${username}:${password}@${cluster1}:${mongoPort},${cluster2}:${mongoPort},${cluster3}:${mongoPort}/${dbName}?ssl=true&replicaSet=${replicaSet}&authSource=${authSource}`;

    // Connect to the MongoDB database
    MongoClient.connect(mongoUri, (err, db) => {
        if (!err) {
            console.log('Successfully connected to MongoDB instance.');

            // Get the 20 latest leaderboard stats
            let stats = getLatestCompletionStats(db, 20);

            // Start the WebSocket server
            const server = new WebSocket.Server({server: httpServer});
            // Define a function for sending some data to all connected clients
            server.broadcast = (data) => {
                data = JSON.stringify(data);
                server.clients.forEach((client) => {
                    if (client.readyState === WebSocket.OPEN) {
                        client.send(data);
                    }
                })
            };
            server.on('connection', (ws) => {
                console.log('Client connected');
                console.log(stats);

                // Send latest leaderboard stats
                ws.send(JSON.stringify(stats));

                // Register message event listener
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
                                    if (response.result.ok === 1) {
                                        console.log(`Successfully saved attempt to db (ObjectID: ${response.insertedId})`);
                                        stats = getLatestCompletionStats(db, 20);
                                        server.broadcast(stats);
                                    } else {
                                        console.error(`Error saving attempt to db:\n${response}`)
                                    }
                                });
                        }

                    } catch (e) {
                        console.warn(`Bad message type received:\n${message}`)
                    }
                });
                ws.on('close', () => {
                    console.log('Client disconnected');
                })
            });
            console.log(`WebSocket server listening on port ${port}`);
        } else {
            console.error(err);
            console.error('Error connecting to MongoDB instance.');
        }
    });
}