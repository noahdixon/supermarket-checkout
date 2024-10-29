
#include "checkout_register.h"
#include "io_helper.h"

#include <chrono>
#include <iomanip>
#include <ctime>
#include <sstream>

CheckoutRegister::CheckoutRegister(const Catalog& catalog) : catalog(catalog) {}

void CheckoutRegister::scanItem(const std::string& itemName, int quantity) {
    // Check quantity is valid
    if (quantity < 1) {
        throw std::runtime_error("Item quantity for item '" + itemName + "' must be an integer larger than 0.");
    }

    // Get item id and ensure item exists
    int itemId = catalog.getItemId(itemName);
    if (itemId == -1) {
        throw std::runtime_error("Item '" + itemName + "' does not exist in Supermarket.");
    }

    // Check if item has already been added to cart, if so update quantity and return
    auto it = quantityOfCartItem.find(itemId);
    if (it != quantityOfCartItem.end()) {
        it->second += quantity; 
        return;
    }

    // New item is being added, add id to cart and quantity map
    cartIds.push_back(itemId);
    quantityOfCartItem[itemId] = quantity;

    // Check if item may be eligible for deal, and add deal to potential deals
    const CatalogItem& item = catalog.getItem(itemId);
    if (item.dealId != -1) {
        potentialDeals.insert(item.dealId);
    }
}

void CheckoutRegister::removeItem(const std::string& itemName) {
    // Get item id and ensure item exists
    int itemId = catalog.getItemId(itemName);
    if (itemId == -1) {
        throw std::runtime_error("Item '" + itemName + "' does not exist in Supermarket.");
    }

    // Check that item is in cart
    auto it = quantityOfCartItem.find(itemId);
    if (it == quantityOfCartItem.end()) {
        throw std::runtime_error("Item '" + itemName + "' is not currently in your cart.");
    }

    // Remove item from cartIds, and cartQuantities
    cartIds.remove(itemId);
    quantityOfCartItem.erase(itemId);
}

void::CheckoutRegister::printCart() {
    // Column widths
    const int nameWidth = 26;   
    const int quantityWidth = 8;
    const int totalWidth = nameWidth + quantityWidth;
    
    // Header
    IOHelper::printSolidLine(totalWidth);
    IOHelper::printCentered("Your Cart", totalWidth);
    IOHelper::printSolidLine(totalWidth);

    // Items header
    std::cout << std::setw(nameWidth) << std::left << "Item";
    std::cout << std::setw(quantityWidth) << std::right << "Quantity" << std::endl;
    IOHelper::printDashedLine(totalWidth);

    // Iterate over items in cart
    for (const int& itemId : cartIds) {
        // Print name
        std::cout << std::setw(nameWidth) << std::left << catalog.getItem(itemId).name;
        //Print quantity
        std::cout << std::setw(quantityWidth) << std::right << quantityOfCartItem[itemId] << std::endl;
    }
    IOHelper::printSolidLine(totalWidth);
}

void CheckoutRegister::calculateDeals() {
    // Iterate over potential deals
    for (const int& dealId : potentialDeals) {

        // Get deal from catalog
        const auto& deal = catalog.getDeal(dealId);

        // Array to store current deal group
        std::array<int, 3> curGroup = {-1, -1, -1};

        // Track current index in current group
        int curIndex = 0;

        // Iterate over ids in deal
        for (const int& itemId : deal) {
            // Check that item is included in cart and if so get quantity, if not continue to next item
            auto it = quantityOfCartItem.find(itemId);
            if (it == quantityOfCartItem.end()) {
                continue;
            }
            int quantity = it->second;

            while (quantity > 0) {
                // Fill current group with item
                while (curIndex < 3 && quantity > 0) {
                    curGroup[curIndex++] = itemId; 
                    quantity--;
                }

                // If the current group is full, add it to the deal groups and reset
                if (curIndex == 3) {
                    dealGroups.push_back(curGroup);
                    // Reset group and index
                    curGroup.fill(-1);
                    curIndex = 0;
                }
            }
        }

        // Set quantities of all items in deal to 0
        for (const int& itemId : deal) {
            auto it = quantityOfCartItem.find(itemId);
            if (it != quantityOfCartItem.end()) {
                it->second = 0;
            }
        }

        // Update quantity for remaining 1-2 items that were not included in a deal
        while (curIndex > 0) {
            int last_item = curGroup[--curIndex];
            quantityOfCartItem[last_item]++;
        }
    }
}


void CheckoutRegister::printReceipt(std::ostream& out) {
    double total = 0;

    // Column widths
    const int itemWidth = 30;   
    const int priceWidth = 10;
    const int totalWidth = itemWidth + priceWidth;
    
    // Receipt header section
    IOHelper::printSolidLine(totalWidth, out);
    IOHelper::printCentered("Supermarket", totalWidth, out);
    IOHelper::printCentered("Customer Receipt", totalWidth, out);
    
    // Get current time
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);

    // Format date and time
    std::tm* localTime = std::localtime(&now_time);
    std::ostringstream dateTimeStream;
    dateTimeStream << std::put_time(localTime, "%Y-%m-%d %H:%M");

    // Print date and time
    IOHelper::printCentered(dateTimeStream.str(), totalWidth, out);
    IOHelper::printSolidLine(totalWidth, out);

    if (dealGroups.size() > 0) {
        // Deals section
        // Track total savings
        double savings = 0;

        // Deal header
        IOHelper::printCentered("Deals", totalWidth, out);
        IOHelper::printSolidLine(totalWidth, out);
        out << std::setw(itemWidth) << std::left << "Item";
        out << std::setw(priceWidth) << std::right << "Price" << std::endl;
        IOHelper::printDashedLine(totalWidth, out);
        
        // Iterate over all deals groups
        for (const std::array<int,3>& group : dealGroups) {
            // Iterate over each item in group
            for (int i = 0; i < 3; i++) {
                // Get item data
                const CatalogItem& item = catalog.getItem(group[i]);
                double price = item.price;
                std::string quantity = " (1)";

                // Check if first and second items are the same
                if (i == 0 && group[0] == group[1]) {
                    // Double price and quantity
                    price *= 2;
                    quantity = " (2)";
                    // Skip second item
                    i++;
                }

                // Print name
                out << std::setw(itemWidth) << std::left << item.name + quantity;
                
                // Print price, checking if free
                if (i == 2) {
                    // Last item in group is free
                    out << std::setw(priceWidth) << std::right << "FREE" << std::endl;
                    savings += price;
                } else {
                    std::ostringstream priceStream;
                    priceStream << "$" << std::fixed << std::setprecision(2) << price;
                    out << std::setw(priceWidth) << std::right << priceStream.str() << std::endl;
                    total += price;
                }
            }
            IOHelper::printDashedLine(totalWidth, out);
        }
        
        // Print savings
        out << "You saved " << "$" << std::fixed << std::setprecision(2) << savings << "!" << std::endl;
        IOHelper::printSolidLine(totalWidth, out);

        // Print items header
        IOHelper::printCentered("Remaining Items", totalWidth, out);
        IOHelper::printSolidLine(totalWidth, out);
    } else {
        // Print items header
        IOHelper::printCentered("Items", totalWidth, out);
        IOHelper::printSolidLine(totalWidth, out);
    }

    // Items section
    out << std::setw(itemWidth) << std::left << "Item";
    out << std::setw(priceWidth) << std::right << "Price" << std::endl;
    IOHelper::printDashedLine(totalWidth, out);

    // Iterate over items in cart
    for (const int& itemId : cartIds) {    
        // Get item quantity, skip if item has quantity 0 (due to being fully included in deals)
        auto it = quantityOfCartItem.find(itemId);
        if (it == quantityOfCartItem.end() || it->second == 0) {
            continue;
        }

        // Get quantity and item data
        int quantity = it->second;
        const auto& item = catalog.getItem(itemId);

        // Print name
        out << std::setw(itemWidth) << std::left << item.name + " (" + std::to_string(quantity) + ") ";

        // Print price
        double price = quantity * item.price;
        std::ostringstream priceStream;
        priceStream << "$" << std::fixed << std::setprecision(2) << price;
        out << std::setw(priceWidth) << std::right << priceStream.str() << std::endl;
        total += price;
    }
    IOHelper::printSolidLine(totalWidth, out);

    // Total section
    out << std::setw(itemWidth) << std::left << "Grand Total:";
    std::ostringstream totalStream;
    totalStream << "$" << std::fixed << std::setprecision(2) << total;
    out << std::setw(priceWidth) << std::right << totalStream.str() << std::endl;

    IOHelper::printSolidLine(totalWidth, out);
    IOHelper::printCentered("Thank you for shopping with us!", totalWidth, out);
    IOHelper::printSolidLine(totalWidth, out);
}

void CheckoutRegister::clearSession() {
    cartIds.clear();
    quantityOfCartItem.clear();
    potentialDeals.clear();
    dealGroups.clear();
}

void CheckoutRegister::checkOut(std::ostream& receiptOutStream) {
    calculateDeals();
    printReceipt(receiptOutStream);
    clearSession();
}
