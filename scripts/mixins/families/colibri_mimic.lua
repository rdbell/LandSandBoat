--[[
Colibri that copy spells cast on it.

localVar                        default     description
--------                        -------     -----------
[colibri]reflect_blue_magic     0           set to 1 for this mob to also reflect blue magic cast on it

https://ffxiclopedia.fandom.com/wiki/Colibri
https://ffxiclopedia.fandom.com/wiki/Greater_Colibri
https://ffxiclopedia.fandom.com/wiki/Chamrosh
--]]

require('scripts/globals/mixins')
require('scripts/globals/magic')

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.colibriMimic = xi.mix.colibriMimic or {}

xi.mix.colibriMimic.acceptPlan = function(isOpenBeak, tookEffect, casterEligible, isBlueMagic, reflectBlueMagic, spellID, now)
    if not isOpenBeak and tookEffect and casterEligible and (not isBlueMagic or reflectBlueMagic) then
        return { spell = spellID, castWindow = now + 30, castTime = now + 6, animationSub = 5 }
    end
    return nil
end

xi.mix.colibriMimic.combatPlan = function(animationSub, spellToMimic, castWindow, castTime, now, silenced)
    if animationSub ~= 5 then
        return nil
    elseif spellToMimic > 0 and now > castTime and castWindow > now and not silenced then
        return { castSpell = spellToMimic, spell = 0, castWindow = 0, castTime = 0, animationSub = 4 }
    elseif spellToMimic == 0 or now > castWindow then
        return { spell = 0, castWindow = 0, castTime = 0, animationSub = 4 }
    end
    return nil
end

g_mixins.families.colibri_mimic = function(colibriMob)
    -- initial state 0
    local closedBeak = 4
    local openBeak   = 5

    colibriMob:addListener('MAGIC_TAKE', 'COLIBRI_MIMIC_MAGIC_TAKE', function(target, caster, spell)
        local plan = xi.mix.colibriMimic.acceptPlan(target:getAnimationSub() == openBeak, spell:tookEffect(), caster:isPC() or caster:isPet(), spell:getSpellGroup() == xi.magic.spellGroup.BLUE, target:getLocalVar('[colibri]reflect_blue_magic') == 1, spell:getID(), GetSystemTime())
        if plan then
            target:setLocalVar('[colibri]spellToMimic', plan.spell)
            target:setLocalVar('[colibri]castWindow', plan.castWindow)
            target:setLocalVar('[colibri]castTime', plan.castTime)
            target:setAnimationSub(plan.animationSub)
        end
    end)

    colibriMob:addListener('COMBAT_TICK', 'COLIBRI_MIMIC_CTICK', function(mob)
        local spellToMimic = mob:getLocalVar('[colibri]spellToMimic')
        local castWindow = mob:getLocalVar('[colibri]castWindow')
        local castTime = mob:getLocalVar('[colibri]castTime')
        local osTime = GetSystemTime()

        local plan = xi.mix.colibriMimic.combatPlan(mob:getAnimationSub(), spellToMimic, castWindow, castTime, osTime, mob:hasStatusEffect(xi.effect.SILENCE))
        if plan then
            if plan.castSpell then
                mob:castSpell(plan.castSpell)
            end
            mob:setLocalVar('[colibri]spellToMimic', plan.spell)
            mob:setLocalVar('[colibri]castWindow', plan.castWindow)
            mob:setLocalVar('[colibri]castTime', plan.castTime)
            mob:setAnimationSub(plan.animationSub)
        end
    end)

    colibriMob:addListener('DISENGAGE', 'COLIBRI_MIMIC_DISENGAGE', function(mob)
        mob:setLocalVar('[colibri]spellToMimic', 0)
        mob:setLocalVar('[colibri]castWindow', 0)
        mob:setLocalVar('[colibri]castTime', 0)
        if mob:getAnimationSub() ~= closedBeak then
            mob:setAnimationSub(closedBeak)
        end
    end)
end

return g_mixins.families.colibri_mimic
