/*
Project Goal: Inventory for a kitchen. 

Features:
Make inventory/pantry from scratch
Compare quantity on hand against quantity in inventory
Create Grocery List when quantities drop below minimum

Yet to implement:
Create an "Edit inventory" function that:
    a) allows searching item by name
    b) allows editing of quantity and cost
Create a "Receive Groceries" Function that:
    a) steps through grocery list or receipt
    b) "receives" items purchased
    c) confirms cost from receipt
Create a "Recipe" function that:
    a) allows types of recipes to be stored
    b) removes select quantities from inventory based on recipe
    c) determines if ingredients are on hand to cook a recipe
    d) automatically updates grocery list and inventory
*/

#include <iostream>
#include <string>
#include <iomanip>
#include <fstream>
#include <vector>
#include <limits>
#include <ctime>
#include <chrono>
#include "Provision.h"
using namespace std;

// This is the minimum threshold before an item is moved
// To the grocery list. 
const int MINIMUM_INVENTORY = 2;

//Function prototypes
void fillInventory();
Provision* loadInventory(int&);
int binCounter();
void groceryList(int, Provision*);

int main()
{
    int totalBins = binCounter();
    Provision* pantry = loadInventory(totalBins);

    int choice;

    // Menu options; added as functions are added.
    cout << "*****Inventory Management System*****\n";
    cout << "_____________________________________\n";
    cout << "******Enter 1 to make inventory******\n";
    cout << "*****Enter 2 to make grocery list****\n";
    cin >> choice;

    switch(choice)
    {
        case 1: "Making New Inventory:\n";
            fillInventory();
            break;
        case 2: "Completing Grocery List:\n";
            groceryList(totalBins, pantry);
            break;
        default:
            cout << "Invalid Selection!\n";
    }

    delete [] pantry;
    pantry = nullptr;
    return 0;
}


/*
-----------------------------------------------------------------------------
From a blank inventory: Asks generally, how many bins to fill then
takes a single item name, the quantity on hand for that item, and the cost 
per unit. It then creates a .csv file and writes the contents of the array to 
the file.
----------------------------------------------------------------------------- 
*/
void fillInventory() 
{
    int totalBins; // How many Provision objects are held in inventory
    cout << "How many bins are in the pantry? ";
    cin >> totalBins;
    cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n'); // flush buffer
    std::string tempName;
    int tempQuant;
    double tempCost;

    std::ofstream outFile("pantry.csv");
    
    if (!outFile) 
    {
        std::cerr << "Error creating file!\n";
        return;
    }

    Provision* pantry = new Provision[totalBins];

    // Steps through user inputs for item, quantity, and cost
    for (int count = 0; count < totalBins; count++)
    {
        cout << "\nItem name: ";
        cin.ignore();
        getline(cin, tempName);
        pantry[count].setName(tempName);
        cout << "\nQuantity on Hand: ";
        cin >> tempQuant;
        pantry[count].setQuant(tempQuant);
        cout << "\nItem cost: ";
        cin >> tempCost;
        pantry[count].setCost(tempCost);
    }

    // Saves dynamic array to a .csv file for permanence
    for (int count = 0; count < totalBins; count++)
    {
        outFile << pantry[count].getName() << ","
        << pantry[count].getQuant() << ","
        << pantry[count].getCost() << "\n";
    }



    delete [] pantry;
    pantry = nullptr;
    outFile.close();

    cout << "Pantry file has been saved!\n";
}


/*
-----------------------------------------------------------------------------
Takes input file and loads contents into a dynamically allocated array for 
use in other functions. Will only be called from main() which has the delete[] 
in use to keep things memory safe. 
-----------------------------------------------------------------------------
*/
Provision* loadInventory(int& totalBins)
{   
    totalBins = binCounter();
    std::string line;

    std::ifstream inFile("pantry.csv");

    if (!inFile)
    {
        std::cerr << "Error opening pantry.csv!\n";
        return nullptr;
    }

    inFile.clear();                 // clear EOF flag
    inFile.seekg(0);                // rewind to beginning

    Provision* pantry = new Provision[totalBins];

    std::string tempName;
    int tempQuant;
    double tempCost;

    for (int count = 0; count < totalBins; count++)
    {
        getline(inFile, tempName, ',');
        inFile >> tempQuant;
        inFile.ignore(); // skip comma
        inFile >> tempCost;
        inFile.ignore(); // skip newline

        pantry[count].setName(tempName);
        pantry[count].setQuant(tempQuant);
        pantry[count].setCost(tempCost);
    }

    inFile.close();
    return pantry; 
}

/*
-----------------------------------------------------------------------------
Sets the value for total number of bins from the number of line items in the 
.csv file that's being used to allow data to persist between operations. 
-----------------------------------------------------------------------------
*/

int binCounter()
{
    int totalBins = 0 ; // How many Provision objects are held in inventory
    
    std::string line;

    std::ifstream inFile("pantry.csv");

    if (!inFile) 
    {
        std::cerr << "Error reading file!\n";
    }

    // Determines how long the dynamic array needs to be

    while (std::getline(inFile, line)) 
    {
        if (!line.empty())
            ++totalBins;
    }
    return totalBins;
}

/*
-----------------------------------------------------------------------------
Compares the inventory on hand versus a minimum value then creates a list in 
a .txt file from the items on hand that are below the minimum threshold
----------------------------------------------------------------------------- 
*/
void groceryList(int totalBins, Provision* pantryList)
{
    std::ofstream outFile("grocery_list.txt");

    // Gets current time for grocery list print out
    auto now = std::chrono::system_clock::now();
    std::time_t now_time = std::chrono::system_clock::to_time_t(now);
    
    if (!outFile) 
    {
        std::cerr << "Error creating file!\n";
    }
    
    Provision* shoppingList = new Provision[totalBins]; 

    int onHand, shoppingCount = 0; 

    for (int count = 0; count < totalBins; count++)
    {
        onHand = pantryList[count].getQuant();

        
        if (onHand < MINIMUM_INVENTORY)
        {
            shoppingList[shoppingCount].setName(pantryList[count].getName());
            shoppingList[shoppingCount].setQuant(pantryList[count].getQuant());
            shoppingList[shoppingCount].setCost(pantryList[count].getCost());
            shoppingCount++;
        }
        else
            cout << pantryList[count].getName() << ": Inventory OK" << endl;

    }

    // Calculates the estimated total from previously recorded costs for items in inventory.
    double estimatedCost;
    for (int count = 0; count < shoppingCount; count++)
    {
        estimatedCost += shoppingList[count].getTotalCost();
    }

    // Formatting for the .txt file for easier readability
    outFile << "**********Grocery List***********\n";
    outFile << std::put_time(std::localtime(&now_time), "%A, %B %d, %Y %I:%M %p") << "\n";
    outFile << "*********************************\n\n";
    for (int count = 0; count < shoppingCount; count++)
    {
        outFile << shoppingList[count].getName() << "\n";
    }
    outFile << "\n";
    outFile << "*********************************\n";
    outFile << "Estimated Price for Groceries: \n" << setw(12) << right
        << setprecision(2) << showpoint << fixed << "$" <<(estimatedCost * 2) << endl;

    cout << "Grocery List Created!" << endl;

    delete[] shoppingList;
    shoppingList = nullptr;
    outFile.close();
}