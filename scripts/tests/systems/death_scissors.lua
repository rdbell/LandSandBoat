require('scripts/actions/mobskills/death_scissors')

describe('Death Scissors mob skill', function()
    it('uses its critical physical plan and guarantees King Vinegarroon criticals', function()
        local params, damage = nil, nil
        local pool = 0
        local mob = {
            getWeaponDmg = function() return 77 end,
            getPool = function() return pool end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, request)
            params = request
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end

        local scissors = require('scripts/actions/mobskills/death_scissors')
        assert(scissors.onMobSkillCheck({}, {}, {}) == 0)
        assert(scissors.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1)
        assert(params.fTP[1] == 4 and params.fTP[2] == 4 and params.fTP[3] == 4)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(params.attackMultiplier[1] == 2.5 and params.attackMultiplier[2] == 2.5 and params.attackMultiplier[3] == 2.5)
        assert(params.canCrit and params.criticalChance[1] == 0.10 and params.criticalChance[2] == 0.20 and params.criticalChance[3] == 0.25)
        assert(damage == nil)

        pool = xi.mobPool.KING_VINEGARROON
        xi.mobskills.processDamage = function() return true end
        assert(scissors.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        assert(params.criticalChance[1] == 1 and params.criticalChance[2] == 1 and params.criticalChance[3] == 1)
        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
    end)
end)
