require('scripts/actions/mobskills/raging_fists')
describe('Raging Fists mob skill', function()
    it('uses its fivefold H2H plan and damages only after processing', function()
        local fists = require('scripts/actions/mobskills/raging_fists')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.HAND_TO_HAND }
        end
        xi.mobskills.processDamage = function() return false end
        assert(fists.onMobSkillCheck(target, mob, {}) == 0 and fists.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.numHits == 5 and params.fTP[1] == 1 and params.fTP[2] == 1.5 and params.fTP[3] == 2 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        fists.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
