#include "catalog.h"
#include "io_helper.h"

#include <iostream>
#include <iomanip>
#include <fstream>
#include <sstream>
#include <algorithm>

const std::unordered_set<std::string> Catalog::reservedNames = {
    "Remove", "Cart", "Items", "Deals", "Checkout", "Options"
};

void Catalog::addItem(const std::string& name, double price) {
    // Check if the name is in reserved names
    if (reservedNames.find(name) != reservedNames.end()) {
        throw std::runtime_error("Item name '" + name + "' is reserved and cannot be added to the catalog.");
    }

    // Check if item already exists in catalog
    if (itemIdMap.find(name) != itemIdMap.end()) {
        throw std::runtime_error("Item '" + name + "' already exists in the catalog.");
    }

    // Map item name to index in items vector (index also serves as item id), and add item to vector
    int id = items.size();
    items.emplace_back(name, price);
    itemIdMap[name] = id;
}

void Catalog::addDeal(const std::string& names) {
    // Check that string is not empty
    if (names.empty()) {
        throw std::runtime_error("Empty deals may not be added to the catalog.");
    }

    // Get id for this deal (will be index into deals Vector)
    int dealId = deals.size();

    // Initialize new deal
    deals.emplace_back();
    std::vector<int>& deal = deals.back();

    // Iterate over item names separated by commas
    std::stringstream ss(names);
    std::string itemName;
    while (std::getline(ss, itemName, ',')) {
        // Convert item name to Camel Case
        IOHelper::toCamelCase(itemName);

        // Lookup item name in map to retrieve item id
        auto it = itemIdMap.find(itemName);
        if (it == itemIdMap.end()) {
            throw std::runtime_error("Item '" + itemName + "' does not exist in the catalog and may not be included in deals.");
        }

        // Get item object
        const CatalogItem& item = items[it->second];

        // Check if item is already in a deal
        if (item.dealId != -1) {
            throw std::runtime_error("Item '" + itemName + "' is already included in a deal and may not be included again.");
        }

        // Set deal id on item objet
        items[it->second].dealId = dealId;

        // Add item ID to deal set
        deal.push_back(it->second);
    }

    // Sort items within deal by price (highest to lowest)
    std::sort(deal.begin(), deal.end(), [this](int id1, int id2) {
        return items[id1].price > items[id2].price;
    });
}

int Catalog::getItemId(const std::string& itemName) const {
    auto it = itemIdMap.find(itemName);
    if (it == itemIdMap.end()) {
        return -1;
    }

    // Return item
    return it->second;
}

const CatalogItem& Catalog::getItem(int itemId) const {
    if (itemId < 0 || itemId > items.size()) {
        throw std::runtime_error("Error: Item with id '" + std::to_string(itemId) + "' does not exist."); 
    };

    // Return item
    return items[itemId];
}

const std::vector<int>& Catalog::getDeal(int dealId) const {
    if (dealId < 0 || dealId > deals.size()) {
        throw std::runtime_error("Error: Deal with id '" + std::to_string(dealId) + "' does not exist."); 
    };

    // Return item
    return deals[dealId];
}

void Catalog::readItemsFromFile(const std::string& filepath) {
    // Open file
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: '" + filepath + "'. Please ensure it exists.");
    }

    std::string line, itemName, priceStr;
    double itemPrice;

    // Skip first line
    std::getline(file, line);

    // Iterate over lines in file
    while (std::getline(file, line)) {
        std::stringstream ss(line);

        // Read item
        if (!std::getline(ss, itemName, ',')) { 
            throw std::runtime_error("Cannot read an item name in file: '" + filepath +"'.");
        }

        // Read price
        if (!std::getline(ss, priceStr, ',')) {
            throw std::runtime_error("Cannot read price for item: '" + itemName + "' in file: '" + filepath +"'.");
        }

        try {
            // Convert price to double
            itemPrice = std::stod(priceStr);
        } catch (const std::invalid_argument& e) {
            // Handle invalid price conversion
            throw std::runtime_error("Invalid price for item: '" + itemName + "' in file: '" + filepath +"'.");
        } 

        // Convert item name to Camel Case
        IOHelper::toCamelCase(itemName);

        addItem(itemName, itemPrice);
    }
}

void Catalog::readDealsFromFile(const std::string& filepath) {
    // Open file
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: '" + filepath + "'. Please ensure it exists.");
    }

    std::string line;

    // Skip first line
    std::getline(file, line);

    // Iterate over lines in file and add deals
    while (std::getline(file, line)) {
        addDeal(line);
    }
}

void Catalog::printItems() const {
    // Column widths
    const int nameWidth = 25;
    const int priceWidth = 15;
    const int totalWidth = nameWidth + priceWidth;
    
    // Header
    IOHelper::printSolidLine(totalWidth);
    IOHelper::printCentered("Supermarket Items", totalWidth);
    IOHelper::printSolidLine(totalWidth);

    // Column headers
    std::cout << std::setw(nameWidth) << std::left << "Item";
    std::cout << std::setw(priceWidth) << std::right << "Price" << std::endl;
    IOHelper::printDashedLine(totalWidth);

    // Items
    for (const CatalogItem& item : items) {
        // Print name
        std::cout << std::setw(nameWidth) << std::left << item.name;

        // Print price
        std::stringstream priceStream;
        priceStream << "$" << std::fixed << std::setprecision(2) << item.price << " / unit";
        std::cout << std::setw(priceWidth) << std::right << priceStream.str() << std::endl;
    }
    IOHelper::printSolidLine(totalWidth);
}

void Catalog::printDeals() const {
    // Column widths
    const int typeWidth = 6;   
    const int itemsWidth = 60;
    const int totalWidth = typeWidth + itemsWidth;
    
    // Header
    IOHelper::printSolidLine(totalWidth);
    IOHelper::printCentered("Supermarket Deals", totalWidth);
    IOHelper::printSolidLine(totalWidth);

    // Deal types
    IOHelper::printCentered("Deal Types", totalWidth);
    IOHelper::printDashedLine(totalWidth);
    std::cout << "Type A: Buy 2 of this item and get a 3rd free!" << std::endl;
    std::cout << "Type B: Buy any 3 of these items (duplicates allowed) and the" << std::endl;
    std::cout << "        cheapest is free!" << std::endl;
    IOHelper::printDashedLine(totalWidth);
    IOHelper::printCentered("Active Deals", totalWidth);
    IOHelper::printDashedLine(totalWidth);

    // Column headers
    std::cout << std::setw(typeWidth) << std::left << "Type";
    std::cout << std::setw(itemsWidth) << std::right << "Items" << std::endl;
    IOHelper::printDashedLine(totalWidth);

    // Deals
    for (const auto& deal : deals) {
        if (deal.size() > 1) {
            std::cout << std::setw(typeWidth) << std::left << "B";
        } else {
            std::cout << std::setw(typeWidth) << std::left << "A";
        }
        
        std::stringstream itemsStream;
        bool first = true;

        for (int id : deal) {
            if (!first) {
                itemsStream << ", ";
            }
            itemsStream << items[id].name;
            first = false;
        }

        // Print item names
        std::cout << std::setw(itemsWidth) << std::right << itemsStream.str() << std::endl;
    }
    IOHelper::printSolidLine(totalWidth);
}