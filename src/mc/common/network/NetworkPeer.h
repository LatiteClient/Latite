#pragma once

namespace SDK {
    struct NetworkStatus {
        int32_t level;
        int64_t currentPing;
        int64_t averagePing;

    private:
        char pad[0x30];
    };

    class NetworkPeer {
    public:
        NetworkStatus getNetworkStatus() {
            NetworkStatus status {};
            memory::callVirtual<void, NetworkStatus&>(this, 2, status);
            return status;
        }
    };
}
