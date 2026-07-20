require('scripts/actions/mobskills/tidal_dive')
describe('Tidal Dive mob skill', function()
    it('requires water terrain and applies bind+weight after processing', function()
        local skill = require('scripts/actions/mobskills/tidal_dive')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusCalls, terrain = nil, nil, {}, xi.terrain.GRASS
        local zone = { getTerrainType = function() return terrain end }
        local mob = {
            getZone = function() return zone end,
            getPos = function() return {} end,
            getWeaponDmg = function() return 80 end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 1)
        terrain = xi.terrain.DEEP_WATER
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        terrain = xi.terrain.SHALLOW_WATER
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusCalls[#statusCalls + 1] = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3 and #statusCalls == 0)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 100)
        assert(statusCalls[1][3] == xi.effect.BIND and statusCalls[1][6] == 60)
        assert(statusCalls[2][3] == xi.effect.WEIGHT and statusCalls[2][4] == 50 and statusCalls[2][6] == 120)
    end)
end)
