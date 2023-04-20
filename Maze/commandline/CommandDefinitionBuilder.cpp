//
//  CommandDefinitionBuilder.cpp
//  maze
//
//  Created by John Kooistra on 2023-04-20.
//

#include "CommandDefinitionBuilder.h"

CommandDefinitionBuilder::CommandDefinitionBuilder(std::shared_ptr<CommandDefinitionCollector> sink, std::string const &name, std::string const &description)
:   sink(sink)
,   definition(name, description)
{
    definition.common = true;
}

CommandDefinitionBuilder &CommandDefinitionBuilder::intArgument(std::string const &argName, std::optional<int> defaultValue) {
    baseArgumentType = CommandArgument::Int;
    definition.argumentName = argName;
    definition.argumentDefault = CommandValue::from(defaultValue);
    setArgumentType();
    return *this;
}

CommandDefinitionBuilder &CommandDefinitionBuilder::stringArgument(std::string const &argName, std::optional<std::string> defaultValue) {
    baseArgumentType = CommandArgument::String;
    definition.argumentName = argName;
    definition.argumentDefault = CommandValue::from(defaultValue);
    setArgumentType();
    return *this;
}

CommandDefinitionBuilder &CommandDefinitionBuilder::setOptional() {
    optional = true;
    setArgumentType();
    return *this;
}

CommandDefinitionBuilder &CommandDefinitionBuilder::setUncommon() {
    definition.common = false;
    return *this;
}

CommandDefinitionBuilder &CommandDefinitionBuilder::addMessage(std::string const &message) {
    definition.messages.push_back(message);
    return *this;
}

CommandDefinition CommandDefinitionBuilder::build() {
    if (sink != nullptr) {
        sink->add(definition);
        
        // Only allow building once per the normal convention of the builder convention.
        sink = nullptr;
    }
    return definition;
}

void CommandDefinitionBuilder::setArgumentType() {
    switch (baseArgumentType) {
        case CommandArgument::Int:
        case CommandArgument::OptionalInt:
            definition.argumentType = optional ? CommandArgument::OptionalInt : CommandArgument::Int;
            break;
        case CommandArgument::String:
        case CommandArgument::OptionalString:
            definition.argumentType = optional ? CommandArgument::OptionalString : CommandArgument::String;
            break;
        case CommandArgument::None:
            break;
    }
}
