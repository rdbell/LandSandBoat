require('scripts/actions/mobskills/raging_axe')
describe('Raging Axe mob skill', function()
    it('uses its twofold slashing plan and damages only after processing', function()
        local axe = require('scripts/actions/mobskills/raging_axe')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(axe.onMobSkillCheck(target, mob, {}) == 0 and axe.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.numHits == 2 and params.fTP[1] == 1 and params.fTP[2] == 1.5 and params.fTP[3] == 2 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        axe.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
