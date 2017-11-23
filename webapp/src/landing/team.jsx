import React, {Component} from 'react';
import * as teamData from './team-member-info.json';
import TeamMember from "./team-member";


export default class Team extends Component {

    render() {
        const members = [];
        teamData.members.forEach((member) => {
            members.push(<TeamMember
                key={member.name}
                name={member.name}
                website={member.website}
                title={member.title}
                image={member.image}
            />)
        });
        return (
            <section className="landing-page-section cell team">
                <div className="landing-page-section-content cell grid-x">
                    <h1 className="landing-section-title team-title">Team</h1>
                    <div className="team-members grid-x cell">
                        {members}
                    </div>
                </div>
            </section>
        )
    }
}