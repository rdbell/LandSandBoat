describe('Ice Shot mob skill', function()
    it('uses its fractional-base Ice magical plan and damages only after processing', function()
        local shot = require('scripts/actions/mobskills/ice_shot')
        local move = xi.mobskills.mobMagicalMove
        local process = xi.mobskills.processDamage
        local params, damage
        local mob = { getMainLvl = function() return 75 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }

        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.ICE }
        end
        xi.mobskills.processDamage = function() return false end

        assert(shot.onMobSkillCheck(target, mob, {}) == 0)
        assert(shot.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 187.5 and params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1)
        assert(params.element == xi.element.ICE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.ICE)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil)

        xi.mobskills.processDamage = function() return true end
        shot.onMobWeaponSkill(mob, target, {}, {})

        xi.mobskills.mobMagicalMove = move
        xi.mobskills.processDamage = process
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.ICE)
    end)
end)
