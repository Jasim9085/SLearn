#ifndef T760_PIPELINE_TYPES_H
#define T760_PIPELINE_TYPES_H

#include "t760_engine/tensor/Tensor.h"
#include "t760_engine/core/Types.h"
#include <vector>
#include <memory>
#include <unordered_map>

namespace t760 {

// Represents the state of a single, ongoing conversation.
// The key here is the KV cache, which will be a pair of tensors (Key, Value) for each layer.
struct ConversationState {
    ConversationHandle handle;
    std::vector<std::pair<std::unique_ptr<Tensor>, std::unique_ptr<Tensor>>> kv_cache;
    size_t processed_token_count = 0;
};

}

#endif // T760_PIPELINE_TYPES_H