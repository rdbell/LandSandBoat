require('scripts/actions/mobskills/shoulder_tackle')
describe('Shoulder Tackle mob skill', function()
    it('uses twofold H2H plan and applies TP-scaled Stun', function()
        local tackle = require('scripts/actions/mobskills/shoulder_tackle')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local calc = xi.mobskills.calculateDuration
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 20 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = { getTP = function() return 1000 end }
        xi.mobskills.calculateDuration = function(a, b, c)
            assert(a == 1000 and b == 2 and c == 6)
            return 4
        end
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 40, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.HAND_TO_HAND }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(tackle.onMobSkillCheck(target, mob, skill) == 0 and tackle.onMobWeaponSkill(mob, target, skill, {}) == 40)
        assert(params.numHits == 2 and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        tackle.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        xi.mobskills.calculateDuration = calc
        assert(damage[1] == 40 and statusParams[3] == xi.effect.STUN and statusParams[6] == 4)
    end)
end)
