require('scripts/actions/mobskills/leeching_current')

describe('Leeching Current mob skill', function()
    it('uses the default or configured base split across targets and drains HP only after processing', function()
        local current = require('scripts/actions/mobskills/leeching_current')
        local move, process, drain = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove
        local params, drainCall = nil, nil
        local base = 0
        local mob = { getLocalVar = function(_, name) assert(name == 'leechingCurrent'); return base end }
        local target = {}
        local skill = { getTotalTargets = function() return 4 end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.NONE }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobDrainMove = function(_, _, drainType, amount) drainCall = { drainType, amount } end

        assert(current.onMobSkillCheck(target, mob, skill) == 0)
        assert(current.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 250 and params.fTP[1] == 1 and params.fTP[2] == 1 and params.fTP[3] == 1)
        assert(params.element == xi.element.NONE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.NONE)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.WIPE_SHADOWS and params.skipMagicBonusDiff)
        assert(drainCall == nil)

        base = 101
        skill.getTotalTargets = function() return 2 end
        xi.mobskills.processDamage = function() return true end
        assert(current.onMobWeaponSkill(mob, target, skill, {}) == 123)

        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove = move, process, drain
        assert(params.baseDamage == 50.5)
        assert(drainCall[1] == xi.mobskills.drainType.HP and drainCall[2] == 123)
    end)
end)
