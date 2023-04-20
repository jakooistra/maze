//
//  CommandParser.h
//  maze
//
//  Created by John Kooistra on 2023-04-17.
//

#ifndef CommandParser_h
#define CommandParser_h

#include <memory>
#include <unordered_map>
#include <vector>

#include "Command.h"
#include "CommandDefinition.h"
#include "CommandDefinitionBuilder.h"
#include "CommandDefinitionCollector.h"

struct CommandParserResult {
    bool valid { true };
    std::string invalidMessage;
    std::vector<Command> commands;
    
    CommandParserResult const &invalidate(std::string const &message) {
        invalidMessage = message;
        valid = false;
        return *this;
    }
};

class CommandParser
:   public CommandDefinitionCollector
,   public std::enable_shared_from_this<CommandParser>
{
private:
    std::vector<CommandDefinition> commands;
    std::unordered_map<std::string, CommandArgument> argumentTypes;
    
public:
    virtual CommandDefinition add(CommandDefinition const &definition);
    
    // Start building a command definition and return the builder.
    CommandDefinitionBuilder add(std::string const &name, std::string const &description);
    
    CommandParserResult parse(int argc, const char * argv[]) const;
    CommandParserResult parse(std::vector<std::string> const &args) const;
    
    void printUsage(std::string const &programName) const;
    
private:
    std::string getFullCommand(std::string const &programName, bool commonArgumentsOnly) const;
    void printDetailedArguments() const;
};

#endif /* CommandParser_h */
