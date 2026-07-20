require('scripts/actions/mobskills/discharge')

describe('Discharge mob skill', function()
    it('uses its Thunder plan and applies Paralysis only after processed damage', function()
        local discharge = require('scripts/actions/mobskills/discharge')
        local magicalMove, processDamage, statusEffectMove = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, paralysis = nil, nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.THUNDER }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) paralysis = { ... } end

        assert(discharge.onMobSkillCheck({}, {}, {}) == 0)
        assert(discharge.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 4 and params.fTP[2] == 4 and params.fTP[3] == 4)
        assert(params.element == xi.element.THUNDER and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.THUNDER)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and params.dStatMultiplier == 1 and damage == nil and paralysis == nil)

        xi.mobskills.processDamage = function() return true end
        assert(discharge.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = magicalMove, processDamage, statusEffectMove
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.THUNDER)
        assert(paralysis[3] == xi.effect.PARALYSIS and paralysis[4] == 20 and paralysis[5] == 0 and paralysis[6] == 180)
    end)
end)
