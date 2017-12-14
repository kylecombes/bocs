import React, {Component} from 'react';
// import SVGInline from 'react-svg-inline';
// import headerSVG from '../assets/landing-header.svg';

export default class LandingPageHeader extends Component {

    render() {
        return (
            <section className="landing-page-header cell">
                <div className="landing-page-section-content cell grid-x">
                    <div className="medium-6 cell">
                        <img className="bocs-logo" alt="BOCS logo" src="http://poe.olin.edu/2017/Poezzle/assets/landing-header.svg"/>
                    </div>
                    <div className="medium-6 cell">
                        <img alt="BOCS image" src="http://poe.olin.edu/2017/Poezzle/assets/BOCS-top.jpg"/>
                    </div>
                </div>
            </section>
        )
    }

}