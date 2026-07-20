require('scripts/actions/mobskills/mind_blast')

describe('Mind Blast mob skill', function()
    it('uses its Thunder magical plan and applies TP-scaled Paralysis only after processing', function()
        local mind = require('scripts/actions/mobskills/mind_blast')
        local move, process, status, duration = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove, xi.mobskills.calculateDuration
        local params, damage, paralysis = nil, nil, nil
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = { getTP = function() return 1500 end }

        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.THUNDER }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) paralysis = { ... } end
        xi.mobskills.calculateDuration = function(tp, min, max)
            assert(tp == 1500 and min == 15 and max == 45)
            return 30
        end

        assert(mind.onMobSkillCheck(target, mob, skill) == 0 and mind.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        assert(params.element == xi.element.THUNDER and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.THUNDER)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and params.dStatMultiplier == 1.5)
        assert(damage == nil and paralysis == nil)

        xi.mobskills.processDamage = function() return true end
        assert(mind.onMobWeaponSkill(mob, target, skill, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove, xi.mobskills.calculateDuration = move, process, status, duration

        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.THUNDER)
        assert(paralysis[3] == xi.effect.PARALYSIS and paralysis[4] == 20 and paralysis[5] == 0 and paralysis[6] == 30)
    end)
end)
