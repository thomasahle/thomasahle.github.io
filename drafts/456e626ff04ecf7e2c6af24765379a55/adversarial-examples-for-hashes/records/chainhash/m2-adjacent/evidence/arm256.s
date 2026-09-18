	.section	__TEXT,__text,regular,pure_instructions
	.build_version macos, 26, 2	sdk_version 26, 2
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ; -- Begin function audit_batch1
lCPI0_0:
	.byte	27                              ; 0x1b
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	27                              ; 0x1b
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.section	__TEXT,__text,regular,pure_instructions
	.globl	_audit_batch1
	.p2align	2
_audit_batch1:                          ; @audit_batch1
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #144
	stp	x22, x21, [sp, #96]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #112]            ; 16-byte Folded Spill
	stp	x29, x30, [sp, #128]            ; 16-byte Folded Spill
	add	x29, sp, #128
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
Lloh0:
	adrp	x8, ___stack_chk_guard@GOTPAGE
Lloh1:
	ldr	x8, [x8, ___stack_chk_guard@GOTPAGEOFF]
Lloh2:
	ldr	x8, [x8]
	stur	x8, [x29, #-40]
	ldr	q18, [x0, #256]
	ldr	d0, [x0, #272]
	eor.8b	v19, v18, v0
	adrp	x19, lCPI0_0@PAGE
	cmp	x2, #257
	b.lo	LBB0_9
; %bb.1:
	ldr	q0, [x19, lCPI0_0@PAGEOFF]
	mov	x8, x2
LBB0_2:                                 ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB0_3 Depth 2
	mov	x9, #0                          ; =0x0
	movi.2d	v1, #0000000000000000
	movi.2d	v2, #0000000000000000
LBB0_3:                                 ;   Parent Loop BB0_2 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	x10, x1, x9
	ld2.2d	{ v3, v4 }, [x10], #32
	add	x11, x0, x9
	ld2.2d	{ v5, v6 }, [x11], #32
	eor.16b	v7, v5, v3
	eor.16b	v3, v6, v4
	; InlineAsm Start
	pmull.1q	v4, v7, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v7, v3
	; InlineAsm End
	eor3.16b	v2, v4, v2, v3
	ld2.2d	{ v3, v4 }, [x10]
	ld2.2d	{ v5, v6 }, [x11]
	eor.16b	v7, v5, v3
	eor.16b	v3, v6, v4
	; InlineAsm Start
	pmull.1q	v4, v7, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v7, v3
	; InlineAsm End
	eor3.16b	v1, v4, v1, v3
	add	x10, x9, #64
	cmp	x9, #192
	mov	x9, x10
	b.lo	LBB0_3
; %bb.4:                                ;   in Loop: Header=BB0_2 Depth=1
	eor.16b	v3, v2, v18
	eor3.16b	v2, v2, v18, v1
	ext.16b	v2, v2, v2, #8
	; InlineAsm Start
	pmull.1q	v2, v2, v19
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v4, v2, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v5, v4, v0
	; InlineAsm End
	eor3.16b	v1, v3, v1, v5
	eor3.16b	v19, v1, v2, v4
	add	x1, x1, #256
	sub	x8, x8, #256
	cmp	x8, #256
	b.hi	LBB0_2
; %bb.5:
	cmp	x8, #256
	b.ne	LBB0_10
LBB0_6:
	mov	x8, #0                          ; =0x0
	movi.2d	v0, #0000000000000000
	movi.2d	v1, #0000000000000000
LBB0_7:                                 ; =>This Inner Loop Header: Depth=1
	add	x9, x1, x8
	ld2.2d	{ v2, v3 }, [x9], #32
	add	x10, x0, x8
	ld2.2d	{ v4, v5 }, [x10], #32
	eor.16b	v6, v4, v2
	eor.16b	v2, v5, v3
	; InlineAsm Start
	pmull.1q	v3, v6, v2
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v2, v6, v2
	; InlineAsm End
	eor3.16b	v1, v3, v1, v2
	ld2.2d	{ v2, v3 }, [x9]
	ld2.2d	{ v4, v5 }, [x10]
	eor.16b	v6, v4, v2
	eor.16b	v2, v5, v3
	; InlineAsm Start
	pmull.1q	v3, v6, v2
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v2, v6, v2
	; InlineAsm End
	eor3.16b	v0, v3, v0, v2
	add	x9, x8, #64
	cmp	x8, #192
	mov	x8, x9
	b.lo	LBB0_7
; %bb.8:
	eor.16b	v6, v0, v1
	b	LBB0_20
LBB0_9:
	mov	x8, x2
	cmp	x2, #256
	b.eq	LBB0_6
LBB0_10:
	cmp	x8, #64
	b.lo	LBB0_14
; %bb.11:
	mov	x9, #0                          ; =0x0
	movi.2d	v7, #0000000000000000
	mov	x10, x0
	mov	x11, x1
	movi.2d	v16, #0000000000000000
LBB0_12:                                ; =>This Inner Loop Header: Depth=1
	mov	x12, x11
	ld2.2d	{ v0, v1 }, [x12], #32
	mov	x13, x10
	ld2.2d	{ v2, v3 }, [x13], #32
	eor.16b	v4, v2, v0
	ld2.2d	{ v5, v6 }, [x12]
	eor.16b	v0, v3, v1
	; InlineAsm Start
	pmull.1q	v1, v4, v0
	; InlineAsm End
	ld2.2d	{ v2, v3 }, [x13]
	; InlineAsm Start
	pmull2.1q	v0, v4, v0
	; InlineAsm End
	eor3.16b	v16, v1, v16, v0
	eor.16b	v0, v2, v5
	eor.16b	v1, v3, v6
	; InlineAsm Start
	pmull.1q	v2, v0, v1
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v0, v0, v1
	; InlineAsm End
	eor3.16b	v7, v2, v7, v0
	sub	x9, x9, #64
	add	x11, x11, #64
	add	x10, x10, #64
	add	x12, x8, x9
	cmp	x12, #63
	b.hi	LBB0_12
; %bb.13:
	neg	x20, x9
	mov	x8, x12
	subs	x9, x12, #32
	b.hs	LBB0_15
	b	LBB0_16
LBB0_14:
	mov	x20, #0                         ; =0x0
	movi.2d	v16, #0000000000000000
	movi.2d	v7, #0000000000000000
	subs	x9, x8, #32
	b.lo	LBB0_16
LBB0_15:
	add	x8, x0, x20
	add	x10, x1, x20
	ld2.2d	{ v0, v1 }, [x10]
	ld2.2d	{ v2, v3 }, [x8]
	eor.16b	v4, v2, v0
	eor.16b	v0, v3, v1
	; InlineAsm Start
	pmull.1q	v1, v4, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v0, v4, v0
	; InlineAsm End
	eor.16b	v0, v1, v0
	eor.16b	v16, v0, v16
	orr	x20, x20, #0x20
	mov	x8, x9
LBB0_16:
	subs	x9, x8, #16
	b.hs	LBB0_22
; %bb.17:
	cbz	x8, LBB0_19
LBB0_18:
	stp	xzr, xzr, [x29, #-56]
	mov	x21, x0
	sub	x0, x29, #56
	add	x1, x1, x20
	mov	x22, x2
	mov	x2, x8
	stp	q19, q18, [sp, #32]             ; 32-byte Folded Spill
	stp	q16, q7, [sp]                   ; 32-byte Folded Spill
	bl	_memcpy
	ldp	q16, q7, [sp]                   ; 32-byte Folded Reload
	ldp	q19, q18, [sp, #32]             ; 32-byte Folded Reload
	mov	x0, x21
	mov	x2, x22
	and	x8, x20, #0xfffffffffffffff8
	add	x8, x21, x8
	ldp	d0, d1, [x29, #-56]
	ldp	d2, d3, [x8]
	eor.8b	v0, v2, v0
	eor.8b	v1, v3, v1
	; InlineAsm Start
	pmull.1q	v0, v0, v1
	; InlineAsm End
	eor.16b	v7, v0, v7
LBB0_19:
	eor.16b	v6, v7, v16
LBB0_20:
	ldp	d3, d4, [x0, #280]
	ldp	d1, d2, [x0, #296]
	ldp	d0, d5, [x0, #312]
	ldur	x8, [x29, #-40]
Lloh3:
	adrp	x9, ___stack_chk_guard@GOTPAGE
Lloh4:
	ldr	x9, [x9, ___stack_chk_guard@GOTPAGEOFF]
Lloh5:
	ldr	x9, [x9]
	cmp	x9, x8
	b.ne	LBB0_23
; %bb.21:
	fmov	d7, d18
	dup.2d	v16, x2
	eor.16b	v17, v16, v6
	eor3.16b	v6, v16, v6, v18
	eor3.16b	v16, v17, v18, v7
	ext.16b	v16, v16, v16, #8
	; InlineAsm Start
	pmull.1q	v16, v16, v19
	; InlineAsm End
	ldr	q17, [x19, lCPI0_0@PAGEOFF]
	; InlineAsm Start
	pmull2.1q	v18, v16, v17
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v19, v18, v17
	; InlineAsm End
	eor3.16b	v16, v16, v19, v18
	eor3.16b	v6, v6, v7, v16
	add.2d	v5, v6, v5
	; InlineAsm Start
	pmull.1q	v6, v5, v5
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v7, v6, v17
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v7, v17
	; InlineAsm End
	eor.16b	v6, v6, v16
	eor3.16b	v6, v6, v3, v7
	eor.8b	v3, v3, v4
	eor3.16b	v3, v3, v5, v6
	; InlineAsm Start
	pmull.1q	v3, v6, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v4, v3, v17
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v6, v4, v17
	; InlineAsm End
	eor.16b	v2, v2, v6
	eor3.16b	v2, v2, v3, v4
	eor.16b	v1, v1, v5
	; InlineAsm Start
	pmull.1q	v1, v1, v2
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v2, v1, v17
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v2, v17
	; InlineAsm End
	eor.16b	v0, v0, v3
	eor3.16b	v0, v0, v1, v2
	fmov	x0, d0
	ldp	x29, x30, [sp, #128]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #112]            ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #96]             ; 16-byte Folded Reload
	add	sp, sp, #144
	ret
LBB0_22:
	and	x8, x20, #0xfffffffffffffff8
	add	x8, x0, x8
	add	x10, x1, x20
	ldp	d0, d1, [x10]
	ldp	d2, d3, [x8]
	eor.8b	v0, v2, v0
	eor.8b	v1, v3, v1
	; InlineAsm Start
	pmull.1q	v0, v0, v1
	; InlineAsm End
	eor.16b	v7, v0, v7
	add	x20, x20, #16
	mov	x8, x9
	cbnz	x9, LBB0_18
	b	LBB0_19
LBB0_23:
	bl	___stack_chk_fail
	.loh AdrpLdrGotLdr	Lloh0, Lloh1, Lloh2
	.loh AdrpLdrGotLdr	Lloh3, Lloh4, Lloh5
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ; -- Begin function audit_batch2
lCPI1_0:
	.byte	27                              ; 0x1b
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	27                              ; 0x1b
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.section	__TEXT,__text,regular,pure_instructions
	.globl	_audit_batch2
	.p2align	2
_audit_batch2:                          ; @audit_batch2
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #144
	stp	x22, x21, [sp, #96]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #112]            ; 16-byte Folded Spill
	stp	x29, x30, [sp, #128]            ; 16-byte Folded Spill
	add	x29, sp, #128
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
Lloh6:
	adrp	x8, ___stack_chk_guard@GOTPAGE
Lloh7:
	ldr	x8, [x8, ___stack_chk_guard@GOTPAGEOFF]
Lloh8:
	ldr	x8, [x8]
	stur	x8, [x29, #-40]
	ldr	q18, [x0, #256]
	ldr	d0, [x0, #272]
	eor.8b	v19, v18, v0
	adrp	x19, lCPI1_0@PAGE
	cmp	x2, #513
	b.lo	LBB1_8
; %bb.1:
	ldr	q0, [x19, lCPI1_0@PAGEOFF]
	mov	x8, x2
LBB1_2:                                 ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB1_3 Depth 2
                                        ;     Child Loop BB1_5 Depth 2
	mov	x9, #0                          ; =0x0
	movi.2d	v1, #0000000000000000
	movi.2d	v2, #0000000000000000
LBB1_3:                                 ;   Parent Loop BB1_2 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	x10, x1, x9
	ld2.2d	{ v3, v4 }, [x10], #32
	add	x11, x0, x9
	ld2.2d	{ v5, v6 }, [x11], #32
	eor.16b	v7, v5, v3
	eor.16b	v3, v6, v4
	; InlineAsm Start
	pmull.1q	v4, v7, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v7, v3
	; InlineAsm End
	eor3.16b	v2, v4, v2, v3
	ld2.2d	{ v3, v4 }, [x10]
	ld2.2d	{ v5, v6 }, [x11]
	eor.16b	v7, v5, v3
	eor.16b	v3, v6, v4
	; InlineAsm Start
	pmull.1q	v4, v7, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v7, v3
	; InlineAsm End
	eor3.16b	v1, v4, v1, v3
	add	x10, x9, #64
	cmp	x9, #192
	mov	x9, x10
	b.lo	LBB1_3
; %bb.4:                                ;   in Loop: Header=BB1_2 Depth=1
	mov	x9, #0                          ; =0x0
	eor.16b	v3, v2, v18
	movi.2d	v2, #0000000000000000
	movi.2d	v4, #0000000000000000
LBB1_5:                                 ;   Parent Loop BB1_2 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	x10, x1, x9
	add	x11, x10, #256
	ld2.2d	{ v5, v6 }, [x11]
	add	x11, x0, x9
	ld2.2d	{ v16, v17 }, [x11], #32
	eor.16b	v7, v16, v5
	eor.16b	v5, v17, v6
	; InlineAsm Start
	pmull.1q	v6, v7, v5
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v5, v7, v5
	; InlineAsm End
	eor3.16b	v4, v6, v4, v5
	add	x10, x10, #288
	ld2.2d	{ v5, v6 }, [x10]
	ld2.2d	{ v16, v17 }, [x11]
	eor.16b	v7, v16, v5
	eor.16b	v5, v17, v6
	; InlineAsm Start
	pmull.1q	v6, v7, v5
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v5, v7, v5
	; InlineAsm End
	eor3.16b	v2, v6, v2, v5
	add	x10, x9, #64
	cmp	x9, #192
	mov	x9, x10
	b.lo	LBB1_5
; %bb.6:                                ;   in Loop: Header=BB1_2 Depth=1
	eor.16b	v1, v3, v1
	ext.16b	v3, v1, v1, #8
	eor.16b	v5, v4, v18
	eor3.16b	v4, v4, v18, v2
	ext.16b	v4, v4, v4, #8
	; InlineAsm Start
	pmull.1q	v1, v4, v1
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v3, v4, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v4, v1, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v6, v4, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v7, v3, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v7, v0
	; InlineAsm End
	eor3.16b	v3, v3, v16, v7
	; InlineAsm Start
	pmull.1q	v3, v3, v19
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v7, v3, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v7, v0
	; InlineAsm End
	eor.16b	v6, v6, v16
	eor3.16b	v2, v5, v2, v6
	eor3.16b	v1, v2, v1, v4
	eor3.16b	v19, v1, v3, v7
	add	x1, x1, #512
	sub	x8, x8, #512
	cmp	x8, #512
	b.hi	LBB1_2
; %bb.7:
	cmp	x8, #257
	b.hs	LBB1_9
	b	LBB1_13
LBB1_8:
	mov	x8, x2
	cmp	x2, #257
	b.lo	LBB1_13
LBB1_9:
	ldr	q0, [x19, lCPI1_0@PAGEOFF]
LBB1_10:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB1_11 Depth 2
	mov	x9, #0                          ; =0x0
	movi.2d	v1, #0000000000000000
	movi.2d	v2, #0000000000000000
LBB1_11:                                ;   Parent Loop BB1_10 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	x10, x1, x9
	ld2.2d	{ v3, v4 }, [x10], #32
	add	x11, x0, x9
	ld2.2d	{ v5, v6 }, [x11], #32
	eor.16b	v7, v5, v3
	eor.16b	v3, v6, v4
	; InlineAsm Start
	pmull.1q	v4, v7, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v7, v3
	; InlineAsm End
	eor3.16b	v2, v4, v2, v3
	ld2.2d	{ v3, v4 }, [x10]
	ld2.2d	{ v5, v6 }, [x11]
	eor.16b	v7, v5, v3
	eor.16b	v3, v6, v4
	; InlineAsm Start
	pmull.1q	v4, v7, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v7, v3
	; InlineAsm End
	eor3.16b	v1, v4, v1, v3
	add	x10, x9, #64
	cmp	x9, #192
	mov	x9, x10
	b.lo	LBB1_11
; %bb.12:                               ;   in Loop: Header=BB1_10 Depth=1
	eor.16b	v3, v2, v18
	eor3.16b	v2, v2, v18, v1
	ext.16b	v2, v2, v2, #8
	; InlineAsm Start
	pmull.1q	v2, v2, v19
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v4, v2, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v5, v4, v0
	; InlineAsm End
	eor3.16b	v1, v3, v1, v5
	eor3.16b	v19, v1, v2, v4
	add	x1, x1, #256
	sub	x8, x8, #256
	cmp	x8, #256
	b.hi	LBB1_10
LBB1_13:
	cmp	x8, #256
	b.ne	LBB1_17
; %bb.14:
	mov	x8, #0                          ; =0x0
	movi.2d	v0, #0000000000000000
	movi.2d	v1, #0000000000000000
LBB1_15:                                ; =>This Inner Loop Header: Depth=1
	add	x9, x1, x8
	ld2.2d	{ v2, v3 }, [x9], #32
	add	x10, x0, x8
	ld2.2d	{ v4, v5 }, [x10], #32
	eor.16b	v6, v4, v2
	eor.16b	v2, v5, v3
	; InlineAsm Start
	pmull.1q	v3, v6, v2
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v2, v6, v2
	; InlineAsm End
	eor3.16b	v1, v3, v1, v2
	ld2.2d	{ v2, v3 }, [x9]
	ld2.2d	{ v4, v5 }, [x10]
	eor.16b	v6, v4, v2
	eor.16b	v2, v5, v3
	; InlineAsm Start
	pmull.1q	v3, v6, v2
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v2, v6, v2
	; InlineAsm End
	eor3.16b	v0, v3, v0, v2
	add	x9, x8, #64
	cmp	x8, #192
	mov	x8, x9
	b.lo	LBB1_15
; %bb.16:
	eor.16b	v6, v0, v1
	b	LBB1_27
LBB1_17:
	cmp	x8, #64
	b.lo	LBB1_21
; %bb.18:
	mov	x9, #0                          ; =0x0
	movi.2d	v7, #0000000000000000
	mov	x10, x0
	mov	x11, x1
	movi.2d	v16, #0000000000000000
LBB1_19:                                ; =>This Inner Loop Header: Depth=1
	mov	x12, x11
	ld2.2d	{ v0, v1 }, [x12], #32
	mov	x13, x10
	ld2.2d	{ v2, v3 }, [x13], #32
	eor.16b	v4, v2, v0
	ld2.2d	{ v5, v6 }, [x12]
	eor.16b	v0, v3, v1
	; InlineAsm Start
	pmull.1q	v1, v4, v0
	; InlineAsm End
	ld2.2d	{ v2, v3 }, [x13]
	; InlineAsm Start
	pmull2.1q	v0, v4, v0
	; InlineAsm End
	eor3.16b	v16, v1, v16, v0
	eor.16b	v0, v2, v5
	eor.16b	v1, v3, v6
	; InlineAsm Start
	pmull.1q	v2, v0, v1
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v0, v0, v1
	; InlineAsm End
	eor3.16b	v7, v2, v7, v0
	sub	x9, x9, #64
	add	x11, x11, #64
	add	x10, x10, #64
	add	x12, x8, x9
	cmp	x12, #63
	b.hi	LBB1_19
; %bb.20:
	neg	x20, x9
	mov	x8, x12
	subs	x9, x12, #32
	b.hs	LBB1_22
	b	LBB1_23
LBB1_21:
	mov	x20, #0                         ; =0x0
	movi.2d	v16, #0000000000000000
	movi.2d	v7, #0000000000000000
	subs	x9, x8, #32
	b.lo	LBB1_23
LBB1_22:
	add	x8, x0, x20
	add	x10, x1, x20
	ld2.2d	{ v0, v1 }, [x10]
	ld2.2d	{ v2, v3 }, [x8]
	eor.16b	v4, v2, v0
	eor.16b	v0, v3, v1
	; InlineAsm Start
	pmull.1q	v1, v4, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v0, v4, v0
	; InlineAsm End
	eor.16b	v0, v1, v0
	eor.16b	v16, v0, v16
	orr	x20, x20, #0x20
	mov	x8, x9
LBB1_23:
	subs	x9, x8, #16
	b.hs	LBB1_29
; %bb.24:
	cbz	x8, LBB1_26
LBB1_25:
	stp	xzr, xzr, [x29, #-56]
	mov	x21, x0
	sub	x0, x29, #56
	add	x1, x1, x20
	mov	x22, x2
	mov	x2, x8
	stp	q19, q18, [sp, #32]             ; 32-byte Folded Spill
	stp	q16, q7, [sp]                   ; 32-byte Folded Spill
	bl	_memcpy
	ldp	q16, q7, [sp]                   ; 32-byte Folded Reload
	ldp	q19, q18, [sp, #32]             ; 32-byte Folded Reload
	mov	x0, x21
	mov	x2, x22
	and	x8, x20, #0xfffffffffffffff8
	add	x8, x21, x8
	ldp	d0, d1, [x29, #-56]
	ldp	d2, d3, [x8]
	eor.8b	v0, v2, v0
	eor.8b	v1, v3, v1
	; InlineAsm Start
	pmull.1q	v0, v0, v1
	; InlineAsm End
	eor.16b	v7, v0, v7
LBB1_26:
	eor.16b	v6, v7, v16
LBB1_27:
	ldp	d3, d4, [x0, #280]
	ldp	d1, d2, [x0, #296]
	ldp	d0, d5, [x0, #312]
	ldur	x8, [x29, #-40]
Lloh9:
	adrp	x9, ___stack_chk_guard@GOTPAGE
Lloh10:
	ldr	x9, [x9, ___stack_chk_guard@GOTPAGEOFF]
Lloh11:
	ldr	x9, [x9]
	cmp	x9, x8
	b.ne	LBB1_30
; %bb.28:
	fmov	d7, d18
	dup.2d	v16, x2
	eor.16b	v17, v16, v6
	eor3.16b	v6, v16, v6, v18
	eor3.16b	v16, v17, v18, v7
	ext.16b	v16, v16, v16, #8
	; InlineAsm Start
	pmull.1q	v16, v16, v19
	; InlineAsm End
	ldr	q17, [x19, lCPI1_0@PAGEOFF]
	; InlineAsm Start
	pmull2.1q	v18, v16, v17
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v19, v18, v17
	; InlineAsm End
	eor3.16b	v16, v16, v19, v18
	eor3.16b	v6, v6, v7, v16
	add.2d	v5, v6, v5
	; InlineAsm Start
	pmull.1q	v6, v5, v5
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v7, v6, v17
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v7, v17
	; InlineAsm End
	eor.16b	v6, v6, v16
	eor3.16b	v6, v6, v3, v7
	eor.8b	v3, v3, v4
	eor3.16b	v3, v3, v5, v6
	; InlineAsm Start
	pmull.1q	v3, v6, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v4, v3, v17
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v6, v4, v17
	; InlineAsm End
	eor.16b	v2, v2, v6
	eor3.16b	v2, v2, v3, v4
	eor.16b	v1, v1, v5
	; InlineAsm Start
	pmull.1q	v1, v1, v2
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v2, v1, v17
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v2, v17
	; InlineAsm End
	eor.16b	v0, v0, v3
	eor3.16b	v0, v0, v1, v2
	fmov	x0, d0
	ldp	x29, x30, [sp, #128]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #112]            ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #96]             ; 16-byte Folded Reload
	add	sp, sp, #144
	ret
LBB1_29:
	and	x8, x20, #0xfffffffffffffff8
	add	x8, x0, x8
	add	x10, x1, x20
	ldp	d0, d1, [x10]
	ldp	d2, d3, [x8]
	eor.8b	v0, v2, v0
	eor.8b	v1, v3, v1
	; InlineAsm Start
	pmull.1q	v0, v0, v1
	; InlineAsm End
	eor.16b	v7, v0, v7
	add	x20, x20, #16
	mov	x8, x9
	cbnz	x9, LBB1_25
	b	LBB1_26
LBB1_30:
	bl	___stack_chk_fail
	.loh AdrpLdrGotLdr	Lloh6, Lloh7, Lloh8
	.loh AdrpLdrGotLdr	Lloh9, Lloh10, Lloh11
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ; -- Begin function audit_batch4
lCPI2_0:
	.byte	27                              ; 0x1b
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	27                              ; 0x1b
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.section	__TEXT,__text,regular,pure_instructions
	.globl	_audit_batch4
	.p2align	2
_audit_batch4:                          ; @audit_batch4
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #144
	stp	x22, x21, [sp, #96]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #112]            ; 16-byte Folded Spill
	stp	x29, x30, [sp, #128]            ; 16-byte Folded Spill
	add	x29, sp, #128
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
Lloh12:
	adrp	x8, ___stack_chk_guard@GOTPAGE
Lloh13:
	ldr	x8, [x8, ___stack_chk_guard@GOTPAGEOFF]
Lloh14:
	ldr	x8, [x8]
	stur	x8, [x29, #-40]
	ldr	q23, [x0, #256]
	ldr	d0, [x0, #272]
	eor.8b	v24, v23, v0
	adrp	x19, lCPI2_0@PAGE
	cmp	x2, #1025
	b.lo	LBB2_12
; %bb.1:
	ldr	q0, [x19, lCPI2_0@PAGEOFF]
	mov	x8, x2
LBB2_2:                                 ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB2_3 Depth 2
                                        ;     Child Loop BB2_5 Depth 2
                                        ;     Child Loop BB2_7 Depth 2
                                        ;     Child Loop BB2_9 Depth 2
	mov	x9, #0                          ; =0x0
	movi.2d	v1, #0000000000000000
	movi.2d	v2, #0000000000000000
LBB2_3:                                 ;   Parent Loop BB2_2 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	x10, x1, x9
	ld2.2d	{ v3, v4 }, [x10], #32
	add	x11, x0, x9
	ld2.2d	{ v5, v6 }, [x11], #32
	eor.16b	v7, v5, v3
	eor.16b	v3, v6, v4
	; InlineAsm Start
	pmull.1q	v4, v7, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v7, v3
	; InlineAsm End
	eor3.16b	v2, v4, v2, v3
	ld2.2d	{ v3, v4 }, [x10]
	ld2.2d	{ v5, v6 }, [x11]
	eor.16b	v7, v5, v3
	eor.16b	v3, v6, v4
	; InlineAsm Start
	pmull.1q	v4, v7, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v7, v3
	; InlineAsm End
	eor3.16b	v1, v4, v1, v3
	add	x10, x9, #64
	cmp	x9, #192
	mov	x9, x10
	b.lo	LBB2_3
; %bb.4:                                ;   in Loop: Header=BB2_2 Depth=1
	mov	x9, #0                          ; =0x0
	eor.16b	v3, v2, v23
	movi.2d	v2, #0000000000000000
	movi.2d	v4, #0000000000000000
LBB2_5:                                 ;   Parent Loop BB2_2 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	x10, x1, x9
	add	x11, x10, #256
	ld2.2d	{ v5, v6 }, [x11]
	add	x11, x0, x9
	ld2.2d	{ v16, v17 }, [x11], #32
	eor.16b	v7, v16, v5
	eor.16b	v5, v17, v6
	; InlineAsm Start
	pmull.1q	v6, v7, v5
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v5, v7, v5
	; InlineAsm End
	eor3.16b	v4, v6, v4, v5
	add	x10, x10, #288
	ld2.2d	{ v5, v6 }, [x10]
	ld2.2d	{ v16, v17 }, [x11]
	eor.16b	v7, v16, v5
	eor.16b	v5, v17, v6
	; InlineAsm Start
	pmull.1q	v6, v7, v5
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v5, v7, v5
	; InlineAsm End
	eor3.16b	v2, v6, v2, v5
	add	x10, x9, #64
	cmp	x9, #192
	mov	x9, x10
	b.lo	LBB2_5
; %bb.6:                                ;   in Loop: Header=BB2_2 Depth=1
	mov	x9, #0                          ; =0x0
	eor.16b	v1, v3, v1
	ext.16b	v3, v1, v1, #8
	eor3.16b	v6, v4, v23, v2
	ext.16b	v2, v6, v6, #8
	; InlineAsm Start
	pmull.1q	v7, v2, v1
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v1, v2, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v2, v7, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v2, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v1, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v4, v3, v0
	; InlineAsm End
	movi.2d	v5, #0000000000000000
	movi.2d	v17, #0000000000000000
LBB2_7:                                 ;   Parent Loop BB2_2 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	x10, x1, x9
	add	x11, x10, #512
	ld2.2d	{ v18, v19 }, [x11]
	add	x11, x0, x9
	ld2.2d	{ v20, v21 }, [x11], #32
	eor.16b	v22, v20, v18
	eor.16b	v18, v21, v19
	; InlineAsm Start
	pmull.1q	v19, v22, v18
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v18, v22, v18
	; InlineAsm End
	eor3.16b	v17, v19, v17, v18
	add	x10, x10, #544
	ld2.2d	{ v18, v19 }, [x10]
	ld2.2d	{ v20, v21 }, [x11]
	eor.16b	v22, v20, v18
	eor.16b	v18, v21, v19
	; InlineAsm Start
	pmull.1q	v19, v22, v18
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v18, v22, v18
	; InlineAsm End
	eor3.16b	v5, v19, v5, v18
	add	x10, x9, #64
	cmp	x9, #192
	mov	x9, x10
	b.lo	LBB2_7
; %bb.8:                                ;   in Loop: Header=BB2_2 Depth=1
	mov	x9, #0                          ; =0x0
	eor3.16b	v7, v6, v16, v7
	eor.16b	v16, v17, v23
	movi.2d	v6, #0000000000000000
	movi.2d	v17, #0000000000000000
LBB2_9:                                 ;   Parent Loop BB2_2 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	x10, x1, x9
	add	x11, x10, #768
	ld2.2d	{ v18, v19 }, [x11]
	add	x11, x0, x9
	ld2.2d	{ v20, v21 }, [x11], #32
	eor.16b	v22, v20, v18
	eor.16b	v18, v21, v19
	; InlineAsm Start
	pmull.1q	v19, v22, v18
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v18, v22, v18
	; InlineAsm End
	eor3.16b	v17, v19, v17, v18
	add	x10, x10, #800
	ld2.2d	{ v18, v19 }, [x10]
	ld2.2d	{ v20, v21 }, [x11]
	eor.16b	v22, v20, v18
	eor.16b	v18, v21, v19
	; InlineAsm Start
	pmull.1q	v19, v22, v18
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v18, v22, v18
	; InlineAsm End
	eor3.16b	v6, v19, v6, v18
	add	x10, x9, #64
	cmp	x9, #192
	mov	x9, x10
	b.lo	LBB2_9
; %bb.10:                               ;   in Loop: Header=BB2_2 Depth=1
	eor.16b	v2, v7, v2
	eor.16b	v5, v16, v5
	eor3.16b	v1, v1, v4, v3
	ext.16b	v3, v5, v5, #8
	eor.16b	v4, v17, v23
	eor3.16b	v7, v17, v23, v6
	ext.16b	v7, v7, v7, #8
	; InlineAsm Start
	pmull.1q	v5, v7, v5
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v3, v7, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v7, v5, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v7, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v3, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v18, v17, v0
	; InlineAsm End
	eor3.16b	v3, v3, v18, v17
	; InlineAsm Start
	pmull.1q	v2, v3, v2
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v1, v3, v1
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v2, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v3, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v18, v1, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v19, v18, v0
	; InlineAsm End
	eor3.16b	v1, v1, v19, v18
	; InlineAsm Start
	pmull.1q	v1, v1, v24
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v18, v1, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v19, v18, v0
	; InlineAsm End
	eor3.16b	v16, v16, v17, v19
	eor3.16b	v4, v4, v6, v16
	eor3.16b	v4, v4, v5, v7
	eor3.16b	v2, v4, v2, v3
	eor3.16b	v24, v2, v1, v18
	add	x1, x1, #1024
	sub	x8, x8, #1024
	cmp	x8, #1024
	b.hi	LBB2_2
; %bb.11:
	cmp	x8, #257
	b.hs	LBB2_13
	b	LBB2_17
LBB2_12:
	mov	x8, x2
	cmp	x2, #257
	b.lo	LBB2_17
LBB2_13:
	ldr	q0, [x19, lCPI2_0@PAGEOFF]
LBB2_14:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB2_15 Depth 2
	mov	x9, #0                          ; =0x0
	movi.2d	v1, #0000000000000000
	movi.2d	v2, #0000000000000000
LBB2_15:                                ;   Parent Loop BB2_14 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	x10, x1, x9
	ld2.2d	{ v3, v4 }, [x10], #32
	add	x11, x0, x9
	ld2.2d	{ v5, v6 }, [x11], #32
	eor.16b	v7, v5, v3
	eor.16b	v3, v6, v4
	; InlineAsm Start
	pmull.1q	v4, v7, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v7, v3
	; InlineAsm End
	eor3.16b	v2, v4, v2, v3
	ld2.2d	{ v3, v4 }, [x10]
	ld2.2d	{ v5, v6 }, [x11]
	eor.16b	v7, v5, v3
	eor.16b	v3, v6, v4
	; InlineAsm Start
	pmull.1q	v4, v7, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v7, v3
	; InlineAsm End
	eor3.16b	v1, v4, v1, v3
	add	x10, x9, #64
	cmp	x9, #192
	mov	x9, x10
	b.lo	LBB2_15
; %bb.16:                               ;   in Loop: Header=BB2_14 Depth=1
	eor.16b	v3, v2, v23
	eor3.16b	v2, v2, v23, v1
	ext.16b	v2, v2, v2, #8
	; InlineAsm Start
	pmull.1q	v2, v2, v24
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v4, v2, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v5, v4, v0
	; InlineAsm End
	eor3.16b	v1, v3, v1, v5
	eor3.16b	v24, v1, v2, v4
	add	x1, x1, #256
	sub	x8, x8, #256
	cmp	x8, #256
	b.hi	LBB2_14
LBB2_17:
	cmp	x8, #256
	b.ne	LBB2_21
; %bb.18:
	mov	x8, #0                          ; =0x0
	movi.2d	v0, #0000000000000000
	movi.2d	v1, #0000000000000000
LBB2_19:                                ; =>This Inner Loop Header: Depth=1
	add	x9, x1, x8
	ld2.2d	{ v2, v3 }, [x9], #32
	add	x10, x0, x8
	ld2.2d	{ v4, v5 }, [x10], #32
	eor.16b	v6, v4, v2
	eor.16b	v2, v5, v3
	; InlineAsm Start
	pmull.1q	v3, v6, v2
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v2, v6, v2
	; InlineAsm End
	eor3.16b	v1, v3, v1, v2
	ld2.2d	{ v2, v3 }, [x9]
	ld2.2d	{ v4, v5 }, [x10]
	eor.16b	v6, v4, v2
	eor.16b	v2, v5, v3
	; InlineAsm Start
	pmull.1q	v3, v6, v2
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v2, v6, v2
	; InlineAsm End
	eor3.16b	v0, v3, v0, v2
	add	x9, x8, #64
	cmp	x8, #192
	mov	x8, x9
	b.lo	LBB2_19
; %bb.20:
	eor.16b	v6, v0, v1
	b	LBB2_31
LBB2_21:
	cmp	x8, #64
	b.lo	LBB2_25
; %bb.22:
	mov	x9, #0                          ; =0x0
	movi.2d	v7, #0000000000000000
	mov	x10, x0
	mov	x11, x1
	movi.2d	v16, #0000000000000000
LBB2_23:                                ; =>This Inner Loop Header: Depth=1
	mov	x12, x11
	ld2.2d	{ v0, v1 }, [x12], #32
	mov	x13, x10
	ld2.2d	{ v2, v3 }, [x13], #32
	eor.16b	v4, v2, v0
	ld2.2d	{ v5, v6 }, [x12]
	eor.16b	v0, v3, v1
	; InlineAsm Start
	pmull.1q	v1, v4, v0
	; InlineAsm End
	ld2.2d	{ v2, v3 }, [x13]
	; InlineAsm Start
	pmull2.1q	v0, v4, v0
	; InlineAsm End
	eor3.16b	v16, v1, v16, v0
	eor.16b	v0, v2, v5
	eor.16b	v1, v3, v6
	; InlineAsm Start
	pmull.1q	v2, v0, v1
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v0, v0, v1
	; InlineAsm End
	eor3.16b	v7, v2, v7, v0
	sub	x9, x9, #64
	add	x11, x11, #64
	add	x10, x10, #64
	add	x12, x8, x9
	cmp	x12, #63
	b.hi	LBB2_23
; %bb.24:
	neg	x20, x9
	mov	x8, x12
	subs	x9, x12, #32
	b.hs	LBB2_26
	b	LBB2_27
LBB2_25:
	mov	x20, #0                         ; =0x0
	movi.2d	v16, #0000000000000000
	movi.2d	v7, #0000000000000000
	subs	x9, x8, #32
	b.lo	LBB2_27
LBB2_26:
	add	x8, x0, x20
	add	x10, x1, x20
	ld2.2d	{ v0, v1 }, [x10]
	ld2.2d	{ v2, v3 }, [x8]
	eor.16b	v4, v2, v0
	eor.16b	v0, v3, v1
	; InlineAsm Start
	pmull.1q	v1, v4, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v0, v4, v0
	; InlineAsm End
	eor.16b	v0, v1, v0
	eor.16b	v16, v0, v16
	orr	x20, x20, #0x20
	mov	x8, x9
LBB2_27:
	subs	x9, x8, #16
	b.hs	LBB2_33
; %bb.28:
	cbz	x8, LBB2_30
LBB2_29:
	stp	xzr, xzr, [x29, #-56]
	mov	x21, x0
	sub	x0, x29, #56
	add	x1, x1, x20
	mov	x22, x2
	mov	x2, x8
	stp	q24, q23, [sp, #32]             ; 32-byte Folded Spill
	stp	q16, q7, [sp]                   ; 32-byte Folded Spill
	bl	_memcpy
	ldp	q16, q7, [sp]                   ; 32-byte Folded Reload
	ldp	q24, q23, [sp, #32]             ; 32-byte Folded Reload
	mov	x0, x21
	mov	x2, x22
	and	x8, x20, #0xfffffffffffffff8
	add	x8, x21, x8
	ldp	d0, d1, [x29, #-56]
	ldp	d2, d3, [x8]
	eor.8b	v0, v2, v0
	eor.8b	v1, v3, v1
	; InlineAsm Start
	pmull.1q	v0, v0, v1
	; InlineAsm End
	eor.16b	v7, v0, v7
LBB2_30:
	eor.16b	v6, v7, v16
LBB2_31:
	ldp	d3, d4, [x0, #280]
	ldp	d1, d2, [x0, #296]
	ldp	d0, d5, [x0, #312]
	ldur	x8, [x29, #-40]
Lloh15:
	adrp	x9, ___stack_chk_guard@GOTPAGE
Lloh16:
	ldr	x9, [x9, ___stack_chk_guard@GOTPAGEOFF]
Lloh17:
	ldr	x9, [x9]
	cmp	x9, x8
	b.ne	LBB2_34
; %bb.32:
	fmov	d7, d23
	dup.2d	v16, x2
	eor.16b	v17, v16, v6
	eor3.16b	v6, v16, v6, v23
	eor3.16b	v16, v17, v23, v7
	ext.16b	v16, v16, v16, #8
	; InlineAsm Start
	pmull.1q	v16, v16, v24
	; InlineAsm End
	ldr	q17, [x19, lCPI2_0@PAGEOFF]
	; InlineAsm Start
	pmull2.1q	v18, v16, v17
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v19, v18, v17
	; InlineAsm End
	eor3.16b	v16, v16, v19, v18
	eor3.16b	v6, v6, v7, v16
	add.2d	v5, v6, v5
	; InlineAsm Start
	pmull.1q	v6, v5, v5
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v7, v6, v17
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v7, v17
	; InlineAsm End
	eor.16b	v6, v6, v16
	eor3.16b	v6, v6, v3, v7
	eor.8b	v3, v3, v4
	eor3.16b	v3, v3, v5, v6
	; InlineAsm Start
	pmull.1q	v3, v6, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v4, v3, v17
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v6, v4, v17
	; InlineAsm End
	eor.16b	v2, v2, v6
	eor3.16b	v2, v2, v3, v4
	eor.16b	v1, v1, v5
	; InlineAsm Start
	pmull.1q	v1, v1, v2
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v2, v1, v17
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v2, v17
	; InlineAsm End
	eor.16b	v0, v0, v3
	eor3.16b	v0, v0, v1, v2
	fmov	x0, d0
	ldp	x29, x30, [sp, #128]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #112]            ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #96]             ; 16-byte Folded Reload
	add	sp, sp, #144
	ret
LBB2_33:
	and	x8, x20, #0xfffffffffffffff8
	add	x8, x0, x8
	add	x10, x1, x20
	ldp	d0, d1, [x10]
	ldp	d2, d3, [x8]
	eor.8b	v0, v2, v0
	eor.8b	v1, v3, v1
	; InlineAsm Start
	pmull.1q	v0, v0, v1
	; InlineAsm End
	eor.16b	v7, v0, v7
	add	x20, x20, #16
	mov	x8, x9
	cbnz	x9, LBB2_29
	b	LBB2_30
LBB2_34:
	bl	___stack_chk_fail
	.loh AdrpLdrGotLdr	Lloh12, Lloh13, Lloh14
	.loh AdrpLdrGotLdr	Lloh15, Lloh16, Lloh17
	.cfi_endproc
                                        ; -- End function
	.section	__TEXT,__literal16,16byte_literals
	.p2align	4, 0x0                          ; -- Begin function audit_batch8
lCPI3_0:
	.byte	27                              ; 0x1b
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	27                              ; 0x1b
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.byte	0                               ; 0x0
	.section	__TEXT,__text,regular,pure_instructions
	.globl	_audit_batch8
	.p2align	2
_audit_batch8:                          ; @audit_batch8
	.cfi_startproc
; %bb.0:
	sub	sp, sp, #144
	stp	x22, x21, [sp, #96]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #112]            ; 16-byte Folded Spill
	stp	x29, x30, [sp, #128]            ; 16-byte Folded Spill
	add	x29, sp, #128
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
Lloh18:
	adrp	x8, ___stack_chk_guard@GOTPAGE
Lloh19:
	ldr	x8, [x8, ___stack_chk_guard@GOTPAGEOFF]
Lloh20:
	ldr	x8, [x8]
	stur	x8, [x29, #-40]
	ldr	q28, [x0, #256]
	ldr	d0, [x0, #272]
	eor.8b	v29, v28, v0
	adrp	x19, lCPI3_0@PAGE
	cmp	x2, #2049
	b.lo	LBB3_20
; %bb.1:
	ldr	q0, [x19, lCPI3_0@PAGEOFF]
	mov	x8, x2
LBB3_2:                                 ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB3_3 Depth 2
                                        ;     Child Loop BB3_5 Depth 2
                                        ;     Child Loop BB3_7 Depth 2
                                        ;     Child Loop BB3_9 Depth 2
                                        ;     Child Loop BB3_11 Depth 2
                                        ;     Child Loop BB3_13 Depth 2
                                        ;     Child Loop BB3_15 Depth 2
                                        ;     Child Loop BB3_17 Depth 2
	mov	x9, #0                          ; =0x0
	movi.2d	v1, #0000000000000000
	movi.2d	v2, #0000000000000000
LBB3_3:                                 ;   Parent Loop BB3_2 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	x10, x1, x9
	ld2.2d	{ v3, v4 }, [x10], #32
	add	x11, x0, x9
	ld2.2d	{ v5, v6 }, [x11], #32
	eor.16b	v7, v5, v3
	eor.16b	v3, v6, v4
	; InlineAsm Start
	pmull.1q	v4, v7, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v7, v3
	; InlineAsm End
	eor3.16b	v2, v4, v2, v3
	ld2.2d	{ v3, v4 }, [x10]
	ld2.2d	{ v5, v6 }, [x11]
	eor.16b	v7, v5, v3
	eor.16b	v3, v6, v4
	; InlineAsm Start
	pmull.1q	v4, v7, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v7, v3
	; InlineAsm End
	eor3.16b	v1, v4, v1, v3
	add	x10, x9, #64
	cmp	x9, #192
	mov	x9, x10
	b.lo	LBB3_3
; %bb.4:                                ;   in Loop: Header=BB3_2 Depth=1
	mov	x9, #0                          ; =0x0
	eor.16b	v3, v2, v28
	movi.2d	v2, #0000000000000000
	movi.2d	v4, #0000000000000000
LBB3_5:                                 ;   Parent Loop BB3_2 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	x10, x1, x9
	add	x11, x10, #256
	ld2.2d	{ v5, v6 }, [x11]
	add	x11, x0, x9
	ld2.2d	{ v16, v17 }, [x11], #32
	eor.16b	v7, v16, v5
	eor.16b	v5, v17, v6
	; InlineAsm Start
	pmull.1q	v6, v7, v5
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v5, v7, v5
	; InlineAsm End
	eor3.16b	v4, v6, v4, v5
	add	x10, x10, #288
	ld2.2d	{ v5, v6 }, [x10]
	ld2.2d	{ v16, v17 }, [x11]
	eor.16b	v7, v16, v5
	eor.16b	v5, v17, v6
	; InlineAsm Start
	pmull.1q	v6, v7, v5
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v5, v7, v5
	; InlineAsm End
	eor3.16b	v2, v6, v2, v5
	add	x10, x9, #64
	cmp	x9, #192
	mov	x9, x10
	b.lo	LBB3_5
; %bb.6:                                ;   in Loop: Header=BB3_2 Depth=1
	mov	x9, #0                          ; =0x0
	eor.16b	v1, v3, v1
	ext.16b	v3, v1, v1, #8
	eor3.16b	v6, v4, v28, v2
	ext.16b	v2, v6, v6, #8
	; InlineAsm Start
	pmull.1q	v7, v2, v1
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v1, v2, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v2, v7, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v2, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v1, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v4, v3, v0
	; InlineAsm End
	movi.2d	v5, #0000000000000000
	movi.2d	v17, #0000000000000000
LBB3_7:                                 ;   Parent Loop BB3_2 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	x10, x1, x9
	add	x11, x10, #512
	ld2.2d	{ v18, v19 }, [x11]
	add	x11, x0, x9
	ld2.2d	{ v20, v21 }, [x11], #32
	eor.16b	v22, v20, v18
	eor.16b	v18, v21, v19
	; InlineAsm Start
	pmull.1q	v19, v22, v18
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v18, v22, v18
	; InlineAsm End
	eor3.16b	v17, v19, v17, v18
	add	x10, x10, #544
	ld2.2d	{ v18, v19 }, [x10]
	ld2.2d	{ v20, v21 }, [x11]
	eor.16b	v22, v20, v18
	eor.16b	v18, v21, v19
	; InlineAsm Start
	pmull.1q	v19, v22, v18
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v18, v22, v18
	; InlineAsm End
	eor3.16b	v5, v19, v5, v18
	add	x10, x9, #64
	cmp	x9, #192
	mov	x9, x10
	b.lo	LBB3_7
; %bb.8:                                ;   in Loop: Header=BB3_2 Depth=1
	mov	x9, #0                          ; =0x0
	eor3.16b	v7, v6, v16, v7
	eor.16b	v16, v17, v28
	movi.2d	v6, #0000000000000000
	movi.2d	v17, #0000000000000000
LBB3_9:                                 ;   Parent Loop BB3_2 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	x10, x1, x9
	add	x11, x10, #768
	ld2.2d	{ v18, v19 }, [x11]
	add	x11, x0, x9
	ld2.2d	{ v20, v21 }, [x11], #32
	eor.16b	v22, v20, v18
	eor.16b	v18, v21, v19
	; InlineAsm Start
	pmull.1q	v19, v22, v18
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v18, v22, v18
	; InlineAsm End
	eor3.16b	v17, v19, v17, v18
	add	x10, x10, #800
	ld2.2d	{ v18, v19 }, [x10]
	ld2.2d	{ v20, v21 }, [x11]
	eor.16b	v22, v20, v18
	eor.16b	v18, v21, v19
	; InlineAsm Start
	pmull.1q	v19, v22, v18
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v18, v22, v18
	; InlineAsm End
	eor3.16b	v6, v19, v6, v18
	add	x10, x9, #64
	cmp	x9, #192
	mov	x9, x10
	b.lo	LBB3_9
; %bb.10:                               ;   in Loop: Header=BB3_2 Depth=1
	mov	x9, #0                          ; =0x0
	eor.16b	v2, v7, v2
	eor.16b	v7, v16, v5
	eor3.16b	v1, v1, v4, v3
	ext.16b	v3, v7, v7, #8
	eor3.16b	v5, v17, v28, v6
	ext.16b	v4, v5, v5, #8
	; InlineAsm Start
	pmull.1q	v7, v4, v7
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v3, v4, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v7, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v16, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v4, v3, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v6, v4, v0
	; InlineAsm End
	eor3.16b	v3, v3, v6, v4
	; InlineAsm Start
	pmull.1q	v18, v3, v2
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v1, v3, v1
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v2, v18, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v19, v2, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v1, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v4, v3, v0
	; InlineAsm End
	movi.2d	v6, #0000000000000000
	movi.2d	v20, #0000000000000000
LBB3_11:                                ;   Parent Loop BB3_2 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	x10, x1, x9
	add	x11, x10, #1024
	ld2.2d	{ v21, v22 }, [x11]
	add	x11, x0, x9
	ld2.2d	{ v23, v24 }, [x11], #32
	eor.16b	v25, v23, v21
	eor.16b	v21, v24, v22
	; InlineAsm Start
	pmull.1q	v22, v25, v21
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v21, v25, v21
	; InlineAsm End
	eor3.16b	v20, v22, v20, v21
	add	x10, x10, #1056
	ld2.2d	{ v21, v22 }, [x10]
	ld2.2d	{ v23, v24 }, [x11]
	eor.16b	v25, v23, v21
	eor.16b	v21, v24, v22
	; InlineAsm Start
	pmull.1q	v22, v25, v21
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v21, v25, v21
	; InlineAsm End
	eor3.16b	v6, v22, v6, v21
	add	x10, x9, #64
	cmp	x9, #192
	mov	x9, x10
	b.lo	LBB3_11
; %bb.12:                               ;   in Loop: Header=BB3_2 Depth=1
	mov	x9, #0                          ; =0x0
	eor.16b	v17, v17, v19
	eor3.16b	v5, v17, v5, v7
	eor3.16b	v5, v5, v16, v18
	eor.16b	v16, v20, v28
	movi.2d	v7, #0000000000000000
	movi.2d	v17, #0000000000000000
LBB3_13:                                ;   Parent Loop BB3_2 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	x10, x1, x9
	add	x11, x10, #1280
	ld2.2d	{ v18, v19 }, [x11]
	add	x11, x0, x9
	ld2.2d	{ v20, v21 }, [x11], #32
	eor.16b	v22, v20, v18
	eor.16b	v18, v21, v19
	; InlineAsm Start
	pmull.1q	v19, v22, v18
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v18, v22, v18
	; InlineAsm End
	eor3.16b	v17, v19, v17, v18
	add	x10, x10, #1312
	ld2.2d	{ v18, v19 }, [x10]
	ld2.2d	{ v20, v21 }, [x11]
	eor.16b	v22, v20, v18
	eor.16b	v18, v21, v19
	; InlineAsm Start
	pmull.1q	v19, v22, v18
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v18, v22, v18
	; InlineAsm End
	eor3.16b	v7, v19, v7, v18
	add	x10, x9, #64
	cmp	x9, #192
	mov	x9, x10
	b.lo	LBB3_13
; %bb.14:                               ;   in Loop: Header=BB3_2 Depth=1
	mov	x9, #0                          ; =0x0
	eor.16b	v6, v16, v6
	ext.16b	v16, v6, v6, #8
	eor3.16b	v19, v17, v28, v7
	ext.16b	v7, v19, v19, #8
	; InlineAsm Start
	pmull.1q	v20, v7, v6
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v6, v7, v16
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v7, v20, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v21, v7, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v6, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v16, v0
	; InlineAsm End
	movi.2d	v18, #0000000000000000
	movi.2d	v22, #0000000000000000
LBB3_15:                                ;   Parent Loop BB3_2 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	x10, x1, x9
	add	x11, x10, #1536
	ld2.2d	{ v23, v24 }, [x11]
	add	x11, x0, x9
	ld2.2d	{ v25, v26 }, [x11], #32
	eor.16b	v27, v25, v23
	eor.16b	v23, v26, v24
	; InlineAsm Start
	pmull.1q	v24, v27, v23
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v23, v27, v23
	; InlineAsm End
	eor3.16b	v22, v24, v22, v23
	add	x10, x10, #1568
	ld2.2d	{ v23, v24 }, [x10]
	ld2.2d	{ v25, v26 }, [x11]
	eor.16b	v27, v25, v23
	eor.16b	v23, v26, v24
	; InlineAsm Start
	pmull.1q	v24, v27, v23
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v23, v27, v23
	; InlineAsm End
	eor3.16b	v18, v24, v18, v23
	add	x10, x9, #64
	cmp	x9, #192
	mov	x9, x10
	b.lo	LBB3_15
; %bb.16:                               ;   in Loop: Header=BB3_2 Depth=1
	mov	x9, #0                          ; =0x0
	eor3.16b	v20, v19, v21, v20
	eor.16b	v21, v22, v28
	movi.2d	v19, #0000000000000000
	movi.2d	v22, #0000000000000000
LBB3_17:                                ;   Parent Loop BB3_2 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	x10, x1, x9
	add	x11, x10, #1792
	ld2.2d	{ v23, v24 }, [x11]
	add	x11, x0, x9
	ld2.2d	{ v25, v26 }, [x11], #32
	eor.16b	v27, v25, v23
	eor.16b	v23, v26, v24
	; InlineAsm Start
	pmull.1q	v24, v27, v23
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v23, v27, v23
	; InlineAsm End
	eor3.16b	v22, v24, v22, v23
	add	x10, x10, #1824
	ld2.2d	{ v23, v24 }, [x10]
	ld2.2d	{ v25, v26 }, [x11]
	eor.16b	v27, v25, v23
	eor.16b	v23, v26, v24
	; InlineAsm Start
	pmull.1q	v24, v27, v23
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v23, v27, v23
	; InlineAsm End
	eor3.16b	v19, v24, v19, v23
	add	x10, x9, #64
	cmp	x9, #192
	mov	x9, x10
	b.lo	LBB3_17
; %bb.18:                               ;   in Loop: Header=BB3_2 Depth=1
	eor.16b	v2, v5, v2
	eor.16b	v5, v20, v7
	eor.16b	v7, v21, v18
	eor3.16b	v1, v1, v4, v3
	eor3.16b	v3, v6, v17, v16
	ext.16b	v4, v7, v7, #8
	eor.16b	v6, v22, v28
	eor3.16b	v16, v22, v28, v19
	ext.16b	v16, v16, v16, #8
	; InlineAsm Start
	pmull.1q	v7, v16, v7
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v4, v16, v4
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v7, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v16, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v18, v4, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v20, v18, v0
	; InlineAsm End
	eor3.16b	v4, v4, v20, v18
	; InlineAsm Start
	pmull.1q	v5, v4, v5
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v3, v4, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v4, v5, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v18, v4, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v20, v3, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v21, v20, v0
	; InlineAsm End
	eor3.16b	v3, v3, v21, v20
	; InlineAsm Start
	pmull.1q	v2, v3, v2
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v1, v3, v1
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v2, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v20, v3, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v21, v1, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v22, v21, v0
	; InlineAsm End
	eor3.16b	v1, v1, v22, v21
	; InlineAsm Start
	pmull.1q	v1, v1, v29
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v21, v1, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v22, v21, v0
	; InlineAsm End
	eor.16b	v17, v17, v18
	eor3.16b	v17, v17, v20, v22
	eor3.16b	v6, v6, v19, v17
	eor3.16b	v6, v6, v7, v16
	eor3.16b	v4, v6, v5, v4
	eor3.16b	v2, v4, v2, v3
	eor3.16b	v29, v2, v1, v21
	add	x1, x1, #2048
	sub	x8, x8, #2048
	cmp	x8, #2048
	b.hi	LBB3_2
; %bb.19:
	cmp	x8, #257
	b.hs	LBB3_21
	b	LBB3_25
LBB3_20:
	mov	x8, x2
	cmp	x2, #257
	b.lo	LBB3_25
LBB3_21:
	ldr	q0, [x19, lCPI3_0@PAGEOFF]
LBB3_22:                                ; =>This Loop Header: Depth=1
                                        ;     Child Loop BB3_23 Depth 2
	mov	x9, #0                          ; =0x0
	movi.2d	v1, #0000000000000000
	movi.2d	v2, #0000000000000000
LBB3_23:                                ;   Parent Loop BB3_22 Depth=1
                                        ; =>  This Inner Loop Header: Depth=2
	add	x10, x1, x9
	ld2.2d	{ v3, v4 }, [x10], #32
	add	x11, x0, x9
	ld2.2d	{ v5, v6 }, [x11], #32
	eor.16b	v7, v5, v3
	eor.16b	v3, v6, v4
	; InlineAsm Start
	pmull.1q	v4, v7, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v7, v3
	; InlineAsm End
	eor3.16b	v2, v4, v2, v3
	ld2.2d	{ v3, v4 }, [x10]
	ld2.2d	{ v5, v6 }, [x11]
	eor.16b	v7, v5, v3
	eor.16b	v3, v6, v4
	; InlineAsm Start
	pmull.1q	v4, v7, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v7, v3
	; InlineAsm End
	eor3.16b	v1, v4, v1, v3
	add	x10, x9, #64
	cmp	x9, #192
	mov	x9, x10
	b.lo	LBB3_23
; %bb.24:                               ;   in Loop: Header=BB3_22 Depth=1
	eor.16b	v3, v2, v28
	eor3.16b	v2, v2, v28, v1
	ext.16b	v2, v2, v2, #8
	; InlineAsm Start
	pmull.1q	v2, v2, v29
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v4, v2, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v5, v4, v0
	; InlineAsm End
	eor3.16b	v1, v3, v1, v5
	eor3.16b	v29, v1, v2, v4
	add	x1, x1, #256
	sub	x8, x8, #256
	cmp	x8, #256
	b.hi	LBB3_22
LBB3_25:
	cmp	x8, #256
	b.ne	LBB3_29
; %bb.26:
	mov	x8, #0                          ; =0x0
	movi.2d	v0, #0000000000000000
	movi.2d	v1, #0000000000000000
LBB3_27:                                ; =>This Inner Loop Header: Depth=1
	add	x9, x1, x8
	ld2.2d	{ v2, v3 }, [x9], #32
	add	x10, x0, x8
	ld2.2d	{ v4, v5 }, [x10], #32
	eor.16b	v6, v4, v2
	eor.16b	v2, v5, v3
	; InlineAsm Start
	pmull.1q	v3, v6, v2
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v2, v6, v2
	; InlineAsm End
	eor3.16b	v1, v3, v1, v2
	ld2.2d	{ v2, v3 }, [x9]
	ld2.2d	{ v4, v5 }, [x10]
	eor.16b	v6, v4, v2
	eor.16b	v2, v5, v3
	; InlineAsm Start
	pmull.1q	v3, v6, v2
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v2, v6, v2
	; InlineAsm End
	eor3.16b	v0, v3, v0, v2
	add	x9, x8, #64
	cmp	x8, #192
	mov	x8, x9
	b.lo	LBB3_27
; %bb.28:
	eor.16b	v6, v0, v1
	b	LBB3_39
LBB3_29:
	cmp	x8, #64
	b.lo	LBB3_33
; %bb.30:
	mov	x9, #0                          ; =0x0
	movi.2d	v7, #0000000000000000
	mov	x10, x0
	mov	x11, x1
	movi.2d	v16, #0000000000000000
LBB3_31:                                ; =>This Inner Loop Header: Depth=1
	mov	x12, x11
	ld2.2d	{ v0, v1 }, [x12], #32
	mov	x13, x10
	ld2.2d	{ v2, v3 }, [x13], #32
	eor.16b	v4, v2, v0
	ld2.2d	{ v5, v6 }, [x12]
	eor.16b	v0, v3, v1
	; InlineAsm Start
	pmull.1q	v1, v4, v0
	; InlineAsm End
	ld2.2d	{ v2, v3 }, [x13]
	; InlineAsm Start
	pmull2.1q	v0, v4, v0
	; InlineAsm End
	eor3.16b	v16, v1, v16, v0
	eor.16b	v0, v2, v5
	eor.16b	v1, v3, v6
	; InlineAsm Start
	pmull.1q	v2, v0, v1
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v0, v0, v1
	; InlineAsm End
	eor3.16b	v7, v2, v7, v0
	sub	x9, x9, #64
	add	x11, x11, #64
	add	x10, x10, #64
	add	x12, x8, x9
	cmp	x12, #63
	b.hi	LBB3_31
; %bb.32:
	neg	x20, x9
	mov	x8, x12
	subs	x9, x12, #32
	b.hs	LBB3_34
	b	LBB3_35
LBB3_33:
	mov	x20, #0                         ; =0x0
	movi.2d	v16, #0000000000000000
	movi.2d	v7, #0000000000000000
	subs	x9, x8, #32
	b.lo	LBB3_35
LBB3_34:
	add	x8, x0, x20
	add	x10, x1, x20
	ld2.2d	{ v0, v1 }, [x10]
	ld2.2d	{ v2, v3 }, [x8]
	eor.16b	v4, v2, v0
	eor.16b	v0, v3, v1
	; InlineAsm Start
	pmull.1q	v1, v4, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v0, v4, v0
	; InlineAsm End
	eor.16b	v0, v1, v0
	eor.16b	v16, v0, v16
	orr	x20, x20, #0x20
	mov	x8, x9
LBB3_35:
	subs	x9, x8, #16
	b.hs	LBB3_41
; %bb.36:
	cbz	x8, LBB3_38
LBB3_37:
	stp	xzr, xzr, [x29, #-56]
	mov	x21, x0
	sub	x0, x29, #56
	add	x1, x1, x20
	mov	x22, x2
	mov	x2, x8
	stp	q29, q28, [sp, #32]             ; 32-byte Folded Spill
	stp	q16, q7, [sp]                   ; 32-byte Folded Spill
	bl	_memcpy
	ldp	q16, q7, [sp]                   ; 32-byte Folded Reload
	ldp	q29, q28, [sp, #32]             ; 32-byte Folded Reload
	mov	x0, x21
	mov	x2, x22
	and	x8, x20, #0xfffffffffffffff8
	add	x8, x21, x8
	ldp	d0, d1, [x29, #-56]
	ldp	d2, d3, [x8]
	eor.8b	v0, v2, v0
	eor.8b	v1, v3, v1
	; InlineAsm Start
	pmull.1q	v0, v0, v1
	; InlineAsm End
	eor.16b	v7, v0, v7
LBB3_38:
	eor.16b	v6, v7, v16
LBB3_39:
	ldp	d3, d4, [x0, #280]
	ldp	d1, d2, [x0, #296]
	ldp	d0, d5, [x0, #312]
	ldur	x8, [x29, #-40]
Lloh21:
	adrp	x9, ___stack_chk_guard@GOTPAGE
Lloh22:
	ldr	x9, [x9, ___stack_chk_guard@GOTPAGEOFF]
Lloh23:
	ldr	x9, [x9]
	cmp	x9, x8
	b.ne	LBB3_42
; %bb.40:
	fmov	d7, d28
	dup.2d	v16, x2
	eor.16b	v17, v16, v6
	eor3.16b	v6, v16, v6, v28
	eor3.16b	v16, v17, v28, v7
	ext.16b	v16, v16, v16, #8
	; InlineAsm Start
	pmull.1q	v16, v16, v29
	; InlineAsm End
	ldr	q17, [x19, lCPI3_0@PAGEOFF]
	; InlineAsm Start
	pmull2.1q	v18, v16, v17
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v19, v18, v17
	; InlineAsm End
	eor3.16b	v16, v16, v19, v18
	eor3.16b	v6, v6, v7, v16
	add.2d	v5, v6, v5
	; InlineAsm Start
	pmull.1q	v6, v5, v5
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v7, v6, v17
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v7, v17
	; InlineAsm End
	eor.16b	v6, v6, v16
	eor3.16b	v6, v6, v3, v7
	eor.8b	v3, v3, v4
	eor3.16b	v3, v3, v5, v6
	; InlineAsm Start
	pmull.1q	v3, v6, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v4, v3, v17
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v6, v4, v17
	; InlineAsm End
	eor.16b	v2, v2, v6
	eor3.16b	v2, v2, v3, v4
	eor.16b	v1, v1, v5
	; InlineAsm Start
	pmull.1q	v1, v1, v2
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v2, v1, v17
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v2, v17
	; InlineAsm End
	eor.16b	v0, v0, v3
	eor3.16b	v0, v0, v1, v2
	fmov	x0, d0
	ldp	x29, x30, [sp, #128]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #112]            ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #96]             ; 16-byte Folded Reload
	add	sp, sp, #144
	ret
LBB3_41:
	and	x8, x20, #0xfffffffffffffff8
	add	x8, x0, x8
	add	x10, x1, x20
	ldp	d0, d1, [x10]
	ldp	d2, d3, [x8]
	eor.8b	v0, v2, v0
	eor.8b	v1, v3, v1
	; InlineAsm Start
	pmull.1q	v0, v0, v1
	; InlineAsm End
	eor.16b	v7, v0, v7
	add	x20, x20, #16
	mov	x8, x9
	cbnz	x9, LBB3_37
	b	LBB3_38
LBB3_42:
	bl	___stack_chk_fail
	.loh AdrpLdrGotLdr	Lloh18, Lloh19, Lloh20
	.loh AdrpLdrGotLdr	Lloh21, Lloh22, Lloh23
	.cfi_endproc
                                        ; -- End function
	.globl	_audit_ph                       ; -- Begin function audit_ph
	.p2align	2
_audit_ph:                              ; @audit_ph
	.cfi_startproc
; %bb.0:
	mov	x8, #0                          ; =0x0
	movi.2d	v0, #0000000000000000
	movi.2d	v1, #0000000000000000
LBB4_1:                                 ; =>This Inner Loop Header: Depth=1
	add	x9, x1, x8
	ld2.2d	{ v2, v3 }, [x9], #32
	add	x10, x0, x8
	ld2.2d	{ v4, v5 }, [x10], #32
	eor.16b	v6, v4, v2
	eor.16b	v2, v5, v3
	; InlineAsm Start
	pmull.1q	v3, v6, v2
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v2, v6, v2
	; InlineAsm End
	eor3.16b	v1, v3, v1, v2
	ld2.2d	{ v2, v3 }, [x9]
	ld2.2d	{ v4, v5 }, [x10]
	eor.16b	v6, v4, v2
	eor.16b	v2, v5, v3
	; InlineAsm Start
	pmull.1q	v3, v6, v2
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v2, v6, v2
	; InlineAsm End
	eor3.16b	v0, v3, v0, v2
	add	x9, x8, #64
	cmp	x8, #192
	mov	x8, x9
	b.lo	LBB4_1
; %bb.2:
	eor.16b	v0, v0, v1
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	_audit_ld2_group                ; -- Begin function audit_ld2_group
	.p2align	2
_audit_ld2_group:                       ; @audit_ld2_group
	.cfi_startproc
; %bb.0:
	ld2.2d	{ v0, v1 }, [x1]
	ld2.2d	{ v2, v3 }, [x0]
	eor.16b	v4, v2, v0
	eor.16b	v0, v3, v1
	; InlineAsm Start
	pmull.1q	v1, v4, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v0, v4, v0
	; InlineAsm End
	eor.16b	v0, v0, v1
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	_audit_strided_group            ; -- Begin function audit_strided_group
	.p2align	2
_audit_strided_group:                   ; @audit_strided_group
	.cfi_startproc
; %bb.0:
	ldp	q0, q1, [x0]
	ldp	q2, q3, [x1]
	eor.16b	v0, v2, v0
	eor.16b	v1, v3, v1
	; InlineAsm Start
	pmull.1q	v2, v0, v1
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v0, v0, v1
	; InlineAsm End
	eor.16b	v0, v0, v2
	ret
	.cfi_endproc
                                        ; -- End function
	.globl	_audit_shuffle_group            ; -- Begin function audit_shuffle_group
	.p2align	2
_audit_shuffle_group:                   ; @audit_shuffle_group
	.cfi_startproc
; %bb.0:
	ldp	q0, q1, [x1]
	ldp	q2, q3, [x0]
	eor.16b	v0, v2, v0
	eor.16b	v1, v3, v1
	zip1.2d	v2, v0, v1
	zip2.2d	v0, v0, v1
	; InlineAsm Start
	pmull.1q	v1, v2, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v0, v2, v0
	; InlineAsm End
	eor.16b	v0, v0, v1
	ret
	.cfi_endproc
                                        ; -- End function
.subsections_via_symbols
