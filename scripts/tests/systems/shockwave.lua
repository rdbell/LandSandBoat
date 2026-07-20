require('scripts/actions/mobskills/shockwave')
describe('Shockwave mob skill', function()
    it('uses slashing physical plan and applies TP-scaled Sleep', function()
        local wave = require('scripts/actions/mobskills/shockwave')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local calc = xi.mobskills.calculateDuration
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 55 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = { getTP = function() return 1000 end }
        xi.mobskills.calculateDuration = function(a, b, c)
            assert(a == 1000 and b == 60 and c == 180)
            return 120
        end
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 70, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(wave.onMobSkillCheck(target, mob, skill) == 0 and wave.onMobWeaponSkill(mob, target, skill, {}) == 70)
        assert(params.fTP[1] == 1 and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        wave.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        xi.mobskills.calculateDuration = calc
        assert(damage[1] == 70 and statusParams[3] == xi.effect.SLEEP_I and statusParams[6] == 120)
    end)
end)
