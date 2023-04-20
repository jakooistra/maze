//
//  CommandDefinition.h
//  maze
//
//  Created by John Kooistra on 2023-04-17.
//

#ifndef CommandDefinition_h
#define CommandDefinition_h

#include <string>
#include <vector>

#include "CommandValue.h"

enum class CommandArgument {
    None,
    Int,
    String,
    OptionalInt,
    OptionalString,
};

// TODO: add multiple-argument expectation to definition
// TODO: add default value to command parameter, read it from main.cpp
struct CommandDefinition {
    std::string name;
    std::string description;
    
    CommandArgument argumentType { CommandArgument::None };
    std::string argumentName;
    std::optional<CommandValue> argumentDefault;
    
    std::vector<std::string> messages;
    
    // True if this parameter is expected to be defined more often.
    // Used when printing usage instructions.
    bool common { false };
    
    CommandDefinition(std::string const &name, std::string const &description);
    
    std::string usageArgument() const;
};

inline bool isRequired(CommandArgument type) {
    switch (type) {
        case CommandArgument::None:
        case CommandArgument::OptionalInt:
        case CommandArgument::OptionalString:
            return false;
        case CommandArgument::Int:
        case CommandArgument::String:
            return true;
    }
}

#endif /* CommandDefinition_h */
