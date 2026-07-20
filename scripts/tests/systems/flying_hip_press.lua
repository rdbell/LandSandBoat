describe('Flying Hip Press mob skill', function()
    it('uses its pool-specific magical Wind plans and damages only after processing', function()
        local press = require('scripts/actions/mobskills/flying_hip_press')
        local move, process = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage
        local params, damage, pool = nil, nil, 0
        local mob = { getMainLvl = function() return 75 end, getPool = function() return pool end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WIND }
        end
        xi.mobskills.processDamage = function() return false end
        assert(press.onMobSkillCheck(target, mob, {}) == 0 and press.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2 and params.element == xi.element.WIND and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.WIND and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS and damage == nil)
        pool = xi.mobPool.BUGBOY
        press.onMobWeaponSkill(mob, target, {}, {})
        assert(params.fTP == 7)
        pool = xi.mobPool.BUGBEAR_MATMAN
        xi.mobskills.processDamage = function() return true end
        press.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage = move, process
        assert(params.fTP == 10 and damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.WIND)
    end)
end)
