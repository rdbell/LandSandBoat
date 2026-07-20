require('scripts/actions/mobskills/thunderbolt_raptor')
describe('Thunderbolt Raptor mob skill', function()
    it('denies Uleguerand Range and uses thunder breath + stun', function()
        local skill = require('scripts/actions/mobskills/thunderbolt_raptor')
        local move, process, status = xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams = nil, nil, nil
        local mob = { getZoneID = function() return 5 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        mob.getZoneID = function() return 1 end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobBreathMove = function(_, _, _, _, value)
            params = value
            return { damage = 200, attackType = xi.attackType.BREATH, damageType = xi.damageType.THUNDER }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 200)
        assert(params.percentMultipier == 0.0833 and params.damageCap == 500 and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobBreathMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 200 and statusParams[3] == xi.effect.STUN and statusParams[6] == 7)
    end)
end)
