-----------------------------------
-- Functions that return an integer value meant to be additive.
-- Pure injects dual-wired to OmegaXI internal/damageadditions (slice 6697 / 6084 / 6091).
-----------------------------------
xi = xi or {}
xi.combat = xi.combat or {}
xi.combat.damage = xi.combat.damage or {}
-----------------------------------

xi.combat.damage.souleaterBaseFraction = 0.1

-----------------------------------
-- Pure injects
-----------------------------------

-- Pure Souleater raw bonus before non-DRK half / self-damage.
-- params: hasEffect, hp, souleaterEffect, souleaterEffectII (percent units)
xi.combat.damage.souleaterBonusFromParams = function(params)
    if not params.hasEffect then
        return 0
    end

    local se = (params.souleaterEffect or 0) / 100
    local seII = (params.souleaterEffectII or 0) / 100
    local hp = params.hp or 0

    return math.floor(hp * (xi.combat.damage.souleaterBaseFraction + se + seII))
end

-- Pure pre-stoneskin self-damage once full bonus and STALWART_SOUL percent are injected.
-- params: bonusDamage, stalwartSoulPercent
xi.combat.damage.souleaterSelfDamageFromParams = function(params)
    local bonusDamage = params.bonusDamage or 0
    if bonusDamage <= 0 then
        return 0
    end

    return bonusDamage * (1 - (params.stalwartSoulPercent or 0) / 100)
end

-- Pure souleaterAddition damage return half (self-damage is host residual).
-- params: hasEffect, hp, souleaterEffect, souleaterEffectII, mainJob
xi.combat.damage.souleaterAdditionFromParams = function(params)
    local bonus = xi.combat.damage.souleaterBonusFromParams(params)
    if bonus > 0 and params.mainJob ~= xi.job.DRK then
        return math.floor(bonus / 2)
    end

    return bonus
end

-- Pure consumeManaAddition bonus half (setMP/delStatus are host residual).
-- params: hasEffect, mp
xi.combat.damage.consumeManaAdditionFromParams = function(params)
    if not params.hasEffect then
        return 0
    end

    return math.floor((params.mp or 0) / 10)
end

-----------------------------------
-- Entity hosts (inject → pure + side effects)
-----------------------------------

xi.combat.damage.souleaterAddition = function(actor)
    -- http://wiki.ffo.jp/html/1705.html
    -- https://www.ffxiah.com/forum/topic/21497/stalwart-soul/ some anecdotal data that aligns with JP
    -- https://www.bg-wiki.com/ffxi/Agwu%27s_Scythe Souleater Effect that goes beyond established cap, Stalwart Soul bonus being additive to trait

    local hasEffect = actor:hasStatusEffect(xi.effect.SOULEATER)
    if not hasEffect then
        return 0
    end

    local souleaterEffect = actor:getMaxGearMod(xi.mod.SOULEATER_EFFECT)
    local souleaterEffectII = actor:getMod(xi.mod.SOULEATER_EFFECT_II)
    local stalwartSoulPercent = actor:getMod(xi.mod.STALWART_SOUL)
    local hp = actor:getHP()
    local mainJob = actor:getMainJob()

    local bonusParams = {
        hasEffect         = true,
        hp                = hp,
        souleaterEffect   = souleaterEffect,
        souleaterEffectII = souleaterEffectII,
        mainJob           = mainJob,
    }

    local bonusDamage = xi.combat.damage.souleaterBonusFromParams(bonusParams)

    if bonusDamage > 0 then
        local selfDamage = xi.combat.damage.souleaterSelfDamageFromParams({
            bonusDamage          = bonusDamage,
            stalwartSoulPercent  = stalwartSoulPercent,
        })

        selfDamage = utils.handleStoneskin(actor, selfDamage)
        actor:delHP(selfDamage)
    end

    return xi.combat.damage.souleaterAdditionFromParams(bonusParams)
end

xi.combat.damage.consumeManaAddition = function(actor)
    local hasEffect = actor:hasStatusEffect(xi.effect.CONSUME_MANA)
    if not hasEffect then
        return 0
    end

    local bonusDamage = xi.combat.damage.consumeManaAdditionFromParams({
        hasEffect = true,
        mp        = actor:getMP(),
    })

    actor:setMP(0)
    actor:delStatusEffect(xi.effect.CONSUME_MANA)

    return bonusDamage
end
