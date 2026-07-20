describe('Fire Meeble Warble mob skill', function()
    it('uses its Fire plan and applies Plague and Burn only after processing', function()
        local fireMeebleWarble = require('scripts/actions/mobskills/fire_meeble_warble')
        local magicalMove, processDamage, statusMove = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statuses = nil, nil, {}
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.FIRE }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, ...) statuses[#statuses + 1] = { ... } end
        assert(fireMeebleWarble.onMobSkillCheck(target, mob, {}) == 0)
        assert(fireMeebleWarble.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 9 and params.fTP[2] == 9 and params.fTP[3] == 9)
        assert(params.element == xi.element.FIRE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.FIRE and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS)
        assert(damage == nil and #statuses == 0)
        xi.mobskills.processDamage = function() return true end
        assert(fireMeebleWarble.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = magicalMove, processDamage, statusMove
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.FIRE)
        assert(#statuses == 2)
        assert(statuses[1][1] == xi.effect.PLAGUE and statuses[1][2] == 30 and statuses[1][3] == 3 and statuses[1][4] == 60)
        assert(statuses[2][1] == xi.effect.BURN and statuses[2][2] == 50 and statuses[2][3] == 3 and statuses[2][4] == 60)
    end)
end)
