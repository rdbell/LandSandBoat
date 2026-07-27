require('scripts/globals/job_utils/ranger')

describe('Ranger Eagle Eye Shot', function()
    local function useEagleEyeShot(params)
        local addedMod
        local animation
        local message
        local request
        local oldDoRangedWeaponskill = xi.weaponskills.doRangedWeaponskill
        xi.weaponskills.doRangedWeaponskill = function(_, _, wsid, wsParams, tp, _, isEagleEyeShot)
            assert(wsid == 0 and tp == 1000 and isEagleEyeShot)
            request = wsParams
            return params.damage or 123, 0, params.tpHits or 0, params.extraHits or 0
        end
        local player = {
            getWeaponSkillType = function(_, slot)
                assert(slot == xi.slot.RANGED)
                return params.rangedSkill or xi.skill.ARCHERY
            end,
            getJobPointLevel = function(_, jp)
                assert(jp == xi.jp.EAGLE_EYE_SHOT_EFFECT)
                return params.jp or 0
            end,
            addMod = function(_, mod, value)
                assert(mod == xi.mod.ALL_WSDMG_ALL_HITS)
                addedMod = value
            end,
        }
        local target = { getID = function() return 9 end }
        local action = {
            getAnimation = function() return 10 end,
            setAnimation = function(_, id, value)
                assert(id == 9)
                animation = value
            end,
            messageID = function(_, id, value)
                assert(id == 9)
                message = value
            end,
        }

        local damage = xi.job_utils.ranger.useEagleEyeShot(player, target, {}, action)
        xi.weaponskills.doRangedWeaponskill = oldDoRangedWeaponskill
        return { damage = damage, mod = addedMod, animation = animation, message = message, request = request }
    end

    it('builds its fixed ranged weaponskill request and applies its JP damage bonus', function()
        local result = useEagleEyeShot({ rangedSkill = xi.skill.MARKSMANSHIP, jp = 4, tpHits = 1 })

        assert(result.damage == 123 and result.mod == 12 and result.animation == 11)
        assert(result.message == xi.msg.basic.JA_DAMAGE)
        assert(result.request.numHits == 1 and result.request.ignoreShadows)
        assert(result.request.ftpMod[1] == 5.0 and result.request.ftpMod[2] == 5.0 and result.request.ftpMod[3] == 5.0)
        assert(result.request.critVaries[1] == 0.0 and result.request.critVaries[2] == 0.0 and result.request.critVaries[3] == 0.0)
        assert(result.request.str_wsc == 0 and result.request.dex_wsc == 0 and result.request.vit_wsc == 0)
        assert(result.request.agi_wsc == 0 and result.request.int_wsc == 0 and result.request.mnd_wsc == 0 and result.request.chr_wsc == 0)
        assert(result.request.enmityMult == 0.5)
    end)

    it('uses the miss message only when both hit counts are zero', function()
        local result = useEagleEyeShot({ extraHits = 1 })
        assert(result.message == xi.msg.basic.JA_DAMAGE)

        result = useEagleEyeShot({})
        assert(result.mod == 0 and result.animation == nil and result.message == xi.msg.basic.JA_MISS_2)
    end)
end)
