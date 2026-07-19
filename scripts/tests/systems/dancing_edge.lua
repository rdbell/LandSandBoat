require('scripts/actions/mobskills/dancing_edge')

describe('Dancing Edge mob skill', function()
    it('is always available', function()
        local skill = require('scripts/actions/mobskills/dancing_edge')

        assert(skill.onMobSkillCheck({}, {}, {}) == 0)
    end)

    it('uses its five-hit physical plan and applies processed damage', function()
        local params, damage = nil, nil
        local mob = {
            getWeaponDmg = function()
                return 77
            end,
        }
        local target = {
            takeDamage = function(_, amount, source, attackType, damageType)
                damage = { amount, source, attackType, damageType }
            end,
        }
        local physicalMove = xi.mobskills.mobPhysicalMove
        local processDamage = xi.mobskills.processDamage
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, request)
            params = request
            return {
                damage = 123,
                attackType = xi.attackType.PHYSICAL,
                damageType = xi.damageType.PIERCING,
            }
        end
        xi.mobskills.processDamage = function()
            return true
        end

        local edge = require('scripts/actions/mobskills/dancing_edge')
        assert(edge.onMobWeaponSkill(mob, target, {}, {}) == 123)

        xi.mobskills.mobPhysicalMove = physicalMove
        xi.mobskills.processDamage = processDamage
        assert(params.baseDamage == 77 and params.numHits == 5)
        assert(params.fTP[1] == 1.1875 and params.fTP[2] == 1.1875 and params.fTP[3] == 1.1875)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.PIERCING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_5)
        assert(params.accuracyModifier[1] == 0 and params.accuracyModifier[2] == 30 and params.accuracyModifier[3] == 60)
        assert(damage[1] == 123 and damage[2] == mob)
        assert(damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.PIERCING)
    end)
end)
