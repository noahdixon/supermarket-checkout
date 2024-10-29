#include "io_helper.h"

void IOHelper::printCentered(const std::string& str, int totalWidth, std::ostream& out) {
    // If string is longer than width, just print string
    if (str.length() >= totalWidth) {
        out << str << std::endl;
    }
    
    // Calculate left padding
    int padding = (totalWidth - str.length()) / 2;
    // Print padding spaces and string
    out << std::string(padding, ' ') << str << std::endl;
}

void IOHelper::printSolidLine(int length, std::ostream& out) {
    out << std::string(length, '-') << std::endl;
}

void IOHelper::printDashedLine(int length, std::ostream& out) {
    int iters = length / 2;
    for (int i = 0; i < iters; ++i) {
        out << "- ";
    }
    out << std::endl;
}

void IOHelper::toCamelCase(std::string& str) {
    // Capitalize first character
    bool capitalizeNext = true;  
    for (int i = 0; i < str.size(); ++i) {
        if (std::isspace(str[i])) {
            // Set flag to capitalize after space
            capitalizeNext = true; 

        } else if (capitalizeNext) {

            // Capitalize current character
            str[i] = std::toupper(str[i]);

            // Reset flag
            capitalizeNext = false; 
        } else {
            // Lowercase current character
            str[i] = std::tolower(str[i]); 
        }
    }
}

int IOHelper::fullStoi(const std::string& str) {
    std::size_t pos = 0;
    int num = std::stoi(str, &pos);

    // Check if string was not fully parsed due to non numeric characters
    if (pos != str.length()) {
        throw std::invalid_argument("String '" + str + "' cannot be converted to an int.");
    }
    return num;
}