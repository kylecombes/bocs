import React, {Component} from 'react';
import LandingPageHeader from './header';
import LandingPageFeatures from './features';
import Team from './team';
import LandingPageOrigin from './origin';
import Development from './development/development';


export default class LandingPage extends Component {

    render() {
        return (
            <div className="landing-page grid-x">
                <LandingPageHeader/>
                <LandingPageFeatures/>
                <LandingPageOrigin/>
                <Team/>
                <Development/>
            </div>
        )
    }
}