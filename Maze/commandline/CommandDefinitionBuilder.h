//
//  CommandDefinitionBuilder.h
//  maze
//
//  Created by John Kooistra on 2023-04-20.
//

#ifndef CommandDefinitionBuilder_h
#define CommandDefinitionBuilder_h

#include <optional>

#include "CommandDefinition.h"
#include "CommandDefinitionCollector.h"

class CommandDefinitionBuilder {
private:
    std::shared_ptr<CommandDefinitionCollector> sink;
    
    CommandDefinition definition;
    
    bool optional { false };
    CommandArgument baseArgumentType { CommandArgument::None };
    
public:
    CommandDefinitionBuilder(std::shared_ptr<CommandDefinitionCollector> sink, std::string const &name, std::string const &description);
    virtual ~CommandDefinitionBuilder() {}
    
    CommandDefinitionBuilder &intArgument(std::string const &argName, std::optional<int> defaultValue = std::nullopt);
    CommandDefinitionBuilder &stringArgument(std::string const &argName, std::optional<std::string> defaultValue = std::nullopt);
    CommandDefinitionBuilder &setOptional();
    CommandDefinitionBuilder &setUncommon();
    CommandDefinitionBuilder &addMessage(std::string const &message);
    
    CommandDefinition build();
    
private:
    void setArgumentType();
};

#endif /* CommandDefinitionBuilder_h */
