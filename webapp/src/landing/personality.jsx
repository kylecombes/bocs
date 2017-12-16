import React, {Component} from 'react';

export default class Personality extends Component {

    render() {
        return (
            <section className="landing-page-section cell personality">
                <div className="landing-page-section-content cell grid-x">
                    <h1 className="landing-section-title">Personifiable</h1>
                    <div className="personality-content-container cell grid-x">
                        <div className="personality-img-container cell large-5">
                            <img src="http://poe.olin.edu/2017/Poezzle/assets/BOCS-AI.jpg"/>
                        </div>
                        <div className="personality-content cell large-7">
                            <h1 className="personality-title">Meet BOCS</h1>
                            <h2 className="personality-subtitle">Your new intellectual opponent</h2>
                            It may seem like BOCS thinks it’s smarter than you. That’s because it is. When a bombastic
                            personality is trapped in a 14” cube, it’s bound to get a bit agitated. Nevertheless, the
                            BOCS is here to actively guide you through our six challenging puzzles with its snarky
                            computer-generated voice. If you can solve its puzzles, it might just show you some respect.
                        </div>
                    </div>
                    <p className="personality-example">Or create your own persona</p>
                </div>
            </section>
        )
    }

}