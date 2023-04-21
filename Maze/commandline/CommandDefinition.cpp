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

std::string CommandDefinition::commandUsage() const {
    if (singleArgument) {
        return singleCommandUsage();
    } else {
        std::stringstream stream;
        stream << singleCommandUsage() << " [" << singleCommandUsage(false) << "] ...";
        return stream.str();
    }
}

std::string CommandDefinition::singleCommandUsage(bool showOptionalityOfArgument) const {
    std::stringstream stream;
    stream << "-" << name;
    if (argumentType != CommandArgument::None) {
        stream << " ";
        if (showOptionalityOfArgument && !isRequired(argumentType)) {
            stream << "[";
        }
        stream << argumentName;
        if (showOptionalityOfArgument && !isRequired(argumentType)) {
            stream << "]";
        }
    }
    return stream.str();
}
