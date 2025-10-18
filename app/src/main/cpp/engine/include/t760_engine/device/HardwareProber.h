#ifndef T760_HARDWARE_PROBER_H
#define T760_HARDWARE_PROBER_H

#include "t760_engine/device/DeviceCapabilities.h"

namespace t760 {

class HardwareProber {
public:
    static SystemHardwareCapabilities probe();
};

}

#endif // T760_HARDWARE_PROBER_H