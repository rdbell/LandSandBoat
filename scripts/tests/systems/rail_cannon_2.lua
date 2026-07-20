require('scripts/actions/mobskills/rail_cannon_2')
describe('Rail Cannon 2 mob skill', function()
    it('uses light magical plan fTP 5.0 dStat 1.5', function()
        local skill = require('scripts/actions/mobskills/rail_cannon_2')
        local params = nil
        local origM, origD = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        xi.mobskills.mobMagicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 200, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 200)
        assert(params.element == xi.element.LIGHT and params.fTP[1] == 5.0 and params.dStatMultiplier == 1.5)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = origM, origD
    end)
end)
