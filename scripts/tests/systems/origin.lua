require('scripts/actions/mobskills/origin')
describe('Origin mob skill', function()
    it('uses physical fTP 3/6/9 and drains HP/MP on process', function()
        local skill = require('scripts/actions/mobskills/origin')
        local params, hp, mpDel, mpAdd = nil, 0, 0, 0
        local origP, origD = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        xi.mobskills.mobPhysicalMove = function(mob, target, sk, action, p)
            params = p
            return { damage = 200, attackType = p.attackType, damageType = p.damageType }
        end
        xi.mobskills.processDamage = function() return true end
        local mob = {
            getWeaponDmg = function() return 50 end,
            addHP = function(_, v) hp = v end,
            addMP = function(_, v) mpAdd = v end,
        }
        local target = {
            getHP = function() return 150 end,
            getMP = function() return 80 end,
            isUndead = function() return false end,
            takeDamage = function() end,
            delMP = function(_, v) mpDel = v end,
        }
        assert(skill.onMobSkillCheck(target, mob, {}) == 0)
        assert(skill.onMobWeaponSkill(mob, target, {}, {}) == 200)
        assert(params.fTP[1] == 3.0 and params.fTP[2] == 6.0 and params.fTP[3] == 9.0)
        assert(hp == 150 and mpDel == 80 and mpAdd == 80)
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = origP, origD
    end)
end)
