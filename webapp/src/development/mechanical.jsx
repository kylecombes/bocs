import React, {Component} from 'react';
import SketchfabCAD from './sketchfab-cad';
import CaptionedImage from "./image";

export default class Mechanical extends Component {

    render() {
        return (
            <section className="page mechanical">
                <h1 className="landing-section-title light">Mechanical</h1>
                <p>The overarching goals of this project were to create an interactive, enjoyable, and inconspicuous
                    puzzle box that could keep people entertained for a reasonable amount of time. Recognizing that our
                    creation would be left unattended in a public space (e.g. at MIT for curious students to interact
                    with), we wanted to make sure our creation looked unthreatening but still visually appealing. We
                    decided the best way to do this would be to create a simple cube in which all the inputs blended
                    well into the faces with interesting geometrically shaped sheet metal pieces on the exterior walls.
                    We painted all the exterior walls white to give the BOCS a clean, attractive appearance. We wanted
                    the BOCS to be reasonably compact, so to achieve this, we prototyped all inputs beforehand and
                    created a sizing assembly that let us determine the smallest possible cube that we could. The result
                    was a 14-inch cube whose walls fit together with dowel pegs, thus making the BOCS easy to take apart
                    for maintenance but seamless on the exterior.</p>
                <p>The BOCS features five retractable I/O devices that allow users to creatively input their solutions
                    to the puzzles that the BOCS has to offer. Several are implemented on the Multi-Input cube, which
                    rotates about its center to reveal various input methods using a servo. The front of the cube is a
                    solid white facade to blend in with the exterior of the BOCS, and the other sides have slots for
                    three easy-to-change input methods. The two that we currently have installed are the Trellis and the
                    Telegraph. The Trellis is a 4x4 illuminated button pad that can display any combination or pattern
                    of lights and receive any combination of button inputs. The telegraph is an immensely satisfying
                    large white button with a limit switch that allows for the input of messages like Morse Code or even
                    binary.</p>
                    <div className="cell grid-x">
                        <SketchfabCAD id="65642feb21404d3397cc276643e71a6a"/>
                        <SketchfabCAD id="40e94a3dfec54df094892324d1193963"/>
                </div>
                <CaptionedImage src="http://poe.olin.edu/2017/Poezzle/assets/token.jpg" caption="A token for winning the BOCS" className="align-right" style={{maxWidth: '20em'}}/>
                <p>The BOCS also features a servo-actuated standard 12-button number pad with discreet wiring that flips
                    out from the wall when in use. It allows for standard and intuitive numerical input, which can be
                    applied to many puzzles. Other input methods include the Piano, which slides out from the BOCS on
                    drawer slides and has seven piano keys ranging from F to B; and the Drawer, which uses a limit switch
                    and an ultrasonic sensor to detect the size of the inputted objects. Each of these inputs is
                    conducive to creative and potentially non-intuitive puzzles, which can be created and added to the
                    BOCS by other users.</p>
                <p>Although the BOCS may seem a bit condescending at times, it is compassionate enough to reward good
                    behavior and intelligence with the Prize Dispenser. Once the puzzles are successfully solved, the
                    BOCS dispenses rolls of Smarties using a slotted turnwheel, along with a laser-engraved commemorative
                    token.</p>
                <p>Our full CAD can be downloaded <a className="link" href="http://poe.olin.edu/2017/Poezzle/assets/BOCS-CAD.zip">here</a>.</p>
            </section>
        )
    }

}