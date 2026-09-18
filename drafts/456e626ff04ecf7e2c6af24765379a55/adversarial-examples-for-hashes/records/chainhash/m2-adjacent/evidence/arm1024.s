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
	ldr	q18, [x0, #1024]
	ldr	d0, [x0, #1040]
	eor.8b	v19, v18, v0
	adrp	x19, lCPI0_0@PAGE
	cmp	x2, #1025
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
	cmp	x9, #960
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
	add	x1, x1, #1024
	sub	x8, x8, #1024
	cmp	x8, #1024
	b.hi	LBB0_2
; %bb.5:
	cmp	x8, #1024
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
	cmp	x8, #960
	mov	x8, x9
	b.lo	LBB0_7
; %bb.8:
	eor.16b	v6, v0, v1
	b	LBB0_20
LBB0_9:
	mov	x8, x2
	cmp	x2, #1024
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
	ldr	d5, [x0, #1088]
	ldr	d3, [x0, #1048]
	ldr	d4, [x0, #1056]
	ldr	d2, [x0, #1072]
	ldr	d1, [x0, #1064]
	ldr	d0, [x0, #1080]
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
	.globl	_audit_ph                       ; -- Begin function audit_ph
	.p2align	2
_audit_ph:                              ; @audit_ph
	.cfi_startproc
; %bb.0:
	mov	x8, #0                          ; =0x0
	movi.2d	v0, #0000000000000000
	movi.2d	v1, #0000000000000000
LBB1_1:                                 ; =>This Inner Loop Header: Depth=1
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
	cmp	x8, #960
	mov	x8, x9
	b.lo	LBB1_1
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
