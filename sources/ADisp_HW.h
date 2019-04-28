#ifndef _ADISP_HW_H
#define _ADISP_HW_H

#ifdef WARPUP
#pragma pack(push,2)
#endif
#include <Warp3D/Warp3D.h>
#include <exec/types.h>
#ifdef WARPUP
#include <Warp3D/Warp3D_protos.h>
#pragma pack(pop)
#endif

BOOL HWDriver_init(struct amigamesa_context *c,struct TagItem *tagList);

#define CHECK_SUCCESS           0
#define CHECK_FAILURE           -1      /* hw acceleration disabled */
#define CHECK_INACTIVE          1       /* hw not ready */
#define CHECK_NOZMEM            2       /* AllocZBuffer failed */
#define CHECK_NOHW              3       /* no acceleration possible */
#define CHECK_NODRAWBUF         4       /* draw buffer disabled */
#define CHECK_TEXMODE           5       /* unsupported texturing */
#define CHECK_NOIMAGE           6       /* no texture image */
#define CHECK_NOTEXMAPPING      7       /* texture mapping unsupported */
#define CHECK_NOREPEAT_LIN      8       /* no GL_REPEAT in linear mode */
#define CHECK_NOCLAMP_LIN       9       /* no GL_CLAMP in linear mode */
#define CHECK_TEXOV_LIN         10      /* too large texture in linear mode */
#define CHECK_NOPERSP           11      /* no perspective correction */
#define CHECK_NOREPEAT_P        12      /* no GL_REPEAT in persp. mode */
#define CHECK_NOCLAMP_P         13      /* no GL_CLAMP in persp. mode */
#define CHECK_TEXOV_P           14      /* too large texture in persp. mode */
#define CHECK_NOBILINEAR        15      /* bilinear filtering unsupported */
#define CHECK_NOMIPMAP          16      /* mipmapping unsupported */
#define CHECK_NODEPTHFILTER     17      /* depth filter unsupported */
#define CHECK_NOREPLACE         18      /* GL_REPLACE unsupported */
#define CHECK_NODECAL           19      /* GL_DECAL unsupported */
#define CHECK_NOMODULATE        20      /* GL_MODULATE unsupported */
#define CHECK_NOBLEND           21      /* GL_BLEND unsupported */
#define CHECK_NORECTTEX         22      /* rectangular textures unsupported */
#define CHECK_NOTEXOBJ          23      /* no current texture defined */
#define CHECK_NOTEXIMAGE        24      /* no texture image */
#define CHECK_FORMATERR         25      /* texformat unsupported */
#define CHECK_FORMATERR2        26      /* mipmaps with different texformats */
#define CHECK_NOW3DTEX          27      /* no W3D texture object defined */
#define CHECK_NOSMOOTH          28      /* smooth shading unsupported */
#define CHECK_NOFLAT            29      /* flat shading unsupported */
#define CHECK_NOZBUFFER         30      /* Z buffering unsupported */
#define CHECK_NOZMODES          31      /* Z compare modes unsupported */
#define CHECK_NOZUPDATE         32      /* Z buffer update unsupported */
#define CHECK_NOALPHATEST       33      /* Alpha test unsupported */
#define CHECK_NOAMODES          34      /* alpha test modes unsupported */
#define CHECK_NOBLENDING        35      /* blending unsupported */
#define CHECK_NOBLENDFACTORS    36      /* blending factors unsupported */
#define CHECK_NOFOGGING         37      /* fogging unsupported */
#define CHECK_FOGLINEAR         38      /* linear fog unsupported */
#define CHECK_FOGEXP            39      /* exp fog unsupported */
#define CHECK_FOGEXP2           40      /* exp^2 fog unsupported */
#define CHECK_AAPOINT           41      /* point antialising unsupported */
#define CHECK_AALINE            42      /* line antialising unsupported */
#define CHECK_AAPOLY            43      /* polygon antialising unsupported */
#define CHECK_NODITHERING       44      /* dithering unsupported */
#define CHECK_NOSCISSOR         45      /* scissoring unsupported */
#define CHECK_NOLOGICOP         46      /* logic ops unsupported */
#define CHECK_NOMASKING         47      /* masking unsupported */
#define CHECK_NOSTENCIL         48      /* stencil buffer unsupported */
#define CHECK_NOSTENCILFUNC     49      /* stencil function unsupported */
#define CHECK_NOSTENCILMASK     50      /* stencil mask unsupported */
#define CHECK_NOSTENCILOP1      51      /* stencil op for sfail unsupported */
#define CHECK_NOSTENCILOP2      52      /* stencil op for dffail unsupported */
#define CHECK_NOSTENCILOP3      53      /* stencil op for dppass unsupported */
#define CHECK_NOSTENCILWMASK    54      /* stencil write mask unsupported */
#define CHECK_NOCI              55      /* color index mode unsupported */
#define CHECK_NOASYM            56      /* asymmetric wrapping not supported */
#define CHECK_DECAL_BLEND       57      /* DECAL / alpha blending problem */
#define CHECK_NO8BIT            58      /* no 8 bit mode support */
#define CHECK_NOSPECULAR        59      /* no specular highlight */
#define CHECK_NOVMEM            60      /* no VMEM for textures */

#define CHECK_MAX               61      /* keep consistent! */

#define DRAW_POINT              0
#define DRAW_LINE               1
#define DRAW_TRIANGLE           2
#define DRAW_QUAD               3



#define HWFLAG_ACTIVE  0x01     /* hardware driver active/inactive */
#define HWFLAG_ZBUFFER 0x02     /* Z buffer allocated */
#define HWFLAG_SBUFFER 0x04     /* Stencil buffer allocated */
#define HWFLAG_RECTTEX 0x10     /* Rectangular textures supported */
#define HWFLAG_STATS   0x20     /* statistics enabled */
#define HWFLAG_NOCLAMP 0x40     /* use HW even if clamping not supported */
#define HWFLAG_NICETEX 0x80     /* highquality texturing */
#define HWFLAG_NICEFOG 0x100    /* highquality fogging */
#define HWFLAG_NOHWLINES 0x200  /* software lines */
#define HWFLAG_DIRTY   0x400    /* flush necessary before Zbuffer access */
#define HWFLAG_NOVMEM  0x800    /* no VMEM for textures */
#define HWFLAG_LOCKED  0x1000   /* in locked state  */

/*	seems that W3D_QueryDriver dont works on most Warp3D drivers
#define QUERY3D(query) W3D_QueryDriver(hwcontext->driver,query,hwcontext->destfmt)
*/

#define QUERY3D(query) W3D_Query(hwcontext->context,query,hwcontext->destfmt)

#define DRAW_MAX                4

static const char * drawstrings[] = {
        "Number of points drawn using 3D hardware:",
        "Number of lines drawn using 3D hardware:",
        "Number of triangles drawn using 3D hardware:",
        "Number of quads drawn using 3D hardware:"
        };


struct Warp3Ddriver
{
        int             scratch1[32];
        W3D_Vertex      vbuffer[VB_SIZE];
        int             scratch2[2];
        W3D_Float       colconv[256];
        W3D_Float       height;
        W3D_Context     *context;
        W3D_Driver      *driver;
        W3D_Color		bordercolor;
        ULONG           destfmt;
        ULONG           error;
        ULONG           flags;
        W3D_Float       wscale;
        W3D_Float       woffset;
        W3D_Float       near;
        W3D_Float       far;
        W3D_Scissor     scissor;
        struct BitMap*  currentbm;
        struct gl_texture_object* currenttex;
        W3D_Texture*    currentw3dtex;
        W3D_Float       texwidth;
        W3D_Float       texheight;
        ULONG           envmode;
        W3D_Color       envcolor;
        int             (*CompCheck)(GLcontext*);
        points_func     (*ChPoint)(GLcontext*);
        line_func       (*ChLine)(GLcontext*);
        triangle_func   (*ChTriangle)(GLcontext*);
        quad_func       (*ChQuad)(GLcontext*);
        linestrip_func  (*ChLineStrip)(GLcontext*);
        tristrip_func   (*ChTriStrip)(GLcontext*);
        trifan_func     (*ChTriFan)(GLcontext*);
        void            (*CopyVertex)(W3D_Vertex*,W3D_Vertex*);
        ULONG           failed;
        ULONG           statarray[CHECK_MAX];
        ULONG           statarray2[DRAW_MAX];
        int             lockmode;
        W3D_Bool        nohw;
};

typedef struct Warp3Ddriver W3Ddriver;


struct Warp3Dtexobj			/* come from tObj->DriverData */
{
        W3D_Texture     *texture;
        void            *teximage;
        ULONG           W3Dformat;
        ULONG           mmask;
        int             width;
        int             height;
        ULONG           minfilter;
        ULONG           magfilter;
        ULONG           wrap_s;
        ULONG           wrap_t;
        W3D_Bool        converted;
        W3D_Bool        hasMipmaps;
        W3D_Bool        MipMapFilter;
        W3D_Bool        MipMapTexture;
        W3D_Bool        dirtymm;
};

typedef struct Warp3Dtexobj W3Dtexobj;

struct Warp3Dteximg			/* come from tObj->Image[i]->DriverData */
{
        GLubyte*        conv_image;
        ULONG           W3DMipmaplevel;
        W3D_Bool        isW3DMap;
};

typedef struct Warp3Dteximg W3Dteximg;

#endif
