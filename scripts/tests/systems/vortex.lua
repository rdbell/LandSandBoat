require('scripts/actions/mobskills/vortex')
describe('Vortex mob skill', function()
    it('uses slashing plan and applies terror/bind/enmity reset after processing', function()
        local skill = require('scripts/actions/mobskills/vortex')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, calls, reset = nil, nil, {}, false
        local mob = {
            getWeaponDmg = function() return 80 end,
            resetEnmity = function() reset = true end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) calls[#calls + 1] = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 1.5 and #calls == 0 and not reset)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 100 and reset)
        assert(calls[1][3] == xi.effect.TERROR and calls[1][6] == 9)
        assert(calls[2][3] == xi.effect.BIND and calls[2][6] == 30)
    end)
end)
