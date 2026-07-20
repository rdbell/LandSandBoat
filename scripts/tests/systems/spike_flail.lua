require('scripts/actions/mobskills/spike_flail')
describe('Spike Flail mob skill', function()
    it('gates buffs/front/flying and uses threefold slashing plan', function()
        local flail = require('scripts/actions/mobskills/spike_flail')
        local move, process = xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage
        local params, damage, effects, anim, front = nil, nil, {}, 0, false
        local mob = {
            hasStatusEffect = function(_, e) return effects[e] == true end,
            getAnimationSub = function() return anim end,
            getWeaponDmg = function() return 80 end,
        }
        local target = {
            isInfront = function(_, m, angle) assert(angle == 128); return front end,
            takeDamage = function(_, ...) damage = { ... } end,
        }
        effects[xi.effect.MIGHTY_STRIKES] = true
        assert(flail.onMobSkillCheck(target, mob, {}) == 1)
        effects = {}
        front = true; assert(flail.onMobSkillCheck(target, mob, {}) == 1)
        front = false; anim = 1; assert(flail.onMobSkillCheck(target, mob, {}) == 1)
        anim = 0; assert(flail.onMobSkillCheck(target, mob, {}) == 0)
        xi.mobskills.mobPhysicalMove = function(_, _, _, _, value)
            params = value
            return { damage = 400, attackType = xi.attackType.PHYSICAL, damageType = xi.damageType.SLASHING }
        end
        xi.mobskills.processDamage = function() return false end
        assert(flail.onMobWeaponSkill(mob, target, {}, {}) == 400)
        assert(params.numHits == 3 and params.fTP[1] == 4 and damage == nil)
        xi.mobskills.processDamage = function() return true end
        flail.onMobWeaponSkill(mob, target, {}, {})
        xi.mobskills.mobPhysicalMove, xi.mobskills.processDamage = move, process
        assert(damage[1] == 400)
    end)
end)
