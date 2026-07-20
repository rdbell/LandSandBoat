require('scripts/actions/mobskills/nocturnal_combustion')

describe('Nocturnal Combustion mob skill', function()
    it('uses its Dark HP-based magical plan, damages only after processing, then self-destructs', function()
        local combustion = require('scripts/actions/mobskills/nocturnal_combustion')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage, finalized = nil, nil, nil
        local mob = {
            setHP = function(_, value) finalized = value end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = { getMobHP = function() return 300 end }

        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.DARK }
        end
        xi.mobskills.processDamage = function() return false end

        assert(combustion.onMobSkillCheck(target, mob, skill) == 0 and combustion.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 100 and params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1)
        assert(params.element == xi.element.DARK and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.DARK)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil)

        xi.mobskills.processDamage = function() return true end
        combustion.onMobWeaponSkill(mob, target, skill, {})
        combustion.onMobSkillFinalize(mob, skill)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process

        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.DARK)
        assert(finalized == 0)
    end)
end)
