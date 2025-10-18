#ifndef T760_TYPES_H
#define T760_TYPES_H

#include <cstdint>
#include <vector>
#include <string>
#include "t760_engine/core/Constants.h"

namespace t760 {

enum class DeviceType : uint8_t {
    CPU = 0,
    GPU = 1,
    NPU = 2,
    SHARED = 3
};

enum class DataType : uint32_t {
    FP32 = 0,
    FP16 = 1,
    BF16 = 2,
    INT8 = 3,
    UINT8 = 4,
    QINT16 = 10,
    QINT8 = 11,
    QINT4 = 12
};

enum class EngineState {
    UNINITIALIZED,
    INITIALIZED,
    MODEL_LOADED,
    INFERENCE_ACTIVE,
    SHUTDOWN,
    ERROR_STATE
};

struct ConversationHandle {
    uint64_t id;
    explicit ConversationHandle(uint64_t id_ = 0) : id(id_) {}
    bool operator==(const ConversationHandle& other) const { return id == other.id; }
    bool is_valid() const { return id != 0; }
};

struct DeviceConfig {
    DeviceType type;
    uint64_t memory_budget_mb = 0;
    bool enabled = true;
};

struct EngineConfig {
    std::vector<DeviceConfig> devices;
    uint32_t max_concurrent_conversations = constants::MAX_CONCURRENT_CONVERSATIONS;
    bool enable_profiling = false;
};

}

#endif // T760_TYPES_H