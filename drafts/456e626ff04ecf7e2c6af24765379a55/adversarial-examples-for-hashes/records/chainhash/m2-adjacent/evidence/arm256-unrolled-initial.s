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
	sub	sp, sp, #160
	stp	d9, d8, [sp, #96]               ; 16-byte Folded Spill
	stp	x22, x21, [sp, #112]            ; 16-byte Folded Spill
	stp	x20, x19, [sp, #128]            ; 16-byte Folded Spill
	stp	x29, x30, [sp, #144]            ; 16-byte Folded Spill
	add	x29, sp, #144
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset b8, -56
	.cfi_offset b9, -64
Lloh0:
	adrp	x8, ___stack_chk_guard@GOTPAGE
Lloh1:
	ldr	x8, [x8, ___stack_chk_guard@GOTPAGEOFF]
Lloh2:
	ldr	x8, [x8]
	stur	x8, [x29, #-56]
	ldr	q8, [x0, #256]
	ldr	d0, [x0, #272]
	eor.8b	v9, v8, v0
	adrp	x19, lCPI0_0@PAGE
	cmp	x2, #257
	b.lo	LBB0_5
; %bb.1:
	mov	x9, x0
	ld2.2d	{ v0, v1 }, [x9], #32
	add	x10, x0, #64
	add	x11, x0, #96
	add	x12, x0, #128
	add	x13, x0, #160
	add	x14, x0, #192
	add	x15, x0, #224
	ldr	q2, [x19, lCPI0_0@PAGEOFF]
	mov	x8, x2
LBB0_2:                                 ; =>This Inner Loop Header: Depth=1
	mov	x16, x1
	ld2.2d	{ v3, v4 }, [x16], #32
	ld2.2d	{ v6, v7 }, [x16]
	eor.16b	v5, v3, v0
	ld2.2d	{ v16, v17 }, [x9]
	eor.16b	v4, v4, v1
	add	x16, x1, #64
	ld2.2d	{ v18, v19 }, [x16]
	; InlineAsm Start
	pmull.1q	v3, v5, v4
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v4, v5, v4
	; InlineAsm End
	ld2.2d	{ v20, v21 }, [x10]
	eor.16b	v22, v20, v18
	add	x16, x1, #96
	ld2.2d	{ v23, v24 }, [x16]
	eor.16b	v18, v21, v19
	ld2.2d	{ v19, v20 }, [x11]
	eor.16b	v21, v19, v23
	; InlineAsm Start
	pmull.1q	v5, v22, v18
	; InlineAsm End
	add	x16, x1, #128
	ld2.2d	{ v25, v26 }, [x16]
	eor.16b	v27, v16, v6
	eor.16b	v19, v20, v24
	ld2.2d	{ v23, v24 }, [x12]
	eor.16b	v6, v17, v7
	add	x16, x1, #160
	ld2.2d	{ v16, v17 }, [x16]
	; InlineAsm Start
	pmull2.1q	v7, v22, v18
	; InlineAsm End
	eor.16b	v18, v23, v25
	ld2.2d	{ v28, v29 }, [x13]
	; InlineAsm Start
	pmull.1q	v20, v21, v19
	; InlineAsm End
	add	x16, x1, #192
	ld2.2d	{ v30, v31 }, [x16]
	; InlineAsm Start
	pmull2.1q	v19, v21, v19
	; InlineAsm End
	eor.16b	v21, v24, v26
	ld2.2d	{ v22, v23 }, [x14]
	; InlineAsm Start
	pmull.1q	v24, v18, v21
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v18, v18, v21
	; InlineAsm End
	eor.16b	v21, v28, v16
	eor.16b	v16, v29, v17
	eor.16b	v17, v22, v30
	eor.16b	v22, v23, v31
	; InlineAsm Start
	pmull.1q	v23, v21, v16
	; InlineAsm End
	add	x16, x1, #224
	ld2.2d	{ v25, v26 }, [x16]
	; InlineAsm Start
	pmull.1q	v28, v27, v6
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v21, v16
	; InlineAsm End
	ld2.2d	{ v29, v30 }, [x15]
	; InlineAsm Start
	pmull2.1q	v6, v27, v6
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v21, v17, v22
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v17, v22
	; InlineAsm End
	eor.16b	v22, v29, v25
	eor.16b	v25, v30, v26
	eor.16b	v3, v3, v4
	; InlineAsm Start
	pmull.1q	v4, v22, v25
	; InlineAsm End
	eor3.16b	v3, v3, v28, v6
	eor3.16b	v3, v3, v5, v7
	eor3.16b	v3, v3, v20, v19
	eor3.16b	v3, v3, v24, v18
	eor3.16b	v3, v3, v23, v16
	; InlineAsm Start
	pmull2.1q	v5, v22, v25
	; InlineAsm End
	eor3.16b	v3, v3, v21, v17
	eor3.16b	v3, v3, v4, v5
	eor.16b	v4, v3, v8
	ext.16b	v4, v4, v4, #8
	; InlineAsm Start
	pmull.1q	v4, v4, v9
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v5, v4, v2
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v6, v5, v2
	; InlineAsm End
	eor3.16b	v4, v4, v6, v5
	eor3.16b	v9, v3, v8, v4
	add	x1, x1, #256
	sub	x8, x8, #256
	cmp	x8, #256
	b.hi	LBB0_2
; %bb.3:
	cmp	x8, #256
	b.ne	LBB0_6
LBB0_4:
	mov	x8, x1
	ld2.2d	{ v0, v1 }, [x8], #32
	mov	x9, x0
	ld2.2d	{ v2, v3 }, [x9], #32
	eor.16b	v4, v2, v0
	eor.16b	v1, v3, v1
	ld2.2d	{ v2, v3 }, [x8]
	; InlineAsm Start
	pmull.1q	v0, v4, v1
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v1, v4, v1
	; InlineAsm End
	ld2.2d	{ v4, v5 }, [x9]
	eor.16b	v6, v4, v2
	add	x8, x1, #64
	ld2.2d	{ v16, v17 }, [x8]
	eor.16b	v3, v5, v3
	add	x8, x0, #64
	ld2.2d	{ v4, v5 }, [x8]
	; InlineAsm Start
	pmull.1q	v2, v6, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v6, v3
	; InlineAsm End
	eor.16b	v6, v4, v16
	eor.16b	v5, v5, v17
	; InlineAsm Start
	pmull.1q	v4, v6, v5
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v5, v6, v5
	; InlineAsm End
	add	x8, x0, #96
	add	x9, x1, #96
	ld2.2d	{ v6, v7 }, [x9]
	ld2.2d	{ v16, v17 }, [x8]
	eor.16b	v18, v16, v6
	eor.16b	v7, v17, v7
	; InlineAsm Start
	pmull.1q	v6, v18, v7
	; InlineAsm End
	add	x8, x1, #128
	ld2.2d	{ v16, v17 }, [x8]
	; InlineAsm Start
	pmull2.1q	v7, v18, v7
	; InlineAsm End
	add	x8, x0, #128
	ld2.2d	{ v18, v19 }, [x8]
	eor.16b	v20, v18, v16
	add	x8, x1, #160
	ld2.2d	{ v21, v22 }, [x8]
	eor.16b	v16, v19, v17
	add	x8, x0, #160
	ld2.2d	{ v17, v18 }, [x8]
	; InlineAsm Start
	pmull.1q	v19, v20, v16
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v20, v16
	; InlineAsm End
	eor.16b	v20, v17, v21
	eor.16b	v17, v18, v22
	; InlineAsm Start
	pmull.1q	v18, v20, v17
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v20, v17
	; InlineAsm End
	add	x8, x0, #192
	add	x9, x1, #192
	ld2.2d	{ v20, v21 }, [x9]
	ld2.2d	{ v22, v23 }, [x8]
	eor.16b	v24, v22, v20
	eor.16b	v20, v23, v21
	; InlineAsm Start
	pmull.1q	v21, v24, v20
	; InlineAsm End
	add	x8, x1, #224
	ld2.2d	{ v22, v23 }, [x8]
	; InlineAsm Start
	pmull2.1q	v20, v24, v20
	; InlineAsm End
	add	x8, x0, #224
	ld2.2d	{ v24, v25 }, [x8]
	eor.16b	v26, v24, v22
	eor.16b	v22, v25, v23
	; InlineAsm Start
	pmull.1q	v23, v26, v22
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v22, v26, v22
	; InlineAsm End
	eor.16b	v0, v1, v0
	eor3.16b	v0, v0, v2, v3
	eor3.16b	v0, v0, v4, v5
	eor3.16b	v0, v0, v6, v7
	eor3.16b	v0, v0, v19, v16
	eor3.16b	v0, v0, v18, v17
	eor3.16b	v0, v0, v21, v20
	eor3.16b	v6, v0, v23, v22
	b	LBB0_16
LBB0_5:
	mov	x8, x2
	cmp	x2, #256
	b.eq	LBB0_4
LBB0_6:
	cmp	x8, #64
	b.lo	LBB0_10
; %bb.7:
	mov	x9, #0                          ; =0x0
	movi.2d	v7, #0000000000000000
	mov	x10, x0
	mov	x11, x1
	movi.2d	v16, #0000000000000000
LBB0_8:                                 ; =>This Inner Loop Header: Depth=1
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
	b.hi	LBB0_8
; %bb.9:
	neg	x20, x9
	mov	x8, x12
	subs	x9, x12, #32
	b.hs	LBB0_11
	b	LBB0_12
LBB0_10:
	mov	x20, #0                         ; =0x0
	movi.2d	v16, #0000000000000000
	movi.2d	v7, #0000000000000000
	subs	x9, x8, #32
	b.lo	LBB0_12
LBB0_11:
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
LBB0_12:
	subs	x9, x8, #16
	b.hs	LBB0_18
; %bb.13:
	cbz	x8, LBB0_15
LBB0_14:
	stp	xzr, xzr, [sp, #72]
	mov	x21, x0
	add	x0, sp, #72
	add	x1, x1, x20
	mov	x22, x2
	mov	x2, x8
	stp	q9, q8, [sp, #32]               ; 32-byte Folded Spill
	stp	q16, q7, [sp]                   ; 32-byte Folded Spill
	bl	_memcpy
	ldp	q16, q7, [sp]                   ; 32-byte Folded Reload
	ldp	q9, q8, [sp, #32]               ; 32-byte Folded Reload
	mov	x0, x21
	mov	x2, x22
	and	x8, x20, #0xfffffffffffffff8
	add	x8, x21, x8
	ldp	d0, d1, [sp, #72]
	ldp	d2, d3, [x8]
	eor.8b	v0, v2, v0
	eor.8b	v1, v3, v1
	; InlineAsm Start
	pmull.1q	v0, v0, v1
	; InlineAsm End
	eor.16b	v7, v0, v7
LBB0_15:
	eor.16b	v6, v7, v16
LBB0_16:
	ldp	d3, d4, [x0, #280]
	ldp	d1, d2, [x0, #296]
	ldp	d0, d5, [x0, #312]
	ldur	x8, [x29, #-56]
Lloh3:
	adrp	x9, ___stack_chk_guard@GOTPAGE
Lloh4:
	ldr	x9, [x9, ___stack_chk_guard@GOTPAGEOFF]
Lloh5:
	ldr	x9, [x9]
	cmp	x9, x8
	b.ne	LBB0_19
; %bb.17:
	fmov	d7, d8
	dup.2d	v16, x2
	eor.16b	v17, v16, v6
	eor3.16b	v6, v16, v6, v8
	eor3.16b	v16, v17, v8, v7
	ext.16b	v16, v16, v16, #8
	; InlineAsm Start
	pmull.1q	v16, v16, v9
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
	ldp	x29, x30, [sp, #144]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #128]            ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #112]            ; 16-byte Folded Reload
	ldp	d9, d8, [sp, #96]               ; 16-byte Folded Reload
	add	sp, sp, #160
	ret
LBB0_18:
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
	cbnz	x9, LBB0_14
	b	LBB0_15
LBB0_19:
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
	sub	sp, sp, #176
	stp	d11, d10, [sp, #96]             ; 16-byte Folded Spill
	stp	d9, d8, [sp, #112]              ; 16-byte Folded Spill
	stp	x22, x21, [sp, #128]            ; 16-byte Folded Spill
	stp	x20, x19, [sp, #144]            ; 16-byte Folded Spill
	stp	x29, x30, [sp, #160]            ; 16-byte Folded Spill
	add	x29, sp, #160
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset b8, -56
	.cfi_offset b9, -64
	.cfi_offset b10, -72
	.cfi_offset b11, -80
Lloh6:
	adrp	x8, ___stack_chk_guard@GOTPAGE
Lloh7:
	ldr	x8, [x8, ___stack_chk_guard@GOTPAGEOFF]
Lloh8:
	ldr	x8, [x8]
	stur	x8, [x29, #-72]
	ldr	q10, [x0, #256]
	ldr	d0, [x0, #272]
	eor.8b	v11, v10, v0
	adrp	x19, lCPI1_0@PAGE
	cmp	x2, #513
	b.lo	LBB1_4
; %bb.1:
	mov	x9, x0
	ld2.2d	{ v0, v1 }, [x9], #32
	add	x10, x0, #64
	add	x11, x0, #96
	add	x12, x0, #128
	add	x13, x0, #160
	add	x14, x0, #192
	add	x15, x0, #224
	ldr	q2, [x19, lCPI1_0@PAGEOFF]
	mov	x8, x2
LBB1_2:                                 ; =>This Inner Loop Header: Depth=1
	mov	x16, x1
	ld2.2d	{ v3, v4 }, [x16], #32
	add	x17, x1, #64
	ld2.2d	{ v5, v6 }, [x17]
	add	x17, x1, #96
	ld2.2d	{ v16, v17 }, [x17]
	add	x17, x1, #128
	ld2.2d	{ v18, v19 }, [x17]
	add	x17, x1, #288
	ld2.2d	{ v20, v21 }, [x17]
	add	x17, x1, #320
	ld2.2d	{ v22, v23 }, [x17]
	add	x17, x1, #352
	ld2.2d	{ v24, v25 }, [x17]
	ld2.2d	{ v26, v27 }, [x9]
	ld2.2d	{ v28, v29 }, [x10]
	eor.16b	v30, v28, v5
	eor.16b	v31, v29, v6
	ld2.2d	{ v8, v9 }, [x11]
	eor.16b	v5, v22, v28
	eor.16b	v6, v23, v29
	eor.16b	v22, v8, v16
	ld2.2d	{ v28, v29 }, [x12]
	eor.16b	v23, v9, v17
	eor.16b	v7, v24, v8
	eor.16b	v16, v25, v9
	ld2.2d	{ v24, v25 }, [x16]
	eor.16b	v8, v28, v18
	eor.16b	v9, v29, v19
	eor.16b	v19, v20, v26
	eor.16b	v20, v21, v27
	eor.16b	v21, v26, v24
	eor.16b	v24, v27, v25
	add	x16, x1, #384
	ld2.2d	{ v25, v26 }, [x16]
	eor.16b	v17, v25, v28
	eor.16b	v18, v26, v29
	eor.16b	v25, v3, v0
	eor.16b	v3, v4, v1
	; InlineAsm Start
	pmull.1q	v4, v25, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v25, v3
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v25, v21, v24
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v26, v30, v31
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v27, v30, v31
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v28, v22, v23
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v22, v22, v23
	; InlineAsm End
	add	x16, x1, #160
	; InlineAsm Start
	pmull2.1q	v21, v21, v24
	; InlineAsm End
	ld2.2d	{ v23, v24 }, [x16]
	eor.16b	v3, v4, v3
	; InlineAsm Start
	pmull.1q	v4, v8, v9
	; InlineAsm End
	eor3.16b	v3, v3, v25, v21
	ld2.2d	{ v29, v30 }, [x13]
	; InlineAsm Start
	pmull2.1q	v21, v8, v9
	; InlineAsm End
	eor.16b	v25, v29, v23
	eor.16b	v23, v30, v24
	; InlineAsm Start
	pmull.1q	v24, v25, v23
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v23, v25, v23
	; InlineAsm End
	add	x16, x1, #416
	eor3.16b	v25, v3, v26, v27
	ld2.2d	{ v26, v27 }, [x16]
	add	x16, x1, #192
	eor.16b	v3, v26, v29
	eor.16b	v26, v27, v30
	ld2.2d	{ v29, v30 }, [x16]
	eor3.16b	v22, v25, v28, v22
	ld2.2d	{ v27, v28 }, [x14]
	add	x16, x1, #224
	eor.16b	v25, v27, v29
	eor3.16b	v4, v22, v4, v21
	ld2.2d	{ v21, v22 }, [x16]
	eor3.16b	v4, v4, v24, v23
	add	x16, x1, #448
	ld2.2d	{ v23, v24 }, [x16]
	eor.16b	v29, v28, v30
	eor.16b	v30, v23, v27
	eor.16b	v23, v24, v28
	ld2.2d	{ v27, v28 }, [x15]
	; InlineAsm Start
	pmull.1q	v24, v25, v29
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v25, v25, v29
	; InlineAsm End
	eor.16b	v29, v27, v21
	eor.16b	v21, v28, v22
	add	x16, x1, #480
	eor3.16b	v4, v4, v24, v25
	ld2.2d	{ v24, v25 }, [x16]
	add	x16, x1, #256
	eor.16b	v22, v24, v27
	eor.16b	v24, v25, v28
	ld2.2d	{ v27, v28 }, [x16]
	; InlineAsm Start
	pmull.1q	v25, v29, v21
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v21, v29, v21
	; InlineAsm End
	eor3.16b	v4, v4, v25, v21
	eor.16b	v21, v27, v0
	eor.16b	v25, v28, v1
	; InlineAsm Start
	pmull.1q	v27, v21, v25
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v21, v21, v25
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v25, v19, v20
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v19, v19, v20
	; InlineAsm End
	eor.16b	v20, v27, v21
	eor3.16b	v19, v20, v25, v19
	; InlineAsm Start
	pmull.1q	v20, v5, v6
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v5, v5, v6
	; InlineAsm End
	eor3.16b	v5, v19, v20, v5
	; InlineAsm Start
	pmull.1q	v6, v7, v16
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v7, v7, v16
	; InlineAsm End
	eor3.16b	v5, v5, v6, v7
	; InlineAsm Start
	pmull.1q	v6, v17, v18
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v7, v17, v18
	; InlineAsm End
	eor3.16b	v5, v5, v6, v7
	; InlineAsm Start
	pmull.1q	v6, v3, v26
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v3, v26
	; InlineAsm End
	eor3.16b	v3, v5, v6, v3
	; InlineAsm Start
	pmull.1q	v5, v30, v23
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v6, v30, v23
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v7, v22, v24
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v22, v24
	; InlineAsm End
	eor.16b	v4, v4, v10
	eor3.16b	v3, v3, v5, v6
	eor3.16b	v3, v3, v7, v16
	ext.16b	v5, v4, v4, #8
	eor.16b	v3, v3, v10
	ext.16b	v6, v3, v3, #8
	; InlineAsm Start
	pmull.1q	v4, v6, v4
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v5, v6, v5
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v6, v5, v2
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v7, v4, v2
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v6, v2
	; InlineAsm End
	eor3.16b	v5, v5, v16, v6
	; InlineAsm Start
	pmull2.1q	v6, v7, v2
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v5, v5, v11
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v5, v2
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v16, v2
	; InlineAsm End
	eor3.16b	v4, v6, v17, v4
	eor3.16b	v3, v4, v3, v7
	eor3.16b	v11, v3, v5, v16
	add	x1, x1, #512
	sub	x8, x8, #512
	cmp	x8, #512
	b.hi	LBB1_2
; %bb.3:
	cmp	x8, #257
	b.hs	LBB1_5
	b	LBB1_7
LBB1_4:
	mov	x8, x2
	cmp	x2, #257
	b.lo	LBB1_7
LBB1_5:
	mov	x9, x0
	ld2.2d	{ v0, v1 }, [x9], #32
	add	x10, x0, #64
	add	x11, x0, #96
	add	x12, x0, #128
	add	x13, x0, #160
	add	x14, x0, #192
	add	x15, x0, #224
	ldr	q2, [x19, lCPI1_0@PAGEOFF]
LBB1_6:                                 ; =>This Inner Loop Header: Depth=1
	mov	x16, x1
	ld2.2d	{ v3, v4 }, [x16], #32
	ld2.2d	{ v6, v7 }, [x16]
	eor.16b	v5, v3, v0
	ld2.2d	{ v16, v17 }, [x9]
	eor.16b	v4, v4, v1
	add	x16, x1, #64
	ld2.2d	{ v18, v19 }, [x16]
	; InlineAsm Start
	pmull.1q	v3, v5, v4
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v4, v5, v4
	; InlineAsm End
	ld2.2d	{ v20, v21 }, [x10]
	eor.16b	v22, v20, v18
	add	x16, x1, #96
	ld2.2d	{ v23, v24 }, [x16]
	eor.16b	v18, v21, v19
	ld2.2d	{ v19, v20 }, [x11]
	eor.16b	v21, v19, v23
	; InlineAsm Start
	pmull.1q	v5, v22, v18
	; InlineAsm End
	add	x16, x1, #128
	ld2.2d	{ v25, v26 }, [x16]
	eor.16b	v27, v16, v6
	eor.16b	v19, v20, v24
	ld2.2d	{ v23, v24 }, [x12]
	eor.16b	v6, v17, v7
	add	x16, x1, #160
	ld2.2d	{ v16, v17 }, [x16]
	; InlineAsm Start
	pmull2.1q	v7, v22, v18
	; InlineAsm End
	eor.16b	v18, v23, v25
	ld2.2d	{ v28, v29 }, [x13]
	; InlineAsm Start
	pmull.1q	v20, v21, v19
	; InlineAsm End
	add	x16, x1, #192
	ld2.2d	{ v30, v31 }, [x16]
	; InlineAsm Start
	pmull2.1q	v19, v21, v19
	; InlineAsm End
	eor.16b	v21, v24, v26
	ld2.2d	{ v22, v23 }, [x14]
	; InlineAsm Start
	pmull.1q	v24, v18, v21
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v18, v18, v21
	; InlineAsm End
	eor.16b	v21, v28, v16
	eor.16b	v16, v29, v17
	eor.16b	v17, v22, v30
	eor.16b	v22, v23, v31
	; InlineAsm Start
	pmull.1q	v23, v21, v16
	; InlineAsm End
	add	x16, x1, #224
	ld2.2d	{ v25, v26 }, [x16]
	; InlineAsm Start
	pmull.1q	v28, v27, v6
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v21, v16
	; InlineAsm End
	ld2.2d	{ v29, v30 }, [x15]
	; InlineAsm Start
	pmull2.1q	v6, v27, v6
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v21, v17, v22
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v17, v22
	; InlineAsm End
	eor.16b	v22, v29, v25
	eor.16b	v25, v30, v26
	eor.16b	v3, v3, v4
	; InlineAsm Start
	pmull.1q	v4, v22, v25
	; InlineAsm End
	eor3.16b	v3, v3, v28, v6
	eor3.16b	v3, v3, v5, v7
	eor3.16b	v3, v3, v20, v19
	eor3.16b	v3, v3, v24, v18
	eor3.16b	v3, v3, v23, v16
	; InlineAsm Start
	pmull2.1q	v5, v22, v25
	; InlineAsm End
	eor3.16b	v3, v3, v21, v17
	eor3.16b	v3, v3, v4, v5
	eor.16b	v4, v3, v10
	ext.16b	v4, v4, v4, #8
	; InlineAsm Start
	pmull.1q	v4, v4, v11
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v5, v4, v2
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v6, v5, v2
	; InlineAsm End
	eor3.16b	v4, v4, v6, v5
	eor3.16b	v11, v3, v10, v4
	add	x1, x1, #256
	sub	x8, x8, #256
	cmp	x8, #256
	b.hi	LBB1_6
LBB1_7:
	cmp	x8, #256
	b.ne	LBB1_9
; %bb.8:
	mov	x8, x1
	ld2.2d	{ v0, v1 }, [x8], #32
	mov	x9, x0
	ld2.2d	{ v2, v3 }, [x9], #32
	eor.16b	v4, v2, v0
	eor.16b	v1, v3, v1
	ld2.2d	{ v2, v3 }, [x8]
	; InlineAsm Start
	pmull.1q	v0, v4, v1
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v1, v4, v1
	; InlineAsm End
	ld2.2d	{ v4, v5 }, [x9]
	eor.16b	v6, v4, v2
	add	x8, x1, #64
	ld2.2d	{ v16, v17 }, [x8]
	eor.16b	v3, v5, v3
	add	x8, x0, #64
	ld2.2d	{ v4, v5 }, [x8]
	; InlineAsm Start
	pmull.1q	v2, v6, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v6, v3
	; InlineAsm End
	eor.16b	v6, v4, v16
	eor.16b	v5, v5, v17
	; InlineAsm Start
	pmull.1q	v4, v6, v5
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v5, v6, v5
	; InlineAsm End
	add	x8, x0, #96
	add	x9, x1, #96
	ld2.2d	{ v6, v7 }, [x9]
	ld2.2d	{ v16, v17 }, [x8]
	eor.16b	v18, v16, v6
	eor.16b	v7, v17, v7
	; InlineAsm Start
	pmull.1q	v6, v18, v7
	; InlineAsm End
	add	x8, x1, #128
	ld2.2d	{ v16, v17 }, [x8]
	; InlineAsm Start
	pmull2.1q	v7, v18, v7
	; InlineAsm End
	add	x8, x0, #128
	ld2.2d	{ v18, v19 }, [x8]
	eor.16b	v20, v18, v16
	add	x8, x1, #160
	ld2.2d	{ v21, v22 }, [x8]
	eor.16b	v16, v19, v17
	add	x8, x0, #160
	ld2.2d	{ v17, v18 }, [x8]
	; InlineAsm Start
	pmull.1q	v19, v20, v16
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v20, v16
	; InlineAsm End
	eor.16b	v20, v17, v21
	eor.16b	v17, v18, v22
	; InlineAsm Start
	pmull.1q	v18, v20, v17
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v20, v17
	; InlineAsm End
	add	x8, x0, #192
	add	x9, x1, #192
	ld2.2d	{ v20, v21 }, [x9]
	ld2.2d	{ v22, v23 }, [x8]
	eor.16b	v24, v22, v20
	eor.16b	v20, v23, v21
	; InlineAsm Start
	pmull.1q	v21, v24, v20
	; InlineAsm End
	add	x8, x1, #224
	ld2.2d	{ v22, v23 }, [x8]
	; InlineAsm Start
	pmull2.1q	v20, v24, v20
	; InlineAsm End
	add	x8, x0, #224
	ld2.2d	{ v24, v25 }, [x8]
	eor.16b	v26, v24, v22
	eor.16b	v22, v25, v23
	; InlineAsm Start
	pmull.1q	v23, v26, v22
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v22, v26, v22
	; InlineAsm End
	eor.16b	v0, v1, v0
	eor3.16b	v0, v0, v2, v3
	eor3.16b	v0, v0, v4, v5
	eor3.16b	v0, v0, v6, v7
	eor3.16b	v0, v0, v19, v16
	eor3.16b	v0, v0, v18, v17
	eor3.16b	v0, v0, v21, v20
	eor3.16b	v6, v0, v23, v22
	b	LBB1_19
LBB1_9:
	cmp	x8, #64
	b.lo	LBB1_13
; %bb.10:
	mov	x9, #0                          ; =0x0
	movi.2d	v7, #0000000000000000
	mov	x10, x0
	mov	x11, x1
	movi.2d	v16, #0000000000000000
LBB1_11:                                ; =>This Inner Loop Header: Depth=1
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
	b.hi	LBB1_11
; %bb.12:
	neg	x20, x9
	mov	x8, x12
	subs	x9, x12, #32
	b.hs	LBB1_14
	b	LBB1_15
LBB1_13:
	mov	x20, #0                         ; =0x0
	movi.2d	v16, #0000000000000000
	movi.2d	v7, #0000000000000000
	subs	x9, x8, #32
	b.lo	LBB1_15
LBB1_14:
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
LBB1_15:
	subs	x9, x8, #16
	b.hs	LBB1_21
; %bb.16:
	cbz	x8, LBB1_18
LBB1_17:
	stp	xzr, xzr, [sp, #72]
	mov	x21, x0
	add	x0, sp, #72
	add	x1, x1, x20
	mov	x22, x2
	mov	x2, x8
	stp	q11, q10, [sp, #32]             ; 32-byte Folded Spill
	stp	q16, q7, [sp]                   ; 32-byte Folded Spill
	bl	_memcpy
	ldp	q16, q7, [sp]                   ; 32-byte Folded Reload
	ldp	q11, q10, [sp, #32]             ; 32-byte Folded Reload
	mov	x0, x21
	mov	x2, x22
	and	x8, x20, #0xfffffffffffffff8
	add	x8, x21, x8
	ldp	d0, d1, [sp, #72]
	ldp	d2, d3, [x8]
	eor.8b	v0, v2, v0
	eor.8b	v1, v3, v1
	; InlineAsm Start
	pmull.1q	v0, v0, v1
	; InlineAsm End
	eor.16b	v7, v0, v7
LBB1_18:
	eor.16b	v6, v7, v16
LBB1_19:
	ldp	d3, d4, [x0, #280]
	ldp	d1, d2, [x0, #296]
	ldp	d0, d5, [x0, #312]
	ldur	x8, [x29, #-72]
Lloh9:
	adrp	x9, ___stack_chk_guard@GOTPAGE
Lloh10:
	ldr	x9, [x9, ___stack_chk_guard@GOTPAGEOFF]
Lloh11:
	ldr	x9, [x9]
	cmp	x9, x8
	b.ne	LBB1_22
; %bb.20:
	fmov	d7, d10
	dup.2d	v16, x2
	eor.16b	v17, v16, v6
	eor3.16b	v6, v16, v6, v10
	eor3.16b	v16, v17, v10, v7
	ext.16b	v16, v16, v16, #8
	; InlineAsm Start
	pmull.1q	v16, v16, v11
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
	ldp	x29, x30, [sp, #160]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #144]            ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #128]            ; 16-byte Folded Reload
	ldp	d9, d8, [sp, #112]              ; 16-byte Folded Reload
	ldp	d11, d10, [sp, #96]             ; 16-byte Folded Reload
	add	sp, sp, #176
	ret
LBB1_21:
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
	cbnz	x9, LBB1_17
	b	LBB1_18
LBB1_22:
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
	stp	d15, d14, [sp, #-128]!          ; 16-byte Folded Spill
	stp	d13, d12, [sp, #16]             ; 16-byte Folded Spill
	stp	d11, d10, [sp, #32]             ; 16-byte Folded Spill
	stp	d9, d8, [sp, #48]               ; 16-byte Folded Spill
	stp	x28, x27, [sp, #64]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #80]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #96]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #112]            ; 16-byte Folded Spill
	add	x29, sp, #112
	sub	sp, sp, #640
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w27, -56
	.cfi_offset w28, -64
	.cfi_offset b8, -72
	.cfi_offset b9, -80
	.cfi_offset b10, -88
	.cfi_offset b11, -96
	.cfi_offset b12, -104
	.cfi_offset b13, -112
	.cfi_offset b14, -120
	.cfi_offset b15, -128
Lloh12:
	adrp	x8, ___stack_chk_guard@GOTPAGE
Lloh13:
	ldr	x8, [x8, ___stack_chk_guard@GOTPAGEOFF]
Lloh14:
	ldr	x8, [x8]
	stur	x8, [x29, #-120]
	ldr	q8, [x0, #256]
	ldr	d0, [x0, #272]
	eor.8b	v9, v8, v0
	adrp	x19, lCPI2_0@PAGE
	cmp	x2, #1025
	b.lo	LBB2_4
; %bb.1:
	mov	x9, x0
	ld2.2d	{ v0, v1 }, [x9], #32
	mov	x8, sp
	st1.2d	{ v0, v1 }, [x8]                ; 32-byte Folded Spill
	add	x10, x0, #64
	add	x11, x0, #96
	add	x12, x0, #128
	add	x13, x0, #160
	add	x14, x0, #192
	add	x15, x0, #224
	ldr	q20, [x19, lCPI2_0@PAGEOFF]
	mov	x8, x2
	str	q8, [sp, #32]                   ; 16-byte Folded Spill
LBB2_2:                                 ; =>This Inner Loop Header: Depth=1
	stur	q9, [x29, #-160]                ; 16-byte Folded Spill
	mov	x16, x1
	ld2.2d	{ v0, v1 }, [x16], #32
	sub	x17, x29, #192
	st1.2d	{ v0, v1 }, [x17]               ; 32-byte Folded Spill
	add	x17, x1, #64
	ld2.2d	{ v18, v19 }, [x17]
	add	x17, x1, #96
	ld2.2d	{ v5, v6 }, [x17]
	add	x17, x1, #128
	ld2.2d	{ v27, v28 }, [x17]
	add	x17, x1, #288
	ld2.2d	{ v16, v17 }, [x17]
	add	x17, x1, #320
	ld2.2d	{ v22, v23 }, [x17]
	add	x17, x1, #352
	ld2.2d	{ v25, v26 }, [x17]
	add	x17, x1, #544
	ld2.2d	{ v3, v4 }, [x17]
	add	x17, x1, #576
	ld2.2d	{ v0, v1 }, [x17]
	add	x17, x1, #608
	ld2.2d	{ v12, v13 }, [x17]
	add	x17, x1, #800
	ld2.2d	{ v29, v30 }, [x17]
	add	x17, x1, #832
	ld2.2d	{ v7, v8 }, [x17]
	ld2.2d	{ v9, v10 }, [x10]
	ld2.2d	{ v14, v15 }, [x11]
	eor.16b	v2, v9, v18
	str	q2, [sp, #96]                   ; 16-byte Folded Spill
	eor.16b	v2, v10, v19
	str	q2, [sp, #80]                   ; 16-byte Folded Spill
	eor.16b	v2, v14, v5
	str	q2, [sp, #128]                  ; 16-byte Folded Spill
	eor.16b	v2, v15, v6
	str	q2, [sp, #112]                  ; 16-byte Folded Spill
	ld2.2d	{ v5, v6 }, [x9]
	eor.16b	v2, v16, v5
	str	q2, [sp, #224]                  ; 16-byte Folded Spill
	eor.16b	v2, v17, v6
	str	q2, [sp, #208]                  ; 16-byte Folded Spill
	eor.16b	v2, v22, v9
	str	q2, [sp, #256]                  ; 16-byte Folded Spill
	eor.16b	v2, v23, v10
	str	q2, [sp, #240]                  ; 16-byte Folded Spill
	eor.16b	v2, v0, v9
	str	q2, [sp, #448]                  ; 16-byte Folded Spill
	eor.16b	v0, v1, v10
	str	q0, [sp, #416]                  ; 16-byte Folded Spill
	eor.16b	v2, v7, v9
	eor.16b	v0, v8, v10
	stp	q0, q2, [x29, #-224]            ; 32-byte Folded Spill
	eor.16b	v7, v3, v5
	ld2.2d	{ v0, v1 }, [x16]
	eor.16b	v2, v4, v6
	stp	q2, q7, [sp, #336]              ; 32-byte Folded Spill
	eor.16b	v2, v29, v5
	stur	q2, [x29, #-256]                ; 16-byte Folded Spill
	eor.16b	v2, v30, v6
	str	q2, [sp, #480]                  ; 16-byte Folded Spill
	add	x16, x1, #864
	eor.16b	v22, v5, v0
	eor.16b	v24, v6, v1
	eor.16b	v2, v25, v14
	ld2.2d	{ v4, v5 }, [x12]
	eor.16b	v0, v26, v15
	stp	q0, q2, [sp, #48]               ; 32-byte Folded Spill
	eor.16b	v2, v12, v14
	eor.16b	v0, v13, v15
	stp	q0, q2, [sp, #304]              ; 32-byte Folded Spill
	ld2.2d	{ v0, v1 }, [x16]
	add	x16, x1, #384
	eor.16b	v2, v0, v14
	stur	q2, [x29, #-240]                ; 16-byte Folded Spill
	ld2.2d	{ v29, v30 }, [x16]
	eor.16b	v0, v1, v15
	str	q0, [sp, #464]                  ; 16-byte Folded Spill
	eor.16b	v25, v4, v27
	eor.16b	v27, v5, v28
	eor.16b	v7, v29, v4
	eor.16b	v18, v30, v5
	add	x16, x1, #640
	ld2.2d	{ v28, v29 }, [x16]
	eor.16b	v2, v28, v4
	eor.16b	v0, v29, v5
	stp	q0, q2, [sp, #144]              ; 32-byte Folded Spill
	add	x16, x1, #896
	ld2.2d	{ v28, v29 }, [x16]
	eor.16b	v0, v28, v4
	str	q0, [sp, #432]                  ; 16-byte Folded Spill
	eor.16b	v2, v29, v5
	add	x16, x1, #160
	ld2.2d	{ v4, v5 }, [x16]
	ld2.2d	{ v28, v29 }, [x13]
	eor.16b	v0, v28, v4
	stp	q0, q2, [sp, #384]              ; 32-byte Folded Spill
	eor.16b	v0, v29, v5
	str	q0, [sp, #368]                  ; 16-byte Folded Spill
	add	x16, x1, #416
	ld2.2d	{ v4, v5 }, [x16]
	eor.16b	v30, v4, v28
	eor.16b	v8, v5, v29
	add	x16, x1, #672
	ld2.2d	{ v5, v6 }, [x16]
	add	x16, x1, #192
	eor.16b	v19, v5, v28
	ld2.2d	{ v9, v10 }, [x16]
	eor.16b	v4, v6, v29
	add	x16, x1, #928
	ld2.2d	{ v5, v6 }, [x16]
	eor.16b	v2, v5, v28
	eor.16b	v0, v6, v29
	stp	q0, q2, [sp, #272]              ; 32-byte Folded Spill
	ld2.2d	{ v5, v6 }, [x14]
	add	x16, x1, #224
	eor.16b	v2, v5, v9
	ld2.2d	{ v28, v29 }, [x16]
	eor.16b	v0, v6, v10
	stp	q0, q2, [sp, #176]              ; 32-byte Folded Spill
	add	x16, x1, #448
	ld2.2d	{ v9, v10 }, [x16]
	add	x16, x1, #704
	eor.16b	v11, v9, v5
	ld2.2d	{ v15, v16 }, [x16]
	eor.16b	v14, v10, v6
	eor.16b	v9, v15, v5
	ld2.2d	{ v0, v1 }, [x15]
	eor.16b	v10, v16, v6
	add	x16, x1, #960
	ld2.2d	{ v16, v17 }, [x16]
	add	x16, x1, #480
	eor.16b	v15, v16, v5
	ld2.2d	{ v2, v3 }, [x16]
	eor.16b	v13, v17, v6
	eor.16b	v21, v0, v28
	eor.16b	v12, v1, v29
	eor.16b	v29, v2, v0
	eor.16b	v31, v3, v1
	add	x16, x1, #736
	ld2.2d	{ v2, v3 }, [x16]
	eor.16b	v26, v2, v0
	eor.16b	v28, v3, v1
	add	x16, x1, #992
	ld2.2d	{ v16, v17 }, [x16]
	eor.16b	v2, v16, v0
	eor.16b	v23, v17, v1
	mov	x16, sp
	ld1.2d	{ v5, v6 }, [x16]               ; 32-byte Folded Reload
	sub	x16, x29, #192
	ld1.2d	{ v16, v17 }, [x16]             ; 32-byte Folded Reload
	eor.16b	v0, v16, v5
	eor.16b	v1, v17, v6
	; InlineAsm Start
	pmull.1q	v3, v0, v1
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v0, v0, v1
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v1, v22, v24
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v22, v24
	; InlineAsm End
	eor.16b	v0, v3, v0
	eor3.16b	v0, v0, v1, v16
	ldp	q16, q3, [sp, #80]              ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v1, v3, v16
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v3, v16
	; InlineAsm End
	eor3.16b	v0, v0, v1, v3
	ldp	q16, q3, [sp, #112]             ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v1, v3, v16
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v3, v16
	; InlineAsm End
	eor3.16b	v0, v0, v1, v3
	; InlineAsm Start
	pmull.1q	v1, v25, v27
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v25, v27
	; InlineAsm End
	eor3.16b	v0, v0, v1, v3
	add	x16, x1, #256
	ld2.2d	{ v16, v17 }, [x16]
	eor.16b	v1, v16, v5
	eor.16b	v3, v17, v6
	; InlineAsm Start
	pmull.1q	v16, v1, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v1, v1, v3
	; InlineAsm End
	ldp	q22, q17, [sp, #208]            ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v3, v17, v22
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v17, v22
	; InlineAsm End
	eor.16b	v1, v16, v1
	eor3.16b	v1, v1, v3, v17
	ldp	q17, q16, [sp, #240]            ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v3, v16, v17
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v16, v17
	; InlineAsm End
	eor3.16b	v1, v1, v3, v16
	ldp	q17, q16, [sp, #48]             ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v3, v16, v17
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v16, v17
	; InlineAsm End
	eor3.16b	v1, v1, v3, v16
	; InlineAsm Start
	pmull.1q	v3, v7, v18
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v7, v7, v18
	; InlineAsm End
	eor3.16b	v1, v1, v3, v7
	; InlineAsm Start
	pmull.1q	v3, v30, v8
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v7, v30, v8
	; InlineAsm End
	ldr	q8, [sp, #32]                   ; 16-byte Folded Reload
	eor3.16b	v1, v1, v3, v7
	add	x16, x1, #512
	ld2.2d	{ v16, v17 }, [x16]
	; InlineAsm Start
	pmull.1q	v3, v11, v14
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v7, v11, v14
	; InlineAsm End
	eor3.16b	v1, v1, v3, v7
	eor.16b	v3, v16, v5
	eor.16b	v7, v17, v6
	; InlineAsm Start
	pmull.1q	v16, v3, v7
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v3, v7
	; InlineAsm End
	ldp	q18, q17, [sp, #336]            ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v7, v17, v18
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v17, v18
	; InlineAsm End
	eor.16b	v3, v16, v3
	eor3.16b	v3, v3, v7, v17
	ldr	q16, [sp, #448]                 ; 16-byte Folded Reload
	ldr	q17, [sp, #416]                 ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v7, v16, v17
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v16, v17
	; InlineAsm End
	eor3.16b	v3, v3, v7, v16
	ldp	q17, q16, [sp, #304]            ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v7, v16, v17
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v16, v17
	; InlineAsm End
	eor3.16b	v3, v3, v7, v16
	ldp	q17, q16, [sp, #144]            ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v7, v16, v17
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v16, v17
	; InlineAsm End
	eor3.16b	v3, v3, v7, v16
	; InlineAsm Start
	pmull.1q	v7, v19, v4
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v4, v19, v4
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v16, v9, v10
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v9, v10
	; InlineAsm End
	add	x16, x1, #768
	eor3.16b	v3, v3, v7, v4
	ld2.2d	{ v18, v19 }, [x16]
	eor3.16b	v3, v3, v16, v17
	eor.16b	v4, v18, v5
	eor.16b	v7, v19, v6
	; InlineAsm Start
	pmull.1q	v16, v4, v7
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v4, v4, v7
	; InlineAsm End
	ldur	q5, [x29, #-256]                ; 16-byte Folded Reload
	ldr	q6, [sp, #480]                  ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v7, v5, v6
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v5, v6
	; InlineAsm End
	eor.16b	v4, v16, v4
	eor3.16b	v4, v4, v7, v17
	; InlineAsm Start
	pmull.1q	v7, v29, v31
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v29, v31
	; InlineAsm End
	eor3.16b	v1, v1, v7, v16
	ldp	q6, q5, [x29, #-224]            ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v7, v5, v6
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v5, v6
	; InlineAsm End
	eor3.16b	v4, v4, v7, v16
	; InlineAsm Start
	pmull.1q	v7, v26, v28
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v26, v28
	; InlineAsm End
	eor3.16b	v3, v3, v7, v16
	ldur	q5, [x29, #-240]                ; 16-byte Folded Reload
	ldr	q6, [sp, #464]                  ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v7, v5, v6
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v5, v6
	; InlineAsm End
	eor3.16b	v4, v4, v7, v16
	ldp	q6, q5, [sp, #368]              ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v7, v5, v6
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v5, v6
	; InlineAsm End
	eor3.16b	v0, v0, v7, v16
	ldr	q5, [sp, #432]                  ; 16-byte Folded Reload
	ldr	q6, [sp, #400]                  ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v7, v5, v6
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v5, v6
	; InlineAsm End
	eor3.16b	v4, v4, v7, v16
	ldp	q6, q5, [sp, #176]              ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v7, v5, v6
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v5, v6
	; InlineAsm End
	eor3.16b	v0, v0, v7, v16
	ldp	q6, q5, [sp, #272]              ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v7, v5, v6
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v5, v6
	; InlineAsm End
	eor3.16b	v4, v4, v7, v16
	; InlineAsm Start
	pmull.1q	v7, v21, v12
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v5, v21, v12
	; InlineAsm End
	eor3.16b	v0, v0, v7, v5
	; InlineAsm Start
	pmull.1q	v5, v15, v13
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v6, v15, v13
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v7, v2, v23
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v2, v2, v23
	; InlineAsm End
	eor.16b	v1, v1, v8
	eor.16b	v0, v0, v8
	eor3.16b	v4, v4, v5, v6
	ext.16b	v5, v1, v1, #8
	; InlineAsm Start
	pmull.1q	v6, v5, v0
	; InlineAsm End
	ext.16b	v0, v0, v0, #8
	eor.16b	v3, v3, v8
	; InlineAsm Start
	pmull.1q	v0, v5, v0
	; InlineAsm End
	eor3.16b	v2, v4, v7, v2
	eor.16b	v2, v2, v8
	ext.16b	v4, v2, v2, #8
	; InlineAsm Start
	pmull.1q	v5, v4, v3
	; InlineAsm End
	ext.16b	v3, v3, v3, #8
	; InlineAsm Start
	pmull2.1q	v7, v6, v20
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v3, v4, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v4, v7, v20
	; InlineAsm End
	eor.16b	v4, v6, v4
	; InlineAsm Start
	pmull2.1q	v6, v0, v20
	; InlineAsm End
	eor3.16b	v1, v4, v1, v7
	; InlineAsm Start
	pmull2.1q	v4, v6, v20
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v7, v5, v20
	; InlineAsm End
	eor3.16b	v0, v0, v4, v6
	; InlineAsm Start
	pmull2.1q	v4, v3, v20
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v6, v4, v20
	; InlineAsm End
	eor3.16b	v3, v3, v6, v4
	; InlineAsm Start
	pmull.1q	v1, v3, v1
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v0, v3, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v1, v20
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v4, v0, v20
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v6, v4, v20
	; InlineAsm End
	eor3.16b	v0, v0, v6, v4
	; InlineAsm Start
	pmull2.1q	v4, v7, v20
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v6, v3, v20
	; InlineAsm End
	ldur	q16, [x29, #-160]               ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v0, v0, v16
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v0, v20
	; InlineAsm End
	eor.16b	v4, v4, v6
	; InlineAsm Start
	pmull2.1q	v6, v16, v20
	; InlineAsm End
	eor3.16b	v4, v4, v6, v5
	eor3.16b	v2, v4, v2, v7
	eor3.16b	v1, v2, v1, v3
	eor3.16b	v9, v1, v0, v16
	add	x1, x1, #1024
	sub	x8, x8, #1024
	cmp	x8, #1024
	b.hi	LBB2_2
; %bb.3:
	cmp	x8, #257
	b.hs	LBB2_5
	b	LBB2_7
LBB2_4:
	mov	x8, x2
	cmp	x2, #257
	b.lo	LBB2_7
LBB2_5:
	mov	x9, x0
	ld2.2d	{ v0, v1 }, [x9], #32
	add	x10, x0, #64
	add	x11, x0, #96
	add	x12, x0, #128
	add	x13, x0, #160
	add	x14, x0, #192
	add	x15, x0, #224
	ldr	q2, [x19, lCPI2_0@PAGEOFF]
LBB2_6:                                 ; =>This Inner Loop Header: Depth=1
	mov	x16, x1
	ld2.2d	{ v3, v4 }, [x16], #32
	ld2.2d	{ v6, v7 }, [x16]
	eor.16b	v5, v3, v0
	ld2.2d	{ v16, v17 }, [x9]
	eor.16b	v4, v4, v1
	add	x16, x1, #64
	ld2.2d	{ v18, v19 }, [x16]
	; InlineAsm Start
	pmull.1q	v3, v5, v4
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v4, v5, v4
	; InlineAsm End
	ld2.2d	{ v20, v21 }, [x10]
	eor.16b	v22, v20, v18
	add	x16, x1, #96
	ld2.2d	{ v23, v24 }, [x16]
	eor.16b	v18, v21, v19
	ld2.2d	{ v19, v20 }, [x11]
	eor.16b	v21, v19, v23
	; InlineAsm Start
	pmull.1q	v5, v22, v18
	; InlineAsm End
	add	x16, x1, #128
	ld2.2d	{ v25, v26 }, [x16]
	eor.16b	v27, v16, v6
	eor.16b	v19, v20, v24
	ld2.2d	{ v23, v24 }, [x12]
	eor.16b	v6, v17, v7
	add	x16, x1, #160
	ld2.2d	{ v16, v17 }, [x16]
	; InlineAsm Start
	pmull2.1q	v7, v22, v18
	; InlineAsm End
	eor.16b	v18, v23, v25
	ld2.2d	{ v28, v29 }, [x13]
	; InlineAsm Start
	pmull.1q	v20, v21, v19
	; InlineAsm End
	add	x16, x1, #192
	ld2.2d	{ v30, v31 }, [x16]
	; InlineAsm Start
	pmull2.1q	v19, v21, v19
	; InlineAsm End
	eor.16b	v21, v24, v26
	ld2.2d	{ v22, v23 }, [x14]
	; InlineAsm Start
	pmull.1q	v24, v18, v21
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v18, v18, v21
	; InlineAsm End
	eor.16b	v21, v28, v16
	eor.16b	v16, v29, v17
	eor.16b	v17, v22, v30
	eor.16b	v22, v23, v31
	; InlineAsm Start
	pmull.1q	v23, v21, v16
	; InlineAsm End
	add	x16, x1, #224
	ld2.2d	{ v25, v26 }, [x16]
	; InlineAsm Start
	pmull.1q	v28, v27, v6
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v21, v16
	; InlineAsm End
	ld2.2d	{ v29, v30 }, [x15]
	; InlineAsm Start
	pmull2.1q	v6, v27, v6
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v21, v17, v22
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v17, v22
	; InlineAsm End
	eor.16b	v22, v29, v25
	eor.16b	v25, v30, v26
	eor.16b	v3, v3, v4
	; InlineAsm Start
	pmull.1q	v4, v22, v25
	; InlineAsm End
	eor3.16b	v3, v3, v28, v6
	eor3.16b	v3, v3, v5, v7
	eor3.16b	v3, v3, v20, v19
	eor3.16b	v3, v3, v24, v18
	eor3.16b	v3, v3, v23, v16
	; InlineAsm Start
	pmull2.1q	v5, v22, v25
	; InlineAsm End
	eor3.16b	v3, v3, v21, v17
	eor3.16b	v3, v3, v4, v5
	eor.16b	v4, v3, v8
	ext.16b	v4, v4, v4, #8
	; InlineAsm Start
	pmull.1q	v4, v4, v9
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v5, v4, v2
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v6, v5, v2
	; InlineAsm End
	eor3.16b	v4, v4, v6, v5
	eor3.16b	v9, v3, v8, v4
	add	x1, x1, #256
	sub	x8, x8, #256
	cmp	x8, #256
	b.hi	LBB2_6
LBB2_7:
	cmp	x8, #256
	b.ne	LBB2_9
; %bb.8:
	mov	x8, x1
	ld2.2d	{ v0, v1 }, [x8], #32
	mov	x9, x0
	ld2.2d	{ v2, v3 }, [x9], #32
	eor.16b	v4, v2, v0
	eor.16b	v1, v3, v1
	ld2.2d	{ v2, v3 }, [x8]
	; InlineAsm Start
	pmull.1q	v0, v4, v1
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v1, v4, v1
	; InlineAsm End
	ld2.2d	{ v4, v5 }, [x9]
	eor.16b	v6, v4, v2
	add	x8, x1, #64
	ld2.2d	{ v16, v17 }, [x8]
	eor.16b	v3, v5, v3
	add	x8, x0, #64
	ld2.2d	{ v4, v5 }, [x8]
	; InlineAsm Start
	pmull.1q	v2, v6, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v6, v3
	; InlineAsm End
	eor.16b	v6, v4, v16
	eor.16b	v5, v5, v17
	; InlineAsm Start
	pmull.1q	v4, v6, v5
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v5, v6, v5
	; InlineAsm End
	add	x8, x0, #96
	add	x9, x1, #96
	ld2.2d	{ v6, v7 }, [x9]
	ld2.2d	{ v16, v17 }, [x8]
	eor.16b	v18, v16, v6
	eor.16b	v7, v17, v7
	; InlineAsm Start
	pmull.1q	v6, v18, v7
	; InlineAsm End
	add	x8, x1, #128
	ld2.2d	{ v16, v17 }, [x8]
	; InlineAsm Start
	pmull2.1q	v7, v18, v7
	; InlineAsm End
	add	x8, x0, #128
	ld2.2d	{ v18, v19 }, [x8]
	eor.16b	v20, v18, v16
	add	x8, x1, #160
	ld2.2d	{ v21, v22 }, [x8]
	eor.16b	v16, v19, v17
	add	x8, x0, #160
	ld2.2d	{ v17, v18 }, [x8]
	; InlineAsm Start
	pmull.1q	v19, v20, v16
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v20, v16
	; InlineAsm End
	eor.16b	v20, v17, v21
	eor.16b	v17, v18, v22
	; InlineAsm Start
	pmull.1q	v18, v20, v17
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v20, v17
	; InlineAsm End
	add	x8, x0, #192
	add	x9, x1, #192
	ld2.2d	{ v20, v21 }, [x9]
	ld2.2d	{ v22, v23 }, [x8]
	eor.16b	v24, v22, v20
	eor.16b	v20, v23, v21
	; InlineAsm Start
	pmull.1q	v21, v24, v20
	; InlineAsm End
	add	x8, x1, #224
	ld2.2d	{ v22, v23 }, [x8]
	; InlineAsm Start
	pmull2.1q	v20, v24, v20
	; InlineAsm End
	add	x8, x0, #224
	ld2.2d	{ v24, v25 }, [x8]
	eor.16b	v26, v24, v22
	eor.16b	v22, v25, v23
	; InlineAsm Start
	pmull.1q	v23, v26, v22
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v22, v26, v22
	; InlineAsm End
	eor.16b	v0, v1, v0
	eor3.16b	v0, v0, v2, v3
	eor3.16b	v0, v0, v4, v5
	eor3.16b	v0, v0, v6, v7
	eor3.16b	v0, v0, v19, v16
	eor3.16b	v0, v0, v18, v17
	eor3.16b	v0, v0, v21, v20
	eor3.16b	v6, v0, v23, v22
	b	LBB2_19
LBB2_9:
	cmp	x8, #64
	b.lo	LBB2_13
; %bb.10:
	mov	x9, #0                          ; =0x0
	movi.2d	v7, #0000000000000000
	mov	x10, x0
	mov	x11, x1
	movi.2d	v16, #0000000000000000
LBB2_11:                                ; =>This Inner Loop Header: Depth=1
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
	b.hi	LBB2_11
; %bb.12:
	neg	x20, x9
	mov	x8, x12
	subs	x9, x12, #32
	b.hs	LBB2_14
	b	LBB2_15
LBB2_13:
	mov	x20, #0                         ; =0x0
	movi.2d	v16, #0000000000000000
	movi.2d	v7, #0000000000000000
	subs	x9, x8, #32
	b.lo	LBB2_15
LBB2_14:
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
LBB2_15:
	subs	x9, x8, #16
	b.hs	LBB2_21
; %bb.16:
	cbz	x8, LBB2_18
LBB2_17:
	stp	xzr, xzr, [x29, #-136]
	mov	x21, x0
	sub	x0, x29, #136
	add	x1, x1, x20
	mov	x22, x2
	mov	x2, x8
	str	q8, [sp, #32]                   ; 16-byte Folded Spill
	stur	q9, [x29, #-160]                ; 16-byte Folded Spill
	stp	q16, q7, [x29, #-208]           ; 32-byte Folded Spill
	bl	_memcpy
	ldp	q16, q7, [x29, #-208]           ; 32-byte Folded Reload
	ldur	q9, [x29, #-160]                ; 16-byte Folded Reload
	ldr	q8, [sp, #32]                   ; 16-byte Folded Reload
	mov	x0, x21
	mov	x2, x22
	and	x8, x20, #0xfffffffffffffff8
	add	x8, x21, x8
	ldp	d0, d1, [x29, #-136]
	ldp	d2, d3, [x8]
	eor.8b	v0, v2, v0
	eor.8b	v1, v3, v1
	; InlineAsm Start
	pmull.1q	v0, v0, v1
	; InlineAsm End
	eor.16b	v7, v0, v7
LBB2_18:
	eor.16b	v6, v7, v16
LBB2_19:
	ldp	d3, d4, [x0, #280]
	ldp	d1, d2, [x0, #296]
	ldp	d0, d5, [x0, #312]
	ldur	x8, [x29, #-120]
Lloh15:
	adrp	x9, ___stack_chk_guard@GOTPAGE
Lloh16:
	ldr	x9, [x9, ___stack_chk_guard@GOTPAGEOFF]
Lloh17:
	ldr	x9, [x9]
	cmp	x9, x8
	b.ne	LBB2_22
; %bb.20:
	fmov	d7, d8
	dup.2d	v16, x2
	eor.16b	v17, v16, v6
	eor3.16b	v6, v16, v6, v8
	eor3.16b	v16, v17, v8, v7
	ext.16b	v16, v16, v16, #8
	; InlineAsm Start
	pmull.1q	v16, v16, v9
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
	add	sp, sp, #640
	ldp	x29, x30, [sp, #112]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #96]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #80]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #64]             ; 16-byte Folded Reload
	ldp	d9, d8, [sp, #48]               ; 16-byte Folded Reload
	ldp	d11, d10, [sp, #32]             ; 16-byte Folded Reload
	ldp	d13, d12, [sp, #16]             ; 16-byte Folded Reload
	ldp	d15, d14, [sp], #128            ; 16-byte Folded Reload
	ret
LBB2_21:
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
	cbnz	x9, LBB2_17
	b	LBB2_18
LBB2_22:
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
	stp	d15, d14, [sp, #-128]!          ; 16-byte Folded Spill
	stp	d13, d12, [sp, #16]             ; 16-byte Folded Spill
	stp	d11, d10, [sp, #32]             ; 16-byte Folded Spill
	stp	d9, d8, [sp, #48]               ; 16-byte Folded Spill
	stp	x28, x27, [sp, #64]             ; 16-byte Folded Spill
	stp	x22, x21, [sp, #80]             ; 16-byte Folded Spill
	stp	x20, x19, [sp, #96]             ; 16-byte Folded Spill
	stp	x29, x30, [sp, #112]            ; 16-byte Folded Spill
	add	x29, sp, #112
	sub	sp, sp, #1552
	.cfi_def_cfa w29, 16
	.cfi_offset w30, -8
	.cfi_offset w29, -16
	.cfi_offset w19, -24
	.cfi_offset w20, -32
	.cfi_offset w21, -40
	.cfi_offset w22, -48
	.cfi_offset w27, -56
	.cfi_offset w28, -64
	.cfi_offset b8, -72
	.cfi_offset b9, -80
	.cfi_offset b10, -88
	.cfi_offset b11, -96
	.cfi_offset b12, -104
	.cfi_offset b13, -112
	.cfi_offset b14, -120
	.cfi_offset b15, -128
Lloh18:
	adrp	x8, ___stack_chk_guard@GOTPAGE
Lloh19:
	ldr	x8, [x8, ___stack_chk_guard@GOTPAGEOFF]
Lloh20:
	ldr	x8, [x8]
	stur	x8, [x29, #-120]
	ldr	q10, [x0, #256]
	ldr	d0, [x0, #272]
	eor.8b	v8, v10, v0
	adrp	x19, lCPI3_0@PAGE
	cmp	x2, #2049
	b.lo	LBB3_4
; %bb.1:
	mov	x9, x0
	ld2.2d	{ v0, v1 }, [x9], #32
	mov	x8, sp
	st1.2d	{ v0, v1 }, [x8]                ; 32-byte Folded Spill
	add	x10, x0, #64
	add	x11, x0, #96
	add	x12, x0, #128
	add	x13, x0, #160
	add	x14, x0, #192
	add	x15, x0, #224
	ldr	q28, [x19, lCPI3_0@PAGEOFF]
	mov	x8, x2
	str	q10, [sp, #32]                  ; 16-byte Folded Spill
LBB3_2:                                 ; =>This Inner Loop Header: Depth=1
	stur	q8, [x29, #-160]                ; 16-byte Folded Spill
	mov	x16, x1
	ld2.2d	{ v0, v1 }, [x16], #32
	sub	x17, x29, #192
	st1.2d	{ v0, v1 }, [x17]               ; 32-byte Folded Spill
	add	x17, x1, #64
	ld2.2d	{ v18, v19 }, [x17]
	add	x17, x1, #288
	ld2.2d	{ v3, v4 }, [x17]
	add	x17, x1, #320
	ld2.2d	{ v24, v25 }, [x17]
	add	x17, x1, #544
	ld2.2d	{ v0, v1 }, [x17]
	ld2.2d	{ v8, v9 }, [x9]
	ld2.2d	{ v26, v27 }, [x10]
	ld2.2d	{ v6, v7 }, [x16]
	add	x16, x1, #576
	ld2.2d	{ v29, v30 }, [x16]
	eor.16b	v5, v3, v8
	add	x16, x1, #800
	ld2.2d	{ v16, v17 }, [x16]
	eor.16b	v2, v4, v9
	stur	q2, [x29, #-240]                ; 16-byte Folded Spill
	add	x16, x1, #832
	ld2.2d	{ v21, v22 }, [x16]
	eor.16b	v2, v0, v8
	stp	q2, q5, [x29, #-224]            ; 32-byte Folded Spill
	add	x16, x1, #1056
	ld2.2d	{ v2, v3 }, [x16]
	eor.16b	v0, v1, v9
	str	q0, [sp, #1360]                 ; 16-byte Folded Spill
	add	x16, x1, #1088
	ld2.2d	{ v0, v1 }, [x16]
	eor.16b	v4, v16, v8
	stur	q4, [x29, #-256]                ; 16-byte Folded Spill
	add	x16, x1, #1312
	ld2.2d	{ v4, v5 }, [x16]
	eor.16b	v20, v8, v6
	str	q20, [sp, #1200]                ; 16-byte Folded Spill
	eor.16b	v16, v17, v9
	str	q16, [sp, #1280]                ; 16-byte Folded Spill
	eor.16b	v16, v2, v8
	str	q16, [sp, #1344]                ; 16-byte Folded Spill
	eor.16b	v2, v3, v9
	str	q2, [sp, #1328]                 ; 16-byte Folded Spill
	eor.16b	v2, v4, v8
	str	q2, [sp, #1392]                 ; 16-byte Folded Spill
	eor.16b	v2, v5, v9
	str	q2, [sp, #1376]                 ; 16-byte Folded Spill
	eor.16b	v2, v9, v7
	str	q2, [sp, #1184]                 ; 16-byte Folded Spill
	add	x16, x1, #1568
	ld2.2d	{ v2, v3 }, [x16]
	eor.16b	v4, v2, v8
	str	q4, [sp, #1312]                 ; 16-byte Folded Spill
	eor.16b	v2, v3, v9
	str	q2, [sp, #1296]                 ; 16-byte Folded Spill
	add	x16, x1, #1824
	ld2.2d	{ v2, v3 }, [x16]
	eor.16b	v4, v2, v8
	str	q4, [sp, #1264]                 ; 16-byte Folded Spill
	eor.16b	v2, v3, v9
	str	q2, [sp, #1248]                 ; 16-byte Folded Spill
	add	x16, x1, #1344
	eor.16b	v2, v26, v18
	str	q2, [sp, #496]                  ; 16-byte Folded Spill
	eor.16b	v2, v27, v19
	str	q2, [sp, #464]                  ; 16-byte Folded Spill
	eor.16b	v2, v24, v26
	str	q2, [sp, #400]                  ; 16-byte Folded Spill
	eor.16b	v2, v25, v27
	str	q2, [sp, #368]                  ; 16-byte Folded Spill
	eor.16b	v4, v29, v26
	eor.16b	v2, v30, v27
	stp	q2, q4, [sp, #704]              ; 32-byte Folded Spill
	eor.16b	v2, v21, v26
	str	q2, [sp, #848]                  ; 16-byte Folded Spill
	eor.16b	v2, v22, v27
	str	q2, [sp, #816]                  ; 16-byte Folded Spill
	eor.16b	v2, v0, v26
	eor.16b	v0, v1, v27
	stp	q0, q2, [sp, #992]              ; 32-byte Folded Spill
	ld2.2d	{ v0, v1 }, [x16]
	eor.16b	v2, v0, v26
	str	q2, [sp, #1104]                 ; 16-byte Folded Spill
	eor.16b	v0, v1, v27
	str	q0, [sp, #1088]                 ; 16-byte Folded Spill
	add	x16, x1, #1600
	ld2.2d	{ v0, v1 }, [x16]
	add	x16, x1, #96
	eor.16b	v2, v0, v26
	str	q2, [sp, #1168]                 ; 16-byte Folded Spill
	ld2.2d	{ v2, v3 }, [x16]
	eor.16b	v0, v1, v27
	str	q0, [sp, #1136]                 ; 16-byte Folded Spill
	add	x16, x1, #1856
	ld2.2d	{ v0, v1 }, [x16]
	eor.16b	v4, v0, v26
	str	q4, [sp, #1232]                 ; 16-byte Folded Spill
	eor.16b	v0, v1, v27
	str	q0, [sp, #1216]                 ; 16-byte Folded Spill
	ld2.2d	{ v0, v1 }, [x11]
	add	x16, x1, #352
	eor.16b	v6, v0, v2
	ld2.2d	{ v4, v5 }, [x16]
	eor.16b	v2, v1, v3
	stp	q2, q6, [sp, #288]              ; 32-byte Folded Spill
	add	x16, x1, #608
	ld2.2d	{ v2, v3 }, [x16]
	add	x16, x1, #864
	eor.16b	v6, v4, v0
	str	q6, [sp, #240]                  ; 16-byte Folded Spill
	ld2.2d	{ v6, v7 }, [x16]
	eor.16b	v4, v5, v1
	str	q4, [sp, #224]                  ; 16-byte Folded Spill
	eor.16b	v4, v2, v0
	eor.16b	v2, v3, v1
	stp	q2, q4, [sp, #512]              ; 32-byte Folded Spill
	eor.16b	v4, v6, v0
	eor.16b	v2, v7, v1
	stp	q2, q4, [sp, #640]              ; 32-byte Folded Spill
	add	x16, x1, #1120
	ld2.2d	{ v2, v3 }, [x16]
	eor.16b	v4, v2, v0
	eor.16b	v2, v3, v1
	stp	q2, q4, [sp, #864]              ; 32-byte Folded Spill
	add	x16, x1, #1376
	ld2.2d	{ v2, v3 }, [x16]
	eor.16b	v4, v2, v0
	eor.16b	v2, v3, v1
	stp	q2, q4, [sp, #960]              ; 32-byte Folded Spill
	add	x16, x1, #1632
	ld2.2d	{ v2, v3 }, [x16]
	add	x16, x1, #128
	eor.16b	v4, v2, v0
	str	q4, [sp, #1072]                 ; 16-byte Folded Spill
	ld2.2d	{ v4, v5 }, [x16]
	eor.16b	v2, v3, v1
	str	q2, [sp, #1040]                 ; 16-byte Folded Spill
	add	x16, x1, #1888
	ld2.2d	{ v2, v3 }, [x16]
	eor.16b	v6, v2, v0
	str	q6, [sp, #1152]                 ; 16-byte Folded Spill
	eor.16b	v0, v3, v1
	str	q0, [sp, #1120]                 ; 16-byte Folded Spill
	ld2.2d	{ v0, v1 }, [x12]
	add	x16, x1, #384
	eor.16b	v6, v0, v4
	ld2.2d	{ v2, v3 }, [x16]
	eor.16b	v4, v1, v5
	stp	q4, q6, [sp, #128]              ; 32-byte Folded Spill
	add	x16, x1, #640
	ld2.2d	{ v4, v5 }, [x16]
	add	x16, x1, #896
	eor.16b	v10, v2, v0
	ld2.2d	{ v6, v7 }, [x16]
	eor.16b	v31, v3, v1
	eor.16b	v2, v4, v0
	str	q2, [sp, #272]                  ; 16-byte Folded Spill
	eor.16b	v2, v5, v1
	str	q2, [sp, #256]                  ; 16-byte Folded Spill
	eor.16b	v4, v6, v0
	eor.16b	v2, v7, v1
	stp	q2, q4, [sp, #416]              ; 32-byte Folded Spill
	add	x16, x1, #1152
	ld2.2d	{ v2, v3 }, [x16]
	eor.16b	v4, v2, v0
	eor.16b	v2, v3, v1
	stp	q2, q4, [sp, #672]              ; 32-byte Folded Spill
	add	x16, x1, #1408
	ld2.2d	{ v2, v3 }, [x16]
	eor.16b	v4, v2, v0
	str	q4, [sp, #832]                  ; 16-byte Folded Spill
	eor.16b	v2, v3, v1
	str	q2, [sp, #800]                  ; 16-byte Folded Spill
	add	x16, x1, #1664
	ld2.2d	{ v2, v3 }, [x16]
	add	x16, x1, #160
	eor.16b	v4, v2, v0
	str	q4, [sp, #944]                  ; 16-byte Folded Spill
	ld2.2d	{ v4, v5 }, [x16]
	eor.16b	v2, v3, v1
	str	q2, [sp, #912]                  ; 16-byte Folded Spill
	add	x16, x1, #1920
	ld2.2d	{ v2, v3 }, [x16]
	eor.16b	v6, v2, v0
	str	q6, [sp, #1056]                 ; 16-byte Folded Spill
	eor.16b	v0, v3, v1
	str	q0, [sp, #1024]                 ; 16-byte Folded Spill
	ld2.2d	{ v0, v1 }, [x13]
	add	x16, x1, #416
	eor.16b	v27, v0, v4
	ld2.2d	{ v2, v3 }, [x16]
	eor.16b	v26, v1, v5
	add	x16, x1, #672
	ld2.2d	{ v4, v5 }, [x16]
	add	x16, x1, #928
	eor.16b	v6, v2, v0
	str	q6, [sp, #64]                   ; 16-byte Folded Spill
	ld2.2d	{ v6, v7 }, [x16]
	eor.16b	v2, v3, v1
	str	q2, [sp, #48]                   ; 16-byte Folded Spill
	eor.16b	v2, v4, v0
	str	q2, [sp, #112]                  ; 16-byte Folded Spill
	eor.16b	v2, v5, v1
	str	q2, [sp, #96]                   ; 16-byte Folded Spill
	eor.16b	v4, v6, v0
	eor.16b	v2, v7, v1
	stp	q2, q4, [sp, #160]              ; 32-byte Folded Spill
	add	x16, x1, #1184
	ld2.2d	{ v2, v3 }, [x16]
	eor.16b	v4, v2, v0
	str	q4, [sp, #480]                  ; 16-byte Folded Spill
	eor.16b	v2, v3, v1
	str	q2, [sp, #448]                  ; 16-byte Folded Spill
	add	x16, x1, #1440
	ld2.2d	{ v2, v3 }, [x16]
	eor.16b	v4, v2, v0
	eor.16b	v2, v3, v1
	stp	q2, q4, [sp, #608]              ; 32-byte Folded Spill
	add	x16, x1, #1696
	ld2.2d	{ v2, v3 }, [x16]
	add	x16, x1, #192
	eor.16b	v4, v2, v0
	str	q4, [sp, #784]                  ; 16-byte Folded Spill
	ld2.2d	{ v6, v7 }, [x16]
	eor.16b	v2, v3, v1
	str	q2, [sp, #736]                  ; 16-byte Folded Spill
	add	x16, x1, #1952
	ld2.2d	{ v2, v3 }, [x16]
	eor.16b	v4, v2, v0
	str	q4, [sp, #928]                  ; 16-byte Folded Spill
	eor.16b	v0, v3, v1
	str	q0, [sp, #896]                  ; 16-byte Folded Spill
	ld2.2d	{ v24, v25 }, [x14]
	add	x16, x1, #448
	eor.16b	v18, v24, v6
	ld2.2d	{ v0, v1 }, [x16]
	eor.16b	v17, v25, v7
	add	x16, x1, #704
	ld2.2d	{ v6, v7 }, [x16]
	add	x16, x1, #960
	eor.16b	v16, v0, v24
	ld2.2d	{ v8, v9 }, [x16]
	eor.16b	v20, v1, v25
	eor.16b	v21, v6, v24
	eor.16b	v19, v7, v25
	eor.16b	v30, v8, v24
	eor.16b	v29, v9, v25
	add	x16, x1, #1216
	ld2.2d	{ v6, v7 }, [x16]
	eor.16b	v2, v6, v24
	eor.16b	v0, v7, v25
	stp	q0, q2, [sp, #192]              ; 32-byte Folded Spill
	add	x16, x1, #1472
	ld2.2d	{ v6, v7 }, [x16]
	eor.16b	v2, v6, v24
	eor.16b	v0, v7, v25
	stp	q0, q2, [sp, #320]              ; 32-byte Folded Spill
	add	x16, x1, #1728
	ld2.2d	{ v6, v7 }, [x16]
	add	x16, x1, #224
	eor.16b	v2, v6, v24
	ld2.2d	{ v8, v9 }, [x16]
	eor.16b	v0, v7, v25
	stp	q0, q2, [sp, #576]              ; 32-byte Folded Spill
	add	x16, x1, #1984
	ld2.2d	{ v6, v7 }, [x16]
	eor.16b	v2, v6, v24
	eor.16b	v0, v7, v25
	stp	q0, q2, [sp, #752]              ; 32-byte Folded Spill
	ld2.2d	{ v0, v1 }, [x15]
	add	x16, x1, #480
	eor.16b	v15, v0, v8
	ld2.2d	{ v6, v7 }, [x16]
	eor.16b	v13, v1, v9
	add	x16, x1, #736
	ld2.2d	{ v24, v25 }, [x16]
	add	x16, x1, #992
	eor.16b	v14, v6, v0
	ld2.2d	{ v8, v9 }, [x16]
	eor.16b	v12, v7, v1
	eor.16b	v6, v24, v0
	eor.16b	v11, v25, v1
	eor.16b	v24, v8, v0
	eor.16b	v7, v9, v1
	add	x16, x1, #1248
	ld2.2d	{ v8, v9 }, [x16]
	eor.16b	v2, v8, v0
	str	q2, [sp, #80]                   ; 16-byte Folded Spill
	eor.16b	v25, v9, v1
	add	x16, x1, #1504
	ld2.2d	{ v2, v3 }, [x16]
	eor.16b	v9, v2, v0
	eor.16b	v8, v3, v1
	add	x16, x1, #1760
	ld2.2d	{ v2, v3 }, [x16]
	add	x16, x1, #256
	eor.16b	v4, v2, v0
	str	q4, [sp, #384]                  ; 16-byte Folded Spill
	ld2.2d	{ v4, v5 }, [x16]
	eor.16b	v2, v3, v1
	str	q2, [sp, #352]                  ; 16-byte Folded Spill
	add	x16, x1, #2016
	ld2.2d	{ v2, v3 }, [x16]
	eor.16b	v22, v2, v0
	eor.16b	v0, v3, v1
	stp	q0, q22, [sp, #544]             ; 32-byte Folded Spill
	mov	x16, sp
	ld1.2d	{ v22, v23 }, [x16]             ; 32-byte Folded Reload
	eor.16b	v0, v4, v22
	eor.16b	v1, v5, v23
	; InlineAsm Start
	pmull.1q	v2, v0, v1
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v0, v0, v1
	; InlineAsm End
	ldur	q3, [x29, #-208]                ; 16-byte Folded Reload
	ldur	q4, [x29, #-240]                ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v1, v3, v4
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v3, v4
	; InlineAsm End
	eor.16b	v0, v2, v0
	eor3.16b	v0, v0, v1, v3
	sub	x16, x29, #192
	ld1.2d	{ v2, v3 }, [x16]               ; 32-byte Folded Reload
	eor.16b	v1, v2, v22
	eor.16b	v2, v3, v23
	; InlineAsm Start
	pmull.1q	v3, v1, v2
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v1, v1, v2
	; InlineAsm End
	ldr	q4, [sp, #1200]                 ; 16-byte Folded Reload
	ldr	q5, [sp, #1184]                 ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v2, v4, v5
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v4, v4, v5
	; InlineAsm End
	eor.16b	v1, v3, v1
	eor3.16b	v1, v1, v2, v4
	ldr	q3, [sp, #400]                  ; 16-byte Folded Reload
	ldr	q4, [sp, #368]                  ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v2, v3, v4
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v3, v4
	; InlineAsm End
	eor3.16b	v0, v0, v2, v3
	ldr	q3, [sp, #496]                  ; 16-byte Folded Reload
	ldr	q4, [sp, #464]                  ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v2, v3, v4
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v3, v4
	; InlineAsm End
	eor3.16b	v1, v1, v2, v3
	ldp	q4, q3, [sp, #224]              ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v2, v3, v4
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v3, v4
	; InlineAsm End
	eor3.16b	v0, v0, v2, v3
	ldp	q4, q3, [sp, #288]              ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v2, v3, v4
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v3, v4
	; InlineAsm End
	eor3.16b	v1, v1, v2, v3
	; InlineAsm Start
	pmull.1q	v2, v10, v31
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v10, v31
	; InlineAsm End
	eor3.16b	v0, v0, v2, v3
	ldp	q4, q3, [sp, #128]              ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v2, v3, v4
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v3, v4
	; InlineAsm End
	eor3.16b	v1, v1, v2, v3
	ldp	q4, q3, [sp, #48]               ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v2, v3, v4
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v3, v4
	; InlineAsm End
	eor3.16b	v0, v0, v2, v3
	; InlineAsm Start
	pmull.1q	v2, v27, v26
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v27, v26
	; InlineAsm End
	eor3.16b	v1, v1, v2, v3
	; InlineAsm Start
	pmull.1q	v2, v16, v20
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v16, v20
	; InlineAsm End
	eor3.16b	v0, v0, v2, v3
	; InlineAsm Start
	pmull.1q	v2, v18, v17
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v18, v17
	; InlineAsm End
	eor3.16b	v1, v1, v2, v3
	; InlineAsm Start
	pmull.1q	v2, v14, v12
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v14, v12
	; InlineAsm End
	eor3.16b	v5, v0, v2, v3
	; InlineAsm Start
	pmull.1q	v0, v15, v13
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v2, v15, v13
	; InlineAsm End
	eor3.16b	v17, v1, v0, v2
	add	x16, x1, #512
	ld2.2d	{ v0, v1 }, [x16]
	eor.16b	v2, v0, v22
	eor.16b	v0, v1, v23
	; InlineAsm Start
	pmull.1q	v1, v2, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v0, v2, v0
	; InlineAsm End
	ldur	q3, [x29, #-224]                ; 16-byte Folded Reload
	ldr	q4, [sp, #1360]                 ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v2, v3, v4
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v3, v4
	; InlineAsm End
	eor.16b	v0, v1, v0
	eor3.16b	v0, v0, v2, v3
	ldp	q3, q2, [sp, #704]              ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v1, v2, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v2, v2, v3
	; InlineAsm End
	eor3.16b	v0, v0, v1, v2
	ldp	q3, q2, [sp, #512]              ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v1, v2, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v2, v2, v3
	; InlineAsm End
	eor3.16b	v0, v0, v1, v2
	ldp	q3, q2, [sp, #256]              ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v1, v2, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v2, v2, v3
	; InlineAsm End
	eor3.16b	v0, v0, v1, v2
	ldp	q3, q2, [sp, #96]               ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v1, v2, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v2, v2, v3
	; InlineAsm End
	ldr	q10, [sp, #32]                  ; 16-byte Folded Reload
	eor3.16b	v0, v0, v1, v2
	; InlineAsm Start
	pmull.1q	v1, v21, v19
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v2, v21, v19
	; InlineAsm End
	eor3.16b	v0, v0, v1, v2
	; InlineAsm Start
	pmull.1q	v1, v6, v11
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v2, v6, v11
	; InlineAsm End
	eor3.16b	v0, v0, v1, v2
	add	x16, x1, #768
	ld2.2d	{ v1, v2 }, [x16]
	eor.16b	v3, v1, v22
	eor.16b	v1, v2, v23
	; InlineAsm Start
	pmull.1q	v2, v3, v1
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v1, v3, v1
	; InlineAsm End
	ldur	q4, [x29, #-256]                ; 16-byte Folded Reload
	ldr	q6, [sp, #1280]                 ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v3, v4, v6
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v4, v4, v6
	; InlineAsm End
	eor.16b	v1, v2, v1
	eor3.16b	v1, v1, v3, v4
	ldr	q3, [sp, #848]                  ; 16-byte Folded Reload
	ldr	q4, [sp, #816]                  ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v2, v3, v4
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v3, v4
	; InlineAsm End
	eor3.16b	v1, v1, v2, v3
	ldp	q4, q3, [sp, #640]              ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v2, v3, v4
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v3, v4
	; InlineAsm End
	eor3.16b	v1, v1, v2, v3
	ldp	q4, q3, [sp, #416]              ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v2, v3, v4
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v3, v4
	; InlineAsm End
	eor3.16b	v1, v1, v2, v3
	ldp	q4, q3, [sp, #160]              ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v2, v3, v4
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v3, v4
	; InlineAsm End
	eor3.16b	v1, v1, v2, v3
	; InlineAsm Start
	pmull.1q	v2, v30, v29
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v30, v29
	; InlineAsm End
	eor3.16b	v1, v1, v2, v3
	; InlineAsm Start
	pmull.1q	v2, v24, v7
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v24, v7
	; InlineAsm End
	eor3.16b	v1, v1, v2, v3
	eor.16b	v3, v0, v10
	eor.16b	v0, v1, v10
	ext.16b	v1, v3, v3, #8
	ext.16b	v4, v0, v0, #8
	; InlineAsm Start
	pmull.1q	v1, v4, v1
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v2, v1, v28
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v6, v2, v28
	; InlineAsm End
	eor3.16b	v1, v1, v6, v2
	eor.16b	v2, v5, v10
	eor.16b	v6, v17, v10
	ext.16b	v7, v2, v2, #8
	; InlineAsm Start
	pmull.1q	v5, v7, v6
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v5, v28
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v16, v28
	; InlineAsm End
	eor.16b	v5, v5, v17
	eor3.16b	v5, v5, v2, v16
	add	x16, x1, #1024
	ld2.2d	{ v16, v17 }, [x16]
	eor.16b	v2, v16, v22
	eor.16b	v16, v17, v23
	; InlineAsm Start
	pmull.1q	v17, v2, v16
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v2, v2, v16
	; InlineAsm End
	ldr	q18, [sp, #1344]                ; 16-byte Folded Reload
	ldr	q19, [sp, #1328]                ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v16, v18, v19
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v18, v18, v19
	; InlineAsm End
	eor.16b	v2, v17, v2
	eor3.16b	v2, v2, v16, v18
	ldp	q18, q17, [sp, #992]            ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v16, v17, v18
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v17, v18
	; InlineAsm End
	eor3.16b	v2, v2, v16, v17
	ldp	q18, q17, [sp, #864]            ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v16, v17, v18
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v17, v18
	; InlineAsm End
	eor3.16b	v2, v2, v16, v17
	ldp	q18, q17, [sp, #672]            ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v16, v17, v18
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v17, v18
	; InlineAsm End
	eor3.16b	v2, v2, v16, v17
	ldr	q17, [sp, #480]                 ; 16-byte Folded Reload
	ldr	q18, [sp, #448]                 ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v16, v17, v18
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v17, v18
	; InlineAsm End
	eor3.16b	v2, v2, v16, v17
	ldp	q18, q17, [sp, #192]            ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v16, v17, v18
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v17, v18
	; InlineAsm End
	eor3.16b	v2, v2, v16, v17
	ldr	q17, [sp, #80]                  ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v16, v17, v25
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v17, v25
	; InlineAsm End
	eor3.16b	v16, v2, v16, v17
	ext.16b	v2, v6, v6, #8
	; InlineAsm Start
	pmull.1q	v2, v7, v2
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v6, v2, v28
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v7, v6, v28
	; InlineAsm End
	eor3.16b	v2, v2, v7, v6
	add	x16, x1, #1280
	ld2.2d	{ v6, v7 }, [x16]
	eor.16b	v17, v6, v22
	eor.16b	v6, v7, v23
	; InlineAsm Start
	pmull.1q	v7, v17, v6
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v6, v17, v6
	; InlineAsm End
	ldr	q18, [sp, #1392]                ; 16-byte Folded Reload
	ldr	q19, [sp, #1376]                ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v17, v18, v19
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v18, v18, v19
	; InlineAsm End
	eor.16b	v6, v7, v6
	eor3.16b	v6, v6, v17, v18
	ldr	q17, [sp, #1104]                ; 16-byte Folded Reload
	ldr	q18, [sp, #1088]                ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v7, v17, v18
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v17, v18
	; InlineAsm End
	eor3.16b	v6, v6, v7, v17
	ldp	q18, q17, [sp, #960]            ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v7, v17, v18
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v17, v18
	; InlineAsm End
	eor3.16b	v6, v6, v7, v17
	ldr	q17, [sp, #832]                 ; 16-byte Folded Reload
	ldr	q18, [sp, #800]                 ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v7, v17, v18
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v17, v18
	; InlineAsm End
	eor3.16b	v6, v6, v7, v17
	ldp	q18, q17, [sp, #608]            ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v7, v17, v18
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v17, v18
	; InlineAsm End
	eor3.16b	v6, v6, v7, v17
	ldp	q18, q17, [sp, #320]            ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v7, v17, v18
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v17, v18
	; InlineAsm End
	eor3.16b	v6, v6, v7, v17
	; InlineAsm Start
	pmull.1q	v7, v9, v8
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v9, v8
	; InlineAsm End
	eor3.16b	v6, v6, v7, v17
	add	x16, x1, #1536
	eor.16b	v16, v16, v10
	eor.16b	v6, v6, v10
	ext.16b	v17, v6, v6, #8
	; InlineAsm Start
	pmull.1q	v7, v17, v16
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v18, v7, v28
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v19, v18, v28
	; InlineAsm End
	ld2.2d	{ v20, v21 }, [x16]
	eor.16b	v7, v7, v19
	eor3.16b	v6, v7, v6, v18
	eor.16b	v7, v20, v22
	eor.16b	v18, v21, v23
	; InlineAsm Start
	pmull.1q	v19, v7, v18
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v7, v7, v18
	; InlineAsm End
	ldr	q20, [sp, #1312]                ; 16-byte Folded Reload
	ldr	q21, [sp, #1296]                ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v18, v20, v21
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v20, v20, v21
	; InlineAsm End
	eor.16b	v7, v19, v7
	eor3.16b	v7, v7, v18, v20
	ldr	q19, [sp, #1168]                ; 16-byte Folded Reload
	ldr	q20, [sp, #1136]                ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v18, v19, v20
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v19, v19, v20
	; InlineAsm End
	eor3.16b	v7, v7, v18, v19
	ldr	q19, [sp, #1072]                ; 16-byte Folded Reload
	ldr	q20, [sp, #1040]                ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v18, v19, v20
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v19, v19, v20
	; InlineAsm End
	eor3.16b	v7, v7, v18, v19
	ldr	q19, [sp, #944]                 ; 16-byte Folded Reload
	ldr	q20, [sp, #912]                 ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v18, v19, v20
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v19, v19, v20
	; InlineAsm End
	eor3.16b	v7, v7, v18, v19
	ldr	q19, [sp, #784]                 ; 16-byte Folded Reload
	ldr	q20, [sp, #736]                 ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v18, v19, v20
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v19, v19, v20
	; InlineAsm End
	eor3.16b	v18, v7, v18, v19
	; InlineAsm Start
	pmull.1q	v19, v4, v3
	; InlineAsm End
	ldp	q7, q4, [sp, #576]              ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v3, v4, v7
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v4, v4, v7
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v7, v19, v28
	; InlineAsm End
	eor3.16b	v18, v18, v3, v4
	; InlineAsm Start
	pmull2.1q	v20, v7, v28
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v3, v1, v5
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v4, v3, v28
	; InlineAsm End
	ext.16b	v5, v16, v16, #8
	; InlineAsm Start
	pmull.1q	v5, v17, v5
	; InlineAsm End
	ldr	q17, [sp, #384]                 ; 16-byte Folded Reload
	ldr	q21, [sp, #352]                 ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v16, v17, v21
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v17, v21
	; InlineAsm End
	add	x16, x1, #1792
	; InlineAsm Start
	pmull2.1q	v21, v4, v28
	; InlineAsm End
	eor3.16b	v16, v18, v16, v17
	ld2.2d	{ v17, v18 }, [x16]
	eor3.16b	v19, v20, v21, v19
	; InlineAsm Start
	pmull2.1q	v20, v5, v28
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v21, v20, v28
	; InlineAsm End
	eor3.16b	v5, v5, v21, v20
	eor.16b	v20, v17, v22
	eor.16b	v17, v18, v23
	; InlineAsm Start
	pmull.1q	v18, v20, v17
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v20, v17
	; InlineAsm End
	ldr	q21, [sp, #1264]                ; 16-byte Folded Reload
	ldr	q22, [sp, #1248]                ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v20, v21, v22
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v21, v21, v22
	; InlineAsm End
	eor.16b	v17, v18, v17
	eor3.16b	v17, v17, v20, v21
	ldr	q20, [sp, #1232]                ; 16-byte Folded Reload
	ldr	q21, [sp, #1216]                ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v18, v20, v21
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v20, v20, v21
	; InlineAsm End
	eor3.16b	v17, v17, v18, v20
	ldr	q20, [sp, #1152]                ; 16-byte Folded Reload
	ldr	q21, [sp, #1120]                ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v18, v20, v21
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v20, v20, v21
	; InlineAsm End
	eor3.16b	v17, v17, v18, v20
	ldr	q20, [sp, #1056]                ; 16-byte Folded Reload
	ldr	q21, [sp, #1024]                ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v18, v20, v21
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v20, v20, v21
	; InlineAsm End
	eor3.16b	v17, v17, v18, v20
	ldr	q20, [sp, #928]                 ; 16-byte Folded Reload
	ldr	q21, [sp, #896]                 ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v18, v20, v21
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v20, v20, v21
	; InlineAsm End
	eor3.16b	v17, v17, v18, v20
	; InlineAsm Start
	pmull.1q	v1, v1, v2
	; InlineAsm End
	ldp	q20, q18, [sp, #752]            ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v2, v18, v20
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v18, v18, v20
	; InlineAsm End
	ldp	q22, q21, [sp, #544]            ; 32-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v20, v21, v22
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v21, v21, v22
	; InlineAsm End
	eor3.16b	v2, v17, v2, v18
	; InlineAsm Start
	pmull2.1q	v17, v1, v28
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v18, v17, v28
	; InlineAsm End
	eor.16b	v16, v16, v10
	eor3.16b	v2, v2, v20, v21
	ext.16b	v20, v16, v16, #8
	eor.16b	v2, v2, v10
	eor3.16b	v1, v1, v18, v17
	ext.16b	v17, v2, v2, #8
	; InlineAsm Start
	pmull.1q	v18, v17, v20
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v20, v18, v28
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v21, v20, v28
	; InlineAsm End
	eor3.16b	v18, v18, v21, v20
	eor3.16b	v0, v19, v0, v7
	; InlineAsm Start
	pmull.1q	v7, v17, v16
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v7, v28
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v6, v18, v6
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v5, v18, v5
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v5, v28
	; InlineAsm End
	eor3.16b	v0, v0, v3, v4
	; InlineAsm Start
	pmull2.1q	v3, v17, v28
	; InlineAsm End
	eor3.16b	v3, v5, v3, v17
	; InlineAsm Start
	pmull.1q	v1, v3, v1
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v4, v1, v28
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v5, v4, v28
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v16, v28
	; InlineAsm End
	eor3.16b	v1, v1, v5, v4
	; InlineAsm Start
	pmull2.1q	v4, v6, v28
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v5, v4, v28
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v0, v3, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v0, v28
	; InlineAsm End
	ldur	q18, [x29, #-160]               ; 16-byte Folded Reload
	; InlineAsm Start
	pmull.1q	v1, v1, v18
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v18, v3, v28
	; InlineAsm End
	eor3.16b	v5, v17, v5, v18
	; InlineAsm Start
	pmull2.1q	v17, v1, v28
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v18, v17, v28
	; InlineAsm End
	eor3.16b	v5, v5, v18, v7
	eor3.16b	v2, v5, v2, v16
	eor3.16b	v2, v2, v6, v4
	eor3.16b	v0, v2, v0, v3
	eor3.16b	v8, v0, v1, v17
	add	x1, x1, #2048
	sub	x8, x8, #2048
	cmp	x8, #2048
	b.hi	LBB3_2
; %bb.3:
	cmp	x8, #257
	b.hs	LBB3_5
	b	LBB3_7
LBB3_4:
	mov	x8, x2
	cmp	x2, #257
	b.lo	LBB3_7
LBB3_5:
	mov	x9, x0
	ld2.2d	{ v0, v1 }, [x9], #32
	add	x10, x0, #64
	add	x11, x0, #96
	add	x12, x0, #128
	add	x13, x0, #160
	add	x14, x0, #192
	add	x15, x0, #224
	ldr	q2, [x19, lCPI3_0@PAGEOFF]
LBB3_6:                                 ; =>This Inner Loop Header: Depth=1
	mov	x16, x1
	ld2.2d	{ v3, v4 }, [x16], #32
	ld2.2d	{ v6, v7 }, [x16]
	eor.16b	v5, v3, v0
	ld2.2d	{ v16, v17 }, [x9]
	eor.16b	v4, v4, v1
	add	x16, x1, #64
	ld2.2d	{ v18, v19 }, [x16]
	; InlineAsm Start
	pmull.1q	v3, v5, v4
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v4, v5, v4
	; InlineAsm End
	ld2.2d	{ v20, v21 }, [x10]
	eor.16b	v22, v20, v18
	add	x16, x1, #96
	ld2.2d	{ v23, v24 }, [x16]
	eor.16b	v18, v21, v19
	ld2.2d	{ v19, v20 }, [x11]
	eor.16b	v21, v19, v23
	; InlineAsm Start
	pmull.1q	v5, v22, v18
	; InlineAsm End
	add	x16, x1, #128
	ld2.2d	{ v25, v26 }, [x16]
	eor.16b	v27, v16, v6
	eor.16b	v19, v20, v24
	ld2.2d	{ v23, v24 }, [x12]
	eor.16b	v6, v17, v7
	add	x16, x1, #160
	ld2.2d	{ v16, v17 }, [x16]
	; InlineAsm Start
	pmull2.1q	v7, v22, v18
	; InlineAsm End
	eor.16b	v18, v23, v25
	ld2.2d	{ v28, v29 }, [x13]
	; InlineAsm Start
	pmull.1q	v20, v21, v19
	; InlineAsm End
	add	x16, x1, #192
	ld2.2d	{ v30, v31 }, [x16]
	; InlineAsm Start
	pmull2.1q	v19, v21, v19
	; InlineAsm End
	eor.16b	v21, v24, v26
	ld2.2d	{ v22, v23 }, [x14]
	; InlineAsm Start
	pmull.1q	v24, v18, v21
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v18, v18, v21
	; InlineAsm End
	eor.16b	v21, v28, v16
	eor.16b	v16, v29, v17
	eor.16b	v17, v22, v30
	eor.16b	v22, v23, v31
	; InlineAsm Start
	pmull.1q	v23, v21, v16
	; InlineAsm End
	add	x16, x1, #224
	ld2.2d	{ v25, v26 }, [x16]
	; InlineAsm Start
	pmull.1q	v28, v27, v6
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v21, v16
	; InlineAsm End
	ld2.2d	{ v29, v30 }, [x15]
	; InlineAsm Start
	pmull2.1q	v6, v27, v6
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v21, v17, v22
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v17, v22
	; InlineAsm End
	eor.16b	v22, v29, v25
	eor.16b	v25, v30, v26
	eor.16b	v3, v3, v4
	; InlineAsm Start
	pmull.1q	v4, v22, v25
	; InlineAsm End
	eor3.16b	v3, v3, v28, v6
	eor3.16b	v3, v3, v5, v7
	eor3.16b	v3, v3, v20, v19
	eor3.16b	v3, v3, v24, v18
	eor3.16b	v3, v3, v23, v16
	; InlineAsm Start
	pmull2.1q	v5, v22, v25
	; InlineAsm End
	eor3.16b	v3, v3, v21, v17
	eor3.16b	v3, v3, v4, v5
	eor.16b	v4, v3, v10
	ext.16b	v4, v4, v4, #8
	; InlineAsm Start
	pmull.1q	v4, v4, v8
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v5, v4, v2
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v6, v5, v2
	; InlineAsm End
	eor3.16b	v4, v4, v6, v5
	eor3.16b	v8, v3, v10, v4
	add	x1, x1, #256
	sub	x8, x8, #256
	cmp	x8, #256
	b.hi	LBB3_6
LBB3_7:
	cmp	x8, #256
	b.ne	LBB3_9
; %bb.8:
	mov	x8, x1
	ld2.2d	{ v0, v1 }, [x8], #32
	mov	x9, x0
	ld2.2d	{ v2, v3 }, [x9], #32
	eor.16b	v4, v2, v0
	eor.16b	v1, v3, v1
	ld2.2d	{ v2, v3 }, [x8]
	; InlineAsm Start
	pmull.1q	v0, v4, v1
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v1, v4, v1
	; InlineAsm End
	ld2.2d	{ v4, v5 }, [x9]
	eor.16b	v6, v4, v2
	add	x8, x1, #64
	ld2.2d	{ v16, v17 }, [x8]
	eor.16b	v3, v5, v3
	add	x8, x0, #64
	ld2.2d	{ v4, v5 }, [x8]
	; InlineAsm Start
	pmull.1q	v2, v6, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v6, v3
	; InlineAsm End
	eor.16b	v6, v4, v16
	eor.16b	v5, v5, v17
	; InlineAsm Start
	pmull.1q	v4, v6, v5
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v5, v6, v5
	; InlineAsm End
	add	x8, x0, #96
	add	x9, x1, #96
	ld2.2d	{ v6, v7 }, [x9]
	ld2.2d	{ v16, v17 }, [x8]
	eor.16b	v18, v16, v6
	eor.16b	v7, v17, v7
	; InlineAsm Start
	pmull.1q	v6, v18, v7
	; InlineAsm End
	add	x8, x1, #128
	ld2.2d	{ v16, v17 }, [x8]
	; InlineAsm Start
	pmull2.1q	v7, v18, v7
	; InlineAsm End
	add	x8, x0, #128
	ld2.2d	{ v18, v19 }, [x8]
	eor.16b	v20, v18, v16
	add	x8, x1, #160
	ld2.2d	{ v21, v22 }, [x8]
	eor.16b	v16, v19, v17
	add	x8, x0, #160
	ld2.2d	{ v17, v18 }, [x8]
	; InlineAsm Start
	pmull.1q	v19, v20, v16
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v16, v20, v16
	; InlineAsm End
	eor.16b	v20, v17, v21
	eor.16b	v17, v18, v22
	; InlineAsm Start
	pmull.1q	v18, v20, v17
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v17, v20, v17
	; InlineAsm End
	add	x8, x0, #192
	add	x9, x1, #192
	ld2.2d	{ v20, v21 }, [x9]
	ld2.2d	{ v22, v23 }, [x8]
	eor.16b	v24, v22, v20
	eor.16b	v20, v23, v21
	; InlineAsm Start
	pmull.1q	v21, v24, v20
	; InlineAsm End
	add	x8, x1, #224
	ld2.2d	{ v22, v23 }, [x8]
	; InlineAsm Start
	pmull2.1q	v20, v24, v20
	; InlineAsm End
	add	x8, x0, #224
	ld2.2d	{ v24, v25 }, [x8]
	eor.16b	v26, v24, v22
	eor.16b	v22, v25, v23
	; InlineAsm Start
	pmull.1q	v23, v26, v22
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v22, v26, v22
	; InlineAsm End
	eor.16b	v0, v1, v0
	eor3.16b	v0, v0, v2, v3
	eor3.16b	v0, v0, v4, v5
	eor3.16b	v0, v0, v6, v7
	eor3.16b	v0, v0, v19, v16
	eor3.16b	v0, v0, v18, v17
	eor3.16b	v0, v0, v21, v20
	eor3.16b	v6, v0, v23, v22
	b	LBB3_19
LBB3_9:
	cmp	x8, #64
	b.lo	LBB3_13
; %bb.10:
	mov	x9, #0                          ; =0x0
	movi.2d	v7, #0000000000000000
	mov	x10, x0
	mov	x11, x1
	movi.2d	v16, #0000000000000000
LBB3_11:                                ; =>This Inner Loop Header: Depth=1
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
	b.hi	LBB3_11
; %bb.12:
	neg	x20, x9
	mov	x8, x12
	subs	x9, x12, #32
	b.hs	LBB3_14
	b	LBB3_15
LBB3_13:
	mov	x20, #0                         ; =0x0
	movi.2d	v16, #0000000000000000
	movi.2d	v7, #0000000000000000
	subs	x9, x8, #32
	b.lo	LBB3_15
LBB3_14:
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
LBB3_15:
	subs	x9, x8, #16
	b.hs	LBB3_21
; %bb.16:
	cbz	x8, LBB3_18
LBB3_17:
	stp	xzr, xzr, [x29, #-136]
	mov	x21, x0
	sub	x0, x29, #136
	add	x1, x1, x20
	mov	x22, x2
	mov	x2, x8
	str	q10, [sp, #32]                  ; 16-byte Folded Spill
	stur	q8, [x29, #-160]                ; 16-byte Folded Spill
	stp	q16, q7, [x29, #-208]           ; 32-byte Folded Spill
	bl	_memcpy
	ldp	q16, q7, [x29, #-208]           ; 32-byte Folded Reload
	ldur	q8, [x29, #-160]                ; 16-byte Folded Reload
	ldr	q10, [sp, #32]                  ; 16-byte Folded Reload
	mov	x0, x21
	mov	x2, x22
	and	x8, x20, #0xfffffffffffffff8
	add	x8, x21, x8
	ldp	d0, d1, [x29, #-136]
	ldp	d2, d3, [x8]
	eor.8b	v0, v2, v0
	eor.8b	v1, v3, v1
	; InlineAsm Start
	pmull.1q	v0, v0, v1
	; InlineAsm End
	eor.16b	v7, v0, v7
LBB3_18:
	eor.16b	v6, v7, v16
LBB3_19:
	ldp	d3, d4, [x0, #280]
	ldp	d1, d2, [x0, #296]
	ldp	d0, d5, [x0, #312]
	ldur	x8, [x29, #-120]
Lloh21:
	adrp	x9, ___stack_chk_guard@GOTPAGE
Lloh22:
	ldr	x9, [x9, ___stack_chk_guard@GOTPAGEOFF]
Lloh23:
	ldr	x9, [x9]
	cmp	x9, x8
	b.ne	LBB3_22
; %bb.20:
	fmov	d7, d10
	dup.2d	v16, x2
	eor.16b	v17, v16, v6
	eor3.16b	v6, v16, v6, v10
	eor3.16b	v16, v17, v10, v7
	ext.16b	v16, v16, v16, #8
	; InlineAsm Start
	pmull.1q	v16, v16, v8
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
	add	sp, sp, #1552
	ldp	x29, x30, [sp, #112]            ; 16-byte Folded Reload
	ldp	x20, x19, [sp, #96]             ; 16-byte Folded Reload
	ldp	x22, x21, [sp, #80]             ; 16-byte Folded Reload
	ldp	x28, x27, [sp, #64]             ; 16-byte Folded Reload
	ldp	d9, d8, [sp, #48]               ; 16-byte Folded Reload
	ldp	d11, d10, [sp, #32]             ; 16-byte Folded Reload
	ldp	d13, d12, [sp, #16]             ; 16-byte Folded Reload
	ldp	d15, d14, [sp], #128            ; 16-byte Folded Reload
	ret
LBB3_21:
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
	cbnz	x9, LBB3_17
	b	LBB3_18
LBB3_22:
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
	add	x8, x1, #64
	mov	x11, x1
	ld2.2d	{ v0, v1 }, [x11], #32
	add	x9, x1, #96
	add	x12, x1, #128
	add	x15, x0, #64
	add	x14, x0, #96
	add	x10, x0, #128
	add	x13, x0, #160
	mov	x16, x0
	ld2.2d	{ v2, v3 }, [x16], #32
	add	x17, x1, #160
	add	x2, x0, #192
	ld2.2d	{ v4, v5 }, [x11]
	add	x11, x1, #192
	add	x0, x0, #224
	ld2.2d	{ v6, v7 }, [x16]
	add	x16, x1, #224
	ld2.2d	{ v16, v17 }, [x8]
	eor.16b	v18, v2, v0
	eor.16b	v0, v3, v1
	ld2.2d	{ v1, v2 }, [x15]
	eor.16b	v3, v6, v4
	eor.16b	v4, v7, v5
	ld2.2d	{ v5, v6 }, [x9]
	eor.16b	v7, v1, v16
	eor.16b	v1, v2, v17
	ld2.2d	{ v16, v17 }, [x14]
	eor.16b	v2, v16, v5
	eor.16b	v5, v17, v6
	ld2.2d	{ v16, v17 }, [x12]
	; InlineAsm Start
	pmull.1q	v6, v18, v0
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v0, v18, v0
	; InlineAsm End
	eor.16b	v0, v6, v0
	; InlineAsm Start
	pmull.1q	v6, v3, v4
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v3, v4
	; InlineAsm End
	; InlineAsm Start
	pmull.1q	v4, v7, v1
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v1, v7, v1
	; InlineAsm End
	ld2.2d	{ v18, v19 }, [x10]
	eor3.16b	v0, v4, v0, v1
	; InlineAsm Start
	pmull.1q	v1, v2, v5
	; InlineAsm End
	eor3.16b	v1, v6, v3, v1
	ld2.2d	{ v3, v4 }, [x17]
	eor.16b	v6, v18, v16
	eor.16b	v7, v19, v17
	ld2.2d	{ v16, v17 }, [x13]
	; InlineAsm Start
	pmull.1q	v18, v6, v7
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v6, v6, v7
	; InlineAsm End
	eor3.16b	v0, v18, v0, v6
	; InlineAsm Start
	pmull2.1q	v2, v2, v5
	; InlineAsm End
	eor.16b	v5, v16, v3
	eor.16b	v3, v17, v4
	; InlineAsm Start
	pmull.1q	v4, v5, v3
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v3, v5, v3
	; InlineAsm End
	eor3.16b	v1, v1, v2, v4
	ld2.2d	{ v4, v5 }, [x11]
	ld2.2d	{ v6, v7 }, [x2]
	eor.16b	v2, v6, v4
	eor.16b	v4, v7, v5
	; InlineAsm Start
	pmull.1q	v5, v2, v4
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v2, v2, v4
	; InlineAsm End
	ld2.2d	{ v6, v7 }, [x16]
	eor3.16b	v0, v5, v0, v2
	ld2.2d	{ v4, v5 }, [x0]
	eor.16b	v2, v4, v6
	eor.16b	v4, v5, v7
	; InlineAsm Start
	pmull.1q	v5, v2, v4
	; InlineAsm End
	; InlineAsm Start
	pmull2.1q	v2, v2, v4
	; InlineAsm End
	eor3.16b	v1, v1, v3, v5
	eor3.16b	v0, v1, v2, v0
	ret
	.cfi_endproc
                                        ; -- End function
.subsections_via_symbols
