require('scripts/actions/mobskills/spirit_absorption_gorger')
describe('Spirit Absorption Gorger mob skill', function()
    it('uses none-element magical plan and drains HP after processing', function()
        local abs = require('scripts/actions/mobskills/spirit_absorption_gorger')
        local move, process, drainMove = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove
        local params, drainArgs, message = nil, nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.NONE }
        end
        xi.mobskills.mobDrainMove = function(...) drainArgs = { ... }; return 456 end
        xi.mobskills.processDamage = function() return false end
        assert(abs.onMobSkillCheck({}, mob, skill) == 0 and abs.onMobWeaponSkill(mob, {}, skill, {}) == 100)
        assert(params.baseDamage == 52 and params.fTP[1] == 3.5 and params.skipMagicBonusDiff and drainArgs == nil)
        xi.mobskills.processDamage = function() return true end
        abs.onMobWeaponSkill(mob, {}, skill, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove = move, process, drainMove
        assert(drainArgs[3] == xi.mobskills.drainType.HP and drainArgs[4] == 100 and message == 456)
    end)
end)
