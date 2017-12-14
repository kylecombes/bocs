import React, {Component} from 'react';
import CaptionedImage from "./image";

export default class Electrical extends Component {

    render() {
        return (
            <section className="page">
                <h1 className="landing-section-title light">Electrical</h1>
                <p>The BOCS has a variety of input systems, all of which need power and communication lines to allow them to
                    pass information to the BOCS’s central highly advanced central cortex. Subsets of inputs each share a
                    single Arduino, which is connected to a central Raspberry Pi Zero W via serial cables. These Arduinos
                    are powered by a 12V 8Ah lead acid battery. Power from the battery is also routed through a 5V 10A
                    voltage regulator which supplies BOCS’s servos, as well as the two Raspberry Pis and the speakers.</p>
                <CaptionedImage src="http://poe.olin.edu/2017/Poezzle/assets/system-diagram.svg"
                                style={{paddingTop: '3em', paddingBottom: '3em'}}
                                className="align-center"
                                caption="The BOCS consists of two Raspberry Pis which communicate with the inputs via a series of Arduinos."/>
                <p>The BOCS’s piano input has its own separate Arduino Nano, and each key is connected to a digital input
                    pin. The piano’s servo is continuous rotation, controlled with two limit switches that depress as
                    the piano fully extends or retracts.</p>
                <CaptionedImage src="http://poe.olin.edu/2017/Poezzle/assets/pianoduino.svg"
                                style={{maxHeight: '50em'}}
                                className="align-center"
                                caption="The piano input is run by an Arduino with seven momentary-switch key buttons, two limit switches, and a servo."/>
                <p>The BOCS’s prize dispenser and drawer are hooked into a single
                    Arduino Uno; the prize dispenser features two servos, one normal servo and one continuous rotation
                    servo. The continuous servo operates the prize dispensing and simply runs for a given period of time,
                    no need for locational knowledge. The normal servo dispenses tokens. The drawer’s limit switch and
                    ultrasonic sensor allow the Arduino to sense whether or not the drawer is closed and whether or not
                    something is in the drawer.</p>
                <CaptionedImage src="http://poe.olin.edu/2017/Poezzle/assets/arduino_drawer.svg"
                                className="align-center"
                                style={{minWidth: '70%'}}
                                caption="The prize dispenser and drawer are run off another Arduino."/>
                <p>
                    The third Arduino (Arduino Uno) holds the rest of the puzzles: the Trellis, number pad, telegraph,
                    and start button. The telegraph is a limit switch wired into the Arduino as a button. The start
                    button is an amalgamation of a button and an LED ring, which emits an intimidating red glow and is
                    controlled separately from the button. The Trellis led button grid only uses four wires - power,
                    ground, and I2C connection. This allows commands from the Arduino to write to the individual LEDs
                    and read the individual buttons on a minimal number of wires. The final input, the number pad, is
                    not nearly as nice, and employs a wire for each button.
                </p>
                <CaptionedImage src="http://poe.olin.edu/2017/Poezzle/assets/arduino1.svg"
                                className="align-center"
                                style={{minWidth: '70%'}}
                                caption="The prize dispenser and drawer are run off another Arduino."/>
            </section>
        )
    }

}