
/home/thomas-ahle/agents/speedbench-museair-v2/build/museair_v2.o:     file format elf64-x86-64


Disassembly of section .text:

0000000000000000 <_ZL10museair_v2PKvmmPv>:
   0:	41 57                	push   %r15
   2:	49 89 f7             	mov    %rsi,%r15
   5:	48 89 d6             	mov    %rdx,%rsi
   8:	41 56                	push   %r14
   a:	41 55                	push   %r13
   c:	41 54                	push   %r12
   e:	55                   	push   %rbp
   f:	53                   	push   %rbx
  10:	48 89 4c 24 c0       	mov    %rcx,-0x40(%rsp)
  15:	49 83 ff 20          	cmp    $0x20,%r15
  19:	0f 86 f9 02 00 00    	jbe    318 <_ZL10museair_v2PKvmmPv+0x318>
  1f:	62 f2 fd 08 7c c6    	vpbroadcastq %rsi,%xmm0
  25:	c5 f9 db 05 00 00 00 	vpand  0x0(%rip),%xmm0,%xmm0        # 2d <_ZL10museair_v2PKvmmPv+0x2d>
  2c:	00 
  2d:	48 ba aa aa aa aa aa 	movabs $0xaaaaaaaaaaaaaaaa,%rdx
  34:	aa aa aa 
  37:	48 21 f2             	and    %rsi,%rdx
  3a:	48 b8 7a e1 56 9c 58 	movabs $0x5ae31e589c56e17a,%rax
  41:	1e e3 5a 
  44:	48 b9 55 55 55 55 55 	movabs $0x5555555555555555,%rcx
  4b:	55 55 55 
  4e:	c5 f9 ef 0d 00 00 00 	vpxor  0x0(%rip),%xmm0,%xmm1        # 56 <_ZL10museair_v2PKvmmPv+0x56>
  55:	00 
  56:	48 31 c2             	xor    %rax,%rdx
  59:	c5 f9 ef 05 00 00 00 	vpxor  0x0(%rip),%xmm0,%xmm0        # 61 <_ZL10museair_v2PKvmmPv+0x61>
  60:	00 
  61:	48 21 f1             	and    %rsi,%rcx
  64:	48 b8 28 ee bc c0 90 	movabs $0xd24f2590c0bcee28,%rax
  6b:	25 4f d2 
  6e:	48 31 c1             	xor    %rax,%rcx
  71:	4a 8d 44 3f e0       	lea    -0x20(%rdi,%r15,1),%rax
  76:	c4 e1 fb 92 c0       	kmovq  %rax,%k0
  7b:	48 89 54 24 c8       	mov    %rdx,-0x38(%rsp)
  80:	48 89 4c 24 f0       	mov    %rcx,-0x10(%rsp)
  85:	c5 fa 7f 44 24 e0    	vmovdqu %xmm0,-0x20(%rsp)
  8b:	c4 c1 f9 7e ce       	vmovq  %xmm1,%r14
  90:	c4 e3 f9 16 c5 01    	vpextrq $0x1,%xmm0,%rbp
  96:	c4 e3 f9 16 cb 01    	vpextrq $0x1,%xmm1,%rbx
  9c:	c4 e1 f9 7e c0       	vmovq  %xmm0,%rax
  a1:	4c 89 fe             	mov    %r15,%rsi
  a4:	49 83 ff 60          	cmp    $0x60,%r15
  a8:	77 66                	ja     110 <_ZL10museair_v2PKvmmPv+0x110>
  aa:	48 8b 17             	mov    (%rdi),%rdx
  ad:	4c 33 77 08          	xor    0x8(%rdi),%r14
  b1:	48 33 54 24 c8       	xor    -0x38(%rsp),%rdx
  b6:	c4 42 bb f6 ce       	mulx   %r14,%r8,%r9
  bb:	4d 89 f2             	mov    %r14,%r10
  be:	48 89 54 24 a8       	mov    %rdx,-0x58(%rsp)
  c3:	4c 89 c9             	mov    %r9,%rcx
  c6:	4c 31 c1             	xor    %r8,%rcx
  c9:	49 89 ce             	mov    %rcx,%r14
  cc:	48 83 fe 30          	cmp    $0x30,%rsi
  d0:	0f 86 12 03 00 00    	jbe    3e8 <_ZL10museair_v2PKvmmPv+0x3e8>
  d6:	48 33 5f 18          	xor    0x18(%rdi),%rbx
  da:	48 89 da             	mov    %rbx,%rdx
  dd:	4c 33 57 10          	xor    0x10(%rdi),%r10
  e1:	c4 c2 eb f6 ca       	mulx   %r10,%rdx,%rcx
  e6:	49 89 db             	mov    %rbx,%r11
  e9:	48 89 cb             	mov    %rcx,%rbx
  ec:	48 31 d3             	xor    %rdx,%rbx
  ef:	48 83 fe 40          	cmp    $0x40,%rsi
  f3:	0f 87 87 03 00 00    	ja     480 <_ZL10museair_v2PKvmmPv+0x480>
  f9:	49 89 ed             	mov    %rbp,%r13
  fc:	48 8b 4c 24 f0       	mov    -0x10(%rsp),%rcx
 101:	4c 8b 64 24 e0       	mov    -0x20(%rsp),%r12
 106:	48 8b 6c 24 e8       	mov    -0x18(%rsp),%rbp
 10b:	e9 1c 01 00 00       	jmpq   22c <_ZL10museair_v2PKvmmPv+0x22c>
 110:	49 b9 d8 16 60 bb 71 	movabs $0x33ea8f71bb6016d8,%r9
 117:	8f ea 33 
 11a:	4c 89 4c 24 b8       	mov    %r9,-0x48(%rsp)
 11f:	48 33 17             	xor    (%rdi),%rdx
 122:	4c 33 77 08          	xor    0x8(%rdi),%r14
 126:	c4 42 a3 f6 e6       	mulx   %r14,%r11,%r12
 12b:	4c 33 77 10          	xor    0x10(%rdi),%r14
 12f:	48 33 5f 18          	xor    0x18(%rdi),%rbx
 133:	49 89 d1             	mov    %rdx,%r9
 136:	4c 89 64 24 b0       	mov    %r12,-0x50(%rsp)
 13b:	4c 89 f2             	mov    %r14,%rdx
 13e:	c4 62 9b f6 eb       	mulx   %rbx,%r12,%r13
 143:	4c 8b 54 24 b0       	mov    -0x50(%rsp),%r10
 148:	48 33 5f 20          	xor    0x20(%rdi),%rbx
 14c:	48 33 47 28          	xor    0x28(%rdi),%rax
 150:	4d 31 e2             	xor    %r12,%r10
 153:	48 89 da             	mov    %rbx,%rdx
 156:	4c 89 5c 24 a8       	mov    %r11,-0x58(%rsp)
 15b:	4d 29 d6             	sub    %r10,%r14
 15e:	c4 62 ab f6 d8       	mulx   %rax,%r10,%r11
 163:	48 33 47 30          	xor    0x30(%rdi),%rax
 167:	48 33 6f 38          	xor    0x38(%rdi),%rbp
 16b:	48 89 c2             	mov    %rax,%rdx
 16e:	4d 89 e8             	mov    %r13,%r8
 171:	c4 62 9b f6 ed       	mulx   %rbp,%r12,%r13
 176:	4d 31 d0             	xor    %r10,%r8
 179:	4d 89 da             	mov    %r11,%r10
 17c:	48 33 6f 40          	xor    0x40(%rdi),%rbp
 180:	4d 31 e2             	xor    %r12,%r10
 183:	4c 8b 64 24 a8       	mov    -0x58(%rsp),%r12
 188:	48 33 4f 48          	xor    0x48(%rdi),%rcx
 18c:	48 89 ea             	mov    %rbp,%rdx
 18f:	4c 33 64 24 b8       	xor    -0x48(%rsp),%r12
 194:	4c 29 d0             	sub    %r10,%rax
 197:	c4 62 ab f6 d9       	mulx   %rcx,%r10,%r11
 19c:	4c 89 ca             	mov    %r9,%rdx
 19f:	4c 29 e2             	sub    %r12,%rdx
 1a2:	48 33 4f 50          	xor    0x50(%rdi),%rcx
 1a6:	48 33 57 58          	xor    0x58(%rdi),%rdx
 1aa:	4c 29 c3             	sub    %r8,%rbx
 1ad:	4d 89 e8             	mov    %r13,%r8
 1b0:	c4 62 9b f6 e9       	mulx   %rcx,%r12,%r13
 1b5:	4d 31 d0             	xor    %r10,%r8
 1b8:	4c 29 c5             	sub    %r8,%rbp
 1bb:	48 83 ee 60          	sub    $0x60,%rsi
 1bf:	4d 89 e0             	mov    %r12,%r8
 1c2:	4d 31 d8             	xor    %r11,%r8
 1c5:	4c 89 6c 24 b8       	mov    %r13,-0x48(%rsp)
 1ca:	4c 29 c1             	sub    %r8,%rcx
 1cd:	48 83 c7 60          	add    $0x60,%rdi
 1d1:	48 83 fe 60          	cmp    $0x60,%rsi
 1d5:	0f 87 44 ff ff ff    	ja     11f <_ZL10museair_v2PKvmmPv+0x11f>
 1db:	4c 31 ea             	xor    %r13,%rdx
 1de:	c4 e1 f9 6e d2       	vmovq  %rdx,%xmm2
 1e3:	c4 c3 e9 22 c6 01    	vpinsrq $0x1,%r14,%xmm2,%xmm0
 1e9:	c4 e1 f9 6e db       	vmovq  %rbx,%xmm3
 1ee:	c5 f9 7f 44 24 c8    	vmovdqa %xmm0,-0x38(%rsp)
 1f4:	c4 e1 f9 6e e5       	vmovq  %rbp,%xmm4
 1f9:	c4 e3 e1 22 c0 01    	vpinsrq $0x1,%rax,%xmm3,%xmm0
 1ff:	c5 f9 7f 44 24 d8    	vmovdqa %xmm0,-0x28(%rsp)
 205:	c4 e3 d9 22 c1 01    	vpinsrq $0x1,%rcx,%xmm4,%xmm0
 20b:	48 89 54 24 a8       	mov    %rdx,-0x58(%rsp)
 210:	c5 f9 7f 44 24 e8    	vmovdqa %xmm0,-0x18(%rsp)
 216:	48 83 fe 20          	cmp    $0x20,%rsi
 21a:	0f 87 8a fe ff ff    	ja     aa <_ZL10museair_v2PKvmmPv+0xaa>
 220:	49 89 ed             	mov    %rbp,%r13
 223:	49 89 c4             	mov    %rax,%r12
 226:	49 89 db             	mov    %rbx,%r11
 229:	4d 89 f2             	mov    %r14,%r10
 22c:	c4 e1 fb 93 f0       	kmovq  %k0,%rsi
 231:	48 33 2e             	xor    (%rsi),%rbp
 234:	48 89 ea             	mov    %rbp,%rdx
 237:	48 33 4e 08          	xor    0x8(%rsi),%rcx
 23b:	c4 e2 cb f6 f9       	mulx   %rcx,%rsi,%rdi
 240:	48 8b 54 24 a8       	mov    -0x58(%rsp),%rdx
 245:	4d 29 e3             	sub    %r12,%r11
 248:	49 89 f0             	mov    %rsi,%r8
 24b:	c4 e1 fb 93 f0       	kmovq  %k0,%rsi
 250:	48 33 4e 10          	xor    0x10(%rsi),%rcx
 254:	48 33 56 18          	xor    0x18(%rsi),%rdx
 258:	49 89 f9             	mov    %rdi,%r9
 25b:	c4 e2 cb f6 f9       	mulx   %rcx,%rsi,%rdi
 260:	48 29 cd             	sub    %rcx,%rbp
 263:	4c 29 d2             	sub    %r10,%rdx
 266:	48 b9 d1 29 a4 17 93 	movabs $0x4acea09317a429d1,%rcx
 26d:	a0 ce 4a 
 270:	48 89 7c 24 b0       	mov    %rdi,-0x50(%rsp)
 275:	49 ba 1f a0 d0 95 75 	movabs $0xb5d2697595d0a01f,%r10
 27c:	69 d2 b5 
 27f:	4c 31 d2             	xor    %r10,%rdx
 282:	48 31 cd             	xor    %rcx,%rbp
 285:	4d 89 da             	mov    %r11,%r10
 288:	44 89 f9             	mov    %r15d,%ecx
 28b:	49 bb 4f 2b 0e f0 32 	movabs $0x9bb30a32f00e2b4f,%r11
 292:	0a b3 9b 
 295:	48 89 74 24 a8       	mov    %rsi,-0x58(%rsp)
 29a:	48 8b 7c 24 b0       	mov    -0x50(%rsp),%rdi
 29f:	4d 31 da             	xor    %r11,%r10
 2a2:	83 e1 3f             	and    $0x3f,%ecx
 2a5:	74 06                	je     2ad <_ZL10museair_v2PKvmmPv+0x2ad>
 2a7:	48 d3 c2             	rol    %cl,%rdx
 2aa:	49 d3 ca             	ror    %cl,%r10
 2ad:	4d 31 c8             	xor    %r9,%r8
 2b0:	48 31 fe             	xor    %rdi,%rsi
 2b3:	4d 01 e8             	add    %r13,%r8
 2b6:	4c 01 f6             	add    %r14,%rsi
 2b9:	48 01 c3             	add    %rax,%rbx
 2bc:	49 29 f2             	sub    %rsi,%r10
 2bf:	4c 29 c2             	sub    %r8,%rdx
 2c2:	4c 01 fb             	add    %r15,%rbx
 2c5:	c4 42 bb f6 ca       	mulx   %r10,%r8,%r9
 2ca:	49 89 d3             	mov    %rdx,%r11
 2cd:	48 29 dd             	sub    %rbx,%rbp
 2d0:	4c 89 d2             	mov    %r10,%rdx
 2d3:	c4 e2 cb f6 fd       	mulx   %rbp,%rsi,%rdi
 2d8:	4c 89 da             	mov    %r11,%rdx
 2db:	c4 e2 f3 f6 dd       	mulx   %rbp,%rcx,%rbx
 2e0:	4b 8d 14 13          	lea    (%r11,%r10,1),%rdx
 2e4:	48 01 ea             	add    %rbp,%rdx
 2e7:	48 89 d8             	mov    %rbx,%rax
 2ea:	4c 31 c0             	xor    %r8,%rax
 2ed:	48 29 c2             	sub    %rax,%rdx
 2f0:	4c 89 c8             	mov    %r9,%rax
 2f3:	48 31 f0             	xor    %rsi,%rax
 2f6:	48 29 c2             	sub    %rax,%rdx
 2f9:	48 89 f8             	mov    %rdi,%rax
 2fc:	48 31 c8             	xor    %rcx,%rax
 2ff:	48 29 c2             	sub    %rax,%rdx
 302:	48 8b 44 24 c0       	mov    -0x40(%rsp),%rax
 307:	48 89 10             	mov    %rdx,(%rax)
 30a:	5b                   	pop    %rbx
 30b:	5d                   	pop    %rbp
 30c:	41 5c                	pop    %r12
 30e:	41 5d                	pop    %r13
 310:	41 5e                	pop    %r14
 312:	41 5f                	pop    %r15
 314:	c3                   	retq   
 315:	0f 1f 00             	nopl   (%rax)
 318:	b8 10 00 00 00       	mov    $0x10,%eax
 31d:	49 39 c7             	cmp    %rax,%r15
 320:	49 0f 46 c7          	cmovbe %r15,%rax
 324:	49 89 c0             	mov    %rax,%r8
 327:	4c 89 f8             	mov    %r15,%rax
 32a:	48 31 d0             	xor    %rdx,%rax
 32d:	48 ba 64 eb f9 26 6b 	movabs $0x7ab1006b26f9eb64,%rdx
 334:	00 b1 7a 
 337:	48 31 d0             	xor    %rdx,%rax
 33a:	48 ba 57 84 0b 22 94 	movabs $0x21233394220b8457,%rdx
 341:	33 23 21 
 344:	4c 31 fa             	xor    %r15,%rdx
 347:	c4 e2 fb f6 d0       	mulx   %rax,%rax,%rdx
 34c:	48 89 d1             	mov    %rdx,%rcx
 34f:	49 83 ff 07          	cmp    $0x7,%r15
 353:	0f 86 a7 00 00 00    	jbe    400 <_ZL10museair_v2PKvmmPv+0x400>
 359:	48 33 07             	xor    (%rdi),%rax
 35c:	49 89 c2             	mov    %rax,%r10
 35f:	4a 33 4c 07 f8       	xor    -0x8(%rdi,%r8,1),%rcx
 364:	49 83 ff 10          	cmp    $0x10,%r15
 368:	0f 86 ad 00 00 00    	jbe    41b <_ZL10museair_v2PKvmmPv+0x41b>
 36e:	49 8d 47 f0          	lea    -0x10(%r15),%rax
 372:	48 8d 57 10          	lea    0x10(%rdi),%rdx
 376:	48 83 f8 07          	cmp    $0x7,%rax
 37a:	0f 86 78 01 00 00    	jbe    4f8 <_ZL10museair_v2PKvmmPv+0x4f8>
 380:	4c 8b 47 10          	mov    0x10(%rdi),%r8
 384:	4a 8b 5c 3a e8       	mov    -0x18(%rdx,%r15,1),%rbx
 389:	49 31 f0             	xor    %rsi,%r8
 38c:	48 b8 43 3b 9f 7c 55 	movabs $0x47cb9557c9f3b43,%rax
 393:	b9 7c 04 
 396:	49 31 c0             	xor    %rax,%r8
 399:	4c 89 c2             	mov    %r8,%rdx
 39c:	48 b8 28 ee bc c0 90 	movabs $0xd24f2590c0bcee28,%rax
 3a3:	25 4f d2 
 3a6:	c4 62 bb f6 c8       	mulx   %rax,%r8,%r9
 3ab:	48 89 f0             	mov    %rsi,%rax
 3ae:	48 31 d8             	xor    %rbx,%rax
 3b1:	48 ba d8 16 60 bb 71 	movabs $0x33ea8f71bb6016d8,%rdx
 3b8:	8f ea 33 
 3bb:	48 31 d0             	xor    %rdx,%rax
 3be:	48 ba 1f a0 d0 95 75 	movabs $0xb5d2697595d0a01f,%rdx
 3c5:	69 d2 b5 
 3c8:	c4 e2 e3 f6 f0       	mulx   %rax,%rbx,%rsi
 3cd:	4d 31 c2             	xor    %r8,%r10
 3d0:	48 89 d8             	mov    %rbx,%rax
 3d3:	48 31 c8             	xor    %rcx,%rax
 3d6:	4c 31 c8             	xor    %r9,%rax
 3d9:	49 31 f2             	xor    %rsi,%r10
 3dc:	48 89 c1             	mov    %rax,%rcx
 3df:	eb 3a                	jmp    41b <_ZL10museair_v2PKvmmPv+0x41b>
 3e1:	0f 1f 80 00 00 00 00 	nopl   0x0(%rax)
 3e8:	48 8b 4c 24 f0       	mov    -0x10(%rsp),%rcx
 3ed:	49 89 ed             	mov    %rbp,%r13
 3f0:	49 89 c4             	mov    %rax,%r12
 3f3:	49 89 db             	mov    %rbx,%r11
 3f6:	e9 31 fe ff ff       	jmpq   22c <_ZL10museair_v2PKvmmPv+0x22c>
 3fb:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)
 400:	49 83 ff 03          	cmp    $0x3,%r15
 404:	0f 86 be 00 00 00    	jbe    4c8 <_ZL10museair_v2PKvmmPv+0x4c8>
 40a:	44 8b 3f             	mov    (%rdi),%r15d
 40d:	42 8b 54 07 fc       	mov    -0x4(%rdi,%r8,1),%edx
 412:	49 31 c7             	xor    %rax,%r15
 415:	4d 89 fa             	mov    %r15,%r10
 418:	48 31 d1             	xor    %rdx,%rcx
 41b:	48 be 4f 2b 0e f0 32 	movabs $0x9bb30a32f00e2b4f,%rsi
 422:	0a b3 9b 
 425:	4c 31 d6             	xor    %r10,%rsi
 428:	48 b8 d1 29 a4 17 93 	movabs $0x4acea09317a429d1,%rax
 42f:	a0 ce 4a 
 432:	48 31 c8             	xor    %rcx,%rax
 435:	48 89 f2             	mov    %rsi,%rdx
 438:	c4 e2 cb f6 f8       	mulx   %rax,%rsi,%rdi
 43d:	48 b8 42 2a 57 85 a7 	movabs $0xfda811a785572a42,%rax
 444:	11 a8 fd 
 447:	49 29 f2             	sub    %rsi,%r10
 44a:	48 be c6 45 d5 fd 5d 	movabs $0xc2b2435dfdd545c6,%rsi
 451:	43 b2 c2 
 454:	48 29 f9             	sub    %rdi,%rcx
 457:	4c 31 d6             	xor    %r10,%rsi
 45a:	48 31 c8             	xor    %rcx,%rax
 45d:	48 89 f2             	mov    %rsi,%rdx
 460:	c4 e2 cb f6 f8       	mulx   %rax,%rsi,%rdi
 465:	48 29 f9             	sub    %rdi,%rcx
 468:	49 29 f2             	sub    %rsi,%r10
 46b:	4c 31 d1             	xor    %r10,%rcx
 46e:	48 89 ca             	mov    %rcx,%rdx
 471:	e9 8c fe ff ff       	jmpq   302 <_ZL10museair_v2PKvmmPv+0x302>
 476:	66 2e 0f 1f 84 00 00 	nopw   %cs:0x0(%rax,%rax,1)
 47d:	00 00 00 
 480:	48 33 47 28          	xor    0x28(%rdi),%rax
 484:	48 89 c2             	mov    %rax,%rdx
 487:	4c 33 5f 20          	xor    0x20(%rdi),%r11
 48b:	c4 42 bb f6 cb       	mulx   %r11,%r8,%r9
 490:	49 89 c4             	mov    %rax,%r12
 493:	4c 89 c8             	mov    %r9,%rax
 496:	4c 31 c0             	xor    %r8,%rax
 499:	48 83 fe 50          	cmp    $0x50,%rsi
 49d:	76 6d                	jbe    50c <_ZL10museair_v2PKvmmPv+0x50c>
 49f:	4c 33 67 30          	xor    0x30(%rdi),%r12
 4a3:	48 33 6f 38          	xor    0x38(%rdi),%rbp
 4a7:	4c 89 e2             	mov    %r12,%rdx
 4aa:	c4 62 bb f6 cd       	mulx   %rbp,%r8,%r9
 4af:	48 8b 4c 24 f0       	mov    -0x10(%rsp),%rcx
 4b4:	4c 89 cf             	mov    %r9,%rdi
 4b7:	4c 31 c7             	xor    %r8,%rdi
 4ba:	49 89 fd             	mov    %rdi,%r13
 4bd:	e9 6a fd ff ff       	jmpq   22c <_ZL10museair_v2PKvmmPv+0x22c>
 4c2:	66 0f 1f 44 00 00    	nopw   0x0(%rax,%rax,1)
 4c8:	31 d2                	xor    %edx,%edx
 4ca:	4d 85 ff             	test   %r15,%r15
 4cd:	0f 84 3f ff ff ff    	je     412 <_ZL10museair_v2PKvmmPv+0x412>
 4d3:	0f b6 17             	movzbl (%rdi),%edx
 4d6:	42 0f b6 74 07 ff    	movzbl -0x1(%rdi,%r8,1),%esi
 4dc:	48 c1 e2 30          	shl    $0x30,%rdx
 4e0:	48 09 d6             	or     %rdx,%rsi
 4e3:	49 d1 e8             	shr    %r8
 4e6:	42 0f b6 14 07       	movzbl (%rdi,%r8,1),%edx
 4eb:	49 89 f7             	mov    %rsi,%r15
 4ee:	e9 1f ff ff ff       	jmpq   412 <_ZL10museair_v2PKvmmPv+0x412>
 4f3:	0f 1f 44 00 00       	nopl   0x0(%rax,%rax,1)
 4f8:	48 83 f8 03          	cmp    $0x3,%rax
 4fc:	76 20                	jbe    51e <_ZL10museair_v2PKvmmPv+0x51e>
 4fe:	44 8b 47 10          	mov    0x10(%rdi),%r8d
 502:	42 8b 5c 3a ec       	mov    -0x14(%rdx,%r15,1),%ebx
 507:	e9 7d fe ff ff       	jmpq   389 <_ZL10museair_v2PKvmmPv+0x389>
 50c:	49 89 ed             	mov    %rbp,%r13
 50f:	48 8b 4c 24 f0       	mov    -0x10(%rsp),%rcx
 514:	48 8b 6c 24 e8       	mov    -0x18(%rsp),%rbp
 519:	e9 0e fd ff ff       	jmpq   22c <_ZL10museair_v2PKvmmPv+0x22c>
 51e:	0f b6 7f 10          	movzbl 0x10(%rdi),%edi
 522:	46 0f b6 44 3a ef    	movzbl -0x11(%rdx,%r15,1),%r8d
 528:	48 c1 e7 30          	shl    $0x30,%rdi
 52c:	48 d1 e8             	shr    %rax
 52f:	0f b6 1c 02          	movzbl (%rdx,%rax,1),%ebx
 533:	49 09 f8             	or     %rdi,%r8
 536:	e9 4e fe ff ff       	jmpq   389 <_ZL10museair_v2PKvmmPv+0x389>
 53b:	90                   	nop
 53c:	0f 1f 40 00          	nopl   0x0(%rax)

0000000000000540 <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0>:
 540:	41 57                	push   %r15
 542:	41 56                	push   %r14
 544:	41 55                	push   %r13
 546:	41 54                	push   %r12
 548:	55                   	push   %rbp
 549:	53                   	push   %rbx
 54a:	48 83 ec 28          	sub    $0x28,%rsp
 54e:	48 89 7c 24 10       	mov    %rdi,0x10(%rsp)
 553:	48 85 ff             	test   %rdi,%rdi
 556:	0f 84 9b 01 00 00    	je     6f7 <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0x1b7>
 55c:	48 8b 44 24 10       	mov    0x10(%rsp),%rax
 561:	4c 8b 70 18          	mov    0x18(%rax),%r14
 565:	4d 85 f6             	test   %r14,%r14
 568:	0f 84 67 01 00 00    	je     6d5 <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0x195>
 56e:	4d 8b 7e 18          	mov    0x18(%r14),%r15
 572:	4d 85 ff             	test   %r15,%r15
 575:	0f 84 3c 01 00 00    	je     6b7 <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0x177>
 57b:	49 8b 47 18          	mov    0x18(%r15),%rax
 57f:	48 89 44 24 08       	mov    %rax,0x8(%rsp)
 584:	48 85 c0             	test   %rax,%rax
 587:	0f 84 0c 01 00 00    	je     699 <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0x159>
 58d:	48 8b 44 24 08       	mov    0x8(%rsp),%rax
 592:	48 8b 68 18          	mov    0x18(%rax),%rbp
 596:	48 85 ed             	test   %rbp,%rbp
 599:	0f 84 af 00 00 00    	je     64e <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0x10e>
 59f:	4c 8b 6d 18          	mov    0x18(%rbp),%r13
 5a3:	4d 85 ed             	test   %r13,%r13
 5a6:	74 64                	je     60c <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0xcc>
 5a8:	4d 8b 65 18          	mov    0x18(%r13),%r12
 5ac:	4d 85 e4             	test   %r12,%r12
 5af:	74 7f                	je     630 <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0xf0>
 5b1:	4d 8b 4c 24 18       	mov    0x18(%r12),%r9
 5b6:	4d 85 c9             	test   %r9,%r9
 5b9:	0f 84 b1 00 00 00    	je     670 <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0x130>
 5bf:	49 8b 59 18          	mov    0x18(%r9),%rbx
 5c3:	48 85 db             	test   %rbx,%rbx
 5c6:	74 29                	je     5f1 <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0xb1>
 5c8:	48 8b 7b 18          	mov    0x18(%rbx),%rdi
 5cc:	4c 89 4c 24 18       	mov    %r9,0x18(%rsp)
 5d1:	e8 6a ff ff ff       	callq  540 <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0>
 5d6:	48 89 df             	mov    %rbx,%rdi
 5d9:	48 8b 5b 10          	mov    0x10(%rbx),%rbx
 5dd:	be 28 00 00 00       	mov    $0x28,%esi
 5e2:	e8 00 00 00 00       	callq  5e7 <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0xa7>
 5e7:	48 85 db             	test   %rbx,%rbx
 5ea:	4c 8b 4c 24 18       	mov    0x18(%rsp),%r9
 5ef:	75 d7                	jne    5c8 <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0x88>
 5f1:	49 8b 59 10          	mov    0x10(%r9),%rbx
 5f5:	be 28 00 00 00       	mov    $0x28,%esi
 5fa:	4c 89 cf             	mov    %r9,%rdi
 5fd:	e8 00 00 00 00       	callq  602 <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0xc2>
 602:	48 85 db             	test   %rbx,%rbx
 605:	74 69                	je     670 <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0x130>
 607:	49 89 d9             	mov    %rbx,%r9
 60a:	eb b3                	jmp    5bf <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0x7f>
 60c:	4c 8b 65 10          	mov    0x10(%rbp),%r12
 610:	be 28 00 00 00       	mov    $0x28,%esi
 615:	48 89 ef             	mov    %rbp,%rdi
 618:	e8 00 00 00 00       	callq  61d <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0xdd>
 61d:	4d 85 e4             	test   %r12,%r12
 620:	74 2c                	je     64e <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0x10e>
 622:	4c 89 e5             	mov    %r12,%rbp
 625:	e9 75 ff ff ff       	jmpq   59f <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0x5f>
 62a:	66 0f 1f 44 00 00    	nopw   0x0(%rax,%rax,1)
 630:	4d 8b 65 10          	mov    0x10(%r13),%r12
 634:	be 28 00 00 00       	mov    $0x28,%esi
 639:	4c 89 ef             	mov    %r13,%rdi
 63c:	e8 00 00 00 00       	callq  641 <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0x101>
 641:	4d 85 e4             	test   %r12,%r12
 644:	74 c6                	je     60c <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0xcc>
 646:	4d 89 e5             	mov    %r12,%r13
 649:	e9 5a ff ff ff       	jmpq   5a8 <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0x68>
 64e:	48 8b 7c 24 08       	mov    0x8(%rsp),%rdi
 653:	be 28 00 00 00       	mov    $0x28,%esi
 658:	48 8b 6f 10          	mov    0x10(%rdi),%rbp
 65c:	e8 00 00 00 00       	callq  661 <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0x121>
 661:	48 85 ed             	test   %rbp,%rbp
 664:	74 33                	je     699 <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0x159>
 666:	48 89 6c 24 08       	mov    %rbp,0x8(%rsp)
 66b:	e9 1d ff ff ff       	jmpq   58d <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0x4d>
 670:	49 8b 44 24 10       	mov    0x10(%r12),%rax
 675:	be 28 00 00 00       	mov    $0x28,%esi
 67a:	4c 89 e7             	mov    %r12,%rdi
 67d:	48 89 44 24 18       	mov    %rax,0x18(%rsp)
 682:	e8 00 00 00 00       	callq  687 <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0x147>
 687:	48 8b 44 24 18       	mov    0x18(%rsp),%rax
 68c:	48 85 c0             	test   %rax,%rax
 68f:	74 9f                	je     630 <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0xf0>
 691:	49 89 c4             	mov    %rax,%r12
 694:	e9 18 ff ff ff       	jmpq   5b1 <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0x71>
 699:	49 8b 5f 10          	mov    0x10(%r15),%rbx
 69d:	be 28 00 00 00       	mov    $0x28,%esi
 6a2:	4c 89 ff             	mov    %r15,%rdi
 6a5:	e8 00 00 00 00       	callq  6aa <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0x16a>
 6aa:	48 85 db             	test   %rbx,%rbx
 6ad:	74 08                	je     6b7 <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0x177>
 6af:	49 89 df             	mov    %rbx,%r15
 6b2:	e9 c4 fe ff ff       	jmpq   57b <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0x3b>
 6b7:	49 8b 5e 10          	mov    0x10(%r14),%rbx
 6bb:	be 28 00 00 00       	mov    $0x28,%esi
 6c0:	4c 89 f7             	mov    %r14,%rdi
 6c3:	e8 00 00 00 00       	callq  6c8 <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0x188>
 6c8:	48 85 db             	test   %rbx,%rbx
 6cb:	74 08                	je     6d5 <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0x195>
 6cd:	49 89 de             	mov    %rbx,%r14
 6d0:	e9 99 fe ff ff       	jmpq   56e <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0x2e>
 6d5:	48 8b 7c 24 10       	mov    0x10(%rsp),%rdi
 6da:	be 28 00 00 00       	mov    $0x28,%esi
 6df:	48 8b 5f 10          	mov    0x10(%rdi),%rbx
 6e3:	e8 00 00 00 00       	callq  6e8 <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0x1a8>
 6e8:	48 85 db             	test   %rbx,%rbx
 6eb:	74 0a                	je     6f7 <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0x1b7>
 6ed:	48 89 5c 24 10       	mov    %rbx,0x10(%rsp)
 6f2:	e9 65 fe ff ff       	jmpq   55c <_ZNSt8_Rb_treeImmSt9_IdentityImESt4lessImESaImEE8_M_eraseEPSt13_Rb_tree_nodeImE.isra.0+0x1c>
 6f7:	48 83 c4 28          	add    $0x28,%rsp
 6fb:	5b                   	pop    %rbx
 6fc:	5d                   	pop    %rbp
 6fd:	41 5c                	pop    %r12
 6ff:	41 5d                	pop    %r13
 701:	41 5e                	pop    %r14
 703:	41 5f                	pop    %r15
 705:	c3                   	retq   

Disassembly of section .text._ZN8HashInfoD2Ev:

0000000000000000 <_ZN8HashInfoD1Ev>:
   0:	53                   	push   %rbx
   1:	48 89 fb             	mov    %rdi,%rbx
   4:	48 8b 3f             	mov    (%rdi),%rdi
   7:	e8 00 00 00 00       	callq  c <_ZN8HashInfoD1Ev+0xc>
   c:	48 8b 5b 78          	mov    0x78(%rbx),%rbx
  10:	48 85 db             	test   %rbx,%rbx
  13:	74 1f                	je     34 <_ZN8HashInfoD1Ev+0x34>
  15:	48 8b 7b 18          	mov    0x18(%rbx),%rdi
  19:	e8 00 00 00 00       	callq  1e <_ZN8HashInfoD1Ev+0x1e>
  1e:	48 89 df             	mov    %rbx,%rdi
  21:	48 8b 5b 10          	mov    0x10(%rbx),%rbx
  25:	be 28 00 00 00       	mov    $0x28,%esi
  2a:	e8 00 00 00 00       	callq  2f <_ZN8HashInfoD1Ev+0x2f>
  2f:	48 85 db             	test   %rbx,%rbx
  32:	75 e1                	jne    15 <_ZN8HashInfoD1Ev+0x15>
  34:	5b                   	pop    %rbx
  35:	c3                   	retq   

Disassembly of section .text.unlikely:

0000000000000000 <_GLOBAL__sub_I_museair_v2_ref.cold>:
   0:	48 8b 3d 00 00 00 00 	mov    0x0(%rip),%rdi        # 7 <_GLOBAL__sub_I_museair_v2_ref.cold+0x7>
   7:	c5 f8 77             	vzeroupper 
   a:	e8 00 00 00 00       	callq  f <_GLOBAL__sub_I_museair_v2_ref.cold+0xf>
   f:	48 8b 3d 00 00 00 00 	mov    0x0(%rip),%rdi        # 16 <_GLOBAL__sub_I_museair_v2_ref.cold+0x16>
  16:	e8 00 00 00 00       	callq  1b <_GLOBAL__sub_I_museair_v2_ref.cold+0x1b>
  1b:	48 89 ef             	mov    %rbp,%rdi
  1e:	e8 00 00 00 00       	callq  23 <_ZL15Hash_MuseAir_v2+0x3>

Disassembly of section .text.startup:

0000000000000000 <_GLOBAL__sub_I_museair_v2_ref>:
   0:	55                   	push   %rbp
   1:	bf 00 00 00 00       	mov    $0x0,%edi
   6:	bd 00 00 00 00       	mov    $0x0,%ebp
   b:	53                   	push   %rbx
   c:	48 83 ec 08          	sub    $0x8,%rsp
  10:	e8 00 00 00 00       	callq  15 <_GLOBAL__sub_I_museair_v2_ref+0x15>
  15:	bf 00 00 00 00       	mov    $0x0,%edi
  1a:	48 89 c3             	mov    %rax,%rbx
  1d:	48 89 05 00 00 00 00 	mov    %rax,0x0(%rip)        # 24 <_GLOBAL__sub_I_museair_v2_ref+0x24>
  24:	48 c7 05 00 00 00 00 	movq   $0x0,0x0(%rip)        # 2f <_GLOBAL__sub_I_museair_v2_ref+0x2f>
  2b:	00 00 00 00 
  2f:	c7 05 00 00 00 00 02 	movl   $0x2,0x0(%rip)        # 39 <_GLOBAL__sub_I_museair_v2_ref+0x39>
  36:	00 00 00 
  39:	e8 00 00 00 00       	callq  3e <_GLOBAL__sub_I_museair_v2_ref+0x3e>
  3e:	c4 e1 f9 6e c8       	vmovq  %rax,%xmm1
  43:	c4 e3 f1 22 c3 01    	vpinsrq $0x1,%rbx,%xmm1,%xmm0
  49:	b9 00 00 00 00       	mov    $0x0,%ecx
  4e:	b8 00 00 00 00       	mov    $0x0,%eax
  53:	c5 f9 7f 05 00 00 00 	vmovdqa %xmm0,0x0(%rip)        # 5b <_GLOBAL__sub_I_museair_v2_ref+0x5b>
  5a:	00 
  5b:	c4 e1 f9 6e c1       	vmovq  %rcx,%xmm0
  60:	c4 e3 f9 22 c0 01    	vpinsrq $0x1,%rax,%xmm0,%xmm0
  66:	c5 f9 7f 05 00 00 00 	vmovdqa %xmm0,0x0(%rip)        # 6e <_GLOBAL__sub_I_museair_v2_ref+0x6e>
  6d:	00 
  6e:	48 b8 bc ca 40 71 bc 	movabs $0x7140cabc7140cabc,%rax
  75:	ca 40 71 
  78:	62 f2 fd 08 7c c5    	vpbroadcastq %rbp,%xmm0
  7e:	bf 00 00 00 00       	mov    $0x0,%edi
  83:	c7 05 00 00 00 00 00 	movl   $0x0,0x0(%rip)        # 8d <_GLOBAL__sub_I_museair_v2_ref+0x8d>
  8a:	00 00 00 
  8d:	48 c7 05 00 00 00 00 	movq   $0x0,0x0(%rip)        # 98 <_GLOBAL__sub_I_museair_v2_ref+0x98>
  94:	00 00 00 00 
  98:	48 c7 05 00 00 00 00 	movq   $0x0,0x0(%rip)        # a3 <_GLOBAL__sub_I_museair_v2_ref+0xa3>
  9f:	00 00 00 00 
  a3:	48 c7 05 00 00 00 00 	movq   $0x0,0x0(%rip)        # ae <_GLOBAL__sub_I_museair_v2_ref+0xae>
  aa:	00 00 00 00 
  ae:	48 c7 05 00 00 00 00 	movq   $0x0,0x0(%rip)        # b9 <_GLOBAL__sub_I_museair_v2_ref+0xb9>
  b5:	00 00 00 00 
  b9:	48 c7 05 00 00 00 00 	movq   $0x0,0x0(%rip)        # c4 <_GLOBAL__sub_I_museair_v2_ref+0xc4>
  c0:	00 00 00 00 
  c4:	48 c7 05 00 00 00 00 	movq   $0x0,0x0(%rip)        # cf <_GLOBAL__sub_I_museair_v2_ref+0xcf>
  cb:	00 00 00 00 
  cf:	48 c7 05 00 00 00 00 	movq   $0x2400,0x0(%rip)        # da <_GLOBAL__sub_I_museair_v2_ref+0xda>
  d6:	00 24 00 00 
  da:	c7 05 00 00 00 00 40 	movl   $0x40,0x0(%rip)        # e4 <_GLOBAL__sub_I_museair_v2_ref+0xe4>
  e1:	00 00 00 
  e4:	48 89 05 00 00 00 00 	mov    %rax,0x0(%rip)        # eb <_GLOBAL__sub_I_museair_v2_ref+0xeb>
  eb:	48 c7 05 00 00 00 00 	movq   $0x0,0x0(%rip)        # f6 <_GLOBAL__sub_I_museair_v2_ref+0xf6>
  f2:	00 00 00 00 
  f6:	48 c7 05 00 00 00 00 	movq   $0x0,0x0(%rip)        # 101 <_GLOBAL__sub_I_museair_v2_ref+0x101>
  fd:	00 00 00 00 
 101:	48 c7 05 00 00 00 00 	movq   $0x0,0x0(%rip)        # 10c <_GLOBAL__sub_I_museair_v2_ref+0x10c>
 108:	00 00 00 00 
 10c:	c5 fa 7f 05 00 00 00 	vmovdqu %xmm0,0x0(%rip)        # 114 <_GLOBAL__sub_I_museair_v2_ref+0x114>
 113:	00 
 114:	e8 00 00 00 00       	callq  119 <_GLOBAL__sub_I_museair_v2_ref+0x119>
 119:	48 83 c4 08          	add    $0x8,%rsp
 11d:	5b                   	pop    %rbx
 11e:	ba 00 00 00 00       	mov    $0x0,%edx
 123:	be 00 00 00 00       	mov    $0x0,%esi
 128:	bf 00 00 00 00       	mov    $0x0,%edi
 12d:	5d                   	pop    %rbp
 12e:	e9 00 00 00 00       	jmpq   133 <_GLOBAL__sub_I_museair_v2_ref+0x133>
 133:	48 89 c5             	mov    %rax,%rbp
 136:	e9 00 00 00 00       	jmpq   13b <_ZL16THIS_HASH_FAMILY+0x7b>
