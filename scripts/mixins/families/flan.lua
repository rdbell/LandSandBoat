require('scripts/globals/mixins')

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.flan = xi.mix.flan or {}

xi.mix.flan.damagePlan = function(maxHP, physical, magical, damage, isPhysical)
    local accumulated = (isPhysical and physical or magical) + damage
    if accumulated <= maxHP * 0.3 and damage <= maxHP * 0.1 then
        return nil
    end

    if isPhysical then
        return { animationSub = 2, physical = 0, dmgPhys = -5000, dmgRange = -5000, dmgMagic = 0, damage = 0 }
    end
    return { animationSub = 1, magical = 0, dmgPhys = 0, dmgRange = 0, dmgMagic = -5000, damage = 1 }
end

g_mixins.families.flan = function(flanMob)
    flanMob:addListener('TAKE_DAMAGE', 'FLAN_TAKE_DAMAGE', function(mob, damage, attacker, attackType, damageType)
        local accumulatedPhisical = mob:getLocalVar('physical')
        local accumulatedMagical  = mob:getLocalVar('magical')

        local isPhysical = attackType == xi.attackType.PHYSICAL or attackType == xi.attackType.RANGED
        local plan = xi.mix.flan.damagePlan(mob:getMaxHP(), accumulatedPhisical, accumulatedMagical, damage, isPhysical)
        if plan then
            mob:setAnimationSub(plan.animationSub)
            mob:setMod(xi.mod.DMGPHYS, plan.dmgPhys)
            mob:setMod(xi.mod.DMGRANGE, plan.dmgRange)
            mob:setMod(xi.mod.DMGMAGIC, plan.dmgMagic)
            mob:setLocalVar('Damage', plan.damage)
            mob:setLocalVar(isPhysical and 'physical' or 'magical', 0)
        else
            mob:setLocalVar(isPhysical and 'physical' or 'magical', (isPhysical and accumulatedPhisical or accumulatedMagical) + damage)
        end
    end)
end

return g_mixins.families.flan
