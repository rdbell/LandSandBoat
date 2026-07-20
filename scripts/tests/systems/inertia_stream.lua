describe('Inertia Stream mob skill', function()
    it('uses its Light magical plan, preserves Bind on damage, and applies Bind only after processing', function()
        local stream = require('scripts/actions/mobskills/inertia_stream')
        local move, process, effect = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, bind
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.LIGHT } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, ...) bind = { ... } end
        assert(stream.onMobSkillCheck(target, mob, {}) == 0 and stream.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 6 and params.fTP[2] == 6 and params.fTP[3] == 6 and params.element == xi.element.LIGHT and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.LIGHT and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and damage == nil and bind == nil)
        xi.mobskills.processDamage = function() return true end
        stream.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, effect
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.LIGHT and damage[5].breakBind == false)
        assert(bind[1] == xi.effect.BIND and bind[2] == 1 and bind[3] == 0 and bind[4] == 30)
    end)
end)
