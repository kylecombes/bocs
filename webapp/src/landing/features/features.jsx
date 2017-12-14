import React, {Component} from 'react';
import Feature from './feature-single';
import * as featureData from './features.json';

export default class LandingPageFeatures extends Component {

    constructor() {
        super();

        this.state = {
            features: featureData.data,
        }
    }

    render() {

        let feats = [];
        for (let i in this.state.features) {
            const feature = this.state.features[i];
            feats.push(<Feature key={feature.title} title={feature.title} icon={feature.icon} description={feature.description} />);
        }


        return (
            <section className="landing-page-section cell features">
                <div className="landing-page-section-content cell grid-x">
                    <h1 className="landing-section-title features-title">Features</h1>
                    <p className="cell feature-list grid-x">
                        {feats}
                    </p>
                </div>
            </section>
        )
    }
}