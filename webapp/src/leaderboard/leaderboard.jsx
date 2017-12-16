import React, {Component} from 'react';
import StatPane from "./stat-pane";

export default class Leaderboard extends Component {

    constructor() {
        super();

        this.state = {};
    }

    componentDidMount() {
        if (this.props.server) {
            (new WebSocket(this.props.server)).addEventListener('message', (msg) => {
                console.log("Received message");
                msg = JSON.parse(msg.data);
                if (msg.hasOwnProperty('leaderboardStats')) {
                    this.setState({stats: msg.leaderboardStats});
                } else {
                    console.log("Could not find stats in message");
                }
            });
        }
    }

    render() {

        let stats = [];
        if (this.state.stats) {
            for (let key in this.state.stats) {
                const stat = this.state.stats[key];
                stats.push(<StatPane key={key} name={stat.puzzle_id} time={stat.time}/>)
            }
            return (
                <div className="landing-page-section cell leaderboard">
                    <div className="landing-page-section-content cell">
                        <h1 className="landing-section-title">Leaderboard</h1>
                        {stats}
                    </div>
                </div>
            )
        } else {
            return null;
        }

    }

}