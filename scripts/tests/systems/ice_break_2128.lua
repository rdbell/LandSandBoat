describe('Ice Break 2128 mob skill', function()
    it('uses its Ice magical plan, hit-damage message, and TP-scaled Bind only after processing', function()
        local ice = require('scripts/actions/mobskills/ice_break_2128')
        local move = xi.mobskills.mobMagicalMove
        local process = xi.mobskills.processDamage
        local effect = xi.mobskills.mobStatusEffectMove
        local duration = xi.mobskills.calculateDuration
        local params, damage, bind
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = { getTP = function() return 1500 end }

        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.ICE }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, ...) bind = { ... } end
        xi.mobskills.calculateDuration = function(tp, min, max)
            assert(tp == 1500 and min == 120 and max == 180)
            return 150
        end

        assert(ice.onMobSkillCheck(target, mob, skill) == 0)
        assert(ice.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        assert(params.element == xi.element.ICE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.ICE)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and params.dStatMultiplier == 1)
        assert(params.primaryMessage == xi.msg.basic.HIT_DMG and damage == nil and bind == nil)

        xi.mobskills.processDamage = function() return true end
        ice.onMobWeaponSkill(mob, target, skill, {})

        xi.mobskills.mobMagicalMove = move
        xi.mobskills.processDamage = process
        xi.mobskills.mobStatusEffectMove = effect
        xi.mobskills.calculateDuration = duration

        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.ICE)
        assert(damage[5].breakBind == false)
        assert(bind[1] == xi.effect.BIND and bind[2] == 1 and bind[3] == 0 and bind[4] == 150)
    end)
end)
