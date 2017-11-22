import WebSocket from 'ws';

const server = new WebSocket.Server({ port: 2492 });
server.on('connection', (ws) => {
	console.log('Client connected');
	ws.on('message', (message) => {
		console.log("Message received:");
		console.log(message);
		if (message === 'Hello, Mother!') {
			ws.send('Hello, son!');
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
