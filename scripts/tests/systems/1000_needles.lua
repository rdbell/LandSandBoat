require('scripts/actions/mobskills/1000_needles')

describe('1000 Needles mob skill', function()
    it('splits fixed damage across targets and only applies processed damage', function()
        local needles = require('scripts/actions/mobskills/1000_needles')
        local originalPhysicalMove = xi.mobskills.mobPhysicalMove
        local originalProcessDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob, target = {}, {}
        local skill = { getTotalTargets = function() return 4 end }
        local action = {}

        target.takeDamage = function(_, value, source, attackType, damageType)
            damage = { value, source, attackType, damageType }
        end
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 250, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end

        assert(needles.onMobSkillCheck(target, mob, skill) == 0)
        assert(needles.onMobWeaponSkill(mob, target, skill, action) == 250)
        assert(params.baseDamage == 250 and params.numHits == 1)
        assert(params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS)
        assert(params.guaranteedFirstHit and params.skipPDIF and params.skipFSTR)
        assert(params.skipParry and params.skipGuard and params.skipBlock)
        assert(damage == nil)

        skill.getTotalTargets = function() return 3 end
        assert(needles.onMobWeaponSkill(mob, target, skill, action) == 250)
        assert(params.baseDamage == 1000 / 3)

        skill.getTotalTargets = function() return 1 end
        xi.mobskills.processDamage = function() return true end
        assert(needles.onMobWeaponSkill(mob, target, skill, action) == 250)

        xi.mobskills.mobPhysicalMove = originalPhysicalMove
        xi.mobskills.processDamage = originalProcessDamage

        assert(params.baseDamage == 1000)
        assert(damage[1] == 250 and damage[2] == mob)
        assert(damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.PIERCING)
    end)
end)
