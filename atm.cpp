#include <iostream>
#include <string>
#include <limits>
#include <cmath>
#include <sstream>

using namespace std;

// Need a better way of doing sessions
string session = "";

float getFloat()
{
    float val = 0;
    while(std::cout << "Enter a number: " && (!(std::cin >> val) || val < 0 || val != floor(val)))
    {
        std::cout << "Not a positive integer!\n";
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    }
    return val;
}

void logout()
{
    session = "";
}

bool userPINcheck(string user, string pin)
{
    return true; // Add check
}

void login(string user)
{
    string pin = "";
    cout << "PIN number: ";
    cin >> pin;
    if (pin.length() == 4)
    {
        // Check pin and user here!
        if (userPINcheck(user, pin))
            {
                cout << "Authenticated!" << endl;
                session = user; // Grant session access
                cout << endl;
            }
    }
    else
    {
        cout << "Username or PIN incorrect." << endl;
    }
}

int balance()
{
    float currentBalance = 0;
    // Get balance from bank
    return currentBalance;
}

void withdraw(float amount)
{
    float currentBalance = 0;
    // Get balance from bank
    if (currentBalance >= amount)
    {
        currentBalance = currentBalance - amount;
        //Tell bank current balance
        cout << amount << " has been withdrawn." << endl;
    }
    else
    {
        cout << "Insufficient funds." << endl;
    }
}

void transfer(float amount, string otherUser)
{
    // Ask bank if other user exists

    float currentBalance = 0;
    // Get balance from bank
    if (currentBalance > amount)
    {
        currentBalance = currentBalance - amount;
        //Tell bank current balance
        // Tell bank otherUser balance += amount
        cout << currentBalance << " has been transfered." << endl;
    }
    else
    {
        cout << "Insufficient funds or incorrect user." << endl;
    }
}

int main()
{
    //call connection function
    string user = "";
    while(1)
    {
        if (session != "")
        {
            float choice = 0;
            cout << "---------ATM Program---------" << endl;
            cout << "User Menu:" << endl;
            cout << "1) Check Balance" << endl;
            cout << "2) Withdraw Funds" << endl;
            cout << "3) Transfer Funds" << endl;
            cout << "4) Logout" << endl;
            cout << "5) Exit Program" << endl;
            cout << endl;
            choice = getFloat();
            cout << endl;
            if (choice == 1)
            {
                // Send check balance
                // get result and print



                float currentBalance = 0;
                currentBalance = balance();
                cout << "Current balance is: " << currentBalance << endl;
                currentBalance = 0;
                cout << endl;
            }
            else if (choice == 2)
            {
                // Send check balance
                // check if balance > number
                // Send withdraw (number)
                // get result and print




                cout << "Amount to withdraw: " << endl;
                withdraw(getFloat());
                cout << endl;
            }
            else if (choice == 3)
            {
                // check if otherUser exists
                // get balance from bank for user
                // check if user has enough money
                // send transfer request
                // get result and print

                string otherUser = "";
                cout << "Amount to transfer: " << endl;
                float amount = getFloat();
                cout << endl;
                cout << "Please enter user to transfer funds to: ";
                cin >> otherUser;
                transfer(amount, otherUser);
                amount = 0;
                otherUser = "";
                cout << endl;
            }
            else if (choice == 4)
            {
                logout();
                cout << endl;
            }
            else if (choice == 5)
            {
                break;
            }
            else
            {
                cout << "Error: Please enter a valid number." << endl;
            }
        }

        else
        {
            cout << "---------ATM Program---------" << endl;
            cout << "Please log in by entering your username: ";
            cin >> user;
            login(user);
            cout << endl;
        }
    }


    return 0;
}
