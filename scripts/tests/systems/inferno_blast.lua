describe('Inferno Blast mob skill', function()
    it('requires flying and uses its Fire magical plan only after processing', function()
        local blast = require('scripts/actions/mobskills/inferno_blast')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage
        local mob = { getAnimationSub = function() return 1 end, getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.FIRE } end
        xi.mobskills.processDamage = function() return false end
        assert(blast.onMobSkillCheck(target, mob, {}) == 0 and blast.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 7 and params.fTP[2] == 7 and params.fTP[3] == 7 and params.element == xi.element.FIRE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.FIRE and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and damage == nil)
        mob.getAnimationSub = function() return 0 end
        assert(blast.onMobSkillCheck(target, mob, {}) == 1)
        mob.getAnimationSub = function() return 1 end
        xi.mobskills.processDamage = function() return true end
        blast.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.FIRE)
    end)
end)
