require('scripts/actions/mobskills/quietus')
describe('Quietus mob skill', function()
    it('uses its slashing physical plan with ignore-defense and damages only after processing', function()
        local quietus = require('scripts/actions/mobskills/quietus')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(quietus.onMobSkillCheck(target, mob, {}) == 0 and quietus.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.fTP[1] == 3 and params.ignoreDefense[1] == 0.1 and params.ignoreDefense[2] == 0.3 and params.ignoreDefense[3] == 0.5 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        quietus.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
