
experiments/bench4096:     file format elf64-x86-64


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
  40108f:	48 81 ec c8 20 00 00 	sub    rsp,0x20c8
  401096:	e8 c5 ff ff ff       	call   401060 <malloc@plt>
  40109b:	48 8d b4 24 70 10 00 	lea    rsi,[rsp+0x1070]
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
  4010cc:	49 b8 ce 86 96 0e 84 	movabs r8,0xfee6ba840e9686ce
  4010d3:	ba e6 fe 
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
  401123:	b9 09 02 00 00       	mov    ecx,0x209
  401128:	66 0f 6f 15 40 1f 00 	movdqa xmm2,XMMWORD PTR [rip+0x1f40]        # 403070 <__dso_handle+0x68>
  40112f:	00 
  401130:	4c 89 e7             	mov    rdi,r12
  401133:	48 89 e8             	mov    rax,rbp
  401136:	66 44 0f 6f 1d 91 1f 	movdqa xmm11,XMMWORD PTR [rip+0x1f91]        # 4030d0 <__dso_handle+0xc8>
  40113d:	00 00 
  40113f:	66 44 0f 6f 15 98 1f 	movdqa xmm10,XMMWORD PTR [rip+0x1f98]        # 4030e0 <__dso_handle+0xd8>
  401146:	00 00 
  401148:	f3 48 a5             	rep movs QWORD PTR es:[rdi],QWORD PTR ds:[rsi]
  40114b:	0f 11 84 24 78 10 00 	movups XMMWORD PTR [rsp+0x1078],xmm0
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
  40117c:	0f 11 84 24 88 10 00 	movups XMMWORD PTR [rsp+0x1088],xmm0
  401183:	00 
  401184:	66 0f 6f 05 14 1f 00 	movdqa xmm0,XMMWORD PTR [rip+0x1f14]        # 4030a0 <__dso_handle+0x98>
  40118b:	00 
  40118c:	66 0f 6f 3d 8c 1f 00 	movdqa xmm7,XMMWORD PTR [rip+0x1f8c]        # 403120 <__dso_handle+0x118>
  401193:	00 
  401194:	66 0f 6f 35 94 1f 00 	movdqa xmm6,XMMWORD PTR [rip+0x1f94]        # 403130 <__dso_handle+0x128>
  40119b:	00 
  40119c:	66 0f 6f 2d 9c 1f 00 	movdqa xmm5,XMMWORD PTR [rip+0x1f9c]        # 403140 <__dso_handle+0x138>
  4011a3:	00 
  4011a4:	0f 11 84 24 98 10 00 	movups XMMWORD PTR [rsp+0x1098],xmm0
  4011ab:	00 
  4011ac:	66 0f 6f 05 fc 1e 00 	movdqa xmm0,XMMWORD PTR [rip+0x1efc]        # 4030b0 <__dso_handle+0xa8>
  4011b3:	00 
  4011b4:	66 0f 6f 25 94 1f 00 	movdqa xmm4,XMMWORD PTR [rip+0x1f94]        # 403150 <__dso_handle+0x148>
  4011bb:	00 
  4011bc:	66 0f 6f 1d 9c 1f 00 	movdqa xmm3,XMMWORD PTR [rip+0x1f9c]        # 403160 <__dso_handle+0x158>
  4011c3:	00 
  4011c4:	0f 11 84 24 a8 10 00 	movups XMMWORD PTR [rsp+0x10a8],xmm0
  4011cb:	00 
  4011cc:	66 0f 6f 05 ec 1e 00 	movdqa xmm0,XMMWORD PTR [rip+0x1eec]        # 4030c0 <__dso_handle+0xb8>
  4011d3:	00 
  4011d4:	0f 11 84 24 b8 10 00 	movups XMMWORD PTR [rsp+0x10b8],xmm0
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
  4012a5:	48 8d 84 24 78 10 00 	lea    rax,[rsp+0x1078]
  4012ac:	00 
  4012ad:	31 db                	xor    ebx,ebx
  4012af:	41 bd a0 86 01 00    	mov    r13d,0x186a0
  4012b5:	48 89 44 24 18       	mov    QWORD PTR [rsp+0x18],rax
  4012ba:	41 be e8 03 00 00    	mov    r14d,0x3e8
  4012c0:	48 89 da             	mov    rdx,rbx
  4012c3:	48 89 ee             	mov    rsi,rbp
  4012c6:	4c 89 e7             	mov    rdi,r12
  4012c9:	e8 02 0c 00 00       	call   401ed0 <hash>
  4012ce:	49 89 c0             	mov    r8,rax
  4012d1:	48 8b 05 88 3d 00 00 	mov    rax,QWORD PTR [rip+0x3d88]        # 405060 <sink>
  4012d8:	4c 31 c0             	xor    rax,r8
  4012db:	48 89 05 7e 3d 00 00 	mov    QWORD PTR [rip+0x3d7e],rax        # 405060 <sink>
  4012e2:	41 83 ee 01          	sub    r14d,0x1
  4012e6:	75 d8                	jne    4012c0 <main+0x240>
  4012e8:	48 89 da             	mov    rdx,rbx
  4012eb:	be 00 10 00 00       	mov    esi,0x1000
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
  40132d:	e8 9e 0b 00 00       	call   401ed0 <hash>
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
  4013c0:	48 8d 84 24 c8 10 00 	lea    rax,[rsp+0x10c8]
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
  40145a:	48 81 c4 c8 20 00 00 	add    rsp,0x20c8
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
  401561:	49 89 d1             	mov    r9,rdx
  401564:	48 89 e5             	mov    rbp,rsp
  401567:	48 83 e4 c0          	and    rsp,0xffffffffffffffc0
  40156b:	48 8b 87 10 10 00 00 	mov    rax,QWORD PTR [rdi+0x1010]
  401572:	48 33 87 00 10 00 00 	xor    rax,QWORD PTR [rdi+0x1000]
  401579:	c5 fa 6f b7 00 10 00 	vmovdqu xmm6,XMMWORD PTR [rdi+0x1000]
  401580:	00 
  401581:	c4 e1 f9 6e d8       	vmovq  xmm3,rax
  401586:	48 81 fa 00 10 00 00 	cmp    rdx,0x1000
  40158d:	0f 86 65 04 00 00    	jbe    4019f8 <chainhash_x86_avx512+0x498>
  401593:	4c 8d 92 ff ef ff ff 	lea    r10,[rdx-0x1001]
  40159a:	48 8d 8f 00 10 00 00 	lea    rcx,[rdi+0x1000]
  4015a1:	c5 f9 6f 25 a7 1a 00 	vmovdqa xmm4,XMMWORD PTR [rip+0x1aa7]        # 403050 <__dso_handle+0x48>
  4015a8:	00 
  4015a9:	c5 f9 6f 2d af 1a 00 	vmovdqa xmm5,XMMWORD PTR [rip+0x1aaf]        # 403060 <__dso_handle+0x58>
  4015b0:	00 
  4015b1:	49 c1 ea 0c          	shr    r10,0xc
  4015b5:	4d 8d 42 01          	lea    r8,[r10+0x1]
  4015b9:	49 c1 e0 0c          	shl    r8,0xc
  4015bd:	49 01 f0             	add    r8,rsi
  4015c0:	c4 41 39 ef c0       	vpxor  xmm8,xmm8,xmm8
  4015c5:	48 89 f8             	mov    rax,rdi
  4015c8:	48 89 f2             	mov    rdx,rsi
  4015cb:	62 51 fd 48 6f d0    	vmovdqa64 zmm10,zmm8
  4015d1:	62 d1 fd 48 6f f8    	vmovdqa64 zmm7,zmm8
  4015d7:	62 51 fd 48 6f c8    	vmovdqa64 zmm9,zmm8
  4015dd:	0f 1f 00             	nop    DWORD PTR [rax]
  4015e0:	62 f1 7e 48 6f 0a    	vmovdqu32 zmm1,ZMMWORD PTR [rdx]
  4015e6:	62 f1 75 48 ef 00    	vpxord zmm0,zmm1,ZMMWORD PTR [rax]
  4015ec:	48 05 00 01 00 00    	add    rax,0x100
  4015f2:	48 81 c2 00 01 00 00 	add    rdx,0x100
  4015f9:	62 f1 7e 48 6f 4a fd 	vmovdqu32 zmm1,ZMMWORD PTR [rdx-0xc0]
  401600:	62 f1 75 48 ef 48 fd 	vpxord zmm1,zmm1,ZMMWORD PTR [rax-0xc0]
  401607:	62 f3 7d 48 44 c0 10 	vpclmullqhqdq zmm0,zmm0,zmm0
  40160e:	62 f3 75 48 44 c9 10 	vpclmullqhqdq zmm1,zmm1,zmm1
  401615:	62 f1 35 48 ef c0    	vpxord zmm0,zmm9,zmm0
  40161b:	62 f1 45 48 ef d1    	vpxord zmm2,zmm7,zmm1
  401621:	62 71 fd 48 6f c8    	vmovdqa64 zmm9,zmm0
  401627:	62 f1 7d 48 ef ca    	vpxord zmm1,zmm0,zmm2
  40162d:	62 f1 fd 48 6f fa    	vmovdqa64 zmm7,zmm2
  401633:	62 f1 7e 48 6f 52 fe 	vmovdqu32 zmm2,ZMMWORD PTR [rdx-0x80]
  40163a:	62 f1 6d 48 ef 40 fe 	vpxord zmm0,zmm2,ZMMWORD PTR [rax-0x80]
  401641:	62 f1 7e 48 6f 52 ff 	vmovdqu32 zmm2,ZMMWORD PTR [rdx-0x40]
  401648:	62 f1 6d 48 ef 50 ff 	vpxord zmm2,zmm2,ZMMWORD PTR [rax-0x40]
  40164f:	62 f3 7d 48 44 c0 10 	vpclmullqhqdq zmm0,zmm0,zmm0
  401656:	62 f3 6d 48 44 d2 10 	vpclmullqhqdq zmm2,zmm2,zmm2
  40165d:	62 f1 2d 48 ef c0    	vpxord zmm0,zmm10,zmm0
  401663:	62 f1 3d 48 ef d2    	vpxord zmm2,zmm8,zmm2
  401669:	62 71 fd 48 6f d0    	vmovdqa64 zmm10,zmm0
  40166f:	62 71 fd 48 6f c2    	vmovdqa64 zmm8,zmm2
  401675:	62 f1 7d 48 ef c2    	vpxord zmm0,zmm0,zmm2
  40167b:	48 39 c8             	cmp    rax,rcx
  40167e:	0f 85 5c ff ff ff    	jne    4015e0 <chainhash_x86_avx512+0x80>
  401684:	62 f1 7d 48 ef c1    	vpxord zmm0,zmm0,zmm1
  40168a:	48 81 c6 00 10 00 00 	add    rsi,0x1000
  401691:	62 f3 fd 48 3b c1 01 	vextracti64x4 ymm1,zmm0,0x1
  401698:	c5 f5 ef c8          	vpxor  ymm1,ymm1,ymm0
  40169c:	c4 e3 7d 39 c8 01    	vextracti128 xmm0,ymm1,0x1
  4016a2:	c5 f9 ef c1          	vpxor  xmm0,xmm0,xmm1
  4016a6:	c5 f9 ef c6          	vpxor  xmm0,xmm0,xmm6
  4016aa:	c4 e3 79 44 db 01    	vpclmulhqlqdq xmm3,xmm0,xmm3
  4016b0:	c4 e3 61 44 cc 01    	vpclmulhqlqdq xmm1,xmm3,xmm4
  4016b6:	c5 e1 ef d8          	vpxor  xmm3,xmm3,xmm0
  4016ba:	c5 e9 73 d9 08       	vpsrldq xmm2,xmm1,0x8
  4016bf:	c4 e2 51 00 d2       	vpshufb xmm2,xmm5,xmm2
  4016c4:	c5 f1 ef ca          	vpxor  xmm1,xmm1,xmm2
  4016c8:	c5 f1 ef db          	vpxor  xmm3,xmm1,xmm3
  4016cc:	4c 39 c6             	cmp    rsi,r8
  4016cf:	0f 85 eb fe ff ff    	jne    4015c0 <chainhash_x86_avx512+0x60>
  4016d5:	49 f7 da             	neg    r10
  4016d8:	49 c1 e2 0c          	shl    r10,0xc
  4016dc:	4f 8d 94 11 00 f0 ff 	lea    r10,[r9+r10*1-0x1000]
  4016e3:	ff 
  4016e4:	49 81 fa ff 00 00 00 	cmp    r10,0xff
  4016eb:	0f 86 f2 02 00 00    	jbe    4019e3 <chainhash_x86_avx512+0x483>
  4016f1:	49 8d 8a 00 ff ff ff 	lea    rcx,[r10-0x100]
  4016f8:	c4 41 29 ef d2       	vpxor  xmm10,xmm10,xmm10
  4016fd:	48 89 f8             	mov    rax,rdi
  401700:	4c 89 c2             	mov    rdx,r8
  401703:	48 89 ce             	mov    rsi,rcx
  401706:	62 51 fd 48 6f ca    	vmovdqa64 zmm9,zmm10
  40170c:	62 51 fd 48 6f c2    	vmovdqa64 zmm8,zmm10
  401712:	40 30 f6             	xor    sil,sil
  401715:	62 d1 fd 48 6f fa    	vmovdqa64 zmm7,zmm10
  40171b:	48 8d b4 37 00 01 00 	lea    rsi,[rdi+rsi*1+0x100]
  401722:	00 
  401723:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
  401728:	62 f1 7e 48 6f 32    	vmovdqu32 zmm6,ZMMWORD PTR [rdx]
  40172e:	62 f1 4d 48 ef 00    	vpxord zmm0,zmm6,ZMMWORD PTR [rax]
  401734:	48 05 00 01 00 00    	add    rax,0x100
  40173a:	48 81 c2 00 01 00 00 	add    rdx,0x100
  401741:	62 f1 7e 48 6f 72 fd 	vmovdqu32 zmm6,ZMMWORD PTR [rdx-0xc0]
  401748:	62 f3 7d 48 44 c0 10 	vpclmullqhqdq zmm0,zmm0,zmm0
  40174f:	62 f1 45 48 ef c8    	vpxord zmm1,zmm7,zmm0
  401755:	62 f1 4d 48 ef 40 fd 	vpxord zmm0,zmm6,ZMMWORD PTR [rax-0xc0]
  40175c:	62 f1 7e 48 6f 72 fe 	vmovdqu32 zmm6,ZMMWORD PTR [rdx-0x80]
  401763:	62 f1 4d 48 ef 50 fe 	vpxord zmm2,zmm6,ZMMWORD PTR [rax-0x80]
  40176a:	62 f1 fd 48 6f f9    	vmovdqa64 zmm7,zmm1
  401770:	62 f3 7d 48 44 c0 10 	vpclmullqhqdq zmm0,zmm0,zmm0
  401777:	62 f3 6d 48 44 d2 10 	vpclmullqhqdq zmm2,zmm2,zmm2
  40177e:	62 f1 3d 48 ef c0    	vpxord zmm0,zmm8,zmm0
  401784:	62 f1 35 48 ef f2    	vpxord zmm6,zmm9,zmm2
  40178a:	62 f1 7e 48 6f 52 ff 	vmovdqu32 zmm2,ZMMWORD PTR [rdx-0x40]
  401791:	62 f1 6d 48 ef 50 ff 	vpxord zmm2,zmm2,ZMMWORD PTR [rax-0x40]
  401798:	62 71 fd 48 6f c0    	vmovdqa64 zmm8,zmm0
  40179e:	62 71 fd 48 6f ce    	vmovdqa64 zmm9,zmm6
  4017a4:	62 f3 6d 48 44 d2 10 	vpclmullqhqdq zmm2,zmm2,zmm2
  4017ab:	62 f1 2d 48 ef d2    	vpxord zmm2,zmm10,zmm2
  4017b1:	62 71 fd 48 6f d2    	vmovdqa64 zmm10,zmm2
  4017b7:	48 39 f0             	cmp    rax,rsi
  4017ba:	0f 85 68 ff ff ff    	jne    401728 <chainhash_x86_avx512+0x1c8>
  4017c0:	30 c9                	xor    cl,cl
  4017c2:	62 f1 7d 48 ef c6    	vpxord zmm0,zmm0,zmm6
  4017c8:	45 0f b6 d2          	movzx  r10d,r10b
  4017cc:	48 8d 91 00 01 00 00 	lea    rdx,[rcx+0x100]
  4017d3:	62 f1 7d 48 ef c2    	vpxord zmm0,zmm0,zmm2
  4017d9:	49 83 fa 3f          	cmp    r10,0x3f
  4017dd:	76 79                	jbe    401858 <chainhash_x86_avx512+0x2f8>
  4017df:	62 d1 7e 48 6f 34 10 	vmovdqu32 zmm6,ZMMWORD PTR [r8+rdx*1]
  4017e6:	62 f1 4d 48 ef 0c 17 	vpxord zmm1,zmm6,ZMMWORD PTR [rdi+rdx*1]
  4017ed:	49 8d 42 c0          	lea    rax,[r10-0x40]
  4017f1:	48 8d 4a 40          	lea    rcx,[rdx+0x40]
  4017f5:	62 f3 75 48 44 c9 10 	vpclmullqhqdq zmm1,zmm1,zmm1
  4017fc:	62 f1 45 48 ef c9    	vpxord zmm1,zmm7,zmm1
  401802:	48 83 f8 3f          	cmp    rax,0x3f
  401806:	76 44                	jbe    40184c <chainhash_x86_avx512+0x2ec>
  401808:	62 d1 7e 48 6f 74 10 	vmovdqu32 zmm6,ZMMWORD PTR [r8+rdx*1+0x40]
  40180f:	01 
  401810:	62 f1 4d 48 ef 54 17 	vpxord zmm2,zmm6,ZMMWORD PTR [rdi+rdx*1+0x40]
  401817:	01 
  401818:	49 8d 72 80          	lea    rsi,[r10-0x80]
  40181c:	62 f3 6d 48 44 d2 10 	vpclmullqhqdq zmm2,zmm2,zmm2
  401823:	62 f1 75 48 ef ca    	vpxord zmm1,zmm1,zmm2
  401829:	48 83 fe 3f          	cmp    rsi,0x3f
  40182d:	76 1d                	jbe    40184c <chainhash_x86_avx512+0x2ec>
  40182f:	62 d1 7e 48 6f 74 10 	vmovdqu32 zmm6,ZMMWORD PTR [r8+rdx*1+0x80]
  401836:	02 
  401837:	62 f1 4d 48 ef 54 17 	vpxord zmm2,zmm6,ZMMWORD PTR [rdi+rdx*1+0x80]
  40183e:	02 
  40183f:	62 f3 6d 48 44 d2 10 	vpclmullqhqdq zmm2,zmm2,zmm2
  401846:	62 f1 75 48 ef ca    	vpxord zmm1,zmm1,zmm2
  40184c:	48 83 e0 c0          	and    rax,0xffffffffffffffc0
  401850:	41 83 e2 3f          	and    r10d,0x3f
  401854:	48 8d 14 08          	lea    rdx,[rax+rcx*1]
  401858:	62 f1 7d 48 ef c1    	vpxord zmm0,zmm0,zmm1
  40185e:	62 f3 fd 48 3b c1 01 	vextracti64x4 ymm1,zmm0,0x1
  401865:	c5 f5 ef c0          	vpxor  ymm0,ymm1,ymm0
  401869:	c4 e3 7d 39 c1 01    	vextracti128 xmm1,ymm0,0x1
  40186f:	c5 f1 ef c0          	vpxor  xmm0,xmm1,xmm0
  401873:	4d 85 d2             	test   r10,r10
  401876:	0f 85 f3 00 00 00    	jne    40196f <chainhash_x86_avx512+0x40f>
  40187c:	48 8b 87 08 10 00 00 	mov    rax,QWORD PTR [rdi+0x1008]
  401883:	c4 c1 f9 6e f1       	vmovq  xmm6,r9
  401888:	4c 31 c8             	xor    rax,r9
  40188b:	c4 e3 c9 22 c8 01    	vpinsrq xmm1,xmm6,rax,0x1
  401891:	48 8b 87 18 10 00 00 	mov    rax,QWORD PTR [rdi+0x1018]
  401898:	c5 f1 ef c8          	vpxor  xmm1,xmm1,xmm0
  40189c:	c4 e3 71 44 db 01    	vpclmulhqlqdq xmm3,xmm1,xmm3
  4018a2:	c4 e1 f9 6e f0       	vmovq  xmm6,rax
  4018a7:	48 33 87 20 10 00 00 	xor    rax,QWORD PTR [rdi+0x1020]
  4018ae:	c4 e3 61 44 d4 01    	vpclmulhqlqdq xmm2,xmm3,xmm4
  4018b4:	c5 f9 73 da 08       	vpsrldq xmm0,xmm2,0x8
  4018b9:	c5 e9 ef d1          	vpxor  xmm2,xmm2,xmm1
  4018bd:	c5 fa 7e 8f 40 10 00 	vmovq  xmm1,QWORD PTR [rdi+0x1040]
  4018c4:	00 
  4018c5:	c4 e2 51 00 c0       	vpshufb xmm0,xmm5,xmm0
  4018ca:	c5 e1 ef c0          	vpxor  xmm0,xmm3,xmm0
  4018ce:	c5 f9 ef c2          	vpxor  xmm0,xmm0,xmm2
  4018d2:	c5 f9 d4 c1          	vpaddq xmm0,xmm0,xmm1
  4018d6:	c4 e3 79 44 c8 00    	vpclmullqlqdq xmm1,xmm0,xmm0
  4018dc:	c4 e3 71 44 d4 01    	vpclmulhqlqdq xmm2,xmm1,xmm4
  4018e2:	c5 f1 ef ce          	vpxor  xmm1,xmm1,xmm6
  4018e6:	c5 e1 73 da 08       	vpsrldq xmm3,xmm2,0x8
  4018eb:	c4 e2 51 00 db       	vpshufb xmm3,xmm5,xmm3
  4018f0:	c5 e9 ef d3          	vpxor  xmm2,xmm2,xmm3
  4018f4:	c5 f1 ef ca          	vpxor  xmm1,xmm1,xmm2
  4018f8:	c4 e1 f9 6e d0       	vmovq  xmm2,rax
  4018fd:	c5 e9 ef d1          	vpxor  xmm2,xmm2,xmm1
  401901:	c5 e9 ef d0          	vpxor  xmm2,xmm2,xmm0
  401905:	c4 e3 71 44 ca 00    	vpclmullqlqdq xmm1,xmm1,xmm2
  40190b:	c5 fa 7e 97 28 10 00 	vmovq  xmm2,QWORD PTR [rdi+0x1028]
  401912:	00 
  401913:	c4 e3 71 44 dc 01    	vpclmulhqlqdq xmm3,xmm1,xmm4
  401919:	c5 e9 ef c0          	vpxor  xmm0,xmm2,xmm0
  40191d:	c5 c9 73 db 08       	vpsrldq xmm6,xmm3,0x8
  401922:	c5 fa 7e 97 30 10 00 	vmovq  xmm2,QWORD PTR [rdi+0x1030]
  401929:	00 
  40192a:	c4 e2 51 00 f6       	vpshufb xmm6,xmm5,xmm6
  40192f:	c5 e9 ef c9          	vpxor  xmm1,xmm2,xmm1
  401933:	c5 e1 ef de          	vpxor  xmm3,xmm3,xmm6
  401937:	c5 f1 ef cb          	vpxor  xmm1,xmm1,xmm3
  40193b:	c4 e3 79 44 c9 00    	vpclmullqlqdq xmm1,xmm0,xmm1
  401941:	c4 e3 71 44 e4 01    	vpclmulhqlqdq xmm4,xmm1,xmm4
  401947:	c5 f9 73 dc 08       	vpsrldq xmm0,xmm4,0x8
  40194c:	c4 e2 51 00 e8       	vpshufb xmm5,xmm5,xmm0
  401951:	c5 fa 7e 87 38 10 00 	vmovq  xmm0,QWORD PTR [rdi+0x1038]
  401958:	00 
  401959:	c5 d9 ef e5          	vpxor  xmm4,xmm4,xmm5
  40195d:	c5 f9 ef c1          	vpxor  xmm0,xmm0,xmm1
  401961:	c5 f9 ef c4          	vpxor  xmm0,xmm0,xmm4
  401965:	c4 e1 f9 7e c0       	vmovq  rax,xmm0
  40196a:	c5 f8 77             	vzeroupper 
  40196d:	c9                   	leave  
  40196e:	c3                   	ret    
  40196f:	49 01 d0             	add    r8,rdx
  401972:	48 01 fa             	add    rdx,rdi
  401975:	49 83 fa 0f          	cmp    r10,0xf
  401979:	0f 86 2f 01 00 00    	jbe    401aae <chainhash_x86_avx512+0x54e>
  40197f:	c4 c1 7a 6f 30       	vmovdqu xmm6,XMMWORD PTR [r8]
  401984:	c5 c9 ef 0a          	vpxor  xmm1,xmm6,XMMWORD PTR [rdx]
  401988:	49 8d 42 f0          	lea    rax,[r10-0x10]
  40198c:	c4 e3 71 44 c9 10    	vpclmullqhqdq xmm1,xmm1,xmm1
  401992:	48 83 f8 0f          	cmp    rax,0xf
  401996:	76 34                	jbe    4019cc <chainhash_x86_avx512+0x46c>
  401998:	c4 c1 7a 6f 70 10    	vmovdqu xmm6,XMMWORD PTR [r8+0x10]
  40199e:	c5 c9 ef 52 10       	vpxor  xmm2,xmm6,XMMWORD PTR [rdx+0x10]
  4019a3:	49 8d 4a e0          	lea    rcx,[r10-0x20]
  4019a7:	c4 e3 69 44 d2 10    	vpclmullqhqdq xmm2,xmm2,xmm2
  4019ad:	c5 f1 ef ca          	vpxor  xmm1,xmm1,xmm2
  4019b1:	48 83 f9 0f          	cmp    rcx,0xf
  4019b5:	76 15                	jbe    4019cc <chainhash_x86_avx512+0x46c>
  4019b7:	c4 c1 7a 6f 70 20    	vmovdqu xmm6,XMMWORD PTR [r8+0x20]
  4019bd:	c5 c9 ef 52 20       	vpxor  xmm2,xmm6,XMMWORD PTR [rdx+0x20]
  4019c2:	c4 e3 69 44 d2 10    	vpclmullqhqdq xmm2,xmm2,xmm2
  4019c8:	c5 f1 ef ca          	vpxor  xmm1,xmm1,xmm2
  4019cc:	48 83 e0 f0          	and    rax,0xfffffffffffffff0
  4019d0:	48 83 c0 10          	add    rax,0x10
  4019d4:	41 83 e2 0f          	and    r10d,0xf
  4019d8:	75 39                	jne    401a13 <chainhash_x86_avx512+0x4b3>
  4019da:	c5 f9 ef c1          	vpxor  xmm0,xmm0,xmm1
  4019de:	e9 99 fe ff ff       	jmp    40187c <chainhash_x86_avx512+0x31c>
  4019e3:	c5 f1 ef c9          	vpxor  xmm1,xmm1,xmm1
  4019e7:	c5 c1 ef ff          	vpxor  xmm7,xmm7,xmm7
  4019eb:	31 d2                	xor    edx,edx
  4019ed:	62 f1 7d 48 6f c1    	vmovdqa32 zmm0,zmm1
  4019f3:	e9 e1 fd ff ff       	jmp    4017d9 <chainhash_x86_avx512+0x279>
  4019f8:	c5 f9 6f 25 50 16 00 	vmovdqa xmm4,XMMWORD PTR [rip+0x1650]        # 403050 <__dso_handle+0x48>
  4019ff:	00 
  401a00:	49 89 d2             	mov    r10,rdx
  401a03:	49 89 f0             	mov    r8,rsi
  401a06:	c5 f9 6f 2d 52 16 00 	vmovdqa xmm5,XMMWORD PTR [rip+0x1652]        # 403060 <__dso_handle+0x58>
  401a0d:	00 
  401a0e:	e9 d1 fc ff ff       	jmp    4016e4 <chainhash_x86_avx512+0x184>
  401a13:	49 01 c0             	add    r8,rax
  401a16:	48 01 c2             	add    rdx,rax
  401a19:	c5 e9 ef d2          	vpxor  xmm2,xmm2,xmm2
  401a1d:	45 89 d3             	mov    r11d,r10d
  401a20:	48 8d 74 24 f0       	lea    rsi,[rsp-0x10]
  401a25:	4c 89 c0             	mov    rax,r8
  401a28:	c5 f9 7f 54 24 f0    	vmovdqa XMMWORD PTR [rsp-0x10],xmm2
  401a2e:	41 83 fa 08          	cmp    r10d,0x8
  401a32:	73 51                	jae    401a85 <chainhash_x86_avx512+0x525>
  401a34:	31 c9                	xor    ecx,ecx
  401a36:	41 f6 c3 04          	test   r11b,0x4
  401a3a:	75 3e                	jne    401a7a <chainhash_x86_avx512+0x51a>
  401a3c:	41 f6 c3 02          	test   r11b,0x2
  401a40:	75 28                	jne    401a6a <chainhash_x86_avx512+0x50a>
  401a42:	41 83 e3 01          	and    r11d,0x1
  401a46:	75 19                	jne    401a61 <chainhash_x86_avx512+0x501>
  401a48:	c5 fa 6f 12          	vmovdqu xmm2,XMMWORD PTR [rdx]
  401a4c:	c5 e9 ef 54 24 f0    	vpxor  xmm2,xmm2,XMMWORD PTR [rsp-0x10]
  401a52:	c4 e3 69 44 d2 10    	vpclmullqhqdq xmm2,xmm2,xmm2
  401a58:	c5 f1 ef ca          	vpxor  xmm1,xmm1,xmm2
  401a5c:	e9 79 ff ff ff       	jmp    4019da <chainhash_x86_avx512+0x47a>
  401a61:	0f b6 04 08          	movzx  eax,BYTE PTR [rax+rcx*1]
  401a65:	88 04 0e             	mov    BYTE PTR [rsi+rcx*1],al
  401a68:	eb de                	jmp    401a48 <chainhash_x86_avx512+0x4e8>
  401a6a:	44 0f b7 04 08       	movzx  r8d,WORD PTR [rax+rcx*1]
  401a6f:	66 44 89 04 0e       	mov    WORD PTR [rsi+rcx*1],r8w
  401a74:	48 83 c1 02          	add    rcx,0x2
  401a78:	eb c8                	jmp    401a42 <chainhash_x86_avx512+0x4e2>
  401a7a:	8b 08                	mov    ecx,DWORD PTR [rax]
  401a7c:	89 0e                	mov    DWORD PTR [rsi],ecx
  401a7e:	b9 04 00 00 00       	mov    ecx,0x4
  401a83:	eb b7                	jmp    401a3c <chainhash_x86_avx512+0x4dc>
  401a85:	44 89 d0             	mov    eax,r10d
  401a88:	31 c9                	xor    ecx,ecx
  401a8a:	83 e0 f8             	and    eax,0xfffffff8
  401a8d:	89 ce                	mov    esi,ecx
  401a8f:	83 c1 08             	add    ecx,0x8
  401a92:	4d 8b 14 30          	mov    r10,QWORD PTR [r8+rsi*1]
  401a96:	4c 89 54 34 f0       	mov    QWORD PTR [rsp+rsi*1-0x10],r10
  401a9b:	39 c1                	cmp    ecx,eax
  401a9d:	72 ee                	jb     401a8d <chainhash_x86_avx512+0x52d>
  401a9f:	89 c8                	mov    eax,ecx
  401aa1:	48 8d 74 24 f0       	lea    rsi,[rsp-0x10]
  401aa6:	48 01 c6             	add    rsi,rax
  401aa9:	4c 01 c0             	add    rax,r8
  401aac:	eb 86                	jmp    401a34 <chainhash_x86_avx512+0x4d4>
  401aae:	c5 f1 ef c9          	vpxor  xmm1,xmm1,xmm1
  401ab2:	e9 62 ff ff ff       	jmp    401a19 <chainhash_x86_avx512+0x4b9>
  401ab7:	66 0f 1f 84 00 00 00 	nop    WORD PTR [rax+rax*1+0x0]
  401abe:	00 00 

0000000000401ac0 <chainhash_x86_avx2>:
  401ac0:	55                   	push   rbp
  401ac1:	48 89 f9             	mov    rcx,rdi
  401ac4:	48 89 d7             	mov    rdi,rdx
  401ac7:	48 89 e5             	mov    rbp,rsp
  401aca:	48 83 e4 e0          	and    rsp,0xffffffffffffffe0
  401ace:	48 8b 81 10 10 00 00 	mov    rax,QWORD PTR [rcx+0x1010]
  401ad5:	48 33 81 00 10 00 00 	xor    rax,QWORD PTR [rcx+0x1000]
  401adc:	c5 7a 6f 89 00 10 00 	vmovdqu xmm9,XMMWORD PTR [rcx+0x1000]
  401ae3:	00 
  401ae4:	c4 e1 f9 6e f0       	vmovq  xmm6,rax
  401ae9:	48 81 fa 00 10 00 00 	cmp    rdx,0x1000
  401af0:	0f 86 20 03 00 00    	jbe    401e16 <chainhash_x86_avx2+0x356>
  401af6:	4c 8d 82 ff ef ff ff 	lea    r8,[rdx-0x1001]
  401afd:	c5 f9 6f 3d 4b 15 00 	vmovdqa xmm7,XMMWORD PTR [rip+0x154b]        # 403050 <__dso_handle+0x48>
  401b04:	00 
  401b05:	c5 79 6f 05 53 15 00 	vmovdqa xmm8,XMMWORD PTR [rip+0x1553]        # 403060 <__dso_handle+0x58>
  401b0c:	00 
  401b0d:	49 c1 e8 0c          	shr    r8,0xc
  401b11:	49 8d 50 01          	lea    rdx,[r8+0x1]
  401b15:	48 c1 e2 0c          	shl    rdx,0xc
  401b19:	48 01 f2             	add    rdx,rsi
  401b1c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
  401b20:	c4 41 29 ef d2       	vpxor  xmm10,xmm10,xmm10
  401b25:	31 c0                	xor    eax,eax
  401b27:	c5 79 7f d5          	vmovdqa xmm5,xmm10
  401b2b:	c5 79 7f d4          	vmovdqa xmm4,xmm10
  401b2f:	c5 79 7f d3          	vmovdqa xmm3,xmm10
  401b33:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
  401b38:	c5 fe 6f 14 06       	vmovdqu ymm2,YMMWORD PTR [rsi+rax*1]
  401b3d:	c5 ed ef 0c 01       	vpxor  ymm1,ymm2,YMMWORD PTR [rcx+rax*1]
  401b42:	c5 fe 6f 54 06 20    	vmovdqu ymm2,YMMWORD PTR [rsi+rax*1+0x20]
  401b48:	c5 ed ef 44 01 20    	vpxor  ymm0,ymm2,YMMWORD PTR [rcx+rax*1+0x20]
  401b4e:	48 83 c0 40          	add    rax,0x40
  401b52:	c5 79 6f d9          	vmovdqa xmm11,xmm1
  401b56:	c4 e3 7d 39 c9 01    	vextracti128 xmm1,ymm1,0x1
  401b5c:	c5 f9 6f d0          	vmovdqa xmm2,xmm0
  401b60:	c4 e3 7d 39 c0 01    	vextracti128 xmm0,ymm0,0x1
  401b66:	c4 e3 71 44 c9 10    	vpclmullqhqdq xmm1,xmm1,xmm1
  401b6c:	c4 e3 69 44 d2 10    	vpclmullqhqdq xmm2,xmm2,xmm2
  401b72:	c4 43 21 44 db 10    	vpclmullqhqdq xmm11,xmm11,xmm11
  401b78:	c4 e3 79 44 c0 10    	vpclmullqhqdq xmm0,xmm0,xmm0
  401b7e:	c5 a9 ef c0          	vpxor  xmm0,xmm10,xmm0
  401b82:	c4 c1 61 ef db       	vpxor  xmm3,xmm3,xmm11
  401b87:	c5 d9 ef e1          	vpxor  xmm4,xmm4,xmm1
  401b8b:	c5 d1 ef ea          	vpxor  xmm5,xmm5,xmm2
  401b8f:	c5 e1 ef cc          	vpxor  xmm1,xmm3,xmm4
  401b93:	c5 79 6f d0          	vmovdqa xmm10,xmm0
  401b97:	c5 d1 ef d0          	vpxor  xmm2,xmm5,xmm0
  401b9b:	c5 f1 ef ca          	vpxor  xmm1,xmm1,xmm2
  401b9f:	48 3d 00 10 00 00    	cmp    rax,0x1000
  401ba5:	75 91                	jne    401b38 <chainhash_x86_avx2+0x78>
  401ba7:	c4 c1 71 ef c9       	vpxor  xmm1,xmm1,xmm9
  401bac:	48 81 c6 00 10 00 00 	add    rsi,0x1000
  401bb3:	c4 e3 71 44 f6 01    	vpclmulhqlqdq xmm6,xmm1,xmm6
  401bb9:	c4 e3 49 44 c7 01    	vpclmulhqlqdq xmm0,xmm6,xmm7
  401bbf:	c5 c9 ef f1          	vpxor  xmm6,xmm6,xmm1
  401bc3:	c5 e9 73 d8 08       	vpsrldq xmm2,xmm0,0x8
  401bc8:	c4 e2 39 00 d2       	vpshufb xmm2,xmm8,xmm2
  401bcd:	c5 f9 ef c2          	vpxor  xmm0,xmm0,xmm2
  401bd1:	c5 f9 ef f6          	vpxor  xmm6,xmm0,xmm6
  401bd5:	48 39 d6             	cmp    rsi,rdx
  401bd8:	0f 85 42 ff ff ff    	jne    401b20 <chainhash_x86_avx2+0x60>
  401bde:	49 f7 d8             	neg    r8
  401be1:	49 c1 e0 0c          	shl    r8,0xc
  401be5:	4e 8d 8c 07 00 f0 ff 	lea    r9,[rdi+r8*1-0x1000]
  401bec:	ff 
  401bed:	49 83 f9 3f          	cmp    r9,0x3f
  401bf1:	0f 86 12 02 00 00    	jbe    401e09 <chainhash_x86_avx2+0x349>
  401bf7:	4d 8d 41 c0          	lea    r8,[r9-0x40]
  401bfb:	c4 41 31 ef c9       	vpxor  xmm9,xmm9,xmm9
  401c00:	31 c0                	xor    eax,eax
  401c02:	c5 79 7f cc          	vmovdqa xmm4,xmm9
  401c06:	c5 79 7f cd          	vmovdqa xmm5,xmm9
  401c0a:	c5 79 7f cb          	vmovdqa xmm3,xmm9
  401c0e:	49 83 e0 c0          	and    r8,0xffffffffffffffc0
  401c12:	4d 8d 50 40          	lea    r10,[r8+0x40]
  401c16:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
  401c1d:	00 00 00 
  401c20:	c5 fe 6f 14 02       	vmovdqu ymm2,YMMWORD PTR [rdx+rax*1]
  401c25:	c5 ed ef 0c 01       	vpxor  ymm1,ymm2,YMMWORD PTR [rcx+rax*1]
  401c2a:	48 89 c6             	mov    rsi,rax
  401c2d:	c5 fe 6f 54 02 20    	vmovdqu ymm2,YMMWORD PTR [rdx+rax*1+0x20]
  401c33:	c5 ed ef 44 01 20    	vpxor  ymm0,ymm2,YMMWORD PTR [rcx+rax*1+0x20]
  401c39:	48 83 c0 40          	add    rax,0x40
  401c3d:	c5 79 6f d1          	vmovdqa xmm10,xmm1
  401c41:	c4 e3 7d 39 c9 01    	vextracti128 xmm1,ymm1,0x1
  401c47:	c5 f9 6f d0          	vmovdqa xmm2,xmm0
  401c4b:	c4 e3 7d 39 c0 01    	vextracti128 xmm0,ymm0,0x1
  401c51:	c4 43 29 44 d2 10    	vpclmullqhqdq xmm10,xmm10,xmm10
  401c57:	c4 e3 71 44 c9 10    	vpclmullqhqdq xmm1,xmm1,xmm1
  401c5d:	c4 e3 69 44 d2 10    	vpclmullqhqdq xmm2,xmm2,xmm2
  401c63:	c4 e3 79 44 c0 10    	vpclmullqhqdq xmm0,xmm0,xmm0
  401c69:	c5 d1 ef c9          	vpxor  xmm1,xmm5,xmm1
  401c6d:	c5 b1 ef c0          	vpxor  xmm0,xmm9,xmm0
  401c71:	c4 c1 61 ef da       	vpxor  xmm3,xmm3,xmm10
  401c76:	c5 d9 ef e2          	vpxor  xmm4,xmm4,xmm2
  401c7a:	c5 61 ef d1          	vpxor  xmm10,xmm3,xmm1
  401c7e:	c5 f9 6f e9          	vmovdqa xmm5,xmm1
  401c82:	c5 79 6f c8          	vmovdqa xmm9,xmm0
  401c86:	c5 d9 ef c8          	vpxor  xmm1,xmm4,xmm0
  401c8a:	4c 39 c6             	cmp    rsi,r8
  401c8d:	75 91                	jne    401c20 <chainhash_x86_avx2+0x160>
  401c8f:	41 83 e1 3f          	and    r9d,0x3f
  401c93:	c5 29 ef d1          	vpxor  xmm10,xmm10,xmm1
  401c97:	4d 85 c9             	test   r9,r9
  401c9a:	0f 85 f5 00 00 00    	jne    401d95 <chainhash_x86_avx2+0x2d5>
  401ca0:	48 8b 81 08 10 00 00 	mov    rax,QWORD PTR [rcx+0x1008]
  401ca7:	c4 e1 f9 6e ef       	vmovq  xmm5,rdi
  401cac:	48 31 f8             	xor    rax,rdi
  401caf:	c4 e3 d1 22 c8 01    	vpinsrq xmm1,xmm5,rax,0x1
  401cb5:	48 8b 81 18 10 00 00 	mov    rax,QWORD PTR [rcx+0x1018]
  401cbc:	c4 c1 71 ef ca       	vpxor  xmm1,xmm1,xmm10
  401cc1:	c4 e3 71 44 f6 01    	vpclmulhqlqdq xmm6,xmm1,xmm6
  401cc7:	c4 e1 f9 6e e0       	vmovq  xmm4,rax
  401ccc:	48 33 81 20 10 00 00 	xor    rax,QWORD PTR [rcx+0x1020]
  401cd3:	c4 e3 49 44 d7 01    	vpclmulhqlqdq xmm2,xmm6,xmm7
  401cd9:	c5 f9 73 da 08       	vpsrldq xmm0,xmm2,0x8
  401cde:	c5 e9 ef d1          	vpxor  xmm2,xmm2,xmm1
  401ce2:	c5 fa 7e 89 40 10 00 	vmovq  xmm1,QWORD PTR [rcx+0x1040]
  401ce9:	00 
  401cea:	c4 e2 39 00 c0       	vpshufb xmm0,xmm8,xmm0
  401cef:	c5 c9 ef c0          	vpxor  xmm0,xmm6,xmm0
  401cf3:	c5 f9 ef c2          	vpxor  xmm0,xmm0,xmm2
  401cf7:	c5 f9 d4 c1          	vpaddq xmm0,xmm0,xmm1
  401cfb:	c4 e3 79 44 c8 00    	vpclmullqlqdq xmm1,xmm0,xmm0
  401d01:	c4 e3 71 44 d7 01    	vpclmulhqlqdq xmm2,xmm1,xmm7
  401d07:	c5 f1 ef cc          	vpxor  xmm1,xmm1,xmm4
  401d0b:	c5 e1 73 da 08       	vpsrldq xmm3,xmm2,0x8
  401d10:	c4 e2 39 00 db       	vpshufb xmm3,xmm8,xmm3
  401d15:	c5 e9 ef d3          	vpxor  xmm2,xmm2,xmm3
  401d19:	c5 f1 ef ca          	vpxor  xmm1,xmm1,xmm2
  401d1d:	c4 e1 f9 6e d0       	vmovq  xmm2,rax
  401d22:	c5 e9 ef d1          	vpxor  xmm2,xmm2,xmm1
  401d26:	c5 e9 ef d0          	vpxor  xmm2,xmm2,xmm0
  401d2a:	c4 e3 71 44 ca 00    	vpclmullqlqdq xmm1,xmm1,xmm2
  401d30:	c5 fa 7e 91 28 10 00 	vmovq  xmm2,QWORD PTR [rcx+0x1028]
  401d37:	00 
  401d38:	c4 e3 71 44 df 01    	vpclmulhqlqdq xmm3,xmm1,xmm7
  401d3e:	c5 e9 ef c0          	vpxor  xmm0,xmm2,xmm0
  401d42:	c5 d9 73 db 08       	vpsrldq xmm4,xmm3,0x8
  401d47:	c5 fa 7e 91 30 10 00 	vmovq  xmm2,QWORD PTR [rcx+0x1030]
  401d4e:	00 
  401d4f:	c4 e2 39 00 e4       	vpshufb xmm4,xmm8,xmm4
  401d54:	c5 e9 ef c9          	vpxor  xmm1,xmm2,xmm1
  401d58:	c5 e1 ef dc          	vpxor  xmm3,xmm3,xmm4
  401d5c:	c5 f1 ef cb          	vpxor  xmm1,xmm1,xmm3
  401d60:	c4 e3 79 44 c9 00    	vpclmullqlqdq xmm1,xmm0,xmm1
  401d66:	c4 e3 71 44 ff 01    	vpclmulhqlqdq xmm7,xmm1,xmm7
  401d6c:	c5 f9 73 df 08       	vpsrldq xmm0,xmm7,0x8
  401d71:	c4 62 39 00 c0       	vpshufb xmm8,xmm8,xmm0
  401d76:	c5 fa 7e 81 38 10 00 	vmovq  xmm0,QWORD PTR [rcx+0x1038]
  401d7d:	00 
  401d7e:	c4 c1 41 ef f8       	vpxor  xmm7,xmm7,xmm8
  401d83:	c5 f9 ef c1          	vpxor  xmm0,xmm0,xmm1
  401d87:	c5 f9 ef c7          	vpxor  xmm0,xmm0,xmm7
  401d8b:	c4 e1 f9 7e c0       	vmovq  rax,xmm0
  401d90:	c5 f8 77             	vzeroupper 
  401d93:	c9                   	leave  
  401d94:	c3                   	ret    
  401d95:	4c 01 d2             	add    rdx,r10
  401d98:	49 01 ca             	add    r10,rcx
  401d9b:	49 83 f9 0f          	cmp    r9,0xf
  401d9f:	0f 86 22 01 00 00    	jbe    401ec7 <chainhash_x86_avx2+0x407>
  401da5:	c4 c1 7a 6f 2a       	vmovdqu xmm5,XMMWORD PTR [r10]
  401daa:	c5 d1 ef 02          	vpxor  xmm0,xmm5,XMMWORD PTR [rdx]
  401dae:	49 8d 41 f0          	lea    rax,[r9-0x10]
  401db2:	c4 e3 79 44 c0 10    	vpclmullqhqdq xmm0,xmm0,xmm0
  401db8:	48 83 f8 0f          	cmp    rax,0xf
  401dbc:	76 34                	jbe    401df2 <chainhash_x86_avx2+0x332>
  401dbe:	c4 c1 7a 6f 6a 10    	vmovdqu xmm5,XMMWORD PTR [r10+0x10]
  401dc4:	c5 d1 ef 4a 10       	vpxor  xmm1,xmm5,XMMWORD PTR [rdx+0x10]
  401dc9:	49 8d 71 e0          	lea    rsi,[r9-0x20]
  401dcd:	c4 e3 71 44 c9 10    	vpclmullqhqdq xmm1,xmm1,xmm1
  401dd3:	c5 f9 ef c1          	vpxor  xmm0,xmm0,xmm1
  401dd7:	48 83 fe 0f          	cmp    rsi,0xf
  401ddb:	76 15                	jbe    401df2 <chainhash_x86_avx2+0x332>
  401ddd:	c5 fa 6f 6a 20       	vmovdqu xmm5,XMMWORD PTR [rdx+0x20]
  401de2:	c4 c1 51 ef 4a 20    	vpxor  xmm1,xmm5,XMMWORD PTR [r10+0x20]
  401de8:	c4 e3 71 44 c9 10    	vpclmullqhqdq xmm1,xmm1,xmm1
  401dee:	c5 f9 ef c1          	vpxor  xmm0,xmm0,xmm1
  401df2:	48 83 e0 f0          	and    rax,0xfffffffffffffff0
  401df6:	48 83 c0 10          	add    rax,0x10
  401dfa:	41 83 e1 0f          	and    r9d,0xf
  401dfe:	75 31                	jne    401e31 <chainhash_x86_avx2+0x371>
  401e00:	c5 29 ef d0          	vpxor  xmm10,xmm10,xmm0
  401e04:	e9 97 fe ff ff       	jmp    401ca0 <chainhash_x86_avx2+0x1e0>
  401e09:	c4 41 29 ef d2       	vpxor  xmm10,xmm10,xmm10
  401e0e:	45 31 d2             	xor    r10d,r10d
  401e11:	e9 81 fe ff ff       	jmp    401c97 <chainhash_x86_avx2+0x1d7>
  401e16:	49 89 d1             	mov    r9,rdx
  401e19:	c5 f9 6f 3d 2f 12 00 	vmovdqa xmm7,XMMWORD PTR [rip+0x122f]        # 403050 <__dso_handle+0x48>
  401e20:	00 
  401e21:	48 89 f2             	mov    rdx,rsi
  401e24:	c5 79 6f 05 34 12 00 	vmovdqa xmm8,XMMWORD PTR [rip+0x1234]        # 403060 <__dso_handle+0x58>
  401e2b:	00 
  401e2c:	e9 bc fd ff ff       	jmp    401bed <chainhash_x86_avx2+0x12d>
  401e31:	48 01 c2             	add    rdx,rax
  401e34:	49 01 c2             	add    r10,rax
  401e37:	c5 f1 ef c9          	vpxor  xmm1,xmm1,xmm1
  401e3b:	45 89 c8             	mov    r8d,r9d
  401e3e:	48 8d 74 24 f0       	lea    rsi,[rsp-0x10]
  401e43:	48 89 d0             	mov    rax,rdx
  401e46:	c5 f9 7f 4c 24 f0    	vmovdqa XMMWORD PTR [rsp-0x10],xmm1
  401e4c:	41 83 f9 08          	cmp    r9d,0x8
  401e50:	73 4f                	jae    401ea1 <chainhash_x86_avx2+0x3e1>
  401e52:	31 d2                	xor    edx,edx
  401e54:	41 f6 c0 04          	test   r8b,0x4
  401e58:	75 3c                	jne    401e96 <chainhash_x86_avx2+0x3d6>
  401e5a:	41 f6 c0 02          	test   r8b,0x2
  401e5e:	75 26                	jne    401e86 <chainhash_x86_avx2+0x3c6>
  401e60:	41 83 e0 01          	and    r8d,0x1
  401e64:	75 17                	jne    401e7d <chainhash_x86_avx2+0x3bd>
  401e66:	c4 c1 7a 6f 0a       	vmovdqu xmm1,XMMWORD PTR [r10]
  401e6b:	c5 f1 ef 4c 24 f0    	vpxor  xmm1,xmm1,XMMWORD PTR [rsp-0x10]
  401e71:	c4 e3 71 44 c9 10    	vpclmullqhqdq xmm1,xmm1,xmm1
  401e77:	c5 f9 ef c1          	vpxor  xmm0,xmm0,xmm1
  401e7b:	eb 83                	jmp    401e00 <chainhash_x86_avx2+0x340>
  401e7d:	0f b6 04 10          	movzx  eax,BYTE PTR [rax+rdx*1]
  401e81:	88 04 16             	mov    BYTE PTR [rsi+rdx*1],al
  401e84:	eb e0                	jmp    401e66 <chainhash_x86_avx2+0x3a6>
  401e86:	44 0f b7 0c 10       	movzx  r9d,WORD PTR [rax+rdx*1]
  401e8b:	66 44 89 0c 16       	mov    WORD PTR [rsi+rdx*1],r9w
  401e90:	48 83 c2 02          	add    rdx,0x2
  401e94:	eb ca                	jmp    401e60 <chainhash_x86_avx2+0x3a0>
  401e96:	8b 10                	mov    edx,DWORD PTR [rax]
  401e98:	89 16                	mov    DWORD PTR [rsi],edx
  401e9a:	ba 04 00 00 00       	mov    edx,0x4
  401e9f:	eb b9                	jmp    401e5a <chainhash_x86_avx2+0x39a>
  401ea1:	41 83 e1 f8          	and    r9d,0xfffffff8
  401ea5:	31 c0                	xor    eax,eax
  401ea7:	89 c6                	mov    esi,eax
  401ea9:	83 c0 08             	add    eax,0x8
  401eac:	4c 8b 1c 32          	mov    r11,QWORD PTR [rdx+rsi*1]
  401eb0:	4c 89 5c 34 f0       	mov    QWORD PTR [rsp+rsi*1-0x10],r11
  401eb5:	44 39 c8             	cmp    eax,r9d
  401eb8:	72 ed                	jb     401ea7 <chainhash_x86_avx2+0x3e7>
  401eba:	48 8d 74 24 f0       	lea    rsi,[rsp-0x10]
  401ebf:	48 01 c6             	add    rsi,rax
  401ec2:	48 01 d0             	add    rax,rdx
  401ec5:	eb 8b                	jmp    401e52 <chainhash_x86_avx2+0x392>
  401ec7:	c5 f9 ef c0          	vpxor  xmm0,xmm0,xmm0
  401ecb:	e9 67 ff ff ff       	jmp    401e37 <chainhash_x86_avx2+0x377>

0000000000401ed0 <hash>:
  401ed0:	41 57                	push   r15
  401ed2:	41 56                	push   r14
  401ed4:	49 89 f6             	mov    r14,rsi
  401ed7:	41 55                	push   r13
  401ed9:	49 89 fd             	mov    r13,rdi
  401edc:	41 54                	push   r12
  401ede:	55                   	push   rbp
  401edf:	53                   	push   rbx
  401ee0:	48 89 54 24 c8       	mov    QWORD PTR [rsp-0x38],rdx
  401ee5:	8b 35 6d 31 00 00    	mov    esi,DWORD PTR [rip+0x316d]        # 405058 <cached.0>
  401eeb:	85 f6                	test   esi,esi
  401eed:	0f 84 97 04 00 00    	je     40238a <hash+0x4ba>
  401ef3:	8d 46 ff             	lea    eax,[rsi-0x1]
  401ef6:	83 fe 03             	cmp    esi,0x3
  401ef9:	0f 84 2b 05 00 00    	je     40242a <hash+0x55a>
  401eff:	83 f8 01             	cmp    eax,0x1
  401f02:	0f 84 dd 04 00 00    	je     4023e5 <hash+0x515>
  401f08:	49 8b 85 10 10 00 00 	mov    rax,QWORD PTR [r13+0x1010]
  401f0f:	48 8b 6c 24 c8       	mov    rbp,QWORD PTR [rsp-0x38]
  401f14:	4c 89 6c 24 d0       	mov    QWORD PTR [rsp-0x30],r13
  401f19:	48 89 44 24 d8       	mov    QWORD PTR [rsp-0x28],rax
  401f1e:	49 8b 85 00 10 00 00 	mov    rax,QWORD PTR [r13+0x1000]
  401f25:	48 89 44 24 e0       	mov    QWORD PTR [rsp-0x20],rax
  401f2a:	49 8b 85 08 10 00 00 	mov    rax,QWORD PTR [r13+0x1008]
  401f31:	48 89 44 24 e8       	mov    QWORD PTR [rsp-0x18],rax
  401f36:	b8 00 10 00 00       	mov    eax,0x1000
  401f3b:	48 39 c5             	cmp    rbp,rax
  401f3e:	48 0f 46 c5          	cmovbe rax,rbp
  401f42:	48 89 44 24 c0       	mov    QWORD PTR [rsp-0x40],rax
  401f47:	48 89 c3             	mov    rbx,rax
  401f4a:	48 83 fd 0f          	cmp    rbp,0xf
  401f4e:	0f 86 43 03 00 00    	jbe    402297 <hash+0x3c7>
  401f54:	48 83 eb 10          	sub    rbx,0x10
  401f58:	4c 8b 5c 24 d0       	mov    r11,QWORD PTR [rsp-0x30]
  401f5d:	4c 89 f0             	mov    rax,r14
  401f60:	45 31 c0             	xor    r8d,r8d
  401f63:	48 89 5c 24 f0       	mov    QWORD PTR [rsp-0x10],rbx
  401f68:	48 83 e3 f0          	and    rbx,0xfffffffffffffff0
  401f6c:	45 31 e4             	xor    r12d,r12d
  401f6f:	4d 8d 6c 1e 10       	lea    r13,[r14+rbx*1+0x10]
  401f74:	eb 3f                	jmp    401fb5 <hash+0xe5>
  401f76:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
  401f7d:	00 00 00 
  401f80:	b9 41 00 00 00       	mov    ecx,0x41
  401f85:	49 89 f7             	mov    r15,rsi
  401f88:	44 29 d1             	sub    ecx,r10d
  401f8b:	49 d3 ef             	shr    r15,cl
  401f8e:	4c 21 fa             	and    rdx,r15
  401f91:	48 31 d3             	xor    rbx,rdx
  401f94:	41 83 fa 40          	cmp    r10d,0x40
  401f98:	0f 85 ea 00 00 00    	jne    402088 <hash+0x1b8>
  401f9e:	48 83 c0 10          	add    rax,0x10
  401fa2:	4d 31 cc             	xor    r12,r9
  401fa5:	49 31 d8             	xor    r8,rbx
  401fa8:	49 83 c3 10          	add    r11,0x10
  401fac:	49 39 c5             	cmp    r13,rax
  401faf:	0f 84 d8 00 00 00    	je     40208d <hash+0x1bd>
  401fb5:	0f b6 50 09          	movzx  edx,BYTE PTR [rax+0x9]
  401fb9:	0f b6 48 0a          	movzx  ecx,BYTE PTR [rax+0xa]
  401fbd:	31 db                	xor    ebx,ebx
  401fbf:	45 31 c9             	xor    r9d,r9d
  401fc2:	0f b6 78 0f          	movzx  edi,BYTE PTR [rax+0xf]
  401fc6:	0f b6 70 07          	movzx  esi,BYTE PTR [rax+0x7]
  401fca:	48 c1 e1 10          	shl    rcx,0x10
  401fce:	48 c1 e2 08          	shl    rdx,0x8
  401fd2:	48 c1 e7 38          	shl    rdi,0x38
  401fd6:	48 09 ca             	or     rdx,rcx
  401fd9:	0f b6 48 08          	movzx  ecx,BYTE PTR [rax+0x8]
  401fdd:	48 c1 e6 38          	shl    rsi,0x38
  401fe1:	48 09 ca             	or     rdx,rcx
  401fe4:	0f b6 48 0b          	movzx  ecx,BYTE PTR [rax+0xb]
  401fe8:	48 c1 e1 18          	shl    rcx,0x18
  401fec:	48 09 d1             	or     rcx,rdx
  401fef:	0f b6 50 0c          	movzx  edx,BYTE PTR [rax+0xc]
  401ff3:	48 c1 e2 20          	shl    rdx,0x20
  401ff7:	48 09 ca             	or     rdx,rcx
  401ffa:	0f b6 48 0d          	movzx  ecx,BYTE PTR [rax+0xd]
  401ffe:	48 c1 e1 28          	shl    rcx,0x28
  402002:	48 09 d1             	or     rcx,rdx
  402005:	0f b6 50 0e          	movzx  edx,BYTE PTR [rax+0xe]
  402009:	48 c1 e2 30          	shl    rdx,0x30
  40200d:	48 09 ca             	or     rdx,rcx
  402010:	0f b6 48 02          	movzx  ecx,BYTE PTR [rax+0x2]
  402014:	48 09 d7             	or     rdi,rdx
  402017:	0f b6 50 01          	movzx  edx,BYTE PTR [rax+0x1]
  40201b:	49 33 7b 08          	xor    rdi,QWORD PTR [r11+0x8]
  40201f:	48 c1 e1 10          	shl    rcx,0x10
  402023:	48 c1 e2 08          	shl    rdx,0x8
  402027:	48 09 ca             	or     rdx,rcx
  40202a:	0f b6 08             	movzx  ecx,BYTE PTR [rax]
  40202d:	48 09 ca             	or     rdx,rcx
  402030:	0f b6 48 03          	movzx  ecx,BYTE PTR [rax+0x3]
  402034:	48 c1 e1 18          	shl    rcx,0x18
  402038:	48 09 d1             	or     rcx,rdx
  40203b:	0f b6 50 04          	movzx  edx,BYTE PTR [rax+0x4]
  40203f:	48 c1 e2 20          	shl    rdx,0x20
  402043:	48 09 ca             	or     rdx,rcx
  402046:	0f b6 48 05          	movzx  ecx,BYTE PTR [rax+0x5]
  40204a:	48 c1 e1 28          	shl    rcx,0x28
  40204e:	48 09 d1             	or     rcx,rdx
  402051:	0f b6 50 06          	movzx  edx,BYTE PTR [rax+0x6]
  402055:	48 c1 e2 30          	shl    rdx,0x30
  402059:	48 09 ca             	or     rdx,rcx
  40205c:	48 09 d6             	or     rsi,rdx
  40205f:	49 33 33             	xor    rsi,QWORD PTR [r11]
  402062:	31 c9                	xor    ecx,ecx
  402064:	48 89 fa             	mov    rdx,rdi
  402067:	49 89 f2             	mov    r10,rsi
  40206a:	48 d3 ea             	shr    rdx,cl
  40206d:	49 d3 e2             	shl    r10,cl
  402070:	83 e2 01             	and    edx,0x1
  402073:	48 f7 da             	neg    rdx
  402076:	49 21 d2             	and    r10,rdx
  402079:	4d 31 d1             	xor    r9,r10
  40207c:	44 8d 51 01          	lea    r10d,[rcx+0x1]
  402080:	85 c9                	test   ecx,ecx
  402082:	0f 85 f8 fe ff ff    	jne    401f80 <hash+0xb0>
  402088:	44 89 d1             	mov    ecx,r10d
  40208b:	eb d7                	jmp    402064 <hash+0x194>
  40208d:	48 8b 44 24 f0       	mov    rax,QWORD PTR [rsp-0x10]
  402092:	48 8b 54 24 c0       	mov    rdx,QWORD PTR [rsp-0x40]
  402097:	48 83 e0 f0          	and    rax,0xfffffffffffffff0
  40209b:	83 e2 0f             	and    edx,0xf
  40209e:	48 83 c0 10          	add    rax,0x10
  4020a2:	48 85 d2             	test   rdx,rdx
  4020a5:	0f 84 76 01 00 00    	je     402221 <hash+0x351>
  4020ab:	41 0f b6 3c 06       	movzx  edi,BYTE PTR [r14+rax*1]
  4020b0:	48 83 fa 01          	cmp    rdx,0x1
  4020b4:	74 7f                	je     402135 <hash+0x265>
  4020b6:	41 0f b6 4c 06 01    	movzx  ecx,BYTE PTR [r14+rax*1+0x1]
  4020bc:	48 c1 e1 08          	shl    rcx,0x8
  4020c0:	48 09 cf             	or     rdi,rcx
  4020c3:	48 83 fa 02          	cmp    rdx,0x2
  4020c7:	74 6c                	je     402135 <hash+0x265>
  4020c9:	41 0f b6 4c 06 02    	movzx  ecx,BYTE PTR [r14+rax*1+0x2]
  4020cf:	48 c1 e1 10          	shl    rcx,0x10
  4020d3:	48 09 cf             	or     rdi,rcx
  4020d6:	48 83 fa 03          	cmp    rdx,0x3
  4020da:	74 59                	je     402135 <hash+0x265>
  4020dc:	41 0f b6 4c 06 03    	movzx  ecx,BYTE PTR [r14+rax*1+0x3]
  4020e2:	48 c1 e1 18          	shl    rcx,0x18
  4020e6:	48 09 cf             	or     rdi,rcx
  4020e9:	48 83 fa 04          	cmp    rdx,0x4
  4020ed:	74 46                	je     402135 <hash+0x265>
  4020ef:	41 0f b6 4c 06 04    	movzx  ecx,BYTE PTR [r14+rax*1+0x4]
  4020f5:	48 c1 e1 20          	shl    rcx,0x20
  4020f9:	48 09 cf             	or     rdi,rcx
  4020fc:	48 83 fa 05          	cmp    rdx,0x5
  402100:	74 33                	je     402135 <hash+0x265>
  402102:	41 0f b6 4c 06 05    	movzx  ecx,BYTE PTR [r14+rax*1+0x5]
  402108:	48 c1 e1 28          	shl    rcx,0x28
  40210c:	48 09 cf             	or     rdi,rcx
  40210f:	48 83 fa 06          	cmp    rdx,0x6
  402113:	74 20                	je     402135 <hash+0x265>
  402115:	41 0f b6 4c 06 06    	movzx  ecx,BYTE PTR [r14+rax*1+0x6]
  40211b:	48 c1 e1 30          	shl    rcx,0x30
  40211f:	48 09 cf             	or     rdi,rcx
  402122:	48 83 fa 07          	cmp    rdx,0x7
  402126:	76 0d                	jbe    402135 <hash+0x265>
  402128:	41 0f b6 4c 06 07    	movzx  ecx,BYTE PTR [r14+rax*1+0x7]
  40212e:	48 c1 e1 38          	shl    rcx,0x38
  402132:	48 09 cf             	or     rdi,rcx
  402135:	48 8b 5c 24 d0       	mov    rbx,QWORD PTR [rsp-0x30]
  40213a:	48 8d 70 08          	lea    rsi,[rax+0x8]
  40213e:	45 31 c9             	xor    r9d,r9d
  402141:	48 33 3c 03          	xor    rdi,QWORD PTR [rbx+rax*1]
  402145:	48 83 fa 08          	cmp    rdx,0x8
  402149:	76 7c                	jbe    4021c7 <hash+0x2f7>
  40214b:	45 0f b6 4c 06 08    	movzx  r9d,BYTE PTR [r14+rax*1+0x8]
  402151:	48 8d 4a f8          	lea    rcx,[rdx-0x8]
  402155:	48 83 fa 09          	cmp    rdx,0x9
  402159:	74 6c                	je     4021c7 <hash+0x2f7>
  40215b:	41 0f b6 54 06 09    	movzx  edx,BYTE PTR [r14+rax*1+0x9]
  402161:	48 c1 e2 08          	shl    rdx,0x8
  402165:	49 09 d1             	or     r9,rdx
  402168:	48 83 f9 02          	cmp    rcx,0x2
  40216c:	74 59                	je     4021c7 <hash+0x2f7>
  40216e:	41 0f b6 54 06 0a    	movzx  edx,BYTE PTR [r14+rax*1+0xa]
  402174:	48 c1 e2 10          	shl    rdx,0x10
  402178:	49 09 d1             	or     r9,rdx
  40217b:	48 83 f9 03          	cmp    rcx,0x3
  40217f:	74 46                	je     4021c7 <hash+0x2f7>
  402181:	41 0f b6 54 06 0b    	movzx  edx,BYTE PTR [r14+rax*1+0xb]
  402187:	48 c1 e2 18          	shl    rdx,0x18
  40218b:	49 09 d1             	or     r9,rdx
  40218e:	48 83 f9 04          	cmp    rcx,0x4
  402192:	74 33                	je     4021c7 <hash+0x2f7>
  402194:	41 0f b6 54 06 0c    	movzx  edx,BYTE PTR [r14+rax*1+0xc]
  40219a:	48 c1 e2 20          	shl    rdx,0x20
  40219e:	49 09 d1             	or     r9,rdx
  4021a1:	48 83 f9 05          	cmp    rcx,0x5
  4021a5:	74 20                	je     4021c7 <hash+0x2f7>
  4021a7:	41 0f b6 54 06 0d    	movzx  edx,BYTE PTR [r14+rax*1+0xd]
  4021ad:	48 c1 e2 28          	shl    rdx,0x28
  4021b1:	49 09 d1             	or     r9,rdx
  4021b4:	48 83 f9 07          	cmp    rcx,0x7
  4021b8:	75 0d                	jne    4021c7 <hash+0x2f7>
  4021ba:	41 0f b6 44 06 0e    	movzx  eax,BYTE PTR [r14+rax*1+0xe]
  4021c0:	48 c1 e0 30          	shl    rax,0x30
  4021c4:	49 09 c1             	or     r9,rax
  4021c7:	48 8b 44 24 d0       	mov    rax,QWORD PTR [rsp-0x30]
  4021cc:	45 31 d2             	xor    r10d,r10d
  4021cf:	31 d2                	xor    edx,edx
  4021d1:	31 c9                	xor    ecx,ecx
  4021d3:	4c 33 0c 30          	xor    r9,QWORD PTR [rax+rsi*1]
  4021d7:	66 0f 1f 84 00 00 00 	nop    WORD PTR [rax+rax*1+0x0]
  4021de:	00 00 
  4021e0:	4c 89 c8             	mov    rax,r9
  4021e3:	48 89 fe             	mov    rsi,rdi
  4021e6:	48 d3 e8             	shr    rax,cl
  4021e9:	48 d3 e6             	shl    rsi,cl
  4021ec:	83 e0 01             	and    eax,0x1
  4021ef:	48 f7 d8             	neg    rax
  4021f2:	48 21 c6             	and    rsi,rax
  4021f5:	48 31 f2             	xor    rdx,rsi
  4021f8:	8d 71 01             	lea    esi,[rcx+0x1]
  4021fb:	85 c9                	test   ecx,ecx
  4021fd:	0f 84 8d 00 00 00    	je     402290 <hash+0x3c0>
  402203:	b9 41 00 00 00       	mov    ecx,0x41
  402208:	48 89 fb             	mov    rbx,rdi
  40220b:	29 f1                	sub    ecx,esi
  40220d:	48 d3 eb             	shr    rbx,cl
  402210:	48 21 d8             	and    rax,rbx
  402213:	49 31 c2             	xor    r10,rax
  402216:	83 fe 40             	cmp    esi,0x40
  402219:	75 75                	jne    402290 <hash+0x3c0>
  40221b:	49 31 d4             	xor    r12,rdx
  40221e:	4d 31 d0             	xor    r8,r10
  402221:	48 2b 6c 24 c0       	sub    rbp,QWORD PTR [rsp-0x40]
  402226:	75 0b                	jne    402233 <hash+0x363>
  402228:	48 8b 44 24 c8       	mov    rax,QWORD PTR [rsp-0x38]
  40222d:	49 31 c4             	xor    r12,rax
  402230:	49 31 c0             	xor    r8,rax
  402233:	48 8b 4c 24 d8       	mov    rcx,QWORD PTR [rsp-0x28]
  402238:	4c 33 44 24 e8       	xor    r8,QWORD PTR [rsp-0x18]
  40223d:	ba 40 00 00 00       	mov    edx,0x40
  402242:	31 f6                	xor    esi,esi
  402244:	48 33 4c 24 e0       	xor    rcx,QWORD PTR [rsp-0x20]
  402249:	48 89 c8             	mov    rax,rcx
  40224c:	48 d1 e9             	shr    rcx,1
  40224f:	83 e0 01             	and    eax,0x1
  402252:	48 f7 d8             	neg    rax
  402255:	4c 21 c0             	and    rax,r8
  402258:	48 31 c6             	xor    rsi,rax
  40225b:	4b 8d 04 00          	lea    rax,[r8+r8*1]
  40225f:	49 c1 f8 3f          	sar    r8,0x3f
  402263:	41 83 e0 1b          	and    r8d,0x1b
  402267:	49 31 c0             	xor    r8,rax
  40226a:	83 ea 01             	sub    edx,0x1
  40226d:	75 da                	jne    402249 <hash+0x379>
  40226f:	4c 31 e6             	xor    rsi,r12
  402272:	48 89 74 24 d8       	mov    QWORD PTR [rsp-0x28],rsi
  402277:	48 85 ed             	test   rbp,rbp
  40227a:	74 2b                	je     4022a7 <hash+0x3d7>
  40227c:	4c 03 74 24 c0       	add    r14,QWORD PTR [rsp-0x40]
  402281:	e9 b0 fc ff ff       	jmp    401f36 <hash+0x66>
  402286:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
  40228d:	00 00 00 
  402290:	89 f1                	mov    ecx,esi
  402292:	e9 49 ff ff ff       	jmp    4021e0 <hash+0x310>
  402297:	48 89 c2             	mov    rdx,rax
  40229a:	45 31 c0             	xor    r8d,r8d
  40229d:	45 31 e4             	xor    r12d,r12d
  4022a0:	31 c0                	xor    eax,eax
  4022a2:	e9 fb fd ff ff       	jmp    4020a2 <hash+0x1d2>
  4022a7:	4c 8b 6c 24 d0       	mov    r13,QWORD PTR [rsp-0x30]
  4022ac:	48 89 f2             	mov    rdx,rsi
  4022af:	31 c0                	xor    eax,eax
  4022b1:	bf 40 00 00 00       	mov    edi,0x40
  4022b6:	49 03 95 40 10 00 00 	add    rdx,QWORD PTR [r13+0x1040]
  4022bd:	48 89 d1             	mov    rcx,rdx
  4022c0:	49 89 d0             	mov    r8,rdx
  4022c3:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
  4022c8:	4c 89 c6             	mov    rsi,r8
  4022cb:	49 d1 e8             	shr    r8,1
  4022ce:	83 e6 01             	and    esi,0x1
  4022d1:	48 f7 de             	neg    rsi
  4022d4:	48 21 ce             	and    rsi,rcx
  4022d7:	48 31 f0             	xor    rax,rsi
  4022da:	48 8d 34 09          	lea    rsi,[rcx+rcx*1]
  4022de:	48 c1 f9 3f          	sar    rcx,0x3f
  4022e2:	83 e1 1b             	and    ecx,0x1b
  4022e5:	48 31 f1             	xor    rcx,rsi
  4022e8:	83 ef 01             	sub    edi,0x1
  4022eb:	75 db                	jne    4022c8 <hash+0x3f8>
  4022ed:	49 8b bd 20 10 00 00 	mov    rdi,QWORD PTR [r13+0x1020]
  4022f4:	31 f6                	xor    esi,esi
  4022f6:	41 b8 40 00 00 00    	mov    r8d,0x40
  4022fc:	48 31 d7             	xor    rdi,rdx
  4022ff:	48 31 c7             	xor    rdi,rax
  402302:	49 33 85 18 10 00 00 	xor    rax,QWORD PTR [r13+0x1018]
  402309:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
  402310:	48 89 f9             	mov    rcx,rdi
  402313:	48 d1 ef             	shr    rdi,1
  402316:	83 e1 01             	and    ecx,0x1
  402319:	48 f7 d9             	neg    rcx
  40231c:	48 21 c1             	and    rcx,rax
  40231f:	48 31 ce             	xor    rsi,rcx
  402322:	48 8d 0c 00          	lea    rcx,[rax+rax*1]
  402326:	48 c1 f8 3f          	sar    rax,0x3f
  40232a:	83 e0 1b             	and    eax,0x1b
  40232d:	48 31 c8             	xor    rax,rcx
  402330:	41 83 e8 01          	sub    r8d,0x1
  402334:	75 da                	jne    402310 <hash+0x440>
  402336:	49 8b 85 28 10 00 00 	mov    rax,QWORD PTR [r13+0x1028]
  40233d:	49 33 b5 30 10 00 00 	xor    rsi,QWORD PTR [r13+0x1030]
  402344:	48 89 f1             	mov    rcx,rsi
  402347:	be 40 00 00 00       	mov    esi,0x40
  40234c:	48 31 d0             	xor    rax,rdx
  40234f:	90                   	nop
  402350:	48 89 ca             	mov    rdx,rcx
  402353:	48 d1 e9             	shr    rcx,1
  402356:	83 e2 01             	and    edx,0x1
  402359:	48 f7 da             	neg    rdx
  40235c:	48 21 c2             	and    rdx,rax
  40235f:	48 31 d5             	xor    rbp,rdx
  402362:	48 8d 14 00          	lea    rdx,[rax+rax*1]
  402366:	48 c1 f8 3f          	sar    rax,0x3f
  40236a:	83 e0 1b             	and    eax,0x1b
  40236d:	48 31 d0             	xor    rax,rdx
  402370:	83 ee 01             	sub    esi,0x1
  402373:	75 db                	jne    402350 <hash+0x480>
  402375:	49 8b 85 38 10 00 00 	mov    rax,QWORD PTR [r13+0x1038]
  40237c:	5b                   	pop    rbx
  40237d:	48 31 e8             	xor    rax,rbp
  402380:	5d                   	pop    rbp
  402381:	41 5c                	pop    r12
  402383:	41 5d                	pop    r13
  402385:	41 5e                	pop    r14
  402387:	41 5f                	pop    r15
  402389:	c3                   	ret    
  40238a:	89 f0                	mov    eax,esi
  40238c:	0f a2                	cpuid  
  40238e:	85 c0                	test   eax,eax
  402390:	74 6d                	je     4023ff <hash+0x52f>
  402392:	b8 01 00 00 00       	mov    eax,0x1
  402397:	0f a2                	cpuid  
  402399:	81 e1 02 02 00 18    	and    ecx,0x18000202
  40239f:	81 f9 02 02 00 18    	cmp    ecx,0x18000202
  4023a5:	75 58                	jne    4023ff <hash+0x52f>
  4023a7:	89 f1                	mov    ecx,esi
  4023a9:	0f 01 d0             	xgetbv 
  4023ac:	89 c7                	mov    edi,eax
  4023ae:	83 e0 06             	and    eax,0x6
  4023b1:	83 f8 06             	cmp    eax,0x6
  4023b4:	75 49                	jne    4023ff <hash+0x52f>
  4023b6:	89 f0                	mov    eax,esi
  4023b8:	0f a2                	cpuid  
  4023ba:	83 f8 06             	cmp    eax,0x6
  4023bd:	76 40                	jbe    4023ff <hash+0x52f>
  4023bf:	b8 07 00 00 00       	mov    eax,0x7
  4023c4:	89 f1                	mov    ecx,esi
  4023c6:	0f a2                	cpuid  
  4023c8:	f6 c3 20             	test   bl,0x20
  4023cb:	74 32                	je     4023ff <hash+0x52f>
  4023cd:	81 e7 e6 00 00 00    	and    edi,0xe6
  4023d3:	81 ff e6 00 00 00    	cmp    edi,0xe6
  4023d9:	74 33                	je     40240e <hash+0x53e>
  4023db:	c7 05 73 2c 00 00 02 	mov    DWORD PTR [rip+0x2c73],0x2        # 405058 <cached.0>
  4023e2:	00 00 00 
  4023e5:	48 8b 54 24 c8       	mov    rdx,QWORD PTR [rsp-0x38]
  4023ea:	4c 89 f6             	mov    rsi,r14
  4023ed:	5b                   	pop    rbx
  4023ee:	4c 89 ef             	mov    rdi,r13
  4023f1:	5d                   	pop    rbp
  4023f2:	41 5c                	pop    r12
  4023f4:	41 5d                	pop    r13
  4023f6:	41 5e                	pop    r14
  4023f8:	41 5f                	pop    r15
  4023fa:	e9 c1 f6 ff ff       	jmp    401ac0 <chainhash_x86_avx2>
  4023ff:	c7 05 4f 2c 00 00 01 	mov    DWORD PTR [rip+0x2c4f],0x1        # 405058 <cached.0>
  402406:	00 00 00 
  402409:	e9 fa fa ff ff       	jmp    401f08 <hash+0x38>
  40240e:	81 e3 00 00 01 00    	and    ebx,0x10000
  402414:	74 c5                	je     4023db <hash+0x50b>
  402416:	80 e5 04             	and    ch,0x4
  402419:	74 c0                	je     4023db <hash+0x50b>
  40241b:	c7 05 33 2c 00 00 03 	mov    DWORD PTR [rip+0x2c33],0x3        # 405058 <cached.0>
  402422:	00 00 00 
  402425:	48 8b 54 24 c8       	mov    rdx,QWORD PTR [rsp-0x38]
  40242a:	5b                   	pop    rbx
  40242b:	4c 89 f6             	mov    rsi,r14
  40242e:	5d                   	pop    rbp
  40242f:	4c 89 ef             	mov    rdi,r13
  402432:	41 5c                	pop    r12
  402434:	41 5d                	pop    r13
  402436:	41 5e                	pop    r14
  402438:	41 5f                	pop    r15
  40243a:	e9 21 f1 ff ff       	jmp    401560 <chainhash_x86_avx512>

Disassembly of section .fini:

0000000000402440 <_fini>:
  402440:	f3 0f 1e fa          	endbr64 
  402444:	48 83 ec 08          	sub    rsp,0x8
  402448:	48 83 c4 08          	add    rsp,0x8
  40244c:	c3                   	ret    
