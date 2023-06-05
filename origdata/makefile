# Set shell to bash instead of sh allow for brace expansion
SHELL=/bin/bash

DEST=../data
SRC=.

.PHONY: default
default: all ;

SPRITES=bridge tiles2 Duck1 Duck_back Duck_front Capy_01 Capy_02 Capy_03 Chicken_back Chicken_front \
	Chicken_side chart bubble1 frame FoleFront1 Pelican_01 pickups bullets mapicons Boterham
SHEET_FILES=$(patsubst %,$(DEST)/%.png,$(SPRITES))

$(SHEET_FILES): $(DEST)/%.png: $(SRC)/%.ase
	aseprite -b $< --sheet $@

.PHONY: all
all: $(SHEET_FILES)
