//
//  CommandParser.cpp
//  maze
//
//  Created by John Kooistra on 2023-04-17.
//

#include <iostream>
#include <optional>

#include "CommandParser.h"

CommandDefinition CommandParser::add(CommandDefinition const &definition) {
    if (argumentTypes.find(definition.name) == argumentTypes.end()) {
        commands.push_back(definition);
        argumentTypes[definition.name] = definition.argumentType;
    } else {
        std::cerr << "ERROR: command with name \"" << definition.name << "\" already exists." << std::endl;
    }
    return definition;
}

CommandDefinition CommandParser::add(std::string const &name, std::string const &description) {
    return add({
        name,
        CommandArgument::None,
        "",
        description
    });
}

CommandDefinition CommandParser::add(std::string const &name, CommandArgument type, std::string const &typeName, std::string const &description) {
    return add({
        name,
        type,
        typeName,
        description
    });
}

CommandParserResult CommandParser::parse(int argc, const char * argv[]) const {
    std::vector<std::string> args(argv + 1, argv + argc);
    return parse(args);
}

CommandParserResult CommandParser::parse(std::vector<std::string> const &args) const {
    CommandParserResult result;
    std::optional<Command> partialCommand;
    for (auto token : args) {
        if (partialCommand.has_value()) {
            auto expectedType = argumentTypes.find(partialCommand->name)->second;
            switch (expectedType) {
                case CommandArgument::None:
                    return result.invalidate("The parser screwed up, this should not be possible.");
                case CommandArgument::String:
                case CommandArgument::OptionalString:
                    if (isRequired(expectedType) || token.empty() || token[0] != '-') {
                        partialCommand->stringValue = token;
                        partialCommand->hasValue = true;
                    }
                    break;
                case CommandArgument::Int:
                case CommandArgument::OptionalInt:
                    if (isRequired(expectedType) || token.empty() || token[0] != '-') {
                        partialCommand->intValue = std::stoi(token);
                        partialCommand->hasValue = true;
                    }
                    break;
            }
            bool consumedToken = partialCommand->hasValue;
            result.commands.push_back(*partialCommand);
            partialCommand.reset();
            if (consumedToken) {
                continue;
            }
        }
        
        if (token.size() > 0 && token[0] == '-') {
            const auto strBegin = token.find_first_not_of("-");
            if (strBegin == std::string::npos) {
                return result.invalidate("A command consisted only of - characters: \"" + token + "\".");
            }

            std::string commandName = token.substr(strBegin, token.length() - strBegin);
            for (auto command : commands) {
                if (command.name == commandName) {
                    partialCommand = Command(commandName);
                    break;
                }
            }
            if (partialCommand.has_value()) {
                auto expectedType = argumentTypes.find(partialCommand->name)->second;
                if (expectedType == CommandArgument::None) {
                    result.commands.push_back(*partialCommand);
                    partialCommand.reset();
                }
            } else {
                return result;
            }
        } else {
            return result;
        }
    }
    
    // If a required argument is missing, invalidate the result.
    if (partialCommand.has_value() && isRequired(argumentTypes.find(partialCommand->name)->second)) {
        result.invalidate("Non-optional argument was missing for command \"" + partialCommand->name + "\"");
    }
    
    return result;
}

void CommandParser::printUsage() const {
    std::cout << "Usage: " << std::endl;
    // TODO: print usage given the command names and descriptions specified.
}

