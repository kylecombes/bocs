import React, {Component} from 'react';
import LandingPageHeader from './header';
import LandingPageFeatures from './features/features.jsx';
import LandingPageDevKit from './dev-kit';
import Personality from './personality';
import Team from './team';
import LandingPageOrigin from './origin';
import DemoVideo from '../landing/demo-video';
import Leaderboard from '../leaderboard/leaderboard';


export default class LandingPage extends Component {

    constructor() {
        super();

        // TODO Load server URI from environment variable
        this.state = {
            leaderboardServer: 'ws://poe-bocs.herokuapp.com'
        }
    }

    render() {
        return (
            <div className="landing-page grid-x">
                <LandingPageHeader/>
                <LandingPageFeatures/>
                <Personality/>
                <Leaderboard server={this.state.leaderboardServer}/>
                <DemoVideo/>
                <LandingPageDevKit/>
                <Team/>
                <LandingPageOrigin/>
            </div>
        )
    }
}