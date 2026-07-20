describe('Heat Barrier mob skill',function()
 it('applies Blaze Spikes then Enfire from main level and returns Blaze Spikes',function()
  local barrier=require('scripts/actions/mobskills/heat_barrier');local buff=xi.mobskills.mobBuffMove;local calls,message={},nil;local mob={getMainLvl=function()return 100 end};local target={};local skill={setMsg=function(_,value)message=value end};xi.mobskills.mobBuffMove=function(...)calls[#calls+1]={...};return 456 end
  assert(barrier.onMobSkillCheck(target,mob,skill)==0 and barrier.onMobWeaponSkill(mob,target,skill,{})==xi.effect.BLAZE_SPIKES);xi.mobskills.mobBuffMove=buff;assert(#calls==2 and calls[1][1]==mob and calls[1][2]==xi.effect.BLAZE_SPIKES and calls[1][3]==80 and calls[1][4]==0 and calls[1][5]==180 and calls[2][1]==mob and calls[2][2]==xi.effect.ENFIRE and calls[2][3]==40 and calls[2][4]==0 and calls[2][5]==300 and message==456)
 end)
end)
