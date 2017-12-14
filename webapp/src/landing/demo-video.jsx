import React, {Component} from 'react';

export default class DemoVideo extends Component {

    render() {
        return (
            <section className="landing-page-section cell demo-video">
                <div className="landing-page-section-content cell grid-x">
                    <h1 className="landing-section-title">Demo</h1>
                    <div className="cell" style={{textAlign: 'center'}}>
                    <iframe
                        width="700"
                        height="394"
                        src="https://www.youtube.com/embed/rxg3Dh6ItP4?showinfo=0"
                        frameBorder="0"
                        gesture="media"
                        allow="encrypted-media"
                        allowFullScreen={true}/>
                    </div>
                </div>
            </section>
        )
    }

}