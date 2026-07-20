describe('Gravity Field mob skill',function()
 it('forwards random-duration Slow message and returns Slow',function()
  local field=require('scripts/actions/mobskills/gravity_field');local status,random=xi.mobskills.mobStatusEffectMove,math.random;local args,message=nil,nil;local skill={setMsg=function(_,v)message=v end};xi.mobskills.mobStatusEffectMove=function(...)args={...};return 456 end;math.random=function(min,max)assert(min==240 and max==420);return 300 end
  assert(field.onMobSkillCheck({}, {}, skill)==0 and field.onMobWeaponSkill({}, {}, skill,{})==xi.effect.SLOW);xi.mobskills.mobStatusEffectMove,math.random=status,random;assert(message==456 and args[3]==xi.effect.SLOW and args[4]==4500 and args[5]==0 and args[6]==300)
 end)
end)
