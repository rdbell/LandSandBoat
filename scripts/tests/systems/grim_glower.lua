describe('Grim Glower mob skill',function()
 it('suppresses its message and returns zero',function()
  local glower=require('scripts/actions/mobskills/grim_glower');local message=nil;local skill={setMsg=function(_,value)message=value end}
  assert(glower.onMobSkillCheck({}, {}, skill)==0 and glower.onMobWeaponSkill({}, {}, skill,{})==0 and message==xi.msg.basic.NONE)
 end)
end)
