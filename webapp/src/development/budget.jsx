import React, {Component} from 'react';
import * as items from './budget-items.json';

export default class Budget extends Component {

    constructor() {
        super();

        this.state = {
            items: items.data
        }
    }

    render() {

        let budgetItems = [];

        // Add header
        budgetItems.push(
            <div className="budget-header" key="header">
                <span className="name">Item</span>
                <span className="unit-cost">Unit Cost</span>
                <span className="quantity">Quantity</span>
                <span className="shipping">Shipping Cost</span>
                <span className="from-budget">From Budget</span>
                <span className="total-cost">Total Cost</span>
            </div>
        );

        let runningTotal = 0;
        let budgetCost = 0;

        for (let i in this.state.items) {
            let item = this.state.items[i];
            const totalCost = item.unitCost * item.quantity + item.shippingCost;
            runningTotal += totalCost;
            if (item.fromBudget) {
                budgetCost += totalCost;
            }
            budgetItems.push(
                <div className="budget-item" key={item.name}><a href={item.url} alt={item.name} target="_blank"><span className="name">{item.name}</span></a>
                    <span className="unit-cost">${item.unitCost}</span>
                    <span className="quantity">{item.quantity}</span>
                    <span className="shipping">${item.shippingCost}</span>
                    <span className="from-budget">{item.fromBudget ? 'Yes' : 'No'}</span>
                    <span className="total-cost">${item.fromBudget ? totalCost.toFixed(2) : '0'}</span>
                </div>
            )
        }

        budgetItems.push(
            <div className="budget-item" key="total"><span className="name">Total</span>
                <span className="unit-cost"></span>
                <span className="quantity"></span>
                <span className="shipping"></span>
                <span className="from-budget">${budgetCost.toFixed(2)}</span>
                <span className="total-cost">${runningTotal.toFixed(2)}</span>
            </div>
        );

        return (
            <section className="page budget">
                <h1 className="landing-section-title light">Budget</h1>
                <p>At the outset, Olin provided us with a $250 budget for materials we needed to purchase. We managed to
                    stay completely under this threshold and refrain from spending any money out of pocket. However, if
                    you include the value of parts we were able to obtain for free (from ourselves or other sources), the
                    total valuation of the BOCS does exceed $250.</p>
                {budgetItems}
            </section>
        )
    }

}