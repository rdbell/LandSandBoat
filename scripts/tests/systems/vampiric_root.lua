require('scripts/actions/mobskills/vampiric_root')
describe('Vampiric Root mob skill', function()
    it('requires dispelable buffs, drains HP, and steals all positive effects after processing', function()
        local skill = require('scripts/actions/mobskills/vampiric_root')
        local move, process, drain = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove
        local params, message, steals, n = nil, nil, 0, 0
        local sk = { setMsg = function(_, v) message = v end }
        local mob = {
            getWeaponDmg = function() return 80 end,
            stealStatusEffect = function()
                steals = steals + 1
                if steals <= 2 then return 10 end
                return 0
            end,
        }
        local target = {
            countEffectWithFlag = function(_, f)
                assert(f == xi.effectFlag.DISPELABLE)
                return n
            end,
        }
        n = 0; assert(skill.onMobSkillCheck(target, mob, sk) == 1)
        n = 2; assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.PIERCING }
        end
        xi.mobskills.mobDrainMove = function() return 123 end
        xi.mobskills.processDamage = function() return false end
        steals = 0
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 100)
        assert(params.skipPDIF and params.fTP[1] == 2.0 and message == nil and steals == 0)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, sk, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobDrainMove = move, process, drain
        assert(message == 123 and steals == 3) -- 2 successes + 1 terminating 0
    end)
end)
