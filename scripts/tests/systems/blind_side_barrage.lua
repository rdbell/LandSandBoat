require('scripts/actions/mobskills/blind_side_barrage')
describe('Blind Side Barrage mob skill', function()
    it('uses Blunt crit plan and level-scaled MND/INT Down', function()
        local skill = require('scripts/actions/mobskills/blind_side_barrage')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        local params, damage, effects = nil, nil, {}
        local mob = {
            getWeaponDmg = function() return 50 end,
            getMainLvl = function() return 50 end,
        }
        local target = { takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 80, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobStatusEffectMove = function(_, _, status, power, tick, duration)
            table.insert(effects, { status, power, tick, duration })
        end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 80)
        assert(params.canCrit and damage == nil and #effects == 0)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 80)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(damage[1] == 80)
        assert(effects[1][1] == xi.effect.MND_DOWN and effects[1][2] == 13 and effects[1][4] == 120)
        assert(effects[2][1] == xi.effect.INT_DOWN and effects[2][2] == 13)
    end)
end)
