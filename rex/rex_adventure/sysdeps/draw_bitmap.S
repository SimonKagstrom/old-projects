;;; Copyright (C) 2002-2003,  Simon Kagstrom
;;;
;;;  Filename:     draw_bitmap.S
;;;  Description:  Assembly implementation of fe_draw_bitmap()
;;;
;;; This program is free software; you can redistribute it and/or
;;; modify it under the terms of the GNU General Public License
;;; as published by the Free Software Foundation; either version 2
;;; of the License, or (at your option) any later version.
;;;
;;; This program is distributed in the hope that it will be useful,
;;; but WITHOUT ANY WARRANTY; without even the implied warranty of
;;; MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
;;; GNU General Public License for more details.
;;;
;;; You should have received a copy of the GNU General Public License
;;; along with this program; if not, write to the Free Software
;;; Foundation, Inc., 59 Temple Place - Suite 330,
;;; Boston, MA  02111-1307, USA.
;;;
;;; $Id: draw_bitmap.S,v 1.3 2003/02/23 12:20:58 ska Exp $
;;;

#define p_bitmap_offs 8
#define x_offs        6
#define y_offs        4
#define inverted_offs 2

;;; fe_draw_bitmap(p_bitmap, x, y, inverted)
;;;
;;; h = p_bitmap.height;
;;; w = p_bitmap.width;
;;; byte_w = (w/8)-1;
;;;
;;; while(h-- > 0)
;;;   for (i=w-1; i>=0; i--)
;;;     b = p_bitmap + (h << byte_w)+(i >> 3);
;;;     a = (1 << ((w-1)-i)) & 7;
;;;     if ( (b & a) && inverted)
;;;       clear_pixel(x+i, y+h)
;;;     else
;;;       set_pixel(x+i, y+h)
;;;
;;; register D holds h
;;; register E holds i
;;;
	ld	HL,p_bitmap_offs ; HL == p_bitmap
	add	HL,SP

	ld	C, (HL)		; C = w
	inc	HL
	ld	B, (HL)		; B = h
	ld	D, B		; D = h
	sra	C
	sra	C
	sra	C		; B == w >> 3
	dec	C		; B == (w >> 3)-1 (byte_w)

	push	BC		; byte_w (in low byte) and w (in high byte) now on stack

	;; while (h-- > 0)
.loop_h
	pop	BC
	push	BC
	ld	E, C		; E = i (w here)

	dec	D		; h--

	;; for (i = w-1; i>=0; i--)
.loop_i
	dec	E		; i--

	ld	HL, 2
	add	HL, SP		; HL points to w/byte_w
	ld	A, (HL)		; A = w
	dec	A		; A = w-1
	sub	E		; A = (w-1)-i
	and	7		; A = ((w-1)-i) & 7

	ld	B, A
	ld	A, 1

	;; Calculate 1 << (((w-1)-i) & 7)
.l1
	sla	A
	djnz	l1
	push	AF		; Pushed 1 << (((w-1)-i) & 7)

	ld	HL, 5
	add	HL, SP		; HL points to byte_w
	ld	B, (HL)		; B = byte_w
	ld	A, D		; A = h

	;; Calculate A = (h << byte_w) + (i>>3)
.l2
	sla	A
	djnz	l2

	ld	B, E		; B = i
	sra	B
	sra	B
	sra	B		; B = i >> 3
	add	A, B		; A = (h << byte_w) + (i>>3)

	;; Calculate A = (p_bitmap + (h << byte_w) + (i>>3)) & (1 << (((w-1)-i) & 7))
	ld	HL, p_bitmap_offs + 4
	add	HL, SP
	ld	B, (HL)		; BC = p_bitmap
	inc	HL
	ld	C, (HL)
	ld	H, B
	ld	L, C		; HL = p_bitmap
	ld	B, 0
	ld	C, A		; BC = A
	add	HL, BC		; HL = p_bitmap + (h << byte_w) + (i>>3)
	inc	HL
	inc	HL		; First two bytes are the width/height
	pop	AF		; A = 1 << (((w-1)-i) & 7)
	and	(HL)		; A = *(p_bitmap + (h << byte_w) + (i>>3)) & (1 << (((w-1)-i) & 7))

	;; if A is non-zero here and invert is 0 we should set the pixel
	jr	Z, pxl_not_set
	ld	HL, inverted_offs+2
	add	HL, SP
	ld	A, (HL)		; A = inverted
	cp	1
	jr	NZ, set_pxl
	jr	clr_pxl

.pxl_not_set
	ld	HL, inverted_offs+2
	add	HL, SP
	ld	A, (HL)		; A = inverted
	cp	1
	jr	Z, set_pxl
	jr	clr_pxl

.set_pxl
	push	DE		; save i,h
	ld	HL, y_offs+2
	add	HL, SP		; HL points to y
	ld	B, (HL)
	inc	HL
	ld	C, (HL)		; BC = y
	inc	HL
	inc	HL		; HL points to x
	push	BC
	ld	B, (HL)
	inc	HL
	ld	C, (HL)		; BC = x
	push	BC
	call	DsSetPixel	; Draw this pixel
	pop	DE		; restore i,h
	jr	out
.clr_pxl
	push	DE		; save i,h
	ld	HL, y_offs+2
	add	HL, SP		; HL points to y
	ld	B, (HL)
	inc	HL
	ld	C, (HL)		; BC = y
	inc	HL
	inc	HL		; HL points to x
	push	BC
	ld	B, (HL)
	inc	HL
	ld	C, (HL)		; BC = x
	push	BC
	call	DsClearPixel	; Clear this pixel
	pop	DE		; restore i,h
.out
	ld	A,E
	cp	0
	jp	NZ, loop_i	; if i!=0 goto loop_i

	ld	A, D
	cp	0
	jp	NZ, loop_h	; if (h==0) goto loop_h

	pop	AF		; pop w/byte_w (restore stack)
	ret			; old PC is now on top of the stack
