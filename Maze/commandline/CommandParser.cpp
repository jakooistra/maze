//
//  CommandParser.cpp
//  maze
//
//  Created by John Kooistra on 2023-04-17.
//

#include <iomanip>
#include <iostream>
#include <optional>
#include <sstream>

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

CommandDefinitionBuilder CommandParser::add(std::string const &name, std::string const &description) {
    return CommandDefinitionBuilder(shared_from_this(), name, description);
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
                        partialCommand->value = token;
                    }
                    break;
                case CommandArgument::Int:
                case CommandArgument::OptionalInt:
                    if (isRequired(expectedType) || token.empty() || token[0] != '-') {
                        partialCommand->value = std::stoi(token);
                    }
                    break;
            }
            bool consumedToken = partialCommand->has_value();
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
    if (partialCommand.has_value()) {
        if (isRequired(argumentTypes.find(partialCommand->name)->second)) {
            result.invalidate("Non-optional argument was missing for command \"" + partialCommand->name + "\"");
        } else {
            result.commands.push_back(*partialCommand);
            partialCommand.reset();
        }
    }
    
    return result;
}

void CommandParser::printUsage(std::string const &programName) const {
    std::cout << "Usage:" << std::endl;
    std::cout << "  " << getFullCommand(programName, true) << std::endl;
    std::cout << "  " << getFullCommand(programName, false) << std::endl;
    printDetailedArguments();
}

std::string CommandParser::getFullCommand(std::string const &programName, bool commonArgumentsOnly) const {
    std::stringstream stream;
    stream << programName;
    for (auto command : commands) {
        if (!commonArgumentsOnly || command.common) {
            stream << " [" << command.commandUsage() << "]";
        }
    }
    return stream.str();
}

void CommandParser::printDetailedArguments() const {
    std::cout << "Arguments:" << std::endl;
    int commandNameWidths = 0;
    for (auto command : commands) {
        commandNameWidths = std::max(commandNameWidths, (int)command.singleCommandUsage().length());
    }
    commandNameWidths += 1;
    for (auto command : commands) {
        std::vector<std::string> messages = { command.description };
        if (command.argumentDefault.has_value()) {
            std::stringstream message;
            message << "  (" << command.argumentDefault->str() << ") if unspecified.";
            messages.push_back(message.str());
        }
        for (auto &message : command.messages) {
            messages.push_back("  " + message);
        }
        if (!command.singleArgument) {
            messages.push_back("  Can be specified multiple times.");
        }
        for (int i = 0; i < messages.size(); ++i) {
            std::string prefix = (i == 0) ? command.singleCommandUsage() : "";
            std::cout
                << "  "
                << std::left
                << std::setw(commandNameWidths)
                << prefix
                << messages[i]
                << std::endl;
        }
    }
}
