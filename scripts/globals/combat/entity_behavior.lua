-----------------------------------
-- Global file for globably/commonly used entity behavior/patterns.
-- Pure isEntityBusy dual-wired to OmegaXI internal/entitybusy (slice 6700 / 0907).
-----------------------------------
xi = xi or {}
xi.combat = xi.combat or {}
xi.combat.behavior = xi.combat.behavior or {}

xi.combat.behavior.isBusyLocalVarKey = 'isBusy'

-- Pure free-action pose gate (NONE / BASIC_ATTACK / ROAMING).
xi.combat.behavior.isFreeActionCategory = function(currAction)
    return currAction == xi.action.category.NONE or
        currAction == xi.action.category.BASIC_ATTACK or
        currAction == xi.action.category.ROAMING
end

-- Pure isEntityBusy once action/PC/queue/status/local-var injects are supplied.
-- params: currentAction, isPC, actionQueueEmpty,
--   hasSleepI, hasSleepII, hasLullaby, hasStun, hasTerror, hasPetrification,
--   isBusyLocalVar
xi.combat.behavior.isEntityBusyFromParams = function(params)
    -- Check poses (actions).
    local currAction = params.currentAction or xi.action.category.NONE
    if not xi.combat.behavior.isFreeActionCategory(currAction) then
        return true
    end

    -- Check action queue.
    if
        not params.isPC and
        not params.actionQueueEmpty
    then
        return true
    end

    -- Check status effects.
    if
        params.hasSleepI or
        params.hasSleepII or -- Unused, but let's check it anyway, for the future.
        params.hasLullaby or -- Unused, but let's check it anyway, for the future.
        params.hasStun or
        params.hasTerror or
        params.hasPetrification
    then
        return true
    end

    -- Check "isBusy" local variable. For special actions (Bahamut's Megaflare or Ultima's... Ultima, for example).
    if (params.isBusyLocalVar or 0) > 0 then
        return true
    end

    return false
end

-- Entity host: inject action/PC/queue/status/local-var → pure.
xi.combat.behavior.isEntityBusy = function(actor)
    return xi.combat.behavior.isEntityBusyFromParams({
        currentAction    = actor:getCurrentAction(),
        isPC             = actor:isPC(),
        actionQueueEmpty = actor:actionQueueEmpty(),
        hasSleepI        = actor:hasStatusEffect(xi.effect.SLEEP_I),
        hasSleepII       = actor:hasStatusEffect(xi.effect.SLEEP_II),
        hasLullaby       = actor:hasStatusEffect(xi.effect.LULLABY),
        hasStun          = actor:hasStatusEffect(xi.effect.STUN),
        hasTerror        = actor:hasStatusEffect(xi.effect.TERROR),
        hasPetrification = actor:hasStatusEffect(xi.effect.PETRIFICATION),
        isBusyLocalVar   = actor:getLocalVar(xi.combat.behavior.isBusyLocalVarKey),
    })
end

-----------------------------------
-- Pure chooseAction helpers (OmegaXI slice 6701 dual-wire / 1037)
-- Dual-wired to internal/combatbehavior.
-----------------------------------
xi.combat.behavior.allyMaxDistance = 8
xi.combat.behavior.defaultWeight = 100

-- Elemental DoT effects that need getEffectToRemove / getNullificatingEffect gates.
xi.combat.behavior.elementalDoTEffects =
{
    [xi.effect.BURN ] = true,
    [xi.effect.FROST] = true,
    [xi.effect.CHOKE] = true,
    [xi.effect.RASP ] = true,
    [xi.effect.SHOCK] = true,
    [xi.effect.DROWN] = true,
}

xi.combat.behavior.isElementalDoT = function(effectId)
    return xi.combat.behavior.elementalDoTEffects[effectId] == true
end

-- Pure Lua `weight or 100` for a missing weight only (explicit 0 stays 0).
-- present=false → default; present=true → use weight as-is.
xi.combat.behavior.normalizeWeight = function(weight, present)
    if not present then
        return xi.combat.behavior.defaultWeight
    end

    return weight or 0
end

-- Pure desired flags for disableAllActions / enableAllActions.
xi.combat.behavior.disableAllActionsFlags = function()
    return { autoAttack = false, magicCasting = false, mobAbility = false }
end

xi.combat.behavior.enableAllActionsFlags = function()
    return { autoAttack = true, magicCasting = true, mobAbility = true }
end

-- Pure total weight of actionList entries { actionId, target, weight }.
xi.combat.behavior.totalWeight = function(actionList)
    local total = 0
    for i = 1, #actionList do
        total = total + (actionList[i][3] or 0)
    end

    return total
end

-- Pure weighted pick once filtered list and dN roll are known.
-- roll should be in 1..totalWeight (math.random(1, totalWeight)).
-- returns actionId, target, ok
xi.combat.behavior.pickWeightedFromParams = function(actionList, roll)
    local total = xi.combat.behavior.totalWeight(actionList)
    if total <= 0 or #actionList == 0 then
        return 0, nil, false
    end

    local weight = 0
    for i = 1, #actionList do
        weight = weight + (actionList[i][3] or 0)
        if (roll or 0) <= weight then
            return actionList[i][1], actionList[i][2], true
        end
    end

    return 0, nil, false
end

-- For "decoration" type mobs and faked actions.
xi.combat.behavior.disableAllActions = function(actor)
    local flags = xi.combat.behavior.disableAllActionsFlags()
    actor:setAutoAttackEnabled(flags.autoAttack)
    actor:setMagicCastingEnabled(flags.magicCasting)
    actor:setMobAbilityEnabled(flags.mobAbility)
end

xi.combat.behavior.enableAllActions = function(actor)
    local flags = xi.combat.behavior.enableAllActionsFlags()
    actor:setAutoAttackEnabled(flags.autoAttack)
    actor:setMagicCastingEnabled(flags.magicCasting)
    actor:setMobAbilityEnabled(flags.mobAbility)
end

xi.combat.behavior.chooseAction = function(actor, mainTarget, optionalTargets, actionTable)
    local actionList = {}

    -- Build new table with actions that meet the conditions.
    for entry = 1, #actionTable do
        local actionId          = actionTable[entry][1]        -- The ID of the action.
        local actionTarget      = actionTable[entry][2]        -- The main target of the action.
        local actionAllowAllies = actionTable[entry][3]        -- Boolean. Determine if we check "optionalTargets" tables for the condition. NOTE: Needs condition.
        local actionType        = actionTable[entry][4]        -- Determines the condition type.
        local actionCondition   = actionTable[entry][5]        -- The condition. (HP/MP under threshold, effect present.)
        local effectTier        = actionTable[entry][6] or 0   -- Currently used only for effect tiers.
        local actionWeight      = actionTable[entry][7] or 100 -- How likely it will be for the action to be chosen.

        switch (actionType): caseof
        {
            [xi.action.type.NONE] = function()
                table.insert(actionList, { actionId, actionTarget, actionWeight })
            end,

            [xi.action.type.DAMAGE_TARGET] = function()
                if actor:isEngaged() then
                    table.insert(actionList, { actionId, actionTarget, actionWeight })
                end
            end,

            [xi.action.type.DAMAGE_FORCE_SELF] = function()
                table.insert(actionList, { actionId, actor, actionWeight })
            end,

            [xi.action.type.HEALING_TARGET] = function()
                -- Check self.
                if actor:getHPP() <= actionCondition then
                    table.insert(actionList, { actionId, actor, actionWeight })
                end

                -- Check allies.
                if actionAllowAllies and optionalTargets then
                    for _, allyEntity in pairs(optionalTargets) do
                        if
                            allyEntity and
                            allyEntity:isAlive() and
                            allyEntity:checkDistance(actor) <= 8 and
                            allyEntity:getHPP() <= actionCondition
                        then
                            table.insert(actionList, { actionId, allyEntity, actionWeight })
                        end
                    end
                end
            end,

            -- For Self-targeted AoE cures.
            [xi.action.type.HEALING_FORCE_SELF] = function()
                -- Check self.
                if actor:getHPP() <= actionCondition then
                    table.insert(actionList, { actionId, actor, actionWeight })

                -- Check allies.
                else
                    if actionAllowAllies and optionalTargets then
                        for _, allyEntity in pairs(optionalTargets) do
                            if
                                allyEntity and
                                allyEntity:isAlive() and
                                allyEntity:checkDistance(actor) <= 8 and
                                allyEntity:getHPP() <= actionCondition
                            then
                                table.insert(actionList, { actionId, actor, actionWeight })
                                break
                            end
                        end
                    end
                end
            end,

            [xi.action.type.HEALING_EFFECT] = function()
                -- Check self.
                if actor:hasStatusEffect(actionCondition) then
                    table.insert(actionList, { actionId, actor, actionWeight })
                end

                -- Check allies.
                if actionAllowAllies and optionalTargets then
                    for _, allyEntity in pairs(optionalTargets) do
                        if
                            allyEntity and
                            allyEntity:isAlive() and
                            allyEntity:checkDistance(actor) <= 8 and
                            allyEntity:hasStatusEffect(actionCondition)
                        then
                            table.insert(actionList, { actionId, allyEntity, actionWeight })
                        end
                    end
                end
            end,

            [xi.action.type.ENHANCING_TARGET] = function()
                -- Check self.
                if
                    not actor:hasStatusEffect(actionCondition) and
                    not xi.data.statusEffect.isEffectNullified(actor, actionCondition, effectTier)
                then
                    table.insert(actionList, { actionId, actor, actionWeight })
                end

                -- Check allies.
                if actionAllowAllies and optionalTargets then
                    for _, allyEntity in pairs(optionalTargets) do
                        if
                            allyEntity and
                            allyEntity:isAlive() and
                            allyEntity:checkDistance(actor) <= 8 and
                            not allyEntity:hasStatusEffect(actionCondition) and
                            not xi.data.statusEffect.isEffectNullified(allyEntity, actionCondition, effectTier)
                        then
                            table.insert(actionList, { actionId, allyEntity, actionWeight })
                        end
                    end
                end
            end,

            -- For Self-targeted AoE enhancements.
            [xi.action.type.ENHANCING_FORCE_SELF] = function()
                -- Check self.
                if
                    not actor:hasStatusEffect(actionCondition) and
                    not xi.data.statusEffect.isEffectNullified(actor, actionCondition, effectTier)
                then
                    table.insert(actionList, { actionId, actor, actionWeight })

                -- Check allies.
                else
                    if actionAllowAllies and optionalTargets then
                        for _, allyEntity in pairs(optionalTargets) do
                            if
                                allyEntity and
                                allyEntity:isAlive() and
                                allyEntity:checkDistance(actor) <= 8 and
                                not allyEntity:hasStatusEffect(actionCondition) and
                                not xi.data.statusEffect.isEffectNullified(allyEntity, actionCondition, effectTier)
                            then
                                table.insert(actionList, { actionId, actor, actionWeight })
                                break
                            end
                        end
                    end
                end
            end,

            [xi.action.type.ENFEEBLING_TARGET] = function()
                if
                    actor:isEngaged() and
                    not actionTarget:hasStatusEffect(actionCondition) and
                    not xi.data.statusEffect.isEffectNullified(actionTarget, actionCondition, effectTier)
                then
                    -- Special condition: Silence
                    if actionCondition == xi.effect.SILENCE then
                        if xi.data.job.isInnateCaster(actionTarget) then
                            table.insert(actionList, { actionId, actionTarget, actionWeight })
                        end

                    -- Special condition: Elemental DoT incompatibilities. This will ensure we only cast stackable effects.
                    elseif
                        actionCondition == xi.effect.BURN or
                        actionCondition == xi.effect.CHOKE or
                        actionCondition == xi.effect.DROWN or
                        actionCondition == xi.effect.FROST or
                        actionCondition == xi.effect.RASP or
                        actionCondition == xi.effect.SHOCK
                    then
                        if
                            not actionTarget:hasStatusEffect(xi.data.statusEffect.getEffectToRemove(actionCondition)) and
                            not actionTarget:hasStatusEffect(xi.data.statusEffect.getNullificatingEffect(actionCondition))
                        then
                            table.insert(actionList, { actionId, actionTarget, actionWeight })
                        end

                    -- No special conditions.
                    else
                        table.insert(actionList, { actionId, actionTarget, actionWeight })
                    end
                end
            end,

            -- For self-targeted AoE enfeeblements. Use with care.
            [xi.action.type.ENFEEBLING_FORCE_SELF] = function()
                if
                    not actionTarget:hasStatusEffect(actionCondition) and
                    not xi.data.statusEffect.isEffectNullified(actionTarget, actionCondition, effectTier)
                then
                    -- Special condition: Silence
                    if actionCondition == xi.effect.SILENCE then
                        if xi.data.job.isInnateCaster(actionTarget) then
                            table.insert(actionList, { actionId, actor, actionWeight })
                        end

                    -- Special condition: Elemental DoT incompatibilities. This will ensure we only cast stackable effects.
                    elseif
                        actionCondition == xi.effect.BURN or
                        actionCondition == xi.effect.CHOKE or
                        actionCondition == xi.effect.DROWN or
                        actionCondition == xi.effect.FROST or
                        actionCondition == xi.effect.RASP or
                        actionCondition == xi.effect.SHOCK
                    then
                        if
                            not actionTarget:hasStatusEffect(xi.data.statusEffect.getEffectToRemove(actionCondition)) and
                            not actionTarget:hasStatusEffect(xi.data.statusEffect.getNullificatingEffect(actionCondition))
                        then
                            table.insert(actionList, { actionId, actor, actionWeight })
                        end

                    -- No special conditions.
                    else
                        table.insert(actionList, { actionId, actor, actionWeight })
                    end
                end
            end,

            [xi.action.type.DRAIN_HP] = function()
                if
                    actor:isEngaged() and
                    not actionTarget:isUndead()
                then
                    if
                        actionCondition == nil or
                        (actionCondition and actor:getHPP() <= actionCondition)
                    then
                        table.insert(actionList, { actionId, actionTarget, actionWeight })
                    end
                end
            end,

            [xi.action.type.DRAIN_MP] = function()
                if
                    actor:isEngaged() and
                    not actionTarget:isUndead() and
                    actionTarget:getMP() > 0
                then
                    if
                        actionCondition == nil or
                        (actionCondition and actor:getMPP() <= actionCondition)
                    then
                        table.insert(actionList, { actionId, actionTarget, actionWeight })
                    end
                end
            end,
        }
    end

    -- Something went wrong or nothing to cast right now.
    if #actionList == 0 then
        return 0, nil
    end

    -- Choose action and target via pure weighted pick.
    local totalWeight = xi.combat.behavior.totalWeight(actionList)
    local randomRoll = math.random(1, totalWeight)
    local actionId, actionTarget = xi.combat.behavior.pickWeightedFromParams(actionList, randomRoll)

    return actionId, actionTarget
end
