require('scripts/actions/mobskills/string_clipper')
describe('String Clipper mob skill', function()
    it('uses twofold slashing plan with attack multiplier', function()
        local clip = require('scripts/actions/mobskills/string_clipper')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage = nil, nil
        local mob = { getWeaponDmg = function() return 30 end }
        local target = { takeDamage = function(_, ...) damage = { ... } end }
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 60, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(clip.onMobSkillCheck(target, mob, {}) == 0 and clip.onMobWeaponSkill(mob, target, {}, {}) == 60)
        assert(params.numHits == 2 and params.fTP[1] == 2 and params.attackMultiplier[1] == 1.25 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        clip.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 60)
    end)
end)
