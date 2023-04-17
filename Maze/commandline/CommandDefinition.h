//
//  CommandDefinition.h
//  maze
//
//  Created by John Kooistra on 2023-04-17.
//

#ifndef CommandDefinition_h
#define CommandDefinition_h

#include <string>

enum class CommandArgument {
    None,
    Int,
    String,
    OptionalInt,
    OptionalString,
};

struct CommandDefinition {
    std::string name;
    CommandArgument argumentType;
    std::string argumentName;
    std::string description;
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
