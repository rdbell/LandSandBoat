#pragma once

#include "common/cbasetypes.h"

// Pure dboxutils open-state policy helpers.
//
// Slice 2849: IsSendBoxOpen / IsRecvBoxOpen / IsAnyDeliveryBoxOpen type gates.
// Slice 2850: OpenPlan / PlanOpenSendBox / PlanOpenRecvBox Clean+SetType plans.
//
// Production hosts in dboxutils.cpp. Helpers take host-injected container type
// scalars only (no CCharEntity / CUContainer pointers or universal_container
// enum headers). Packet push stays host-side.

namespace dboxutilshelpers
{

// IsSendBoxOpen mirrors dboxutils::IsSendBoxOpen pure half:
//   type == sendDeliveryBoxType
// Host injects GetType() and UCONTAINER_SEND_DELIVERYBOX.
inline auto IsSendBoxOpen(const uint8 type, const uint8 sendDeliveryBoxType) -> bool
{
    return type == sendDeliveryBoxType;
}

// IsRecvBoxOpen mirrors dboxutils::IsRecvBoxOpen pure half:
//   type == recvDeliveryBoxType
// Host injects GetType() and UCONTAINER_RECV_DELIVERYBOX.
inline auto IsRecvBoxOpen(const uint8 type, const uint8 recvDeliveryBoxType) -> bool
{
    return type == recvDeliveryBoxType;
}

// IsAnyDeliveryBoxOpen mirrors dboxutils::IsAnyDeliveryBoxOpen pure half:
//   IsSendBoxOpen(type, send) || IsRecvBoxOpen(type, recv)
// Host injects GetType() and both delivery-box type constants.
inline auto IsAnyDeliveryBoxOpen(const uint8 type, const uint8 sendDeliveryBoxType, const uint8 recvDeliveryBoxType) -> bool
{
    return IsSendBoxOpen(type, sendDeliveryBoxType) || IsRecvBoxOpen(type, recvDeliveryBoxType);
}

// OpenPlan is the pure UContainer mutation plan for OpenSendBox / OpenRecvBox
// (slice 2850): always Clean, then SetType to the target delivery-box type.
// Packet emission (DeliOpen / PostOpen) remains host-owned after apply.
struct OpenPlan
{
    bool  clean{};
    uint8 setType{};
};

// PlanOpenSendBox mirrors OpenSendBox's Clean + SetType half.
// Host injects sendDeliveryBoxType = UCONTAINER_SEND_DELIVERYBOX.
inline auto PlanOpenSendBox(const uint8 sendDeliveryBoxType) -> OpenPlan
{
    return OpenPlan{
        .clean   = true,
        .setType = sendDeliveryBoxType,
    };
}

// PlanOpenRecvBox mirrors OpenRecvBox's Clean + SetType half.
// Host injects recvDeliveryBoxType = UCONTAINER_RECV_DELIVERYBOX.
inline auto PlanOpenRecvBox(const uint8 recvDeliveryBoxType) -> OpenPlan
{
    return OpenPlan{
        .clean   = true,
        .setType = recvDeliveryBoxType,
    };
}

} // namespace dboxutilshelpers
