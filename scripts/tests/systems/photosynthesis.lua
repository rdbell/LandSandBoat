require('scripts/actions/mobskills/photosynthesis')

describe('Photosynthesis mob skill', function()
    it('allows use only daytime without Regen and applies level-scaled Regen', function()
        local photo = require('scripts/actions/mobskills/photosynthesis')
        local buff = xi.mobskills.mobBuffMove
        local hour, hasRegen, params, message = 12, false, nil, nil
        stub('VanadielHour', function() return hour end)
        local mob = {
            hasStatusEffect = function(_, effect)
                assert(effect == xi.effect.REGEN)
                return hasRegen
            end,
            getMainLvl = function() return 55 end,
        }
        local skill = { setMsg = function(_, value) message = value end }
        xi.mobskills.mobBuffMove = function(...)
            params = { ... }
            return 456
        end

        hour, hasRegen = 5, false
        assert(photo.onMobSkillCheck({}, mob, skill) == 1)
        hour, hasRegen = 6, false
        assert(photo.onMobSkillCheck({}, mob, skill) == 0)
        hour, hasRegen = 18, false
        assert(photo.onMobSkillCheck({}, mob, skill) == 0)
        hour, hasRegen = 19, false
        assert(photo.onMobSkillCheck({}, mob, skill) == 1)
        hour, hasRegen = 12, true
        assert(photo.onMobSkillCheck({}, mob, skill) == 1)

        hour, hasRegen = 12, false
        assert(photo.onMobWeaponSkill(mob, {}, skill, {}) == xi.effect.REGEN)
        xi.mobskills.mobBuffMove = buff
        assert(params[1] == mob and params[2] == xi.effect.REGEN and params[3] == 5 and params[4] == 0 and params[5] == 120)
        assert(message == 456)
    end)
end)
