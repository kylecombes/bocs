import React, {Component} from 'react';

export default class LandingPageOrigin extends Component {

    render() {
        return (
            <section className="landing-page-section cell origin">
                <div className="landing-page-section-content cell grid-x">
                    <h1 className="landing-section-title light">Origin</h1>
                    <div className="cell large-9">
                        <p>The BOCS is a project developed as part of the <a href="http://poe.olin.edu/" alt="Principles of Engineering">
                            Principles of Engineering class</a> at <a href="http://www.olin.edu/" target="_blank" alt="Olin College of Engineering">Olin College of Engineering</a>.
                        </p>
                        <p>
                            The idea for the BOCS first came about during a trip to Boston. A few members of the eventual
                            team were strolling through the commons en route to Newbury Street when the subject of MIT
                            hacks came up. It was agreed upon by the party that there had not been many spectacular hacks
                            in recent years.
                        </p>
                        <p>The solution proposed to this problem was to create a device that could be placed in an MIT public
                            area and goad students into trying to interact with it. Puzzles were also agreed upon to be a
                            quality method of engagement, and as such the device became a puzzle box. The box would be
                            intellectually challenging to the extent that it would take a top-level engineering student a
                            non-trivial amount of time to solve it, and have several puzzles in it, thereby extending its
                            interaction and prank time.
                        </p>
                        <p>Thus, BOCS was born. Though it has gone through many iterations and is not forecast to see MIT’s campus
                            within the semester, much of the driving force behind the original concept can still be seen
                            in BOC’s final form.
                        </p>
                    </div>
                    <img alt="Olin College Principles of Engineering" src="http://poe.olin.edu/2017/Poezzle/assets/poe.svg" className="origin-poe cell large-3"/>
                </div>
            </section>
        )
    }

}