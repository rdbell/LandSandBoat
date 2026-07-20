describe('Floral Bouquet mob skill', function()
    it('sleeps enemies, charms allied vermin toward the mob target, and has no effect on other allies', function()
        local bouquet=require('scripts/actions/mobskills/floral_bouquet'); local status=xi.mobskills.mobStatusEffectMove
        local same, eco, mobTarget, effect, message, engaged=false,0,nil,nil,nil,nil
        local mob={getAllegiance=function() return 1 end,getTarget=function() return mobTarget end}; local target={getAllegiance=function() return same and 1 or 2 end,getEcosystem=function() return eco end,engage=function(_,id) engaged=id end}; local skill={setMsg=function(_,value) message=value end}
        xi.mobskills.mobStatusEffectMove=function(_,_,...) effect={...}; return 123 end
        assert(bouquet.onMobSkillCheck(target,mob,skill)==0 and bouquet.onMobWeaponSkill(mob,target,skill,nil)==xi.effect.SLEEP_I)
        assert(effect[1]==xi.effect.SLEEP_I and effect[2]==1 and effect[3]==0 and effect[4]==30 and message==123)
        same,eco,mobTarget,effect,message=true,xi.ecosystem.VERMIN,{getTargID=function() return 55 end},nil,nil
        assert(bouquet.onMobWeaponSkill(mob,target,skill,nil)==xi.effect.CHARM_I and engaged==55 and message==xi.msg.basic.SKILL_ENFEEB_IS)
        eco,message=0,nil; assert(bouquet.onMobWeaponSkill(mob,target,skill,nil)==0 and message==xi.msg.basic.SKILL_NO_EFFECT)
        xi.mobskills.mobStatusEffectMove=status
    end)
end)
