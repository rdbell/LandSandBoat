require('scripts/actions/mobskills/rush')
describe('Rush mob skill', function()
    it('uses fivefold blunt plan and damages only after processing', function()
        local rush = require('scripts/actions/mobskills/rush')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(rush.onMobSkillCheck(target, mob, {}) == 0 and rush.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.numHits == 5 and params.fTP[1] == 1 and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_5 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        rush.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
