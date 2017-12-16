import React, {Component} from 'react';

export default class SketchfabCAD extends Component {

    render() {
        let src = this.props.id ? `https://sketchfab.com/models/${this.props.id}/embed` : null;
        return (
            <div className="sketchfab-container cell large-6 medium-12">
                <iframe src={src} allowFullScreen={true} />
            </div>
        )
    }

}