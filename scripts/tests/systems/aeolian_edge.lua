require('scripts/actions/mobskills/aeolian_edge')
describe('Aeolian Edge mob skill', function()
    it('uses Wind magical plan with INT dStat and processed damage', function()
        local skill = require('scripts/actions/mobskills/aeolian_edge')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local target = {
            takeDamage = function(_, value, source, attackType, damageType)
                damage = { value, source, attackType, damageType }
            end,
        }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 120, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.WIND }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 120)
        assert(params.baseDamage == 52 and params.fTP[1] == 2.75 and params.fTP[2] == 3.5 and params.fTP[3] == 4.0)
        assert(params.dStatMultiplier == 1 and params.dStatAttackerMod == xi.mod.INT and params.dStatDefenderMod == xi.mod.INT)
        assert(damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 120)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
        assert(damage[1] == 120)
    end)
end)
