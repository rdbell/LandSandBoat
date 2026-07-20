require('scripts/actions/mobskills/seismostomp')
describe('Seismostomp mob skill', function()
    it('uses blunt physical plan and applies Stun after processing', function()
        local stomp = require('scripts/actions/mobskills/seismostomp')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getWeaponDmg = function() return 48 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 96, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(stomp.onMobSkillCheck(target, mob, {}) == 0 and stomp.onMobWeaponSkill(mob, target, {}, {}) == 96)
        assert(params.fTP[1] == 2 and damage == nil and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        stomp.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 96 and statusParams[3] == xi.effect.STUN and statusParams[6] == 4)
    end)
end)
