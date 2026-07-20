describe('Impulsion mob skill', function()
    it('uses its captured no-element plan and applies Petrification then Blindness only after processing', function()
        local impulsion = require('scripts/actions/mobskills/impulsion')
        local move, process, effect = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, effects = nil, nil, {}
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.NONE } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, ...) effects[#effects + 1] = { ... } end
        assert(impulsion.onMobSkillCheck(target, mob, {}) == 0 and impulsion.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 75 and params.fTP[1] == 2.75 and params.fTP[2] == 2.75 and params.fTP[3] == 2.75 and params.element == xi.element.NONE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.NONE and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil and #effects == 0)
        xi.mobskills.processDamage = function() return true end
        impulsion.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, effect
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.NONE)
        assert(#effects == 2 and effects[1][1] == xi.effect.PETRIFICATION and effects[1][2] == 1 and effects[1][3] == 0 and effects[1][4] == 15 and effects[2][1] == xi.effect.BLINDNESS and effects[2][2] == 15 and effects[2][3] == 0 and effects[2][4] == 60)
    end)
end)
