require('scripts/actions/mobskills/uriel_blade')
describe('Uriel Blade mob skill', function()
    it('readies WS 238, applies fTPBonus under 1000 TP, and flashes after processing', function()
        local skill = require('scripts/actions/mobskills/uriel_blade')
        local move, process, status = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove
        local params, damage, statusParams, ready = nil, nil, nil, nil
        local mob = {
            getWeaponDmg = function() return 80 end,
            messageBasic = function(_, msg, p0, p1) ready = { msg, p0, p1 } end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        local sk = { getTP = function() return 500 end }
        assert(skill.onMobSkillCheck(target, mob, sk) == 0)
        assert(ready[1] == xi.msg.basic.READIES_WS and ready[3] == xi.weaponskill.URIEL_BLADE)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.mobStatusEffectMove = function(...) statusParams = { ... } end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, sk, {}) == 100)
        assert(params.fTP[1] == 4.5 and params.fTPBonus == 1000 and params.str_wSC == 0.32 and statusParams == nil)
        xi.mobskills.processDamage = function() return true end
        skill.onMobWeaponSkill(mob, target, sk, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage, xi.mobskills.mobStatusEffectMove = move, process, status
        assert(damage[1] == 100 and statusParams[3] == xi.effect.FLASH and statusParams[6] == 15)
    end)
end)
