require('scripts/actions/mobskills/self-destruct_bomb_321')
describe('Self-Destruct Bomb 321 mob skill', function()
    it('uses fire breath WIPE_SHADOWS base 9999+HP fTP 2.5', function()
        local skill = require('scripts/actions/mobskills/self-destruct_bomb_321')
        local params = nil
        local origM, origD = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        xi.mobskills.mobMagicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 100, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        local mob = { getHP = function() return 50 end }
        local target = { takeDamage = function() end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.baseDamage == 10049 and params.fTP[1] == 2.5 and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = origM, origD
    end)
end)
