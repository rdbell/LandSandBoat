require('scripts/globals/job_utils/ranger')

describe('Ranger Shadowbind', function()
    local function useShadowbind(params)
        local effect
        local message
        local animation
        local ammoRemoved = 0
        local oldRandom = math.random
        local oldShouldUseAmmo = xi.combat.ranged.shouldUseAmmo
        math.random = function(low, high)
            assert(low == 0 and high == 99)
            return params.roll or 0
        end
        xi.combat.ranged.shouldUseAmmo = function() return params.shouldUseAmmo end
        local player = {
            getWeaponSkillType = function() return params.rangedSkill or xi.skill.ARCHERY end,
            getMod = function(_, mod)
                assert(mod == xi.mod.SHADOW_BIND_EXT)
                return params.extension or 0
            end,
            getJobPointLevel = function(_, jp)
                assert(jp == xi.jp.SHADOWBIND_DURATION)
                return params.jpDuration or 0
            end,
            removeAmmo = function(_, count) ammoRemoved = ammoRemoved + count end,
        }
        local target = {
            getID = function() return 9 end,
            getMod = function(_, mod)
                assert(mod == xi.mod.BIND_MEVA)
                return params.bindMeva or 0
            end,
            hasStatusEffect = function() return params.alreadyBound or false end,
            addStatusEffect = function(_, id, values) effect = { id = id, values = values } end,
        }
        local ability = { setMsg = function(_, value) message = value end }
        local action = {
            getAnimation = function() return 10 end,
            setAnimation = function(_, id, value)
                assert(id == 9)
                animation = value
            end,
        }

        local result = xi.job_utils.ranger.useShadowbind(player, target, ability, action)
        math.random = oldRandom
        xi.combat.ranged.shouldUseAmmo = oldShouldUseAmmo
        return { result = result, effect = effect, message = message, animation = animation, ammoRemoved = ammoRemoved }
    end

    it('adds Bind with duration, Marksmanship animation, and conditional ammo consumption', function()
        local result = useShadowbind({ rangedSkill = xi.skill.MARKSMANSHIP, extension = 5, jpDuration = 10, bindMeva = 50, roll = 50, shouldUseAmmo = true })
        assert(result.result == xi.effect.BIND and result.message == xi.msg.basic.IS_EFFECT)
        assert(result.animation == 11 and result.ammoRemoved == 1)
        assert(result.effect.id == xi.effect.BIND and result.effect.values.duration == 45)
    end)

    it('reports a miss without applying Bind or consuming ammo when already bound', function()
        local result = useShadowbind({ alreadyBound = true, shouldUseAmmo = false })
        assert(result.result == xi.effect.BIND and result.message == xi.msg.basic.JA_MISS)
        assert(result.animation == nil and result.effect == nil and result.ammoRemoved == 0)
    end)
end)
