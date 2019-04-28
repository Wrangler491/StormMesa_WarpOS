 *
 * Mesa 3-D graphics library
 * Copyright (C) 1995  Brian Paul  (brianp@ssec.wisc.edu)
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the Free
 * Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *

* 68K Assembler stubs for SWFSDriver_SwapBuffer_FS/AGA (should not
* be done by PPC)

		near    a4,-1
		include "GL/Amigamesa.i"
		include "exec/ports.i"
		include "intuition/screens.i"
		include "graphics/rastport.i"
		include "graphics/gfx.i"
		include "graphics/view.i"
		include "cybergraphics/cybergraphics.i"

		include "exec_lib.i"
		include "intuition_lib.i"
		include "graphics_lib.i"
		include "cybergraphics_lib.i"

		NREF    _SysBase
		NREF    _GfxBase
		NREF    _IntuitionBase
		NREF    _CyberGfxBase

		XDEF    _SwapBuffer_FS
		XDEF    _SwapBuffer_FS3
		XDEF    _SwapBuffer_AGA
		XDEF    _FinalSwap

_SwapBuffer_FS
		movem.l d2/d3/a2/a3/a6,-(sp)
		move.l  a0,a3
		move.l  AMC_SBUF2(a3),a0
		move.l  sb_DBufInfo(a0),a0
		move.l  AMC_DBPORT(a3),dbi_SafeMessage+MN_REPLYPORT(a0)
.loop
		move.l  _IntuitionBase,a6
		move.l  AMC_SCREEN(a3),a0
		move.l  AMC_SBUF2(a3),a1
		jsr     _LVOChangeScreenBuffer(a6)
		tst.l   d0
		bne.b   .cont
		move.l  _GfxBase,a6
		jsr     _LVOWaitTOF(a6)
		bra.b   .loop
.cont
		move.l  AMC_SBUF1(a3),a0
		move.l  AMC_SBUF2(a3),a1
		move.l  a1,AMC_SBUF1(a3)
		move.l  a0,AMC_SBUF2(a3)
		move.l  AMC_BACK_RP(a3),a2
		move.l  sb_BitMap(a0),d2
		move.l  d2,rp_BitMap(a2)
		move.l  AMC_BACK2_RP(a3),a2
		move.l  sb_BitMap(a1),d3
		move.l  d3,rp_BitMap(a2)
		move.l  _CyberGfxBase,a6
		move.l  d2,a0
		move.l  #CYBRMATTR_DISPADR,d0
		jsr     _LVOGetCyberMapAttr(a6)
		move.l  d0,AMC_BACKARRAY(a3)
		move.l  d3,a0
		move.l  #CYBRMATTR_DISPADR,d0
		jsr     _LVOGetCyberMapAttr(a6)
		move.l  d0,AMC_FRONTARRAY(a3)
		move.l  AMC_SCREEN(a3),a0
		lea     sc_ViewPort(a0),a0
		move.l  _GfxBase,a6
		jsr     _LVOWaitBOVP(a6)
.loop2
		move.l  _SysBase,a6
		move.l  AMC_DBPORT(a3),a0
		jsr     _LVOGetMsg(a6)
		tst.l   d0
		bne.b   .cont2
		move.l  AMC_DBPORT(a3),a0
		moveq   #0,d1
		move.b  MP_SIGBIT(a0),d1
		moveq   #1,d0
		lsl.l   d1,d0
		jsr     _LVOWait(a6)
		bra.b   .loop2
.cont2
		eor.l   #1,AMC_SWAPCOUNTER(a3)
		movem.l (sp)+,d2/d3/a2/a3/a6
		rts

_SwapBuffer_FS3
		movem.l d2/d3/a2/a3/a6,-(sp)
		move.l  a0,a3
.loop
		move.l  _IntuitionBase,a6
		move.l  AMC_SCREEN(a3),a0
		move.l  AMC_SBUF2(a3),a1
		jsr     _LVOChangeScreenBuffer(a6)
		tst.l   d0
		bne.b   .cont
		move.l  _GfxBase,a6
		jsr     _LVOWaitTOF(a6)
		bra.b   .loop
.cont
		move.l  AMC_SBUF1(a3),a0
		move.l  AMC_SBUF2(a3),a1
		move.l  AMC_SBUF3(a3),a2
		move.l  a1,AMC_SBUF1(a3)
		move.l  a2,AMC_SBUF2(a3)
		move.l  a0,AMC_SBUF3(a3)
		move.l  AMC_BACK_RP(a3),a0
		move.l  sb_BitMap(a2),d2
		move.l  d2,rp_BitMap(a0)
		move.l  AMC_BACK2_RP(a3),a0
		move.l  sb_BitMap(a1),d3
		move.l  d3,rp_BitMap(a0)
		move.l  _CyberGfxBase,a6
		move.l  d2,a0
		move.l  #CYBRMATTR_DISPADR,d0
		jsr     _LVOGetCyberMapAttr(a6)
		move.l  d0,AMC_BACKARRAY(a3)
		move.l  d3,a0
		move.l  #CYBRMATTR_DISPADR,d0
		jsr     _LVOGetCyberMapAttr(a6)
		move.l  d0,AMC_FRONTARRAY(a3)
		move.l  AMC_SWAPCOUNTER(a3),d0
		addq.l  #1,d0
		cmp.l   #3,d0
		bne.b   .skip
		moveq   #0,d0
.skip
		move.l  d0,AMC_SWAPCOUNTER(a3)
		movem.l (sp)+,d2/d3/a2/a3/a6
		rts


_SwapBuffer_AGA
		movem.l a2/a3/a6,-(sp)
		move.l  a0,a3
		move.l  AMC_SBUF2(a3),a0
		move.l  sb_DBufInfo(a0),a0
		move.l  AMC_DBPORT(a3),dbi_SafeMessage+MN_REPLYPORT(a0)
.loop
		move.l  _IntuitionBase,a6
		move.l  AMC_SCREEN(a3),a0
		move.l  AMC_SBUF2(a3),a1
		jsr     _LVOChangeScreenBuffer(a6)
		tst.l   d0
		bne.b   .cont
		move.l  _GfxBase,a6
		jsr     _LVOWaitTOF(a6)
		bra.b   .loop
.cont
		move.l  AMC_SBUF1(a3),a0
		move.l  AMC_SBUF2(a3),a1
		move.l  a1,AMC_SBUF1(a3)
		move.l  a0,AMC_SBUF2(a3)
		move.l  AMC_BACKARRAY(a3),d0
		move.l  AMC_FRONTARRAY(a3),AMC_BACKARRAY(a3)
		move.l  d0,AMC_FRONTARRAY(a3)
		move.l  AMC_BACK_RP(a3),a2
		move.l  sb_BitMap(a0),d0
		move.l  d0,rp_BitMap(a2)
		move.l  AMC_SCREEN(a3),a0
		lea     sc_ViewPort(a0),a0
		move.l  _GfxBase,a6
		jsr     _LVOWaitBOVP(a6)
.loop2
		move.l  _SysBase,a6
		move.l  AMC_DBPORT(a3),a0
		jsr     _LVOGetMsg(a6)
		tst.l   d0
		bne.b   .cont2
		move.l  AMC_DBPORT(a3),a0
		moveq   #0,d1
		move.b  MP_SIGBIT(a0),d1
		moveq   #1,d0
		lsl.l   d1,d0
		jsr     _LVOWait(a6)
		bra.b   .loop2
.cont2
		eor.l   #1,AMC_SWAPCOUNTER(a3)
		movem.l (sp)+,a2/a3/a6
		rts

_FinalSwap
		movem.l a2/a3/a6,-(sp)
		move.l  a0,a3
		move.l  AMC_SBUF_INITIAL(a3),a0
		move.l  sb_DBufInfo(a0),a0
		clr.l   dbi_SafeMessage+MN_REPLYPORT(a0)
.loop
		move.l  _IntuitionBase,a6
		move.l  AMC_SCREEN(a3),a0
		move.l  AMC_SBUF_INITIAL(a3),a1
		jsr     _LVOChangeScreenBuffer(a6)
		tst.l   d0
		bne.b   .cont
		move.l  _GfxBase,a6
		jsr     _LVOWaitTOF(a6)
		bra.b   .loop
.cont
		movem.l (sp)+,a2/a3/a6
		rts

