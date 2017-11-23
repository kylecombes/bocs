import React, {Component} from 'react';
// import SVGInline from 'react-svg-inline';
// import headerSVG from '../assets/landing-header.svg';

export default class LandingPageHeader extends Component {

    render() {
        return (
            <section className="landing-page-header cell">
                <div className="landing-page-section-content cell grid-x">
                    <div className="medium-6 cell">
                        <img alt="BOCS logo" src="./assets/landing-header.svg"/>
                    </div>
                    <div className="medium-6 cell">
                        [Insert legit image of BOCS here]
                    </div>
                </div>
            </section>
        )
    }

}