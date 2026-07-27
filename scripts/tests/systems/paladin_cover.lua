require('scripts/globals/job_utils/paladin')

describe('Paladin Cover', function()
    it('adds the calculated self effect and records the covered target', function()
        local effect
        local localVar
        local message
        local player = {
            getStat = function(_, mod)
                if mod == xi.mod.VIT then return 40 end
                assert(mod == xi.mod.MND)
                return 40
            end,
            getMerit = function(_, merit)
                assert(merit == xi.merit.COVER_EFFECT_LENGTH)
                return 5
            end,
            getMod = function(_, mod)
                if mod == xi.mod.COVER_DURATION then return 2 end
                assert(mod == xi.mod.COVER_TO_MP)
                return 9
            end,
            getJobPointLevel = function(_, jp)
                assert(jp == xi.jp.COVER_DURATION)
                return 3
            end,
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
            setLocalVar = function(_, name, value) localVar = { name = name, value = value } end,
        }
        local target = {
            getStat = function(_, mod) assert(mod == xi.mod.VIT); return 20 end,
            getID = function() return 1234 end,
        }
        local ability = { setMsg = function(_, value) message = value end }

        local result = xi.job_utils.paladin.useCover(player, target, ability)

        assert(result == nil and effect.id == xi.effect.COVER)
        assert(effect.values.power == 9 and effect.values.duration == 35 and effect.values.origin == player)
        assert(localVar.name == 'COVER_ABILITY_TARGET' and localVar.value == 1234 and message == xi.msg.basic.COVER_SUCCESS)
    end)
end)
