;;; Copyright (C) 2002,  Simon Kagstrom
;;;
;;;  File path:     draw_bitmap.S
;;;  Description:   Assembly implementation of fe_draw_bitmap()
;;;
;;;  @version@
;;;
;;;  $Id: draw_bitmap.S,v 1.1 2003/02/09 08:14:52 ska Exp $
;;;

;;;

#define p_bitmap_offs 8
#define x_offs        6
#define y_offs        4
#define inverted_offs 2

	ld	HL,p_bitmap_offs ; HL == p_bitmap
	add	HL,SP

	ld	B, (HL)		; B = w
	inc	HL
	ld	C, (HL)		; C = h
	ld	D, C		; D = h
	sra	B
	sra	B
	sra	B		; B == w >> 3
	dec	B		; B == (w >> 3)-1 (byte_w)

	push	BC		; byte_w (in low byte) and w (in high byte) now on stack

	;; while (h > 0)
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

	ld	HL, 4
	add	HL, SP		; HL points to w/byte_w
	ld	B, (HL)
	inc	HL
	ld	C, (HL)		; B = w, C = byte_w
	ld	B, C		; B = byte_w
	ld	A, D		; A = h

	;; Calculate A = h << byte_w
.l2
	sla	A
	djnz	l2

	ld	B, E		; B = i
	sra	B
	sra	B
	sra	B		; B = i >> 3
	add	A, B		; A = (h << byte_w) + (i>>3)

	ld	B, A
	ld	C, 0		; BC = A
	ld	HL, p_bitmap_offs + 2
	add	HL, SP
	add	HL, BC		; HL = p_bitmap + (h << byte_w) + (i>>3)
	inc	HL
	inc	HL		; First two bytes are the width/height
	pop	AF		; A = 1 << (((w-1)-i) & 7)
	and	(HL)		; A = (p_bitmap + (h << byte_w) + (i>>3)) & (1 << (((w-1)-i) & 7))

	;; if A is non-zero here and invert is 0 we should set the pixel
	jr	Z, pxl_not_set
	ld	HL, inverted_offs
	add	HL, SP
	ld	A, (HL)		; A = inverted
	cp	1
	jr	NZ, set_pxl
	jr	clr_pxl

.pxl_not_set
	ld	HL, inverted_offs
	add	HL, SP
	ld	A, (HL)		; A = inverted
	cp	1
	jr	Z, set_pxl
	jr	clr_pxl

.set_pxl
	ld	HL, y_offs
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
	jr	out
.clr_pxl
	ld	HL, y_offs
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
.out
	jp	NZ, loop_i

	ld	A, D		; A = h
	cp	0		; A == 0?
	jr	NZ, loop_h	; continue while-loop
