#ifndef CATALOG_H
#define CATALOG_H

#include "catalog_item.h"

#include <unordered_map>
#include <unordered_set>
#include <vector>
#include <string>

/**
 * The Supermarket items catalog which stores item prices and deals.
*/
class Catalog {
    private:
        /**
         * Maps item name strings to their item id.
         * An item's id is its index into the items vector.
        */
        std::unordered_map<std::string, int> itemIdMap;

        /**
         * Vector of all items in the Supermarket. Items are indexed using their item id.
        */
        std::vector<CatalogItem> items;

        /**
         * Vector of item deals. The deals are represented as vectors of the item id's in the deal, 
         * ordered highest to lowest by price. The deals are indexed using their deal id.
        */
        std::vector<std::vector<int>> deals;

        /**
         * A set of keywords that cannot be used as item names because they are used as commands
         * for reading user input.
        */
        static const std::unordered_set<std::string> reservedNames;

        /**
         * Adds an item to the catalog.
         * @param name The item name.
         * @param price The item price in USD.
        */
        void addItem(const std::string& name, double price);

        /**
         * Adds a deal for a set of one or more items to the catalog.
         * @param names A string of item names separated by commas. 
         * Each item must already be present in the catalog, and must not be included in any 
         * other deals for the deal to add succesfully.
        */
        void addDeal(const std::string& itemNames);


    public:
        /**
         * Gets an item id using its name.
         * @param itemName The item name.
         * @returns The item id.
        */
        int getItemId(const std::string& itemName) const;

        /**
         * Gets an item object using its id
         * @param itemId The item id.
         * @returns The item object.
        */
        const CatalogItem& getItem(int itemId) const;

        /**
         * Gets a deal set based on the deal id.
         * @param dealId The deal id.
         * @returns A vector containing the ids of all the items in the deals, ordered highest
         * to lowest by item price. 
        */
        const std::vector<int>& getDeal(int dealId) const;

        /**
         * Reads items and prices from a csv file and adds them to the catalog.
         * @param filepath The path to the file.
        */
        void readItemsFromFile(const std::string& filepath);

        /**
         * Reads deal item sets from a csv file and adds them to the catalog.
         * @param filepath The path to the file.
        */
        void readDealsFromFile(const std::string& filepath);

        /**
         * Prints catalog items to the standard output.
        */
        void printItems() const;

        /**
         * Prints catalog deals to the standard output.
        */
        void printDeals() const;
};

#endif