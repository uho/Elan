#ifndef _PrimitiveP_h
#define _PrimitiveP_h

#include <X11/CoreP.h>
#include <X11/Xaw/SimpleP.h>
#include "Primitive.h"

typedef struct {
    int dummy;
} PrimitiveClassPart;

typedef struct _PrimitiveClassRec {
    CoreClassPart	core_class;
    SimpleClassPart	simple_class;
    PrimitiveClassPart	primitive_class;
} PrimitiveClassRec;

extern PrimitiveClassRec primitiveClassRec;

typedef struct {
    /* resources */
    Pixel		foreground;
    XFontStruct*	font;

    /* private state */
    GC			normalgc;
    GC			invertgc;
    int			fontheight;
    int			maxfontwidth;
} PrimitivePart;

typedef struct _PrimitiveRec {
    CorePart		core;
    SimplePart 		simple;
    PrimitivePart	primitive;
} PrimitiveRec;

#endif /* _PrimitiveP_h */
