//
//  CommandDefinitionCollector.h
//  Maze
//
//  Created by John Kooistra on 2023-04-20.
//

#ifndef CommandDefinitionCollector_h
#define CommandDefinitionCollector_h

#include "CommandDefinition.h"

class CommandDefinitionCollector {
public:
    virtual CommandDefinition add(CommandDefinition const &definition) = 0;
};

#endif /* CommandDefinitionCollector_h */
