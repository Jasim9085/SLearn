#ifndef T760_FORMAT_PARSER_H
#define T760_FORMAT_PARSER_H

#include "t760_engine/model/ModelConfig.h"
#include <string>
#include <memory>

namespace t760 {

// Responsible for reading the binary .t760 file and parsing its metadata headers.
// It does NOT load the tensor data itself.
class T760FormatParser {
public:
    static std::unique_ptr<ModelConfig> parse_metadata(const std::string& file_path);
};

}

#endif // T760_FORMAT_PARSER_H