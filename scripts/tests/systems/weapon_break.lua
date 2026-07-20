require('scripts/actions/mobskills/weapon_break')
describe('Weapon Break mob skill', function()
    it('uses slashing plan and TP-scaled attack down after processing', function()
        local skill = require('scripts/actions/mobskills/weapon_break')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 80 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local sk = { getTP = function() return 1000 end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 100)
        assert(params.fTP[1] == 1.0 and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, sk, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 100 and statusParams[3] == xi.effect.ATTACK_DOWN and statusParams[4] == 25 and statusParams[6] == 180)
    end)
end)
