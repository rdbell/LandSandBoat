require('scripts/actions/mobskills/cold_wave')
describe('Cold Wave mob skill', function()
    it('scales Frost power by level and overrides for COLD_WAVE_2', function()
        local skill = require('scripts/actions/mobskills/cold_wave')
        local execute = xi.combat.action.executeMobskillStatusEffect
        local effectTable, lvl = nil, 50
        local mob = { getMainLvl = function() return lvl end }
        local sk = { getID = function() return 0 end }
        xi.combat.action.executeMobskillStatusEffect = function(_, _, _, table)
            effectTable = table
            return xi.effect.FROST
        end
        assert(skill.onMobSkillCheck({}, mob, sk) == 0)
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == xi.effect.FROST)
        assert(effectTable[1].effectId == xi.effect.FROST)
        assert(effectTable[1].power == 12 and effectTable[1].tick == 3 and effectTable[1].duration == 180)
        sk.getID = function() return xi.mobSkill.COLD_WAVE_2 end
        assert(skill.onMobWeaponSkill(mob, {}, sk, {}) == xi.effect.FROST)
        assert(effectTable[1].power == 32)
        lvl = 3
        sk.getID = function() return 0 end
        skill.onMobWeaponSkill(mob, {}, sk, {})
        assert(effectTable[1].power == 1)
        xi.combat.action.executeMobskillStatusEffect = execute
    end)
end)
