//
//  CommandValue.h
//  Maze
//
//  Created by John Kooistra on 2023-04-20.
//

#ifndef CommandValue_h
#define CommandValue_h

#include <optional>
#include <string>
#include <sstream>

struct CommandValue {
    std::string string;
    int integer;
    
    CommandValue(std::string const &value) : string(value), integer(0) {}
    CommandValue(int value) : string(""), integer(value) {}
    
    static std::optional<CommandValue> from(std::optional<int> value) {
        return value.has_value() ? std::optional<CommandValue>(*value) : std::nullopt;
    }
    
    static std::optional<CommandValue> from(std::optional<std::string> value) {
        return value.has_value() ? std::optional<CommandValue>(*value) : std::nullopt;
    }
    
    std::string str() const {
        if (string.length() > 0) {
            return string;
        } else {
            std::stringstream text;
            text << integer;
            return text.str();
        }
    }
};

#endif /* CommandValue_h */
