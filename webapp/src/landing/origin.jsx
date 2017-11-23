import React, {Component} from 'react';

export default class LandingPageOrigin extends Component {

    render() {
        return (
            <section className="landing-page-section cell origin">
                <div className="landing-page-section-content cell grid-x">
                    <h1 className="landing-section-title light">Origin</h1>
                    <p className="cell large-9">[Insert PoE origin story here]</p>
                    <img alt="Olin College Principles of Engineering" src="assets/poe.svg" className="cell large-3"/>
                </div>
            </section>
        )
    }

}