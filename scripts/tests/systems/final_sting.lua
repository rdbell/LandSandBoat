describe('Final Sting mob skill', function()
    it('requires at most 50% HP, uses its magical plan, and self-destructs on finalization', function()
        local finalSting = require('scripts/actions/mobskills/final_sting')
        local magicalMove, processDamage = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage, hp, animationSub = nil, nil, nil, nil
        local mob = {
            getHPP = function() return 50 end,
            setHP = function(_, value) hp = value end,
            setAnimationSub = function(_, value) animationSub = value end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = { getMobHP = function() return 500 end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.NONE }
        end
        xi.mobskills.processDamage = function() return false end
        assert(finalSting.onMobSkillCheck(target, mob, skill) == 0)
        mob.getHPP = function() return 51 end
        assert(finalSting.onMobSkillCheck(target, mob, skill) == 1)
        assert(finalSting.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 500 and params.fTP[1] == 0.5 and params.fTP[2] == 0.5 and params.fTP[3] == 0.5)
        assert(params.element == xi.element.NONE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.NONE and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(params.skipDamageAdjustment and params.skipMagicBonusDiff and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(finalSting.onMobWeaponSkill(mob, target, skill, {}) == 123)
        finalSting.onMobSkillFinalize(mob, skill)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = magicalMove, processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.NONE)
        assert(hp == 0 and animationSub == 1)
    end)
end)
