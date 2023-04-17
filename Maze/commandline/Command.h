//
//  Command.h
//  maze
//
//  Created by John Kooistra on 2023-04-17.
//

#ifndef Command_h
#define Command_h

#include <string>

struct Command {
    std::string name;
    
    bool hasValue { false };
    std::string stringValue;
    int intValue { 0 };
    
    Command(std::string const &_name) : name(_name) {}
};

#endif /* Command_h */
