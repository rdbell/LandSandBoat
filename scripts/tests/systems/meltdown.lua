require('scripts/actions/mobskills/meltdown')

describe('Meltdown mob skill', function()
    it('rejects notorious mobs, damages only after processing, then self-destructs', function()
        local meltdown = require('scripts/actions/mobskills/meltdown')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage, finalized = nil, nil, nil
        local notorious = false
        local mob = {
            isMobType = function(_, value) assert(value == xi.mobType.NOTORIOUS); return notorious end,
            setHP = function(_, value) finalized = value end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = { getMobHP = function() return 501 end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.LIGHT }
        end
        xi.mobskills.processDamage = function() return false end

        assert(meltdown.onMobSkillCheck(target, mob, skill) == 0 and meltdown.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 250.5 and params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1)
        assert(params.element == xi.element.LIGHT and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.LIGHT and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil)
        notorious = true
        assert(meltdown.onMobSkillCheck(target, mob, skill) == 1)
        notorious = false
        xi.mobskills.processDamage = function() return true end
        meltdown.onMobWeaponSkill(mob, target, skill, {})
        meltdown.onMobSkillFinalize(mob, skill)

        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.LIGHT and finalized == 0)
    end)
end)
