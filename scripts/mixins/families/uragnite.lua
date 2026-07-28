--[[
https://ffxiclopedia.fandom.com/wiki/Category:Uragnites
https://www.bg-wiki.com/bg/Category:Uragnite

Uragnite mob can optionally be modified by calling xi.mix.uragnite.config(mob, params) from within onMobSpawn.

params is a table that can contain the following keys:
    inShellSkillList : skill list given to mob when it enters shell (default: 250)
    noShellSkillList : skill list given to mob when it exits shell (default: 251)
    chanceToShell    : percent chance to enter shell when hit by a physical attack (default: 20)
    timeInShellMin   : least time mob can stay in shell, in seconds (default: 30)
    timeInShellMax   : most time mob can stay in shell, in seconds (default: 45)
    inShellRegen     : amount of regen mob gets while in shell (default: 50)

Example:

xi.mix.uragnite.config(mob, {
    chanceToShell = 10,
    timeInShellMin = 45,
    timeInShellMin = 60,
})

--]]
require('scripts/globals/mixins')
-----------------------------------
xi = xi or {}
xi.mix = xi.mix or {}
xi.mix.uragnite = xi.mix.uragnite or {}

xi.mix.uragnite.shouldEnterShell = function(isPhysical, roll, chanceToShell, animationSub)
    return isPhysical and roll <= chanceToShell and bit.band(animationSub, 1) == 0
end

xi.mix.uragnite.enterPlan = function(animationSub, regen, skillList)
    return {
        animationSub = animationSub + 1,
        autoAttack = false,
        damageTakenMod = -7500,
        regen = regen,
        skillList = skillList,
        noMove = 1,
    }
end

xi.mix.uragnite.exitPlan = function(animationSub, regen, skillList)
    return {
        animationSub = animationSub - 1,
        autoAttack = true,
        damageTakenMod = -7500,
        regen = regen,
        skillList = skillList,
        noMove = 0,
    }
end

g_mixins = g_mixins or {}
g_mixins.families = g_mixins.families or {}

local function enterShell(mob)
    local plan = xi.mix.uragnite.enterPlan(mob:getAnimationSub(), mob:getLocalVar('[uragnite]inShellRegen'), mob:getLocalVar('[uragnite]inShellSkillList'))
    mob:setAnimationSub(plan.animationSub)
    mob:setAutoAttackEnabled(plan.autoAttack)
    mob:addMod(xi.mod.UDMGPHYS, plan.damageTakenMod)
    mob:addMod(xi.mod.UDMGRANGE, plan.damageTakenMod)
    mob:addMod(xi.mod.UDMGMAGIC, plan.damageTakenMod)
    mob:addMod(xi.mod.UDMGBREATH, plan.damageTakenMod)
    mob:addMod(xi.mod.REGEN, plan.regen)
    mob:setMobMod(xi.mobMod.SKILL_LIST, plan.skillList)
    mob:setMobMod(xi.mobMod.NO_MOVE, plan.noMove)
end

local function exitShell(mob)
    local plan = xi.mix.uragnite.exitPlan(mob:getAnimationSub(), mob:getLocalVar('[uragnite]inShellRegen'), mob:getLocalVar('[uragnite]noShellSkillList'))
    mob:setAnimationSub(plan.animationSub)
    mob:setAutoAttackEnabled(plan.autoAttack)
    mob:delMod(xi.mod.UDMGPHYS, plan.damageTakenMod)
    mob:delMod(xi.mod.UDMGRANGE, plan.damageTakenMod)
    mob:delMod(xi.mod.UDMGMAGIC, plan.damageTakenMod)
    mob:delMod(xi.mod.UDMGBREATH, plan.damageTakenMod)
    mob:delMod(xi.mod.REGEN, plan.regen)
    mob:setMobMod(xi.mobMod.SKILL_LIST, plan.skillList)
    mob:setMobMod(xi.mobMod.NO_MOVE, plan.noMove)
end

xi.mix.uragnite.config = function(mob, params)
    if params.inShellSkillList and type(params.inShellSkillList) == 'number' then
        mob:setLocalVar('[uragnite]inShellSkillList', params.inShellSkillList)
    end

    if params.noShellSkillList and type(params.noShellSkillList) == 'number' then
        mob:setLocalVar('[uragnite]noShellSkillList', params.noShellSkillList)
    end

    if params.chanceToShell and type(params.chanceToShell) == 'number' then
        mob:setLocalVar('[uragnite]chanceToShell', params.chanceToShell)
    end

    if params.timeInShellMin and type(params.timeInShellMin) == 'number' then
        mob:setLocalVar('[uragnite]timeInShellMin', params.timeInShellMin)
    end

    if params.timeInShellMax and type(params.timeInShellMax) == 'number' then
        mob:setLocalVar('[uragnite]timeInShellMax', params.timeInShellMax)
    end

    if params.inShellRegen and type(params.inShellRegen) == 'number' then
        mob:setLocalVar('[uragnite]inShellRegen', params.inShellRegen)
    end
end

g_mixins.families.uragnite = function(uragniteMob)
    -- at spawn, give mob default skill lists for in-shell and out-of-shell states
    -- these defaults can be overwritten by using xi.mix.uragnite.config() in onMobSpawn.

    uragniteMob:addListener('PRESPAWN', 'URAGNITE_SPAWN', function(mob)
        mob:setLocalVar('[uragnite]noShellSkillList', 251)
        mob:setLocalVar('[uragnite]inShellSkillList', 250)
        mob:setLocalVar('[uragnite]chanceToShell', 20)
        mob:setLocalVar('[uragnite]timeInShellMin', 30)
        mob:setLocalVar('[uragnite]timeInShellMax', 45)
        mob:setLocalVar('[uragnite]inShellRegen', 50)
    end)

    uragniteMob:addListener('TAKE_DAMAGE', 'URAGNITE_TAKE_DAMAGE', function(mob, amount, attacker, attackType, damageType)
        if xi.mix.uragnite.shouldEnterShell(
            attackType == xi.attackType.PHYSICAL,
            math.random(1, 100),
            mob:getLocalVar('[uragnite]chanceToShell'),
            mob:getAnimationSub())
        then
            enterShell(mob)
            local timeInShell = math.random(mob:getLocalVar('[uragnite]timeInShellMin'), mob:getLocalVar('[uragnite]timeInShellMax'))
            mob:timer(timeInShell * 1000, function(mobArg)
                exitShell(mobArg)
            end)
        end
    end)
end

return g_mixins.families.uragnite
