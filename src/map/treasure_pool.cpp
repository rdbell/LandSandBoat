/*
===========================================================================

  Copyright (c) 2025 LandSandBoat Dev Teams

  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License
  along with this program.  If not, see http://www.gnu.org/licenses/

===========================================================================
*/

#include "common/logging.h"
#include "common/timer.h"
#include "roe.h"

#include "packets/s2c/0x0d2_trophy_list.h"
#include "packets/s2c/0x0d3_trophy_solution.h"

#include "item_container.h"
#include "recast_container.h"
#include "treasure_pool.h"
#include "treasure_pool_capacity.h"
#include "utils/charutils.h"
#include "utils/itemutils.h"

static constexpr timer::duration treasure_checktime = 3s;
static constexpr timer::duration treasure_livetime  = 5min;

CTreasurePool::CTreasurePool(const TreasurePoolType PoolType)
: m_count(0)
, m_TreasurePoolType(PoolType)
{
    for (uint8 i = 0; i < TREASUREPOOL_SIZE; ++i)
    {
        m_PoolItems[i].ID     = 0;
        m_PoolItems[i].SlotID = i;
    }

    m_Members.reserve(static_cast<std::size_t>(PoolType));
}

auto CTreasurePool::getPoolType() const -> TreasurePoolType
{
    return m_TreasurePoolType;
}

auto CTreasurePool::getItems() const -> const std::array<TreasurePoolItem, TREASUREPOOL_SIZE>&
{
    return m_PoolItems;
}

auto CTreasurePool::itemCount() const -> uint8
{
    return m_count;
}

auto CTreasurePool::getMembers() const -> const std::vector<CCharEntity*>&
{
    return m_Members;
}

auto CTreasurePool::memberCount() const -> size_t
{
    return m_Members.size();
}

bool CTreasurePool::isMember(const CCharEntity* PChar)
{
    return std::find(m_Members.begin(), m_Members.end(), PChar) != m_Members.end();
}

void CTreasurePool::addMember(CCharEntity* PChar)
{
    if (PChar == nullptr || PChar->PTreasurePool != this)
    {
        ShowWarning("CTreasurePool::AddMember() - PChar was null, or PTreasurePool mismatched.");
        return;
    }

    if (std::find(m_Members.begin(), m_Members.end(), PChar) != m_Members.end())
    {
        ShowWarning("CTreasurePool::AddMember() - PChar was already in the members list!");
        return;
    }

    m_Members.emplace_back(PChar);

    if (m_TreasurePoolType == TreasurePoolType::Solo && PChar->PParty)
    {
        m_TreasurePoolType = TreasurePoolType::Party;
    }
    else if (m_TreasurePoolType == TreasurePoolType::Party && PChar->PParty && PChar->PParty->m_PAlliance)
    {
        m_TreasurePoolType = TreasurePoolType::Alliance;
    }

    updatePool(PChar);
}

void CTreasurePool::delMember(CCharEntity* PChar)
{
    if (PChar == nullptr || PChar->PTreasurePool != this)
    {
        ShowWarning("CTreasurePool::DelMember() - PChar was null, or PTreasurePool mismatched.");
        return;
    }

    // if(m_TreasurePoolType != TREASUREPOOL_ZONE)
    // Zone drops e.g. Dynamis DO NOT remove previous lot info. Everything else does.
    // ^ TODO: verify what happens when a winner leaves zone
    for (int i = 0; i < 10; i++)
    {
        if (!m_PoolItems[i].Lotters.empty())
        {
            auto lotterIterator = m_PoolItems[i].Lotters.begin();
            while (lotterIterator != m_PoolItems[i].Lotters.end())
            {
                // remove their lot info
                if (LotInfo* info = &(*lotterIterator); PChar->id == info->member->id)
                {
                    lotterIterator = m_PoolItems[i].Lotters.erase(lotterIterator);
                    continue;
                }
                ++lotterIterator;
            }
        }
    }

    auto memberToDelete = std::find(m_Members.begin(), m_Members.end(), PChar);
    if (memberToDelete != m_Members.end())
    {
        PChar->PTreasurePool = nullptr;
        m_Members.erase(memberToDelete);
    }

    if ((m_TreasurePoolType == TreasurePoolType::Party || m_TreasurePoolType == TreasurePoolType::Alliance) && memberCount() == 1)
    {
        m_TreasurePoolType = TreasurePoolType::Solo;
    }

    if (m_TreasurePoolType != TreasurePoolType::Zone && memberCount() == 0)
    {
        // TODO: This entire system needs rewriting to both:
        //     : - Make it stable
        //     : - Get rid of `delete this` and manage memory nicely
        delete this; // cpp.sh allow
        return;
    }
}

/************************************************************************
 *                                                                       *
 *  Adding an item to treasure pool. Returns item count in pool.         *
 *                                                                       *
 ************************************************************************/

uint8 CTreasurePool::addItem(uint16 ItemID, CBaseEntity* PEntity)
{
    uint8             SlotID     = 0;
    uint8             FreeSlotID = treasurepoolhelpers::FreeSlotUnset;
    timer::time_point oldest     = timer::time_point::max();
    const CItem*      PNewItem   = xi::items::lookup(ItemID);

    if (treasurepoolhelpers::ShouldRejectNullItem(PNewItem == nullptr))
    {
        return m_count; // no change
    }

    // Check if everyone in the treasure pool already has this item
    // Some items do not honor this check and will be added to the party pool regardless
    const bool skipRareCheck = treasurepoolhelpers::ShouldSkipRareCheck(
        m_TreasurePoolType == TreasurePoolType::Solo,
        PNewItem->hasFlag(ItemFlag::NoRareCheck));
    if (treasurepoolhelpers::ShouldApplyRareMemberCheck(PNewItem->hasFlag(ItemFlag::Rare), skipRareCheck))
    {
        bool doesNotHaveRareItem = false;

        for (const auto& member : m_Members)
        {
            // Someone doesn't have the rare item
            if (!charutils::HasItem(member, PNewItem->getID()))
            {
                doesNotHaveRareItem = true;
                break;
            }
        }

        // If everyone has this rare item, don't add it to the pool
        if (treasurepoolhelpers::ShouldRejectRareAllHave(true, doesNotHaveRareItem))
        {
            return m_count; // no change
        }
    }

    for (SlotID = 0; SlotID < 10; ++SlotID)
    {
        if (m_PoolItems[SlotID].ID == 0)
        {
            FreeSlotID = SlotID;
            break;
        }
    }

    if (treasurepoolhelpers::IsFreeSlotUnset(FreeSlotID))
    {
        // find the oldest non-rare and non-ex item
        for (SlotID = 0; SlotID < 10; ++SlotID)
        {
            const CItem* PItem = xi::items::lookup(m_PoolItems[SlotID].ID);
            if (PItem != nullptr &&
                treasurepoolhelpers::CanEvictNonRareNonExclusive(
                    PItem->hasFlag(ItemFlag::Rare),
                    PItem->hasFlag(ItemFlag::Exclusive)) &&
                treasurepoolhelpers::PreferOlderTimestamp(
                    m_PoolItems[SlotID].TimeStamp.time_since_epoch().count(),
                    oldest.time_since_epoch().count()))
            {
                FreeSlotID = SlotID;
                oldest     = m_PoolItems[SlotID].TimeStamp;
            }
        }
        if (treasurepoolhelpers::IsFreeSlotUnset(FreeSlotID))
        {
            // find the oldest non-ex item
            for (SlotID = 0; SlotID < 10; ++SlotID)
            {
                const CItem* PItem = xi::items::lookup(m_PoolItems[SlotID].ID);
                if (PItem != nullptr &&
                    treasurepoolhelpers::CanEvictNonExclusive(PItem->hasFlag(ItemFlag::Exclusive)) &&
                    treasurepoolhelpers::PreferOlderTimestamp(
                        m_PoolItems[SlotID].TimeStamp.time_since_epoch().count(),
                        oldest.time_since_epoch().count()))
                {
                    FreeSlotID = SlotID;
                    oldest     = m_PoolItems[SlotID].TimeStamp;
                }
            }

            if (treasurepoolhelpers::IsFreeSlotUnset(FreeSlotID))
            {
                // find the oldest item
                for (SlotID = 0; SlotID < 10; ++SlotID)
                {
                    if (treasurepoolhelpers::PreferOlderTimestamp(
                            m_PoolItems[SlotID].TimeStamp.time_since_epoch().count(),
                            oldest.time_since_epoch().count()))
                    {
                        FreeSlotID = SlotID;
                        oldest     = m_PoolItems[SlotID].TimeStamp;
                    }
                }

                if (treasurepoolhelpers::IsFreeSlotUnset(FreeSlotID))
                {
                    // default fallback
                    FreeSlotID = treasurepoolhelpers::DefaultFallbackSlot();
                }
            }
        }
    }

    if (treasurepoolhelpers::ShouldForceCheckOnFullPoolInsert(SlotID))
    {
        m_PoolItems[FreeSlotID].TimeStamp = timer::start_time;
        checkTreasureItem(timer::now(), FreeSlotID);
    }

    m_count++;
    m_PoolItems[FreeSlotID].ID        = ItemID;
    m_PoolItems[FreeSlotID].TimeStamp = timer::now() - treasure_checktime;

    for (const auto& member : m_Members)
    {
        // Issue RoE event for loot item and issue treasure pool packet
        roeutils::event(ROE_EVENT::ROE_LOOTITEM, member, RoeDatagram("itemid", m_PoolItems[FreeSlotID].ID));
        member->pushPacket<GP_SERV_COMMAND_TROPHY_LIST>(&m_PoolItems[FreeSlotID], PEntity, false);
    }

    if (treasurepoolhelpers::ShouldAutoResolveSolo(memberCount()))
    {
        checkTreasureItem(timer::now(), FreeSlotID);
    }

    return m_count;
}

void CTreasurePool::updatePool(CCharEntity* PChar)
{
    using treasurepoolhelpers::PlanUpdatePool;

    const bool charNull     = PChar == nullptr;
    const bool poolMismatch = !charNull && PChar->PTreasurePool != this;
    const bool isDisappear  = !charNull && PChar->status == STATUS_TYPE::DISAPPEAR;

    const auto plan = PlanUpdatePool(charNull, poolMismatch, isDisappear);
    if (plan.reject)
    {
        ShowWarning("CTreasurePool::UpdatePool() - PChar was null, or PTreasurePool mismatched.");
        return;
    }

    if (plan.pushTrophyLists)
    {
        for (auto& m_PoolItem : m_PoolItems)
        {
            PChar->pushPacket<GP_SERV_COMMAND_TROPHY_LIST>(&m_PoolItem, nullptr, true);
        }
    }
}

void CTreasurePool::flush()
{
    using treasurepoolhelpers::PlanFlush;

    const auto plan = PlanFlush(m_count);
    if (plan.runChecks)
    {
        const auto tick = timer::now() + treasure_checktime + 1s;

        for (uint8 i = 0; i < TREASUREPOOL_SIZE; ++i)
        {
            checkTreasureItem(tick, i);
        }
    }
}

/************************************************************************
 *                                                                       *
 *  Character refuses/votes for item in treasure pool                    *
 *                                                                       *
 ************************************************************************/

// Go pure half: treasurepool.PlanLotItemDisposition + HighestLotter (6595).
void CTreasurePool::lotItem(CCharEntity* PChar, uint8 SlotID, uint16 Lot)
{
    using treasurepoolhelpers::LotItemPreflight;
    using treasurepoolhelpers::PlanLotItemPreflight;

    const bool charNull      = PChar == nullptr;
    const bool poolMismatch  = !charNull && PChar->PTreasurePool != this;
    const bool slotOutOfRange = treasurepoolhelpers::IsSlotOutOfRange(SlotID);

    bool  itemNull       = true;
    uint8 freeSlots      = 0;
    bool  itemIsRare     = false;
    bool  alreadyHasItem = false;

    // Host only resolves slot-dependent facts when earlier gates can pass.
    if (!charNull && !poolMismatch && !slotOutOfRange)
    {
        const CItem* PItem = xi::items::lookup(m_PoolItems[SlotID].ID);
        itemNull           = PItem == nullptr;
        if (!itemNull)
        {
            freeSlots      = PChar->getStorage(LOC_INVENTORY)->GetFreeSlotsCount();
            itemIsRare     = PItem->hasFlag(ItemFlag::Rare);
            alreadyHasItem = charutils::HasItem(PChar, m_PoolItems[SlotID].ID);
        }
    }

    switch (PlanLotItemPreflight(charNull, poolMismatch, slotOutOfRange, itemNull, freeSlots, itemIsRare, alreadyHasItem))
    {
        case LotItemPreflight::RejectMember:
            ShowWarning("CTreasurePool::LotItem() - PChar was null, or PTreasurePool mismatched.");
            return;
        case LotItemPreflight::RejectSlot:
            return;
        case LotItemPreflight::RejectItem:
            ShowWarning(fmt::format("Player {} is trying to lot on an item that doesn't exist (PItem was nullptr) (Packet injection?)!", PChar->getName()).c_str());
            return;
        case LotItemPreflight::RejectFullInventory:
            ShowError(fmt::format("Player {} is trying to lot on item {} while full inventory (Packet injection)!", PChar->getName(), m_PoolItems[SlotID].ID));
            return;
        case LotItemPreflight::RejectRareOwned:
            ShowError(fmt::format("Player {} is trying to lot on item {} (Rare) while already holding one (Packet injection)! ", PChar->getName(), m_PoolItems[SlotID].ID));
            return;
        case LotItemPreflight::Proceed:
            break;
    }

    LotInfo li;
    li.lot    = Lot;
    li.member = PChar;

    m_PoolItems[SlotID].Lotters.emplace_back(li);

    // Find the highest lotter
    CCharEntity* highestLotter = nullptr;
    uint16       highestLot    = 0;
    for (const LotInfo& lotInfo : m_PoolItems[SlotID].Lotters)
    {
        if (treasurepoolhelpers::IsHigherLot(lotInfo.lot, highestLot))
        {
            highestLotter = lotInfo.member;
            highestLot    = treasurepoolhelpers::HigherLotSelection(highestLot, lotInfo.lot);
        }
    }

    // Player lots Item for XXX message (always host-side after a recorded lot).
    for (const auto& member : m_Members)
    {
        member->pushPacket<GP_SERV_COMMAND_TROPHY_SOLUTION>(highestLotter, highestLot, PChar, SlotID, Lot);
    }

    // if all lotters have lotted, evaluate immediately.
    const auto postLot = treasurepoolhelpers::PlanPostLot(m_PoolItems[SlotID].Lotters.size(), memberCount());
    if (postLot.evaluateImmediately)
    {
        checkTreasureItem(m_Tick, SlotID);
    }
}

// Go pure half: treasurepool.PlanPassItemDisposition (6595).
void CTreasurePool::passItem(CCharEntity* PChar, uint8 SlotID)
{
    using treasurepoolhelpers::PassItemPreflight;
    using treasurepoolhelpers::PlanPassItemPreflight;

    const bool charNull       = PChar == nullptr;
    const bool poolMismatch   = !charNull && PChar->PTreasurePool != this;
    const bool slotOutOfRange = treasurepoolhelpers::IsSlotOutOfRange(SlotID);

    switch (PlanPassItemPreflight(charNull, poolMismatch, slotOutOfRange))
    {
        case PassItemPreflight::RejectMember:
            ShowWarning("CTreasurePool::PassItem() - PChar was null, or PTreasurePool mismatched.");
            return;
        case PassItemPreflight::RejectSlot:
            return;
        case PassItemPreflight::Proceed:
            break;
    }

    LotInfo li;
    li.lot               = 0;
    li.member            = PChar;
    bool hasLottedBefore = false;

    // if this member has lotted on this item previously, set their lot to 0.
    for (auto& Lotter : m_PoolItems[SlotID].Lotters)
    {
        if (Lotter.member->id == PChar->id)
        {
            Lotter.lot      = 0;
            hasLottedBefore = true;
            break;
        }
    }

    if (!hasLottedBefore)
    {
        m_PoolItems[SlotID].Lotters.emplace_back(li);
    }

    // Find the highest lotter
    CCharEntity* highestLotter = nullptr;
    uint16       highestLot    = 0;
    for (const LotInfo& lotInfo : m_PoolItems[SlotID].Lotters)
    {
        if (lotInfo.lot > highestLot)
        {
            highestLotter = lotInfo.member;
            highestLot    = lotInfo.lot;
        }
    }

    uint16 PassedLot = 65535; // passed mask is FF FF
    // Player lots Item for XXX message
    for (const auto& member : m_Members)
    {
        member->pushPacket<GP_SERV_COMMAND_TROPHY_SOLUTION>(highestLotter, highestLot, PChar, SlotID, PassedLot);
    }

    // if all lotters have lotted, evaluate immediately.
    if (m_PoolItems[SlotID].Lotters.size() == memberCount())
    {
        checkTreasureItem(m_Tick, SlotID);
    }
}

bool CTreasurePool::hasLottedItem(CCharEntity* PChar, uint8 SlotID)
{
    if (SlotID >= TREASUREPOOL_SIZE)
    {
        return false;
    }

    for (const auto& lotter : m_PoolItems[SlotID].Lotters)
    {
        if (lotter.member->id == PChar->id)
        {
            return true;
        }
    }

    return false;
}

bool CTreasurePool::hasPassedItem(CCharEntity* PChar, uint8 SlotID)
{
    if (SlotID >= TREASUREPOOL_SIZE)
    {
        return false;
    }

    for (auto& lotter : m_PoolItems[SlotID].Lotters)
    {
        if (lotter.member->id == PChar->id)
        {
            return lotter.lot == 0;
        }
    }

    return false;
}

void CTreasurePool::checkItems(timer::time_point tick)
{
    if (m_count != 0)
    {
        if ((tick - m_Tick > treasure_checktime))
        {
            for (uint8 i = 0; i < TREASUREPOOL_SIZE; ++i)
            {
                checkTreasureItem(tick, i);
            }
            m_Tick = tick;
        }
    }
}

// Go pure half: treasurepool.PlanCheckTreasureGate/PlanCheckTreasureItem (6596).
void CTreasurePool::checkTreasureItem(timer::time_point tick, uint8 SlotID)
{
    if (m_PoolItems[SlotID].ID == 0)
    {
        return;
    }

    if ((tick - m_PoolItems[SlotID].TimeStamp) > treasure_livetime ||
        (memberCount() == 1 && m_Members[0]->getStorage(LOC_INVENTORY)->GetFreeSlotsCount() != 0) ||
        m_PoolItems[SlotID].Lotters.size() == memberCount())
    {
        // Find item's highest lotter
        LotInfo highestInfo;

        for (auto curInfo : m_PoolItems[SlotID].Lotters)
        {
            if (curInfo.lot > highestInfo.lot)
            {
                highestInfo = curInfo;
            }
        }

        // Check to see if we have any lotters (excluding anyone who passed)
        if (highestInfo.member != nullptr && highestInfo.lot != 0)
        {
            if (highestInfo.member->getStorage(LOC_INVENTORY)->GetFreeSlotsCount() != 0)
            {
                // add item as they have room!
                if (charutils::AddItem(highestInfo.member, LOC_INVENTORY, m_PoolItems[SlotID].ID, 1, true) != ERROR_SLOTID)
                {
                    treasureWon(highestInfo.member, SlotID);
                }
                else
                {
                    treasureError(highestInfo.member, SlotID);
                }
            }
            else
            {
                // drop the item
                treasureLost(SlotID);
            }
        }
        else
        {
            // No one has lotted on this item - Give to random member who has not passed
            std::vector<CCharEntity*> candidates;
            for (auto& member : m_Members)
            {
                if (charutils::HasItem(member, m_PoolItems[SlotID].ID) && xi::items::lookup(m_PoolItems[SlotID].ID)->hasFlag(ItemFlag::Rare))
                {
                    continue;
                }

                if (member->getStorage(LOC_INVENTORY)->GetFreeSlotsCount() != 0 && !hasPassedItem(member, SlotID))
                {
                    candidates.emplace_back(member);
                }
            }

            if (candidates.empty())
            {
                treasureLost(SlotID);
            }
            else
            {
                // select random member from this pool to give item to
                CCharEntity* PChar = candidates.at(xirand::GetRandomNumber(candidates.size()));
                if (charutils::AddItem(PChar, LOC_INVENTORY, m_PoolItems[SlotID].ID, 1, true) != ERROR_SLOTID)
                {
                    treasureWon(PChar, SlotID);
                }
                else
                {
                    treasureError(PChar, SlotID);
                }
            }
        }
    }
}

// Go pure half: treasurepool.PlanTreasureWon (6597).
void CTreasurePool::treasureWon(CCharEntity* winner, uint8 SlotID)
{
    if (winner == nullptr || winner->PTreasurePool != this || m_PoolItems[SlotID].ID == 0)
    {
        ShowWarning("CTreasurePool::TreasureError() - Winner, or Winner Treasure Pool mismatch, or Pool ID = 0.");
        return;
    }

    m_PoolItems[SlotID].TimeStamp = timer::start_time;

    for (const auto& member : m_Members)
    {
        member->pushPacket<GP_SERV_COMMAND_TROPHY_SOLUTION>(winner, SlotID, 0, GP_TROPHY_SOLUTION_STATE::Win);
    }
    m_count--;

    m_PoolItems[SlotID].ID = 0;
    m_PoolItems[SlotID].Lotters.clear();
}

// Go pure half: treasurepool.PlanTreasureError (6597).
void CTreasurePool::treasureError(CCharEntity* winner, uint8 SlotID)
{
    if (winner == nullptr || winner->PTreasurePool != this || m_PoolItems[SlotID].ID == 0)
    {
        ShowWarning("CTreasurePool::TreasureError() - Winner, or Winner Treasure Pool mismatch, or Pool ID = 0.");
        return;
    }

    m_PoolItems[SlotID].TimeStamp = timer::start_time;

    for (const auto& member : m_Members)
    {
        member->pushPacket<GP_SERV_COMMAND_TROPHY_SOLUTION>(winner, SlotID, -1, GP_TROPHY_SOLUTION_STATE::WinError);
    }
    m_count--;

    m_PoolItems[SlotID].ID = 0;
    m_PoolItems[SlotID].Lotters.clear();
}

// Go pure half: treasurepool.PlanTreasureLost (6597).
void CTreasurePool::treasureLost(uint8 SlotID)
{
    if (m_PoolItems[SlotID].ID == 0)
    {
        ShowWarning("Pool Items for SlotID (%d) was 0.", SlotID);
        return;
    }

    m_PoolItems[SlotID].TimeStamp = timer::start_time;

    for (const auto& member : m_Members)
    {
        member->pushPacket<GP_SERV_COMMAND_TROPHY_SOLUTION>(SlotID, GP_TROPHY_SOLUTION_STATE::WinError);
    }
    m_count--;

    m_PoolItems[SlotID].ID = 0;
    m_PoolItems[SlotID].Lotters.clear();
}
