local lycopodium = require('scripts/mixins/families/lycopodium')

describe('Lycopodium mixin', function()
    it('plans passive, combat, and disengage transitions', function()
        local spawn = xi.mix.lycopodium.spawnPlan()
        assert(spawn.setCombat and not spawn.combatEnabled and spawn.alwaysAggro)
        assert(xi.mix.lycopodium.roamPlan(99, false) == nil)
        local roam = xi.mix.lycopodium.roamPlan(100, false)
        assert(roam.setCombat and not roam.combatEnabled and roam.resetDamaged)
        assert(xi.mix.lycopodium.roamPlan(100, true).resetDamaged == false)
        assert(xi.mix.lycopodium.disengagePlan(true) == nil)
        assert(xi.mix.lycopodium.disengagePlan(false).combatEnabled == false)

        local engage = xi.mix.lycopodium.engagePlan(100, true)
        assert(engage.disengageTime == 145 and engage.setCombat and engage.combatEnabled)
        assert(xi.mix.lycopodium.engagePlan(100, false).setCombat == false)
        assert(xi.mix.lycopodium.combatPlan(false, 99, 100, 110, 120, 0).combatEnabled)
        assert(xi.mix.lycopodium.combatPlan(true, 100, 100, 110, 120, 0).combatEnabled)
        local stop = xi.mix.lycopodium.combatPlan(false, 100, 100, 121, 120, 0)
        assert(stop.disengage and stop.disengageTime == 0)
        assert(xi.mix.lycopodium.combatPlan(false, 100, 100, 120, 120, 0) == nil)
        assert(xi.mix.lycopodium.combatPlan(false, 100, 100, 121, 120, 1) == nil)
        assert(xi.mix.lycopodium.damagePlan(false).markDamaged)
        assert(xi.mix.lycopodium.damagePlan(true).markDamaged == false)

        local mob = { listeners = {}, localVars = {}, hpp = 100, hp = 100, maxHP = 100, battleTime = 100 }
        function mob:addListener(event, _, callback) self.listeners[event] = callback end
        function mob:setAutoAttackEnabled(value) self.autoAttack = value end
        function mob:setMobAbilityEnabled(value) self.mobAbility = value end
        function mob:setMobMod(_, value) self.alwaysAggro = value end
        function mob:setLocalVar(name, value) self.localVars[name] = value end
        function mob:getLocalVar(name) return self.localVars[name] or 0 end
        function mob:getHPP() return self.hpp end
        function mob:isAlly() return false end
        function mob:getHP() return self.hp end
        function mob:getMaxHP() return self.maxHP end
        function mob:getBattleTime() return self.battleTime end
        function mob:disengage() self.didDisengage = true end

        lycopodium(mob)
        mob.listeners.SPAWN(mob)
        assert(not mob.autoAttack and not mob.mobAbility and mob.alwaysAggro == 1)
        mob.listeners.TAKE_DAMAGE(mob)
        assert(mob.autoAttack and mob.mobAbility and mob.localVars['[lycopodium]damaged'] == 1)
        mob.listeners.ROAM_TICK(mob)
        assert(mob.localVars['[lycopodium]damaged'] == 0)
        mob.listeners.ENGAGE(mob)
        mob.battleTime = 146
        mob.listeners.COMBAT_TICK(mob)
        assert(mob.didDisengage and mob.localVars['[lycopodium]disengageTime'] == 0)
    end)
end)
