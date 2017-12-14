import React, {Component} from 'react';

export default class CaptionedImage extends Component {

    render() {

        let classes = "captioned-image";
        if (this.props.className) {
            classes = `${classes} ${this.props.className}`;
        }

        return (
            <div className={classes} style={this.props.style}>
                <img src={this.props.src} alt={this.props.alt}/>
                <p className="image-caption">{this.props.caption}</p>
            </div>
        )

    }
}