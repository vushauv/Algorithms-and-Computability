-- Pandoc Lua filter for the Stage 2 report.

-- The Markdown files are separated by horizontal rules (---). They have no
-- meaning in the PDF, so drop them.
function HorizontalRule(el)
  return {}
end

-- Pseudocode lines longer than LIMIT characters would run out of the frame.
-- A trailing "// comment" moves to its own line above the code, indented like
-- the code; a line that is still too long is wrapped at a space, and the
-- continuation is indented further. The text of the pseudocode is unchanged.
local LIMIT = 88

local function wrap(line, out)
  while utf8.len(line) > LIMIT do
    local indent = line:match("^(%s*)")
    local cut = nil
    local pos = 0
    for p, _ in utf8.codes(line) do
      pos = pos + 1
      if pos > LIMIT then break end
      if line:sub(p, p) == " " and pos > #indent then cut = p end
    end
    if not cut then break end
    table.insert(out, line:sub(1, cut - 1))
    line = indent .. "        " .. line:sub(cut + 1)
  end
  table.insert(out, line)
end

function CodeBlock(el)
  local out = {}
  for line in (el.text .. "\n"):gmatch("(.-)\n") do
    if utf8.len(line) > LIMIT then
      local code, comment = line:match("^(.-)%s+(//.*)$")
      if code then
        local indent = code:match("^(%s*)")
        table.insert(out, indent .. comment)
        wrap(code, out)
      else
        wrap(line, out)
      end
    else
      table.insert(out, line)
    end
  end
  el.text = table.concat(out, "\n")
  return el
end
