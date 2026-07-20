require('scripts/actions/mobskills/benthic_typhoon')
describe('Benthic Typhoon mob skill', function()
    it('uses Piercing plan and processed dual def downs', function()
        local skill = require('scripts/actions/mobskills/benthic_typhoon')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        local params, damage, effects = nil, nil, {}
        local mob = { getWeaponDmg = function() return 50 end }
        local target = { takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 110, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, status, power, tick, duration)
            table.insert(effects, { status, power, tick, duration })
        end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 110)
        assert(params.fTP[1] == 2.3 and #effects == 0 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 110)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(damage[1] == 110)
        assert(effects[1][1] == xi.effect.MAGIC_DEF_DOWN and effects[1][2] == 30 and effects[1][4] == 60)
        assert(effects[2][1] == xi.effect.DEFENSE_DOWN and effects[2][2] == 30 and effects[2][4] == 60)
    end)
end)
