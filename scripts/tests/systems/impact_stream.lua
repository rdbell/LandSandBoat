describe('Impact Stream mob skill', function()
    it('uses its no-element magical plan and TP-scaled Defense Down plus Stun only after processing', function()
        local stream = require('scripts/actions/mobskills/impact_stream')
        local move = xi.mobskills.mobMagicalMove
        local process = xi.mobskills.processDamage
        local effect = xi.mobskills.mobStatusEffectMove
        local params, damage, effects = nil, nil, {}
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = { getTP = function() return 2000 end }

        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.NONE }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, ...) effects[#effects + 1] = { ... } end

        assert(stream.onMobSkillCheck(target, mob, skill) == 0)
        assert(stream.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 75 and params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        assert(params.element == xi.element.NONE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.NONE)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and damage == nil and #effects == 0)

        xi.mobskills.processDamage = function() return true end
        stream.onMobWeaponSkill(mob, target, skill, {})

        xi.mobskills.mobMagicalMove = move
        xi.mobskills.processDamage = process
        xi.mobskills.mobStatusEffectMove = effect
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.NONE)
        assert(#effects == 2)
        assert(effects[1][1] == xi.effect.DEFENSE_DOWN and effects[1][2] == 55 and effects[1][3] == 0 and effects[1][4] == 60)
        assert(effects[2][1] == xi.effect.STUN and effects[2][2] == 1 and effects[2][3] == 0 and effects[2][4] == 4)
    end)
end)
