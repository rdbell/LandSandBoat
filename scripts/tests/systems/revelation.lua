require('scripts/actions/mobskills/revelation')
describe('Revelation mob skill', function()
    it('uses its light magical plan and damages only after processing', function()
        local rev = require('scripts/actions/mobskills/revelation')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.LIGHT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(rev.onMobSkillCheck(target, mob, {}) == 0 and rev.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.element == xi.element.LIGHT and damage == nil)
        xi.mobskills.processDamage = function() return true end
        rev.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123)
    end)
end)
