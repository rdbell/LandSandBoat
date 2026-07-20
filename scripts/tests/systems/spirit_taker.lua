require('scripts/actions/mobskills/spirit_taker')
describe('Spirit Taker mob skill', function()
    it('uses blunt physical plan and converts damage to MP after processing', function()
        local taker = require('scripts/actions/mobskills/spirit_taker')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage, mp = nil, nil, nil
        local mob = {
            getWeaponDmg = function() return 40 end,
            addMP = function(_, v) mp = v end,
        }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 75, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.BLUNT }
        end
        xi.mobskills.processDamage = function() return false end
        assert(taker.onMobSkillCheck(target, mob, {}) == 0 and taker.onMobWeaponSkill(mob, target, {}, {}) == 75)
        assert(params.fTP[1] == 1.0 and params.fTP[2] == 1.5 and params.fTP[3] == 2.0 and damage == nil and mp == nil)
        xi.mobskills.processDamage = function() return true end
        taker.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 75 and mp == 75)
    end)
end)
