require('scripts/actions/mobskills/pounce')
describe('Pounce mob skill', function()
    it('uses its slashing physical plan and damages only after processing', function()
        local pounce = require('scripts/actions/mobskills/pounce')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(pounce.onMobSkillCheck(target, mob, {}) == 0 and pounce.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.fTP[1] == 2 and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        pounce.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
