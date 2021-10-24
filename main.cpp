//
// Created by mikayel on 10/22/21.
//
#include <memory>
#include <string>
#include <iostream>
#include "Reader.h"
#include "Executable.h"
#include "Utils.h"

void PrintPromt()
{
    std::cout << "db> " << std::flush;
}
/** Problems
 * 1. Table should contain list<MemoryPage> as it is a big luxury to have continuous memory that much.
 */
/**
 * For now, it will:
 * * support two operations: inserting a row and printing all rows
 * * reside only in memory (no persistence to disk)
 * * support a single, hard-coded table
 *
 * id	    integer
 * username	varchar(32)
 * email	varchar(255)
 *
 * Here’s my plan:
 * Store rows in blocks of memory called pages
 * Each page stores as many rows as it can fit
 * Rows are serialized into a compact representation with each page
 * Pages are only allocated as needed
 * Keep a fixed-size array of pointers to pages
 */
int main(int argc, char** argv)
{
    std::string input;
    auto t = std::make_shared<simple_db::Table>();
    int i = 0;
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
            auto stmt = simple_db::Statement::Prepare(input, t);
            simple_db::Execute(*stmt);
        }
    }
}

