require('scripts/actions/mobskills/orcish_counterstance')

describe('Orcish Counterstance mob skill', function()
    it('allows use only in Shadowreign Era and applies fixed Counterstance self-buff', function()
        local counterstance = require('scripts/actions/mobskills/orcish_counterstance')
        local buff = xi.mobskills.mobBuffMove
        local continent, params, message = xi.continent.THE_SHADOWREIGN_ERA, nil, nil
        local mob = {
            getContinentID = function()
                return continent
            end,
        }
        local skill = { setMsg = function(_, value) message = value end }

        xi.mobskills.mobBuffMove = function(...)
            params = { ... }
            return 456
        end

        continent = xi.continent.THE_MIDDLE_LANDS
        assert(counterstance.onMobSkillCheck({}, mob, skill) == 1)
        continent = xi.continent.THE_ARADJIAH_CONTINENT
        assert(counterstance.onMobSkillCheck({}, mob, skill) == 1)
        continent = xi.continent.THE_SHADOWREIGN_ERA
        assert(counterstance.onMobSkillCheck({}, mob, skill) == 0)
        continent = xi.continent.OTHER_AREAS
        assert(counterstance.onMobSkillCheck({}, mob, skill) == 1)

        continent = xi.continent.THE_SHADOWREIGN_ERA
        assert(counterstance.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.COUNTERSTANCE)
        xi.mobskills.mobBuffMove = buff

        assert(params[1] == mob and params[2] == xi.effect.COUNTERSTANCE)
        assert(params[3] == 10 and params[4] == 0 and params[5] == 60)
        assert(message == 456)
    end)
end)
