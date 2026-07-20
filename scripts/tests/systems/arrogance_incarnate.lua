require('scripts/actions/mobskills/arrogance_incarnate')
describe('Arrogance Incarnate mob skill', function()
    it('computes HP/TP formula then applies host pipeline and breath damage', function()
        local skill = require('scripts/actions/mobskills/arrogance_incarnate')
        local origAdj = xi.combat.damage.calculateDamageAdjustment
        local origAbs = xi.spells.damage.calculateAbsorption
        local origNull = xi.spells.damage.calculateNullification
        local origPhalanx = utils.handlePhalanx
        local origStoneskin = utils.handleStoneskin
        local damage, woken, enmity = nil, false, nil
        xi.combat.damage.calculateDamageAdjustment = function() return 1 end
        xi.spells.damage.calculateAbsorption = function() return 1 end
        xi.spells.damage.calculateNullification = function() return 1 end
        utils.handlePhalanx = function(_, d) return d end
        utils.handleStoneskin = function(_, d) return d end
        local mob = { getHP = function() return 2560 end }
        local sk = { getTP = function() return 1000 end }
        local target = {
            handleSevereDamage = function(_, d) return d end,
            wakeUp = function() woken = true end,
            updateEnmityFromDamage = function(_, m, d) enmity = { m, d } end,
            takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end,
        }
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        -- raw: floor(2560*(floor(16)+16)/256)*2.5 = 320*2.5 = 800
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 800)
        xi.combat.damage.calculateDamageAdjustment = origAdj
        xi.spells.damage.calculateAbsorption = origAbs
        xi.spells.damage.calculateNullification = origNull
        utils.handlePhalanx = origPhalanx
        utils.handleStoneskin = origStoneskin
        assert(damage[1] == 800 and damage[3] == xi.attackType.BREATH and damage[4] == xi.damageType.ELEMENTAL)
        assert(woken and enmity[2] == 800)
    end)
end)
