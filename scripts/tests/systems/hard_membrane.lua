describe('Hard Membrane mob skill',function()
 it('rejects an existing Evasion Boost and forwards its scaled buff message',function()
  local membrane=require('scripts/actions/mobskills/hard_membrane');local duration,buff=xi.mobskills.calculateDuration,xi.mobskills.mobBuffMove;local call,message=nil,nil;local active=false;local mob={hasStatusEffect=function(_,effect)return active and effect==xi.effect.EVASION_BOOST end};local skill={getTP=function()return 1500 end,setMsg=function(_,value)message=value end};xi.mobskills.calculateDuration=function(tp,min,max)assert(tp==1500 and min==180 and max==300);return 270 end;xi.mobskills.mobBuffMove=function(...)call={...};return 456 end
  assert(membrane.onMobSkillCheck({},mob,skill)==0 and membrane.onMobWeaponSkill(mob,{},skill,{})==xi.effect.EVASION_BOOST);active=true;assert(membrane.onMobSkillCheck({},mob,skill)==1);xi.mobskills.calculateDuration,xi.mobskills.mobBuffMove=duration,buff;assert(message==456 and call[2]==xi.effect.EVASION_BOOST and call[3]==25 and call[4]==0 and call[5]==270)
 end)
end)
