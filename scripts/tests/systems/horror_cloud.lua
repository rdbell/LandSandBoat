describe('Horror Cloud mob skill',function()
 it('requests Slow, forwards the status message, and returns Slow',function()
  local cloud=require('scripts/actions/mobskills/horror_cloud');local effect=xi.mobskills.mobStatusEffectMove;local call,message=nil,nil;local mob,target={},{};local skill={setMsg=function(_,value)message=value end};xi.mobskills.mobStatusEffectMove=function(...)call={...};return 456 end
  assert(cloud.onMobSkillCheck(target,mob,skill)==0 and cloud.onMobWeaponSkill(mob,target,skill,{})==xi.effect.SLOW);xi.mobskills.mobStatusEffectMove=effect;assert(call[1]==mob and call[2]==target and call[3]==xi.effect.SLOW and call[4]==5000 and call[5]==0 and call[6]==120 and message==456)
 end)
end)
