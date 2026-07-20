describe('Infernal Scythe mob skill', function()
    it('uses its Dark magical plan and TP-scaled Attack Down only after processing', function()
        local scythe = require('scripts/actions/mobskills/infernal_scythe')
        local move, process, effect = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, attackDown
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = { getTP = function() return 1500 end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.DARK } end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, ...) attackDown = { ... } end
        assert(scythe.onMobSkillCheck(target, mob, skill) == 0 and scythe.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 3.5 and params.fTP[2] == 3.5 and params.fTP[3] == 3.5 and params.element == xi.element.DARK and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.DARK and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil and attackDown == nil)
        xi.mobskills.processDamage = function() return true end
        scythe.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, effect
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.DARK)
        assert(attackDown[1] == xi.effect.ATTACK_DOWN and attackDown[2] == 25 and attackDown[3] == 0 and attackDown[4] == 270)
    end)
end)
