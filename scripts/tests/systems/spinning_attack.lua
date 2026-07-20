require('scripts/actions/mobskills/spinning_attack')
describe('Spinning Attack mob skill', function()
    it('uses twofold H2H plan and damages only after processing', function()
        local atk = require('scripts/actions/mobskills/spinning_attack')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 30 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 60, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.HAND_TO_HAND }
        end
        xi.mobskills.processDamage = function() return false end
        assert(atk.onMobSkillCheck(target, mob, {}) == 0 and atk.onMobWeaponSkill(mob, target, {}, {}) == 60)
        assert(params.numHits == 2 and params.fTP[1] == 1 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        atk.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 60)
    end)
end)
