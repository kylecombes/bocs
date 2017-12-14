import React, {Component} from 'react';
import LandingPageHeader from './header';
import LandingPageFeatures from './features/features.jsx';
import Personality from './personality';
import Team from './team';
import LandingPageOrigin from './origin';
import Development from './development/development';
import Leaderboard from '../leaderboard/leaderboard';


export default class LandingPage extends Component {

    render() {
        return (
            <div className="landing-page grid-x">
                <LandingPageHeader/>
                <LandingPageFeatures/>
                <Personality/>
                <Leaderboard/>
                <LandingPageOrigin/>
                <Team/>
                <Development/>
            </div>
        )
    }
}