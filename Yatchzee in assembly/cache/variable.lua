
threshold = 1000
orig = 100000
i = orig
function cache_access(is_ins, is_write, address)
  --print('access: ' .. tostring(is_ins) .. ' : ' .. tostring(is_write) .. ' : ' .. tostring(address))

  if i <= 0 then
    i = orig
  end

  if i > threshold then
    i = i - 100
  else
    i = i - 1
  end

  return i
end
