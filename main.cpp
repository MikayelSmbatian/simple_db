//
// Created by mikayel on 10/22/21.
//
#include <string>
#include <iostream>

void print_promt()
{
    std::cout << "db> ";
}

int main(int argc, char** argv)
{
    std::string input;
    while(true)
    {
        print_promt();
        std::getline(std::cin, input);
        if(input == ".exit")
        {
            break;
        }
        else
        {
            std::cout << "Unrecognized command: " << input << "\n";
        }
    }
}

