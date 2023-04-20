//
//  CommandDefinition.cpp
//  Maze
//
//  Created by John Kooistra on 2023-04-20.
//

#include <sstream>

#include "CommandDefinition.h"

CommandDefinition::CommandDefinition(std::string const &name, std::string const &description)
:   name(name)
,   description(description)
{
}

std::string CommandDefinition::usageArgument() const {
    std::stringstream stream;
    stream << "-" << name;
    if (argumentType != CommandArgument::None) {
        stream << " ";
        if (!isRequired(argumentType)) {
            stream << "[";
        }
        stream << argumentName;
        if (!isRequired(argumentType)) {
            stream << "]";
        }
    }
    return stream.str();
}
