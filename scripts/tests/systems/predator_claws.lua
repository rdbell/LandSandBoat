require('scripts/actions/mobskills/predator_claws')

describe('Predator Claws mob skill', function()
    it('uses its threefold slashing plan and damages only after processing', function()
        local claws = require('scripts/actions/mobskills/predator_claws')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(claws.onMobSkillCheck(target, mob, {}) == 0 and claws.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 3 and params.fTP[1] == 2)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        claws.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[4] == xi.damageType.SLASHING)
    end)
end)
