require('scripts/actions/mobskills/rail_cannon')
describe('Rail Cannon mob skill', function()
    it('uses its light magical plan and damages only after processing', function()
        local cannon = require('scripts/actions/mobskills/rail_cannon')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.LIGHT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(cannon.onMobSkillCheck(target, mob, {}) == 0 and cannon.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 5 and params.dStatMultiplier == 1.5 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        cannon.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
