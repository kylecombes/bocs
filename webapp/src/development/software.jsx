import React, {Component} from 'react';
import CaptionedImage from "./image";

export default class Software extends Component {

    constructor() {
        super();

        this.state = {
            code: `  if (handshakeCompleted || !usingHandshake) {

    // Do connected stuff (check inputs, check for serial messages, etc)

  } else { // We have yet to establish a connection with the computer
    if (Serial.available() > 0) { // The computer is responding
      String msg = Serial.readString();
      handshakeCompleted = msg.equals("Hello from computer\\n");
      Serial.println(handshakeCompleted ? "Connected" : "Not connected");
      expectedHeartbeatByTime = millis() + HEARTBEAT_TIMEOUT;
    } else if (millis() > nextBroadcastTime) { // Cry out for a computer
      Serial.println("Hello from arduino_drawer");
      nextBroadcastTime = millis() + BROADCAST_INTERVAL;
    }
  }`
        }
    }

    render() {
        return (
            <section className="page">
                <h1 className="landing-section-title light">Software</h1>
                <p>The software system running the BOCS is what ties everything together. Each Arduino is connected to
                    a central Raspberry Pi Zero W, which runs core puzzle code written in Python. Due to the DAC pHAT and
                    the PaPiRus e-ink hat using the same PWM pin, a second Raspberry Pi was (unfortunately) required to
                    play audio. It thus runs a simple audio server. A brief overview can
                    be found below but, for an in-depth guide to programming the BOCS, be sure to&nbsp;
                    <a href="https://github.com/kylecombes/bocs/wiki" target="_blank">check out our GitHub wiki</a>.
                </p>
                <CaptionedImage src="http://poe.olin.edu/2017/Poezzle/assets/Software-overview.svg"
                                className="align-center"
                                caption="The non-embedded software side of the BOCS consists of multiple components. Each gray dashed box represents a software subsystem. Each solid box represents a module within that subsystem."/>
                <h2>Writing Your Own Puzzles</h2>
                <p>One of the most important features of the BOCS is the ability to easily write your own puzzles without
                    knowing the inner workings of the BOCS and its various inputs. It's all done in Python, which makes
                    life a lot easier. We've already written an extensive guide on&nbsp;
                    <a href="https://github.com/kylecombes/bocs/wiki/Writing-a-Puzzle-for-the-BOCS" target="_blank">
                        our GitHub wiki</a>, so head on over there if you'd like to check out how it's done.
                </p>
                <h2>Arduinos</h2>
                <p>Each Arduino is completely independent of the others. The requirements for the Arduinos to
                    connect to the rest of the BOCS are fourfold:
                </p>
                <ul>
                    <li>Perform a handshake by saying hello to the computer and accepting a hello in return</li>
                    <li>Transmit input messages back to the Raspberry Pi encoded in a standard JSON format</li>
                    <li>Accept string messages sent over serial to control any output</li>
                    <li>Send a "ba-dump" message (a heartbeat) over serial to the computer at least once per 10 seconds</li>
                </ul>
                <p>For detailed information, complete with formatted and colored sample code, on how to add your own Arduino
                    for new BOCS I/O, head on over to this&nbsp;
                    <a href="https://github.com/kylecombes/bocs/wiki/Adding-an-I-O-Arduino">guide on our wiki</a>.
                </p>
                <h4>Performing the Handshake</h4>
                <p>Registering with the RasPi is quite straightforward: simply keep transmitting "Hello from &lt;arduino_name&gt;"
                    until the computer responds "Hello from computer", at which point the Arduino should transmit "connected"
                    over serial to let the computer know the Arduino is listening for messages. The Arduino can now
                    consider itself connected.
                </p>
                <h4>Transmitting Input Messages</h4>
                <p>Sending input messages back to the computer is also quite straightforward. Simply send a JSON string
                over serial with an "event_id" and a "data" attribute. Here's an example (remove the newlines in your
                    implementation -- they've just been added here for readability):
                </p>
                <div className="code json">{'{'}<br/>
                    &nbsp;&nbsp;&nbsp;"event_id": "2",<br/>
                    &nbsp;&nbsp;&nbsp;"data": "3241"<br/>
                {'}'}
                </div>
                <p>This message will get parsed by the Raspberry Pi and passed on to the puzzle.</p>
                <p>Why JSON, you ask? Well, it allows various data types, lengths, and complexities to be sent to the computer
                    from the Arduino. It's future-proofing for inputs you build that may want to send data types we haven't
                    anticipated.
                </p>
                <h4>Accepting Messages from the Raspberry Pi</h4>
                <p>Messages sent from the Raspberry Pi to the Arduinos are not encoded as JSON due to the extremely slow
                    JSON-decoding performance of an Arduino. Rather, messages begin with an identifier character followed
                    by the message "payload" (a string encoding data in any format you care to parse). For a look at some
                well-documented Arduino code running multiple inputs, take a look&nbsp;
                    <a href="https://github.com/kylecombes/bocs/blob/master/arduino/arduino1/arduino1.ino" target="_blank">here</a>.
                </p>
                <h4>Emitting Heartbeats</h4>
                <p>This is likely the easiest item on this list. Quite literally all you need to do is transmit "ba-dump"
                    (a heartbeat sound) at least once every 10 seconds (every 3 seconds is a good number). If the computer
                    doesn't receive a message within that timeframe, it will go back into handshake mode.
                </p>
                <CaptionedImage src="http://poe.olin.edu/2017/Poezzle/assets/Arduino-overview.svg" className="align-center"
                caption="Every Arduino has the same lifecycle, consisting of a pairing mode and a normal operating mode, in which it checks for messages from the computer and sends any input messages to the computer."
                />
                <p>All of our code is available in our <a href="https://github.com/kylecombes/bocs" target="_blank">GitHub repo</a>.</p>
            </section>
        )
    }
}