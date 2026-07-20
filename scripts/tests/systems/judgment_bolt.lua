describe('Judgment Bolt mob skill', function()
    it('uses its Thunder magical plan and damages only after processing', function()
        local bolt = require('scripts/actions/mobskills/judgment_bolt')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.THUNDER } end
        xi.mobskills.processDamage = function() return false end
        assert(bolt.onMobSkillCheck(target, mob, {}) == 0 and bolt.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 9 and params.fTP[2] == 9 and params.fTP[3] == 9 and params.element == xi.element.THUNDER and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.THUNDER and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and params.dStatMultiplier == 2 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        bolt.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.THUNDER)
    end)
end)
