#include "Factory.hpp"

using INetworkPtr = std::unique_ptr<RIS::INetwork>;

namespace RIS
{
    INetworkPtr NetworkFactory::Create()
    {
        return INetworkPtr(nullptr);
    }
}
