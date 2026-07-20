require('scripts/actions/mobskills/starburst')
describe('Starburst mob skill', function()
    it('uses light/dark magical plan with TP fTP table', function()
        local skill = require('scripts/actions/mobskills/starburst')
        local params = nil
        local origM, origD, origR = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, math.random
        xi.mobskills.mobMagicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 100, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        math.random = function() return 25 end
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.element == xi.element.LIGHT and params.fTP[1] == 1.0 and params.fTP[3] == 2.5)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, math.random = origM, origD, origR
    end)
end)
