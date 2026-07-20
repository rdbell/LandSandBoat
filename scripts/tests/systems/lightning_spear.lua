require('scripts/actions/mobskills/lightning_spear')

describe('Lightning Spear mob skill', function()
    it('uses a Thunder magical plan and applies Amnesia only after processing', function()
        local spear = require('scripts/actions/mobskills/lightning_spear')
        local move, process, status, duration = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove, xi.mobskills.calculateDuration
        local params, damage, amnesia = nil, nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.THUNDER }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, ...) amnesia = { ... } end
        xi.mobskills.calculateDuration = function(tp, minimum)
            assert(tp == 30 and minimum == 120)
            return 120
        end

        assert(spear.onMobSkillCheck(target, mob, {}) == 0)
        assert(spear.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 20 and params.fTP[2] == 20 and params.fTP[3] == 20)
        assert(params.element == xi.element.THUNDER and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.THUNDER)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil and amnesia == nil)

        xi.mobskills.processDamage = function() return true end
        assert(spear.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove, xi.mobskills.calculateDuration = move, process, status, duration
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.THUNDER)
        assert(amnesia[1] == xi.effect.AMNESIA and amnesia[2] == 1 and amnesia[3] == 0 and amnesia[4] == 120)
    end)
end)
