describe('Ice Guillotine mob skill', function()
    it('uses its Ice magical plan and applies its ordered effects only after damage processing', function()
        local guillotine = require('scripts/actions/mobskills/ice_guillotine')
        local move = xi.mobskills.mobMagicalMove
        local process = xi.mobskills.processDamage
        local effect = xi.mobskills.mobStatusEffectMove
        local params, damage, effects = nil, nil, {}
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }

        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.ICE }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, ...) effects[#effects + 1] = { ... } end

        assert(guillotine.onMobSkillCheck(target, mob, {}) == 0)
        assert(guillotine.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 3 and params.fTP[2] == 3 and params.fTP[3] == 3)
        assert(params.element == xi.element.ICE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.ICE)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil and #effects == 0)

        xi.mobskills.processDamage = function() return true end
        guillotine.onMobWeaponSkill(mob, target, {}, {})

        xi.mobskills.mobMagicalMove = move
        xi.mobskills.processDamage = process
        xi.mobskills.mobStatusEffectMove = effect

        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.ICE)
        assert(#effects == 2)
        assert(effects[1][1] == xi.effect.MAX_HP_DOWN and effects[1][2] == 50 and effects[1][3] == 0 and effects[1][4] == 180)
        assert(effects[2][1] == xi.effect.AMNESIA and effects[2][2] == 1 and effects[2][3] == 0 and effects[2][4] == 60)
    end)
end)
