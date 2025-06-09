#include "RakNetConnector.h"

SDK::RakNetConnector* SDK::RakNetConnector::get() {
    return ClientInstance::get()->packetSender->networkSystem->remoteConnector->rakNetConnector;
}
