describe('Ion Shower mob skill', function()
    it('uses its Thunder plan and applies Stun only after processed damage', function()
        local shower = require('scripts/actions/mobskills/ion_shower')
        local move, process, status = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, stun
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }

        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.THUNDER }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(...) stun = { ... } end

        assert(shower.onMobSkillCheck(target, mob, {}) == 0 and shower.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        assert(params.element == xi.element.THUNDER and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.THUNDER and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and params.dStatMultiplier == 1 and damage == nil and stun == nil)

        xi.mobskills.processDamage = function() return true end
        shower.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status

        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.THUNDER)
        assert(stun[1] == mob and stun[2] == target and stun[3] == xi.effect.STUN and stun[4] == 1 and stun[5] == 0 and stun[6] == 5)
    end)
end)
