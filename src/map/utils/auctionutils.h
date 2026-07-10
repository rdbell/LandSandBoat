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

#pragma once

#include "common/cbasetypes.h"

struct GP_AUC_PARAM_BID;
struct GP_AUC_PARAM_ASKCOMMIT;
struct GP_AUC_PARAM_LOT;
class CCharEntity;

namespace auctionutils
{

namespace detail
{
auto AuctionFee(uint32 limitPrice, uint32 baseFee, float taxRate, uint32 maxFee) -> uint32;
auto TransactionQuantity(bool stack, uint32 stackSize) -> uint32;
} // namespace detail

void SellingItems(CCharEntity* PChar, GP_AUC_PARAM_ASKCOMMIT param);
void OpenListOfSales(CCharEntity* PChar);
void RetrieveListOfItemsSoldByPlayer(CCharEntity* PChar);
void ProofOfPurchase(CCharEntity* PChar, GP_AUC_PARAM_LOT param);
auto PurchasingItems(CCharEntity* PChar, GP_AUC_PARAM_BID param) -> bool;
void CancelSale(CCharEntity* PChar, int8_t AucWorkIndex);
void UpdateSaleListByPlayer(CCharEntity* PChar, int8_t AucWorkIndex);

}; // namespace auctionutils
