#include "catalog.h"
#include "checkout_register.h"
#include "io_helper.h"

#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <filesystem>

/**
 * Prints a welcome message to a user.
*/
void printWelcome() {
    IOHelper::printSolidLine(55);
    IOHelper::printCentered("Welcome to Supermarket Checkout Simulator!", 55);
    IOHelper::printSolidLine(55);
}

/**
 * Prints input options to a user.
*/
void printOptions() {
    std::cout << "- Scan items by typing an item name below followed by a single space and the quantity of item." << std::endl;
    std::cout << "- To remove an item from your cart type 'remove <item>' (removes all quantites of item)." << std::endl;
    std::cout << "- To view the items currently in your cart, type 'cart'." << std::endl;
    std::cout << "- To view a complete list of available items in the Supermarket, type 'items'." << std::endl;
    std::cout << "- To view a complete list of available deals in the Supermarket, type 'deals'." << std::endl;
    std::cout << "- If you are finished entering items, type 'checkout' to print your receipt." << std::endl;
    std::cout << "- To repeat these options, type 'options'." << std::endl;
}

/**
 * Asks a user to sequentially enter items with quantity via the command line
 * and scans these items into the register until the user is ready to checkout.
 * @param catalog The Supermarket catalog.
 * @param checkoutRegister The catalog register maintaining the user's cart.
*/
void promptUserForInput(const Catalog& catalog, CheckoutRegister& checkoutRegister) {
    printWelcome();
    printOptions();
    std::string input;
    while (true) {
        // Read input and set case
        std::cout << "> ";
        std::getline(std::cin, input);
        IOHelper::toCamelCase(input);
        // Break if user is ready for checkout
        if (input == "Checkout") {
            break;
        }
        // Show options
        if (input == "Options") {
            printOptions();
            continue;
        }
        // Show items or deals
        if (input == "Items") {
            catalog.printItems();
            continue;
        }
        if (input == "Deals") {
            catalog.printDeals();
            continue;
        }
        // Show cart
        if (input == "Cart") {
            checkoutRegister.printCart();
            continue;
        }
        // Remove item
        if (input.find("Remove ") == 0) {
            std::string itemName = input.substr(7);
            try {
                checkoutRegister.removeItem(itemName);
            } catch (const std::runtime_error& e) {
                std::cerr << "Error: " << e.what() << std::endl;
            }
            continue;
        }

        // Scan item and quantity
        // Find last space in input to separate item name and quantity
        std::size_t lastSpacePos = input.find_last_of(' ');
        if (lastSpacePos == std::string::npos) {
            std::cout << "Error: Invalid input. Please enter in the format '<item> <quantity>' or use 'remove <item>'." << std::endl;
            continue;
        }
        std::string itemName = input.substr(0, lastSpacePos);
        try {
            // Try parsing quantity and converting to int
            std::string quantityStr = input.substr(lastSpacePos + 1);
            int quantity = IOHelper::fullStoi(quantityStr);
            // Scan item
            checkoutRegister.scanItem(itemName, quantity);

        } catch (const std::invalid_argument& e) {
            std::cerr << "Error: Invalid quantity. Please enter a valid integer larger than 0." << std::endl;
        }  catch (const std::out_of_range& e) {
            std::cerr << "Error: Quantity out of range." << std::endl; 
        } catch (const std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
        }
    }
}

/**
 * Reads items from a csv file and scans them into the register.
 * @param checkoutRegister The catalog register maintaining the cart.
 * @param filepath The filepath to the csv file.
*/
void readFileInput(CheckoutRegister& checkoutRegister, const std::string& filepath) {
    // Open file
    std::ifstream file(filepath);
    if (!file.is_open()) {
        throw std::runtime_error("Cannot open file: '" + filepath + "'. Please ensure it exists.");
    }

    std::string line;
    // Skip first line
    std::getline(file, line);

    // Iterate over lines in file and scan items
    while (std::getline(file, line)) {
        std::stringstream ss(line);
        std::string itemName;
        std::string quantityStr;
        std::getline(ss, itemName, ',');
        std::getline(ss, quantityStr, ',');

        try {
            // Try parsing quantity and converting to int
            int quantity = IOHelper::fullStoi(quantityStr);
            // Scan item
            checkoutRegister.scanItem(itemName, quantity);

        } catch (const std::invalid_argument& e) {
            throw std::runtime_error("Invalid quantity for item: '" + itemName + "' in file: '" + filepath +"'.");
        } catch (const std::out_of_range& e) {
            throw std::runtime_error("Invalid quantity for item: '" + itemName + "' in file: '" + filepath +"'.");
        } catch (const std::runtime_error& e) {
            throw std::runtime_error("Issue in input file: '" + filepath +"': " + e.what());
        }
    }    
}

/**
 * Entry point to program.
*/
int main(int argc, char* argv[]) {

    // Parse command line arguments
    bool isFileInput = false;
    bool isFileOutput = false;
    for (int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        // Check for input flag
        if (arg == "-i") {
            isFileInput = true;
        // Check for output flag
        } else if (arg == "-o") {
            isFileOutput = true;
        } else {
            std::cerr << "Error: " << "Unknown argument passed: " << arg << std::endl;
            return 1;
        }
    }

    // Initialize catalog, read items, and read deals
    Catalog catalog;
    try {
        catalog.readItemsFromFile("data/items.csv");
        catalog.readDealsFromFile("data/deals.csv");
    } catch (const std::runtime_error& e) {
        std::cerr << "Error: " << e.what() << std::endl;
        return 1;
    }

    // Initialize checkout register
    CheckoutRegister checkoutRegister(catalog);

    // Scan items
    if (isFileInput) {
        // Try scanning items from input file
        try {
            readFileInput(checkoutRegister, "input/shopping_list.csv");
        } catch (const std::runtime_error& e) {
            std::cerr << "Error: " << e.what() << std::endl;
            return 1;
        }
    } else {
        // Prompt user to scan items manually
        promptUserForInput(catalog, checkoutRegister);
    }

    // Checkout
    if (isFileOutput) {
        // Create output directory if it does not exist
        std::filesystem::create_directory("output");

        // Clear or create receipt file
        std::ofstream file("output/receipt.txt");

        // Check if file was successfully open
        if (!file.is_open()) {
            std::cerr << "Error: Could not create or open file: 'output/receipt.txt'." << std::endl;
            return 1;
        }

        // Print receipt to file
        checkoutRegister.checkOut(file);
    } else {
       // Print receipt to console
        checkoutRegister.checkOut();
    }
}