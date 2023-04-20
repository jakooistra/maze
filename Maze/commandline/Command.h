//
//  Command.h
//  maze
//
//  Created by John Kooistra on 2023-04-17.
//

#ifndef Command_h
#define Command_h

#include <optional>
#include <string>

#include "CommandValue.h"

struct Command {
    std::string name;
    std::optional<CommandValue> value;
    
    Command(std::string const &_name) : name(_name) {}
    
    bool has_value() const { return value.has_value(); }
};

#endif /* Command_h */
