import React, {Component} from 'react';

export default class DevelopmentSprintRow extends Component {

    render() {
        return (
            <div className="sprint cell">
                <div className="sprint-title-container">
                    <p className="sprint-title">Sprint {this.props.number}</p>
                </div>
                {this.props.mechanical ? <div className="sprint-col mechanical">{this.props.mechanical}</div> : null}
                {this.props.electrical ? <div className="sprint-col electrical">{this.props.electrical}</div> : null}
                {this.props.software ? <div className="sprint-col software">{this.props.software}</div> : null}
            </div>
        )
    }

}