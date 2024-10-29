#ifndef CATALOG_ITEM_H
#define CATALOG_ITEM_H

#include <string>

/**
 * An item available for purchase in the Supermarket.
*/
struct CatalogItem {
    /**
     * Item name.
    */
    std::string name;

    /**
     * Item price in USD.
    */
    double price;

    /**
     * The id of the deal that applies to this item.
     * If no deals apply, this is set to -1.
    */
    int dealId;

    /**
     * Instantiates a catalog item.
     * @param name The item name.
     * @param price The item price in USD.
    */
    CatalogItem(const std::string& name, double price);
};

#endif