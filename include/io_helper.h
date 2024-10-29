#ifndef IO_HELPER_H
#define IO_HELPER_H

#include <string>
#include <iostream>

/**
 * Helper class containing static methods to format input and output.
*/
class IOHelper {
public:
    /**
     * Prints a string centered in a specified total width.
     * @param str The string.
     * @param totalWidth The width to center the string in.
     * @param out The output stream to print the stirng to.
    */
    static void printCentered(const std::string& str, int totalWidth, std::ostream& out = std::cout);

    /**
     * Prints a solid line of a specified length.
     * @param length The length of the line to be printed.
     * @param out The output stream to print the line to.
    */
    static void printSolidLine(int length, std::ostream& out = std::cout);

    /**
     * Prints a dashed line of a specified length.
     * @param length The length of the line to be printed.
     * @param out The output stream to print the line to.
    */
    static void printDashedLine(int length, std::ostream& out = std::cout);

    /**
     * Converts an input string to Camel Case in place by capitalizing the first character
     * in the string and after spaces.
     * @param str The input string.
    */
    static void toCamelCase(std::string& str);

    /**
     * Converts a string to an int using std::stoi. Checks that
     * the full string is numeric and convertable to an int, else throws an exception.
     * @param str The input string.
    */
    static int fullStoi(const std::string& str);
};

#endif