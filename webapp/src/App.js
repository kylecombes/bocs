import React, { Component } from 'react';
import LandingPage from './landing/landing-page';
import Footer from './footer/footer';
import './App.css';
import Mechanical from "./development/mechanical";
import Electrical from "./development/electrical";
import Software from "./development/software";
import Header from "./header";
import Budget from "./development/budget";

export default class App extends Component {

    constructor() {
        super();

        this.state = {
            page: 'landing'
        };

        this.changePage = this.changePage.bind(this);
    }

    changePage(page) {
        this.setState({page});
    }

    render() {

        let content = null;

        switch (this.state.page) {
            case 'landing':
                content = <LandingPage/>;
                break;
            case 'mechanical':
                content = <Mechanical/>;
                break;
            case 'electrical':
                content = <Electrical/>;
                break;
            case 'software':
                content = <Software/>;
                break;
            case 'budget':
                content = <Budget/>;
                break;
        }

        return (
            <div className="App">
                <Header setPage={this.changePage}/>
                {content}
                <Footer/>
            </div>
        );
    }
}
