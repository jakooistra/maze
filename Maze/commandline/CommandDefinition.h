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

struct CommandDefinition {
    // The command line argument for the command. If this string is "w" the command would be "-w" when passed.
    std::string name;
    
    // If false, the command can be usefully passed multiple times.
    bool singleArgument { true };
    
    CommandArgument argumentType { CommandArgument::None };
    std::string argumentName;
    std::optional<CommandValue> argumentDefault;
    
    // The main description of the command, shown when printing usage.
    std::string description;
    
    // Sub-messages shown when printing usage.
    std::vector<std::string> messages;
    
    // True if this parameter is expected to be defined more often.
    // Used when printing usage instructions.
    bool common { false };
    
    CommandDefinition(std::string const &name, std::string const &description);
    
    std::string commandUsage() const;
    std::string singleCommandUsage(bool showOptionalityOfArgument = true) const;
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
