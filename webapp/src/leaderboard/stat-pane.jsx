import React, {Component} from 'react';

export default class StatPane extends Component {

    render() {

        let time = parseInt(this.props.time);
        let minutes = Math.floor(time / 60);
        let seconds = time % 60;
        time = "";
        if (minutes > 0) {
            time = minutes + (minutes === 1 ? ' minute ' : ' minutes ');
        }
        time += seconds + (seconds === 1 ? ' second ' : ' seconds ');

        return (
            <div className="cell stat-pane">
                <span className="puzzle-name">{this.props.name}</span>
                <span className="puzzle-time">{time}</span>
            </div>
        )
    }

}