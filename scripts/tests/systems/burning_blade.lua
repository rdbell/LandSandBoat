require('scripts/actions/mobskills/burning_blade')
describe('Burning Blade mob skill', function()
    it('emits READIES_WS and uses Fire magical plan', function()
        local skill = require('scripts/actions/mobskills/burning_blade')
        local magicalMove = xi.mobskills.mobMagicalMove
        local processDamage = xi.mobskills.processDamage
        local params, damage, msg = nil, nil, nil
        local mob = {
            getMainLvl = function() return 50 end,
            messageBasic = function(_, m, p2, p3) msg = { m, p2, p3 } end,
        }
        local target = { takeDamage = function(_, v, s, a, d) damage = { v, s, a, d } end }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(msg[1] == xi.msg.basic.READIES_WS and msg[3] == xi.weaponskill.BURNING_BLADE)
        xi.mobskills.mobMagicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 100, attackType = xi.attackType.MAGICAL, damageType = xi.damageType.FIRE }
        end
        xi.mobskills.processDamage = function() return false end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(params.baseDamage == 52 and params.fTP[2] == 2.0 and params.fTP[3] == 2.5)
        assert(params.dStatMultiplier == 1 and params.dStatAttackerMod == xi.mod.INT)
        assert(damage == nil)
        xi.mobskills.processDamage = function() return true end
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 100)
        assert(damage[1] == 100)
        xi.mobskills.mobMagicalMove = magicalMove
        xi.mobskills.processDamage = processDamage
    end)
end)
