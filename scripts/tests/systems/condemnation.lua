require('scripts/actions/mobskills/condemnation')
describe('Condemnation mob skill', function()
    it('admits Dynamis/Uleguerand and uses three-hit Stun plan', function()
        local skill = require('scripts/actions/mobskills/condemnation')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local statusMove = xi.mobskills.mobStatusEffectMove
        local params, damage, status = nil, nil, nil
        local zone = 0
        local mob = {
            isInDynamis = function() return false end,
            getZoneID = function() return zone end,
            getWeaponDmg = function() return 50 end,
        }
        local target = { takeDamage = function(_, v) damage = v end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        zone = xi.zone.ULEGUERAND_RANGE
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        mob.isInDynamis = function() return true end
        zone = 0
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.mobStatusEffectMove = function(_, _, effect, power, tick, duration)
            status = { effect, power, tick, duration }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.numHits == 3 and params.fTP[1] == 3 and status == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(damage == 100 and status[1] == xi.effect.STUN and status[4] == 12)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusMove
    end)
end)
