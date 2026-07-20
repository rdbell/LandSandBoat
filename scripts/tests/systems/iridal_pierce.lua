describe('Iridal Pierce mob skill', function()
    it('uses its captured Light plan and damages only after processing', function()
        local pierce = require('scripts/actions/mobskills/iridal_pierce')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage
        local mob = { getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }

        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.LIGHT }
        end
        xi.mobskills.processDamage = function() return false end

        assert(pierce.onMobSkillCheck(target, mob, {}) == 0 and pierce.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 7 and params.fTP[2] == 7 and params.fTP[3] == 7)
        assert(params.element == xi.element.LIGHT and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.LIGHT and params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and damage == nil)

        xi.mobskills.processDamage = function() return true end
        pierce.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process

        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.LIGHT)
    end)
end)
