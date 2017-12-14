import React, {Component} from 'react';
import MechSprint1 from "./mechanical";
import ElecSprint1 from "./electrical";
import SoftwareContent from "./software";
// import DevelopmentSprintRow from "./sprint";
import Budget from "./budget";



export default class Development extends Component {

    constructor() {
        super();

        // this.sprints = [
        //     {
        //         number: 1,
        //         content: {
        //             mech: <MechSprint1/>,
        //             elec: <ElecSprint1/>,
        //             sw: <SWSprint1/>,
        //         }
        //     }
        // ];
        //
        // this.state = {
        //     showingMechanical: true,
        //     showingElectrical: true,
        //     showingSoftware: true,
        // }
    }


    render() {

        const rows = [];

        // this.sprints.forEach((sprint) => {
        //     const mech = this.state.showingMechanical ? sprint.content.mech : null;
        //     const elec = this.state.showingElectrical ? sprint.content.elec : null;
        //     const sw = this.state.showingSoftware ? sprint.content.sw : null;
        //     rows.push(<DevelopmentSprintRow number={sprint.number} mechanical={mech} electrical={elec} software={sw}/>)
        // });

        return (
            <section className="landing-page-section cell development">
                <div className="landing-page-section-content">
                    <h1 className="landing-section-title light">Development</h1>
                    <SoftwareContent/>
                    <Budget/>
                </div>
            </section>
        )
    }

}