require('scripts/actions/mobskills/wild_rage')
describe('Wild Rage mob skill', function()
    it('scales Platoon fTP and poisons as King Vinegarroon when processed', function()
        local skill = require('scripts/actions/mobskills/wild_rage')
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        local statusEffectMove = xi.mobskills.mobStatusEffectMove
        local params, damage, effect = nil, nil, nil
        local mob = {
            getWeaponDmg = function() return 70 end,
            getPool = function() return 0 end,
            getBattlefield = function() return nil end,
        }
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
        xi.mobskills.mobStatusEffectMove = function(_, _, status, power, tick, duration)
            effect = { status, power, tick, duration }
        end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.baseDamage == 70 and params.fTP[1] == 2.0 and params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3)
        assert(damage == nil and effect == nil)

        mob.getPool = function() return xi.mobPool.PLATOON_SCORPION end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 2.0)

        mob.getBattlefield = function()
            return { getLocalVar = function(_, name) assert(name == 'scorpionsDefeated'); return 3 end }
        end
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.fTP[1] == 3.5 and params.fTP[2] == 3.5 and params.fTP[3] == 3.5)
        assert(damage[1] == 100 and effect == nil)

        mob.getPool = function() return xi.mobPool.KING_VINEGARROON end
        effect = nil
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        xi.mobskills.mobStatusEffectMove = statusEffectMove
        assert(effect[1] == xi.effect.POISON and effect[2] == 25 and effect[3] == 3 and effect[4] == 60)
    end)
end)
