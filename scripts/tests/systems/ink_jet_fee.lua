describe('Ink Jet Fee mob skill', function()
    it('uses its Dark magical hit-damage plan and applies Blindness only after processing', function()
        local jet = require('scripts/actions/mobskills/ink_jet_fee')
        local move, process, effect = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, blindness
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.DARK } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, ...) blindness = { ... } end
        assert(jet.onMobSkillCheck(target, mob, {}) == 0 and jet.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 1.5 and params.fTP[2] == 2 and params.fTP[3] == 2.5 and params.element == xi.element.DARK and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.DARK and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and params.primaryMessage == xi.msg.basic.HIT_DMG and damage == nil and blindness == nil)
        xi.mobskills.processDamage = function() return true end
        jet.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, effect
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.DARK)
        assert(blindness[1] == xi.effect.BLINDNESS and blindness[2] == 80 and blindness[3] == 0 and blindness[4] == 120)
    end)
end)
