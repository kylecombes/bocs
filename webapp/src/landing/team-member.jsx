import React, {Component} from 'react';


export default class TeamMember extends Component {

    render() {
        const name = <p className="team-member-name">{this.props.name}</p>;
        const nameElem = this.props.website
            ? <a href={this.props.website} target="_blank">{name}</a>
            : name;
        return (
            <div className="team-member medium-6 large-4 cell">
                <img className="team-member-image" alt={this.props.name} src={this.props.image}/>
                {nameElem}
                <p className="team-member-title">{this.props.title}</p>
            </div>
        )
    }
}