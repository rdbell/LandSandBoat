require('scripts/actions/mobskills/blade_retsu')
describe('Blade Retsu mob skill', function()
    it('uses two-hit plan and level-scaled Paralysis with TP duration', function()
        local skill = require('scripts/actions/mobskills/blade_retsu')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        local params, damage, effect = nil, nil, nil
        local mob = {
            getWeaponDmg = function() return 50 end,
            getMainLvl = function() return 50 end,
        }
        local sk = { getTP = function() return 1000 end }
        local target = {
            getMainLvl = function() return 50 end,
            takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end,
        }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 80, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, status, power, tick, duration)
            effect = { status, power, tick, duration }
        end
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 80)
        assert(params.numHits == 2 and damage == nil and effect == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 80)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(damage[1] == 80 and effect[1] == xi.effect.PARALYSIS and effect[2] == 30 and effect[4] == 30)
    end)
end)
