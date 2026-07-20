describe('Howling mob skill',function()
 it('requests Paralysis, forwards the status message, and returns Paralysis',function()
  local howl=require('scripts/actions/mobskills/howling');local effect=xi.mobskills.mobStatusEffectMove;local call,message=nil,nil;local mob,target={},{};local skill={setMsg=function(_,value)message=value end};xi.mobskills.mobStatusEffectMove=function(...)call={...};return 456 end
  assert(howl.onMobSkillCheck(target,mob,skill)==0 and howl.onMobWeaponSkill(mob,target,skill,{})==xi.effect.PARALYSIS);xi.mobskills.mobStatusEffectMove=effect;assert(call[1]==mob and call[2]==target and call[3]==xi.effect.PARALYSIS and call[4]==35 and call[5]==0 and call[6]==60 and message==456)
 end)
end)
