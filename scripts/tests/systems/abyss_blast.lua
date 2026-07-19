require('scripts/actions/mobskills/abyss_blast')

describe('Abyss Blast mob skill', function()
    it('uses Dark magical parameters and applies damage and Blindness only after processing', function()
        local move, process, effectMove = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, blindness = nil, nil, nil
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.DARK }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) blindness = { ... } end
        local blast = require('scripts/actions/mobskills/abyss_blast')
        assert(blast.onMobSkillCheck(target, mob, {}) == 0 and blast.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 2.5 and params.fTP[2] == 3.5 and params.fTP[3] == 4.5)
        assert(params.element == xi.element.DARK and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.DARK)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil and blindness == nil)
        xi.mobskills.processDamage = function() return true end
        assert(blast.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, effectMove
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.DARK)
        assert(blindness[3] == xi.effect.BLINDNESS and blindness[4] == 20 and blindness[5] == 0 and blindness[6] == 120)
    end)
end)
