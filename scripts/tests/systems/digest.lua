require('scripts/actions/mobskills/digest')

describe('Digest mob skill', function()
    it('rejects close Slime-family users and drains HP only after processed magical damage', function()
        local digest = require('scripts/actions/mobskills/digest')
        local move, process, drainMove = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove
        local params, drain, message = nil, nil, nil
        local target = {}
        local skill = { setMsg = function(_, value) message = value end }
        local function mob(species, distance)
            return {
                getSpecies = function() return species end,
                checkDistance = function() return distance end,
                getMainLvl = function() return 50 end,
            }
        end

        for _, species in ipairs({ xi.mobSpecies.BOIL, xi.mobSpecies.CLOT, xi.mobSpecies.SCUM, xi.mobSpecies.SLIME }) do
            assert(digest.onMobSkillCheck(target, mob(species, 2.9), skill) == 1)
            assert(digest.onMobSkillCheck(target, mob(species, 3), skill) == 0)
        end
        assert(digest.onMobSkillCheck(target, mob(0, 0), skill) == 0)

        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123 }
        end
        xi.mobskills.processDamage = function() return false end
        xi.mobskills.mobDrainMove = function(_, _, drainType, amount)
            drain = { drainType, amount }
            return 456
        end
        local user = mob(xi.mobSpecies.SLIME, 3)
        assert(digest.onMobWeaponSkill(user, target, skill, {}) == 123)
        assert(params.baseDamage == 52 and params.fTP[1] == 2 and params.fTP[2] == 2 and params.fTP[3] == 2)
        assert(params.element == xi.element.NONE and params.attackType == xi.attackType.MAGICAL and params.damageType == xi.damageType.NONE)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_1 and params.skipMagicBonusDiff == true)
        assert(drain == nil and message == nil)

        xi.mobskills.processDamage = function() return true end
        assert(digest.onMobWeaponSkill(user, target, skill, {}) == 123)
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove = move, process, drainMove

        assert(drain[1] == xi.mobskills.drainType.HP and drain[2] == 123 and message == 456)
    end)
end)
