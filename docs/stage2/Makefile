# Builds the Stage 2 report. Run from this directory: make, make clean.
# Requires pandoc and XeLaTeX (TeX Live with fontspec, unicode-math, the
# TeX Gyre fonts and DejaVu Sans Mono).

SECTIONS := $(sort $(wildcard [0-9]*.md))
PANDOC_FLAGS := --from markdown-yaml_metadata_block-simple_tables-multiline_tables --shift-heading-level-by=-1 \
	--lua-filter=tex/filters.lua --include-in-header=tex/preamble.tex \
	--pdf-engine=xelatex --metadata=date:"October 30, 2026" \
	--metadata=author:"Oleh Shuptar" --metadata=author:"Rafsan Rubayet" \
	--metadata=author:"Vasili Vushau" --metadata=author:"Zuzanna Wszołek"

all: stage2_report.pdf

# One Markdown file with the whole report, in reading order.
# A blank line is put between the files: pandoc needs one before a heading.
stage2_report.md: $(SECTIONS)
	for f in $(SECTIONS); do cat $$f; echo; done > $@

# The LaTeX source, for a look at the conversion or for manual edits.
stage2_report.tex: stage2_report.md tex/preamble.tex tex/filters.lua
	pandoc $(PANDOC_FLAGS) --standalone -o $@ $<

stage2_report.pdf: stage2_report.md tex/preamble.tex tex/filters.lua
	pandoc $(PANDOC_FLAGS) -o $@ $<

clean:
	rm -f stage2_report.tex stage2_report.pdf

.PHONY: all clean
