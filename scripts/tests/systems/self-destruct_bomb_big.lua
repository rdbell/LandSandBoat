require('scripts/actions/mobskills/self-destruct_bomb_big')
describe('Self-Destruct Bomb Big mob skill', function()
    it('admits non-NM under 90% HPP and uses fire breath plan', function()
        local skill = require('scripts/actions/mobskills/self-destruct_bomb_big')
        local params = nil
        local origM, origD, origR = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, math.random
        xi.mobskills.mobMagicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 100, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        math.random = function() return 1.0 end
        local mob = {
            isMobType = function() return false end,
            getHPP = function() return 50 end,
            getHP = function() return 500 end,
        }
        local target = { getMaxHP = function() return 1000 end, takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        mob.getHPP = function() return 90 end
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        mob.getHPP = function() return 50 end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.element == xi.element.FIRE and params.fTP[1] == 1.0)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, math.random = origM, origD, origR
    end)
end)
