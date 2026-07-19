require('scripts/actions/mobskills/cannonball')

describe('Cannonball mob skill', function()
    it('requires curled form and uses weapon damage physical parameters', function()
        local cannonball = require('scripts/actions/mobskills/cannonball')
        local originalPhysicalMove = xi.mobskills.mobPhysicalMove
        local originalProcessDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = {
            getAnimationSub = function() return 5 end,
            getWeaponDmg = function() return 77 end,
        }
        local target = { takeDamage = function(_, value, source, attackType, damageType) damage = { value, source, attackType, damageType } end }
        local skill, action = {}, {}

        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end

        assert(cannonball.onMobSkillCheck(target, mob, skill) == 0)
        assert(cannonball.onMobSkillCheck(target, { getAnimationSub = function() return 4 end }, skill) == 1)
        assert(cannonball.onMobWeaponSkill(mob, target, skill, action) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1)
        assert(params.fTP[1] == 1.5 and params.fTP[2] == 1.5 and params.fTP[3] == 1.5)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.BLUNT)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and params.isCannonball)
        assert(damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(cannonball.onMobWeaponSkill(mob, target, skill, action) == 123)

        xi.mobskills.mobPhysicalMove = originalPhysicalMove
        xi.mobskills.processDamage = originalProcessDamage

        assert(damage[1] == 123 and damage[2] == mob)
        assert(damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.BLUNT)
    end)
end)
