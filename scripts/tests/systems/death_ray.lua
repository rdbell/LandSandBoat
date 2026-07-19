require('scripts/actions/mobskills/death_ray')

describe('Death Ray mob skill', function()
    it('uses its Dark magical plan and applies processed damage', function()
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 52 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        xi.mobskills.mobMagicalMove = function(_, _, _, _, request)
            params = request
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.DARK }
        end
        xi.mobskills.processDamage = function() return false end

        local deathRay = require('scripts/actions/mobskills/death_ray')
        assert(deathRay.onMobSkillCheck({}, {}, {}) == 0)
        assert(deathRay.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 52)
        assert(params.fTP[1] == 3 and params.fTP[2] == 3.75 and params.fTP[3] == 4.5)
        assert(params.element == xi.element.DARK and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.DARK)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(deathRay.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.MAGICAL and damage[4] == xi.damageType.DARK)
    end)
end)
