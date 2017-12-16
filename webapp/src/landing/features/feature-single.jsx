import React, {Component} from 'react';

export default class Feature extends Component {

    render() {
        return (
            <div className="cell feature large-3 medium-6">
                <img className="feature-icon" src={this.props.icon}/>
                <p className="feature-title">{this.props.title}</p>
                <p className="feature-description">{this.props.description}</p>
            </div>
        )
    }

}