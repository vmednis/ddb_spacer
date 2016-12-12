OUTPUT_GTK3 :=ddb_spacer_gtk3.so
OUTPUT_GTK2 :=ddb_spacer_gtk2.so

#You might have to need this
CFLAGS_DEADBEEF :=-I/opt/deadbeef/include
CFLAGS_GTK3 :=`pkg-config --cflags gtk+-3.0`
CFLAGS_GTK2 :=`pkg-config --cflags gtk+-2.0`

LDFLAGS_GTK3 :=`pkg-config --libs gtk+-3.0`
LDFLAGS_GTK2 :=`pkg-config --libs gtk+-2.0`

CC :=gcc
CFLAGS +=-Wall -g -fPIC -std=c99 -D_GNU_SOURCE $(CFLAGS_DEADBEEF)
LDFLAGS +=-shared -g

OUTPUT_DIR :=bin

OUTPUT_DIR_GTK3 :=gtk3
OUTPUT_DIR_GTK2 :=gtk2

all: clean gtk2 gtk3

gtk3: prepare_dir_gtk3 $(OUTPUT_DIR)/$(OUTPUT_DIR_GTK3)/spacer.o $(OUTPUT_DIR)/$(OUTPUT_DIR_GTK3)/$(OUTPUT_GTK3)

gtk2: prepare_dir_gtk2 $(OUTPUT_DIR)/$(OUTPUT_DIR_GTK2)/spacer.o $(OUTPUT_DIR)/$(OUTPUT_DIR_GTK2)/$(OUTPUT_GTK2)

prepare_dir_gtk3:
	@echo "Creating dirs for gtk3 version"
	@mkdir -p $(OUTPUT_DIR)/$(OUTPUT_DIR_GTK3)

prepare_dir_gtk2:
	@echo "Creating dirs for gtk2 version"
	@mkdir -p $(OUTPUT_DIR)/$(OUTPUT_DIR_GTK2)

#Linking
$(OUTPUT_DIR)/$(OUTPUT_DIR_GTK3)/$(OUTPUT_GTK3): $(OUTPUT_DIR)/$(OUTPUT_DIR_GTK3)/spacer.o
	@echo "Linking gtk3 version"
	@$(CC) $(LDFLAGS) $< $(LDFLAGS_GTK3) -o $@

$(OUTPUT_DIR)/$(OUTPUT_DIR_GTK2)/$(OUTPUT_GTK2): $(OUTPUT_DIR)/$(OUTPUT_DIR_GTK2)/spacer.o
	@echo "Linking gtk2 version"
	@$(CC) $(LDFLAGS) $< $(LDFLAGS_GTK2) -o $@

#Compiling
$(OUTPUT_DIR)/$(OUTPUT_DIR_GTK3)/spacer.o: spacer.c
	@echo "Compiling gtk3 version"
	@$(CC) $(CFLAGS) $(CFLAGS_DEADBEEF) $(CFLAGS_GTK3) $< -c -o $@

$(OUTPUT_DIR)/$(OUTPUT_DIR_GTK2)/spacer.o: spacer.c
	@echo "Compiling gtk2 version"
	@$(CC) $(CFLAGS) $(CFLAGS_DEADBEEF) $(CFLAGS_GTK2) $< -c -o $@

clean:
	@rm -rf $(OUTPUT_DIR)
