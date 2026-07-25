-----------------------------------
-- White Mage Job Utilities
-- Dual-wired pure inject forms (slice 6731 / 0895):
--   isRemovable, benedictionHeal/Clamped/RemovesDoom,
--   checkDevotion/MartyrFromParams, devotion*/martyr* products,
--   oneHourRecastFromParams, fixed effect param pins
-- Parity: internal/whitemage
-----------------------------------
xi = xi or {}
xi.job_utils = xi.job_utils or {}
xi.job_utils.white_mage = xi.job_utils.white_mage or {}

-----------------------------------
-- Pure inject pins (internal/whitemage)
-----------------------------------
xi.job_utils.white_mage.afflatusPower            = 8
xi.job_utils.white_mage.afflatusDuration         = 7200
xi.job_utils.white_mage.asylumPower              = 3
xi.job_utils.white_mage.asylumDuration           = 30
xi.job_utils.white_mage.divineSealPower          = 1
xi.job_utils.white_mage.divineSealDuration       = 60
xi.job_utils.white_mage.divineCaressPower        = 3
xi.job_utils.white_mage.divineCaressDuration     = 60
xi.job_utils.white_mage.sacrosanctityPower       = 3
xi.job_utils.white_mage.sacrosanctityDuration    = 60
xi.job_utils.white_mage.benedictionDoomChance    = 33
xi.job_utils.white_mage.devotionMeritBase        = 5
xi.job_utils.white_mage.devotionBaseMPPercent    = 25
xi.job_utils.white_mage.devotionHPCostFraction   = 0.25
xi.job_utils.white_mage.martyrMeritBase          = 5
xi.job_utils.white_mage.martyrBaseHPPercent      = 200
xi.job_utils.white_mage.martyrHPCostFraction     = 0.25
xi.job_utils.white_mage.minHPForDevotionMartyr   = 4
xi.job_utils.white_mage.oneHourRecastSecondsPerMod = 60
xi.job_utils.white_mage.msgCannotPerformTarg     = 72
xi.job_utils.white_mage.msgUnableToUseJA         = 87

local removables =
{
    xi.effect.FLASH,              xi.effect.BLINDNESS,      xi.effect.MAX_HP_DOWN,    xi.effect.MAX_MP_DOWN,
    xi.effect.PARALYSIS,          xi.effect.POISON,         xi.effect.CURSE_I,        xi.effect.CURSE_II,
    xi.effect.DISEASE,            xi.effect.PLAGUE,         xi.effect.WEIGHT,         xi.effect.BIND,
    xi.effect.BIO,                xi.effect.DIA,            xi.effect.BURN,           xi.effect.FROST,
    xi.effect.CHOKE,              xi.effect.RASP,           xi.effect.SHOCK,          xi.effect.DROWN,
    xi.effect.STR_DOWN,           xi.effect.DEX_DOWN,       xi.effect.VIT_DOWN,       xi.effect.AGI_DOWN,
    xi.effect.INT_DOWN,           xi.effect.MND_DOWN,       xi.effect.CHR_DOWN,       xi.effect.ADDLE,
    xi.effect.SLOW,               xi.effect.HELIX,          xi.effect.ACCURACY_DOWN,  xi.effect.ATTACK_DOWN,
    xi.effect.EVASION_DOWN,       xi.effect.DEFENSE_DOWN,   xi.effect.MAGIC_ACC_DOWN, xi.effect.MAGIC_ATK_DOWN,
    xi.effect.MAGIC_EVASION_DOWN, xi.effect.MAGIC_DEF_DOWN, xi.effect.MAX_TP_DOWN,    xi.effect.SILENCE,
    xi.effect.PETRIFICATION
}

-- Export catalog for pure tests (read-only table).
xi.job_utils.white_mage.removables = removables

xi.job_utils.white_mage.removablesCount = function()
    return #removables
end

-- Pure isRemovable (internal/whitemage.IsRemovable).
xi.job_utils.white_mage.isRemovable = function(effectID)
    for _, id in ipairs(removables) do
        if id == effectID then
            return true
        end
    end

    return false
end

-- Pure Benediction heal before clamp.
-- params: targetMaxHP, casterMainLvl, targetMainLvl
xi.job_utils.white_mage.benedictionHealFromParams = function(params)
    params = params or {}
    local targetMainLvl = params.targetMainLvl or 0
    if targetMainLvl <= 0 then
        return 0
    end

    return math.floor(((params.targetMaxHP or 0) * (params.casterMainLvl or 0)) / targetMainLvl)
end

-- Pure missing-HP clamp.
xi.job_utils.white_mage.benedictionHealClamped = function(heal, currentHP, maxHP)
    local missing = (maxHP or 0) - (currentHP or 0)
    if missing < 0 then
        missing = 0
    end

    heal = heal or 0
    if heal > missing then
        return missing
    end

    if heal < 0 then
        return 0
    end

    return heal
end

-- Pure Doom clear: chance 33 > roll (1..100).
xi.job_utils.white_mage.benedictionRemovesDoom = function(roll)
    return xi.job_utils.white_mage.benedictionDoomChance > (roll or 0)
end

-- Pure Devotion/Martyr check (self / HP < 4).
-- params: actorID, targetID, actorHP
-- returns msg id
xi.job_utils.white_mage.checkDevotionFromParams = function(params)
    params = params or {}
    if (params.actorID or 0) == (params.targetID or 0) then
        return xi.job_utils.white_mage.msgCannotPerformTarg
    end

    if (params.actorHP or 0) < xi.job_utils.white_mage.minHPForDevotionMartyr then
        return xi.job_utils.white_mage.msgUnableToUseJA
    end

    return 0
end

xi.job_utils.white_mage.checkMartyrFromParams = function(params)
    return xi.job_utils.white_mage.checkDevotionFromParams(params)
end

-- Pure Devotion products.
xi.job_utils.white_mage.devotionMPPercent = function(devotionMerit)
    local meritBonus = (devotionMerit or 0) - xi.job_utils.white_mage.devotionMeritBase
    return (xi.job_utils.white_mage.devotionBaseMPPercent + meritBonus) / 100
end

xi.job_utils.white_mage.devotionDamageHP = function(playerHP)
    return math.floor((playerHP or 0) * xi.job_utils.white_mage.devotionHPCostFraction)
end

xi.job_utils.white_mage.devotionHealMP = function(playerHP, mpPercent)
    return (playerHP or 0) * (mpPercent or 0)
end

xi.job_utils.white_mage.devotionHealMPClamped = function(healMP, targetMP, targetMaxMP)
    local missing = (targetMaxMP or 0) - (targetMP or 0)
    if missing < 0 then
        missing = 0
    end

    healMP = healMP or 0
    if healMP < 0 then
        return 0
    end

    if healMP > missing then
        return missing
    end

    return healMP
end

-- Pure Martyr products.
xi.job_utils.white_mage.martyrHPPercent = function(martyrMerit)
    local meritBonus = (martyrMerit or 0) - xi.job_utils.white_mage.martyrMeritBase
    return (xi.job_utils.white_mage.martyrBaseHPPercent + meritBonus) / 100
end

xi.job_utils.white_mage.martyrDamageHP = function(playerHP)
    return math.floor((playerHP or 0) * xi.job_utils.white_mage.martyrHPCostFraction)
end

xi.job_utils.white_mage.martyrHealHP = function(damageHP, hpPercent)
    return (damageHP or 0) * (hpPercent or 0)
end

xi.job_utils.white_mage.martyrHealHPClamped = function(healHP, targetHP, targetMaxHP)
    local missing = (targetMaxHP or 0) - (targetHP or 0)
    if missing < 0 then
        missing = 0
    end

    healHP = healHP or 0
    if healHP < 0 then
        return 0
    end

    if healHP > missing then
        return missing
    end

    return healHP
end

-- Pure one-hour recast reduction.
xi.job_utils.white_mage.oneHourRecastFromParams = function(params)
    params = params or {}
    return math.max(0, (params.abilityRecast or 0)
        - (params.oneHourRecastMod or 0) * xi.job_utils.white_mage.oneHourRecastSecondsPerMod)
end

-----------------------------------
-- Ability Check Functions (hosts → pure)
-----------------------------------
xi.job_utils.white_mage.checkAsylum = function(player, target, ability)
    ability:setRecast(xi.job_utils.white_mage.oneHourRecastFromParams({
        abilityRecast    = ability:getRecast(),
        oneHourRecastMod = player:getMod(xi.mod.ONE_HOUR_RECAST),
    }))
    return 0, 0
end

xi.job_utils.white_mage.checkBenediction = function(player, target, ability)
    ability:setRecast(xi.job_utils.white_mage.oneHourRecastFromParams({
        abilityRecast    = ability:getRecast(),
        oneHourRecastMod = player:getMod(xi.mod.ONE_HOUR_RECAST),
    }))
    return 0, 0
end

xi.job_utils.white_mage.checkDevotion = function(player, target, ability)
    return xi.job_utils.white_mage.checkDevotionFromParams({
        actorID  = player:getID(),
        targetID = target:getID(),
        actorHP  = player:getHP(),
    }), 0
end

xi.job_utils.white_mage.checkMartyr = function(player, target, ability)
    return xi.job_utils.white_mage.checkMartyrFromParams({
        actorID  = player:getID(),
        targetID = target:getID(),
        actorHP  = player:getHP(),
    }), 0
end

-----------------------------------
-- Ability Use Functions (hosts → pure)
-----------------------------------
xi.job_utils.white_mage.useAfflatusMisery = function(player, target, ability)
    target:delStatusEffect(xi.effect.AFFLATUS_SOLACE)
    target:delStatusEffect(xi.effect.AFFLATUS_MISERY)
    target:addStatusEffect(xi.effect.AFFLATUS_MISERY, {
        power    = xi.job_utils.white_mage.afflatusPower,
        duration = xi.job_utils.white_mage.afflatusDuration,
        origin   = player,
    })

    return xi.effect.AFFLATUS_MISERY
end

xi.job_utils.white_mage.useAfflatusSolace = function(player, target, ability)
    target:delStatusEffect(xi.effect.AFFLATUS_SOLACE)
    target:delStatusEffect(xi.effect.AFFLATUS_MISERY)
    target:addStatusEffect(xi.effect.AFFLATUS_SOLACE, {
        power    = xi.job_utils.white_mage.afflatusPower,
        duration = xi.job_utils.white_mage.afflatusDuration,
        origin   = player,
    })

    return xi.effect.AFFLATUS_SOLACE
end

xi.job_utils.white_mage.useAsylum = function(player, target, ability)
    target:addStatusEffect(xi.effect.ASYLUM, {
        power    = xi.job_utils.white_mage.asylumPower,
        duration = xi.job_utils.white_mage.asylumDuration,
        origin   = player,
    })

    return xi.effect.ASYLUM
end

xi.job_utils.white_mage.useBenediction = function(player, target, ability)
    -- To Do: Benediction can remove Charm only while in Assault Mission Lamia No.13
    for _, effect in ipairs(removables) do
        if target:hasStatusEffect(effect) then
            target:delStatusEffect(effect)
        end
    end

    local heal = xi.job_utils.white_mage.benedictionHealFromParams({
        targetMaxHP    = target:getMaxHP(),
        casterMainLvl  = player:getMainLvl(),
        targetMainLvl  = target:getMainLvl(),
    })
    heal = xi.job_utils.white_mage.benedictionHealClamped(heal, target:getHP(), target:getMaxHP())

    if
        target:hasStatusEffect(xi.effect.DOOM) and
        xi.job_utils.white_mage.benedictionRemovesDoom(math.random(1, 100))
    then
        target:delStatusEffect(xi.effect.DOOM)
    end

    player:updateEnmityFromCure(target, heal)
    target:addHP(heal)
    target:wakeUp()

    return heal
end

xi.job_utils.white_mage.useDevotion = function(player, target, ability, action)
    local mpPercent = xi.job_utils.white_mage.devotionMPPercent(player:getMerit(xi.merit.DEVOTION))
    local damageHP  = xi.job_utils.white_mage.devotionDamageHP(player:getHP())

    -- If stoneskin is present, it should absorb damage
    damageHP = utils.handleStoneskin(player, damageHP)

    local healMP = xi.job_utils.white_mage.devotionHealMP(player:getHP(), mpPercent)
    healMP = xi.job_utils.white_mage.devotionHealMPClamped(healMP, target:getMP(), target:getMaxMP())

    player:delHP(damageHP)
    target:addMP(healMP)

    return healMP
end

xi.job_utils.white_mage.useDivineCaress = function(player, target, ability)
    player:addStatusEffect(xi.effect.DIVINE_CARESS_I, {
        power    = xi.job_utils.white_mage.divineCaressPower,
        duration = xi.job_utils.white_mage.divineCaressDuration,
        origin   = player,
    })

    return xi.effect.DIVINE_CARESS_I
end

xi.job_utils.white_mage.useDivineSeal = function(player, target, ability)
    player:addStatusEffect(xi.effect.DIVINE_SEAL, {
        power    = xi.job_utils.white_mage.divineSealPower,
        duration = xi.job_utils.white_mage.divineSealDuration,
        origin   = player,
    })

    return xi.effect.DIVINE_SEAL
end

xi.job_utils.white_mage.useMartyr = function(player, target, ability, action)
    local hpPercent = xi.job_utils.white_mage.martyrHPPercent(player:getMerit(xi.merit.MARTYR))
    local damageHP  = xi.job_utils.white_mage.martyrDamageHP(player:getHP())

    -- We need to capture this here because the base damage is the basis for the heal
    local healHP = xi.job_utils.white_mage.martyrHealHP(damageHP, hpPercent)
    healHP = xi.job_utils.white_mage.martyrHealHPClamped(healHP, target:getHP(), target:getMaxHP())

    -- If stoneskin is present, it should absorb damage
    damageHP = utils.handleStoneskin(player, damageHP)
    player:delHP(damageHP)
    target:addHP(healHP)

    return healHP
end

xi.job_utils.white_mage.useSacrosanctity = function(player, target, ability)
    target:addStatusEffect(xi.effect.SACROSANCTITY, {
        power    = xi.job_utils.white_mage.sacrosanctityPower,
        duration = xi.job_utils.white_mage.sacrosanctityDuration,
        origin   = player,
    })

    return xi.effect.SACROSANCTITY
end
