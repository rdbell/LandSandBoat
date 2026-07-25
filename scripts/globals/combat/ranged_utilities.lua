-----------------------------------
-- Handles the ranged attack and accuracy penalties based on distance from target.
-- Pure injects dual-wired to OmegaXI internal/rangeddist (slice 6698 / 0845 / 6102).
-----------------------------------
xi = xi or {}
xi.combat = xi.combat or {}
xi.combat.ranged = xi.combat.ranged or {}
-----------------------------------

xi.combat.ranged.maxInnerPenalty = 25
xi.combat.ranged.maxOuterPenalty = 20
xi.combat.ranged.maxDistance = 25
xi.combat.ranged.unlimitedShotRecycleChance = 100

-- This table provides the default sweet spot ranges for various weapon types, agnostic of mob sizes altogether
xi.combat.ranged.sweetSpotDefaults = {
    ['throwing'] = { 0.0, 1.3 },
    ['cannon'  ] = { 3.0, 4.3 }, -- needs re-verification
    ['gun'     ] = { 3.0, 4.3 },
    ['shortbow'] = { 4.0, 6.4 },
    ['crossbow'] = { 5.0, 8.4 },
    ['longbow' ] = { 6.0, 9.5 },
}

-- This table provides the sweet spot ranges for weapons, assuming a mob size of 1
-- TODO: this needs re-verification due to better understanding of hitbox sizes
xi.combat.ranged.sweetSpots = {
    [xi.item.YOICHINOYUMI_75               ] = { 5.5, 9.5 },
    [xi.item.YOICHINOYUMI_80               ] = { 5.5, 9.5 },
    [xi.item.YOICHINOYUMI_85               ] = { 5.5, 9.5 },
    [xi.item.YOICHINOYUMI_90               ] = { 5.5, 9.5 },
    [xi.item.YOICHINOYUMI_95               ] = { 5.5, 9.5 },
    [xi.item.YOICHINOYUMI_99               ] = { 5.5, 9.5 },
    [xi.item.YOICHINOYUMI_99_II            ] = { 5.5, 9.5 },
    [xi.item.YOICHINOYUMI_119              ] = { 5.5, 9.5 },
    [xi.item.YOICHINOYUMI_119_II           ] = { 5.5, 9.5 },
    [xi.item.YOICHINOYUMI_119_III          ] = { 5.5, 9.5 },
    [xi.item.YOICHINOYUMI_119_III_NO_QUIVER] = { 5.5, 9.5 },
}

-----------------------------------
-- Pure injects
-----------------------------------

-- Pure getSweetSpotByAttacker once equip fields are injected.
-- params: hasWeapon, weaponId, weaponSkillType, weaponSubSkillType
-- returns { start, end }
xi.combat.ranged.resolveSweetSpotFromParams = function(params)
    local weaponId = params.weaponId or 0
    local sweetSpotForWeaponId = xi.combat.ranged.sweetSpots[weaponId]
    if sweetSpotForWeaponId ~= nil then
        return sweetSpotForWeaponId
    end

    if not params.hasWeapon then
        return xi.combat.ranged.sweetSpotDefaults['throwing']
    end

    local weaponSkillType = params.weaponSkillType
    local weaponSubSkillType = params.weaponSubSkillType

    if weaponSkillType == xi.skill.ARCHERY and weaponSubSkillType == 4 then
        return xi.combat.ranged.sweetSpotDefaults['longbow']
    elseif weaponSkillType == xi.skill.ARCHERY and weaponSubSkillType == 0 then
        return xi.combat.ranged.sweetSpotDefaults['shortbow']
    elseif weaponSkillType == xi.skill.MARKSMANSHIP and weaponSubSkillType == 0 then
        return xi.combat.ranged.sweetSpotDefaults['crossbow']
    elseif weaponSkillType == xi.skill.MARKSMANSHIP and weaponSubSkillType == 1 then
        return xi.combat.ranged.sweetSpotDefaults['gun']
    elseif weaponSkillType == xi.skill.MARKSMANSHIP and weaponSubSkillType == 2 then
        return xi.combat.ranged.sweetSpotDefaults['cannon']
    end

    return xi.combat.ranged.sweetSpotDefaults['throwing']
end

-- Pure centroid expansion: sweet band + defender and attacker hitboxes.
xi.combat.ranged.centroidFromParams = function(params)
    local sweet = params.sweetSpot or { 0, 0 }
    local pad = (params.defenderHitbox or 0) + (params.attackerHitbox or 0)

    return sweet[1] + pad, sweet[2] + pad
end

-- Pure attackDistancePenalty once PC gate, sweet spot, distance, hitboxes, cSkillMax injected.
-- params: isPC, distance, sweetSpot {start,end}, defenderHitbox, attackerHitbox, cSkillMax
xi.combat.ranged.attackDistancePenaltyFromParams = function(params)
    if not params.isPC then
        return 0
    end

    local distance = params.distance or 0
    local centroidStart, centroidEnd = xi.combat.ranged.centroidFromParams(params)
    local cSkillMax = params.cSkillMax or 0

    local penaltyPercentage
    if distance < centroidStart then
        -- Linear interpolation between 0 and centroidStart of maxInnerPenalty
        penaltyPercentage = -xi.combat.ranged.maxInnerPenalty +
            (xi.combat.ranged.maxInnerPenalty * (distance / centroidStart))
    elseif distance <= centroidEnd then
        -- No penalty in sweet spot
        penaltyPercentage = 0
    else
        -- Linear interpolation between centroidEnd and maxDistance of maxOuterPenalty
        penaltyPercentage = xi.combat.ranged.maxOuterPenalty *
            (distance - centroidEnd) / (xi.combat.ranged.maxDistance - centroidEnd)
    end

    return math.abs(math.ceil((penaltyPercentage / 100) * cSkillMax))
end

-- Pure accuracyDistancePenalty once PC gate, sweet spot end, distance, hitboxes, mainLvl injected.
-- params: isPC, distance, sweetSpot {start,end}, defenderHitbox, attackerHitbox, mainLvl
xi.combat.ranged.accuracyDistancePenaltyFromParams = function(params)
    if not params.isPC then
        return 0
    end

    local distance = params.distance or 0
    local _, centroidEnd = xi.combat.ranged.centroidFromParams(params)

    if distance <= centroidEnd then
        return 0
    end

    -- Linear interpolation between centroidEnd and maxDistance
    local penaltyPercentage = (distance - centroidEnd) / (xi.combat.ranged.maxDistance - centroidEnd)
    return math.abs(math.floor(penaltyPercentage * ((params.mainLvl or 0) / 2)))
end

-- Pure shouldUseAmmo once PC / recycle injects and d100 roll are known.
-- params: isPC, recycleMod, recycleMerit, ammoConsumptionJP, hasUnlimitedShot, roll1to100
-- returns useAmmo (bool), deleteUnlimitedShot (bool)
xi.combat.ranged.shouldUseAmmoFromParams = function(params)
    if not params.isPC then
        return false, false
    end

    local deleteUnlimitedShot = params.hasUnlimitedShot and true or false
    local recycleChance = (params.recycleMod or 0) + (params.recycleMerit or 0) +
        (params.ammoConsumptionJP or 0)

    if params.hasUnlimitedShot then
        recycleChance = xi.combat.ranged.unlimitedShotRecycleChance
    end

    if (params.roll1to100 or 0) <= recycleChance then
        return false, deleteUnlimitedShot
    end

    return true, deleteUnlimitedShot
end

-----------------------------------
-- Entity hosts (inject → pure)
-----------------------------------

xi.combat.ranged.getSweetSpotByAttacker = function(attacker)
    local weapon = attacker:getEquippedItem(xi.slot.RANGED)
    local hasWeapon = weapon ~= nil
    local weaponSkillType = 0
    local weaponSubSkillType = 0
    local weaponId = 0

    if hasWeapon then
        weaponSkillType = attacker:getWeaponSkillType(xi.slot.RANGED)
        weaponSubSkillType = attacker:getWeaponSubSkillType(xi.slot.RANGED)
        weaponId = weapon:getID()
    end

    return xi.combat.ranged.resolveSweetSpotFromParams({
        hasWeapon          = hasWeapon,
        weaponId           = weaponId,
        weaponSkillType    = weaponSkillType,
        weaponSubSkillType = weaponSubSkillType,
    })
end

xi.combat.ranged.attackDistancePenalty = function(attacker, defender)
    if not attacker:isPC() then
        return 0
    end

    local sweetSpot = xi.combat.ranged.getSweetSpotByAttacker(attacker)

    return xi.combat.ranged.attackDistancePenaltyFromParams({
        isPC            = true,
        distance        = attacker:checkDistance(defender),
        sweetSpot       = sweetSpot,
        defenderHitbox  = defender:getHitboxSize(),
        attackerHitbox  = attacker:getHitboxSize(),
        cSkillMax       = attacker:getMaxSkillLevel(attacker:getMainLvl(), xi.job.WAR, xi.skill.EVASION),
    })
end

xi.combat.ranged.accuracyDistancePenalty = function(attacker, defender)
    if not attacker:isPC() then
        return 0
    end

    local sweetSpot = xi.combat.ranged.getSweetSpotByAttacker(attacker)

    return xi.combat.ranged.accuracyDistancePenaltyFromParams({
        isPC            = true,
        distance        = attacker:checkDistance(defender),
        sweetSpot       = sweetSpot,
        defenderHitbox  = defender:getHitboxSize(),
        attackerHitbox  = attacker:getHitboxSize(),
        mainLvl         = attacker:getMainLvl(),
    })
end

xi.combat.ranged.shouldUseAmmo = function(attacker)
    if not attacker:isPC() then
        return false
    end

    local hasUnlimitedShot = attacker:hasStatusEffect(xi.effect.UNLIMITED_SHOT)
    local useAmmo, deleteUnlimitedShot = xi.combat.ranged.shouldUseAmmoFromParams({
        isPC               = true,
        recycleMod         = attacker:getMod(xi.mod.RECYCLE),
        recycleMerit       = attacker:getMerit(xi.merit.RECYCLE),
        ammoConsumptionJP  = attacker:getJobPointLevel(xi.jp.AMMO_CONSUMPTION),
        hasUnlimitedShot   = hasUnlimitedShot,
        roll1to100         = math.random(1, 100),
    })

    -- Host residual: delete Unlimited Shot when pure marks it (LSB always deletes
    -- when present; TODO: allegedly Unlimited Shot doesn't remove itself unless you hit)
    if deleteUnlimitedShot then
        attacker:delStatusEffect(xi.effect.UNLIMITED_SHOT)
    end

    return useAmmo
end
