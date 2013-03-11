srcdir=src/
bindir=lib/

bin_PROGRAMS=cw
cw_SOURCES= \
io/file.c \
io/gl/framebuffer.c \
io/gl/gl.c \
io/gl/mesh.c \
io/gl/shader.c \
io/gl/texture.c \
io/tga.c \
util/algo/bits.c \
util/algo/search.c \
util/algo/sort.c \
util/hash.c \
util/math/angles.c \
util/math/convert.c \
util/math/geom/ball.c \
util/math/geom/line.c \
util/math/geom/spline.c \
util/math/geom/tri.c \
util/math/matrix.c \
util/math/raster.c \
util/math/scalar.c \
util/math/vec.c \
util/noise.c \
util/random.c \
util/script/luaapi.c \
util/str.c \
util/struct/iterator.c \
util/struct/kdtree.c \
util/struct/list.c \
util/struct/octree.c \
util/struct/rtree.c \
util/struct/varray.c \
util/time.c \
#util/struct/voctree.c
#util/struct/tmesh.c \

AUTOMAKE_OPTIONS = foreign
CFLAGS= -std=gnu99 -Wall -Winit-self -pedantic 
LDFLAGS= -lGLEW -lGL -lm -llua

srcobjs:=$(sort $(foreach f,$(cw_SOURCES),$(notdir $(f:.c=.o))))
srcdirs:=$(sort $(foreach f,$(cw_SOURCES),$(srcdir)/$(dir $(f))))

vpath %.c $(srcdirs)
vpath %.o $(buildir)
vpath %.d $(buildir)


all: $(srcobjs)
	$(CC) $(CFLAGS) $(LDFLAGS) build/*.o -o $(bindir)/obj

%.o: %.c
	$(CC) -c $(CFLAGS) $< -o build/$@ -Isrc

