require('scripts/actions/mobskills/osmosis')

describe('Osmosis mob skill', function()
    it('drains HP then steals a dispelable effect after processed magical damage', function()
        local osmosis = require('scripts/actions/mobskills/osmosis')
        local oldMove, oldProcess, oldDrain = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove
        local params, order, message = nil, {}, nil
        local mob = {
            getMainLvl = function() return 50 end,
            stealStatusEffect = function(_, target, flag) order[#order + 1] = { 'steal', target, flag } end,
        }
        local target = {}
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, got) params = got return { damage = 123 } end
        xi.mobskills.processDamage = function() return true end
        xi.mobskills.mobDrainMove = function(_, _, drainType, damage)
            order[#order + 1] = { 'drain', drainType, damage }
            return 456
        end

        assert(osmosis.onMobSkillCheck(target, mob, skill) == 0)
        assert(osmosis.onMobWeaponSkill(mob, target, skill, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove = oldMove, oldProcess, oldDrain

        assert(params.baseDamage == 52 and params.fTP[1] == 5 and params.fTP[2] == 5 and params.fTP[3] == 5)
        assert(params.skipMagicBonusDiff and params.shadowBehavior == xi.mobskills.shadowBehavior.IGNORE_SHADOWS)
        assert(order[1][1] == 'drain' and order[1][2] == xi.mobskills.drainType.HP and order[1][3] == 123)
        assert(order[2][1] == 'steal' and order[2][2] == target and order[2][3] == xi.effectFlag.DISPELABLE)
        assert(message == 456)
    end)
end)
