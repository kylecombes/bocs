import React, { Component } from 'react';
import LandingPage from './landing/landing-page';
import Footer from './footer/footer';
import './App.css';

export default class App extends Component {
    render() {
        return (
            <div className="App">
                <LandingPage/>
                <Footer/>
            </div>
        );
    }
}
