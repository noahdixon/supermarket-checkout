#ifndef CHECKOUT_REGISTER_H
#define CHECKOUT_REGISTER_H

#include "catalog.h"

#include <iostream>
#include <unordered_map>
#include <vector>
#include <list>
#include <string>
#include <set>
#include <array>

/**
 * A Supermaket register used to handle checkout. 
 * Scans user items in their cart, calculates maximum deals, and prints user receipts.
*/
class CheckoutRegister {
    private:
        /**
         * Reference to the catalog.
        */
        const Catalog& catalog;

        /**
         * List of ids of user scanned items.
        */
        std::list<int> cartIds;

        /**
         * Maps user scanned item id's to the desired quantity of that item.
        */
        std::unordered_map<int, int> quantityOfCartItem;
        
        /**
         * Stores a set of deal ids that may be applicable based on the scanned items.
        */
        std::set<int> potentialDeals;

        /**
         * After calculateDeals() is called, stores groups of items that form deals,
         * represented as arrays of the item ids.
        */
        std::list<std::array<int,3>> dealGroups;

        /**
         * Calculates which items should be grouped together to maximize customer savings,
         * and stores these groups as arrays of item ids in dealGroups.
        */
        void calculateDeals();

        /**
         * Prints the receipt for the customer session to an output stream.
         * @param out the output stream.
        */
        void printReceipt(std::ostream& out);

        /**
         * Clears all state sepecific to a customer session.
        */
        void clearSession();

    public:
        /**
         * Instantiates a checkout register.
         * @param catalog The catalog for the register to reference items and deals.
        */
        CheckoutRegister(const Catalog& catalog);

        /**
         * Scans an item of some quantity into a user's cart.
         * @param itemName The name of the desired item.
         * @param quantity The desired item quantity.
        */
        void scanItem(const std::string& itemName, int quantity);

        /**
         * Removes an item from a users cart.
         * @param itemName The name of the item.
        */
        void removeItem(const std::string& itemName);

        /**
         * Prints a users cart to the standard output
        */
        void printCart();

        /**
         * Calculates maximum deal groups, prints a user's receipt,
         * and clears all cart state from the register.
         * @param receiptOutStream The output stream for the receipt to be printed to.
        */
        void checkOut(std::ostream& receiptOutStream = std::cout);
};

#endif