require('scripts/actions/mobskills/acid_spray')

describe('Acid Spray mob skill', function()
    it('uses Water magical parameters and level-scaled Poison only after processing succeeds', function()
        local move, process, effect = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, applied = nil, nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = {
            takeDamage = function(_, value, source, attackType, damageType)
                damage = { value, source, attackType, damageType }
            end,
        }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, p)
            params = p
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WATER }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) applied = { ... } end
        local skill = require('scripts/actions/mobskills/acid_spray')
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1)
        assert(params.element == xi.element.WATER and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.WATER)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS)
        assert(damage == nil and applied == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, effect
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.WATER)
        assert(applied[3] == xi.effect.POISON and applied[4] == 10 and applied[5] == 3 and applied[6] == 60)
    end)
end)
