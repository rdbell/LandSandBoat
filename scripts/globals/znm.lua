-----------------------------------
-- Zeni NM System + Helpers
--
-- Soultrapper         : !additem 18721
-- Blank Soul Plate    : !additem 18722
-- Soultrapper 2000    : !additem 18724
-- Blank HS Soul Plate : !additem 18725
-- Soul Plate          : !additem 2477
-- Sanraku & Ryo       : !pos -127.0 0.9 22.6 50
-----------------------------------
local ID = zones[xi.zone.AHT_URHGAN_WHITEGATE]
-----------------------------------
xi = xi or {}
xi.znm = xi.znm or {}

-----------------------------------
-- Sanraku's Interest and Recommended Fauna
-- Applies bonuses to soul plate zeni-value
-----------------------------------

-- Called during JstMidnight tick
xi.znm.sanrakuRotationPlan = function(interest, fauna, trades)
    return { interest = interest, fauna = fauna, trades = trades }
end

xi.znm.UpdateSanrakusMobs = function()
    local plan = xi.znm.sanrakuRotationPlan(
        math.random(#xi.znm.SANRAKUS_INTEREST),
        math.random(#xi.znm.SANRAKUS_FAUNA),
        math.random(0, 250)
    )

    SetServerVariable('[ZNM][Sanraku]Interest', plan.interest)
    SetServerVariable('[ZNM][Sanraku]Fauna', plan.fauna)
    SetServerVariable('[ZNM][Sanraku]Trades', plan.trades)
end

xi.znm.shouldRotateSanrakuTrades = function(currentTrades)
    return currentTrades >= 500
end

xi.znm.nextSanrakuTradeCount = function(currentTrades)
    return currentTrades + 1
end

xi.znm.serverPlateTradesPlan = function(currentTrades)
    if xi.znm.shouldRotateSanrakuTrades(currentTrades) then
        return { rotate = true }
    end

    return { trades = xi.znm.nextSanrakuTradeCount(currentTrades) }
end

xi.znm.shouldInitializeSanrakuSelection = function(selection)
    return selection == nil or selection == 0
end

xi.znm.selectSanrakuSelection = function(selection, initialSelection)
    if xi.znm.shouldInitializeSanrakuSelection(selection) then
        return initialSelection
    end

    return selection
end

xi.znm.serverPlateTrades = function()
    local currentTrades = GetServerVariable('[ZNM][Sanraku]Trades')
    local plan = xi.znm.serverPlateTradesPlan(currentTrades)

    if plan.rotate then
        xi.znm.UpdateSanrakusMobs()
    else
        SetServerVariable('[ZNM][Sanraku]Trades', plan.trades)
    end
end

-- Get Sanraku's 'Subject of Interest'
xi.znm.getSanrakusInterest = function()
    local interest = GetServerVariable('[ZNM][Sanraku]Interest')

    -- Initialize the server var if it hasn't been already
    if xi.znm.shouldInitializeSanrakuSelection(interest) then
        interest = xi.znm.selectSanrakuSelection(interest, math.random(#xi.znm.SANRAKUS_INTEREST))
        SetServerVariable('[ZNM][Sanraku]Interest', interest)
    end

    return interest
end

-- Get Sanraku's 'Recommended Fauna'
xi.znm.getSanrakusFauna = function()
    local fauna = GetServerVariable('[ZNM][Sanraku]Fauna')

    -- Initialize the server var if it hasn't been already
    if xi.znm.shouldInitializeSanrakuSelection(fauna) then
        fauna = xi.znm.selectSanrakuSelection(fauna, math.random(#xi.znm.SANRAKUS_FAUNA))
        SetServerVariable('[ZNM][Sanraku]Fauna', fauna)
    end

    return fauna
end

--- Is this mob Sanraku's current 'Recommended Fauna'?

-- Pure halves of the soul-plate valuation, taking the catalog rows directly so
-- they are testable without the Sanraku server variables or a player.

xi.znm.isCurrentFaunaRow = function(plateData, faunaRow)
    local zoneID   = plateData.zoneId
    local mobName  = plateData.signature

    if faunaRow.zone ~= zoneID then
        return false
    else
        if type(faunaRow.name) == 'table' then
            for iter = 1, #faunaRow.name do
                if faunaRow.name[iter] == mobName then
                    return true
                end
            end
        else
            if faunaRow.name == mobName then
                return true
            end
        end
    end

    return false
end

-- Main interest objective

xi.znm.isCurrentFamilyRow = function(plateData, interestRow, currectInterest)
    local family = plateData.familyId

    if family == interestRow.family then
        -- Handle elementals as all have same family
        if currectInterest >= 45 and currectInterest <= 51 then
            if plateData.signature ~= interestRow.name then
                return false
            end
        end

        return true
    end

    return false
end

-- Secondary interest objective
xi.znm.isCurrentEcosystemRow = function(plateData, interestRow)
    local family = plateData.familyId

    if utils.contains(family, interestRow.ecoSystem) then
        return true
    end

    return false
end

-- Server-variable-bound wrappers kept for existing callers.
xi.znm.isCurrentFauna = function(plateData)
    return xi.znm.isCurrentFaunaRow(plateData, xi.znm.SANRAKUS_FAUNA[xi.znm.getSanrakusFauna()])
end

xi.znm.isCurrentFamily = function(plateData)
    local currectInterest = xi.znm.getSanrakusInterest()
    return xi.znm.isCurrentFamilyRow(plateData, xi.znm.SANRAKUS_INTEREST[currectInterest], currectInterest)
end

xi.znm.isCurrentEcosystem = function(plateData)
    return xi.znm.isCurrentEcosystemRow(plateData, xi.znm.SANRAKUS_INTEREST[xi.znm.getSanrakusInterest()])
end

-- Which bonus a soul plate earns. Only the first match counts, so the richer
-- fauna bonus wins over family, which wins over ecosystem.
xi.znm.plateBonusKind = function(plateData, interestRow, currectInterest, faunaRow)
    if xi.znm.isCurrentFaunaRow(plateData, faunaRow) then
        return 'Fauna'
    elseif xi.znm.isCurrentFamilyRow(plateData, interestRow, currectInterest) then
        return 'family'
    elseif xi.znm.isCurrentEcosystemRow(plateData, interestRow) then
        return 'ecoSystem'
    end

    return 'none'
end

xi.znm.plateBonusZeni = function(bonusKind)
    if bonusKind == 'Fauna' then
        return xi.znm.SOULPLATE_FAUNA
    elseif bonusKind == 'family' then
        return xi.znm.SOULPLATE_INTEREST
    elseif bonusKind == 'ecoSystem' then
        return xi.znm.SOULPLATE_ECOSYSTEM
    end

    return 0
end

-- Final plate value: quality plus the bonus, thirded for low-level characters
-- so pictures are not farmed on mules, then clamped.
xi.znm.plateZeniValue = function(quality, bonusKind, mainLevel)
    local zeni = quality + xi.znm.plateBonusZeni(bonusKind)

    if mainLevel <= 10 then
        zeni = zeni / 3
    end

    return utils.clamp(zeni, xi.znm.SOULPLATE_MIN_VALUE, xi.znm.SOULPLATE_MAX_VALUE)
end

xi.znm.calculatePlateZeni = function(player, plateData)
    -- Cache the soulplate value on the player
    local currectInterest = xi.znm.getSanrakusInterest()
    local bonus           = xi.znm.plateBonusKind(
        plateData,
        xi.znm.SANRAKUS_INTEREST[currectInterest],
        currectInterest,
        xi.znm.SANRAKUS_FAUNA[xi.znm.getSanrakusFauna()]
    )

    utils.unused(bonus)

    -- to avoid pictures being handed to low level chars, adding this check
    -- low level chars get 1/3 less when they take the pic, customizing to also affect trade in.
    local zeni = xi.znm.plateZeniValue(plateData.quality, bonus, player:getMainLvl())

    -- if player:getDebugMode() then
    --     player:printToPlayer(string.format('name: %s zeni %i, bonus: %s', plateData.signature, zeni, bonus))
    -- end

    return zeni
end

-----------------------------------
-- Soultrapper
-----------------------------------

xi.znm.soultrapper = xi.znm.soultrapper or {}

-----------------------------------
-- onItemCheck
-----------------------------------

xi.znm.soultrapper.onItemCheck = function(target, item, caster)
    -- can not be used on non mobs or Structure type mobs
    if
        not target:isMob() or
        (target:getSpecies() >= 370 and target:getSpecies() <= 379) -- All structures
    then
        return xi.msg.basic.ITEM_CANNOT_USE_TARGET
    end

    if
        caster:hasStatusEffect(xi.effect.INVISIBLE) or
        caster:hasStatusEffect(xi.effect.SNEAK) or
        caster:hasStatusEffect(xi.effect.DEODORIZE) or
        caster:hasStatusEffect(xi.effect.HIDE) or
        caster:hasStatusEffect(xi.effect.CAMOUFLAGE)
    then
        return xi.msg.basic.ITEM_NO_USE_SNEAK, caster:getEquipID(xi.slot.RANGED)
    end

    local id = caster:getEquipID(xi.slot.AMMO)
    if
        id ~= xi.item.BLANK_SOUL_PLATE and
        id ~= xi.item.BLANK_HIGH_SPEED_SOUL_PLATE
    then
        return xi.msg.basic.ITEM_NO_ITEMS_EQUIPPED
    end

    if caster:getFreeSlotsCount() == 0 then
        return xi.msg.basic.FULL_INVENTORY
    end

    return 0
end

-----------------------------------
-- onItemUse
-----------------------------------

xi.znm.soultrapper.onItemUse = function(target, player, item)
    -- Soul plate not guaranteed
    -- to validate long term: some posts hint at level correction on success rate vs. higher level mobs.
    if math.random(100) > xi.znm.SOULTRAPPER_SUCCESS * xi.znm.SOULPLATE_HS_MULT then
        -- todo, message should show to all in area
        player:timer(4000, function(playerArg)
            playerArg:messageBasic(xi.msg.basic.SOULTRAPPER_FAILED)
        end)

        player:removeAmmo(1)
    else
        -- Deduct blank soul plate
        player:removeAmmo(1)

        -- Determine quality starting value
        local quality = xi.znm.soultrapper.getZeniValue(target, player)

        -- Pick a skill totally at random...
        local skillIndex, skillEntry = xi.pankration.getRandomFeralSkill(target)

        -- Add plate
        local plate = player:addItem({ id = xi.item.SOUL_PLATE, silent = true })
        if plate then
            plate:setExData({
                signature   = target:getName(),
                zoneId      = target:getZoneID(),
                familyId    = target:getFamily(),
                poolId      = target:getPool(),
                level       = target:getMainLvl(),
                quality     = quality,
                feralSkill  = skillIndex,
                feralPoints = skillEntry.fp,
            })
        end

        -- todo, message should show to all in area
        player:timer(4000, function(playerArg)
            playerArg:messageBasic(xi.msg.basic.SOULTRAPPER_SUCCESS, 0, xi.item.SOUL_PLATE)
        end)
    end
end

-----------------------------------
-- onItemUse Helpers
-----------------------------------

xi.znm.soultrapper.getZeniValue = function(target, player)
    local hpp      = target:getHPP()
    local isNM     = target:isNM()
    local distance = player:checkDistance(target)
    local isFacing = target:isFacing(player)
    local level    = target:getMainLvl()

    -- Starting value
    local zeni = 10

    -- Distance Component
    zeni = zeni * utils.clamp((1 / distance) * 8, 1, 1.5)

    -- Size Component
    zeni = zeni + (target:getHitboxSize() * 5) -- needs verification

    -- Angle/Facing Component
    if isFacing then
        zeni = zeni * xi.znm.SOULPLATE_FACING_MULT
    end

    -- HP% Component
    zeni = zeni * 1 + math.abs(hpp - 100) / 6

    if
        target:getSpecies() == xi.mobSpecies.EUVHI and
        target:getZoneID() == 33
    then -- Dahak and Aw'euvhi
        zeni = zeni + xi.znm.SOULPLATE_UNIQUE_AMOUNT
    elseif
        target:getSpecies() == xi.mobSpecies.CHIGOE or
        target:getSpecies() == xi.mobSpecies.DJIGGA
    then -- chigoe penalty
        zeni = zeni - xi.znm.SOULPLATE_UNIQUE_AMOUNT
    -- Generic NM/Rarity Component
    elseif isNM then
        if level >= 80 then
            zeni = zeni * xi.znm.SOULPLATE_HNM_MULT
        else
            zeni = zeni * xi.znm.SOULPLATE_NM_MULT
        end
    end

    -- level component. dependent on mob level, not player. need to validate level above 75 to see if the values are the same to prove/disprove
    if level < 75 then
        zeni = zeni - (75 - level)
    else
        zeni = zeni + (level - 75)
    end

    zeni = utils.clamp(zeni, xi.znm.SOULPLATE_MIN_VALUE, xi.znm.SOULPLATE_MAX_VALUE)

    -- Add a little randomness
    zeni = zeni + math.random(6)

    -- Having claim on the mob you take pictures of increases Zeni reward significantly.
    -- If another party has claim on the mob, you will only receive 1-5 Zeni for the pictures you take, even in the best possible situation.
    if not player:hasClaim(target) then
        zeni = math.max(1, zeni * 0.01)
    end

    if player:getMainLvl() <= 10 then
        zeni = zeni / 3
    end

    -- Sanitize Zeni
    zeni = math.floor(zeni) -- Remove any floating point information
    zeni = utils.clamp(zeni, xi.znm.SOULPLATE_MIN_VALUE, xi.znm.SOULPLATE_MAX_VALUE)

    return zeni
end

-----------------------------------
-- Ryo
-----------------------------------

xi.znm.ryo = xi.znm.ryo or {}

-----------------------------------
-- onTrade
-----------------------------------

xi.znm.ryo.onTrade = function(player, npc, trade)
    local outcome = xi.znm.ryo.tradeOutcome(npcUtil.tradeHasExactly(trade, xi.item.SOUL_PLATE))

    if outcome == 'plate_trade' then
        -- Cache the soulplate value on the player
        local item = trade:getItem(0)
        local zeni = xi.znm.calculatePlateZeni(player, item:getExData())
        xi.znm.ryo.setTradedPlateValue(player, zeni)

        player:startEvent(914)
    end
end

-----------------------------------
-- onTrigger
-----------------------------------

xi.znm.ryo.onTrigger = function(player, npc)
    local outcome = xi.znm.ryo.triggerOutcome(xi.znm.playerHasSpokenToSanrakuBefore(player))

    if outcome == 'menu' then
        player:startEvent(913)
    else
        player:showText(npc, ID.text.MASTER_FORBID)
    end
end

-----------------------------------
-- onEventUpdate
-----------------------------------

xi.znm.ryo.onEventUpdate = function(player, csid, option, npc)
    local outcome = xi.znm.ryo.eventUpdateOutcome(csid, option)

    if outcome == 'plate_value' then -- Get approximate value of traded soulplate
        local plan = xi.znm.ryo.plateValueUpdatePlan(xi.znm.ryo.tradedPlateValue(player))

        if plan.clearTradedPlateValue then
            xi.znm.ryo.setTradedPlateValue(player, 0)
        end
        player:updateEvent(plan.zeniValue)
    elseif outcome == 'interest' then -- 'Sanraku's subject of interest'
        local param = xi.znm.getSanrakusInterest()

        player:updateEvent(param, 0)
    elseif outcome == 'fauna' then -- 'Sanraku's recommended fauna'
        local param = xi.znm.getSanrakusFauna()

        player:updateEvent(param, 0)
    elseif outcome == 'zeni_balance' then -- 'My zeni balance'
        player:updateEvent(player:getCurrency('zeni_point'), 0)
    elseif outcome == 'zeni_status' then
        local nextZeniStatus = xi.znm.ryo.nextZeniStatus(option, player:getVar('ZeniStatus'))
        if nextZeniStatus then
            player:setVar('ZeniStatus', nextZeniStatus)
        else
            player:updateEvent(0, 0)
        end
    elseif outcome == 'menu' then
        player:updateEvent(xi.znm.ryo.menuParam(player:getVar('ZeniStatus'), player:getCurrency('zeni_point')))
    elseif outcome == 'default' then
        player:updateEvent(0, 0)
    end
end

xi.znm.ryo.onEventFinish = function(player, csid, option, npc)
    local outcome = xi.znm.ryo.eventFinishOutcome(csid)

    if outcome == 'clean_trade' then
        local item = player:getTrade():getItem()
        local plan = xi.znm.ryo.tradeCleanupPlan(item ~= nil)

        if plan.clearReservedValue then
            item:setReservedValue(0)
        end

        if plan.cleanTrade then
            player:getTrade():clean()
        end
    end
end

-----------------------------------
-- Ryo General Helpers
-----------------------------------

xi.znm.ryo.tradeOutcome = function(hasExactlySoulPlate)
    if hasExactlySoulPlate then
        return 'plate_trade'
    end
end

xi.znm.ryo.triggerOutcome = function(hasSpokenToSanraku)
    if hasSpokenToSanraku then
        return 'menu'
    end

    return 'forbidden'
end

xi.znm.ryo.eventUpdateOutcome = function(csid, option)
    if csid == 914 then
        return 'plate_value'
    elseif csid ~= 913 then
        return nil
    elseif option == 200 then
        return 'interest'
    elseif option == 201 then
        return 'fauna'
    elseif option == 300 then
        return 'zeni_balance'
    elseif option == 401 or option == 402 then
        return 'zeni_status'
    elseif option == 404 then
        return 'menu'
    end

    return 'default'
end

xi.znm.ryo.eventFinishOutcome = function(csid)
    if csid == 914 then
        return 'clean_trade'
    end
end

xi.znm.ryo.tradeCleanupPlan = function(hasTradeItem)
    return { clearReservedValue = hasTradeItem, cleanTrade = true }
end

xi.znm.ryo.menuParam = function(zeniStatus, zeni)
    local menuOptions = 175

    if zeniStatus >= 2 then -- add 'sanrakus subject of interest' and 'recommended fauna'
        menuOptions = menuOptions - 12
    end

    if zeni ~= 0 then -- add 'whats zeni' and 'my zeni balance' and 'islet's'
        menuOptions = menuOptions - 131
        if zeni >= 1000 then
            menuOptions = menuOptions - 32
        end
    end

    return menuOptions
end

xi.znm.ryo.nextZeniStatus = function(option, zeniStatus)
    if option == 401 and zeniStatus == 1 then
        return 2
    elseif option == 402 and zeniStatus == 2 then -- ask about gaining access to islet's
        return 3
    end
end

xi.znm.ryo.plateValueUpdatePlan = function(zeniValue)
    return { zeniValue = zeniValue, clearTradedPlateValue = true }
end

xi.znm.ryo.tradedPlateValue = function(player)
    return player:getLocalVar('[ZNM][Ryo]SoulPlateValue')
end

xi.znm.ryo.setTradedPlateValue = function(player, zeni)
    player:setLocalVar('[ZNM][Ryo]SoulPlateValue', zeni)
end

-----------------------------------
-- Sanraku
-----------------------------------

xi.znm.sanraku = xi.znm.sanraku or {}

-----------------------------------
-- onTrade
-----------------------------------

xi.znm.sanraku.onTrade = function(player, npc, trade)
    if trade:getItemCount() == 1 then -- One soul plate or trophy at a time
        local item = trade:getItem(0)

        if trade:getItemId() == xi.item.SOUL_PLATE then
            xi.znm.sanraku.handleTradeWithPlate(player, npc, item)
        else -- Check Trophy trading (for ZNM seals)
            xi.znm.sanraku.handleTradeWithTrophy(player, npc, item)
        end
    end
end

-----------------------------------
-- onTrade Helpers
-----------------------------------

xi.znm.sanraku.plateTradeAdmissionPlan = function(hasRhapsodyInAzure, tradedPlates, tradeLimit)
    if hasRhapsodyInAzure then
        return { resetTracking = true, accept = true }
    end

    return { accept = tradedPlates < tradeLimit }
end

xi.znm.sanraku.handleTradeWithPlate = function(player, npc, item)
    local hasRhapsody = player:hasKeyItem(xi.ki.RHAPSODY_IN_AZURE)
    local tradeLimit  = xi.znm.SOULPLATE_TRADE_LIMIT
    local tradedPlates = 0

    if not hasRhapsody then
        tradedPlates = xi.znm.sanraku.platesTradedToday(player)
    end

    local plan = xi.znm.sanraku.plateTradeAdmissionPlan(hasRhapsody, tradedPlates, tradeLimit)

    if plan.resetTracking then
        xi.znm.resetDailyTrackingVars(player)
    elseif not plan.accept then
        player:showText(npc, ID.text.APPRECIATE_MORE, 1, xi.item.SOUL_PLATE, tradeLimit)
        local tradeItem = player:getTrade():getItem()
        if tradeItem then
            tradeItem:setReservedValue(0)
        end

        player:getTrade():clean()
        return
    end

    -- Cache the soulplate value on the player
    local zeni = xi.znm.calculatePlateZeni(player, item:getExData())
    xi.znm.sanraku.setTradedPlateValue(player, zeni)
    xi.znm.serverPlateTrades()

    player:startEvent(910, zeni)
end

xi.znm.plateTradeDayPlan = function(currentDay, storedDay, tradedPlates)
    if currentDay ~= storedDay then
        return { reset = true, tradedPlates = 0 }
    end

    return { tradedPlates = tradedPlates }
end

xi.znm.sanraku.platesTradedToday = function(player)
    local currentDay = VanadielUniqueDay()
    local storedDay  = xi.znm.playerTradingDay(player)
    local plan       = xi.znm.plateTradeDayPlan(currentDay, storedDay, xi.znm.numberOfTradedPlates(player))

    if plan.reset then
        xi.znm.resetDailyTrackingVars(player)
    end

    return plan.tradedPlates
end

xi.znm.sanraku.handleTradeWithTrophy = function(player, npc, item)
    local znmSeal = xi.znm.TROPHIES[item:getID()]
    local plan = xi.znm.sanraku.trophyTradePlan(znmSeal, znmSeal and player:hasKeyItem(znmSeal))

    if plan.kind == 'already_owned' then
        player:showText(npc, ID.text.SINGLE_TALLY)
    elseif plan.kind == 'stage' then
        xi.znm.sanraku.setTradedTrophySeal(player, plan.seal)
        player:startEvent(912, 0, 0, 1, plan.seal)
    end
end

xi.znm.sanraku.trophyTradePlan = function(seal, hasSeal)
    if not seal then
        return { kind = 'ignore' }
    elseif hasSeal then
        return { kind = 'already_owned' }
    end

    return { kind = 'stage', seal = seal }
end

-----------------------------------
-- onTrigger
-----------------------------------

xi.znm.sanraku.triggerOutcome = function(hasSpokenBefore, hasZeni)
    if not hasSpokenBefore then
        return 'introduction'
    elseif not hasZeni then
        return 'no_zeni'
    end

    return 'menu'
end

xi.znm.sanraku.onTrigger = function(player, npc)
    -- ZNM and Zeni Ineractions
    local outcome = xi.znm.sanraku.triggerOutcome(
        xi.znm.playerHasSpokenToSanrakuBefore(player),
        player:getCurrency('zeni_point') ~= 0
    )

    if outcome == 'introduction' then
        player:startEvent(908)
    elseif outcome == 'no_zeni' then
        player:showText(npc, ID.text.HOPES_REST)
    else
        local param = xi.znm.sanraku.menu(player)
        player:startEvent(909, param)
    end
end

-----------------------------------
-- onTrigger Helpers
-----------------------------------

-- Update Sanraku's ZNM menu (csid 909) based on owned seals
xi.znm.sanraku.menu = function(player)
    -- Default: Tier 1 ZNMs + 'Don't Ask'
    -- (if bit = 0: add ZNM to Sanraku's Menu)
    local param = xi.znm.DefaultMenu

    for bitmask, seal in pairs(xi.znm.MENU_BITMASKS) do
        -- Check for each key item
        if type(seal) == 'table' then -- Higher tier ZNMs require 3 seals
            if
                player:hasKeyItem(seal[1]) and
                player:hasKeyItem(seal[2]) and
                player:hasKeyItem(seal[3])
            then
                param = bit.band(param, bit.bnot(bitmask))
            end
        else
            if player:hasKeyItem(seal) then
                param = bit.band(param, bit.bnot(bitmask))
            end
        end
    end

    return param
end

-----------------------------------
-- onEventUpdate
-----------------------------------

xi.znm.sanraku.eventUpdateOutcome = function(csid, option)
    if csid ~= 909 then
        return nil
    elseif option == 1 or option == 500 then
        return 'islets_menu'
    elseif option >= 300 and option <= 302 then
        return 'islets_access'
    elseif option >= 100 and option <= 130 then
        return 'confirm_info'
    elseif option >= 400 and option <= 440 then
        return 'confirmed_info'
    end
end

xi.znm.sanraku.isletsMenuParam = function(zeniStatus)
    return zeniStatus >= 3 and 1 or 0
end

xi.znm.sanraku.onEventUpdate = function(player, csid, option, npc)
    local outcome = xi.znm.sanraku.eventUpdateOutcome(csid, option)

    if outcome == 'islets_menu' then
        player:updateEvent(xi.znm.sanraku.isletsMenuParam(player:getVar('ZeniStatus')))
    elseif outcome == 'islets_access' then
        xi.znm.sanraku.handleGainingAccessToIslets(player, option)
    elseif outcome == 'confirm_info' then
        xi.znm.sanraku.handleConfirmingDesiredZNMInfo(player, option)
    elseif outcome == 'confirmed_info' then
        xi.znm.sanraku.handleConfirmedZNMInfo(player, option)
    end
end

-----------------------------------
-- onEventUpdate Helpers
-----------------------------------

xi.znm.sanraku.isletAccessPlan = function(hasRhapsodyInAzure, option)
    local zeniCost = hasRhapsodyInAzure and 50 or 500
    local keyItem = xi.ki.SICKLEMOON_SALT + option - 300

    return { zeniCost = zeniCost, keyItem = keyItem }
end

xi.znm.sanraku.isletAccessOutcome = function(hasEnoughZeni, hasSalt)
    if not hasEnoughZeni then
        return 'no_zeni'
    elseif hasSalt then
        return 'already_owned'
    end

    return 'purchase'
end

xi.znm.sanraku.handleGainingAccessToIslets = function(player, option)
    local plan = xi.znm.sanraku.isletAccessPlan(player:hasKeyItem(xi.keyItem.RHAPSODY_IN_AZURE), option)
    local zeniCost = plan.zeniCost
    local keyItem = plan.keyItem

    local hasEnoughZeni = player:getCurrency('zeni_point') >= zeniCost
    local hasSalt = false
    if hasEnoughZeni then
        hasSalt = player:hasKeyItem(keyItem)
    end

    local outcome = xi.znm.sanraku.isletAccessOutcome(hasEnoughZeni, hasSalt)

    if outcome == 'no_zeni' then
        player:updateEvent(2)
    elseif outcome == 'already_owned' then
        player:showText(GetNPCByID(ID.npc.SANRAKU), ID.text.ALREADY_IN_POSSESSION)
    else
        player:addKeyItem(keyItem)
        player:delCurrency('zeni_point', zeniCost)
        player:updateEvent(1, zeniCost, 0, keyItem)
    end
end

xi.znm.sanraku.handleConfirmingDesiredZNMInfo = function(player, option)
    -- Give the correct ZNM's zeni cost
    local diff      = xi.znm.sanraku.confirmingZNMPopIndex(option)
    local zeniCost = xi.znm.getPopPrice(xi.znm.POP_ITEMS[diff].mob, xi.znm.POP_ITEMS[diff].tier)

    player:updateEvent(0, 0, 0, 0, 0, 0, zeniCost)
end

xi.znm.sanraku.confirmingZNMPopIndex = function(option)
    return option - 99
end

xi.znm.sanraku.confirmedZNMPopIndex = function(option)
    return math.min(option - 399, 31)
end

xi.znm.sanraku.confirmedZNMPopPurchaseOutcome = function(hasEnoughZeni, hasFreeSlots, hasPopItem)
    if not hasEnoughZeni then
        return 'no_zeni'
    elseif not hasFreeSlots or hasPopItem then
        return 'unavailable'
    end

    return 'purchase'
end

xi.znm.sanraku.handleConfirmedZNMInfo = function(player, option)
    -- (440 because Warden's option is offset by 10 for some reason)
    local diff     = xi.znm.sanraku.confirmedZNMPopIndex(option) -- Determine the desired ZNM
    local popItem  = xi.znm.POP_ITEMS[diff].item
    local znmTier  = xi.znm.POP_ITEMS[diff].tier
    local mob      = xi.znm.POP_ITEMS[diff].mob
    local zeniCost = xi.znm.getPopPrice(mob, znmTier)

    local hasEnoughZeni = player:getCurrency('zeni_point') >= zeniCost
    local hasFreeSlots = false
    local hasPopItem = false

    if hasEnoughZeni then
        hasFreeSlots = player:getFreeSlotsCount() ~= 0
        if hasFreeSlots then
            hasPopItem = player:hasItem(popItem)
        end
    end

    local outcome = xi.znm.sanraku.confirmedZNMPopPurchaseOutcome(hasEnoughZeni, hasFreeSlots, hasPopItem)

    if outcome == 'no_zeni' then
        player:updateEvent(2)
    elseif outcome == 'unavailable' then
        player:updateEvent(4)
    else
        -- Deduct zeni from player, increase future pop-item costs
        player:delCurrency('zeni_point', zeniCost)
        xi.znm.updatePopPrice(mob, znmTier)

        -- Give the pop item and remove the corresponding seal(s), if applicable
        player:addItem(popItem)

        local seal = xi.znm.POP_ITEMS[diff].seal

        if type(seal) == 'table' then -- Three-seal ZNMs (Tinnin, etc.)
            player:delKeyItem(seal[1])
            player:delKeyItem(seal[2])
            player:delKeyItem(seal[3])
            player:updateEvent(1, zeniCost, popItem, seal[1], seal[2], seal[3])
        elseif seal == 0 then -- Tier 1s have no seal
            player:updateEvent(1, zeniCost, popItem)
        else -- One-seal ZNMs
            player:delKeyItem(seal)
            player:updateEvent(1, zeniCost, popItem, seal)
        end
    end
end

-----------------------------------
-- onEventFinish
-----------------------------------

xi.znm.sanraku.eventFinishOutcome = function(csid)
    if csid == 910 then
        return 'complete_plate_trade'
    elseif csid == 908 then
        return 'mark_introduction_seen'
    elseif csid == 912 then
        return 'complete_trophy_trade'
    end
end

xi.znm.sanraku.onEventFinish = function(player, csid, option, npc)
    local outcome = xi.znm.sanraku.eventFinishOutcome(csid)

    if outcome == 'complete_plate_trade' then
        xi.znm.sanraku.handleCompletedTradeWithPlate(player)
    elseif outcome == 'mark_introduction_seen' then
        xi.znm.setPlayerHasSpokenToSanrakuBefore(player)
    elseif outcome == 'complete_trophy_trade' then
        xi.znm.sanraku.handleCompletedTradeWithTrophy(player)
    end
end

-----------------------------------
-- onEventFinish Helpers
-----------------------------------

xi.znm.sanraku.completedPlateTradePlan = function(day, zeniValue)
    return { day = day, zeniValue = zeniValue, clearTradedPlateValue = true }
end

xi.znm.sanraku.handleCompletedTradeWithPlate = function(player)
    local plan = xi.znm.sanraku.completedPlateTradePlan(
        VanadielUniqueDay(),
        xi.znm.sanraku.tradedPlateValue(player)
    )

    player:tradeComplete()
    xi.znm.setPlayerTradingDay(player, plan.day)
    xi.znm.incrementTradedPlates(player)

    if plan.clearTradedPlateValue then
        xi.znm.sanraku.setTradedPlateValue(player, 0)
    end

    player:addCurrency('zeni_point', plan.zeniValue)
end

xi.znm.sanraku.completedTrophyTradePlan = function(seal)
    return { seal = seal, clearTradedTrophySeal = true }
end

xi.znm.sanraku.handleCompletedTradeWithTrophy = function(player)
    local plan = xi.znm.sanraku.completedTrophyTradePlan(xi.znm.sanraku.tradedTrophySeal(player))

    player:tradeComplete()
    player:addKeyItem(plan.seal)
    if plan.clearTradedTrophySeal then
        xi.znm.sanraku.setTradedTrophySeal(player, 0)
    end
end

-----------------------------------
-- Sanraku General Helpers
-----------------------------------

xi.znm.sanraku.tradedPlateValue = function(player)
    return player:getLocalVar('[ZNM][Sanraku]SoulPlateValue')
end

xi.znm.sanraku.setTradedPlateValue = function(player, zeni)
    player:setLocalVar('[ZNM][Sanraku]SoulPlateValue', zeni)
end

xi.znm.sanraku.tradedTrophySeal = function(player)
    return player:getLocalVar('[ZNM]TrophyTrade')
end

xi.znm.sanraku.setTradedTrophySeal = function(player, trophy)
    player:setLocalVar('[ZNM]TrophyTrade', trophy)
end

-----------------------------------
---- ZNM Pop-Item Prices
-----------------------------------

xi.znm.getPopPrice = function(mob, znmTier)
    local popCost = GetServerVariable('[ZNM][' .. mob .. ']PopCost')
    local plan = xi.znm.popPricePlan(popCost, xi.znm.ZNM_POP_COSTS[znmTier].minPrice)

    if plan.initialize then
        SetServerVariable('[ZNM][' .. mob .. ']PopCost', plan.price)
    end

    return plan.price
end

xi.znm.popPricePlan = function(popCost, minPrice)
    if popCost == nil or popCost == 0 then
        return { price = minPrice, initialize = true }
    end

    return { price = popCost }
end

-- pop prices update per purchase at the mob level
xi.znm.updatePopPrice = function(mob, znmTier)
    if not xi.znm.ZNM_STATIC_POP_PRICES then
        local popCost = math.min(xi.znm.getPopPrice(mob, znmTier) + xi.znm.ZNM_POP_COSTS[znmTier].addedPrice,
            xi.znm.ZNM_POP_COSTS[znmTier].maxPrice)
        SetServerVariable('[ZNM][' .. mob .. ']PopCost', popCost)
    end
end

-- Prices decay over time (called every 2 hours)
xi.znm.ZNMPopPriceDecay = function()
    if not xi.znm.ZNM_STATIC_POP_PRICES then
        local popCost = 0
        local mob     = nil
        local znmTier = 1

        for i = 1, 31 do
            mob     = xi.znm.POP_ITEMS[i].mob
            znmTier = xi.znm.POP_ITEMS[i].tier
            popCost = math.max(xi.znm.getPopPrice(mob, znmTier) - xi.znm.ZNM_POP_COSTS[znmTier].decayPrice,
                xi.znm.ZNM_POP_COSTS[znmTier].minPrice)

            SetServerVariable('[ZNM][' .. mob .. ']PopCost', popCost)
        end
    end
end

-----------------------------------
---- General Utility Helpers
-----------------------------------

xi.znm.playerHasSpokenToSanrakuBefore = function(player)
    return player:getVar('ZeniStatus') >= 1
end

xi.znm.setPlayerHasSpokenToSanrakuBefore = function(player)
    player:setVar('ZeniStatus', 1)
end

xi.znm.playerTradingDay = function(player)
    return player:getVar('[ZNM][Sanraku]TradingDay')
end

xi.znm.setPlayerTradingDay = function(player, day)
    player:setVar('[ZNM][Sanraku]TradingDay', day)
end

xi.znm.numberOfTradedPlates = function(player)
    return player:getVar('[ZNM][Sanraku]TradedPlates')
end

xi.znm.incrementTradedPlates = function(player)
    player:incrementCharVar('[ZNM][Sanraku]TradedPlates', 1)
end

xi.znm.dailyTrackingResetPlan = function()
    return { tradingDay = 0, tradedPlates = 0 }
end

xi.znm.resetDailyTrackingVars = function(player)
    local plan = xi.znm.dailyTrackingResetPlan()
    player:setVar('[ZNM][Sanraku]TradingDay', plan.tradingDay)
    player:setVar('[ZNM][Sanraku]TradedPlates', plan.tradedPlates)
end
