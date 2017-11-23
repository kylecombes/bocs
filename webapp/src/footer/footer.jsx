import React, {Component} from 'react';
import * as poweredByTech from './powered-by.json';

export default class Footer extends Component {

    render() {
        const poweredBy = [];
        poweredByTech.technologies.forEach((t) => {
            poweredBy.push(
                <div className="cell large-auto medium-6">
                    <a alt={t.name} href={t.website} target="_blank">
                        <img src={t.logo} alt={t.name}/>
                    </a>
                </div>
            );
        });

        return (
            <footer className="cell">
                <div className="footer-content">
                    <h1 className="powered-by">Powered By</h1>
                    <div className="technologies cell grid-x">
                        {poweredBy}
                    </div>
                </div>
            </footer>
        )
    }

}