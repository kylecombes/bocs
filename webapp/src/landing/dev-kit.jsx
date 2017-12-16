import React, {Component} from 'react';

export default class LandingPageDevKit extends Component {

    render() {
        return (
            <section className="landing-page-section cell dev-kit">
                <div className="landing-page-section-content cell grid-x">
                    <h1 className="landing-section-title light">SDK</h1>
                    <div className="cell large-9">
                        <p>BOCS features a <a href="https://github.com/kylecombes/bocs" alt="BOCS development wiki">fully documented API</a> for writing
                            your own puzzles using Python. Communication with I/O devices is standardized, so you don't
                            even need to know how to communicate with an Arduino.</p>
                        <p>That being said, integrating your own new Arduino-controlled I/O device couldn't be easier.
                        Implementing a standardized message protocol over serial is all that's required to tie into the
                        BOCS platform.</p>
                        <p>Read more about the software system on the <span className="link" onClick={() => this.props.setPage('software')}>
                        Software page</span>.</p>
                    </div>
                    <img alt="Olin College Principles of Engineering" src="http://poe.olin.edu/2017/Poezzle/assets/sdk.svg" className="origin-poe cell large-3"/>
                </div>
            </section>
        )
    }

}