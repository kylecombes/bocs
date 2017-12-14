import React, {Component} from 'react';

export default class Header extends Component {

    render() {
        return (
            <header className="header cell grid-x">
                <img src="http://poe.olin.edu/2017/Poezzle/assets/BOCS-logo.svg" className="logo"/>
                <div className="nav-container">
                    <nav>
                        <span className="nav-item" onClick={() => this.props.setPage('landing')}>Home</span>
                        <span className="nav-item" onClick={() => this.props.setPage('mechanical')}>Mechanical</span>
                        <span className="nav-item" onClick={() => this.props.setPage('electrical')}>Electrical</span>
                        <span className="nav-item" onClick={() => this.props.setPage('software')}>Software</span>
                        <span className="nav-item" onClick={() => this.props.setPage('budget')}>Budget</span>
                    </nav>
                </div>
            </header>
        )
    }

}