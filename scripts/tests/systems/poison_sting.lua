require('scripts/actions/mobskills/poison_sting')

describe('Poison Sting mob skill', function()
    it('uses its slashing physical plan and applies level-scaled Poison after processing', function()
        local sting = require('scripts/actions/mobskills/poison_sting')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 77 end, getMainLvl = function() return 50 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(sting.onMobSkillCheck(target, mob, {}) == 0 and sting.onMobWeaponSkill(mob, target, {}, {}) == 123)
        assert(params.fTP[1] == 1 and params.damageType == xi.damageType.SLASHING)
        assert(damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        sting.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 123)
        assert(statusParams[3] == xi.effect.POISON and statusParams[4] == 8 and statusParams[5] == 3 and statusParams[6] == 60)
    end)
end)
