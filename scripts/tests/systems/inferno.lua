describe('Inferno mob skill', function()
    it('uses its Fire magical plan and damages only after processing', function()
        local inferno = require('scripts/actions/mobskills/inferno')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.FIRE } end
        xi.mobskills.processDamage = function() return false end
        assert(inferno.onMobSkillCheck(target, mob, {}) == 0 and inferno.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 9 and params.fTP[2] == 9 and params.fTP[3] == 9 and params.element == xi.element.FIRE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.FIRE and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and params.dStatMultiplier == 2 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        inferno.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.FIRE)
    end)
end)
