describe('Implosion mob skill', function()
    it('uses its captured Dark magical plan and damages only after processing', function()
        local implosion = require('scripts/actions/mobskills/implosion')
        local move = xi.mobskills.mobMagicalMove
        local process = xi.mobskills.processDamage
        local params, damage
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }

        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.DARK }
        end
        xi.mobskills.processDamage = function() return false end

        assert(implosion.onMobSkillCheck(target, mob, {}) == 0)
        assert(implosion.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1)
        assert(params.element == xi.element.DARK and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.DARK)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and damage == nil)

        xi.mobskills.processDamage = function() return true end
        implosion.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove = move
        xi.mobskills.processDamage = process

        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.DARK)
    end)
end)
