require('scripts/actions/mobskills/tail_crush')
describe('Tail Crush mob skill', function()
    it('uses crit-capable slashing plan and TP-scaled poison after processing', function()
        local skill = require('scripts/actions/mobskills/tail_crush')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 80 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local sk = { getTP = function() return 2000 end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 100)
        assert(params.canCrit == true and params.criticalChance[1] == 0.10 and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, sk, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        -- duration = 30 + min(floor(2000/1000),3)*30 = 90
        assert(damage[1] == 100 and statusParams[3] == xi.effect.POISON and statusParams[4] == 17 and statusParams[5] == 3 and statusParams[6] == 90)
    end)
end)
