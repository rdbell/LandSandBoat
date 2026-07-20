describe('Inferno Blast Alt mob skill', function()
    it('uses its Fire magical hit-damage plan and damages only after processing', function()
        local blast = require('scripts/actions/mobskills/inferno_blast_alt')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value) params = value; return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.FIRE } end
        xi.mobskills.processDamage = function() return false end
        assert(blast.onMobSkillCheck(target, mob, {}) == 0 and blast.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 4 and params.fTP[2] == 4 and params.fTP[3] == 4 and params.element == xi.element.FIRE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.FIRE and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and params.primaryMessage == xi.msg.basic.HIT_DMG and damage == nil)
        xi.mobskills.processDamage = function() return true end
        blast.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.FIRE)
    end)
end)
