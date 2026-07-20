require('scripts/actions/mobskills/shijin_spiral')
describe('Shijin Spiral mob skill', function()
    it('uses fivefold H2H plan and applies TP-scaled Plague', function()
        local spiral = require('scripts/actions/mobskills/shijin_spiral')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local calc = xi.mobskills.calculateDuration
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 25 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = { getTP = function() return 1000 end }
        xi.mobskills.calculateDuration = function(a, b, c)
            assert(a == 1000 and b == 18 and c == 24)
            return 21
        end
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 80, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.HAND_TO_HAND }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(spiral.onMobSkillCheck(target, mob, skill) == 0 and spiral.onMobWeaponSkill(mob, target, skill, {}) == 80)
        assert(params.numHits == 5 and params.fTP[1] == 1.0625 and params.attackMultiplier[1] == 1.05 and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        spiral.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        xi.mobskills.calculateDuration = calc
        assert(damage[1] == 80 and statusParams[3] == xi.effect.PLAGUE and statusParams[4] == 5 and statusParams[5] == 3 and statusParams[6] == 21)
    end)
end)
