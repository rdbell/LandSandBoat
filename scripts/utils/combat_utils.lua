---@class utils
utils = utils or {}

-----------------------------------
-- Shadow mitigation pure helpers
-- Dual-wired to OmegaXI internal/shadowabsorb (slice 6706 / 0877 / 6040).
-----------------------------------

utils.shadowDefaultProcChance       = 50
utils.shadowBlinkFailThreshold      = 20
utils.shadowTakeShadowsBlinkProc    = 80

-- Copy-Image icon for remaining Utsusemi shadows (0 / CI / CI2 / CI3 / CI4+).
utils.copyImageIcon = function(remaining)
    remaining = remaining or 0
    if remaining <= 0 then
        return 0
    elseif remaining == 1 then
        return xi.effect.COPY_IMAGE
    elseif remaining == 2 then
        return xi.effect.COPY_IMAGE_2
    elseif remaining == 3 then
        return xi.effect.COPY_IMAGE_3
    end

    return xi.effect.COPY_IMAGE_4
end

-- Pure damage scale after known consume counts (Go TakeShadowsDamage).
-- if used >= remove → 0; else trunc toward zero of damage * (remove-used)/remove
utils.takeShadowsDamage = function(damage, remove, used)
    damage = damage or 0
    remove = remove or 0
    used   = used or 0
    if remove <= 0 then
        remove = 1
    end

    if used >= remove then
        return 0
    end

    -- Go int(float64) truncates toward zero; positive damage → floor.
    return math.floor(damage * (remove - used) / remove)
end

-- Pure attemptShadowMitigation once NIN/Utsusemi gates and d100 rolls inject.
-- params: attemptedRemovals, isNIN, hasUtsusemi, procChance?, rolls[]
utils.attemptShadowMitigationFromParams = function(params)
    local attempted = params.attemptedRemovals or 0
    if attempted <= 0 then
        return 0
    end

    if not params.isNIN or not params.hasUtsusemi then
        return 0
    end

    local chance = params.procChance or 0
    if chance <= 0 then
        chance = utils.shadowDefaultProcChance
    end

    local rolls = params.rolls or {}
    local mitigated = 0
    for i = 1, attempted do
        local roll = rolls[i] or 101 -- missing → fail
        if roll <= chance then
            mitigated = mitigated + 1
        end
    end

    return math.min(mitigated, attempted - 1)
end

-- Pure takeShadows once mods and Blink rolls inject.
-- params: damage, utsusemi, blink, shadowsToRemove, blinkRolls[]
-- returns: { damage, used, remaining, modID, usedUtsusemi, usedBlink, icon, setIcon, delCopyImage, delBlink }
utils.takeShadowsFromParams = function(params)
    local damage = params.damage or 0
    local remove = params.shadowsToRemove or 0
    if remove <= 0 then
        remove = 1
    end

    local shadowPower = params.utsusemi or 0
    local shadowType  = xi.mod.UTSUSEMI
    local usedUtsu    = true
    if shadowPower == 0 then
        shadowPower = params.blink or 0
        shadowType  = xi.mod.BLINK
        usedUtsu    = false
    end

    if shadowPower == 0 then
        return {
            damage       = damage,
            used         = 0,
            remaining    = 0,
            modID        = 0,
            usedUtsusemi = false,
            usedBlink    = false,
            icon         = 0,
            setIcon      = false,
            delCopyImage = false,
            delBlink     = false,
        }
    end

    local remaining = shadowPower
    local used      = 0
    local outDmg

    if shadowType == xi.mod.BLINK then
        local rolls = params.blinkRolls or {}
        for i = 1, remove do
            local roll = rolls[i] or 101
            if remaining > 0 and roll <= utils.shadowTakeShadowsBlinkProc then
                remaining = remaining - 1
                used      = used + 1
            end
        end

        outDmg = utils.takeShadowsDamage(damage, remove, used)
    else
        if shadowPower >= remove then
            remaining = shadowPower - remove
            used      = remove
            outDmg    = 0
        else
            used      = shadowPower
            remaining = 0
            outDmg    = utils.takeShadowsDamage(damage, remove, used)
        end
    end

    local res = {
        damage       = outDmg,
        used         = used,
        remaining    = remaining,
        modID        = shadowType,
        usedUtsusemi = usedUtsu,
        usedBlink    = not usedUtsu,
        icon         = 0,
        setIcon      = false,
        delCopyImage = false,
        delBlink     = false,
    }

    if remaining <= 0 then
        res.delCopyImage = true
        res.delBlink     = true
    elseif usedUtsu then
        res.icon    = utils.copyImageIcon(remaining)
        res.setIcon = true
    end

    return res
end

-- Pure shadowAbsorb once mod values, remove count, and Blink fail roll inject.
-- params: utsusemi, blink, shadowsToRemove, blinkFailRoll, hasCopyImageEffect
-- returns: { absorbHit, consumed, remaining, usedUtsusemi, usedBlink, icon, setIcon, delCopyImage, delBlink }
utils.shadowAbsorbFromParams = function(params)
    local utsusemi = params.utsusemi or 0
    local blink    = params.blink or 0
    local remove   = params.shadowsToRemove or 0

    if utsusemi == 0 and blink == 0 then
        return {
            absorbHit    = false,
            consumed     = 0,
            remaining    = 0,
            usedUtsusemi = false,
            usedBlink    = false,
            icon         = 0,
            setIcon      = false,
            delCopyImage = false,
            delBlink     = false,
        }
    end

    if utsusemi > 0 then
        local consumed  = utils.clamp(remove, 0, utsusemi)
        local remaining = utsusemi - consumed
        local res =
        {
            absorbHit    = utsusemi >= remove,
            consumed     = consumed,
            remaining    = remaining,
            usedUtsusemi = true,
            usedBlink    = false,
            icon         = 0,
            setIcon      = false,
            delCopyImage = false,
            delBlink     = false,
        }

        if remaining == 0 then
            res.delCopyImage = params.hasCopyImageEffect and true or false
        else
            res.icon    = utils.copyImageIcon(remaining)
            res.setIcon = params.hasCopyImageEffect and true or false
        end

        return res
    end

    -- Blink path (Utsusemi absent).
    if (params.blinkFailRoll or 0) <= utils.shadowBlinkFailThreshold then
        return {
            absorbHit    = false,
            consumed     = 0,
            remaining    = 0,
            usedUtsusemi = false,
            usedBlink    = false,
            icon         = 0,
            setIcon      = false,
            delCopyImage = false,
            delBlink     = false,
        }
    end

    local consumed  = utils.clamp(remove, 0, blink)
    local remaining = blink - consumed
    return {
        absorbHit    = blink >= remove,
        consumed     = consumed,
        remaining    = remaining,
        usedUtsusemi = false,
        usedBlink    = true,
        icon         = 0,
        setIcon      = false,
        delCopyImage = false,
        delBlink     = remaining == 0,
    }
end

-----------------------------------
-- Entity hosts for shadow helpers
-----------------------------------

-- A mechanic that will occasionaly reduce shadows consumed when hit by an AOE skill.
---@nodiscard
---@param actor CBaseEntity
---@param attemptedRemovals integer
---@return integer
function utils.attemptShadowMitigation(actor, attemptedRemovals)
    -- TODO: Does this mechanic work on players who are not NIN main or sub? If so remove NIN requirement.
    -- See Yagyu Darkblade: https://www.bg-wiki.com/ffxi/Yagyu_Darkblade
    -- TODO: Currently unknown exactly what stats affect procChance (Ninjutsu skill).
    local rolls = {}
    local n = attemptedRemovals or 0
    for i = 1, n do
        rolls[i] = math.random(1, 100)
    end

    return utils.attemptShadowMitigationFromParams({
        attemptedRemovals = n,
        isNIN             = actor:getMainJob() == xi.job.NIN or actor:getSubJob() == xi.job.NIN,
        hasUtsusemi       = actor:getMod(xi.mod.UTSUSEMI) > 0,
        procChance        = utils.shadowDefaultProcChance,
        rolls             = rolls,
    })
end

-- TODO: Marked for retirement. See: utils.shadowAbsorb() below.
-- Calculate shadow consumption/damage absorbtion.
---@param actor CBaseEntity
---@param damage integer
---@param shadowsToRemove integer?
---@return integer damage
---@return integer shadowsUsed
function utils.takeShadows(actor, damage, shadowsToRemove)
    local utsusemi = actor:getMod(xi.mod.UTSUSEMI)
    local blink    = actor:getMod(xi.mod.BLINK)
    local remove   = shadowsToRemove or 1
    local blinkRolls = {}

    if utsusemi == 0 and blink > 0 then
        for i = 1, remove do
            blinkRolls[i] = math.random(1, 100)
        end
    end

    local res = utils.takeShadowsFromParams({
        damage          = damage,
        utsusemi        = utsusemi,
        blink           = blink,
        shadowsToRemove = shadowsToRemove,
        blinkRolls      = blinkRolls,
    })

    if res.modID ~= 0 then
        actor:setMod(res.modID, res.remaining)
    end

    if res.setIcon then
        local effect = actor:getStatusEffect(xi.effect.COPY_IMAGE)
        if effect then
            effect:setIcon(res.icon)
        end
    end

    if res.delCopyImage then
        actor:delStatusEffect(xi.effect.COPY_IMAGE)
    end

    if res.delBlink then
        actor:delStatusEffect(xi.effect.BLINK)
    end

    return res.damage, res.used
end

-- Calculate shadow consumption
---@param target CBaseEntity
---@param shadowsToRemove number
---@return boolean, number
function utils.shadowAbsorb(target, shadowsToRemove)
    local utsusemi = target:getMod(xi.mod.UTSUSEMI)
    local blink    = target:getMod(xi.mod.BLINK)
    local failRoll = 100
    if utsusemi == 0 and blink > 0 then
        failRoll = math.random(1, 100)
    end

    local res = utils.shadowAbsorbFromParams({
        utsusemi            = utsusemi,
        blink               = blink,
        shadowsToRemove     = shadowsToRemove,
        blinkFailRoll       = failRoll,
        hasCopyImageEffect  = target:getStatusEffect(xi.effect.COPY_IMAGE) ~= nil,
    })

    if res.usedUtsusemi then
        if res.delCopyImage then
            target:delStatusEffect(xi.effect.COPY_IMAGE)
        elseif res.setIcon then
            local effect = target:getStatusEffect(xi.effect.COPY_IMAGE)
            if effect then
                effect:setIcon(res.icon)
            end
        end

        target:setMod(xi.mod.UTSUSEMI, res.remaining)
    elseif res.usedBlink then
        if res.delBlink then
            target:delStatusEffect(xi.effect.BLINK)
        end

        target:setMod(xi.mod.BLINK, res.remaining)
    end

    -- Retail notes (Blink multi-hit / Zephyr Mantle) deferred; pure path pins math.
    return res.absorbHit, res.consumed
end

-- Calculates Phalanx damage reduction.
---@nodiscard
---@param actor CBaseEntity
---@param damage integer
---@return integer
function utils.handlePhalanx(actor, damage)
    if damage <= 0 then
        return damage
    end

    return utils.clamp(damage - actor:getMod(xi.mod.PHALANX), 0, 99999)
end

-- Returns reduced magic damage from RUN buff, 'One for All'
---@nodiscard
---@param actor CBaseEntity
---@param damage integer
---@return integer
function utils.handleOneForAll(actor, damage)
    if damage <= 0 then
        return damage
    end

    local oneForAllEffect = actor:getStatusEffect(xi.effect.ONE_FOR_ALL)
    if not oneForAllEffect then
        return damage
    end

    return utils.clamp(damage - oneForAllEffect:getPower(), 0, 99999)
end

-- Calculates Stoneskin damage reduction.
---@nodiscard
---@param actor CBaseEntity
---@param damage integer
---@return integer
function utils.handleStoneskin(actor, damage)
    if damage <= 0 then
        return damage
    end

    local stoneskinRemaining = actor:getMod(xi.mod.STONESKIN)
    if stoneskinRemaining <= 0 then
        return damage
    end

    -- Absorb all damage
    if stoneskinRemaining > damage then
        actor:delMod(xi.mod.STONESKIN, damage)

        return 0

    -- Wear off if mitigated damage exceeds stoneskin.
    else
        actor:delStatusEffect(xi.effect.STONESKIN)
        actor:setMod(xi.mod.STONESKIN, 0)

        return damage - stoneskinRemaining
    end
end

-- Handles Automaton attachment "Analyzer", which decreases damage from successive special attacks.
---@param actor CBaseEntity
---@param skill CPetSkill|CMobSkill
---@param damage integer
---@return integer
function utils.handleAutomatonAutoAnalyzer(actor, skill, damage)
    local analyzerModifier = actor:getMod(xi.mod.AUTO_ANALYZER)

    -- If no Analyzer equipped, return unmodified damage.
    if analyzerModifier <= 0 then
        return damage
    end

    local incomingSkill      = skill:getID()
    local analyzedSkillCount = math.min(analyzerModifier, 6)

    -- Check if the incoming skill matches any of the analyzed skills. If so, apply the damage reduction.
    for i = 1, analyzedSkillCount do
        if incomingSkill == actor:getLocalVar('analyzedSkill' .. i) then
            return math.floor(damage * 0.6)
        end
    end

    return damage
end
