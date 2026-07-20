require('scripts/actions/mobskills/spirit_absorption_thinker')
describe('Spirit Absorption Thinker mob skill', function()
    it('uses mainLevel base and drains HP after processing', function()
        local abs = require('scripts/actions/mobskills/spirit_absorption_thinker')
        local move, process, drainMove = xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove
        local params, drainArgs, message = nil, nil, nil
        local mob = { getMainLvl = function() return 50 end }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 80, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.NONE }
        end
        xi.mobskills.mobDrainMove = function(...) drainArgs = { ... }; return 456 end
        xi.mobskills.processDamage = function() return false end
        assert(abs.onMobSkillCheck({}, mob, skill) == 0 and abs.onMobWeaponSkill(mob, {}, skill, {}) == 80)
        assert(params.baseDamage == 50 and params.fTP[1] == 2.0 and drainArgs == nil)
        xi.mobskills.processDamage = function() return true end
        abs.onMobWeaponSkill(mob, {}, skill, {})
        xi.mobskills.mobMagicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove = move, process, drainMove
        assert(drainArgs[4] == 80 and message == 456)
    end)
end)
