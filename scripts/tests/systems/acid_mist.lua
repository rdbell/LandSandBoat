require('scripts/actions/mobskills/acid_mist')

describe('Acid Mist mob skill', function()
    it('uses Water parameters and applies Attack Down only after processed damage', function()
        local move, process, effect = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, applied = nil, nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = {
            takeDamage = function(_, value, source, attackType, damageType)
                damage = { value, source, attackType, damageType }
            end,
        }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, p) params = p; return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WATER } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) applied = { ... } end
        local skill = require('scripts/actions/mobskills/acid_mist')
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 123 and damage == nil and applied == nil)
        assert(params.baseDamage == 52 and params.fTP[1] == 1.75 and params.fTP[2] == 2 and params.fTP[3] == 2.25)
        assert(params.element == xi.element.WATER and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, effect
        assert(damage[1] == 123 and damage[2] == mob)
        assert(damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.WATER)
        assert(applied[3] == xi.effect.ATTACK_DOWN and applied[4] == 50 and applied[5] == 0 and applied[6] == 120)
    end)
end)
