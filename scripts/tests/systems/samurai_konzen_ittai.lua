require('scripts/globals/job_utils/samurai')

describe('Samurai Konzen Ittai host', function()
    it('applies Chainbound and publishes the hit action data', function()
        local applied
        local animation
        local info
        local player = {
            getWeaponSkillType = function()
                return xi.skill.GREAT_KATANA
            end,
        }
        local target = {
            getID = function()
                return 42
            end,
            hasStatusEffect = function()
                return false
            end,
            addStatusEffect = function(_, effect, params)
                applied = { effect = effect, params = params }
            end,
        }
        local action = {
            setAnimation = function(_, targetID, value)
                animation = { targetID = targetID, value = value }
            end,
            info = function(_, targetID, value)
                info = { targetID = targetID, value = value }
            end,
        }

        assert(xi.job_utils.samurai.useKonzenIttai(player, target, {}, action) == 3)
        assert(applied.effect == xi.effect.CHAINBOUND)
        assert(applied.params.power == 2 and applied.params.duration == 10 and applied.params.subPower == 1 and applied.params.icon == 0)
        assert(applied.params.origin == player)
        assert(animation.targetID == 42 and animation.value == 44)
        assert(info.targetID == 42 and info.value == 5)
    end)

    it('sets no effect while still publishing the miss action data', function()
        local msg
        local animation
        local info
        local player = {
            getWeaponSkillType = function()
                return xi.skill.POLEARM
            end,
        }
        local target = {
            getID = function()
                return 42
            end,
            hasStatusEffect = function(_, effect)
                return effect == xi.effect.CHAINBOUND
            end,
            addStatusEffect = function()
                error('Chainbound must not be replaced')
            end,
        }
        local ability = {
            setMsg = function(_, value)
                msg = value
            end,
        }
        local action = {
            setAnimation = function(_, _, value)
                animation = value
            end,
            info = function(_, _, value)
                info = value
            end,
        }

        assert(xi.job_utils.samurai.useKonzenIttai(player, target, ability, action) == 0)
        assert(msg == xi.msg.basic.JA_NO_EFFECT and animation == 42 and info == 1)
    end)
end)
