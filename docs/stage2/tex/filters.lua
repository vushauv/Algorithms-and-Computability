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

-- Tables: pandoc's longtable has rules only at the top, under the header and
-- at the bottom. The report wants a rule between the rows as well, so the
-- table is written here. Columns with a width from the pipe table get a
-- paragraph column of that share of the line width, the others are left
-- aligned and as wide as their content.
local function cell_latex(cell)
  return (pandoc.write(pandoc.Pandoc(cell.contents), "latex"):gsub("%s+$", ""))
end

local function row_latex(row)
  local cells = {}
  for _, cell in ipairs(row.cells) do
    table.insert(cells, cell_latex(cell))
  end
  return table.concat(cells, " & ")
end

function Table(el)
  local ncols = #el.colspecs
  local spec = {}
  for _, cs in ipairs(el.colspecs) do
    local width = cs[2]
    if width == nil or width == "ColWidthDefault" then
      table.insert(spec, "l")
    else
      table.insert(spec, string.format(
        ">{\\raggedright\\arraybackslash}p{(\\linewidth - %d\\tabcolsep) * \\real{%.4f}}",
        2 * ncols, width))
    end
  end
  local out = { "\\begin{longtable}[]{@{}" .. table.concat(spec) .. "@{}}", "\\toprule" }
  for _, row in ipairs(el.head.rows) do
    table.insert(out, row_latex(row) .. " \\\\")
  end
  table.insert(out, "\\midrule")
  table.insert(out, "\\endhead")
  local rows = {}
  for _, body in ipairs(el.bodies) do
    for _, row in ipairs(body.body) do table.insert(rows, row) end
  end
  for i, row in ipairs(rows) do
    table.insert(out, row_latex(row) .. " \\\\")
    table.insert(out, i < #rows and "\\midrule" or "\\bottomrule")
  end
  table.insert(out, "\\end{longtable}")
  return pandoc.RawBlock("latex", table.concat(out, "\n"))
end

-- The abstract is written in 00_title.md as the section "Abstract", so that
-- the Markdown stays readable. Here it becomes the abstract metadata, which
-- the LaTeX template sets on the title page. The table of contents and the
-- body then each start on a new page.
function Pandoc(doc)
  local blocks = doc.blocks
  local first = blocks[1]
  if first and first.t == "Header" and first.level == 1
     and pandoc.utils.stringify(first) == "Abstract" then
    local abstract = {}
    local i = 2
    while blocks[i] and not (blocks[i].t == "Header" and blocks[i].level == 1) do
      table.insert(abstract, blocks[i])
      i = i + 1
    end
    doc.meta.abstract = pandoc.MetaBlocks(abstract)
    doc.meta["include-before"] = pandoc.MetaBlocks({ pandoc.RawBlock("latex", "\\clearpage") })
    local rest = { pandoc.RawBlock("latex", "\\clearpage") }
    for j = i, #blocks do table.insert(rest, blocks[j]) end
    doc.blocks = rest
  end
  return doc
end
