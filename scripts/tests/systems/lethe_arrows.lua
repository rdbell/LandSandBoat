require('scripts/actions/mobskills/lethe_arrows')

describe('Lethe Arrows mob skill', function()
    it('uses its physical plan and applies Bind then Amnesia only after processing', function()
        local arrows = require('scripts/actions/mobskills/lethe_arrows')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, effects = nil, nil, {}
        local mob = { getWeaponDmg = function() return 77 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, ...) effects[#effects + 1] = { ... } end

        assert(arrows.onMobSkillCheck(target, mob, {}) == 0)
        assert(arrows.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 1 and params.fTP[1] == 4 and params.fTP[2] == 4 and params.fTP[3] == 4)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(damage == nil and #effects == 0)

        xi.mobskills.processDamage = function() return true end
        assert(arrows.onMobWeaponSkill(mob, target, {}, {}) == 123)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status

        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.PIERCING and damage[5].breakBind == false)
        assert(#effects == 2)
        assert(effects[1][1] == xi.effect.BIND and effects[1][2] == 1 and effects[1][3] == 0 and effects[1][4] == 120)
        assert(effects[2][1] == xi.effect.AMNESIA and effects[2][2] == 1 and effects[2][3] == 0 and effects[2][4] == 120)
    end)
end)
