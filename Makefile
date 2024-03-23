BUILD_DIR = ./build
SRC_DIRS = ./src

OBJS = \
	build/game.o \
	build/main.o \
	build/utils/mathutils.o \
	build/utils/imageutils.o \
	build/utils/logger.o \
	build/mesh/MeshGen.o \
	build/world/chunk.o \
	build/world/world.o \
	build/player/playerController.o \
	build/ui/debugUI.o \
	build/train/track.o \
	build/train/trackModel.o \

CFLAGS = -Wall -std=c99 -Wno-missing-braces -g -DPLATFORM_DESKTOP
INCLUDE_FLAGS = -I. -I./src -IC:/Tools/raylib/raylib/include
LIB_FLAGS = -lraylib -lopengl32 -lgdi32 -lwinmm

$(BUILD_DIR)/TrainTown: $(OBJS)
	gcc -o $@ $^ $(CFLAGS) $(INCLUDE_FLAGS) $(LIB_FLAGS)

$(BUILD_DIR)/%.o: $(SRC_DIRS)/%.c
	mkdir -p $(dir $@)
	gcc -c $(CFLAGS) $(INCLUDE_FLAGS) -o $@ $<

clean:
	rm -r $(BUILD_DIR)