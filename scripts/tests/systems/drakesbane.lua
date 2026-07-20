require('scripts/actions/mobskills/drakesbane')

describe('Drakesbane mob skill', function()
    it('uses its four-hit critical physical plan and applies damage only after processing', function()
        local drakesbane = require('scripts/actions/mobskills/drakesbane')
        local physicalMove, processDamage = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end

        assert(drakesbane.onMobSkillCheck({}, {}, {}) == 0)
        assert(drakesbane.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 4)
        assert(params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_4 and params.canCrit == true)
        assert(params.criticalChance[1] == 0.1 and params.criticalChance[2] == 0.3 and params.criticalChance[3] == 0.5)
        assert(params.attackMultiplier[1] == 0.8125 and params.attackMultiplier[2] == 0.8125 and params.attackMultiplier[3] == 0.8125)
        assert(damage == nil)

        xi.mobskills.processDamage = function() return true end
        assert(drakesbane.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = physicalMove, processDamage
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.PIERCING)
    end)
end)
