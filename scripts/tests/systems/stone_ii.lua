require('scripts/actions/mobskills/stone_ii')
describe('Stone II mob skill', function()
    it('uses earth magical plan and damages only after processing', function()
        local stone = require('scripts/actions/mobskills/stone_ii')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.EARTH }
        end
        xi.mobskills.processDamage = function() return false end
        assert(stone.onMobSkillCheck(target, mob, {}) == 0 and stone.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.baseDamage == 52 and params.fTP[1] == 3 and params.element == xi.element.EARTH and damage == nil)
        xi.mobskills.processDamage = function() return true end
        stone.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 100)
    end)
end)
