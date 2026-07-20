require('scripts/actions/mobskills/seedspray')
describe('Seedspray mob skill', function()
    it('uses threefold slashing plan and applies Defense Down after processing', function()
        local spray = require('scripts/actions/mobskills/seedspray')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 35 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 90, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(spray.onMobSkillCheck(target, mob, {}) == 0 and spray.onMobWeaponSkill(mob, target, {}, {}) == 90)
        assert(params.numHits == 3 and params.fTP[1] == 1 and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        spray.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 90 and statusParams[3] == xi.effect.DEFENSE_DOWN and statusParams[4] == 33 and statusParams[6] == 60)
    end)
end)
