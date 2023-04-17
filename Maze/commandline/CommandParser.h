//
//  CommandParser.h
//  maze
//
//  Created by John Kooistra on 2023-04-17.
//

#ifndef CommandParser_h
#define CommandParser_h

#include <vector>
#include <unordered_map>

#include "Command.h"
#include "CommandDefinition.h"

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

class CommandParser {
private:
    std::vector<CommandDefinition> commands;
    std::unordered_map<std::string, CommandArgument> argumentTypes;
    
public:
    CommandDefinition add(CommandDefinition const &definition);
    CommandDefinition add(std::string const &name, std::string const &description);
    CommandDefinition add(std::string const &name, CommandArgument type, std::string const &typeName, std::string const &description);
    
    CommandParserResult parse(int argc, const char * argv[]) const;
    CommandParserResult parse(std::vector<std::string> const &args) const;
    
    void printUsage() const;
};

#endif /* CommandParser_h */
