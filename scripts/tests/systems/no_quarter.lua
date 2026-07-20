require('scripts/actions/mobskills/no_quarter')

describe('No Quarter mob skill', function()
    it('uses its three-hit Slashing plan, damages only after processing, then clears physical DT and anim-sub', function()
        local quarter = require('scripts/actions/mobskills/no_quarter')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage, dmgphys, daybreak, animSub = nil, nil, nil, nil, nil
        local now = 123456
        stub('GetSystemTime', function() return now end)
        local mob = {
            getWeaponDmg = function() return 77 end,
            setMod = function(_, mod, value)
                if mod == xi.mod.DMGPHYS then
                    dmgphys = value
                end
            end,
            setLocalVar = function(_, name, value)
                if name == 'DaybreakEndTime' then
                    daybreak = value
                end
            end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local skill = {
            setFinalAnimationSub = function(_, value) animSub = value end,
        }

        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 123, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end

        assert(quarter.onMobSkillCheck(target, mob, skill) == 0 and quarter.onMobWeaponSkill(mob, target, skill, {}) == 123)
        assert(params.baseDamage == 77 and params.numHits == 3 and params.fTP[1] == 0.35 and params.fTP[2] == 0.35 and params.fTP[3] == 0.35)
        assert(params.attackType == xi.attackType.PHYSICAL and params.damageType == xi.damageType.SLASHING)
        assert(params.shadowBehavior == xi.mobskills.shadowBehavior.NUMSHADOWS_3 and damage == nil)
        assert(dmgphys == 0 and daybreak == now and animSub == 0)

        xi.mobskills.processDamage = function() return true end
        dmgphys, daybreak, animSub = nil, nil, nil
        quarter.onMobWeaponSkill(mob, target, skill, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process

        assert(damage[1] == 123 and damage[2] == mob and damage[3] == xi.attackType.PHYSICAL and damage[4] == xi.damageType.SLASHING)
        assert(dmgphys == 0 and daybreak == now and animSub == 0)
    end)
end)
