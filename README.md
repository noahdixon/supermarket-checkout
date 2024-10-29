# Supermarket Checkout
#### A C++ program for managing user checkout processes at a fictional supermarket.

## Table of Contents
- [Overview](#overview)
- [Requirements](#requirements)
- [Getting Started](#getting-started)
  - [Run on Mac](#run-on-mac)
  - [Run on Windows](#run-on-windows)
  - [Troubleshooting](#troubleshooting)
- [User Instructions](#getting-started)
  - [Market Configuration](#market-configuration)
  - [Interacting with the Program](#interacting-with-the-program)
  - [Command Line Arguments](#command-line-arguments)
- [Design Considerations](#design-considerations)
  - [Maximum Savings Algorithm](#maximum-savings-algorithm)
  - [Market Structure](#market-structure)
- [Future Considerations](#future-considerations)

## Overview
The Supermarket is a fictional market that offers a variety of items and two types of deals. This program enables users to scan items at the Supermarket and generates an itemized receipt upon checkout. Additionally, the program automatically identifies the optimal deals for a user's cart to maximize their discount savings.

## Requirements
- **C++17**: The source files for this project are intended to be compiled using C++17.

## Getting Started
### Run on Mac
A makefile is included in the root directory to compile the source files.
1. Run `make` from the root directory. This will compile the source files and place the program executable in /bin.
2. Run `bin/supermarket_checkout` to start the program. Be sure to reference the [User Instructions](#user-instructions) section for an overview of how to use the program.
3. Run `make clean` at any time to remove object files and the executable.

### Run on Windows
I was able to compile the program on a windows machine by following these steps:

1. Install MSYS2 with the g++ compiler.
2. Open a command prompt, navigate to the root directory, and run `g++ -std=c++17 -Iinclude src/*.cpp -o supermarket_checkout`. This will compile the source files and create an executable.
3. Run `supermarket_checkout.exe` to start the program. Be sure to reference the [User Instructions](#user-instructions) section for an overview of how to use the program.

### Troubleshooting
If you run into any issues trying to compile or run the program please do not hesitate to reach out to me at noah24dixon@gmail.com or (508)-635-7875.

## User Instructions
### Market Configuration
The program reads from the items.csv and deals.csv files stored in the /data directory to initialize the items and deals stored in the Supermarket. These files can be modified to change items or deals between executions of the program. 
- Format of items.csv: Each item should be on an individual line in the format `itemName,price`.
- Format of deals.csv: Each deal should be on an individual line in the format `item1,item2,item3` with at least one item per deal.

### Interacting with the Program
After initializing the market, the program will prompt the user to enter items via the command line in the format `item quantity` to scan them into their cart. The following commands are also available:
- Remove Item: Enter `remove itemName` to remove an item from the cart.
- View Cart: Enter `cart` to see a list of all the items currently in the cart.
- View Items: Enter `items` to see a list of all items available in the market.
- View Deals: Enter `deals` to view a list of all deals available in the market.
- Checkout: Enter `checkout` to proceed to checkout.
- View Options: Enter `options` to view a list of these options.

After `checkout` is entered, the program will print an itemized receipt to the console. The receipt will list the items included in each deal group separately from other items, inform a user of their savings, and give them a grand total.

### Command Line Arguments
The program can also optionally read input and write output via files rather than the command line using these command line arguments:
- `-i` read input items via the shopping_list.csv file located in the /input directory, bypassing the user prompting stage.
- `-o` write the receipt output to a receipt.txt file located in the /output directory rather than to the console.

These arguments can be called separately or together, for example: `./supermarket_checkout -i -o`.

## Design Considerations
The following sections describe how I approached two major design challenges in this project: determining which deals to apply to a cart of items during checkout, and effectively structuring the market for efficient access and management.

### Maximum Savings Algorithm
The assignment states: Some items are eligible for discounts based on the following 2 deal types: 
1. buy 3 identical items and pay for 2
2. buy 3 (in a set of items) and the cheapest is free

The description of the second type of deal is a bit ambiguous, as it could be interpreted as "buy any 3 unique items from a set of items" or "buy any 3 items, allowing duplicates, from a set of items". I chose to interpret it as the latter, allowing duplicate items in a deal group. I also chose to allow the deals themselves (the sets of items that may be used together in deal groups) to be as small or large as a user wants, and I chose to disallow items from being included in more than one deal, as I felt this was a reasonable real world constraint.

For example, if the set A, B, C, D, E forms a deal, a customer could get a discount on a deal group of items [A, B, C] or [A, A, B] or [A, A, A], where cheapest item in the group will be free. 

Given this decision, any deal of type 1 above is actually a valid deal of type 2 above, since the deal set of type 2 could be comprised of just 1 item. This means we can use the same algorithm for both deal types to determine, for each deal, the optimal groups of items within that deal that give a user maximum savings based on their cart. This problem exhibits the optimal substructure property, and a greedy algorithm that sequentially forms groups of the most expensive items from the deal set can efficiently find the optimal groups for the most savings. 

For example, if A, B, C, D, E forms a deal where those items are ordered by price, and a user cart is [A, A, A, B, C, D, E], the optimal deal groups are [A, A, A], [B, C, D]. These groups can be formed by sequentially taking the most expensive items from the cart and forming batches of 3. This algorithm is implemented in the `calculateDeals` method of the `CheckoutRegister` to calculate the optimal deal groups right before the receipt is printed.

### Market Structure
The Supermarket application separates the responsibilities of storing items and deals, as well as managing a user's cart during checkout, across several classes:
- `CatalogItem` represents an item in the catalog.
- `Catalog` stores the items and deals available in the market.
- `CheckoutRegister` manages a user's cart during item scanning, calculates optimal deals at checkout, and prints a receipt.

In the `Catalog` class, I chose to maintain 3 data structures to represent the items and deals:
- A vector of `CatalogItem` objects representing all items in the store. I used the index of an item in this vector as its unique identifier (id).
- A map of item names (strings) to their ids for quick lookups by name.
- A vector of deals, where each deal is represented as a vector of item ids, sorted by price. Similar to the items vector, I used the index of a deal in this vector as its id.

Structuring the `Catalog` in this way allowed for efficient, constant-time lookups of `CatalogItem` objects using the map when users entered an item's name. By representing deals with item ids, I maintained constant-time access to each item in a deal while avoiding duplication of `CatalogItem` objects or the need to manage pointers or references to them.

In the `CheckoutRegister` class I chose to represent a user cart state using 3 data structures:
- A list of item ids to track which items are in the cart.
- A map of item ids to quantities to track how many of each item are in the cart.
- A set of deal ids that may apply to the cart. Whenever an item is added, its `dealId` field is checked to determine if it's part of a deal, and if so, the corresponding deal id is added to this set.

Structuring the `CheckoutRegister` in this way allowed me to efficiently perform the maximum savings algorithm during checkout. By iterating over the applicable deals, I could map the item ids in each deal to their corresponding quantities in the cart and use those quantities to build correctly sized deal groups (e.g., groups of three items). This process was further optimized by the fact that the item ids in each deal were already sorted by price, allowing me to easily prioritize higher-priced items for maximum savings. After building the deal groups, I updated the quantity values in the map to ensure that items included in deals were not counted more than once.

## Future Considerations
There are several potential improvements that could enhance the functionality and flexibility of the program in the future:
- Multiple Customers: After printing a receipt, start a new checkout session with the next customer (instead of exiting the program). The `CheckoutRegister` class actually has a `clearSession` method which removes all cart state after the receipt is printed, so this could be easily implemented.
- Concurrent Customers: Allow multiple registers to be instantiated and serve customers concurrently.
- Custom Input/Output File Paths: Allow users to specify the paths for the input (items.csv, discounts.csv, shopping_list.csv) and output (receipt.txt) files via command line arguments, instead of relying on fixed directories.