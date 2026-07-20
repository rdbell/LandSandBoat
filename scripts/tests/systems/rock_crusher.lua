require('scripts/actions/mobskills/rock_crusher')
describe('Rock Crusher mob skill', function()
    it('uses earth magical plan with dStat and damages only after processing', function()
        local crusher = require('scripts/actions/mobskills/rock_crusher')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.EARTH }
        end
        xi.mobskills.processDamage = function() return false end
        assert(crusher.onMobSkillCheck(target, mob, {}) == 0 and crusher.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 1 and params.fTP[2] == 2 and params.fTP[3] == 2.5)
        assert(params.element == xi.element.EARTH and params.dStatMultiplier == 1 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        crusher.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
