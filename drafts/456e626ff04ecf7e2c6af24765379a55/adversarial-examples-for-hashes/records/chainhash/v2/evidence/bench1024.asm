
experiments/bench1024:     file format elf64-x86-64


Disassembly of section .init:

0000000000401000 <_init>:
  401000:	f3 0f 1e fa          	endbr64 
  401004:	48 83 ec 08          	sub    rsp,0x8
  401008:	48 8b 05 e1 3f 00 00 	mov    rax,QWORD PTR [rip+0x3fe1]        # 404ff0 <__gmon_start__>
  40100f:	48 85 c0             	test   rax,rax
  401012:	74 02                	je     401016 <_init+0x16>
  401014:	ff d0                	call   rax
  401016:	48 83 c4 08          	add    rsp,0x8
  40101a:	c3                   	ret    

Disassembly of section .plt:

0000000000401020 <.plt>:
  401020:	ff 35 e2 3f 00 00    	push   QWORD PTR [rip+0x3fe2]        # 405008 <_GLOBAL_OFFSET_TABLE_+0x8>
  401026:	ff 25 e4 3f 00 00    	jmp    QWORD PTR [rip+0x3fe4]        # 405010 <_GLOBAL_OFFSET_TABLE_+0x10>
  40102c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]

0000000000401030 <free@plt>:
  401030:	ff 25 e2 3f 00 00    	jmp    QWORD PTR [rip+0x3fe2]        # 405018 <free@GLIBC_2.2.5>
  401036:	68 00 00 00 00       	push   0x0
  40103b:	e9 e0 ff ff ff       	jmp    401020 <.plt>

0000000000401040 <puts@plt>:
  401040:	ff 25 da 3f 00 00    	jmp    QWORD PTR [rip+0x3fda]        # 405020 <puts@GLIBC_2.2.5>
  401046:	68 01 00 00 00       	push   0x1
  40104b:	e9 d0 ff ff ff       	jmp    401020 <.plt>

0000000000401050 <printf@plt>:
  401050:	ff 25 d2 3f 00 00    	jmp    QWORD PTR [rip+0x3fd2]        # 405028 <printf@GLIBC_2.2.5>
  401056:	68 02 00 00 00       	push   0x2
  40105b:	e9 c0 ff ff ff       	jmp    401020 <.plt>

0000000000401060 <malloc@plt>:
  401060:	ff 25 ca 3f 00 00    	jmp    QWORD PTR [rip+0x3fca]        # 405030 <malloc@GLIBC_2.2.5>
  401066:	68 03 00 00 00       	push   0x3
  40106b:	e9 b0 ff ff ff       	jmp    401020 <.plt>

0000000000401070 <fflush@plt>:
  401070:	ff 25 c2 3f 00 00    	jmp    QWORD PTR [rip+0x3fc2]        # 405038 <fflush@GLIBC_2.2.5>
  401076:	68 04 00 00 00       	push   0x4
  40107b:	e9 a0 ff ff ff       	jmp    401020 <.plt>

Disassembly of section .text:

0000000000401080 <main>:
  401080:	41 57                	push   r15
  401082:	bf 40 00 04 00       	mov    edi,0x40040
  401087:	41 56                	push   r14
  401089:	41 55                	push   r13
  40108b:	41 54                	push   r12
  40108d:	55                   	push   rbp
  40108e:	53                   	push   rbx
  40108f:	48 81 ec c8 08 00 00 	sub    rsp,0x8c8
  401096:	e8 c5 ff ff ff       	call   401060 <malloc@plt>
  40109b:	48 8d b4 24 70 04 00 	lea    rsi,[rsp+0x470]
  4010a2:	00 
  4010a3:	b9 11 00 00 00       	mov    ecx,0x11
  4010a8:	49 bb 15 7c 4a 7f b9 	movabs r11,0x9e3779b97f4a7c15
  4010af:	79 37 9e 
  4010b2:	49 ba b9 e5 e4 1c 6d 	movabs r10,0xbf58476d1ce4e5b9
  4010b9:	47 58 bf 
  4010bc:	48 89 c5             	mov    rbp,rax
  4010bf:	48 89 f7             	mov    rdi,rsi
  4010c2:	49 b9 eb 11 31 13 bb 	movabs r9,0x94d049bb133111eb
  4010c9:	49 d0 94 
  4010cc:	49 b8 4e 67 dc 1e 45 	movabs r8,0xabb024451edc674e
  4010d3:	24 b0 ab 
  4010d6:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
  4010dd:	00 00 00 
  4010e0:	4c 01 d9             	add    rcx,r11
  4010e3:	48 83 c7 08          	add    rdi,0x8
  4010e7:	48 89 ca             	mov    rdx,rcx
  4010ea:	48 c1 ea 1e          	shr    rdx,0x1e
  4010ee:	48 31 ca             	xor    rdx,rcx
  4010f1:	49 0f af d2          	imul   rdx,r10
  4010f5:	48 89 d0             	mov    rax,rdx
  4010f8:	48 c1 e8 1b          	shr    rax,0x1b
  4010fc:	48 31 d0             	xor    rax,rdx
  4010ff:	49 0f af c1          	imul   rax,r9
  401103:	48 89 c2             	mov    rdx,rax
  401106:	48 c1 ea 1f          	shr    rdx,0x1f
  40110a:	48 31 d0             	xor    rax,rdx
  40110d:	48 89 47 f8          	mov    QWORD PTR [rdi-0x8],rax
  401111:	4c 39 c1             	cmp    rcx,r8
  401114:	75 ca                	jne    4010e0 <main+0x60>
  401116:	66 0f 6f 05 62 1f 00 	movdqa xmm0,XMMWORD PTR [rip+0x1f62]        # 403080 <__dso_handle+0x78>
  40111d:	00 
  40111e:	4c 8d 64 24 20       	lea    r12,[rsp+0x20]
  401123:	b9 89 00 00 00       	mov    ecx,0x89
  401128:	66 0f 6f 15 40 1f 00 	movdqa xmm2,XMMWORD PTR [rip+0x1f40]        # 403070 <__dso_handle+0x68>
  40112f:	00 
  401130:	4c 89 e7             	mov    rdi,r12
  401133:	48 89 e8             	mov    rax,rbp
  401136:	66 44 0f 6f 1d 91 1f 	movdqa xmm11,XMMWORD PTR [rip+0x1f91]        # 4030d0 <__dso_handle+0xc8>
  40113d:	00 00 
  40113f:	66 44 0f 6f 15 98 1f 	movdqa xmm10,XMMWORD PTR [rip+0x1f98]        # 4030e0 <__dso_handle+0xd8>
  401146:	00 00 
  401148:	f3 48 a5             	rep movs QWORD PTR es:[rdi],QWORD PTR ds:[rsi]
  40114b:	0f 11 84 24 78 04 00 	movups XMMWORD PTR [rsp+0x478],xmm0
  401152:	00 
  401153:	66 0f 6f 05 35 1f 00 	movdqa xmm0,XMMWORD PTR [rip+0x1f35]        # 403090 <__dso_handle+0x88>
  40115a:	00 
  40115b:	66 44 0f 6f 0d 8c 1f 	movdqa xmm9,XMMWORD PTR [rip+0x1f8c]        # 4030f0 <__dso_handle+0xe8>
  401162:	00 00 
  401164:	66 44 0f 6f 05 93 1f 	movdqa xmm8,XMMWORD PTR [rip+0x1f93]        # 403100 <__dso_handle+0xf8>
  40116b:	00 00 
  40116d:	66 0f 6f 0d 9b 1f 00 	movdqa xmm1,XMMWORD PTR [rip+0x1f9b]        # 403110 <__dso_handle+0x108>
  401174:	00 
  401175:	48 8d 95 40 00 04 00 	lea    rdx,[rbp+0x40040]
  40117c:	0f 11 84 24 88 04 00 	movups XMMWORD PTR [rsp+0x488],xmm0
  401183:	00 
  401184:	66 0f 6f 05 14 1f 00 	movdqa xmm0,XMMWORD PTR [rip+0x1f14]        # 4030a0 <__dso_handle+0x98>
  40118b:	00 
  40118c:	66 0f 6f 3d 8c 1f 00 	movdqa xmm7,XMMWORD PTR [rip+0x1f8c]        # 403120 <__dso_handle+0x118>
  401193:	00 
  401194:	66 0f 6f 35 94 1f 00 	movdqa xmm6,XMMWORD PTR [rip+0x1f94]        # 403130 <__dso_handle+0x128>
  40119b:	00 
  40119c:	66 0f 6f 2d 9c 1f 00 	movdqa xmm5,XMMWORD PTR [rip+0x1f9c]        # 403140 <__dso_handle+0x138>
  4011a3:	00 
  4011a4:	0f 11 84 24 98 04 00 	movups XMMWORD PTR [rsp+0x498],xmm0
  4011ab:	00 
  4011ac:	66 0f 6f 05 fc 1e 00 	movdqa xmm0,XMMWORD PTR [rip+0x1efc]        # 4030b0 <__dso_handle+0xa8>
  4011b3:	00 
  4011b4:	66 0f 6f 25 94 1f 00 	movdqa xmm4,XMMWORD PTR [rip+0x1f94]        # 403150 <__dso_handle+0x148>
  4011bb:	00 
  4011bc:	66 0f 6f 1d 9c 1f 00 	movdqa xmm3,XMMWORD PTR [rip+0x1f9c]        # 403160 <__dso_handle+0x158>
  4011c3:	00 
  4011c4:	0f 11 84 24 a8 04 00 	movups XMMWORD PTR [rsp+0x4a8],xmm0
  4011cb:	00 
  4011cc:	66 0f 6f 05 ec 1e 00 	movdqa xmm0,XMMWORD PTR [rip+0x1eec]        # 4030c0 <__dso_handle+0xb8>
  4011d3:	00 
  4011d4:	0f 11 84 24 b8 04 00 	movups XMMWORD PTR [rsp+0x4b8],xmm0
  4011db:	00 
  4011dc:	66 0f 6f c2          	movdqa xmm0,xmm2
  4011e0:	48 83 c0 10          	add    rax,0x10
  4011e4:	66 41 0f d4 d3       	paddq  xmm2,xmm11
  4011e9:	66 44 0f 6f e8       	movdqa xmm13,xmm0
  4011ee:	66 44 0f 6f e0       	movdqa xmm12,xmm0
  4011f3:	66 44 0f 6f f0       	movdqa xmm14,xmm0
  4011f8:	66 45 0f d4 ea       	paddq  xmm13,xmm10
  4011fd:	66 45 0f d4 f0       	paddq  xmm14,xmm8
  401202:	45 0f c6 e5 88       	shufps xmm12,xmm13,0x88
  401207:	66 44 0f 6f e8       	movdqa xmm13,xmm0
  40120c:	66 44 0f db e1       	pand   xmm12,xmm1
  401211:	66 45 0f d4 e9       	paddq  xmm13,xmm9
  401216:	45 0f c6 ee 88       	shufps xmm13,xmm14,0x88
  40121b:	66 44 0f db e9       	pand   xmm13,xmm1
  401220:	66 44 0f 6f f0       	movdqa xmm14,xmm0
  401225:	66 45 0f 38 2b e5    	packusdw xmm12,xmm13
  40122b:	66 44 0f 6f e8       	movdqa xmm13,xmm0
  401230:	66 44 0f d4 f6       	paddq  xmm14,xmm6
  401235:	66 44 0f d4 ef       	paddq  xmm13,xmm7
  40123a:	66 44 0f db e3       	pand   xmm12,xmm3
  40123f:	45 0f c6 ee 88       	shufps xmm13,xmm14,0x88
  401244:	66 44 0f 6f f0       	movdqa xmm14,xmm0
  401249:	66 0f d4 c4          	paddq  xmm0,xmm4
  40124d:	66 44 0f d4 f5       	paddq  xmm14,xmm5
  401252:	44 0f c6 f0 88       	shufps xmm14,xmm0,0x88
  401257:	66 41 0f 6f c5       	movdqa xmm0,xmm13
  40125c:	66 44 0f db f1       	pand   xmm14,xmm1
  401261:	66 0f db c1          	pand   xmm0,xmm1
  401265:	66 41 0f 38 2b c6    	packusdw xmm0,xmm14
  40126b:	66 0f db c3          	pand   xmm0,xmm3
  40126f:	66 44 0f 67 e0       	packuswb xmm12,xmm0
  401274:	66 41 0f 6f c4       	movdqa xmm0,xmm12
  401279:	66 41 0f fc c4       	paddb  xmm0,xmm12
  40127e:	66 0f fc c0          	paddb  xmm0,xmm0
  401282:	66 0f fc c0          	paddb  xmm0,xmm0
  401286:	66 41 0f fc c4       	paddb  xmm0,xmm12
  40128b:	66 0f fc c0          	paddb  xmm0,xmm0
  40128f:	66 0f fc c0          	paddb  xmm0,xmm0
  401293:	66 41 0f fc c4       	paddb  xmm0,xmm12
  401298:	0f 11 40 f0          	movups XMMWORD PTR [rax-0x10],xmm0
  40129c:	48 39 d0             	cmp    rax,rdx
  40129f:	0f 85 37 ff ff ff    	jne    4011dc <main+0x15c>
  4012a5:	48 8d 84 24 78 04 00 	lea    rax,[rsp+0x478]
  4012ac:	00 
  4012ad:	31 db                	xor    ebx,ebx
  4012af:	41 bd a0 86 01 00    	mov    r13d,0x186a0
  4012b5:	48 89 44 24 18       	mov    QWORD PTR [rsp+0x18],rax
  4012ba:	41 be e8 03 00 00    	mov    r14d,0x3e8
  4012c0:	48 89 da             	mov    rdx,rbx
  4012c3:	48 89 ee             	mov    rsi,rbp
  4012c6:	4c 89 e7             	mov    rdi,r12
  4012c9:	e8 62 0e 00 00       	call   402130 <hash>
  4012ce:	49 89 c0             	mov    r8,rax
  4012d1:	48 8b 05 88 3d 00 00 	mov    rax,QWORD PTR [rip+0x3d88]        # 405060 <sink>
  4012d8:	4c 31 c0             	xor    rax,r8
  4012db:	48 89 05 7e 3d 00 00 	mov    QWORD PTR [rip+0x3d7e],rax        # 405060 <sink>
  4012e2:	41 83 ee 01          	sub    r14d,0x1
  4012e6:	75 d8                	jne    4012c0 <main+0x240>
  4012e8:	48 89 da             	mov    rdx,rbx
  4012eb:	be 00 04 00 00       	mov    esi,0x400
  4012f0:	bf 10 30 40 00       	mov    edi,0x403010
  4012f5:	31 c0                	xor    eax,eax
  4012f7:	e8 54 fd ff ff       	call   401050 <printf@plt>
  4012fc:	44 89 e8             	mov    eax,r13d
  4012ff:	c7 44 24 0c 00 00 00 	mov    DWORD PTR [rsp+0xc],0x0
  401306:	00 
  401307:	48 89 44 24 10       	mov    QWORD PTR [rsp+0x10],rax
  40130c:	0f ae e8             	lfence 
  40130f:	0f 31                	rdtsc  
  401311:	45 31 ff             	xor    r15d,r15d
  401314:	49 89 c6             	mov    r14,rax
  401317:	48 c1 e2 20          	shl    rdx,0x20
  40131b:	49 09 d6             	or     r14,rdx
  40131e:	66 90                	xchg   ax,ax
  401320:	48 89 da             	mov    rdx,rbx
  401323:	48 89 ee             	mov    rsi,rbp
  401326:	4c 89 e7             	mov    rdi,r12
  401329:	41 83 c7 01          	add    r15d,0x1
  40132d:	e8 fe 0d 00 00       	call   402130 <hash>
  401332:	49 89 c0             	mov    r8,rax
  401335:	48 8b 05 24 3d 00 00 	mov    rax,QWORD PTR [rip+0x3d24]        # 405060 <sink>
  40133c:	4c 31 c0             	xor    rax,r8
  40133f:	48 89 05 1a 3d 00 00 	mov    QWORD PTR [rip+0x3d1a],rax        # 405060 <sink>
  401346:	45 39 fd             	cmp    r13d,r15d
  401349:	75 d5                	jne    401320 <main+0x2a0>
  40134b:	0f ae e8             	lfence 
  40134e:	0f 31                	rdtsc  
  401350:	48 c1 e2 20          	shl    rdx,0x20
  401354:	48 09 d0             	or     rax,rdx
  401357:	4c 29 f0             	sub    rax,r14
  40135a:	0f 88 b0 00 00 00    	js     401410 <main+0x390>
  401360:	66 0f ef c0          	pxor   xmm0,xmm0
  401364:	f2 48 0f 2a c0       	cvtsi2sd xmm0,rax
  401369:	66 0f ef c9          	pxor   xmm1,xmm1
  40136d:	83 44 24 0c 01       	add    DWORD PTR [rsp+0xc],0x1
  401372:	44 8b 7c 24 0c       	mov    r15d,DWORD PTR [rsp+0xc]
  401377:	f2 48 0f 2a 4c 24 10 	cvtsi2sd xmm1,QWORD PTR [rsp+0x10]
  40137e:	f2 0f 5e c1          	divsd  xmm0,xmm1
  401382:	41 83 ff 01          	cmp    r15d,0x1
  401386:	0f 84 a4 00 00 00    	je     401430 <main+0x3b0>
  40138c:	be 37 30 40 00       	mov    esi,0x403037
  401391:	bf 39 30 40 00       	mov    edi,0x403039
  401396:	b8 01 00 00 00       	mov    eax,0x1
  40139b:	e8 b0 fc ff ff       	call   401050 <printf@plt>
  4013a0:	41 83 ff 05          	cmp    r15d,0x5
  4013a4:	0f 85 62 ff ff ff    	jne    40130c <main+0x28c>
  4013aa:	bf 40 30 40 00       	mov    edi,0x403040
  4013af:	e8 8c fc ff ff       	call   401040 <puts@plt>
  4013b4:	48 8b 3d 8d 3c 00 00 	mov    rdi,QWORD PTR [rip+0x3c8d]        # 405048 <stdout@@GLIBC_2.2.5>
  4013bb:	e8 b0 fc ff ff       	call   401070 <fflush@plt>
  4013c0:	48 8d 84 24 c8 04 00 	lea    rax,[rsp+0x4c8]
  4013c7:	00 
  4013c8:	48 3b 44 24 18       	cmp    rax,QWORD PTR [rsp+0x18]
  4013cd:	74 7a                	je     401449 <main+0x3c9>
  4013cf:	48 8b 44 24 18       	mov    rax,QWORD PTR [rsp+0x18]
  4013d4:	41 bd a0 86 01 00    	mov    r13d,0x186a0
  4013da:	48 8b 18             	mov    rbx,QWORD PTR [rax]
  4013dd:	48 85 db             	test   rbx,rbx
  4013e0:	74 1b                	je     4013fd <main+0x37d>
  4013e2:	31 d2                	xor    edx,edx
  4013e4:	48 8d 4b 01          	lea    rcx,[rbx+0x1]
  4013e8:	b8 00 00 00 02       	mov    eax,0x2000000
  4013ed:	48 f7 f1             	div    rcx
  4013f0:	ba e8 03 00 00       	mov    edx,0x3e8
  4013f5:	39 d0                	cmp    eax,edx
  4013f7:	0f 43 d0             	cmovae edx,eax
  4013fa:	41 89 d5             	mov    r13d,edx
  4013fd:	48 83 44 24 18 08    	add    QWORD PTR [rsp+0x18],0x8
  401403:	e9 b2 fe ff ff       	jmp    4012ba <main+0x23a>
  401408:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
  40140f:	00 
  401410:	48 89 c2             	mov    rdx,rax
  401413:	83 e0 01             	and    eax,0x1
  401416:	66 0f ef c0          	pxor   xmm0,xmm0
  40141a:	48 d1 ea             	shr    rdx,1
  40141d:	48 09 c2             	or     rdx,rax
  401420:	f2 48 0f 2a c2       	cvtsi2sd xmm0,rdx
  401425:	f2 0f 58 c0          	addsd  xmm0,xmm0
  401429:	e9 3b ff ff ff       	jmp    401369 <main+0x2e9>
  40142e:	66 90                	xchg   ax,ax
  401430:	be 38 30 40 00       	mov    esi,0x403038
  401435:	bf 39 30 40 00       	mov    edi,0x403039
  40143a:	b8 01 00 00 00       	mov    eax,0x1
  40143f:	e8 0c fc ff ff       	call   401050 <printf@plt>
  401444:	e9 c3 fe ff ff       	jmp    40130c <main+0x28c>
  401449:	48 89 ef             	mov    rdi,rbp
  40144c:	e8 df fb ff ff       	call   401030 <free@plt>
  401451:	48 8b 05 08 3c 00 00 	mov    rax,QWORD PTR [rip+0x3c08]        # 405060 <sink>
  401458:	31 c0                	xor    eax,eax
  40145a:	48 81 c4 c8 08 00 00 	add    rsp,0x8c8
  401461:	5b                   	pop    rbx
  401462:	5d                   	pop    rbp
  401463:	41 5c                	pop    r12
  401465:	41 5d                	pop    r13
  401467:	41 5e                	pop    r14
  401469:	41 5f                	pop    r15
  40146b:	c3                   	ret    
  40146c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]

0000000000401470 <_start>:
  401470:	f3 0f 1e fa          	endbr64 
  401474:	31 ed                	xor    ebp,ebp
  401476:	49 89 d1             	mov    r9,rdx
  401479:	5e                   	pop    rsi
  40147a:	48 89 e2             	mov    rdx,rsp
  40147d:	48 83 e4 f0          	and    rsp,0xfffffffffffffff0
  401481:	50                   	push   rax
  401482:	54                   	push   rsp
  401483:	45 31 c0             	xor    r8d,r8d
  401486:	31 c9                	xor    ecx,ecx
  401488:	48 c7 c7 80 10 40 00 	mov    rdi,0x401080
  40148f:	ff 15 4b 3b 00 00    	call   QWORD PTR [rip+0x3b4b]        # 404fe0 <__libc_start_main@GLIBC_2.34>
  401495:	f4                   	hlt    
  401496:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
  40149d:	00 00 00 

00000000004014a0 <_dl_relocate_static_pie>:
  4014a0:	f3 0f 1e fa          	endbr64 
  4014a4:	c3                   	ret    
  4014a5:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
  4014ac:	00 00 00 
  4014af:	90                   	nop

00000000004014b0 <deregister_tm_clones>:
  4014b0:	48 8d 3d 91 3b 00 00 	lea    rdi,[rip+0x3b91]        # 405048 <stdout@@GLIBC_2.2.5>
  4014b7:	48 8d 05 8a 3b 00 00 	lea    rax,[rip+0x3b8a]        # 405048 <stdout@@GLIBC_2.2.5>
  4014be:	48 39 f8             	cmp    rax,rdi
  4014c1:	74 15                	je     4014d8 <deregister_tm_clones+0x28>
  4014c3:	48 8b 05 1e 3b 00 00 	mov    rax,QWORD PTR [rip+0x3b1e]        # 404fe8 <_ITM_deregisterTMCloneTable>
  4014ca:	48 85 c0             	test   rax,rax
  4014cd:	74 09                	je     4014d8 <deregister_tm_clones+0x28>
  4014cf:	ff e0                	jmp    rax
  4014d1:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
  4014d8:	c3                   	ret    
  4014d9:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]

00000000004014e0 <register_tm_clones>:
  4014e0:	48 8d 3d 61 3b 00 00 	lea    rdi,[rip+0x3b61]        # 405048 <stdout@@GLIBC_2.2.5>
  4014e7:	48 8d 35 5a 3b 00 00 	lea    rsi,[rip+0x3b5a]        # 405048 <stdout@@GLIBC_2.2.5>
  4014ee:	48 29 fe             	sub    rsi,rdi
  4014f1:	48 89 f0             	mov    rax,rsi
  4014f4:	48 c1 ee 3f          	shr    rsi,0x3f
  4014f8:	48 c1 f8 03          	sar    rax,0x3
  4014fc:	48 01 c6             	add    rsi,rax
  4014ff:	48 d1 fe             	sar    rsi,1
  401502:	74 14                	je     401518 <register_tm_clones+0x38>
  401504:	48 8b 05 ed 3a 00 00 	mov    rax,QWORD PTR [rip+0x3aed]        # 404ff8 <_ITM_registerTMCloneTable>
  40150b:	48 85 c0             	test   rax,rax
  40150e:	74 08                	je     401518 <register_tm_clones+0x38>
  401510:	ff e0                	jmp    rax
  401512:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
  401518:	c3                   	ret    
  401519:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]

0000000000401520 <__do_global_dtors_aux>:
  401520:	f3 0f 1e fa          	endbr64 
  401524:	80 3d 25 3b 00 00 00 	cmp    BYTE PTR [rip+0x3b25],0x0        # 405050 <completed.0>
  40152b:	75 13                	jne    401540 <__do_global_dtors_aux+0x20>
  40152d:	55                   	push   rbp
  40152e:	48 89 e5             	mov    rbp,rsp
  401531:	e8 7a ff ff ff       	call   4014b0 <deregister_tm_clones>
  401536:	c6 05 13 3b 00 00 01 	mov    BYTE PTR [rip+0x3b13],0x1        # 405050 <completed.0>
  40153d:	5d                   	pop    rbp
  40153e:	c3                   	ret    
  40153f:	90                   	nop
  401540:	c3                   	ret    
  401541:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
  401548:	00 00 00 00 
  40154c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]

0000000000401550 <frame_dummy>:
  401550:	f3 0f 1e fa          	endbr64 
  401554:	eb 8a                	jmp    4014e0 <register_tm_clones>
  401556:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
  40155d:	00 00 00 

0000000000401560 <chainhash_x86_avx512>:
  401560:	55                   	push   rbp
  401561:	48 89 f9             	mov    rcx,rdi
  401564:	48 89 d7             	mov    rdi,rdx
  401567:	48 89 e5             	mov    rbp,rsp
  40156a:	48 83 e4 c0          	and    rsp,0xffffffffffffffc0
  40156e:	48 83 ec 48          	sub    rsp,0x48
  401572:	c5 fa 6f b9 00 04 00 	vmovdqu xmm7,XMMWORD PTR [rcx+0x400]
  401579:	00 
  40157a:	48 8b 81 10 04 00 00 	mov    rax,QWORD PTR [rcx+0x410]
  401581:	48 33 81 00 04 00 00 	xor    rax,QWORD PTR [rcx+0x400]
  401588:	c5 f9 7f 7c 24 f8    	vmovdqa XMMWORD PTR [rsp-0x8],xmm7
  40158e:	c4 e1 f9 6e f8       	vmovq  xmm7,rax
  401593:	c5 f9 7f 7c 24 28    	vmovdqa XMMWORD PTR [rsp+0x28],xmm7
  401599:	48 81 fa 00 04 00 00 	cmp    rdx,0x400
  4015a0:	0f 86 a1 06 00 00    	jbe    401c47 <chainhash_x86_avx512+0x6e7>
  4015a6:	c5 f9 6f 15 a2 1a 00 	vmovdqa xmm2,XMMWORD PTR [rip+0x1aa2]        # 403050 <__dso_handle+0x48>
  4015ad:	00 
  4015ae:	48 8d 92 ff fb ff ff 	lea    rdx,[rdx-0x401]
  4015b5:	62 f1 7e 48 6f 61 0f 	vmovdqu32 zmm4,ZMMWORD PTR [rcx+0x3c0]
  4015bc:	48 c1 ea 0a          	shr    rdx,0xa
  4015c0:	62 e1 7e 48 6f 19    	vmovdqu32 zmm19,ZMMWORD PTR [rcx]
  4015c6:	62 e1 7e 48 6f 51 01 	vmovdqu32 zmm18,ZMMWORD PTR [rcx+0x40]
  4015cd:	48 8d 42 01          	lea    rax,[rdx+0x1]
  4015d1:	c5 f9 7f 54 24 18    	vmovdqa XMMWORD PTR [rsp+0x18],xmm2
  4015d7:	c5 f9 6f 15 81 1a 00 	vmovdqa xmm2,XMMWORD PTR [rip+0x1a81]        # 403060 <__dso_handle+0x58>
  4015de:	00 
  4015df:	62 e1 7e 48 6f 49 02 	vmovdqu32 zmm17,ZMMWORD PTR [rcx+0x80]
  4015e6:	48 c1 e0 0a          	shl    rax,0xa
  4015ea:	62 e1 7e 48 6f 41 03 	vmovdqu32 zmm16,ZMMWORD PTR [rcx+0xc0]
  4015f1:	62 71 7e 48 6f 79 04 	vmovdqu32 zmm15,ZMMWORD PTR [rcx+0x100]
  4015f8:	62 f1 7d 48 7f a4 24 	vmovdqa32 ZMMWORD PTR [rsp-0x78],zmm4
  4015ff:	88 ff ff ff 
  401603:	62 71 7e 48 6f 71 05 	vmovdqu32 zmm14,ZMMWORD PTR [rcx+0x140]
  40160a:	48 01 f0             	add    rax,rsi
  40160d:	62 71 7e 48 6f 69 06 	vmovdqu32 zmm13,ZMMWORD PTR [rcx+0x180]
  401614:	c5 f9 7f 54 24 08    	vmovdqa XMMWORD PTR [rsp+0x8],xmm2
  40161a:	62 71 7e 48 6f 61 07 	vmovdqu32 zmm12,ZMMWORD PTR [rcx+0x1c0]
  401621:	62 71 7e 48 6f 59 08 	vmovdqu32 zmm11,ZMMWORD PTR [rcx+0x200]
  401628:	62 71 7e 48 6f 51 09 	vmovdqu32 zmm10,ZMMWORD PTR [rcx+0x240]
  40162f:	62 71 7e 48 6f 49 0a 	vmovdqu32 zmm9,ZMMWORD PTR [rcx+0x280]
  401636:	62 71 7e 48 6f 41 0b 	vmovdqu32 zmm8,ZMMWORD PTR [rcx+0x2c0]
  40163d:	62 f1 7e 48 6f 79 0c 	vmovdqu32 zmm7,ZMMWORD PTR [rcx+0x300]
  401644:	62 f1 7e 48 6f 71 0d 	vmovdqu32 zmm6,ZMMWORD PTR [rcx+0x340]
  40164b:	62 f1 7e 48 6f 69 0e 	vmovdqu32 zmm5,ZMMWORD PTR [rcx+0x380]
  401652:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
  401658:	62 f1 65 40 ef 06    	vpxord zmm0,zmm19,ZMMWORD PTR [rsi]
  40165e:	62 61 6d 40 ef 7e 01 	vpxord zmm31,zmm18,ZMMWORD PTR [rsi+0x40]
  401665:	48 81 c6 00 04 00 00 	add    rsi,0x400
  40166c:	62 e1 75 40 ef 6e f2 	vpxord zmm21,zmm17,ZMMWORD PTR [rsi-0x380]
  401673:	62 61 7d 40 ef 76 f3 	vpxord zmm30,zmm16,ZMMWORD PTR [rsi-0x340]
  40167a:	62 f1 1d 48 ef 5e f7 	vpxord zmm3,zmm12,ZMMWORD PTR [rsi-0x240]
  401681:	62 f1 35 48 ef 56 fa 	vpxord zmm2,zmm9,ZMMWORD PTR [rsi-0x180]
  401688:	62 e1 05 48 ef 66 f4 	vpxord zmm20,zmm15,ZMMWORD PTR [rsi-0x300]
  40168f:	62 61 0d 48 ef 6e f5 	vpxord zmm29,zmm14,ZMMWORD PTR [rsi-0x2c0]
  401696:	62 61 15 48 ef 66 f6 	vpxord zmm28,zmm13,ZMMWORD PTR [rsi-0x280]
  40169d:	62 61 25 48 ef 5e f8 	vpxord zmm27,zmm11,ZMMWORD PTR [rsi-0x200]
  4016a4:	62 f3 7d 48 44 c0 10 	vpclmullqhqdq zmm0,zmm0,zmm0
  4016ab:	62 f1 4d 48 ef 4e fd 	vpxord zmm1,zmm6,ZMMWORD PTR [rsi-0xc0]
  4016b2:	62 61 2d 48 ef 56 f9 	vpxord zmm26,zmm10,ZMMWORD PTR [rsi-0x1c0]
  4016b9:	62 03 05 40 44 ff 10 	vpclmullqhqdq zmm31,zmm31,zmm31
  4016c0:	62 e1 55 48 ef 7e fe 	vpxord zmm23,zmm5,ZMMWORD PTR [rsi-0x80]
  4016c7:	62 61 3d 48 ef 4e fb 	vpxord zmm25,zmm8,ZMMWORD PTR [rsi-0x140]
  4016ce:	62 a3 55 40 44 ed 10 	vpclmullqhqdq zmm21,zmm21,zmm21
  4016d5:	62 f1 7d 48 6f a4 24 	vmovdqa32 zmm4,ZMMWORD PTR [rsp-0x78]
  4016dc:	88 ff ff ff 
  4016e0:	62 61 45 48 ef 46 fc 	vpxord zmm24,zmm7,ZMMWORD PTR [rsi-0x100]
  4016e7:	62 03 0d 40 44 f6 10 	vpclmullqhqdq zmm30,zmm30,zmm30
  4016ee:	62 e1 5d 48 ef 76 ff 	vpxord zmm22,zmm4,ZMMWORD PTR [rsi-0x40]
  4016f5:	c5 f9 6f 64 24 08    	vmovdqa xmm4,XMMWORD PTR [rsp+0x8]
  4016fb:	62 a3 5d 40 44 e4 10 	vpclmullqhqdq zmm20,zmm20,zmm20
  401702:	62 03 15 40 44 ed 10 	vpclmullqhqdq zmm29,zmm29,zmm29
  401709:	62 03 1d 40 44 e4 10 	vpclmullqhqdq zmm28,zmm28,zmm28
  401710:	62 f3 65 48 44 db 10 	vpclmullqhqdq zmm3,zmm3,zmm3
  401717:	62 91 7d 48 ef c7    	vpxord zmm0,zmm0,zmm31
  40171d:	62 03 25 40 44 db 10 	vpclmullqhqdq zmm27,zmm27,zmm27
  401724:	62 03 2d 40 44 d2 10 	vpclmullqhqdq zmm26,zmm26,zmm26
  40172b:	62 81 55 40 ef ee    	vpxord zmm21,zmm21,zmm30
  401731:	62 f3 6d 48 44 d2 10 	vpclmullqhqdq zmm2,zmm2,zmm2
  401738:	62 b1 7d 48 ef c5    	vpxord zmm0,zmm0,zmm21
  40173e:	62 03 35 40 44 c9 10 	vpclmullqhqdq zmm25,zmm25,zmm25
  401745:	62 81 5d 40 ef e5    	vpxord zmm20,zmm20,zmm29
  40174b:	62 03 3d 40 44 c0 10 	vpclmullqhqdq zmm24,zmm24,zmm24
  401752:	62 81 5d 40 ef e4    	vpxord zmm20,zmm20,zmm28
  401758:	62 f3 75 48 44 c9 10 	vpclmullqhqdq zmm1,zmm1,zmm1
  40175f:	62 b1 7d 48 ef c4    	vpxord zmm0,zmm0,zmm20
  401765:	62 a3 45 40 44 ff 10 	vpclmullqhqdq zmm23,zmm23,zmm23
  40176c:	62 91 65 48 ef db    	vpxord zmm3,zmm3,zmm27
  401772:	62 a3 4d 40 44 f6 10 	vpclmullqhqdq zmm22,zmm22,zmm22
  401779:	62 91 65 48 ef da    	vpxord zmm3,zmm3,zmm26
  40177f:	62 f1 7d 48 ef c3    	vpxord zmm0,zmm0,zmm3
  401785:	62 91 6d 48 ef d1    	vpxord zmm2,zmm2,zmm25
  40178b:	62 91 6d 48 ef d0    	vpxord zmm2,zmm2,zmm24
  401791:	62 f1 7d 48 ef c2    	vpxord zmm0,zmm0,zmm2
  401797:	62 b1 75 48 ef cf    	vpxord zmm1,zmm1,zmm23
  40179d:	62 b1 75 48 ef ce    	vpxord zmm1,zmm1,zmm22
  4017a3:	62 f1 7d 48 ef c1    	vpxord zmm0,zmm0,zmm1
  4017a9:	62 f3 fd 48 3b c1 01 	vextracti64x4 ymm1,zmm0,0x1
  4017b0:	c5 f5 ef c0          	vpxor  ymm0,ymm1,ymm0
  4017b4:	c4 e3 7d 39 c1 01    	vextracti128 xmm1,ymm0,0x1
  4017ba:	c5 f1 ef c0          	vpxor  xmm0,xmm1,xmm0
  4017be:	c5 f9 ef 44 24 f8    	vpxor  xmm0,xmm0,XMMWORD PTR [rsp-0x8]
  4017c4:	c4 e3 79 44 54 24 28 	vpclmulhqlqdq xmm2,xmm0,XMMWORD PTR [rsp+0x28]
  4017cb:	01 
  4017cc:	c4 e3 69 44 5c 24 18 	vpclmulhqlqdq xmm3,xmm2,XMMWORD PTR [rsp+0x18]
  4017d3:	01 
  4017d4:	c5 e9 ef c0          	vpxor  xmm0,xmm2,xmm0
  4017d8:	c5 f1 73 db 08       	vpsrldq xmm1,xmm3,0x8
  4017dd:	c4 e2 59 00 c9       	vpshufb xmm1,xmm4,xmm1
  4017e2:	c5 e1 ef c9          	vpxor  xmm1,xmm3,xmm1
  4017e6:	c5 f1 ef e0          	vpxor  xmm4,xmm1,xmm0
  4017ea:	c5 f9 7f 64 24 28    	vmovdqa XMMWORD PTR [rsp+0x28],xmm4
  4017f0:	48 39 c6             	cmp    rsi,rax
  4017f3:	0f 85 5f fe ff ff    	jne    401658 <chainhash_x86_avx512+0xf8>
  4017f9:	48 f7 da             	neg    rdx
  4017fc:	48 c1 e2 0a          	shl    rdx,0xa
  401800:	48 8d 94 17 00 fc ff 	lea    rdx,[rdi+rdx*1-0x400]
  401807:	ff 
  401808:	48 81 fa ff 00 00 00 	cmp    rdx,0xff
  40180f:	0f 86 1d 04 00 00    	jbe    401c32 <chainhash_x86_avx512+0x6d2>
  401815:	62 f1 7e 48 6f 69 01 	vmovdqu32 zmm5,ZMMWORD PTR [rcx+0x40]
  40181c:	62 f1 55 48 ef 48 01 	vpxord zmm1,zmm5,ZMMWORD PTR [rax+0x40]
  401823:	48 8d b2 00 ff ff ff 	lea    rsi,[rdx-0x100]
  40182a:	62 f1 7e 48 6f 61 02 	vmovdqu32 zmm4,ZMMWORD PTR [rcx+0x80]
  401831:	62 f1 7e 48 6f 39    	vmovdqu32 zmm7,ZMMWORD PTR [rcx]
  401837:	62 f3 75 48 44 e9 10 	vpclmullqhqdq zmm5,zmm1,zmm1
  40183e:	62 f1 45 48 ef 38    	vpxord zmm7,zmm7,ZMMWORD PTR [rax]
  401844:	62 f1 5d 48 ef 48 02 	vpxord zmm1,zmm4,ZMMWORD PTR [rax+0x80]
  40184b:	62 f1 7e 48 6f 51 03 	vmovdqu32 zmm2,ZMMWORD PTR [rcx+0xc0]
  401852:	62 f1 6d 48 ef 50 03 	vpxord zmm2,zmm2,ZMMWORD PTR [rax+0xc0]
  401859:	62 f3 45 48 44 ff 10 	vpclmullqhqdq zmm7,zmm7,zmm7
  401860:	62 f3 75 48 44 e1 10 	vpclmullqhqdq zmm4,zmm1,zmm1
  401867:	62 f3 6d 48 44 da 10 	vpclmullqhqdq zmm3,zmm2,zmm2
  40186e:	62 f1 7d 48 6f f5    	vmovdqa32 zmm6,zmm5
  401874:	62 f1 7d 48 6f c7    	vmovdqa32 zmm0,zmm7
  40187a:	62 f1 7d 48 6f cc    	vmovdqa32 zmm1,zmm4
  401880:	62 f1 7d 48 6f d3    	vmovdqa32 zmm2,zmm3
  401886:	48 81 fe ff 00 00 00 	cmp    rsi,0xff
  40188d:	0f 86 73 01 00 00    	jbe    401a06 <chainhash_x86_avx512+0x4a6>
  401893:	62 f1 7e 48 6f 71 04 	vmovdqu32 zmm6,ZMMWORD PTR [rcx+0x100]
  40189a:	62 f1 4d 48 ef 40 04 	vpxord zmm0,zmm6,ZMMWORD PTR [rax+0x100]
  4018a1:	4c 8d 82 00 fe ff ff 	lea    r8,[rdx-0x200]
  4018a8:	62 f1 7e 48 6f 71 05 	vmovdqu32 zmm6,ZMMWORD PTR [rcx+0x140]
  4018af:	62 f1 4d 48 ef 70 05 	vpxord zmm6,zmm6,ZMMWORD PTR [rax+0x140]
  4018b6:	62 f3 7d 48 44 c0 10 	vpclmullqhqdq zmm0,zmm0,zmm0
  4018bd:	62 f3 4d 48 44 f6 10 	vpclmullqhqdq zmm6,zmm6,zmm6
  4018c4:	62 f1 45 48 ef c0    	vpxord zmm0,zmm7,zmm0
  4018ca:	62 f1 55 48 ef f6    	vpxord zmm6,zmm5,zmm6
  4018d0:	62 f1 fd 48 6f f8    	vmovdqa64 zmm7,zmm0
  4018d6:	62 f1 7e 48 6f 69 06 	vmovdqu32 zmm5,ZMMWORD PTR [rcx+0x180]
  4018dd:	62 f1 55 48 ef 48 06 	vpxord zmm1,zmm5,ZMMWORD PTR [rax+0x180]
  4018e4:	62 f1 7e 48 6f 69 07 	vmovdqu32 zmm5,ZMMWORD PTR [rcx+0x1c0]
  4018eb:	62 f1 55 48 ef 50 07 	vpxord zmm2,zmm5,ZMMWORD PTR [rax+0x1c0]
  4018f2:	62 f3 75 48 44 c9 10 	vpclmullqhqdq zmm1,zmm1,zmm1
  4018f9:	62 f3 6d 48 44 d2 10 	vpclmullqhqdq zmm2,zmm2,zmm2
  401900:	62 f1 5d 48 ef c9    	vpxord zmm1,zmm4,zmm1
  401906:	62 f1 65 48 ef d2    	vpxord zmm2,zmm3,zmm2
  40190c:	49 81 f8 ff 00 00 00 	cmp    r8,0xff
  401913:	0f 86 ed 00 00 00    	jbe    401a06 <chainhash_x86_avx512+0x4a6>
  401919:	62 f1 7e 48 6f 79 08 	vmovdqu32 zmm7,ZMMWORD PTR [rcx+0x200]
  401920:	62 f1 45 48 ef 58 08 	vpxord zmm3,zmm7,ZMMWORD PTR [rax+0x200]
  401927:	62 f1 7e 48 6f 69 09 	vmovdqu32 zmm5,ZMMWORD PTR [rcx+0x240]
  40192e:	62 f3 65 48 44 db 10 	vpclmullqhqdq zmm3,zmm3,zmm3
  401935:	62 f1 7d 48 ef c3    	vpxord zmm0,zmm0,zmm3
  40193b:	62 f1 55 48 ef 58 09 	vpxord zmm3,zmm5,ZMMWORD PTR [rax+0x240]
  401942:	62 f1 7e 48 6f 69 0a 	vmovdqu32 zmm5,ZMMWORD PTR [rcx+0x280]
  401949:	62 f1 fd 48 6f f8    	vmovdqa64 zmm7,zmm0
  40194f:	62 f3 65 48 44 db 10 	vpclmullqhqdq zmm3,zmm3,zmm3
  401956:	62 f1 4d 48 ef f3    	vpxord zmm6,zmm6,zmm3
  40195c:	62 f1 55 48 ef 58 0a 	vpxord zmm3,zmm5,ZMMWORD PTR [rax+0x280]
  401963:	62 f1 7e 48 6f 69 0b 	vmovdqu32 zmm5,ZMMWORD PTR [rcx+0x2c0]
  40196a:	62 f3 65 48 44 db 10 	vpclmullqhqdq zmm3,zmm3,zmm3
  401971:	62 f1 75 48 ef cb    	vpxord zmm1,zmm1,zmm3
  401977:	62 f1 55 48 ef 58 0b 	vpxord zmm3,zmm5,ZMMWORD PTR [rax+0x2c0]
  40197e:	62 f3 65 48 44 db 10 	vpclmullqhqdq zmm3,zmm3,zmm3
  401985:	62 f1 6d 48 ef d3    	vpxord zmm2,zmm2,zmm3
  40198b:	48 81 fa 00 04 00 00 	cmp    rdx,0x400
  401992:	75 72                	jne    401a06 <chainhash_x86_avx512+0x4a6>
  401994:	62 f1 7e 48 6f 78 0c 	vmovdqu32 zmm7,ZMMWORD PTR [rax+0x300]
  40199b:	62 f1 45 48 ef 59 0c 	vpxord zmm3,zmm7,ZMMWORD PTR [rcx+0x300]
  4019a2:	62 f1 7e 48 6f 68 0d 	vmovdqu32 zmm5,ZMMWORD PTR [rax+0x340]
  4019a9:	62 f3 65 48 44 db 10 	vpclmullqhqdq zmm3,zmm3,zmm3
  4019b0:	62 f1 7d 48 ef c3    	vpxord zmm0,zmm0,zmm3
  4019b6:	62 f1 55 48 ef 59 0d 	vpxord zmm3,zmm5,ZMMWORD PTR [rcx+0x340]
  4019bd:	62 f1 7e 48 6f 68 0e 	vmovdqu32 zmm5,ZMMWORD PTR [rax+0x380]
  4019c4:	62 f1 fd 48 6f f8    	vmovdqa64 zmm7,zmm0
  4019ca:	62 f3 65 48 44 db 10 	vpclmullqhqdq zmm3,zmm3,zmm3
  4019d1:	62 f1 4d 48 ef f3    	vpxord zmm6,zmm6,zmm3
  4019d7:	62 f1 55 48 ef 59 0e 	vpxord zmm3,zmm5,ZMMWORD PTR [rcx+0x380]
  4019de:	62 f1 7e 48 6f 68 0f 	vmovdqu32 zmm5,ZMMWORD PTR [rax+0x3c0]
  4019e5:	62 f3 65 48 44 db 10 	vpclmullqhqdq zmm3,zmm3,zmm3
  4019ec:	62 f1 75 48 ef cb    	vpxord zmm1,zmm1,zmm3
  4019f2:	62 f1 55 48 ef 59 0f 	vpxord zmm3,zmm5,ZMMWORD PTR [rcx+0x3c0]
  4019f9:	62 f3 65 48 44 db 10 	vpclmullqhqdq zmm3,zmm3,zmm3
  401a00:	62 f1 6d 48 ef d3    	vpxord zmm2,zmm2,zmm3
  401a06:	40 30 f6             	xor    sil,sil
  401a09:	62 f1 75 48 ef ca    	vpxord zmm1,zmm1,zmm2
  401a0f:	0f b6 d2             	movzx  edx,dl
  401a12:	48 81 c6 00 01 00 00 	add    rsi,0x100
  401a19:	62 f1 75 48 ef ce    	vpxord zmm1,zmm1,zmm6
  401a1f:	48 83 fa 3f          	cmp    rdx,0x3f
  401a23:	76 78                	jbe    401a9d <chainhash_x86_avx512+0x53d>
  401a25:	62 f1 7e 48 6f 2c 31 	vmovdqu32 zmm5,ZMMWORD PTR [rcx+rsi*1]
  401a2c:	62 f1 55 48 ef 04 30 	vpxord zmm0,zmm5,ZMMWORD PTR [rax+rsi*1]
  401a33:	4c 8d 42 c0          	lea    r8,[rdx-0x40]
  401a37:	4c 8d 4e 40          	lea    r9,[rsi+0x40]
  401a3b:	62 f3 7d 48 44 c0 10 	vpclmullqhqdq zmm0,zmm0,zmm0
  401a42:	62 f1 7d 48 ef c7    	vpxord zmm0,zmm0,zmm7
  401a48:	49 83 f8 3f          	cmp    r8,0x3f
  401a4c:	76 44                	jbe    401a92 <chainhash_x86_avx512+0x532>
  401a4e:	62 f1 7e 48 6f 7c 31 	vmovdqu32 zmm7,ZMMWORD PTR [rcx+rsi*1+0x40]
  401a55:	01 
  401a56:	62 f1 45 48 ef 54 30 	vpxord zmm2,zmm7,ZMMWORD PTR [rax+rsi*1+0x40]
  401a5d:	01 
  401a5e:	4c 8d 52 80          	lea    r10,[rdx-0x80]
  401a62:	62 f3 6d 48 44 d2 10 	vpclmullqhqdq zmm2,zmm2,zmm2
  401a69:	62 f1 7d 48 ef c2    	vpxord zmm0,zmm0,zmm2
  401a6f:	49 83 fa 3f          	cmp    r10,0x3f
  401a73:	76 1d                	jbe    401a92 <chainhash_x86_avx512+0x532>
  401a75:	62 f1 7e 48 6f 7c 30 	vmovdqu32 zmm7,ZMMWORD PTR [rax+rsi*1+0x80]
  401a7c:	02 
  401a7d:	62 f1 45 48 ef 54 31 	vpxord zmm2,zmm7,ZMMWORD PTR [rcx+rsi*1+0x80]
  401a84:	02 
  401a85:	62 f3 6d 48 44 d2 10 	vpclmullqhqdq zmm2,zmm2,zmm2
  401a8c:	62 f1 7d 48 ef c2    	vpxord zmm0,zmm0,zmm2
  401a92:	49 83 e0 c0          	and    r8,0xffffffffffffffc0
  401a96:	83 e2 3f             	and    edx,0x3f
  401a99:	4b 8d 34 08          	lea    rsi,[r8+r9*1]
  401a9d:	62 f1 75 48 ef c0    	vpxord zmm0,zmm1,zmm0
  401aa3:	62 f3 fd 48 3b c1 01 	vextracti64x4 ymm1,zmm0,0x1
  401aaa:	c5 f5 ef c0          	vpxor  ymm0,ymm1,ymm0
  401aae:	c4 e3 7d 39 c1 01    	vextracti128 xmm1,ymm0,0x1
  401ab4:	c5 f1 ef c0          	vpxor  xmm0,xmm1,xmm0
  401ab8:	48 85 d2             	test   rdx,rdx
  401abb:	0f 85 01 01 00 00    	jne    401bc2 <chainhash_x86_avx512+0x662>
  401ac1:	48 8b 81 08 04 00 00 	mov    rax,QWORD PTR [rcx+0x408]
  401ac8:	c4 e1 f9 6e ff       	vmovq  xmm7,rdi
  401acd:	c5 f9 6f 6c 24 18    	vmovdqa xmm5,XMMWORD PTR [rsp+0x18]
  401ad3:	c5 f9 6f 74 24 08    	vmovdqa xmm6,XMMWORD PTR [rsp+0x8]
  401ad9:	48 31 f8             	xor    rax,rdi
  401adc:	c4 e3 c1 22 c8 01    	vpinsrq xmm1,xmm7,rax,0x1
  401ae2:	48 8b 81 18 04 00 00 	mov    rax,QWORD PTR [rcx+0x418]
  401ae9:	c5 f1 ef c8          	vpxor  xmm1,xmm1,xmm0
  401aed:	c4 e3 71 44 44 24 28 	vpclmulhqlqdq xmm0,xmm1,XMMWORD PTR [rsp+0x28]
  401af4:	01 
  401af5:	c4 e3 79 44 d5 01    	vpclmulhqlqdq xmm2,xmm0,xmm5
  401afb:	c5 e1 73 da 08       	vpsrldq xmm3,xmm2,0x8
  401b00:	c5 e9 ef d1          	vpxor  xmm2,xmm2,xmm1
  401b04:	c5 fa 7e 89 40 04 00 	vmovq  xmm1,QWORD PTR [rcx+0x440]
  401b0b:	00 
  401b0c:	c4 e2 49 00 db       	vpshufb xmm3,xmm6,xmm3
  401b11:	c5 f9 ef c3          	vpxor  xmm0,xmm0,xmm3
  401b15:	c5 e9 ef c0          	vpxor  xmm0,xmm2,xmm0
  401b19:	c5 f9 d4 c1          	vpaddq xmm0,xmm0,xmm1
  401b1d:	c4 e3 79 44 d0 00    	vpclmullqlqdq xmm2,xmm0,xmm0
  401b23:	c4 e3 69 44 cd 01    	vpclmulhqlqdq xmm1,xmm2,xmm5
  401b29:	c5 e1 73 d9 08       	vpsrldq xmm3,xmm1,0x8
  401b2e:	c4 e2 49 00 db       	vpshufb xmm3,xmm6,xmm3
  401b33:	c5 f1 ef cb          	vpxor  xmm1,xmm1,xmm3
  401b37:	c4 e1 f9 6e d8       	vmovq  xmm3,rax
  401b3c:	48 33 81 20 04 00 00 	xor    rax,QWORD PTR [rcx+0x420]
  401b43:	c5 e9 ef d3          	vpxor  xmm2,xmm2,xmm3
  401b47:	c5 fa 7e 99 28 04 00 	vmovq  xmm3,QWORD PTR [rcx+0x428]
  401b4e:	00 
  401b4f:	c5 f1 ef ca          	vpxor  xmm1,xmm1,xmm2
  401b53:	c4 e1 f9 6e d0       	vmovq  xmm2,rax
  401b58:	c5 e9 ef d1          	vpxor  xmm2,xmm2,xmm1
  401b5c:	c5 e1 ef d8          	vpxor  xmm3,xmm3,xmm0
  401b60:	c5 e9 ef d0          	vpxor  xmm2,xmm2,xmm0
  401b64:	c5 fa 7e 81 30 04 00 	vmovq  xmm0,QWORD PTR [rcx+0x430]
  401b6b:	00 
  401b6c:	c4 e3 71 44 ca 00    	vpclmullqlqdq xmm1,xmm1,xmm2
  401b72:	c4 e3 71 44 d5 01    	vpclmulhqlqdq xmm2,xmm1,xmm5
  401b78:	c5 f9 ef c1          	vpxor  xmm0,xmm0,xmm1
  401b7c:	c5 d9 73 da 08       	vpsrldq xmm4,xmm2,0x8
  401b81:	c4 e2 49 00 e4       	vpshufb xmm4,xmm6,xmm4
  401b86:	c5 e9 ef d4          	vpxor  xmm2,xmm2,xmm4
  401b8a:	c5 f9 ef c2          	vpxor  xmm0,xmm0,xmm2
  401b8e:	c5 fa 7e 91 38 04 00 	vmovq  xmm2,QWORD PTR [rcx+0x438]
  401b95:	00 
  401b96:	c4 e3 61 44 d8 00    	vpclmullqlqdq xmm3,xmm3,xmm0
  401b9c:	c4 e3 61 44 c5 01    	vpclmulhqlqdq xmm0,xmm3,xmm5
  401ba2:	c5 e9 ef d3          	vpxor  xmm2,xmm2,xmm3
  401ba6:	c5 f1 73 d8 08       	vpsrldq xmm1,xmm0,0x8
  401bab:	c4 e2 49 00 c9       	vpshufb xmm1,xmm6,xmm1
  401bb0:	c5 f9 ef c1          	vpxor  xmm0,xmm0,xmm1
  401bb4:	c5 e9 ef c0          	vpxor  xmm0,xmm2,xmm0
  401bb8:	c4 e1 f9 7e c0       	vmovq  rax,xmm0
  401bbd:	c5 f8 77             	vzeroupper 
  401bc0:	c9                   	leave  
  401bc1:	c3                   	ret    
  401bc2:	48 01 f0             	add    rax,rsi
  401bc5:	48 01 ce             	add    rsi,rcx
  401bc8:	48 83 fa 0f          	cmp    rdx,0xf
  401bcc:	0f 86 37 01 00 00    	jbe    401d09 <chainhash_x86_avx512+0x7a9>
  401bd2:	c5 fa 6f 3e          	vmovdqu xmm7,XMMWORD PTR [rsi]
  401bd6:	c5 c1 ef 08          	vpxor  xmm1,xmm7,XMMWORD PTR [rax]
  401bda:	4c 8d 42 f0          	lea    r8,[rdx-0x10]
  401bde:	c4 e3 71 44 c9 10    	vpclmullqhqdq xmm1,xmm1,xmm1
  401be4:	49 83 f8 0f          	cmp    r8,0xf
  401be8:	76 32                	jbe    401c1c <chainhash_x86_avx512+0x6bc>
  401bea:	c5 fa 6f 7e 10       	vmovdqu xmm7,XMMWORD PTR [rsi+0x10]
  401bef:	c5 c1 ef 50 10       	vpxor  xmm2,xmm7,XMMWORD PTR [rax+0x10]
  401bf4:	4c 8d 4a e0          	lea    r9,[rdx-0x20]
  401bf8:	c4 e3 69 44 d2 10    	vpclmullqhqdq xmm2,xmm2,xmm2
  401bfe:	c5 f1 ef ca          	vpxor  xmm1,xmm1,xmm2
  401c02:	49 83 f9 0f          	cmp    r9,0xf
  401c06:	76 14                	jbe    401c1c <chainhash_x86_avx512+0x6bc>
  401c08:	c5 fa 6f 78 20       	vmovdqu xmm7,XMMWORD PTR [rax+0x20]
  401c0d:	c5 c1 ef 56 20       	vpxor  xmm2,xmm7,XMMWORD PTR [rsi+0x20]
  401c12:	c4 e3 69 44 d2 10    	vpclmullqhqdq xmm2,xmm2,xmm2
  401c18:	c5 f1 ef ca          	vpxor  xmm1,xmm1,xmm2
  401c1c:	49 83 e0 f0          	and    r8,0xfffffffffffffff0
  401c20:	49 83 c0 10          	add    r8,0x10
  401c24:	83 e2 0f             	and    edx,0xf
  401c27:	75 42                	jne    401c6b <chainhash_x86_avx512+0x70b>
  401c29:	c5 f9 ef c1          	vpxor  xmm0,xmm0,xmm1
  401c2d:	e9 8f fe ff ff       	jmp    401ac1 <chainhash_x86_avx512+0x561>
  401c32:	c5 f9 ef c0          	vpxor  xmm0,xmm0,xmm0
  401c36:	c5 c1 ef ff          	vpxor  xmm7,xmm7,xmm7
  401c3a:	31 f6                	xor    esi,esi
  401c3c:	62 f1 7d 48 6f c8    	vmovdqa32 zmm1,zmm0
  401c42:	e9 d8 fd ff ff       	jmp    401a1f <chainhash_x86_avx512+0x4bf>
  401c47:	c5 f9 6f 3d 01 14 00 	vmovdqa xmm7,XMMWORD PTR [rip+0x1401]        # 403050 <__dso_handle+0x48>
  401c4e:	00 
  401c4f:	48 89 f0             	mov    rax,rsi
  401c52:	c5 f9 7f 7c 24 18    	vmovdqa XMMWORD PTR [rsp+0x18],xmm7
  401c58:	c5 f9 6f 3d 00 14 00 	vmovdqa xmm7,XMMWORD PTR [rip+0x1400]        # 403060 <__dso_handle+0x58>
  401c5f:	00 
  401c60:	c5 f9 7f 7c 24 08    	vmovdqa XMMWORD PTR [rsp+0x8],xmm7
  401c66:	e9 9d fb ff ff       	jmp    401808 <chainhash_x86_avx512+0x2a8>
  401c6b:	4c 01 c0             	add    rax,r8
  401c6e:	4c 01 c6             	add    rsi,r8
  401c71:	c5 e9 ef d2          	vpxor  xmm2,xmm2,xmm2
  401c75:	41 89 d2             	mov    r10d,edx
  401c78:	4c 8d 4c 24 38       	lea    r9,[rsp+0x38]
  401c7d:	49 89 c0             	mov    r8,rax
  401c80:	c5 f9 7f 54 24 38    	vmovdqa XMMWORD PTR [rsp+0x38],xmm2
  401c86:	83 fa 08             	cmp    edx,0x8
  401c89:	73 55                	jae    401ce0 <chainhash_x86_avx512+0x780>
  401c8b:	31 c0                	xor    eax,eax
  401c8d:	41 f6 c2 04          	test   r10b,0x4
  401c91:	75 40                	jne    401cd3 <chainhash_x86_avx512+0x773>
  401c93:	41 f6 c2 02          	test   r10b,0x2
  401c97:	75 2a                	jne    401cc3 <chainhash_x86_avx512+0x763>
  401c99:	41 83 e2 01          	and    r10d,0x1
  401c9d:	75 19                	jne    401cb8 <chainhash_x86_avx512+0x758>
  401c9f:	c5 fa 6f 16          	vmovdqu xmm2,XMMWORD PTR [rsi]
  401ca3:	c5 e9 ef 54 24 38    	vpxor  xmm2,xmm2,XMMWORD PTR [rsp+0x38]
  401ca9:	c4 e3 69 44 d2 10    	vpclmullqhqdq xmm2,xmm2,xmm2
  401caf:	c5 f1 ef ca          	vpxor  xmm1,xmm1,xmm2
  401cb3:	e9 71 ff ff ff       	jmp    401c29 <chainhash_x86_avx512+0x6c9>
  401cb8:	41 0f b6 14 00       	movzx  edx,BYTE PTR [r8+rax*1]
  401cbd:	41 88 14 01          	mov    BYTE PTR [r9+rax*1],dl
  401cc1:	eb dc                	jmp    401c9f <chainhash_x86_avx512+0x73f>
  401cc3:	41 0f b7 14 00       	movzx  edx,WORD PTR [r8+rax*1]
  401cc8:	66 41 89 14 01       	mov    WORD PTR [r9+rax*1],dx
  401ccd:	48 83 c0 02          	add    rax,0x2
  401cd1:	eb c6                	jmp    401c99 <chainhash_x86_avx512+0x739>
  401cd3:	41 8b 00             	mov    eax,DWORD PTR [r8]
  401cd6:	41 89 01             	mov    DWORD PTR [r9],eax
  401cd9:	b8 04 00 00 00       	mov    eax,0x4
  401cde:	eb b3                	jmp    401c93 <chainhash_x86_avx512+0x733>
  401ce0:	83 e2 f8             	and    edx,0xfffffff8
  401ce3:	45 31 c0             	xor    r8d,r8d
  401ce6:	45 89 c1             	mov    r9d,r8d
  401ce9:	41 83 c0 08          	add    r8d,0x8
  401ced:	4e 8b 1c 08          	mov    r11,QWORD PTR [rax+r9*1]
  401cf1:	4e 89 5c 0c 38       	mov    QWORD PTR [rsp+r9*1+0x38],r11
  401cf6:	41 39 d0             	cmp    r8d,edx
  401cf9:	72 eb                	jb     401ce6 <chainhash_x86_avx512+0x786>
  401cfb:	48 8d 54 24 38       	lea    rdx,[rsp+0x38]
  401d00:	4e 8d 0c 02          	lea    r9,[rdx+r8*1]
  401d04:	49 01 c0             	add    r8,rax
  401d07:	eb 82                	jmp    401c8b <chainhash_x86_avx512+0x72b>
  401d09:	c5 f1 ef c9          	vpxor  xmm1,xmm1,xmm1
  401d0d:	e9 5f ff ff ff       	jmp    401c71 <chainhash_x86_avx512+0x711>
  401d12:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
  401d19:	00 00 00 00 
  401d1d:	0f 1f 00             	nop    DWORD PTR [rax]

0000000000401d20 <chainhash_x86_avx2>:
  401d20:	55                   	push   rbp
  401d21:	48 89 f9             	mov    rcx,rdi
  401d24:	48 89 d7             	mov    rdi,rdx
  401d27:	48 89 e5             	mov    rbp,rsp
  401d2a:	48 83 e4 e0          	and    rsp,0xffffffffffffffe0
  401d2e:	48 8b 81 10 04 00 00 	mov    rax,QWORD PTR [rcx+0x410]
  401d35:	48 33 81 00 04 00 00 	xor    rax,QWORD PTR [rcx+0x400]
  401d3c:	c5 7a 6f 89 00 04 00 	vmovdqu xmm9,XMMWORD PTR [rcx+0x400]
  401d43:	00 
  401d44:	c4 e1 f9 6e f0       	vmovq  xmm6,rax
  401d49:	48 81 fa 00 04 00 00 	cmp    rdx,0x400
  401d50:	0f 86 20 03 00 00    	jbe    402076 <chainhash_x86_avx2+0x356>
  401d56:	4c 8d 82 ff fb ff ff 	lea    r8,[rdx-0x401]
  401d5d:	c5 f9 6f 3d eb 12 00 	vmovdqa xmm7,XMMWORD PTR [rip+0x12eb]        # 403050 <__dso_handle+0x48>
  401d64:	00 
  401d65:	c5 79 6f 05 f3 12 00 	vmovdqa xmm8,XMMWORD PTR [rip+0x12f3]        # 403060 <__dso_handle+0x58>
  401d6c:	00 
  401d6d:	49 c1 e8 0a          	shr    r8,0xa
  401d71:	49 8d 50 01          	lea    rdx,[r8+0x1]
  401d75:	48 c1 e2 0a          	shl    rdx,0xa
  401d79:	48 01 f2             	add    rdx,rsi
  401d7c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
  401d80:	c4 41 29 ef d2       	vpxor  xmm10,xmm10,xmm10
  401d85:	31 c0                	xor    eax,eax
  401d87:	c5 79 7f d5          	vmovdqa xmm5,xmm10
  401d8b:	c5 79 7f d4          	vmovdqa xmm4,xmm10
  401d8f:	c5 79 7f d3          	vmovdqa xmm3,xmm10
  401d93:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
  401d98:	c5 fe 6f 14 06       	vmovdqu ymm2,YMMWORD PTR [rsi+rax*1]
  401d9d:	c5 ed ef 0c 01       	vpxor  ymm1,ymm2,YMMWORD PTR [rcx+rax*1]
  401da2:	c5 fe 6f 54 06 20    	vmovdqu ymm2,YMMWORD PTR [rsi+rax*1+0x20]
  401da8:	c5 ed ef 44 01 20    	vpxor  ymm0,ymm2,YMMWORD PTR [rcx+rax*1+0x20]
  401dae:	48 83 c0 40          	add    rax,0x40
  401db2:	c5 79 6f d9          	vmovdqa xmm11,xmm1
  401db6:	c4 e3 7d 39 c9 01    	vextracti128 xmm1,ymm1,0x1
  401dbc:	c5 f9 6f d0          	vmovdqa xmm2,xmm0
  401dc0:	c4 e3 7d 39 c0 01    	vextracti128 xmm0,ymm0,0x1
  401dc6:	c4 e3 71 44 c9 10    	vpclmullqhqdq xmm1,xmm1,xmm1
  401dcc:	c4 e3 69 44 d2 10    	vpclmullqhqdq xmm2,xmm2,xmm2
  401dd2:	c4 43 21 44 db 10    	vpclmullqhqdq xmm11,xmm11,xmm11
  401dd8:	c4 e3 79 44 c0 10    	vpclmullqhqdq xmm0,xmm0,xmm0
  401dde:	c5 a9 ef c0          	vpxor  xmm0,xmm10,xmm0
  401de2:	c4 c1 61 ef db       	vpxor  xmm3,xmm3,xmm11
  401de7:	c5 d9 ef e1          	vpxor  xmm4,xmm4,xmm1
  401deb:	c5 d1 ef ea          	vpxor  xmm5,xmm5,xmm2
  401def:	c5 e1 ef cc          	vpxor  xmm1,xmm3,xmm4
  401df3:	c5 79 6f d0          	vmovdqa xmm10,xmm0
  401df7:	c5 d1 ef d0          	vpxor  xmm2,xmm5,xmm0
  401dfb:	c5 f1 ef ca          	vpxor  xmm1,xmm1,xmm2
  401dff:	48 3d 00 04 00 00    	cmp    rax,0x400
  401e05:	75 91                	jne    401d98 <chainhash_x86_avx2+0x78>
  401e07:	c4 c1 71 ef c9       	vpxor  xmm1,xmm1,xmm9
  401e0c:	48 81 c6 00 04 00 00 	add    rsi,0x400
  401e13:	c4 e3 71 44 f6 01    	vpclmulhqlqdq xmm6,xmm1,xmm6
  401e19:	c4 e3 49 44 c7 01    	vpclmulhqlqdq xmm0,xmm6,xmm7
  401e1f:	c5 c9 ef f1          	vpxor  xmm6,xmm6,xmm1
  401e23:	c5 e9 73 d8 08       	vpsrldq xmm2,xmm0,0x8
  401e28:	c4 e2 39 00 d2       	vpshufb xmm2,xmm8,xmm2
  401e2d:	c5 f9 ef c2          	vpxor  xmm0,xmm0,xmm2
  401e31:	c5 f9 ef f6          	vpxor  xmm6,xmm0,xmm6
  401e35:	48 39 d6             	cmp    rsi,rdx
  401e38:	0f 85 42 ff ff ff    	jne    401d80 <chainhash_x86_avx2+0x60>
  401e3e:	49 f7 d8             	neg    r8
  401e41:	49 c1 e0 0a          	shl    r8,0xa
  401e45:	4e 8d 8c 07 00 fc ff 	lea    r9,[rdi+r8*1-0x400]
  401e4c:	ff 
  401e4d:	49 83 f9 3f          	cmp    r9,0x3f
  401e51:	0f 86 12 02 00 00    	jbe    402069 <chainhash_x86_avx2+0x349>
  401e57:	4d 8d 41 c0          	lea    r8,[r9-0x40]
  401e5b:	c4 41 31 ef c9       	vpxor  xmm9,xmm9,xmm9
  401e60:	31 c0                	xor    eax,eax
  401e62:	c5 79 7f cc          	vmovdqa xmm4,xmm9
  401e66:	c5 79 7f cd          	vmovdqa xmm5,xmm9
  401e6a:	c5 79 7f cb          	vmovdqa xmm3,xmm9
  401e6e:	49 83 e0 c0          	and    r8,0xffffffffffffffc0
  401e72:	4d 8d 50 40          	lea    r10,[r8+0x40]
  401e76:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
  401e7d:	00 00 00 
  401e80:	c5 fe 6f 14 02       	vmovdqu ymm2,YMMWORD PTR [rdx+rax*1]
  401e85:	c5 ed ef 0c 01       	vpxor  ymm1,ymm2,YMMWORD PTR [rcx+rax*1]
  401e8a:	48 89 c6             	mov    rsi,rax
  401e8d:	c5 fe 6f 54 02 20    	vmovdqu ymm2,YMMWORD PTR [rdx+rax*1+0x20]
  401e93:	c5 ed ef 44 01 20    	vpxor  ymm0,ymm2,YMMWORD PTR [rcx+rax*1+0x20]
  401e99:	48 83 c0 40          	add    rax,0x40
  401e9d:	c5 79 6f d1          	vmovdqa xmm10,xmm1
  401ea1:	c4 e3 7d 39 c9 01    	vextracti128 xmm1,ymm1,0x1
  401ea7:	c5 f9 6f d0          	vmovdqa xmm2,xmm0
  401eab:	c4 e3 7d 39 c0 01    	vextracti128 xmm0,ymm0,0x1
  401eb1:	c4 43 29 44 d2 10    	vpclmullqhqdq xmm10,xmm10,xmm10
  401eb7:	c4 e3 71 44 c9 10    	vpclmullqhqdq xmm1,xmm1,xmm1
  401ebd:	c4 e3 69 44 d2 10    	vpclmullqhqdq xmm2,xmm2,xmm2
  401ec3:	c4 e3 79 44 c0 10    	vpclmullqhqdq xmm0,xmm0,xmm0
  401ec9:	c5 d1 ef c9          	vpxor  xmm1,xmm5,xmm1
  401ecd:	c5 b1 ef c0          	vpxor  xmm0,xmm9,xmm0
  401ed1:	c4 c1 61 ef da       	vpxor  xmm3,xmm3,xmm10
  401ed6:	c5 d9 ef e2          	vpxor  xmm4,xmm4,xmm2
  401eda:	c5 61 ef d1          	vpxor  xmm10,xmm3,xmm1
  401ede:	c5 f9 6f e9          	vmovdqa xmm5,xmm1
  401ee2:	c5 79 6f c8          	vmovdqa xmm9,xmm0
  401ee6:	c5 d9 ef c8          	vpxor  xmm1,xmm4,xmm0
  401eea:	4c 39 c6             	cmp    rsi,r8
  401eed:	75 91                	jne    401e80 <chainhash_x86_avx2+0x160>
  401eef:	41 83 e1 3f          	and    r9d,0x3f
  401ef3:	c5 29 ef d1          	vpxor  xmm10,xmm10,xmm1
  401ef7:	4d 85 c9             	test   r9,r9
  401efa:	0f 85 f5 00 00 00    	jne    401ff5 <chainhash_x86_avx2+0x2d5>
  401f00:	48 8b 81 08 04 00 00 	mov    rax,QWORD PTR [rcx+0x408]
  401f07:	c4 e1 f9 6e ef       	vmovq  xmm5,rdi
  401f0c:	48 31 f8             	xor    rax,rdi
  401f0f:	c4 e3 d1 22 c8 01    	vpinsrq xmm1,xmm5,rax,0x1
  401f15:	48 8b 81 18 04 00 00 	mov    rax,QWORD PTR [rcx+0x418]
  401f1c:	c4 c1 71 ef ca       	vpxor  xmm1,xmm1,xmm10
  401f21:	c4 e3 71 44 f6 01    	vpclmulhqlqdq xmm6,xmm1,xmm6
  401f27:	c4 e1 f9 6e e0       	vmovq  xmm4,rax
  401f2c:	48 33 81 20 04 00 00 	xor    rax,QWORD PTR [rcx+0x420]
  401f33:	c4 e3 49 44 d7 01    	vpclmulhqlqdq xmm2,xmm6,xmm7
  401f39:	c5 f9 73 da 08       	vpsrldq xmm0,xmm2,0x8
  401f3e:	c5 e9 ef d1          	vpxor  xmm2,xmm2,xmm1
  401f42:	c5 fa 7e 89 40 04 00 	vmovq  xmm1,QWORD PTR [rcx+0x440]
  401f49:	00 
  401f4a:	c4 e2 39 00 c0       	vpshufb xmm0,xmm8,xmm0
  401f4f:	c5 c9 ef c0          	vpxor  xmm0,xmm6,xmm0
  401f53:	c5 f9 ef c2          	vpxor  xmm0,xmm0,xmm2
  401f57:	c5 f9 d4 c1          	vpaddq xmm0,xmm0,xmm1
  401f5b:	c4 e3 79 44 c8 00    	vpclmullqlqdq xmm1,xmm0,xmm0
  401f61:	c4 e3 71 44 d7 01    	vpclmulhqlqdq xmm2,xmm1,xmm7
  401f67:	c5 f1 ef cc          	vpxor  xmm1,xmm1,xmm4
  401f6b:	c5 e1 73 da 08       	vpsrldq xmm3,xmm2,0x8
  401f70:	c4 e2 39 00 db       	vpshufb xmm3,xmm8,xmm3
  401f75:	c5 e9 ef d3          	vpxor  xmm2,xmm2,xmm3
  401f79:	c5 f1 ef ca          	vpxor  xmm1,xmm1,xmm2
  401f7d:	c4 e1 f9 6e d0       	vmovq  xmm2,rax
  401f82:	c5 e9 ef d1          	vpxor  xmm2,xmm2,xmm1
  401f86:	c5 e9 ef d0          	vpxor  xmm2,xmm2,xmm0
  401f8a:	c4 e3 71 44 ca 00    	vpclmullqlqdq xmm1,xmm1,xmm2
  401f90:	c5 fa 7e 91 28 04 00 	vmovq  xmm2,QWORD PTR [rcx+0x428]
  401f97:	00 
  401f98:	c4 e3 71 44 df 01    	vpclmulhqlqdq xmm3,xmm1,xmm7
  401f9e:	c5 e9 ef c0          	vpxor  xmm0,xmm2,xmm0
  401fa2:	c5 d9 73 db 08       	vpsrldq xmm4,xmm3,0x8
  401fa7:	c5 fa 7e 91 30 04 00 	vmovq  xmm2,QWORD PTR [rcx+0x430]
  401fae:	00 
  401faf:	c4 e2 39 00 e4       	vpshufb xmm4,xmm8,xmm4
  401fb4:	c5 e9 ef c9          	vpxor  xmm1,xmm2,xmm1
  401fb8:	c5 e1 ef dc          	vpxor  xmm3,xmm3,xmm4
  401fbc:	c5 f1 ef cb          	vpxor  xmm1,xmm1,xmm3
  401fc0:	c4 e3 79 44 c9 00    	vpclmullqlqdq xmm1,xmm0,xmm1
  401fc6:	c4 e3 71 44 ff 01    	vpclmulhqlqdq xmm7,xmm1,xmm7
  401fcc:	c5 f9 73 df 08       	vpsrldq xmm0,xmm7,0x8
  401fd1:	c4 62 39 00 c0       	vpshufb xmm8,xmm8,xmm0
  401fd6:	c5 fa 7e 81 38 04 00 	vmovq  xmm0,QWORD PTR [rcx+0x438]
  401fdd:	00 
  401fde:	c4 c1 41 ef f8       	vpxor  xmm7,xmm7,xmm8
  401fe3:	c5 f9 ef c1          	vpxor  xmm0,xmm0,xmm1
  401fe7:	c5 f9 ef c7          	vpxor  xmm0,xmm0,xmm7
  401feb:	c4 e1 f9 7e c0       	vmovq  rax,xmm0
  401ff0:	c5 f8 77             	vzeroupper 
  401ff3:	c9                   	leave  
  401ff4:	c3                   	ret    
  401ff5:	4c 01 d2             	add    rdx,r10
  401ff8:	49 01 ca             	add    r10,rcx
  401ffb:	49 83 f9 0f          	cmp    r9,0xf
  401fff:	0f 86 22 01 00 00    	jbe    402127 <chainhash_x86_avx2+0x407>
  402005:	c4 c1 7a 6f 2a       	vmovdqu xmm5,XMMWORD PTR [r10]
  40200a:	c5 d1 ef 02          	vpxor  xmm0,xmm5,XMMWORD PTR [rdx]
  40200e:	49 8d 41 f0          	lea    rax,[r9-0x10]
  402012:	c4 e3 79 44 c0 10    	vpclmullqhqdq xmm0,xmm0,xmm0
  402018:	48 83 f8 0f          	cmp    rax,0xf
  40201c:	76 34                	jbe    402052 <chainhash_x86_avx2+0x332>
  40201e:	c4 c1 7a 6f 6a 10    	vmovdqu xmm5,XMMWORD PTR [r10+0x10]
  402024:	c5 d1 ef 4a 10       	vpxor  xmm1,xmm5,XMMWORD PTR [rdx+0x10]
  402029:	49 8d 71 e0          	lea    rsi,[r9-0x20]
  40202d:	c4 e3 71 44 c9 10    	vpclmullqhqdq xmm1,xmm1,xmm1
  402033:	c5 f9 ef c1          	vpxor  xmm0,xmm0,xmm1
  402037:	48 83 fe 0f          	cmp    rsi,0xf
  40203b:	76 15                	jbe    402052 <chainhash_x86_avx2+0x332>
  40203d:	c5 fa 6f 6a 20       	vmovdqu xmm5,XMMWORD PTR [rdx+0x20]
  402042:	c4 c1 51 ef 4a 20    	vpxor  xmm1,xmm5,XMMWORD PTR [r10+0x20]
  402048:	c4 e3 71 44 c9 10    	vpclmullqhqdq xmm1,xmm1,xmm1
  40204e:	c5 f9 ef c1          	vpxor  xmm0,xmm0,xmm1
  402052:	48 83 e0 f0          	and    rax,0xfffffffffffffff0
  402056:	48 83 c0 10          	add    rax,0x10
  40205a:	41 83 e1 0f          	and    r9d,0xf
  40205e:	75 31                	jne    402091 <chainhash_x86_avx2+0x371>
  402060:	c5 29 ef d0          	vpxor  xmm10,xmm10,xmm0
  402064:	e9 97 fe ff ff       	jmp    401f00 <chainhash_x86_avx2+0x1e0>
  402069:	c4 41 29 ef d2       	vpxor  xmm10,xmm10,xmm10
  40206e:	45 31 d2             	xor    r10d,r10d
  402071:	e9 81 fe ff ff       	jmp    401ef7 <chainhash_x86_avx2+0x1d7>
  402076:	49 89 d1             	mov    r9,rdx
  402079:	c5 f9 6f 3d cf 0f 00 	vmovdqa xmm7,XMMWORD PTR [rip+0xfcf]        # 403050 <__dso_handle+0x48>
  402080:	00 
  402081:	48 89 f2             	mov    rdx,rsi
  402084:	c5 79 6f 05 d4 0f 00 	vmovdqa xmm8,XMMWORD PTR [rip+0xfd4]        # 403060 <__dso_handle+0x58>
  40208b:	00 
  40208c:	e9 bc fd ff ff       	jmp    401e4d <chainhash_x86_avx2+0x12d>
  402091:	48 01 c2             	add    rdx,rax
  402094:	49 01 c2             	add    r10,rax
  402097:	c5 f1 ef c9          	vpxor  xmm1,xmm1,xmm1
  40209b:	45 89 c8             	mov    r8d,r9d
  40209e:	48 8d 74 24 f0       	lea    rsi,[rsp-0x10]
  4020a3:	48 89 d0             	mov    rax,rdx
  4020a6:	c5 f9 7f 4c 24 f0    	vmovdqa XMMWORD PTR [rsp-0x10],xmm1
  4020ac:	41 83 f9 08          	cmp    r9d,0x8
  4020b0:	73 4f                	jae    402101 <chainhash_x86_avx2+0x3e1>
  4020b2:	31 d2                	xor    edx,edx
  4020b4:	41 f6 c0 04          	test   r8b,0x4
  4020b8:	75 3c                	jne    4020f6 <chainhash_x86_avx2+0x3d6>
  4020ba:	41 f6 c0 02          	test   r8b,0x2
  4020be:	75 26                	jne    4020e6 <chainhash_x86_avx2+0x3c6>
  4020c0:	41 83 e0 01          	and    r8d,0x1
  4020c4:	75 17                	jne    4020dd <chainhash_x86_avx2+0x3bd>
  4020c6:	c4 c1 7a 6f 0a       	vmovdqu xmm1,XMMWORD PTR [r10]
  4020cb:	c5 f1 ef 4c 24 f0    	vpxor  xmm1,xmm1,XMMWORD PTR [rsp-0x10]
  4020d1:	c4 e3 71 44 c9 10    	vpclmullqhqdq xmm1,xmm1,xmm1
  4020d7:	c5 f9 ef c1          	vpxor  xmm0,xmm0,xmm1
  4020db:	eb 83                	jmp    402060 <chainhash_x86_avx2+0x340>
  4020dd:	0f b6 04 10          	movzx  eax,BYTE PTR [rax+rdx*1]
  4020e1:	88 04 16             	mov    BYTE PTR [rsi+rdx*1],al
  4020e4:	eb e0                	jmp    4020c6 <chainhash_x86_avx2+0x3a6>
  4020e6:	44 0f b7 0c 10       	movzx  r9d,WORD PTR [rax+rdx*1]
  4020eb:	66 44 89 0c 16       	mov    WORD PTR [rsi+rdx*1],r9w
  4020f0:	48 83 c2 02          	add    rdx,0x2
  4020f4:	eb ca                	jmp    4020c0 <chainhash_x86_avx2+0x3a0>
  4020f6:	8b 10                	mov    edx,DWORD PTR [rax]
  4020f8:	89 16                	mov    DWORD PTR [rsi],edx
  4020fa:	ba 04 00 00 00       	mov    edx,0x4
  4020ff:	eb b9                	jmp    4020ba <chainhash_x86_avx2+0x39a>
  402101:	41 83 e1 f8          	and    r9d,0xfffffff8
  402105:	31 c0                	xor    eax,eax
  402107:	89 c6                	mov    esi,eax
  402109:	83 c0 08             	add    eax,0x8
  40210c:	4c 8b 1c 32          	mov    r11,QWORD PTR [rdx+rsi*1]
  402110:	4c 89 5c 34 f0       	mov    QWORD PTR [rsp+rsi*1-0x10],r11
  402115:	44 39 c8             	cmp    eax,r9d
  402118:	72 ed                	jb     402107 <chainhash_x86_avx2+0x3e7>
  40211a:	48 8d 74 24 f0       	lea    rsi,[rsp-0x10]
  40211f:	48 01 c6             	add    rsi,rax
  402122:	48 01 d0             	add    rax,rdx
  402125:	eb 8b                	jmp    4020b2 <chainhash_x86_avx2+0x392>
  402127:	c5 f9 ef c0          	vpxor  xmm0,xmm0,xmm0
  40212b:	e9 67 ff ff ff       	jmp    402097 <chainhash_x86_avx2+0x377>

0000000000402130 <hash>:
  402130:	41 57                	push   r15
  402132:	41 56                	push   r14
  402134:	49 89 f6             	mov    r14,rsi
  402137:	41 55                	push   r13
  402139:	49 89 fd             	mov    r13,rdi
  40213c:	41 54                	push   r12
  40213e:	55                   	push   rbp
  40213f:	53                   	push   rbx
  402140:	48 89 54 24 c8       	mov    QWORD PTR [rsp-0x38],rdx
  402145:	8b 35 0d 2f 00 00    	mov    esi,DWORD PTR [rip+0x2f0d]        # 405058 <cached.0>
  40214b:	85 f6                	test   esi,esi
  40214d:	0f 84 97 04 00 00    	je     4025ea <hash+0x4ba>
  402153:	8d 46 ff             	lea    eax,[rsi-0x1]
  402156:	83 fe 03             	cmp    esi,0x3
  402159:	0f 84 2b 05 00 00    	je     40268a <hash+0x55a>
  40215f:	83 f8 01             	cmp    eax,0x1
  402162:	0f 84 dd 04 00 00    	je     402645 <hash+0x515>
  402168:	49 8b 85 10 04 00 00 	mov    rax,QWORD PTR [r13+0x410]
  40216f:	48 8b 6c 24 c8       	mov    rbp,QWORD PTR [rsp-0x38]
  402174:	4c 89 6c 24 d0       	mov    QWORD PTR [rsp-0x30],r13
  402179:	48 89 44 24 d8       	mov    QWORD PTR [rsp-0x28],rax
  40217e:	49 8b 85 00 04 00 00 	mov    rax,QWORD PTR [r13+0x400]
  402185:	48 89 44 24 e0       	mov    QWORD PTR [rsp-0x20],rax
  40218a:	49 8b 85 08 04 00 00 	mov    rax,QWORD PTR [r13+0x408]
  402191:	48 89 44 24 e8       	mov    QWORD PTR [rsp-0x18],rax
  402196:	b8 00 04 00 00       	mov    eax,0x400
  40219b:	48 39 c5             	cmp    rbp,rax
  40219e:	48 0f 46 c5          	cmovbe rax,rbp
  4021a2:	48 89 44 24 c0       	mov    QWORD PTR [rsp-0x40],rax
  4021a7:	48 89 c3             	mov    rbx,rax
  4021aa:	48 83 fd 0f          	cmp    rbp,0xf
  4021ae:	0f 86 43 03 00 00    	jbe    4024f7 <hash+0x3c7>
  4021b4:	48 83 eb 10          	sub    rbx,0x10
  4021b8:	4c 8b 5c 24 d0       	mov    r11,QWORD PTR [rsp-0x30]
  4021bd:	4c 89 f0             	mov    rax,r14
  4021c0:	45 31 c0             	xor    r8d,r8d
  4021c3:	48 89 5c 24 f0       	mov    QWORD PTR [rsp-0x10],rbx
  4021c8:	48 83 e3 f0          	and    rbx,0xfffffffffffffff0
  4021cc:	45 31 e4             	xor    r12d,r12d
  4021cf:	4d 8d 6c 1e 10       	lea    r13,[r14+rbx*1+0x10]
  4021d4:	eb 3f                	jmp    402215 <hash+0xe5>
  4021d6:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
  4021dd:	00 00 00 
  4021e0:	b9 41 00 00 00       	mov    ecx,0x41
  4021e5:	49 89 f7             	mov    r15,rsi
  4021e8:	44 29 d1             	sub    ecx,r10d
  4021eb:	49 d3 ef             	shr    r15,cl
  4021ee:	4c 21 fa             	and    rdx,r15
  4021f1:	48 31 d3             	xor    rbx,rdx
  4021f4:	41 83 fa 40          	cmp    r10d,0x40
  4021f8:	0f 85 ea 00 00 00    	jne    4022e8 <hash+0x1b8>
  4021fe:	48 83 c0 10          	add    rax,0x10
  402202:	4d 31 cc             	xor    r12,r9
  402205:	49 31 d8             	xor    r8,rbx
  402208:	49 83 c3 10          	add    r11,0x10
  40220c:	49 39 c5             	cmp    r13,rax
  40220f:	0f 84 d8 00 00 00    	je     4022ed <hash+0x1bd>
  402215:	0f b6 50 09          	movzx  edx,BYTE PTR [rax+0x9]
  402219:	0f b6 48 0a          	movzx  ecx,BYTE PTR [rax+0xa]
  40221d:	31 db                	xor    ebx,ebx
  40221f:	45 31 c9             	xor    r9d,r9d
  402222:	0f b6 78 0f          	movzx  edi,BYTE PTR [rax+0xf]
  402226:	0f b6 70 07          	movzx  esi,BYTE PTR [rax+0x7]
  40222a:	48 c1 e1 10          	shl    rcx,0x10
  40222e:	48 c1 e2 08          	shl    rdx,0x8
  402232:	48 c1 e7 38          	shl    rdi,0x38
  402236:	48 09 ca             	or     rdx,rcx
  402239:	0f b6 48 08          	movzx  ecx,BYTE PTR [rax+0x8]
  40223d:	48 c1 e6 38          	shl    rsi,0x38
  402241:	48 09 ca             	or     rdx,rcx
  402244:	0f b6 48 0b          	movzx  ecx,BYTE PTR [rax+0xb]
  402248:	48 c1 e1 18          	shl    rcx,0x18
  40224c:	48 09 d1             	or     rcx,rdx
  40224f:	0f b6 50 0c          	movzx  edx,BYTE PTR [rax+0xc]
  402253:	48 c1 e2 20          	shl    rdx,0x20
  402257:	48 09 ca             	or     rdx,rcx
  40225a:	0f b6 48 0d          	movzx  ecx,BYTE PTR [rax+0xd]
  40225e:	48 c1 e1 28          	shl    rcx,0x28
  402262:	48 09 d1             	or     rcx,rdx
  402265:	0f b6 50 0e          	movzx  edx,BYTE PTR [rax+0xe]
  402269:	48 c1 e2 30          	shl    rdx,0x30
  40226d:	48 09 ca             	or     rdx,rcx
  402270:	0f b6 48 02          	movzx  ecx,BYTE PTR [rax+0x2]
  402274:	48 09 d7             	or     rdi,rdx
  402277:	0f b6 50 01          	movzx  edx,BYTE PTR [rax+0x1]
  40227b:	49 33 7b 08          	xor    rdi,QWORD PTR [r11+0x8]
  40227f:	48 c1 e1 10          	shl    rcx,0x10
  402283:	48 c1 e2 08          	shl    rdx,0x8
  402287:	48 09 ca             	or     rdx,rcx
  40228a:	0f b6 08             	movzx  ecx,BYTE PTR [rax]
  40228d:	48 09 ca             	or     rdx,rcx
  402290:	0f b6 48 03          	movzx  ecx,BYTE PTR [rax+0x3]
  402294:	48 c1 e1 18          	shl    rcx,0x18
  402298:	48 09 d1             	or     rcx,rdx
  40229b:	0f b6 50 04          	movzx  edx,BYTE PTR [rax+0x4]
  40229f:	48 c1 e2 20          	shl    rdx,0x20
  4022a3:	48 09 ca             	or     rdx,rcx
  4022a6:	0f b6 48 05          	movzx  ecx,BYTE PTR [rax+0x5]
  4022aa:	48 c1 e1 28          	shl    rcx,0x28
  4022ae:	48 09 d1             	or     rcx,rdx
  4022b1:	0f b6 50 06          	movzx  edx,BYTE PTR [rax+0x6]
  4022b5:	48 c1 e2 30          	shl    rdx,0x30
  4022b9:	48 09 ca             	or     rdx,rcx
  4022bc:	48 09 d6             	or     rsi,rdx
  4022bf:	49 33 33             	xor    rsi,QWORD PTR [r11]
  4022c2:	31 c9                	xor    ecx,ecx
  4022c4:	48 89 fa             	mov    rdx,rdi
  4022c7:	49 89 f2             	mov    r10,rsi
  4022ca:	48 d3 ea             	shr    rdx,cl
  4022cd:	49 d3 e2             	shl    r10,cl
  4022d0:	83 e2 01             	and    edx,0x1
  4022d3:	48 f7 da             	neg    rdx
  4022d6:	49 21 d2             	and    r10,rdx
  4022d9:	4d 31 d1             	xor    r9,r10
  4022dc:	44 8d 51 01          	lea    r10d,[rcx+0x1]
  4022e0:	85 c9                	test   ecx,ecx
  4022e2:	0f 85 f8 fe ff ff    	jne    4021e0 <hash+0xb0>
  4022e8:	44 89 d1             	mov    ecx,r10d
  4022eb:	eb d7                	jmp    4022c4 <hash+0x194>
  4022ed:	48 8b 44 24 f0       	mov    rax,QWORD PTR [rsp-0x10]
  4022f2:	48 8b 54 24 c0       	mov    rdx,QWORD PTR [rsp-0x40]
  4022f7:	48 83 e0 f0          	and    rax,0xfffffffffffffff0
  4022fb:	83 e2 0f             	and    edx,0xf
  4022fe:	48 83 c0 10          	add    rax,0x10
  402302:	48 85 d2             	test   rdx,rdx
  402305:	0f 84 76 01 00 00    	je     402481 <hash+0x351>
  40230b:	41 0f b6 3c 06       	movzx  edi,BYTE PTR [r14+rax*1]
  402310:	48 83 fa 01          	cmp    rdx,0x1
  402314:	74 7f                	je     402395 <hash+0x265>
  402316:	41 0f b6 4c 06 01    	movzx  ecx,BYTE PTR [r14+rax*1+0x1]
  40231c:	48 c1 e1 08          	shl    rcx,0x8
  402320:	48 09 cf             	or     rdi,rcx
  402323:	48 83 fa 02          	cmp    rdx,0x2
  402327:	74 6c                	je     402395 <hash+0x265>
  402329:	41 0f b6 4c 06 02    	movzx  ecx,BYTE PTR [r14+rax*1+0x2]
  40232f:	48 c1 e1 10          	shl    rcx,0x10
  402333:	48 09 cf             	or     rdi,rcx
  402336:	48 83 fa 03          	cmp    rdx,0x3
  40233a:	74 59                	je     402395 <hash+0x265>
  40233c:	41 0f b6 4c 06 03    	movzx  ecx,BYTE PTR [r14+rax*1+0x3]
  402342:	48 c1 e1 18          	shl    rcx,0x18
  402346:	48 09 cf             	or     rdi,rcx
  402349:	48 83 fa 04          	cmp    rdx,0x4
  40234d:	74 46                	je     402395 <hash+0x265>
  40234f:	41 0f b6 4c 06 04    	movzx  ecx,BYTE PTR [r14+rax*1+0x4]
  402355:	48 c1 e1 20          	shl    rcx,0x20
  402359:	48 09 cf             	or     rdi,rcx
  40235c:	48 83 fa 05          	cmp    rdx,0x5
  402360:	74 33                	je     402395 <hash+0x265>
  402362:	41 0f b6 4c 06 05    	movzx  ecx,BYTE PTR [r14+rax*1+0x5]
  402368:	48 c1 e1 28          	shl    rcx,0x28
  40236c:	48 09 cf             	or     rdi,rcx
  40236f:	48 83 fa 06          	cmp    rdx,0x6
  402373:	74 20                	je     402395 <hash+0x265>
  402375:	41 0f b6 4c 06 06    	movzx  ecx,BYTE PTR [r14+rax*1+0x6]
  40237b:	48 c1 e1 30          	shl    rcx,0x30
  40237f:	48 09 cf             	or     rdi,rcx
  402382:	48 83 fa 07          	cmp    rdx,0x7
  402386:	76 0d                	jbe    402395 <hash+0x265>
  402388:	41 0f b6 4c 06 07    	movzx  ecx,BYTE PTR [r14+rax*1+0x7]
  40238e:	48 c1 e1 38          	shl    rcx,0x38
  402392:	48 09 cf             	or     rdi,rcx
  402395:	48 8b 5c 24 d0       	mov    rbx,QWORD PTR [rsp-0x30]
  40239a:	48 8d 70 08          	lea    rsi,[rax+0x8]
  40239e:	45 31 c9             	xor    r9d,r9d
  4023a1:	48 33 3c 03          	xor    rdi,QWORD PTR [rbx+rax*1]
  4023a5:	48 83 fa 08          	cmp    rdx,0x8
  4023a9:	76 7c                	jbe    402427 <hash+0x2f7>
  4023ab:	45 0f b6 4c 06 08    	movzx  r9d,BYTE PTR [r14+rax*1+0x8]
  4023b1:	48 8d 4a f8          	lea    rcx,[rdx-0x8]
  4023b5:	48 83 fa 09          	cmp    rdx,0x9
  4023b9:	74 6c                	je     402427 <hash+0x2f7>
  4023bb:	41 0f b6 54 06 09    	movzx  edx,BYTE PTR [r14+rax*1+0x9]
  4023c1:	48 c1 e2 08          	shl    rdx,0x8
  4023c5:	49 09 d1             	or     r9,rdx
  4023c8:	48 83 f9 02          	cmp    rcx,0x2
  4023cc:	74 59                	je     402427 <hash+0x2f7>
  4023ce:	41 0f b6 54 06 0a    	movzx  edx,BYTE PTR [r14+rax*1+0xa]
  4023d4:	48 c1 e2 10          	shl    rdx,0x10
  4023d8:	49 09 d1             	or     r9,rdx
  4023db:	48 83 f9 03          	cmp    rcx,0x3
  4023df:	74 46                	je     402427 <hash+0x2f7>
  4023e1:	41 0f b6 54 06 0b    	movzx  edx,BYTE PTR [r14+rax*1+0xb]
  4023e7:	48 c1 e2 18          	shl    rdx,0x18
  4023eb:	49 09 d1             	or     r9,rdx
  4023ee:	48 83 f9 04          	cmp    rcx,0x4
  4023f2:	74 33                	je     402427 <hash+0x2f7>
  4023f4:	41 0f b6 54 06 0c    	movzx  edx,BYTE PTR [r14+rax*1+0xc]
  4023fa:	48 c1 e2 20          	shl    rdx,0x20
  4023fe:	49 09 d1             	or     r9,rdx
  402401:	48 83 f9 05          	cmp    rcx,0x5
  402405:	74 20                	je     402427 <hash+0x2f7>
  402407:	41 0f b6 54 06 0d    	movzx  edx,BYTE PTR [r14+rax*1+0xd]
  40240d:	48 c1 e2 28          	shl    rdx,0x28
  402411:	49 09 d1             	or     r9,rdx
  402414:	48 83 f9 07          	cmp    rcx,0x7
  402418:	75 0d                	jne    402427 <hash+0x2f7>
  40241a:	41 0f b6 44 06 0e    	movzx  eax,BYTE PTR [r14+rax*1+0xe]
  402420:	48 c1 e0 30          	shl    rax,0x30
  402424:	49 09 c1             	or     r9,rax
  402427:	48 8b 44 24 d0       	mov    rax,QWORD PTR [rsp-0x30]
  40242c:	45 31 d2             	xor    r10d,r10d
  40242f:	31 d2                	xor    edx,edx
  402431:	31 c9                	xor    ecx,ecx
  402433:	4c 33 0c 30          	xor    r9,QWORD PTR [rax+rsi*1]
  402437:	66 0f 1f 84 00 00 00 	nop    WORD PTR [rax+rax*1+0x0]
  40243e:	00 00 
  402440:	4c 89 c8             	mov    rax,r9
  402443:	48 89 fe             	mov    rsi,rdi
  402446:	48 d3 e8             	shr    rax,cl
  402449:	48 d3 e6             	shl    rsi,cl
  40244c:	83 e0 01             	and    eax,0x1
  40244f:	48 f7 d8             	neg    rax
  402452:	48 21 c6             	and    rsi,rax
  402455:	48 31 f2             	xor    rdx,rsi
  402458:	8d 71 01             	lea    esi,[rcx+0x1]
  40245b:	85 c9                	test   ecx,ecx
  40245d:	0f 84 8d 00 00 00    	je     4024f0 <hash+0x3c0>
  402463:	b9 41 00 00 00       	mov    ecx,0x41
  402468:	48 89 fb             	mov    rbx,rdi
  40246b:	29 f1                	sub    ecx,esi
  40246d:	48 d3 eb             	shr    rbx,cl
  402470:	48 21 d8             	and    rax,rbx
  402473:	49 31 c2             	xor    r10,rax
  402476:	83 fe 40             	cmp    esi,0x40
  402479:	75 75                	jne    4024f0 <hash+0x3c0>
  40247b:	49 31 d4             	xor    r12,rdx
  40247e:	4d 31 d0             	xor    r8,r10
  402481:	48 2b 6c 24 c0       	sub    rbp,QWORD PTR [rsp-0x40]
  402486:	75 0b                	jne    402493 <hash+0x363>
  402488:	48 8b 44 24 c8       	mov    rax,QWORD PTR [rsp-0x38]
  40248d:	49 31 c4             	xor    r12,rax
  402490:	49 31 c0             	xor    r8,rax
  402493:	48 8b 4c 24 d8       	mov    rcx,QWORD PTR [rsp-0x28]
  402498:	4c 33 44 24 e8       	xor    r8,QWORD PTR [rsp-0x18]
  40249d:	ba 40 00 00 00       	mov    edx,0x40
  4024a2:	31 f6                	xor    esi,esi
  4024a4:	48 33 4c 24 e0       	xor    rcx,QWORD PTR [rsp-0x20]
  4024a9:	48 89 c8             	mov    rax,rcx
  4024ac:	48 d1 e9             	shr    rcx,1
  4024af:	83 e0 01             	and    eax,0x1
  4024b2:	48 f7 d8             	neg    rax
  4024b5:	4c 21 c0             	and    rax,r8
  4024b8:	48 31 c6             	xor    rsi,rax
  4024bb:	4b 8d 04 00          	lea    rax,[r8+r8*1]
  4024bf:	49 c1 f8 3f          	sar    r8,0x3f
  4024c3:	41 83 e0 1b          	and    r8d,0x1b
  4024c7:	49 31 c0             	xor    r8,rax
  4024ca:	83 ea 01             	sub    edx,0x1
  4024cd:	75 da                	jne    4024a9 <hash+0x379>
  4024cf:	4c 31 e6             	xor    rsi,r12
  4024d2:	48 89 74 24 d8       	mov    QWORD PTR [rsp-0x28],rsi
  4024d7:	48 85 ed             	test   rbp,rbp
  4024da:	74 2b                	je     402507 <hash+0x3d7>
  4024dc:	4c 03 74 24 c0       	add    r14,QWORD PTR [rsp-0x40]
  4024e1:	e9 b0 fc ff ff       	jmp    402196 <hash+0x66>
  4024e6:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
  4024ed:	00 00 00 
  4024f0:	89 f1                	mov    ecx,esi
  4024f2:	e9 49 ff ff ff       	jmp    402440 <hash+0x310>
  4024f7:	48 89 c2             	mov    rdx,rax
  4024fa:	45 31 c0             	xor    r8d,r8d
  4024fd:	45 31 e4             	xor    r12d,r12d
  402500:	31 c0                	xor    eax,eax
  402502:	e9 fb fd ff ff       	jmp    402302 <hash+0x1d2>
  402507:	4c 8b 6c 24 d0       	mov    r13,QWORD PTR [rsp-0x30]
  40250c:	48 89 f2             	mov    rdx,rsi
  40250f:	31 c0                	xor    eax,eax
  402511:	bf 40 00 00 00       	mov    edi,0x40
  402516:	49 03 95 40 04 00 00 	add    rdx,QWORD PTR [r13+0x440]
  40251d:	48 89 d1             	mov    rcx,rdx
  402520:	49 89 d0             	mov    r8,rdx
  402523:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
  402528:	4c 89 c6             	mov    rsi,r8
  40252b:	49 d1 e8             	shr    r8,1
  40252e:	83 e6 01             	and    esi,0x1
  402531:	48 f7 de             	neg    rsi
  402534:	48 21 ce             	and    rsi,rcx
  402537:	48 31 f0             	xor    rax,rsi
  40253a:	48 8d 34 09          	lea    rsi,[rcx+rcx*1]
  40253e:	48 c1 f9 3f          	sar    rcx,0x3f
  402542:	83 e1 1b             	and    ecx,0x1b
  402545:	48 31 f1             	xor    rcx,rsi
  402548:	83 ef 01             	sub    edi,0x1
  40254b:	75 db                	jne    402528 <hash+0x3f8>
  40254d:	49 8b bd 20 04 00 00 	mov    rdi,QWORD PTR [r13+0x420]
  402554:	31 f6                	xor    esi,esi
  402556:	41 b8 40 00 00 00    	mov    r8d,0x40
  40255c:	48 31 d7             	xor    rdi,rdx
  40255f:	48 31 c7             	xor    rdi,rax
  402562:	49 33 85 18 04 00 00 	xor    rax,QWORD PTR [r13+0x418]
  402569:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
  402570:	48 89 f9             	mov    rcx,rdi
  402573:	48 d1 ef             	shr    rdi,1
  402576:	83 e1 01             	and    ecx,0x1
  402579:	48 f7 d9             	neg    rcx
  40257c:	48 21 c1             	and    rcx,rax
  40257f:	48 31 ce             	xor    rsi,rcx
  402582:	48 8d 0c 00          	lea    rcx,[rax+rax*1]
  402586:	48 c1 f8 3f          	sar    rax,0x3f
  40258a:	83 e0 1b             	and    eax,0x1b
  40258d:	48 31 c8             	xor    rax,rcx
  402590:	41 83 e8 01          	sub    r8d,0x1
  402594:	75 da                	jne    402570 <hash+0x440>
  402596:	49 8b 85 28 04 00 00 	mov    rax,QWORD PTR [r13+0x428]
  40259d:	49 33 b5 30 04 00 00 	xor    rsi,QWORD PTR [r13+0x430]
  4025a4:	48 89 f1             	mov    rcx,rsi
  4025a7:	be 40 00 00 00       	mov    esi,0x40
  4025ac:	48 31 d0             	xor    rax,rdx
  4025af:	90                   	nop
  4025b0:	48 89 ca             	mov    rdx,rcx
  4025b3:	48 d1 e9             	shr    rcx,1
  4025b6:	83 e2 01             	and    edx,0x1
  4025b9:	48 f7 da             	neg    rdx
  4025bc:	48 21 c2             	and    rdx,rax
  4025bf:	48 31 d5             	xor    rbp,rdx
  4025c2:	48 8d 14 00          	lea    rdx,[rax+rax*1]
  4025c6:	48 c1 f8 3f          	sar    rax,0x3f
  4025ca:	83 e0 1b             	and    eax,0x1b
  4025cd:	48 31 d0             	xor    rax,rdx
  4025d0:	83 ee 01             	sub    esi,0x1
  4025d3:	75 db                	jne    4025b0 <hash+0x480>
  4025d5:	49 8b 85 38 04 00 00 	mov    rax,QWORD PTR [r13+0x438]
  4025dc:	5b                   	pop    rbx
  4025dd:	48 31 e8             	xor    rax,rbp
  4025e0:	5d                   	pop    rbp
  4025e1:	41 5c                	pop    r12
  4025e3:	41 5d                	pop    r13
  4025e5:	41 5e                	pop    r14
  4025e7:	41 5f                	pop    r15
  4025e9:	c3                   	ret    
  4025ea:	89 f0                	mov    eax,esi
  4025ec:	0f a2                	cpuid  
  4025ee:	85 c0                	test   eax,eax
  4025f0:	74 6d                	je     40265f <hash+0x52f>
  4025f2:	b8 01 00 00 00       	mov    eax,0x1
  4025f7:	0f a2                	cpuid  
  4025f9:	81 e1 02 02 00 18    	and    ecx,0x18000202
  4025ff:	81 f9 02 02 00 18    	cmp    ecx,0x18000202
  402605:	75 58                	jne    40265f <hash+0x52f>
  402607:	89 f1                	mov    ecx,esi
  402609:	0f 01 d0             	xgetbv 
  40260c:	89 c7                	mov    edi,eax
  40260e:	83 e0 06             	and    eax,0x6
  402611:	83 f8 06             	cmp    eax,0x6
  402614:	75 49                	jne    40265f <hash+0x52f>
  402616:	89 f0                	mov    eax,esi
  402618:	0f a2                	cpuid  
  40261a:	83 f8 06             	cmp    eax,0x6
  40261d:	76 40                	jbe    40265f <hash+0x52f>
  40261f:	b8 07 00 00 00       	mov    eax,0x7
  402624:	89 f1                	mov    ecx,esi
  402626:	0f a2                	cpuid  
  402628:	f6 c3 20             	test   bl,0x20
  40262b:	74 32                	je     40265f <hash+0x52f>
  40262d:	81 e7 e6 00 00 00    	and    edi,0xe6
  402633:	81 ff e6 00 00 00    	cmp    edi,0xe6
  402639:	74 33                	je     40266e <hash+0x53e>
  40263b:	c7 05 13 2a 00 00 02 	mov    DWORD PTR [rip+0x2a13],0x2        # 405058 <cached.0>
  402642:	00 00 00 
  402645:	48 8b 54 24 c8       	mov    rdx,QWORD PTR [rsp-0x38]
  40264a:	4c 89 f6             	mov    rsi,r14
  40264d:	5b                   	pop    rbx
  40264e:	4c 89 ef             	mov    rdi,r13
  402651:	5d                   	pop    rbp
  402652:	41 5c                	pop    r12
  402654:	41 5d                	pop    r13
  402656:	41 5e                	pop    r14
  402658:	41 5f                	pop    r15
  40265a:	e9 c1 f6 ff ff       	jmp    401d20 <chainhash_x86_avx2>
  40265f:	c7 05 ef 29 00 00 01 	mov    DWORD PTR [rip+0x29ef],0x1        # 405058 <cached.0>
  402666:	00 00 00 
  402669:	e9 fa fa ff ff       	jmp    402168 <hash+0x38>
  40266e:	81 e3 00 00 01 00    	and    ebx,0x10000
  402674:	74 c5                	je     40263b <hash+0x50b>
  402676:	80 e5 04             	and    ch,0x4
  402679:	74 c0                	je     40263b <hash+0x50b>
  40267b:	c7 05 d3 29 00 00 03 	mov    DWORD PTR [rip+0x29d3],0x3        # 405058 <cached.0>
  402682:	00 00 00 
  402685:	48 8b 54 24 c8       	mov    rdx,QWORD PTR [rsp-0x38]
  40268a:	5b                   	pop    rbx
  40268b:	4c 89 f6             	mov    rsi,r14
  40268e:	5d                   	pop    rbp
  40268f:	4c 89 ef             	mov    rdi,r13
  402692:	41 5c                	pop    r12
  402694:	41 5d                	pop    r13
  402696:	41 5e                	pop    r14
  402698:	41 5f                	pop    r15
  40269a:	e9 c1 ee ff ff       	jmp    401560 <chainhash_x86_avx512>

Disassembly of section .fini:

00000000004026a0 <_fini>:
  4026a0:	f3 0f 1e fa          	endbr64 
  4026a4:	48 83 ec 08          	sub    rsp,0x8
  4026a8:	48 83 c4 08          	add    rsp,0x8
  4026ac:	c3                   	ret    
