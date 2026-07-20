require('scripts/actions/mobskills/brain_spike')
describe('Brain Spike mob skill', function()
    it('rejects NM and uses piercing crit plan with Paralysis when processed', function()
        local skill = require('scripts/actions/mobskills/brain_spike')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local statusMove = xi.mobskills.mobStatusEffectMove
        local params, damage, status = nil, nil, nil
        local mob = {
            isMobType = function(_, t) return t == xi.mobType.NOTORIOUS end,
            getWeaponDmg = function() return 50 end,
        }
        assert(skill.onMobSkillCheck({}, mob, {}) == 1)
        mob.isMobType = function() return false end
        assert(skill.onMobSkillCheck({}, mob, {}) == 0)
        local target = {
            takeDamage = function(_, value, source, attackType, damageType)
                damage = { value, source, attackType, damageType }
            end,
        }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 75, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            status = { effect, power, tick, duration }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 75)
        assert(params.canCrit and params.damageType == xi.damageType.PIERCING)
        assert(damage == nil and status == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 75)
        assert(damage[1] == 75 and status[1] == xi.effect.PARALYSIS and status[2] == 15 and status[4] == 90)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusMove
    end)
end)
