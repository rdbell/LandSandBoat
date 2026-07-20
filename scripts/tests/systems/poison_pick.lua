require('scripts/actions/mobskills/poison_pick')

describe('Poison Pick mob skill', function()
    it('uses default or Zizzy Zillah fTP and level-scaled Poison after processing', function()
        local pick = require('scripts/actions/mobskills/poison_pick')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local pool, params, damage, statusParams = 0, nil, nil, nil
        local mob = {
            getWeaponDmg = function() return 77 end,
            getPool = function() return pool end,
            getMainLvl = function() return 50 end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return true end
        assert(pick.onMobSkillCheck(target, mob, {}) == 0)
        pick.onMobWeaponSkill(mob, target, {}, {})
        assert(params.fTP[1] == 1 and statusParams[4] == 13 and statusParams[5] == 3 and statusParams[6] == 60)
        pool = xi.mobPool.ZIZZY_ZILLAH
        pick.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(params.fTP[1] == 1.5)
        assert(damage[1] == 123)
    end)
end)
