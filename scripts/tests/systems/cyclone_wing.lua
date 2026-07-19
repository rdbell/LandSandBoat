require('scripts/actions/mobskills/cyclone_wing')

describe('Cyclone Wing mob skill', function()
    it('rejects the wrong animation or a target behind the mob', function()
        local skill = require('scripts/actions/mobskills/cyclone_wing')
        local target = { isBehind = function() return false end }

        assert(skill.onMobSkillCheck(target, { getAnimationSub = function() return 1 end }, {}) == 1)
        assert(skill.onMobSkillCheck({ isBehind = function() return true end }, { getAnimationSub = function() return 0 end }, {}) == 1)
        assert(skill.onMobSkillCheck(target, { getAnimationSub = function() return 0 end }, {}) == 0)
    end)

    it('uses Dark magical parameters and applies Sleep only after processing succeeds', function()
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
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.DARK }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) applied = { ... } end

        local skill = require('scripts/actions/mobskills/cyclone_wing')
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 4 and params.fTP[2] == 4 and params.fTP[3] == 4)
        assert(params.element == xi.element.DARK and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.DARK)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS)
        assert(damage == nil and applied == nil)

        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 123)

        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, effect
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.DARK)
        assert(applied[3] == xi.effect.SLEEP_I and applied[4] == 1 and applied[5] == 0 and applied[6] == 60)
    end)
end)
