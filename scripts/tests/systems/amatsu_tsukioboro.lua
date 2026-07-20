require('scripts/actions/mobskills/amatsu_tsukioboro')
describe('Amatsu Tsukioboro mob skill', function()
    it('uses its physical plan and processed status', function()
        local skill = require('scripts/actions/mobskills/amatsu_tsukioboro')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 50 end }
        local target = {
            takeDamage = function(_, value, source, attackType, damageType)
                damage = { value, source, attackType, damageType }
            end,
        }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end

        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        local effect = nil
        xi.mobskills.mobStatusEffectMove = function(_, _, status, power, tick, duration)
            effect = { status, power, tick, duration }
        end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.numHits == 1 and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1)
        assert(damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage

        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(effect[1] == xi.effect.SILENCE and effect[2] == 1 and effect[4] == 60)
        assert(damage[1] == 100)
    end)
end)
