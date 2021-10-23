//
// Created by mikayel on 10/22/21.
//
#include <string>
#include <iostream>
#include "Reader.h"
#include "Executable.h"
#include "Utils.h"

void PrintPromt()
{
    std::cout << "db> " << std::flush;
}

int main(int argc, char** argv)
{
    std::string input;
    while(true)
    {
        PrintPromt();
        input = simple_db::ReadCommand(simple_db::ConsoleReader());

        if(simple_db::utils::IsMetaCommand(input))
        {
            simple_db::MetaCommand metaCommand{input};
            simple_db::Execute(metaCommand);
        }
        else
        {
            simple_db::Statement stmt{input};
            simple_db::Execute(stmt);
        }
    }
}

