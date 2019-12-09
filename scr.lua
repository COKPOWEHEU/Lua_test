print("Script start")
z = 1

function func(x, y, s)
  print("script(",s,x,y,")")
  return "AAA", z
end

function func2()
  z = z+1
  print("func2", z)
end
