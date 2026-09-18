
experiments/micro_ports:     file format elf64-x86-64


Disassembly of section .init:

0000000000401000 <_init>:
  401000:	f3 0f 1e fa          	endbr64 
  401004:	48 83 ec 08          	sub    rsp,0x8
  401008:	48 8b 05 e1 5f 00 00 	mov    rax,QWORD PTR [rip+0x5fe1]        # 406ff0 <__gmon_start__>
  40100f:	48 85 c0             	test   rax,rax
  401012:	74 02                	je     401016 <_init+0x16>
  401014:	ff d0                	call   rax
  401016:	48 83 c4 08          	add    rsp,0x8
  40101a:	c3                   	ret    

Disassembly of section .plt:

0000000000401020 <.plt>:
  401020:	ff 35 e2 5f 00 00    	push   QWORD PTR [rip+0x5fe2]        # 407008 <_GLOBAL_OFFSET_TABLE_+0x8>
  401026:	ff 25 e4 5f 00 00    	jmp    QWORD PTR [rip+0x5fe4]        # 407010 <_GLOBAL_OFFSET_TABLE_+0x10>
  40102c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]

0000000000401030 <puts@plt>:
  401030:	ff 25 e2 5f 00 00    	jmp    QWORD PTR [rip+0x5fe2]        # 407018 <puts@GLIBC_2.2.5>
  401036:	68 00 00 00 00       	push   0x0
  40103b:	e9 e0 ff ff ff       	jmp    401020 <.plt>

0000000000401040 <printf@plt>:
  401040:	ff 25 da 5f 00 00    	jmp    QWORD PTR [rip+0x5fda]        # 407020 <printf@GLIBC_2.2.5>
  401046:	68 01 00 00 00       	push   0x1
  40104b:	e9 d0 ff ff ff       	jmp    401020 <.plt>

0000000000401050 <fflush@plt>:
  401050:	ff 25 d2 5f 00 00    	jmp    QWORD PTR [rip+0x5fd2]        # 407028 <fflush@GLIBC_2.2.5>
  401056:	68 02 00 00 00       	push   0x2
  40105b:	e9 c0 ff ff ff       	jmp    401020 <.plt>

Disassembly of section .text:

0000000000401060 <main>:
  401060:	41 55                	push   r13
  401062:	41 54                	push   r12
  401064:	55                   	push   rbp
  401065:	53                   	push   rbx
  401066:	48 81 ec b8 00 00 00 	sub    rsp,0xb8
  40106d:	48 c7 04 24 10 50 40 	mov    QWORD PTR [rsp],0x405010
  401074:	00 
  401075:	48 c7 44 24 08 d0 12 	mov    QWORD PTR [rsp+0x8],0x4012d0
  40107c:	40 00 
  40107e:	48 c7 44 24 10 1d 50 	mov    QWORD PTR [rsp+0x10],0x40501d
  401085:	40 00 
  401087:	48 c7 44 24 18 80 16 	mov    QWORD PTR [rsp+0x18],0x401680
  40108e:	40 00 
  401090:	48 c7 44 24 20 2d 50 	mov    QWORD PTR [rsp+0x20],0x40502d
  401097:	40 00 
  401099:	48 c7 44 24 28 30 1a 	mov    QWORD PTR [rsp+0x28],0x401a30
  4010a0:	40 00 
  4010a2:	48 c7 44 24 30 39 50 	mov    QWORD PTR [rsp+0x30],0x405039
  4010a9:	40 00 
  4010ab:	48 c7 44 24 38 60 1e 	mov    QWORD PTR [rsp+0x38],0x401e60
  4010b2:	40 00 
  4010b4:	48 c7 44 24 40 45 50 	mov    QWORD PTR [rsp+0x40],0x405045
  4010bb:	40 00 
  4010bd:	48 c7 44 24 48 10 22 	mov    QWORD PTR [rsp+0x48],0x402210
  4010c4:	40 00 
  4010c6:	48 c7 44 24 50 51 50 	mov    QWORD PTR [rsp+0x50],0x405051
  4010cd:	40 00 
  4010cf:	48 c7 44 24 58 c0 25 	mov    QWORD PTR [rsp+0x58],0x4025c0
  4010d6:	40 00 
  4010d8:	48 c7 44 24 60 63 50 	mov    QWORD PTR [rsp+0x60],0x405063
  4010df:	40 00 
  4010e1:	48 c7 44 24 68 b0 29 	mov    QWORD PTR [rsp+0x68],0x4029b0
  4010e8:	40 00 
  4010ea:	48 c7 44 24 70 6f 50 	mov    QWORD PTR [rsp+0x70],0x40506f
  4010f1:	40 00 
  4010f3:	48 c7 44 24 78 e0 2d 	mov    QWORD PTR [rsp+0x78],0x402de0
  4010fa:	40 00 
  4010fc:	48 c7 84 24 80 00 00 	mov    QWORD PTR [rsp+0x80],0x40507a
  401103:	00 7a 50 40 00 
  401108:	48 c7 84 24 88 00 00 	mov    QWORD PTR [rsp+0x88],0x403210
  40110f:	00 10 32 40 00 
  401114:	48 c7 84 24 90 00 00 	mov    QWORD PTR [rsp+0x90],0x4050b0
  40111b:	00 b0 50 40 00 
  401120:	48 c7 84 24 98 00 00 	mov    QWORD PTR [rsp+0x98],0x4035c0
  401127:	00 c0 35 40 00 
  40112c:	48 c7 84 24 a0 00 00 	mov    QWORD PTR [rsp+0xa0],0x405085
  401133:	00 85 50 40 00 
  401138:	48 c7 84 24 a8 00 00 	mov    QWORD PTR [rsp+0xa8],0x403cf0
  40113f:	00 f0 3c 40 00 
  401144:	49 89 e4             	mov    r12,rsp
  401147:	4c 8d ac 24 b0 00 00 	lea    r13,[rsp+0xb0]
  40114e:	00 
  40114f:	90                   	nop
  401150:	49 8b 6c 24 08       	mov    rbp,QWORD PTR [r12+0x8]
  401155:	31 db                	xor    ebx,ebx
  401157:	ff d5                	call   rbp
  401159:	49 8b 34 24          	mov    rsi,QWORD PTR [r12]
  40115d:	bf d0 50 40 00       	mov    edi,0x4050d0
  401162:	31 c0                	xor    eax,eax
  401164:	e8 d7 fe ff ff       	call   401040 <printf@plt>
  401169:	ff c3                	inc    ebx
  40116b:	ff d5                	call   rbp
  40116d:	83 fb 01             	cmp    ebx,0x1
  401170:	74 4e                	je     4011c0 <main+0x160>
  401172:	be a3 50 40 00       	mov    esi,0x4050a3
  401177:	bf a5 50 40 00       	mov    edi,0x4050a5
  40117c:	b8 01 00 00 00       	mov    eax,0x1
  401181:	e8 ba fe ff ff       	call   401040 <printf@plt>
  401186:	83 fb 05             	cmp    ebx,0x5
  401189:	75 de                	jne    401169 <main+0x109>
  40118b:	bf ac 50 40 00       	mov    edi,0x4050ac
  401190:	e8 9b fe ff ff       	call   401030 <puts@plt>
  401195:	48 8b 3d 9c 5e 00 00 	mov    rdi,QWORD PTR [rip+0x5e9c]        # 407038 <stdout@@GLIBC_2.2.5>
  40119c:	49 83 c4 10          	add    r12,0x10
  4011a0:	e8 ab fe ff ff       	call   401050 <fflush@plt>
  4011a5:	4d 39 e5             	cmp    r13,r12
  4011a8:	75 a6                	jne    401150 <main+0xf0>
  4011aa:	48 81 c4 b8 00 00 00 	add    rsp,0xb8
  4011b1:	5b                   	pop    rbx
  4011b2:	5d                   	pop    rbp
  4011b3:	41 5c                	pop    r12
  4011b5:	31 c0                	xor    eax,eax
  4011b7:	41 5d                	pop    r13
  4011b9:	c3                   	ret    
  4011ba:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
  4011c0:	be a4 50 40 00       	mov    esi,0x4050a4
  4011c5:	bf a5 50 40 00       	mov    edi,0x4050a5
  4011ca:	b8 01 00 00 00       	mov    eax,0x1
  4011cf:	e8 6c fe ff ff       	call   401040 <printf@plt>
  4011d4:	eb 93                	jmp    401169 <main+0x109>
  4011d6:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
  4011dd:	00 00 00 

00000000004011e0 <_start>:
  4011e0:	f3 0f 1e fa          	endbr64 
  4011e4:	31 ed                	xor    ebp,ebp
  4011e6:	49 89 d1             	mov    r9,rdx
  4011e9:	5e                   	pop    rsi
  4011ea:	48 89 e2             	mov    rdx,rsp
  4011ed:	48 83 e4 f0          	and    rsp,0xfffffffffffffff0
  4011f1:	50                   	push   rax
  4011f2:	54                   	push   rsp
  4011f3:	45 31 c0             	xor    r8d,r8d
  4011f6:	31 c9                	xor    ecx,ecx
  4011f8:	48 c7 c7 60 10 40 00 	mov    rdi,0x401060
  4011ff:	ff 15 db 5d 00 00    	call   QWORD PTR [rip+0x5ddb]        # 406fe0 <__libc_start_main@GLIBC_2.34>
  401205:	f4                   	hlt    
  401206:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
  40120d:	00 00 00 

0000000000401210 <_dl_relocate_static_pie>:
  401210:	f3 0f 1e fa          	endbr64 
  401214:	c3                   	ret    
  401215:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
  40121c:	00 00 00 
  40121f:	90                   	nop

0000000000401220 <deregister_tm_clones>:
  401220:	48 8d 3d 11 5e 00 00 	lea    rdi,[rip+0x5e11]        # 407038 <stdout@@GLIBC_2.2.5>
  401227:	48 8d 05 0a 5e 00 00 	lea    rax,[rip+0x5e0a]        # 407038 <stdout@@GLIBC_2.2.5>
  40122e:	48 39 f8             	cmp    rax,rdi
  401231:	74 15                	je     401248 <deregister_tm_clones+0x28>
  401233:	48 8b 05 ae 5d 00 00 	mov    rax,QWORD PTR [rip+0x5dae]        # 406fe8 <_ITM_deregisterTMCloneTable>
  40123a:	48 85 c0             	test   rax,rax
  40123d:	74 09                	je     401248 <deregister_tm_clones+0x28>
  40123f:	ff e0                	jmp    rax
  401241:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
  401248:	c3                   	ret    
  401249:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]

0000000000401250 <register_tm_clones>:
  401250:	48 8d 3d e1 5d 00 00 	lea    rdi,[rip+0x5de1]        # 407038 <stdout@@GLIBC_2.2.5>
  401257:	48 8d 35 da 5d 00 00 	lea    rsi,[rip+0x5dda]        # 407038 <stdout@@GLIBC_2.2.5>
  40125e:	48 29 fe             	sub    rsi,rdi
  401261:	48 89 f0             	mov    rax,rsi
  401264:	48 c1 ee 3f          	shr    rsi,0x3f
  401268:	48 c1 f8 03          	sar    rax,0x3
  40126c:	48 01 c6             	add    rsi,rax
  40126f:	48 d1 fe             	sar    rsi,1
  401272:	74 14                	je     401288 <register_tm_clones+0x38>
  401274:	48 8b 05 7d 5d 00 00 	mov    rax,QWORD PTR [rip+0x5d7d]        # 406ff8 <_ITM_registerTMCloneTable>
  40127b:	48 85 c0             	test   rax,rax
  40127e:	74 08                	je     401288 <register_tm_clones+0x38>
  401280:	ff e0                	jmp    rax
  401282:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
  401288:	c3                   	ret    
  401289:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]

0000000000401290 <__do_global_dtors_aux>:
  401290:	f3 0f 1e fa          	endbr64 
  401294:	80 3d a5 5d 00 00 00 	cmp    BYTE PTR [rip+0x5da5],0x0        # 407040 <completed.0>
  40129b:	75 13                	jne    4012b0 <__do_global_dtors_aux+0x20>
  40129d:	55                   	push   rbp
  40129e:	48 89 e5             	mov    rbp,rsp
  4012a1:	e8 7a ff ff ff       	call   401220 <deregister_tm_clones>
  4012a6:	c6 05 93 5d 00 00 01 	mov    BYTE PTR [rip+0x5d93],0x1        # 407040 <completed.0>
  4012ad:	5d                   	pop    rbp
  4012ae:	c3                   	ret    
  4012af:	90                   	nop
  4012b0:	c3                   	ret    
  4012b1:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
  4012b8:	00 00 00 00 
  4012bc:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]

00000000004012c0 <frame_dummy>:
  4012c0:	f3 0f 1e fa          	endbr64 
  4012c4:	eb 8a                	jmp    401250 <register_tm_clones>
  4012c6:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
  4012cd:	00 00 00 

00000000004012d0 <mul>:
  4012d0:	62 f1 7d 48 ef c0    	vpxord zmm0,zmm0,zmm0
  4012d6:	62 f1 75 48 ef c9    	vpxord zmm1,zmm1,zmm1
  4012dc:	62 f1 6d 48 ef d2    	vpxord zmm2,zmm2,zmm2
  4012e2:	62 f1 65 48 ef db    	vpxord zmm3,zmm3,zmm3
  4012e8:	62 f1 5d 48 ef e4    	vpxord zmm4,zmm4,zmm4
  4012ee:	62 f1 55 48 ef ed    	vpxord zmm5,zmm5,zmm5
  4012f4:	62 f1 4d 48 ef f6    	vpxord zmm6,zmm6,zmm6
  4012fa:	62 f1 45 48 ef ff    	vpxord zmm7,zmm7,zmm7
  401300:	62 51 3d 48 ef c0    	vpxord zmm8,zmm8,zmm8
  401306:	62 51 35 48 ef c9    	vpxord zmm9,zmm9,zmm9
  40130c:	62 51 2d 48 ef d2    	vpxord zmm10,zmm10,zmm10
  401312:	62 51 25 48 ef db    	vpxord zmm11,zmm11,zmm11
  401318:	62 51 1d 48 ef e4    	vpxord zmm12,zmm12,zmm12
  40131e:	62 51 15 48 ef ed    	vpxord zmm13,zmm13,zmm13
  401324:	62 51 0d 48 ef f6    	vpxord zmm14,zmm14,zmm14
  40132a:	62 51 05 48 ef ff    	vpxord zmm15,zmm15,zmm15
  401330:	0f ae e8             	lfence 
  401333:	0f 31                	rdtsc  
  401335:	48 89 c1             	mov    rcx,rax
  401338:	48 c1 e2 20          	shl    rdx,0x20
  40133c:	48 09 d1             	or     rcx,rdx
  40133f:	b8 a0 86 01 00       	mov    eax,0x186a0
  401344:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
  401348:	62 f1 fd 48 f4 c0    	vpmuludq zmm0,zmm0,zmm0
  40134e:	62 f1 f5 48 f4 c9    	vpmuludq zmm1,zmm1,zmm1
  401354:	62 f1 ed 48 f4 d2    	vpmuludq zmm2,zmm2,zmm2
  40135a:	62 f1 e5 48 f4 db    	vpmuludq zmm3,zmm3,zmm3
  401360:	62 f1 dd 48 f4 e4    	vpmuludq zmm4,zmm4,zmm4
  401366:	62 f1 d5 48 f4 ed    	vpmuludq zmm5,zmm5,zmm5
  40136c:	62 f1 cd 48 f4 f6    	vpmuludq zmm6,zmm6,zmm6
  401372:	62 f1 c5 48 f4 ff    	vpmuludq zmm7,zmm7,zmm7
  401378:	62 51 bd 48 f4 c0    	vpmuludq zmm8,zmm8,zmm8
  40137e:	62 51 b5 48 f4 c9    	vpmuludq zmm9,zmm9,zmm9
  401384:	62 51 ad 48 f4 d2    	vpmuludq zmm10,zmm10,zmm10
  40138a:	62 51 a5 48 f4 db    	vpmuludq zmm11,zmm11,zmm11
  401390:	62 51 9d 48 f4 e4    	vpmuludq zmm12,zmm12,zmm12
  401396:	62 51 95 48 f4 ed    	vpmuludq zmm13,zmm13,zmm13
  40139c:	62 51 8d 48 f4 f6    	vpmuludq zmm14,zmm14,zmm14
  4013a2:	62 51 85 48 f4 ff    	vpmuludq zmm15,zmm15,zmm15
  4013a8:	62 f1 fd 48 f4 c0    	vpmuludq zmm0,zmm0,zmm0
  4013ae:	62 f1 f5 48 f4 c9    	vpmuludq zmm1,zmm1,zmm1
  4013b4:	62 f1 ed 48 f4 d2    	vpmuludq zmm2,zmm2,zmm2
  4013ba:	62 f1 e5 48 f4 db    	vpmuludq zmm3,zmm3,zmm3
  4013c0:	62 f1 dd 48 f4 e4    	vpmuludq zmm4,zmm4,zmm4
  4013c6:	62 f1 d5 48 f4 ed    	vpmuludq zmm5,zmm5,zmm5
  4013cc:	62 f1 cd 48 f4 f6    	vpmuludq zmm6,zmm6,zmm6
  4013d2:	62 f1 c5 48 f4 ff    	vpmuludq zmm7,zmm7,zmm7
  4013d8:	62 51 bd 48 f4 c0    	vpmuludq zmm8,zmm8,zmm8
  4013de:	62 51 b5 48 f4 c9    	vpmuludq zmm9,zmm9,zmm9
  4013e4:	62 51 ad 48 f4 d2    	vpmuludq zmm10,zmm10,zmm10
  4013ea:	62 51 a5 48 f4 db    	vpmuludq zmm11,zmm11,zmm11
  4013f0:	62 51 9d 48 f4 e4    	vpmuludq zmm12,zmm12,zmm12
  4013f6:	62 51 95 48 f4 ed    	vpmuludq zmm13,zmm13,zmm13
  4013fc:	62 51 8d 48 f4 f6    	vpmuludq zmm14,zmm14,zmm14
  401402:	62 51 85 48 f4 ff    	vpmuludq zmm15,zmm15,zmm15
  401408:	62 f1 fd 48 f4 c0    	vpmuludq zmm0,zmm0,zmm0
  40140e:	62 f1 f5 48 f4 c9    	vpmuludq zmm1,zmm1,zmm1
  401414:	62 f1 ed 48 f4 d2    	vpmuludq zmm2,zmm2,zmm2
  40141a:	62 f1 e5 48 f4 db    	vpmuludq zmm3,zmm3,zmm3
  401420:	62 f1 dd 48 f4 e4    	vpmuludq zmm4,zmm4,zmm4
  401426:	62 f1 d5 48 f4 ed    	vpmuludq zmm5,zmm5,zmm5
  40142c:	62 f1 cd 48 f4 f6    	vpmuludq zmm6,zmm6,zmm6
  401432:	62 f1 c5 48 f4 ff    	vpmuludq zmm7,zmm7,zmm7
  401438:	62 51 bd 48 f4 c0    	vpmuludq zmm8,zmm8,zmm8
  40143e:	62 51 b5 48 f4 c9    	vpmuludq zmm9,zmm9,zmm9
  401444:	62 51 ad 48 f4 d2    	vpmuludq zmm10,zmm10,zmm10
  40144a:	62 51 a5 48 f4 db    	vpmuludq zmm11,zmm11,zmm11
  401450:	62 51 9d 48 f4 e4    	vpmuludq zmm12,zmm12,zmm12
  401456:	62 51 95 48 f4 ed    	vpmuludq zmm13,zmm13,zmm13
  40145c:	62 51 8d 48 f4 f6    	vpmuludq zmm14,zmm14,zmm14
  401462:	62 51 85 48 f4 ff    	vpmuludq zmm15,zmm15,zmm15
  401468:	62 f1 fd 48 f4 c0    	vpmuludq zmm0,zmm0,zmm0
  40146e:	62 f1 f5 48 f4 c9    	vpmuludq zmm1,zmm1,zmm1
  401474:	62 f1 ed 48 f4 d2    	vpmuludq zmm2,zmm2,zmm2
  40147a:	62 f1 e5 48 f4 db    	vpmuludq zmm3,zmm3,zmm3
  401480:	62 f1 dd 48 f4 e4    	vpmuludq zmm4,zmm4,zmm4
  401486:	62 f1 d5 48 f4 ed    	vpmuludq zmm5,zmm5,zmm5
  40148c:	62 f1 cd 48 f4 f6    	vpmuludq zmm6,zmm6,zmm6
  401492:	62 f1 c5 48 f4 ff    	vpmuludq zmm7,zmm7,zmm7
  401498:	62 51 bd 48 f4 c0    	vpmuludq zmm8,zmm8,zmm8
  40149e:	62 51 b5 48 f4 c9    	vpmuludq zmm9,zmm9,zmm9
  4014a4:	62 51 ad 48 f4 d2    	vpmuludq zmm10,zmm10,zmm10
  4014aa:	62 51 a5 48 f4 db    	vpmuludq zmm11,zmm11,zmm11
  4014b0:	62 51 9d 48 f4 e4    	vpmuludq zmm12,zmm12,zmm12
  4014b6:	62 51 95 48 f4 ed    	vpmuludq zmm13,zmm13,zmm13
  4014bc:	62 51 8d 48 f4 f6    	vpmuludq zmm14,zmm14,zmm14
  4014c2:	62 51 85 48 f4 ff    	vpmuludq zmm15,zmm15,zmm15
  4014c8:	62 f1 fd 48 f4 c0    	vpmuludq zmm0,zmm0,zmm0
  4014ce:	62 f1 f5 48 f4 c9    	vpmuludq zmm1,zmm1,zmm1
  4014d4:	62 f1 ed 48 f4 d2    	vpmuludq zmm2,zmm2,zmm2
  4014da:	62 f1 e5 48 f4 db    	vpmuludq zmm3,zmm3,zmm3
  4014e0:	62 f1 dd 48 f4 e4    	vpmuludq zmm4,zmm4,zmm4
  4014e6:	62 f1 d5 48 f4 ed    	vpmuludq zmm5,zmm5,zmm5
  4014ec:	62 f1 cd 48 f4 f6    	vpmuludq zmm6,zmm6,zmm6
  4014f2:	62 f1 c5 48 f4 ff    	vpmuludq zmm7,zmm7,zmm7
  4014f8:	62 51 bd 48 f4 c0    	vpmuludq zmm8,zmm8,zmm8
  4014fe:	62 51 b5 48 f4 c9    	vpmuludq zmm9,zmm9,zmm9
  401504:	62 51 ad 48 f4 d2    	vpmuludq zmm10,zmm10,zmm10
  40150a:	62 51 a5 48 f4 db    	vpmuludq zmm11,zmm11,zmm11
  401510:	62 51 9d 48 f4 e4    	vpmuludq zmm12,zmm12,zmm12
  401516:	62 51 95 48 f4 ed    	vpmuludq zmm13,zmm13,zmm13
  40151c:	62 51 8d 48 f4 f6    	vpmuludq zmm14,zmm14,zmm14
  401522:	62 51 85 48 f4 ff    	vpmuludq zmm15,zmm15,zmm15
  401528:	62 f1 fd 48 f4 c0    	vpmuludq zmm0,zmm0,zmm0
  40152e:	62 f1 f5 48 f4 c9    	vpmuludq zmm1,zmm1,zmm1
  401534:	62 f1 ed 48 f4 d2    	vpmuludq zmm2,zmm2,zmm2
  40153a:	62 f1 e5 48 f4 db    	vpmuludq zmm3,zmm3,zmm3
  401540:	62 f1 dd 48 f4 e4    	vpmuludq zmm4,zmm4,zmm4
  401546:	62 f1 d5 48 f4 ed    	vpmuludq zmm5,zmm5,zmm5
  40154c:	62 f1 cd 48 f4 f6    	vpmuludq zmm6,zmm6,zmm6
  401552:	62 f1 c5 48 f4 ff    	vpmuludq zmm7,zmm7,zmm7
  401558:	62 51 bd 48 f4 c0    	vpmuludq zmm8,zmm8,zmm8
  40155e:	62 51 b5 48 f4 c9    	vpmuludq zmm9,zmm9,zmm9
  401564:	62 51 ad 48 f4 d2    	vpmuludq zmm10,zmm10,zmm10
  40156a:	62 51 a5 48 f4 db    	vpmuludq zmm11,zmm11,zmm11
  401570:	62 51 9d 48 f4 e4    	vpmuludq zmm12,zmm12,zmm12
  401576:	62 51 95 48 f4 ed    	vpmuludq zmm13,zmm13,zmm13
  40157c:	62 51 8d 48 f4 f6    	vpmuludq zmm14,zmm14,zmm14
  401582:	62 51 85 48 f4 ff    	vpmuludq zmm15,zmm15,zmm15
  401588:	62 f1 fd 48 f4 c0    	vpmuludq zmm0,zmm0,zmm0
  40158e:	62 f1 f5 48 f4 c9    	vpmuludq zmm1,zmm1,zmm1
  401594:	62 f1 ed 48 f4 d2    	vpmuludq zmm2,zmm2,zmm2
  40159a:	62 f1 e5 48 f4 db    	vpmuludq zmm3,zmm3,zmm3
  4015a0:	62 f1 dd 48 f4 e4    	vpmuludq zmm4,zmm4,zmm4
  4015a6:	62 f1 d5 48 f4 ed    	vpmuludq zmm5,zmm5,zmm5
  4015ac:	62 f1 cd 48 f4 f6    	vpmuludq zmm6,zmm6,zmm6
  4015b2:	62 f1 c5 48 f4 ff    	vpmuludq zmm7,zmm7,zmm7
  4015b8:	62 51 bd 48 f4 c0    	vpmuludq zmm8,zmm8,zmm8
  4015be:	62 51 b5 48 f4 c9    	vpmuludq zmm9,zmm9,zmm9
  4015c4:	62 51 ad 48 f4 d2    	vpmuludq zmm10,zmm10,zmm10
  4015ca:	62 51 a5 48 f4 db    	vpmuludq zmm11,zmm11,zmm11
  4015d0:	62 51 9d 48 f4 e4    	vpmuludq zmm12,zmm12,zmm12
  4015d6:	62 51 95 48 f4 ed    	vpmuludq zmm13,zmm13,zmm13
  4015dc:	62 51 8d 48 f4 f6    	vpmuludq zmm14,zmm14,zmm14
  4015e2:	62 51 85 48 f4 ff    	vpmuludq zmm15,zmm15,zmm15
  4015e8:	62 f1 fd 48 f4 c0    	vpmuludq zmm0,zmm0,zmm0
  4015ee:	62 f1 f5 48 f4 c9    	vpmuludq zmm1,zmm1,zmm1
  4015f4:	62 f1 ed 48 f4 d2    	vpmuludq zmm2,zmm2,zmm2
  4015fa:	62 f1 e5 48 f4 db    	vpmuludq zmm3,zmm3,zmm3
  401600:	62 f1 dd 48 f4 e4    	vpmuludq zmm4,zmm4,zmm4
  401606:	62 f1 d5 48 f4 ed    	vpmuludq zmm5,zmm5,zmm5
  40160c:	62 f1 cd 48 f4 f6    	vpmuludq zmm6,zmm6,zmm6
  401612:	62 f1 c5 48 f4 ff    	vpmuludq zmm7,zmm7,zmm7
  401618:	62 51 bd 48 f4 c0    	vpmuludq zmm8,zmm8,zmm8
  40161e:	62 51 b5 48 f4 c9    	vpmuludq zmm9,zmm9,zmm9
  401624:	62 51 ad 48 f4 d2    	vpmuludq zmm10,zmm10,zmm10
  40162a:	62 51 a5 48 f4 db    	vpmuludq zmm11,zmm11,zmm11
  401630:	62 51 9d 48 f4 e4    	vpmuludq zmm12,zmm12,zmm12
  401636:	62 51 95 48 f4 ed    	vpmuludq zmm13,zmm13,zmm13
  40163c:	62 51 8d 48 f4 f6    	vpmuludq zmm14,zmm14,zmm14
  401642:	62 51 85 48 f4 ff    	vpmuludq zmm15,zmm15,zmm15
  401648:	ff c8                	dec    eax
  40164a:	0f 85 f8 fc ff ff    	jne    401348 <mul+0x78>
  401650:	0f ae e8             	lfence 
  401653:	0f 31                	rdtsc  
  401655:	c5 f8 57 c0          	vxorps xmm0,xmm0,xmm0
  401659:	48 c1 e2 20          	shl    rdx,0x20
  40165d:	48 09 d0             	or     rax,rdx
  401660:	48 29 c8             	sub    rax,rcx
  401663:	62 f1 ff 08 7b c0    	vcvtusi2sd xmm0,xmm0,rax
  401669:	c5 fb 5e 05 8f 3a 00 	vdivsd xmm0,xmm0,QWORD PTR [rip+0x3a8f]        # 405100 <__dso_handle+0xf8>
  401670:	00 
  401671:	c5 f8 77             	vzeroupper 
  401674:	c3                   	ret    
  401675:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
  40167c:	00 00 00 00 

0000000000401680 <ifma>:
  401680:	62 f1 7d 48 ef c0    	vpxord zmm0,zmm0,zmm0
  401686:	62 f1 75 48 ef c9    	vpxord zmm1,zmm1,zmm1
  40168c:	62 f1 6d 48 ef d2    	vpxord zmm2,zmm2,zmm2
  401692:	62 f1 65 48 ef db    	vpxord zmm3,zmm3,zmm3
  401698:	62 f1 5d 48 ef e4    	vpxord zmm4,zmm4,zmm4
  40169e:	62 f1 55 48 ef ed    	vpxord zmm5,zmm5,zmm5
  4016a4:	62 f1 4d 48 ef f6    	vpxord zmm6,zmm6,zmm6
  4016aa:	62 f1 45 48 ef ff    	vpxord zmm7,zmm7,zmm7
  4016b0:	62 51 3d 48 ef c0    	vpxord zmm8,zmm8,zmm8
  4016b6:	62 51 35 48 ef c9    	vpxord zmm9,zmm9,zmm9
  4016bc:	62 51 2d 48 ef d2    	vpxord zmm10,zmm10,zmm10
  4016c2:	62 51 25 48 ef db    	vpxord zmm11,zmm11,zmm11
  4016c8:	62 51 1d 48 ef e4    	vpxord zmm12,zmm12,zmm12
  4016ce:	62 51 15 48 ef ed    	vpxord zmm13,zmm13,zmm13
  4016d4:	62 51 0d 48 ef f6    	vpxord zmm14,zmm14,zmm14
  4016da:	62 51 05 48 ef ff    	vpxord zmm15,zmm15,zmm15
  4016e0:	0f ae e8             	lfence 
  4016e3:	0f 31                	rdtsc  
  4016e5:	48 89 c1             	mov    rcx,rax
  4016e8:	48 c1 e2 20          	shl    rdx,0x20
  4016ec:	48 09 d1             	or     rcx,rdx
  4016ef:	b8 a0 86 01 00       	mov    eax,0x186a0
  4016f4:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
  4016f8:	62 f2 fd 48 b4 c0    	vpmadd52luq zmm0,zmm0,zmm0
  4016fe:	62 f2 f5 48 b4 c9    	vpmadd52luq zmm1,zmm1,zmm1
  401704:	62 f2 ed 48 b4 d2    	vpmadd52luq zmm2,zmm2,zmm2
  40170a:	62 f2 e5 48 b4 db    	vpmadd52luq zmm3,zmm3,zmm3
  401710:	62 f2 dd 48 b4 e4    	vpmadd52luq zmm4,zmm4,zmm4
  401716:	62 f2 d5 48 b4 ed    	vpmadd52luq zmm5,zmm5,zmm5
  40171c:	62 f2 cd 48 b4 f6    	vpmadd52luq zmm6,zmm6,zmm6
  401722:	62 f2 c5 48 b4 ff    	vpmadd52luq zmm7,zmm7,zmm7
  401728:	62 52 bd 48 b4 c0    	vpmadd52luq zmm8,zmm8,zmm8
  40172e:	62 52 b5 48 b4 c9    	vpmadd52luq zmm9,zmm9,zmm9
  401734:	62 52 ad 48 b4 d2    	vpmadd52luq zmm10,zmm10,zmm10
  40173a:	62 52 a5 48 b4 db    	vpmadd52luq zmm11,zmm11,zmm11
  401740:	62 52 9d 48 b4 e4    	vpmadd52luq zmm12,zmm12,zmm12
  401746:	62 52 95 48 b4 ed    	vpmadd52luq zmm13,zmm13,zmm13
  40174c:	62 52 8d 48 b4 f6    	vpmadd52luq zmm14,zmm14,zmm14
  401752:	62 52 85 48 b4 ff    	vpmadd52luq zmm15,zmm15,zmm15
  401758:	62 f2 fd 48 b4 c0    	vpmadd52luq zmm0,zmm0,zmm0
  40175e:	62 f2 f5 48 b4 c9    	vpmadd52luq zmm1,zmm1,zmm1
  401764:	62 f2 ed 48 b4 d2    	vpmadd52luq zmm2,zmm2,zmm2
  40176a:	62 f2 e5 48 b4 db    	vpmadd52luq zmm3,zmm3,zmm3
  401770:	62 f2 dd 48 b4 e4    	vpmadd52luq zmm4,zmm4,zmm4
  401776:	62 f2 d5 48 b4 ed    	vpmadd52luq zmm5,zmm5,zmm5
  40177c:	62 f2 cd 48 b4 f6    	vpmadd52luq zmm6,zmm6,zmm6
  401782:	62 f2 c5 48 b4 ff    	vpmadd52luq zmm7,zmm7,zmm7
  401788:	62 52 bd 48 b4 c0    	vpmadd52luq zmm8,zmm8,zmm8
  40178e:	62 52 b5 48 b4 c9    	vpmadd52luq zmm9,zmm9,zmm9
  401794:	62 52 ad 48 b4 d2    	vpmadd52luq zmm10,zmm10,zmm10
  40179a:	62 52 a5 48 b4 db    	vpmadd52luq zmm11,zmm11,zmm11
  4017a0:	62 52 9d 48 b4 e4    	vpmadd52luq zmm12,zmm12,zmm12
  4017a6:	62 52 95 48 b4 ed    	vpmadd52luq zmm13,zmm13,zmm13
  4017ac:	62 52 8d 48 b4 f6    	vpmadd52luq zmm14,zmm14,zmm14
  4017b2:	62 52 85 48 b4 ff    	vpmadd52luq zmm15,zmm15,zmm15
  4017b8:	62 f2 fd 48 b4 c0    	vpmadd52luq zmm0,zmm0,zmm0
  4017be:	62 f2 f5 48 b4 c9    	vpmadd52luq zmm1,zmm1,zmm1
  4017c4:	62 f2 ed 48 b4 d2    	vpmadd52luq zmm2,zmm2,zmm2
  4017ca:	62 f2 e5 48 b4 db    	vpmadd52luq zmm3,zmm3,zmm3
  4017d0:	62 f2 dd 48 b4 e4    	vpmadd52luq zmm4,zmm4,zmm4
  4017d6:	62 f2 d5 48 b4 ed    	vpmadd52luq zmm5,zmm5,zmm5
  4017dc:	62 f2 cd 48 b4 f6    	vpmadd52luq zmm6,zmm6,zmm6
  4017e2:	62 f2 c5 48 b4 ff    	vpmadd52luq zmm7,zmm7,zmm7
  4017e8:	62 52 bd 48 b4 c0    	vpmadd52luq zmm8,zmm8,zmm8
  4017ee:	62 52 b5 48 b4 c9    	vpmadd52luq zmm9,zmm9,zmm9
  4017f4:	62 52 ad 48 b4 d2    	vpmadd52luq zmm10,zmm10,zmm10
  4017fa:	62 52 a5 48 b4 db    	vpmadd52luq zmm11,zmm11,zmm11
  401800:	62 52 9d 48 b4 e4    	vpmadd52luq zmm12,zmm12,zmm12
  401806:	62 52 95 48 b4 ed    	vpmadd52luq zmm13,zmm13,zmm13
  40180c:	62 52 8d 48 b4 f6    	vpmadd52luq zmm14,zmm14,zmm14
  401812:	62 52 85 48 b4 ff    	vpmadd52luq zmm15,zmm15,zmm15
  401818:	62 f2 fd 48 b4 c0    	vpmadd52luq zmm0,zmm0,zmm0
  40181e:	62 f2 f5 48 b4 c9    	vpmadd52luq zmm1,zmm1,zmm1
  401824:	62 f2 ed 48 b4 d2    	vpmadd52luq zmm2,zmm2,zmm2
  40182a:	62 f2 e5 48 b4 db    	vpmadd52luq zmm3,zmm3,zmm3
  401830:	62 f2 dd 48 b4 e4    	vpmadd52luq zmm4,zmm4,zmm4
  401836:	62 f2 d5 48 b4 ed    	vpmadd52luq zmm5,zmm5,zmm5
  40183c:	62 f2 cd 48 b4 f6    	vpmadd52luq zmm6,zmm6,zmm6
  401842:	62 f2 c5 48 b4 ff    	vpmadd52luq zmm7,zmm7,zmm7
  401848:	62 52 bd 48 b4 c0    	vpmadd52luq zmm8,zmm8,zmm8
  40184e:	62 52 b5 48 b4 c9    	vpmadd52luq zmm9,zmm9,zmm9
  401854:	62 52 ad 48 b4 d2    	vpmadd52luq zmm10,zmm10,zmm10
  40185a:	62 52 a5 48 b4 db    	vpmadd52luq zmm11,zmm11,zmm11
  401860:	62 52 9d 48 b4 e4    	vpmadd52luq zmm12,zmm12,zmm12
  401866:	62 52 95 48 b4 ed    	vpmadd52luq zmm13,zmm13,zmm13
  40186c:	62 52 8d 48 b4 f6    	vpmadd52luq zmm14,zmm14,zmm14
  401872:	62 52 85 48 b4 ff    	vpmadd52luq zmm15,zmm15,zmm15
  401878:	62 f2 fd 48 b4 c0    	vpmadd52luq zmm0,zmm0,zmm0
  40187e:	62 f2 f5 48 b4 c9    	vpmadd52luq zmm1,zmm1,zmm1
  401884:	62 f2 ed 48 b4 d2    	vpmadd52luq zmm2,zmm2,zmm2
  40188a:	62 f2 e5 48 b4 db    	vpmadd52luq zmm3,zmm3,zmm3
  401890:	62 f2 dd 48 b4 e4    	vpmadd52luq zmm4,zmm4,zmm4
  401896:	62 f2 d5 48 b4 ed    	vpmadd52luq zmm5,zmm5,zmm5
  40189c:	62 f2 cd 48 b4 f6    	vpmadd52luq zmm6,zmm6,zmm6
  4018a2:	62 f2 c5 48 b4 ff    	vpmadd52luq zmm7,zmm7,zmm7
  4018a8:	62 52 bd 48 b4 c0    	vpmadd52luq zmm8,zmm8,zmm8
  4018ae:	62 52 b5 48 b4 c9    	vpmadd52luq zmm9,zmm9,zmm9
  4018b4:	62 52 ad 48 b4 d2    	vpmadd52luq zmm10,zmm10,zmm10
  4018ba:	62 52 a5 48 b4 db    	vpmadd52luq zmm11,zmm11,zmm11
  4018c0:	62 52 9d 48 b4 e4    	vpmadd52luq zmm12,zmm12,zmm12
  4018c6:	62 52 95 48 b4 ed    	vpmadd52luq zmm13,zmm13,zmm13
  4018cc:	62 52 8d 48 b4 f6    	vpmadd52luq zmm14,zmm14,zmm14
  4018d2:	62 52 85 48 b4 ff    	vpmadd52luq zmm15,zmm15,zmm15
  4018d8:	62 f2 fd 48 b4 c0    	vpmadd52luq zmm0,zmm0,zmm0
  4018de:	62 f2 f5 48 b4 c9    	vpmadd52luq zmm1,zmm1,zmm1
  4018e4:	62 f2 ed 48 b4 d2    	vpmadd52luq zmm2,zmm2,zmm2
  4018ea:	62 f2 e5 48 b4 db    	vpmadd52luq zmm3,zmm3,zmm3
  4018f0:	62 f2 dd 48 b4 e4    	vpmadd52luq zmm4,zmm4,zmm4
  4018f6:	62 f2 d5 48 b4 ed    	vpmadd52luq zmm5,zmm5,zmm5
  4018fc:	62 f2 cd 48 b4 f6    	vpmadd52luq zmm6,zmm6,zmm6
  401902:	62 f2 c5 48 b4 ff    	vpmadd52luq zmm7,zmm7,zmm7
  401908:	62 52 bd 48 b4 c0    	vpmadd52luq zmm8,zmm8,zmm8
  40190e:	62 52 b5 48 b4 c9    	vpmadd52luq zmm9,zmm9,zmm9
  401914:	62 52 ad 48 b4 d2    	vpmadd52luq zmm10,zmm10,zmm10
  40191a:	62 52 a5 48 b4 db    	vpmadd52luq zmm11,zmm11,zmm11
  401920:	62 52 9d 48 b4 e4    	vpmadd52luq zmm12,zmm12,zmm12
  401926:	62 52 95 48 b4 ed    	vpmadd52luq zmm13,zmm13,zmm13
  40192c:	62 52 8d 48 b4 f6    	vpmadd52luq zmm14,zmm14,zmm14
  401932:	62 52 85 48 b4 ff    	vpmadd52luq zmm15,zmm15,zmm15
  401938:	62 f2 fd 48 b4 c0    	vpmadd52luq zmm0,zmm0,zmm0
  40193e:	62 f2 f5 48 b4 c9    	vpmadd52luq zmm1,zmm1,zmm1
  401944:	62 f2 ed 48 b4 d2    	vpmadd52luq zmm2,zmm2,zmm2
  40194a:	62 f2 e5 48 b4 db    	vpmadd52luq zmm3,zmm3,zmm3
  401950:	62 f2 dd 48 b4 e4    	vpmadd52luq zmm4,zmm4,zmm4
  401956:	62 f2 d5 48 b4 ed    	vpmadd52luq zmm5,zmm5,zmm5
  40195c:	62 f2 cd 48 b4 f6    	vpmadd52luq zmm6,zmm6,zmm6
  401962:	62 f2 c5 48 b4 ff    	vpmadd52luq zmm7,zmm7,zmm7
  401968:	62 52 bd 48 b4 c0    	vpmadd52luq zmm8,zmm8,zmm8
  40196e:	62 52 b5 48 b4 c9    	vpmadd52luq zmm9,zmm9,zmm9
  401974:	62 52 ad 48 b4 d2    	vpmadd52luq zmm10,zmm10,zmm10
  40197a:	62 52 a5 48 b4 db    	vpmadd52luq zmm11,zmm11,zmm11
  401980:	62 52 9d 48 b4 e4    	vpmadd52luq zmm12,zmm12,zmm12
  401986:	62 52 95 48 b4 ed    	vpmadd52luq zmm13,zmm13,zmm13
  40198c:	62 52 8d 48 b4 f6    	vpmadd52luq zmm14,zmm14,zmm14
  401992:	62 52 85 48 b4 ff    	vpmadd52luq zmm15,zmm15,zmm15
  401998:	62 f2 fd 48 b4 c0    	vpmadd52luq zmm0,zmm0,zmm0
  40199e:	62 f2 f5 48 b4 c9    	vpmadd52luq zmm1,zmm1,zmm1
  4019a4:	62 f2 ed 48 b4 d2    	vpmadd52luq zmm2,zmm2,zmm2
  4019aa:	62 f2 e5 48 b4 db    	vpmadd52luq zmm3,zmm3,zmm3
  4019b0:	62 f2 dd 48 b4 e4    	vpmadd52luq zmm4,zmm4,zmm4
  4019b6:	62 f2 d5 48 b4 ed    	vpmadd52luq zmm5,zmm5,zmm5
  4019bc:	62 f2 cd 48 b4 f6    	vpmadd52luq zmm6,zmm6,zmm6
  4019c2:	62 f2 c5 48 b4 ff    	vpmadd52luq zmm7,zmm7,zmm7
  4019c8:	62 52 bd 48 b4 c0    	vpmadd52luq zmm8,zmm8,zmm8
  4019ce:	62 52 b5 48 b4 c9    	vpmadd52luq zmm9,zmm9,zmm9
  4019d4:	62 52 ad 48 b4 d2    	vpmadd52luq zmm10,zmm10,zmm10
  4019da:	62 52 a5 48 b4 db    	vpmadd52luq zmm11,zmm11,zmm11
  4019e0:	62 52 9d 48 b4 e4    	vpmadd52luq zmm12,zmm12,zmm12
  4019e6:	62 52 95 48 b4 ed    	vpmadd52luq zmm13,zmm13,zmm13
  4019ec:	62 52 8d 48 b4 f6    	vpmadd52luq zmm14,zmm14,zmm14
  4019f2:	62 52 85 48 b4 ff    	vpmadd52luq zmm15,zmm15,zmm15
  4019f8:	ff c8                	dec    eax
  4019fa:	0f 85 f8 fc ff ff    	jne    4016f8 <ifma+0x78>
  401a00:	0f ae e8             	lfence 
  401a03:	0f 31                	rdtsc  
  401a05:	c5 f8 57 c0          	vxorps xmm0,xmm0,xmm0
  401a09:	48 c1 e2 20          	shl    rdx,0x20
  401a0d:	48 09 d0             	or     rax,rdx
  401a10:	48 29 c8             	sub    rax,rcx
  401a13:	62 f1 ff 08 7b c0    	vcvtusi2sd xmm0,xmm0,rax
  401a19:	c5 fb 5e 05 df 36 00 	vdivsd xmm0,xmm0,QWORD PTR [rip+0x36df]        # 405100 <__dso_handle+0xf8>
  401a20:	00 
  401a21:	c5 f8 77             	vzeroupper 
  401a24:	c3                   	ret    
  401a25:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
  401a2c:	00 00 00 00 

0000000000401a30 <phz>:
  401a30:	62 f1 7d 48 ef c0    	vpxord zmm0,zmm0,zmm0
  401a36:	62 f1 75 48 ef c9    	vpxord zmm1,zmm1,zmm1
  401a3c:	62 f1 6d 48 ef d2    	vpxord zmm2,zmm2,zmm2
  401a42:	62 f1 65 48 ef db    	vpxord zmm3,zmm3,zmm3
  401a48:	62 f1 5d 48 ef e4    	vpxord zmm4,zmm4,zmm4
  401a4e:	62 f1 55 48 ef ed    	vpxord zmm5,zmm5,zmm5
  401a54:	62 f1 4d 48 ef f6    	vpxord zmm6,zmm6,zmm6
  401a5a:	62 f1 45 48 ef ff    	vpxord zmm7,zmm7,zmm7
  401a60:	62 51 3d 48 ef c0    	vpxord zmm8,zmm8,zmm8
  401a66:	62 51 35 48 ef c9    	vpxord zmm9,zmm9,zmm9
  401a6c:	62 51 2d 48 ef d2    	vpxord zmm10,zmm10,zmm10
  401a72:	62 51 25 48 ef db    	vpxord zmm11,zmm11,zmm11
  401a78:	62 51 1d 48 ef e4    	vpxord zmm12,zmm12,zmm12
  401a7e:	62 51 15 48 ef ed    	vpxord zmm13,zmm13,zmm13
  401a84:	62 51 0d 48 ef f6    	vpxord zmm14,zmm14,zmm14
  401a8a:	62 51 05 48 ef ff    	vpxord zmm15,zmm15,zmm15
  401a90:	0f ae e8             	lfence 
  401a93:	0f 31                	rdtsc  
  401a95:	48 89 c1             	mov    rcx,rax
  401a98:	48 c1 e2 20          	shl    rdx,0x20
  401a9c:	48 09 d1             	or     rcx,rdx
  401a9f:	b8 a0 86 01 00       	mov    eax,0x186a0
  401aa4:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
  401aa8:	62 f3 7d 48 44 c0 10 	vpclmullqhqdq zmm0,zmm0,zmm0
  401aaf:	62 f3 75 48 44 c9 10 	vpclmullqhqdq zmm1,zmm1,zmm1
  401ab6:	62 f3 6d 48 44 d2 10 	vpclmullqhqdq zmm2,zmm2,zmm2
  401abd:	62 f3 65 48 44 db 10 	vpclmullqhqdq zmm3,zmm3,zmm3
  401ac4:	62 f3 5d 48 44 e4 10 	vpclmullqhqdq zmm4,zmm4,zmm4
  401acb:	62 f3 55 48 44 ed 10 	vpclmullqhqdq zmm5,zmm5,zmm5
  401ad2:	62 f3 4d 48 44 f6 10 	vpclmullqhqdq zmm6,zmm6,zmm6
  401ad9:	62 f3 45 48 44 ff 10 	vpclmullqhqdq zmm7,zmm7,zmm7
  401ae0:	62 53 3d 48 44 c0 10 	vpclmullqhqdq zmm8,zmm8,zmm8
  401ae7:	62 53 35 48 44 c9 10 	vpclmullqhqdq zmm9,zmm9,zmm9
  401aee:	62 53 2d 48 44 d2 10 	vpclmullqhqdq zmm10,zmm10,zmm10
  401af5:	62 53 25 48 44 db 10 	vpclmullqhqdq zmm11,zmm11,zmm11
  401afc:	62 53 1d 48 44 e4 10 	vpclmullqhqdq zmm12,zmm12,zmm12
  401b03:	62 53 15 48 44 ed 10 	vpclmullqhqdq zmm13,zmm13,zmm13
  401b0a:	62 53 0d 48 44 f6 10 	vpclmullqhqdq zmm14,zmm14,zmm14
  401b11:	62 53 05 48 44 ff 10 	vpclmullqhqdq zmm15,zmm15,zmm15
  401b18:	62 f3 7d 48 44 c0 10 	vpclmullqhqdq zmm0,zmm0,zmm0
  401b1f:	62 f3 75 48 44 c9 10 	vpclmullqhqdq zmm1,zmm1,zmm1
  401b26:	62 f3 6d 48 44 d2 10 	vpclmullqhqdq zmm2,zmm2,zmm2
  401b2d:	62 f3 65 48 44 db 10 	vpclmullqhqdq zmm3,zmm3,zmm3
  401b34:	62 f3 5d 48 44 e4 10 	vpclmullqhqdq zmm4,zmm4,zmm4
  401b3b:	62 f3 55 48 44 ed 10 	vpclmullqhqdq zmm5,zmm5,zmm5
  401b42:	62 f3 4d 48 44 f6 10 	vpclmullqhqdq zmm6,zmm6,zmm6
  401b49:	62 f3 45 48 44 ff 10 	vpclmullqhqdq zmm7,zmm7,zmm7
  401b50:	62 53 3d 48 44 c0 10 	vpclmullqhqdq zmm8,zmm8,zmm8
  401b57:	62 53 35 48 44 c9 10 	vpclmullqhqdq zmm9,zmm9,zmm9
  401b5e:	62 53 2d 48 44 d2 10 	vpclmullqhqdq zmm10,zmm10,zmm10
  401b65:	62 53 25 48 44 db 10 	vpclmullqhqdq zmm11,zmm11,zmm11
  401b6c:	62 53 1d 48 44 e4 10 	vpclmullqhqdq zmm12,zmm12,zmm12
  401b73:	62 53 15 48 44 ed 10 	vpclmullqhqdq zmm13,zmm13,zmm13
  401b7a:	62 53 0d 48 44 f6 10 	vpclmullqhqdq zmm14,zmm14,zmm14
  401b81:	62 53 05 48 44 ff 10 	vpclmullqhqdq zmm15,zmm15,zmm15
  401b88:	62 f3 7d 48 44 c0 10 	vpclmullqhqdq zmm0,zmm0,zmm0
  401b8f:	62 f3 75 48 44 c9 10 	vpclmullqhqdq zmm1,zmm1,zmm1
  401b96:	62 f3 6d 48 44 d2 10 	vpclmullqhqdq zmm2,zmm2,zmm2
  401b9d:	62 f3 65 48 44 db 10 	vpclmullqhqdq zmm3,zmm3,zmm3
  401ba4:	62 f3 5d 48 44 e4 10 	vpclmullqhqdq zmm4,zmm4,zmm4
  401bab:	62 f3 55 48 44 ed 10 	vpclmullqhqdq zmm5,zmm5,zmm5
  401bb2:	62 f3 4d 48 44 f6 10 	vpclmullqhqdq zmm6,zmm6,zmm6
  401bb9:	62 f3 45 48 44 ff 10 	vpclmullqhqdq zmm7,zmm7,zmm7
  401bc0:	62 53 3d 48 44 c0 10 	vpclmullqhqdq zmm8,zmm8,zmm8
  401bc7:	62 53 35 48 44 c9 10 	vpclmullqhqdq zmm9,zmm9,zmm9
  401bce:	62 53 2d 48 44 d2 10 	vpclmullqhqdq zmm10,zmm10,zmm10
  401bd5:	62 53 25 48 44 db 10 	vpclmullqhqdq zmm11,zmm11,zmm11
  401bdc:	62 53 1d 48 44 e4 10 	vpclmullqhqdq zmm12,zmm12,zmm12
  401be3:	62 53 15 48 44 ed 10 	vpclmullqhqdq zmm13,zmm13,zmm13
  401bea:	62 53 0d 48 44 f6 10 	vpclmullqhqdq zmm14,zmm14,zmm14
  401bf1:	62 53 05 48 44 ff 10 	vpclmullqhqdq zmm15,zmm15,zmm15
  401bf8:	62 f3 7d 48 44 c0 10 	vpclmullqhqdq zmm0,zmm0,zmm0
  401bff:	62 f3 75 48 44 c9 10 	vpclmullqhqdq zmm1,zmm1,zmm1
  401c06:	62 f3 6d 48 44 d2 10 	vpclmullqhqdq zmm2,zmm2,zmm2
  401c0d:	62 f3 65 48 44 db 10 	vpclmullqhqdq zmm3,zmm3,zmm3
  401c14:	62 f3 5d 48 44 e4 10 	vpclmullqhqdq zmm4,zmm4,zmm4
  401c1b:	62 f3 55 48 44 ed 10 	vpclmullqhqdq zmm5,zmm5,zmm5
  401c22:	62 f3 4d 48 44 f6 10 	vpclmullqhqdq zmm6,zmm6,zmm6
  401c29:	62 f3 45 48 44 ff 10 	vpclmullqhqdq zmm7,zmm7,zmm7
  401c30:	62 53 3d 48 44 c0 10 	vpclmullqhqdq zmm8,zmm8,zmm8
  401c37:	62 53 35 48 44 c9 10 	vpclmullqhqdq zmm9,zmm9,zmm9
  401c3e:	62 53 2d 48 44 d2 10 	vpclmullqhqdq zmm10,zmm10,zmm10
  401c45:	62 53 25 48 44 db 10 	vpclmullqhqdq zmm11,zmm11,zmm11
  401c4c:	62 53 1d 48 44 e4 10 	vpclmullqhqdq zmm12,zmm12,zmm12
  401c53:	62 53 15 48 44 ed 10 	vpclmullqhqdq zmm13,zmm13,zmm13
  401c5a:	62 53 0d 48 44 f6 10 	vpclmullqhqdq zmm14,zmm14,zmm14
  401c61:	62 53 05 48 44 ff 10 	vpclmullqhqdq zmm15,zmm15,zmm15
  401c68:	62 f3 7d 48 44 c0 10 	vpclmullqhqdq zmm0,zmm0,zmm0
  401c6f:	62 f3 75 48 44 c9 10 	vpclmullqhqdq zmm1,zmm1,zmm1
  401c76:	62 f3 6d 48 44 d2 10 	vpclmullqhqdq zmm2,zmm2,zmm2
  401c7d:	62 f3 65 48 44 db 10 	vpclmullqhqdq zmm3,zmm3,zmm3
  401c84:	62 f3 5d 48 44 e4 10 	vpclmullqhqdq zmm4,zmm4,zmm4
  401c8b:	62 f3 55 48 44 ed 10 	vpclmullqhqdq zmm5,zmm5,zmm5
  401c92:	62 f3 4d 48 44 f6 10 	vpclmullqhqdq zmm6,zmm6,zmm6
  401c99:	62 f3 45 48 44 ff 10 	vpclmullqhqdq zmm7,zmm7,zmm7
  401ca0:	62 53 3d 48 44 c0 10 	vpclmullqhqdq zmm8,zmm8,zmm8
  401ca7:	62 53 35 48 44 c9 10 	vpclmullqhqdq zmm9,zmm9,zmm9
  401cae:	62 53 2d 48 44 d2 10 	vpclmullqhqdq zmm10,zmm10,zmm10
  401cb5:	62 53 25 48 44 db 10 	vpclmullqhqdq zmm11,zmm11,zmm11
  401cbc:	62 53 1d 48 44 e4 10 	vpclmullqhqdq zmm12,zmm12,zmm12
  401cc3:	62 53 15 48 44 ed 10 	vpclmullqhqdq zmm13,zmm13,zmm13
  401cca:	62 53 0d 48 44 f6 10 	vpclmullqhqdq zmm14,zmm14,zmm14
  401cd1:	62 53 05 48 44 ff 10 	vpclmullqhqdq zmm15,zmm15,zmm15
  401cd8:	62 f3 7d 48 44 c0 10 	vpclmullqhqdq zmm0,zmm0,zmm0
  401cdf:	62 f3 75 48 44 c9 10 	vpclmullqhqdq zmm1,zmm1,zmm1
  401ce6:	62 f3 6d 48 44 d2 10 	vpclmullqhqdq zmm2,zmm2,zmm2
  401ced:	62 f3 65 48 44 db 10 	vpclmullqhqdq zmm3,zmm3,zmm3
  401cf4:	62 f3 5d 48 44 e4 10 	vpclmullqhqdq zmm4,zmm4,zmm4
  401cfb:	62 f3 55 48 44 ed 10 	vpclmullqhqdq zmm5,zmm5,zmm5
  401d02:	62 f3 4d 48 44 f6 10 	vpclmullqhqdq zmm6,zmm6,zmm6
  401d09:	62 f3 45 48 44 ff 10 	vpclmullqhqdq zmm7,zmm7,zmm7
  401d10:	62 53 3d 48 44 c0 10 	vpclmullqhqdq zmm8,zmm8,zmm8
  401d17:	62 53 35 48 44 c9 10 	vpclmullqhqdq zmm9,zmm9,zmm9
  401d1e:	62 53 2d 48 44 d2 10 	vpclmullqhqdq zmm10,zmm10,zmm10
  401d25:	62 53 25 48 44 db 10 	vpclmullqhqdq zmm11,zmm11,zmm11
  401d2c:	62 53 1d 48 44 e4 10 	vpclmullqhqdq zmm12,zmm12,zmm12
  401d33:	62 53 15 48 44 ed 10 	vpclmullqhqdq zmm13,zmm13,zmm13
  401d3a:	62 53 0d 48 44 f6 10 	vpclmullqhqdq zmm14,zmm14,zmm14
  401d41:	62 53 05 48 44 ff 10 	vpclmullqhqdq zmm15,zmm15,zmm15
  401d48:	62 f3 7d 48 44 c0 10 	vpclmullqhqdq zmm0,zmm0,zmm0
  401d4f:	62 f3 75 48 44 c9 10 	vpclmullqhqdq zmm1,zmm1,zmm1
  401d56:	62 f3 6d 48 44 d2 10 	vpclmullqhqdq zmm2,zmm2,zmm2
  401d5d:	62 f3 65 48 44 db 10 	vpclmullqhqdq zmm3,zmm3,zmm3
  401d64:	62 f3 5d 48 44 e4 10 	vpclmullqhqdq zmm4,zmm4,zmm4
  401d6b:	62 f3 55 48 44 ed 10 	vpclmullqhqdq zmm5,zmm5,zmm5
  401d72:	62 f3 4d 48 44 f6 10 	vpclmullqhqdq zmm6,zmm6,zmm6
  401d79:	62 f3 45 48 44 ff 10 	vpclmullqhqdq zmm7,zmm7,zmm7
  401d80:	62 53 3d 48 44 c0 10 	vpclmullqhqdq zmm8,zmm8,zmm8
  401d87:	62 53 35 48 44 c9 10 	vpclmullqhqdq zmm9,zmm9,zmm9
  401d8e:	62 53 2d 48 44 d2 10 	vpclmullqhqdq zmm10,zmm10,zmm10
  401d95:	62 53 25 48 44 db 10 	vpclmullqhqdq zmm11,zmm11,zmm11
  401d9c:	62 53 1d 48 44 e4 10 	vpclmullqhqdq zmm12,zmm12,zmm12
  401da3:	62 53 15 48 44 ed 10 	vpclmullqhqdq zmm13,zmm13,zmm13
  401daa:	62 53 0d 48 44 f6 10 	vpclmullqhqdq zmm14,zmm14,zmm14
  401db1:	62 53 05 48 44 ff 10 	vpclmullqhqdq zmm15,zmm15,zmm15
  401db8:	62 f3 7d 48 44 c0 10 	vpclmullqhqdq zmm0,zmm0,zmm0
  401dbf:	62 f3 75 48 44 c9 10 	vpclmullqhqdq zmm1,zmm1,zmm1
  401dc6:	62 f3 6d 48 44 d2 10 	vpclmullqhqdq zmm2,zmm2,zmm2
  401dcd:	62 f3 65 48 44 db 10 	vpclmullqhqdq zmm3,zmm3,zmm3
  401dd4:	62 f3 5d 48 44 e4 10 	vpclmullqhqdq zmm4,zmm4,zmm4
  401ddb:	62 f3 55 48 44 ed 10 	vpclmullqhqdq zmm5,zmm5,zmm5
  401de2:	62 f3 4d 48 44 f6 10 	vpclmullqhqdq zmm6,zmm6,zmm6
  401de9:	62 f3 45 48 44 ff 10 	vpclmullqhqdq zmm7,zmm7,zmm7
  401df0:	62 53 3d 48 44 c0 10 	vpclmullqhqdq zmm8,zmm8,zmm8
  401df7:	62 53 35 48 44 c9 10 	vpclmullqhqdq zmm9,zmm9,zmm9
  401dfe:	62 53 2d 48 44 d2 10 	vpclmullqhqdq zmm10,zmm10,zmm10
  401e05:	62 53 25 48 44 db 10 	vpclmullqhqdq zmm11,zmm11,zmm11
  401e0c:	62 53 1d 48 44 e4 10 	vpclmullqhqdq zmm12,zmm12,zmm12
  401e13:	62 53 15 48 44 ed 10 	vpclmullqhqdq zmm13,zmm13,zmm13
  401e1a:	62 53 0d 48 44 f6 10 	vpclmullqhqdq zmm14,zmm14,zmm14
  401e21:	62 53 05 48 44 ff 10 	vpclmullqhqdq zmm15,zmm15,zmm15
  401e28:	ff c8                	dec    eax
  401e2a:	0f 85 78 fc ff ff    	jne    401aa8 <phz+0x78>
  401e30:	0f ae e8             	lfence 
  401e33:	0f 31                	rdtsc  
  401e35:	c5 f8 57 c0          	vxorps xmm0,xmm0,xmm0
  401e39:	48 c1 e2 20          	shl    rdx,0x20
  401e3d:	48 09 d0             	or     rax,rdx
  401e40:	48 29 c8             	sub    rax,rcx
  401e43:	62 f1 ff 08 7b c0    	vcvtusi2sd xmm0,xmm0,rax
  401e49:	c5 fb 5e 05 af 32 00 	vdivsd xmm0,xmm0,QWORD PTR [rip+0x32af]        # 405100 <__dso_handle+0xf8>
  401e50:	00 
  401e51:	c5 f8 77             	vzeroupper 
  401e54:	c3                   	ret    
  401e55:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
  401e5c:	00 00 00 00 

0000000000401e60 <phy>:
  401e60:	62 f1 7d 48 ef c0    	vpxord zmm0,zmm0,zmm0
  401e66:	62 f1 75 48 ef c9    	vpxord zmm1,zmm1,zmm1
  401e6c:	62 f1 6d 48 ef d2    	vpxord zmm2,zmm2,zmm2
  401e72:	62 f1 65 48 ef db    	vpxord zmm3,zmm3,zmm3
  401e78:	62 f1 5d 48 ef e4    	vpxord zmm4,zmm4,zmm4
  401e7e:	62 f1 55 48 ef ed    	vpxord zmm5,zmm5,zmm5
  401e84:	62 f1 4d 48 ef f6    	vpxord zmm6,zmm6,zmm6
  401e8a:	62 f1 45 48 ef ff    	vpxord zmm7,zmm7,zmm7
  401e90:	62 51 3d 48 ef c0    	vpxord zmm8,zmm8,zmm8
  401e96:	62 51 35 48 ef c9    	vpxord zmm9,zmm9,zmm9
  401e9c:	62 51 2d 48 ef d2    	vpxord zmm10,zmm10,zmm10
  401ea2:	62 51 25 48 ef db    	vpxord zmm11,zmm11,zmm11
  401ea8:	62 51 1d 48 ef e4    	vpxord zmm12,zmm12,zmm12
  401eae:	62 51 15 48 ef ed    	vpxord zmm13,zmm13,zmm13
  401eb4:	62 51 0d 48 ef f6    	vpxord zmm14,zmm14,zmm14
  401eba:	62 51 05 48 ef ff    	vpxord zmm15,zmm15,zmm15
  401ec0:	0f ae e8             	lfence 
  401ec3:	0f 31                	rdtsc  
  401ec5:	48 89 c1             	mov    rcx,rax
  401ec8:	48 c1 e2 20          	shl    rdx,0x20
  401ecc:	48 09 d1             	or     rcx,rdx
  401ecf:	b8 a0 86 01 00       	mov    eax,0x186a0
  401ed4:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
  401ed8:	c4 e3 7d 44 c0 10    	vpclmullqhqdq ymm0,ymm0,ymm0
  401ede:	c4 e3 75 44 c9 10    	vpclmullqhqdq ymm1,ymm1,ymm1
  401ee4:	c4 e3 6d 44 d2 10    	vpclmullqhqdq ymm2,ymm2,ymm2
  401eea:	c4 e3 65 44 db 10    	vpclmullqhqdq ymm3,ymm3,ymm3
  401ef0:	c4 e3 5d 44 e4 10    	vpclmullqhqdq ymm4,ymm4,ymm4
  401ef6:	c4 e3 55 44 ed 10    	vpclmullqhqdq ymm5,ymm5,ymm5
  401efc:	c4 e3 4d 44 f6 10    	vpclmullqhqdq ymm6,ymm6,ymm6
  401f02:	c4 e3 45 44 ff 10    	vpclmullqhqdq ymm7,ymm7,ymm7
  401f08:	c4 43 3d 44 c0 10    	vpclmullqhqdq ymm8,ymm8,ymm8
  401f0e:	c4 43 35 44 c9 10    	vpclmullqhqdq ymm9,ymm9,ymm9
  401f14:	c4 43 2d 44 d2 10    	vpclmullqhqdq ymm10,ymm10,ymm10
  401f1a:	c4 43 25 44 db 10    	vpclmullqhqdq ymm11,ymm11,ymm11
  401f20:	c4 43 1d 44 e4 10    	vpclmullqhqdq ymm12,ymm12,ymm12
  401f26:	c4 43 15 44 ed 10    	vpclmullqhqdq ymm13,ymm13,ymm13
  401f2c:	c4 43 0d 44 f6 10    	vpclmullqhqdq ymm14,ymm14,ymm14
  401f32:	c4 43 05 44 ff 10    	vpclmullqhqdq ymm15,ymm15,ymm15
  401f38:	c4 e3 7d 44 c0 10    	vpclmullqhqdq ymm0,ymm0,ymm0
  401f3e:	c4 e3 75 44 c9 10    	vpclmullqhqdq ymm1,ymm1,ymm1
  401f44:	c4 e3 6d 44 d2 10    	vpclmullqhqdq ymm2,ymm2,ymm2
  401f4a:	c4 e3 65 44 db 10    	vpclmullqhqdq ymm3,ymm3,ymm3
  401f50:	c4 e3 5d 44 e4 10    	vpclmullqhqdq ymm4,ymm4,ymm4
  401f56:	c4 e3 55 44 ed 10    	vpclmullqhqdq ymm5,ymm5,ymm5
  401f5c:	c4 e3 4d 44 f6 10    	vpclmullqhqdq ymm6,ymm6,ymm6
  401f62:	c4 e3 45 44 ff 10    	vpclmullqhqdq ymm7,ymm7,ymm7
  401f68:	c4 43 3d 44 c0 10    	vpclmullqhqdq ymm8,ymm8,ymm8
  401f6e:	c4 43 35 44 c9 10    	vpclmullqhqdq ymm9,ymm9,ymm9
  401f74:	c4 43 2d 44 d2 10    	vpclmullqhqdq ymm10,ymm10,ymm10
  401f7a:	c4 43 25 44 db 10    	vpclmullqhqdq ymm11,ymm11,ymm11
  401f80:	c4 43 1d 44 e4 10    	vpclmullqhqdq ymm12,ymm12,ymm12
  401f86:	c4 43 15 44 ed 10    	vpclmullqhqdq ymm13,ymm13,ymm13
  401f8c:	c4 43 0d 44 f6 10    	vpclmullqhqdq ymm14,ymm14,ymm14
  401f92:	c4 43 05 44 ff 10    	vpclmullqhqdq ymm15,ymm15,ymm15
  401f98:	c4 e3 7d 44 c0 10    	vpclmullqhqdq ymm0,ymm0,ymm0
  401f9e:	c4 e3 75 44 c9 10    	vpclmullqhqdq ymm1,ymm1,ymm1
  401fa4:	c4 e3 6d 44 d2 10    	vpclmullqhqdq ymm2,ymm2,ymm2
  401faa:	c4 e3 65 44 db 10    	vpclmullqhqdq ymm3,ymm3,ymm3
  401fb0:	c4 e3 5d 44 e4 10    	vpclmullqhqdq ymm4,ymm4,ymm4
  401fb6:	c4 e3 55 44 ed 10    	vpclmullqhqdq ymm5,ymm5,ymm5
  401fbc:	c4 e3 4d 44 f6 10    	vpclmullqhqdq ymm6,ymm6,ymm6
  401fc2:	c4 e3 45 44 ff 10    	vpclmullqhqdq ymm7,ymm7,ymm7
  401fc8:	c4 43 3d 44 c0 10    	vpclmullqhqdq ymm8,ymm8,ymm8
  401fce:	c4 43 35 44 c9 10    	vpclmullqhqdq ymm9,ymm9,ymm9
  401fd4:	c4 43 2d 44 d2 10    	vpclmullqhqdq ymm10,ymm10,ymm10
  401fda:	c4 43 25 44 db 10    	vpclmullqhqdq ymm11,ymm11,ymm11
  401fe0:	c4 43 1d 44 e4 10    	vpclmullqhqdq ymm12,ymm12,ymm12
  401fe6:	c4 43 15 44 ed 10    	vpclmullqhqdq ymm13,ymm13,ymm13
  401fec:	c4 43 0d 44 f6 10    	vpclmullqhqdq ymm14,ymm14,ymm14
  401ff2:	c4 43 05 44 ff 10    	vpclmullqhqdq ymm15,ymm15,ymm15
  401ff8:	c4 e3 7d 44 c0 10    	vpclmullqhqdq ymm0,ymm0,ymm0
  401ffe:	c4 e3 75 44 c9 10    	vpclmullqhqdq ymm1,ymm1,ymm1
  402004:	c4 e3 6d 44 d2 10    	vpclmullqhqdq ymm2,ymm2,ymm2
  40200a:	c4 e3 65 44 db 10    	vpclmullqhqdq ymm3,ymm3,ymm3
  402010:	c4 e3 5d 44 e4 10    	vpclmullqhqdq ymm4,ymm4,ymm4
  402016:	c4 e3 55 44 ed 10    	vpclmullqhqdq ymm5,ymm5,ymm5
  40201c:	c4 e3 4d 44 f6 10    	vpclmullqhqdq ymm6,ymm6,ymm6
  402022:	c4 e3 45 44 ff 10    	vpclmullqhqdq ymm7,ymm7,ymm7
  402028:	c4 43 3d 44 c0 10    	vpclmullqhqdq ymm8,ymm8,ymm8
  40202e:	c4 43 35 44 c9 10    	vpclmullqhqdq ymm9,ymm9,ymm9
  402034:	c4 43 2d 44 d2 10    	vpclmullqhqdq ymm10,ymm10,ymm10
  40203a:	c4 43 25 44 db 10    	vpclmullqhqdq ymm11,ymm11,ymm11
  402040:	c4 43 1d 44 e4 10    	vpclmullqhqdq ymm12,ymm12,ymm12
  402046:	c4 43 15 44 ed 10    	vpclmullqhqdq ymm13,ymm13,ymm13
  40204c:	c4 43 0d 44 f6 10    	vpclmullqhqdq ymm14,ymm14,ymm14
  402052:	c4 43 05 44 ff 10    	vpclmullqhqdq ymm15,ymm15,ymm15
  402058:	c4 e3 7d 44 c0 10    	vpclmullqhqdq ymm0,ymm0,ymm0
  40205e:	c4 e3 75 44 c9 10    	vpclmullqhqdq ymm1,ymm1,ymm1
  402064:	c4 e3 6d 44 d2 10    	vpclmullqhqdq ymm2,ymm2,ymm2
  40206a:	c4 e3 65 44 db 10    	vpclmullqhqdq ymm3,ymm3,ymm3
  402070:	c4 e3 5d 44 e4 10    	vpclmullqhqdq ymm4,ymm4,ymm4
  402076:	c4 e3 55 44 ed 10    	vpclmullqhqdq ymm5,ymm5,ymm5
  40207c:	c4 e3 4d 44 f6 10    	vpclmullqhqdq ymm6,ymm6,ymm6
  402082:	c4 e3 45 44 ff 10    	vpclmullqhqdq ymm7,ymm7,ymm7
  402088:	c4 43 3d 44 c0 10    	vpclmullqhqdq ymm8,ymm8,ymm8
  40208e:	c4 43 35 44 c9 10    	vpclmullqhqdq ymm9,ymm9,ymm9
  402094:	c4 43 2d 44 d2 10    	vpclmullqhqdq ymm10,ymm10,ymm10
  40209a:	c4 43 25 44 db 10    	vpclmullqhqdq ymm11,ymm11,ymm11
  4020a0:	c4 43 1d 44 e4 10    	vpclmullqhqdq ymm12,ymm12,ymm12
  4020a6:	c4 43 15 44 ed 10    	vpclmullqhqdq ymm13,ymm13,ymm13
  4020ac:	c4 43 0d 44 f6 10    	vpclmullqhqdq ymm14,ymm14,ymm14
  4020b2:	c4 43 05 44 ff 10    	vpclmullqhqdq ymm15,ymm15,ymm15
  4020b8:	c4 e3 7d 44 c0 10    	vpclmullqhqdq ymm0,ymm0,ymm0
  4020be:	c4 e3 75 44 c9 10    	vpclmullqhqdq ymm1,ymm1,ymm1
  4020c4:	c4 e3 6d 44 d2 10    	vpclmullqhqdq ymm2,ymm2,ymm2
  4020ca:	c4 e3 65 44 db 10    	vpclmullqhqdq ymm3,ymm3,ymm3
  4020d0:	c4 e3 5d 44 e4 10    	vpclmullqhqdq ymm4,ymm4,ymm4
  4020d6:	c4 e3 55 44 ed 10    	vpclmullqhqdq ymm5,ymm5,ymm5
  4020dc:	c4 e3 4d 44 f6 10    	vpclmullqhqdq ymm6,ymm6,ymm6
  4020e2:	c4 e3 45 44 ff 10    	vpclmullqhqdq ymm7,ymm7,ymm7
  4020e8:	c4 43 3d 44 c0 10    	vpclmullqhqdq ymm8,ymm8,ymm8
  4020ee:	c4 43 35 44 c9 10    	vpclmullqhqdq ymm9,ymm9,ymm9
  4020f4:	c4 43 2d 44 d2 10    	vpclmullqhqdq ymm10,ymm10,ymm10
  4020fa:	c4 43 25 44 db 10    	vpclmullqhqdq ymm11,ymm11,ymm11
  402100:	c4 43 1d 44 e4 10    	vpclmullqhqdq ymm12,ymm12,ymm12
  402106:	c4 43 15 44 ed 10    	vpclmullqhqdq ymm13,ymm13,ymm13
  40210c:	c4 43 0d 44 f6 10    	vpclmullqhqdq ymm14,ymm14,ymm14
  402112:	c4 43 05 44 ff 10    	vpclmullqhqdq ymm15,ymm15,ymm15
  402118:	c4 e3 7d 44 c0 10    	vpclmullqhqdq ymm0,ymm0,ymm0
  40211e:	c4 e3 75 44 c9 10    	vpclmullqhqdq ymm1,ymm1,ymm1
  402124:	c4 e3 6d 44 d2 10    	vpclmullqhqdq ymm2,ymm2,ymm2
  40212a:	c4 e3 65 44 db 10    	vpclmullqhqdq ymm3,ymm3,ymm3
  402130:	c4 e3 5d 44 e4 10    	vpclmullqhqdq ymm4,ymm4,ymm4
  402136:	c4 e3 55 44 ed 10    	vpclmullqhqdq ymm5,ymm5,ymm5
  40213c:	c4 e3 4d 44 f6 10    	vpclmullqhqdq ymm6,ymm6,ymm6
  402142:	c4 e3 45 44 ff 10    	vpclmullqhqdq ymm7,ymm7,ymm7
  402148:	c4 43 3d 44 c0 10    	vpclmullqhqdq ymm8,ymm8,ymm8
  40214e:	c4 43 35 44 c9 10    	vpclmullqhqdq ymm9,ymm9,ymm9
  402154:	c4 43 2d 44 d2 10    	vpclmullqhqdq ymm10,ymm10,ymm10
  40215a:	c4 43 25 44 db 10    	vpclmullqhqdq ymm11,ymm11,ymm11
  402160:	c4 43 1d 44 e4 10    	vpclmullqhqdq ymm12,ymm12,ymm12
  402166:	c4 43 15 44 ed 10    	vpclmullqhqdq ymm13,ymm13,ymm13
  40216c:	c4 43 0d 44 f6 10    	vpclmullqhqdq ymm14,ymm14,ymm14
  402172:	c4 43 05 44 ff 10    	vpclmullqhqdq ymm15,ymm15,ymm15
  402178:	c4 e3 7d 44 c0 10    	vpclmullqhqdq ymm0,ymm0,ymm0
  40217e:	c4 e3 75 44 c9 10    	vpclmullqhqdq ymm1,ymm1,ymm1
  402184:	c4 e3 6d 44 d2 10    	vpclmullqhqdq ymm2,ymm2,ymm2
  40218a:	c4 e3 65 44 db 10    	vpclmullqhqdq ymm3,ymm3,ymm3
  402190:	c4 e3 5d 44 e4 10    	vpclmullqhqdq ymm4,ymm4,ymm4
  402196:	c4 e3 55 44 ed 10    	vpclmullqhqdq ymm5,ymm5,ymm5
  40219c:	c4 e3 4d 44 f6 10    	vpclmullqhqdq ymm6,ymm6,ymm6
  4021a2:	c4 e3 45 44 ff 10    	vpclmullqhqdq ymm7,ymm7,ymm7
  4021a8:	c4 43 3d 44 c0 10    	vpclmullqhqdq ymm8,ymm8,ymm8
  4021ae:	c4 43 35 44 c9 10    	vpclmullqhqdq ymm9,ymm9,ymm9
  4021b4:	c4 43 2d 44 d2 10    	vpclmullqhqdq ymm10,ymm10,ymm10
  4021ba:	c4 43 25 44 db 10    	vpclmullqhqdq ymm11,ymm11,ymm11
  4021c0:	c4 43 1d 44 e4 10    	vpclmullqhqdq ymm12,ymm12,ymm12
  4021c6:	c4 43 15 44 ed 10    	vpclmullqhqdq ymm13,ymm13,ymm13
  4021cc:	c4 43 0d 44 f6 10    	vpclmullqhqdq ymm14,ymm14,ymm14
  4021d2:	c4 43 05 44 ff 10    	vpclmullqhqdq ymm15,ymm15,ymm15
  4021d8:	ff c8                	dec    eax
  4021da:	0f 85 f8 fc ff ff    	jne    401ed8 <phy+0x78>
  4021e0:	0f ae e8             	lfence 
  4021e3:	0f 31                	rdtsc  
  4021e5:	c5 f8 57 c0          	vxorps xmm0,xmm0,xmm0
  4021e9:	48 c1 e2 20          	shl    rdx,0x20
  4021ed:	48 09 d0             	or     rax,rdx
  4021f0:	48 29 c8             	sub    rax,rcx
  4021f3:	62 f1 ff 08 7b c0    	vcvtusi2sd xmm0,xmm0,rax
  4021f9:	c5 fb 5e 05 ff 2e 00 	vdivsd xmm0,xmm0,QWORD PTR [rip+0x2eff]        # 405100 <__dso_handle+0xf8>
  402200:	00 
  402201:	c5 f8 77             	vzeroupper 
  402204:	c3                   	ret    
  402205:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
  40220c:	00 00 00 00 

0000000000402210 <phx>:
  402210:	62 f1 7d 48 ef c0    	vpxord zmm0,zmm0,zmm0
  402216:	62 f1 75 48 ef c9    	vpxord zmm1,zmm1,zmm1
  40221c:	62 f1 6d 48 ef d2    	vpxord zmm2,zmm2,zmm2
  402222:	62 f1 65 48 ef db    	vpxord zmm3,zmm3,zmm3
  402228:	62 f1 5d 48 ef e4    	vpxord zmm4,zmm4,zmm4
  40222e:	62 f1 55 48 ef ed    	vpxord zmm5,zmm5,zmm5
  402234:	62 f1 4d 48 ef f6    	vpxord zmm6,zmm6,zmm6
  40223a:	62 f1 45 48 ef ff    	vpxord zmm7,zmm7,zmm7
  402240:	62 51 3d 48 ef c0    	vpxord zmm8,zmm8,zmm8
  402246:	62 51 35 48 ef c9    	vpxord zmm9,zmm9,zmm9
  40224c:	62 51 2d 48 ef d2    	vpxord zmm10,zmm10,zmm10
  402252:	62 51 25 48 ef db    	vpxord zmm11,zmm11,zmm11
  402258:	62 51 1d 48 ef e4    	vpxord zmm12,zmm12,zmm12
  40225e:	62 51 15 48 ef ed    	vpxord zmm13,zmm13,zmm13
  402264:	62 51 0d 48 ef f6    	vpxord zmm14,zmm14,zmm14
  40226a:	62 51 05 48 ef ff    	vpxord zmm15,zmm15,zmm15
  402270:	0f ae e8             	lfence 
  402273:	0f 31                	rdtsc  
  402275:	48 89 c1             	mov    rcx,rax
  402278:	48 c1 e2 20          	shl    rdx,0x20
  40227c:	48 09 d1             	or     rcx,rdx
  40227f:	b8 a0 86 01 00       	mov    eax,0x186a0
  402284:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
  402288:	c4 e3 79 44 c0 10    	vpclmullqhqdq xmm0,xmm0,xmm0
  40228e:	c4 e3 71 44 c9 10    	vpclmullqhqdq xmm1,xmm1,xmm1
  402294:	c4 e3 69 44 d2 10    	vpclmullqhqdq xmm2,xmm2,xmm2
  40229a:	c4 e3 61 44 db 10    	vpclmullqhqdq xmm3,xmm3,xmm3
  4022a0:	c4 e3 59 44 e4 10    	vpclmullqhqdq xmm4,xmm4,xmm4
  4022a6:	c4 e3 51 44 ed 10    	vpclmullqhqdq xmm5,xmm5,xmm5
  4022ac:	c4 e3 49 44 f6 10    	vpclmullqhqdq xmm6,xmm6,xmm6
  4022b2:	c4 e3 41 44 ff 10    	vpclmullqhqdq xmm7,xmm7,xmm7
  4022b8:	c4 43 39 44 c0 10    	vpclmullqhqdq xmm8,xmm8,xmm8
  4022be:	c4 43 31 44 c9 10    	vpclmullqhqdq xmm9,xmm9,xmm9
  4022c4:	c4 43 29 44 d2 10    	vpclmullqhqdq xmm10,xmm10,xmm10
  4022ca:	c4 43 21 44 db 10    	vpclmullqhqdq xmm11,xmm11,xmm11
  4022d0:	c4 43 19 44 e4 10    	vpclmullqhqdq xmm12,xmm12,xmm12
  4022d6:	c4 43 11 44 ed 10    	vpclmullqhqdq xmm13,xmm13,xmm13
  4022dc:	c4 43 09 44 f6 10    	vpclmullqhqdq xmm14,xmm14,xmm14
  4022e2:	c4 43 01 44 ff 10    	vpclmullqhqdq xmm15,xmm15,xmm15
  4022e8:	c4 e3 79 44 c0 10    	vpclmullqhqdq xmm0,xmm0,xmm0
  4022ee:	c4 e3 71 44 c9 10    	vpclmullqhqdq xmm1,xmm1,xmm1
  4022f4:	c4 e3 69 44 d2 10    	vpclmullqhqdq xmm2,xmm2,xmm2
  4022fa:	c4 e3 61 44 db 10    	vpclmullqhqdq xmm3,xmm3,xmm3
  402300:	c4 e3 59 44 e4 10    	vpclmullqhqdq xmm4,xmm4,xmm4
  402306:	c4 e3 51 44 ed 10    	vpclmullqhqdq xmm5,xmm5,xmm5
  40230c:	c4 e3 49 44 f6 10    	vpclmullqhqdq xmm6,xmm6,xmm6
  402312:	c4 e3 41 44 ff 10    	vpclmullqhqdq xmm7,xmm7,xmm7
  402318:	c4 43 39 44 c0 10    	vpclmullqhqdq xmm8,xmm8,xmm8
  40231e:	c4 43 31 44 c9 10    	vpclmullqhqdq xmm9,xmm9,xmm9
  402324:	c4 43 29 44 d2 10    	vpclmullqhqdq xmm10,xmm10,xmm10
  40232a:	c4 43 21 44 db 10    	vpclmullqhqdq xmm11,xmm11,xmm11
  402330:	c4 43 19 44 e4 10    	vpclmullqhqdq xmm12,xmm12,xmm12
  402336:	c4 43 11 44 ed 10    	vpclmullqhqdq xmm13,xmm13,xmm13
  40233c:	c4 43 09 44 f6 10    	vpclmullqhqdq xmm14,xmm14,xmm14
  402342:	c4 43 01 44 ff 10    	vpclmullqhqdq xmm15,xmm15,xmm15
  402348:	c4 e3 79 44 c0 10    	vpclmullqhqdq xmm0,xmm0,xmm0
  40234e:	c4 e3 71 44 c9 10    	vpclmullqhqdq xmm1,xmm1,xmm1
  402354:	c4 e3 69 44 d2 10    	vpclmullqhqdq xmm2,xmm2,xmm2
  40235a:	c4 e3 61 44 db 10    	vpclmullqhqdq xmm3,xmm3,xmm3
  402360:	c4 e3 59 44 e4 10    	vpclmullqhqdq xmm4,xmm4,xmm4
  402366:	c4 e3 51 44 ed 10    	vpclmullqhqdq xmm5,xmm5,xmm5
  40236c:	c4 e3 49 44 f6 10    	vpclmullqhqdq xmm6,xmm6,xmm6
  402372:	c4 e3 41 44 ff 10    	vpclmullqhqdq xmm7,xmm7,xmm7
  402378:	c4 43 39 44 c0 10    	vpclmullqhqdq xmm8,xmm8,xmm8
  40237e:	c4 43 31 44 c9 10    	vpclmullqhqdq xmm9,xmm9,xmm9
  402384:	c4 43 29 44 d2 10    	vpclmullqhqdq xmm10,xmm10,xmm10
  40238a:	c4 43 21 44 db 10    	vpclmullqhqdq xmm11,xmm11,xmm11
  402390:	c4 43 19 44 e4 10    	vpclmullqhqdq xmm12,xmm12,xmm12
  402396:	c4 43 11 44 ed 10    	vpclmullqhqdq xmm13,xmm13,xmm13
  40239c:	c4 43 09 44 f6 10    	vpclmullqhqdq xmm14,xmm14,xmm14
  4023a2:	c4 43 01 44 ff 10    	vpclmullqhqdq xmm15,xmm15,xmm15
  4023a8:	c4 e3 79 44 c0 10    	vpclmullqhqdq xmm0,xmm0,xmm0
  4023ae:	c4 e3 71 44 c9 10    	vpclmullqhqdq xmm1,xmm1,xmm1
  4023b4:	c4 e3 69 44 d2 10    	vpclmullqhqdq xmm2,xmm2,xmm2
  4023ba:	c4 e3 61 44 db 10    	vpclmullqhqdq xmm3,xmm3,xmm3
  4023c0:	c4 e3 59 44 e4 10    	vpclmullqhqdq xmm4,xmm4,xmm4
  4023c6:	c4 e3 51 44 ed 10    	vpclmullqhqdq xmm5,xmm5,xmm5
  4023cc:	c4 e3 49 44 f6 10    	vpclmullqhqdq xmm6,xmm6,xmm6
  4023d2:	c4 e3 41 44 ff 10    	vpclmullqhqdq xmm7,xmm7,xmm7
  4023d8:	c4 43 39 44 c0 10    	vpclmullqhqdq xmm8,xmm8,xmm8
  4023de:	c4 43 31 44 c9 10    	vpclmullqhqdq xmm9,xmm9,xmm9
  4023e4:	c4 43 29 44 d2 10    	vpclmullqhqdq xmm10,xmm10,xmm10
  4023ea:	c4 43 21 44 db 10    	vpclmullqhqdq xmm11,xmm11,xmm11
  4023f0:	c4 43 19 44 e4 10    	vpclmullqhqdq xmm12,xmm12,xmm12
  4023f6:	c4 43 11 44 ed 10    	vpclmullqhqdq xmm13,xmm13,xmm13
  4023fc:	c4 43 09 44 f6 10    	vpclmullqhqdq xmm14,xmm14,xmm14
  402402:	c4 43 01 44 ff 10    	vpclmullqhqdq xmm15,xmm15,xmm15
  402408:	c4 e3 79 44 c0 10    	vpclmullqhqdq xmm0,xmm0,xmm0
  40240e:	c4 e3 71 44 c9 10    	vpclmullqhqdq xmm1,xmm1,xmm1
  402414:	c4 e3 69 44 d2 10    	vpclmullqhqdq xmm2,xmm2,xmm2
  40241a:	c4 e3 61 44 db 10    	vpclmullqhqdq xmm3,xmm3,xmm3
  402420:	c4 e3 59 44 e4 10    	vpclmullqhqdq xmm4,xmm4,xmm4
  402426:	c4 e3 51 44 ed 10    	vpclmullqhqdq xmm5,xmm5,xmm5
  40242c:	c4 e3 49 44 f6 10    	vpclmullqhqdq xmm6,xmm6,xmm6
  402432:	c4 e3 41 44 ff 10    	vpclmullqhqdq xmm7,xmm7,xmm7
  402438:	c4 43 39 44 c0 10    	vpclmullqhqdq xmm8,xmm8,xmm8
  40243e:	c4 43 31 44 c9 10    	vpclmullqhqdq xmm9,xmm9,xmm9
  402444:	c4 43 29 44 d2 10    	vpclmullqhqdq xmm10,xmm10,xmm10
  40244a:	c4 43 21 44 db 10    	vpclmullqhqdq xmm11,xmm11,xmm11
  402450:	c4 43 19 44 e4 10    	vpclmullqhqdq xmm12,xmm12,xmm12
  402456:	c4 43 11 44 ed 10    	vpclmullqhqdq xmm13,xmm13,xmm13
  40245c:	c4 43 09 44 f6 10    	vpclmullqhqdq xmm14,xmm14,xmm14
  402462:	c4 43 01 44 ff 10    	vpclmullqhqdq xmm15,xmm15,xmm15
  402468:	c4 e3 79 44 c0 10    	vpclmullqhqdq xmm0,xmm0,xmm0
  40246e:	c4 e3 71 44 c9 10    	vpclmullqhqdq xmm1,xmm1,xmm1
  402474:	c4 e3 69 44 d2 10    	vpclmullqhqdq xmm2,xmm2,xmm2
  40247a:	c4 e3 61 44 db 10    	vpclmullqhqdq xmm3,xmm3,xmm3
  402480:	c4 e3 59 44 e4 10    	vpclmullqhqdq xmm4,xmm4,xmm4
  402486:	c4 e3 51 44 ed 10    	vpclmullqhqdq xmm5,xmm5,xmm5
  40248c:	c4 e3 49 44 f6 10    	vpclmullqhqdq xmm6,xmm6,xmm6
  402492:	c4 e3 41 44 ff 10    	vpclmullqhqdq xmm7,xmm7,xmm7
  402498:	c4 43 39 44 c0 10    	vpclmullqhqdq xmm8,xmm8,xmm8
  40249e:	c4 43 31 44 c9 10    	vpclmullqhqdq xmm9,xmm9,xmm9
  4024a4:	c4 43 29 44 d2 10    	vpclmullqhqdq xmm10,xmm10,xmm10
  4024aa:	c4 43 21 44 db 10    	vpclmullqhqdq xmm11,xmm11,xmm11
  4024b0:	c4 43 19 44 e4 10    	vpclmullqhqdq xmm12,xmm12,xmm12
  4024b6:	c4 43 11 44 ed 10    	vpclmullqhqdq xmm13,xmm13,xmm13
  4024bc:	c4 43 09 44 f6 10    	vpclmullqhqdq xmm14,xmm14,xmm14
  4024c2:	c4 43 01 44 ff 10    	vpclmullqhqdq xmm15,xmm15,xmm15
  4024c8:	c4 e3 79 44 c0 10    	vpclmullqhqdq xmm0,xmm0,xmm0
  4024ce:	c4 e3 71 44 c9 10    	vpclmullqhqdq xmm1,xmm1,xmm1
  4024d4:	c4 e3 69 44 d2 10    	vpclmullqhqdq xmm2,xmm2,xmm2
  4024da:	c4 e3 61 44 db 10    	vpclmullqhqdq xmm3,xmm3,xmm3
  4024e0:	c4 e3 59 44 e4 10    	vpclmullqhqdq xmm4,xmm4,xmm4
  4024e6:	c4 e3 51 44 ed 10    	vpclmullqhqdq xmm5,xmm5,xmm5
  4024ec:	c4 e3 49 44 f6 10    	vpclmullqhqdq xmm6,xmm6,xmm6
  4024f2:	c4 e3 41 44 ff 10    	vpclmullqhqdq xmm7,xmm7,xmm7
  4024f8:	c4 43 39 44 c0 10    	vpclmullqhqdq xmm8,xmm8,xmm8
  4024fe:	c4 43 31 44 c9 10    	vpclmullqhqdq xmm9,xmm9,xmm9
  402504:	c4 43 29 44 d2 10    	vpclmullqhqdq xmm10,xmm10,xmm10
  40250a:	c4 43 21 44 db 10    	vpclmullqhqdq xmm11,xmm11,xmm11
  402510:	c4 43 19 44 e4 10    	vpclmullqhqdq xmm12,xmm12,xmm12
  402516:	c4 43 11 44 ed 10    	vpclmullqhqdq xmm13,xmm13,xmm13
  40251c:	c4 43 09 44 f6 10    	vpclmullqhqdq xmm14,xmm14,xmm14
  402522:	c4 43 01 44 ff 10    	vpclmullqhqdq xmm15,xmm15,xmm15
  402528:	c4 e3 79 44 c0 10    	vpclmullqhqdq xmm0,xmm0,xmm0
  40252e:	c4 e3 71 44 c9 10    	vpclmullqhqdq xmm1,xmm1,xmm1
  402534:	c4 e3 69 44 d2 10    	vpclmullqhqdq xmm2,xmm2,xmm2
  40253a:	c4 e3 61 44 db 10    	vpclmullqhqdq xmm3,xmm3,xmm3
  402540:	c4 e3 59 44 e4 10    	vpclmullqhqdq xmm4,xmm4,xmm4
  402546:	c4 e3 51 44 ed 10    	vpclmullqhqdq xmm5,xmm5,xmm5
  40254c:	c4 e3 49 44 f6 10    	vpclmullqhqdq xmm6,xmm6,xmm6
  402552:	c4 e3 41 44 ff 10    	vpclmullqhqdq xmm7,xmm7,xmm7
  402558:	c4 43 39 44 c0 10    	vpclmullqhqdq xmm8,xmm8,xmm8
  40255e:	c4 43 31 44 c9 10    	vpclmullqhqdq xmm9,xmm9,xmm9
  402564:	c4 43 29 44 d2 10    	vpclmullqhqdq xmm10,xmm10,xmm10
  40256a:	c4 43 21 44 db 10    	vpclmullqhqdq xmm11,xmm11,xmm11
  402570:	c4 43 19 44 e4 10    	vpclmullqhqdq xmm12,xmm12,xmm12
  402576:	c4 43 11 44 ed 10    	vpclmullqhqdq xmm13,xmm13,xmm13
  40257c:	c4 43 09 44 f6 10    	vpclmullqhqdq xmm14,xmm14,xmm14
  402582:	c4 43 01 44 ff 10    	vpclmullqhqdq xmm15,xmm15,xmm15
  402588:	ff c8                	dec    eax
  40258a:	0f 85 f8 fc ff ff    	jne    402288 <phx+0x78>
  402590:	0f ae e8             	lfence 
  402593:	0f 31                	rdtsc  
  402595:	c5 f8 57 c0          	vxorps xmm0,xmm0,xmm0
  402599:	48 c1 e2 20          	shl    rdx,0x20
  40259d:	48 09 d0             	or     rax,rdx
  4025a0:	48 29 c8             	sub    rax,rcx
  4025a3:	62 f1 ff 08 7b c0    	vcvtusi2sd xmm0,xmm0,rax
  4025a9:	c5 fb 5e 05 4f 2b 00 	vdivsd xmm0,xmm0,QWORD PTR [rip+0x2b4f]        # 405100 <__dso_handle+0xf8>
  4025b0:	00 
  4025b1:	c5 f8 77             	vzeroupper 
  4025b4:	c3                   	ret    
  4025b5:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
  4025bc:	00 00 00 00 

00000000004025c0 <phl>:
  4025c0:	62 f1 7d 48 ef c0    	vpxord zmm0,zmm0,zmm0
  4025c6:	62 f1 75 48 ef c9    	vpxord zmm1,zmm1,zmm1
  4025cc:	62 f1 6d 48 ef d2    	vpxord zmm2,zmm2,zmm2
  4025d2:	62 f1 65 48 ef db    	vpxord zmm3,zmm3,zmm3
  4025d8:	62 f1 5d 48 ef e4    	vpxord zmm4,zmm4,zmm4
  4025de:	62 f1 55 48 ef ed    	vpxord zmm5,zmm5,zmm5
  4025e4:	62 f1 4d 48 ef f6    	vpxord zmm6,zmm6,zmm6
  4025ea:	62 f1 45 48 ef ff    	vpxord zmm7,zmm7,zmm7
  4025f0:	62 51 3d 48 ef c0    	vpxord zmm8,zmm8,zmm8
  4025f6:	62 51 35 48 ef c9    	vpxord zmm9,zmm9,zmm9
  4025fc:	62 51 2d 48 ef d2    	vpxord zmm10,zmm10,zmm10
  402602:	62 51 25 48 ef db    	vpxord zmm11,zmm11,zmm11
  402608:	62 51 1d 48 ef e4    	vpxord zmm12,zmm12,zmm12
  40260e:	62 51 15 48 ef ed    	vpxord zmm13,zmm13,zmm13
  402614:	62 51 0d 48 ef f6    	vpxord zmm14,zmm14,zmm14
  40261a:	62 51 05 48 ef ff    	vpxord zmm15,zmm15,zmm15
  402620:	0f ae e8             	lfence 
  402623:	0f 31                	rdtsc  
  402625:	48 89 c1             	mov    rcx,rax
  402628:	48 c1 e2 20          	shl    rdx,0x20
  40262c:	48 09 d1             	or     rcx,rdx
  40262f:	b8 a0 86 01 00       	mov    eax,0x186a0
  402634:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
  402638:	66 0f 3a 44 c0 10    	pclmullqhqdq xmm0,xmm0
  40263e:	66 0f 3a 44 c9 10    	pclmullqhqdq xmm1,xmm1
  402644:	66 0f 3a 44 d2 10    	pclmullqhqdq xmm2,xmm2
  40264a:	66 0f 3a 44 db 10    	pclmullqhqdq xmm3,xmm3
  402650:	66 0f 3a 44 e4 10    	pclmullqhqdq xmm4,xmm4
  402656:	66 0f 3a 44 ed 10    	pclmullqhqdq xmm5,xmm5
  40265c:	66 0f 3a 44 f6 10    	pclmullqhqdq xmm6,xmm6
  402662:	66 0f 3a 44 ff 10    	pclmullqhqdq xmm7,xmm7
  402668:	66 45 0f 3a 44 c0 10 	pclmullqhqdq xmm8,xmm8
  40266f:	66 45 0f 3a 44 c9 10 	pclmullqhqdq xmm9,xmm9
  402676:	66 45 0f 3a 44 d2 10 	pclmullqhqdq xmm10,xmm10
  40267d:	66 45 0f 3a 44 db 10 	pclmullqhqdq xmm11,xmm11
  402684:	66 45 0f 3a 44 e4 10 	pclmullqhqdq xmm12,xmm12
  40268b:	66 45 0f 3a 44 ed 10 	pclmullqhqdq xmm13,xmm13
  402692:	66 45 0f 3a 44 f6 10 	pclmullqhqdq xmm14,xmm14
  402699:	66 45 0f 3a 44 ff 10 	pclmullqhqdq xmm15,xmm15
  4026a0:	66 0f 3a 44 c0 10    	pclmullqhqdq xmm0,xmm0
  4026a6:	66 0f 3a 44 c9 10    	pclmullqhqdq xmm1,xmm1
  4026ac:	66 0f 3a 44 d2 10    	pclmullqhqdq xmm2,xmm2
  4026b2:	66 0f 3a 44 db 10    	pclmullqhqdq xmm3,xmm3
  4026b8:	66 0f 3a 44 e4 10    	pclmullqhqdq xmm4,xmm4
  4026be:	66 0f 3a 44 ed 10    	pclmullqhqdq xmm5,xmm5
  4026c4:	66 0f 3a 44 f6 10    	pclmullqhqdq xmm6,xmm6
  4026ca:	66 0f 3a 44 ff 10    	pclmullqhqdq xmm7,xmm7
  4026d0:	66 45 0f 3a 44 c0 10 	pclmullqhqdq xmm8,xmm8
  4026d7:	66 45 0f 3a 44 c9 10 	pclmullqhqdq xmm9,xmm9
  4026de:	66 45 0f 3a 44 d2 10 	pclmullqhqdq xmm10,xmm10
  4026e5:	66 45 0f 3a 44 db 10 	pclmullqhqdq xmm11,xmm11
  4026ec:	66 45 0f 3a 44 e4 10 	pclmullqhqdq xmm12,xmm12
  4026f3:	66 45 0f 3a 44 ed 10 	pclmullqhqdq xmm13,xmm13
  4026fa:	66 45 0f 3a 44 f6 10 	pclmullqhqdq xmm14,xmm14
  402701:	66 45 0f 3a 44 ff 10 	pclmullqhqdq xmm15,xmm15
  402708:	66 0f 3a 44 c0 10    	pclmullqhqdq xmm0,xmm0
  40270e:	66 0f 3a 44 c9 10    	pclmullqhqdq xmm1,xmm1
  402714:	66 0f 3a 44 d2 10    	pclmullqhqdq xmm2,xmm2
  40271a:	66 0f 3a 44 db 10    	pclmullqhqdq xmm3,xmm3
  402720:	66 0f 3a 44 e4 10    	pclmullqhqdq xmm4,xmm4
  402726:	66 0f 3a 44 ed 10    	pclmullqhqdq xmm5,xmm5
  40272c:	66 0f 3a 44 f6 10    	pclmullqhqdq xmm6,xmm6
  402732:	66 0f 3a 44 ff 10    	pclmullqhqdq xmm7,xmm7
  402738:	66 45 0f 3a 44 c0 10 	pclmullqhqdq xmm8,xmm8
  40273f:	66 45 0f 3a 44 c9 10 	pclmullqhqdq xmm9,xmm9
  402746:	66 45 0f 3a 44 d2 10 	pclmullqhqdq xmm10,xmm10
  40274d:	66 45 0f 3a 44 db 10 	pclmullqhqdq xmm11,xmm11
  402754:	66 45 0f 3a 44 e4 10 	pclmullqhqdq xmm12,xmm12
  40275b:	66 45 0f 3a 44 ed 10 	pclmullqhqdq xmm13,xmm13
  402762:	66 45 0f 3a 44 f6 10 	pclmullqhqdq xmm14,xmm14
  402769:	66 45 0f 3a 44 ff 10 	pclmullqhqdq xmm15,xmm15
  402770:	66 0f 3a 44 c0 10    	pclmullqhqdq xmm0,xmm0
  402776:	66 0f 3a 44 c9 10    	pclmullqhqdq xmm1,xmm1
  40277c:	66 0f 3a 44 d2 10    	pclmullqhqdq xmm2,xmm2
  402782:	66 0f 3a 44 db 10    	pclmullqhqdq xmm3,xmm3
  402788:	66 0f 3a 44 e4 10    	pclmullqhqdq xmm4,xmm4
  40278e:	66 0f 3a 44 ed 10    	pclmullqhqdq xmm5,xmm5
  402794:	66 0f 3a 44 f6 10    	pclmullqhqdq xmm6,xmm6
  40279a:	66 0f 3a 44 ff 10    	pclmullqhqdq xmm7,xmm7
  4027a0:	66 45 0f 3a 44 c0 10 	pclmullqhqdq xmm8,xmm8
  4027a7:	66 45 0f 3a 44 c9 10 	pclmullqhqdq xmm9,xmm9
  4027ae:	66 45 0f 3a 44 d2 10 	pclmullqhqdq xmm10,xmm10
  4027b5:	66 45 0f 3a 44 db 10 	pclmullqhqdq xmm11,xmm11
  4027bc:	66 45 0f 3a 44 e4 10 	pclmullqhqdq xmm12,xmm12
  4027c3:	66 45 0f 3a 44 ed 10 	pclmullqhqdq xmm13,xmm13
  4027ca:	66 45 0f 3a 44 f6 10 	pclmullqhqdq xmm14,xmm14
  4027d1:	66 45 0f 3a 44 ff 10 	pclmullqhqdq xmm15,xmm15
  4027d8:	66 0f 3a 44 c0 10    	pclmullqhqdq xmm0,xmm0
  4027de:	66 0f 3a 44 c9 10    	pclmullqhqdq xmm1,xmm1
  4027e4:	66 0f 3a 44 d2 10    	pclmullqhqdq xmm2,xmm2
  4027ea:	66 0f 3a 44 db 10    	pclmullqhqdq xmm3,xmm3
  4027f0:	66 0f 3a 44 e4 10    	pclmullqhqdq xmm4,xmm4
  4027f6:	66 0f 3a 44 ed 10    	pclmullqhqdq xmm5,xmm5
  4027fc:	66 0f 3a 44 f6 10    	pclmullqhqdq xmm6,xmm6
  402802:	66 0f 3a 44 ff 10    	pclmullqhqdq xmm7,xmm7
  402808:	66 45 0f 3a 44 c0 10 	pclmullqhqdq xmm8,xmm8
  40280f:	66 45 0f 3a 44 c9 10 	pclmullqhqdq xmm9,xmm9
  402816:	66 45 0f 3a 44 d2 10 	pclmullqhqdq xmm10,xmm10
  40281d:	66 45 0f 3a 44 db 10 	pclmullqhqdq xmm11,xmm11
  402824:	66 45 0f 3a 44 e4 10 	pclmullqhqdq xmm12,xmm12
  40282b:	66 45 0f 3a 44 ed 10 	pclmullqhqdq xmm13,xmm13
  402832:	66 45 0f 3a 44 f6 10 	pclmullqhqdq xmm14,xmm14
  402839:	66 45 0f 3a 44 ff 10 	pclmullqhqdq xmm15,xmm15
  402840:	66 0f 3a 44 c0 10    	pclmullqhqdq xmm0,xmm0
  402846:	66 0f 3a 44 c9 10    	pclmullqhqdq xmm1,xmm1
  40284c:	66 0f 3a 44 d2 10    	pclmullqhqdq xmm2,xmm2
  402852:	66 0f 3a 44 db 10    	pclmullqhqdq xmm3,xmm3
  402858:	66 0f 3a 44 e4 10    	pclmullqhqdq xmm4,xmm4
  40285e:	66 0f 3a 44 ed 10    	pclmullqhqdq xmm5,xmm5
  402864:	66 0f 3a 44 f6 10    	pclmullqhqdq xmm6,xmm6
  40286a:	66 0f 3a 44 ff 10    	pclmullqhqdq xmm7,xmm7
  402870:	66 45 0f 3a 44 c0 10 	pclmullqhqdq xmm8,xmm8
  402877:	66 45 0f 3a 44 c9 10 	pclmullqhqdq xmm9,xmm9
  40287e:	66 45 0f 3a 44 d2 10 	pclmullqhqdq xmm10,xmm10
  402885:	66 45 0f 3a 44 db 10 	pclmullqhqdq xmm11,xmm11
  40288c:	66 45 0f 3a 44 e4 10 	pclmullqhqdq xmm12,xmm12
  402893:	66 45 0f 3a 44 ed 10 	pclmullqhqdq xmm13,xmm13
  40289a:	66 45 0f 3a 44 f6 10 	pclmullqhqdq xmm14,xmm14
  4028a1:	66 45 0f 3a 44 ff 10 	pclmullqhqdq xmm15,xmm15
  4028a8:	66 0f 3a 44 c0 10    	pclmullqhqdq xmm0,xmm0
  4028ae:	66 0f 3a 44 c9 10    	pclmullqhqdq xmm1,xmm1
  4028b4:	66 0f 3a 44 d2 10    	pclmullqhqdq xmm2,xmm2
  4028ba:	66 0f 3a 44 db 10    	pclmullqhqdq xmm3,xmm3
  4028c0:	66 0f 3a 44 e4 10    	pclmullqhqdq xmm4,xmm4
  4028c6:	66 0f 3a 44 ed 10    	pclmullqhqdq xmm5,xmm5
  4028cc:	66 0f 3a 44 f6 10    	pclmullqhqdq xmm6,xmm6
  4028d2:	66 0f 3a 44 ff 10    	pclmullqhqdq xmm7,xmm7
  4028d8:	66 45 0f 3a 44 c0 10 	pclmullqhqdq xmm8,xmm8
  4028df:	66 45 0f 3a 44 c9 10 	pclmullqhqdq xmm9,xmm9
  4028e6:	66 45 0f 3a 44 d2 10 	pclmullqhqdq xmm10,xmm10
  4028ed:	66 45 0f 3a 44 db 10 	pclmullqhqdq xmm11,xmm11
  4028f4:	66 45 0f 3a 44 e4 10 	pclmullqhqdq xmm12,xmm12
  4028fb:	66 45 0f 3a 44 ed 10 	pclmullqhqdq xmm13,xmm13
  402902:	66 45 0f 3a 44 f6 10 	pclmullqhqdq xmm14,xmm14
  402909:	66 45 0f 3a 44 ff 10 	pclmullqhqdq xmm15,xmm15
  402910:	66 0f 3a 44 c0 10    	pclmullqhqdq xmm0,xmm0
  402916:	66 0f 3a 44 c9 10    	pclmullqhqdq xmm1,xmm1
  40291c:	66 0f 3a 44 d2 10    	pclmullqhqdq xmm2,xmm2
  402922:	66 0f 3a 44 db 10    	pclmullqhqdq xmm3,xmm3
  402928:	66 0f 3a 44 e4 10    	pclmullqhqdq xmm4,xmm4
  40292e:	66 0f 3a 44 ed 10    	pclmullqhqdq xmm5,xmm5
  402934:	66 0f 3a 44 f6 10    	pclmullqhqdq xmm6,xmm6
  40293a:	66 0f 3a 44 ff 10    	pclmullqhqdq xmm7,xmm7
  402940:	66 45 0f 3a 44 c0 10 	pclmullqhqdq xmm8,xmm8
  402947:	66 45 0f 3a 44 c9 10 	pclmullqhqdq xmm9,xmm9
  40294e:	66 45 0f 3a 44 d2 10 	pclmullqhqdq xmm10,xmm10
  402955:	66 45 0f 3a 44 db 10 	pclmullqhqdq xmm11,xmm11
  40295c:	66 45 0f 3a 44 e4 10 	pclmullqhqdq xmm12,xmm12
  402963:	66 45 0f 3a 44 ed 10 	pclmullqhqdq xmm13,xmm13
  40296a:	66 45 0f 3a 44 f6 10 	pclmullqhqdq xmm14,xmm14
  402971:	66 45 0f 3a 44 ff 10 	pclmullqhqdq xmm15,xmm15
  402978:	ff c8                	dec    eax
  40297a:	0f 85 b8 fc ff ff    	jne    402638 <phl+0x78>
  402980:	0f ae e8             	lfence 
  402983:	0f 31                	rdtsc  
  402985:	c5 f8 57 c0          	vxorps xmm0,xmm0,xmm0
  402989:	48 c1 e2 20          	shl    rdx,0x20
  40298d:	48 09 d0             	or     rax,rdx
  402990:	48 29 c8             	sub    rax,rcx
  402993:	62 f1 ff 08 7b c0    	vcvtusi2sd xmm0,xmm0,rax
  402999:	c5 fb 5e 05 5f 27 00 	vdivsd xmm0,xmm0,QWORD PTR [rip+0x275f]        # 405100 <__dso_handle+0xf8>
  4029a0:	00 
  4029a1:	c5 f8 77             	vzeroupper 
  4029a4:	c3                   	ret    
  4029a5:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
  4029ac:	00 00 00 00 

00000000004029b0 <shuf>:
  4029b0:	62 f1 7d 48 ef c0    	vpxord zmm0,zmm0,zmm0
  4029b6:	62 f1 75 48 ef c9    	vpxord zmm1,zmm1,zmm1
  4029bc:	62 f1 6d 48 ef d2    	vpxord zmm2,zmm2,zmm2
  4029c2:	62 f1 65 48 ef db    	vpxord zmm3,zmm3,zmm3
  4029c8:	62 f1 5d 48 ef e4    	vpxord zmm4,zmm4,zmm4
  4029ce:	62 f1 55 48 ef ed    	vpxord zmm5,zmm5,zmm5
  4029d4:	62 f1 4d 48 ef f6    	vpxord zmm6,zmm6,zmm6
  4029da:	62 f1 45 48 ef ff    	vpxord zmm7,zmm7,zmm7
  4029e0:	62 51 3d 48 ef c0    	vpxord zmm8,zmm8,zmm8
  4029e6:	62 51 35 48 ef c9    	vpxord zmm9,zmm9,zmm9
  4029ec:	62 51 2d 48 ef d2    	vpxord zmm10,zmm10,zmm10
  4029f2:	62 51 25 48 ef db    	vpxord zmm11,zmm11,zmm11
  4029f8:	62 51 1d 48 ef e4    	vpxord zmm12,zmm12,zmm12
  4029fe:	62 51 15 48 ef ed    	vpxord zmm13,zmm13,zmm13
  402a04:	62 51 0d 48 ef f6    	vpxord zmm14,zmm14,zmm14
  402a0a:	62 51 05 48 ef ff    	vpxord zmm15,zmm15,zmm15
  402a10:	0f ae e8             	lfence 
  402a13:	0f 31                	rdtsc  
  402a15:	48 89 c1             	mov    rcx,rax
  402a18:	48 c1 e2 20          	shl    rdx,0x20
  402a1c:	48 09 d1             	or     rcx,rdx
  402a1f:	b8 a0 86 01 00       	mov    eax,0x186a0
  402a24:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
  402a28:	62 f1 7d 48 70 c0 b1 	vpshufd zmm0,zmm0,0xb1
  402a2f:	62 f1 7d 48 70 c9 b1 	vpshufd zmm1,zmm1,0xb1
  402a36:	62 f1 7d 48 70 d2 b1 	vpshufd zmm2,zmm2,0xb1
  402a3d:	62 f1 7d 48 70 db b1 	vpshufd zmm3,zmm3,0xb1
  402a44:	62 f1 7d 48 70 e4 b1 	vpshufd zmm4,zmm4,0xb1
  402a4b:	62 f1 7d 48 70 ed b1 	vpshufd zmm5,zmm5,0xb1
  402a52:	62 f1 7d 48 70 f6 b1 	vpshufd zmm6,zmm6,0xb1
  402a59:	62 f1 7d 48 70 ff b1 	vpshufd zmm7,zmm7,0xb1
  402a60:	62 51 7d 48 70 c0 b1 	vpshufd zmm8,zmm8,0xb1
  402a67:	62 51 7d 48 70 c9 b1 	vpshufd zmm9,zmm9,0xb1
  402a6e:	62 51 7d 48 70 d2 b1 	vpshufd zmm10,zmm10,0xb1
  402a75:	62 51 7d 48 70 db b1 	vpshufd zmm11,zmm11,0xb1
  402a7c:	62 51 7d 48 70 e4 b1 	vpshufd zmm12,zmm12,0xb1
  402a83:	62 51 7d 48 70 ed b1 	vpshufd zmm13,zmm13,0xb1
  402a8a:	62 51 7d 48 70 f6 b1 	vpshufd zmm14,zmm14,0xb1
  402a91:	62 51 7d 48 70 ff b1 	vpshufd zmm15,zmm15,0xb1
  402a98:	62 f1 7d 48 70 c0 b1 	vpshufd zmm0,zmm0,0xb1
  402a9f:	62 f1 7d 48 70 c9 b1 	vpshufd zmm1,zmm1,0xb1
  402aa6:	62 f1 7d 48 70 d2 b1 	vpshufd zmm2,zmm2,0xb1
  402aad:	62 f1 7d 48 70 db b1 	vpshufd zmm3,zmm3,0xb1
  402ab4:	62 f1 7d 48 70 e4 b1 	vpshufd zmm4,zmm4,0xb1
  402abb:	62 f1 7d 48 70 ed b1 	vpshufd zmm5,zmm5,0xb1
  402ac2:	62 f1 7d 48 70 f6 b1 	vpshufd zmm6,zmm6,0xb1
  402ac9:	62 f1 7d 48 70 ff b1 	vpshufd zmm7,zmm7,0xb1
  402ad0:	62 51 7d 48 70 c0 b1 	vpshufd zmm8,zmm8,0xb1
  402ad7:	62 51 7d 48 70 c9 b1 	vpshufd zmm9,zmm9,0xb1
  402ade:	62 51 7d 48 70 d2 b1 	vpshufd zmm10,zmm10,0xb1
  402ae5:	62 51 7d 48 70 db b1 	vpshufd zmm11,zmm11,0xb1
  402aec:	62 51 7d 48 70 e4 b1 	vpshufd zmm12,zmm12,0xb1
  402af3:	62 51 7d 48 70 ed b1 	vpshufd zmm13,zmm13,0xb1
  402afa:	62 51 7d 48 70 f6 b1 	vpshufd zmm14,zmm14,0xb1
  402b01:	62 51 7d 48 70 ff b1 	vpshufd zmm15,zmm15,0xb1
  402b08:	62 f1 7d 48 70 c0 b1 	vpshufd zmm0,zmm0,0xb1
  402b0f:	62 f1 7d 48 70 c9 b1 	vpshufd zmm1,zmm1,0xb1
  402b16:	62 f1 7d 48 70 d2 b1 	vpshufd zmm2,zmm2,0xb1
  402b1d:	62 f1 7d 48 70 db b1 	vpshufd zmm3,zmm3,0xb1
  402b24:	62 f1 7d 48 70 e4 b1 	vpshufd zmm4,zmm4,0xb1
  402b2b:	62 f1 7d 48 70 ed b1 	vpshufd zmm5,zmm5,0xb1
  402b32:	62 f1 7d 48 70 f6 b1 	vpshufd zmm6,zmm6,0xb1
  402b39:	62 f1 7d 48 70 ff b1 	vpshufd zmm7,zmm7,0xb1
  402b40:	62 51 7d 48 70 c0 b1 	vpshufd zmm8,zmm8,0xb1
  402b47:	62 51 7d 48 70 c9 b1 	vpshufd zmm9,zmm9,0xb1
  402b4e:	62 51 7d 48 70 d2 b1 	vpshufd zmm10,zmm10,0xb1
  402b55:	62 51 7d 48 70 db b1 	vpshufd zmm11,zmm11,0xb1
  402b5c:	62 51 7d 48 70 e4 b1 	vpshufd zmm12,zmm12,0xb1
  402b63:	62 51 7d 48 70 ed b1 	vpshufd zmm13,zmm13,0xb1
  402b6a:	62 51 7d 48 70 f6 b1 	vpshufd zmm14,zmm14,0xb1
  402b71:	62 51 7d 48 70 ff b1 	vpshufd zmm15,zmm15,0xb1
  402b78:	62 f1 7d 48 70 c0 b1 	vpshufd zmm0,zmm0,0xb1
  402b7f:	62 f1 7d 48 70 c9 b1 	vpshufd zmm1,zmm1,0xb1
  402b86:	62 f1 7d 48 70 d2 b1 	vpshufd zmm2,zmm2,0xb1
  402b8d:	62 f1 7d 48 70 db b1 	vpshufd zmm3,zmm3,0xb1
  402b94:	62 f1 7d 48 70 e4 b1 	vpshufd zmm4,zmm4,0xb1
  402b9b:	62 f1 7d 48 70 ed b1 	vpshufd zmm5,zmm5,0xb1
  402ba2:	62 f1 7d 48 70 f6 b1 	vpshufd zmm6,zmm6,0xb1
  402ba9:	62 f1 7d 48 70 ff b1 	vpshufd zmm7,zmm7,0xb1
  402bb0:	62 51 7d 48 70 c0 b1 	vpshufd zmm8,zmm8,0xb1
  402bb7:	62 51 7d 48 70 c9 b1 	vpshufd zmm9,zmm9,0xb1
  402bbe:	62 51 7d 48 70 d2 b1 	vpshufd zmm10,zmm10,0xb1
  402bc5:	62 51 7d 48 70 db b1 	vpshufd zmm11,zmm11,0xb1
  402bcc:	62 51 7d 48 70 e4 b1 	vpshufd zmm12,zmm12,0xb1
  402bd3:	62 51 7d 48 70 ed b1 	vpshufd zmm13,zmm13,0xb1
  402bda:	62 51 7d 48 70 f6 b1 	vpshufd zmm14,zmm14,0xb1
  402be1:	62 51 7d 48 70 ff b1 	vpshufd zmm15,zmm15,0xb1
  402be8:	62 f1 7d 48 70 c0 b1 	vpshufd zmm0,zmm0,0xb1
  402bef:	62 f1 7d 48 70 c9 b1 	vpshufd zmm1,zmm1,0xb1
  402bf6:	62 f1 7d 48 70 d2 b1 	vpshufd zmm2,zmm2,0xb1
  402bfd:	62 f1 7d 48 70 db b1 	vpshufd zmm3,zmm3,0xb1
  402c04:	62 f1 7d 48 70 e4 b1 	vpshufd zmm4,zmm4,0xb1
  402c0b:	62 f1 7d 48 70 ed b1 	vpshufd zmm5,zmm5,0xb1
  402c12:	62 f1 7d 48 70 f6 b1 	vpshufd zmm6,zmm6,0xb1
  402c19:	62 f1 7d 48 70 ff b1 	vpshufd zmm7,zmm7,0xb1
  402c20:	62 51 7d 48 70 c0 b1 	vpshufd zmm8,zmm8,0xb1
  402c27:	62 51 7d 48 70 c9 b1 	vpshufd zmm9,zmm9,0xb1
  402c2e:	62 51 7d 48 70 d2 b1 	vpshufd zmm10,zmm10,0xb1
  402c35:	62 51 7d 48 70 db b1 	vpshufd zmm11,zmm11,0xb1
  402c3c:	62 51 7d 48 70 e4 b1 	vpshufd zmm12,zmm12,0xb1
  402c43:	62 51 7d 48 70 ed b1 	vpshufd zmm13,zmm13,0xb1
  402c4a:	62 51 7d 48 70 f6 b1 	vpshufd zmm14,zmm14,0xb1
  402c51:	62 51 7d 48 70 ff b1 	vpshufd zmm15,zmm15,0xb1
  402c58:	62 f1 7d 48 70 c0 b1 	vpshufd zmm0,zmm0,0xb1
  402c5f:	62 f1 7d 48 70 c9 b1 	vpshufd zmm1,zmm1,0xb1
  402c66:	62 f1 7d 48 70 d2 b1 	vpshufd zmm2,zmm2,0xb1
  402c6d:	62 f1 7d 48 70 db b1 	vpshufd zmm3,zmm3,0xb1
  402c74:	62 f1 7d 48 70 e4 b1 	vpshufd zmm4,zmm4,0xb1
  402c7b:	62 f1 7d 48 70 ed b1 	vpshufd zmm5,zmm5,0xb1
  402c82:	62 f1 7d 48 70 f6 b1 	vpshufd zmm6,zmm6,0xb1
  402c89:	62 f1 7d 48 70 ff b1 	vpshufd zmm7,zmm7,0xb1
  402c90:	62 51 7d 48 70 c0 b1 	vpshufd zmm8,zmm8,0xb1
  402c97:	62 51 7d 48 70 c9 b1 	vpshufd zmm9,zmm9,0xb1
  402c9e:	62 51 7d 48 70 d2 b1 	vpshufd zmm10,zmm10,0xb1
  402ca5:	62 51 7d 48 70 db b1 	vpshufd zmm11,zmm11,0xb1
  402cac:	62 51 7d 48 70 e4 b1 	vpshufd zmm12,zmm12,0xb1
  402cb3:	62 51 7d 48 70 ed b1 	vpshufd zmm13,zmm13,0xb1
  402cba:	62 51 7d 48 70 f6 b1 	vpshufd zmm14,zmm14,0xb1
  402cc1:	62 51 7d 48 70 ff b1 	vpshufd zmm15,zmm15,0xb1
  402cc8:	62 f1 7d 48 70 c0 b1 	vpshufd zmm0,zmm0,0xb1
  402ccf:	62 f1 7d 48 70 c9 b1 	vpshufd zmm1,zmm1,0xb1
  402cd6:	62 f1 7d 48 70 d2 b1 	vpshufd zmm2,zmm2,0xb1
  402cdd:	62 f1 7d 48 70 db b1 	vpshufd zmm3,zmm3,0xb1
  402ce4:	62 f1 7d 48 70 e4 b1 	vpshufd zmm4,zmm4,0xb1
  402ceb:	62 f1 7d 48 70 ed b1 	vpshufd zmm5,zmm5,0xb1
  402cf2:	62 f1 7d 48 70 f6 b1 	vpshufd zmm6,zmm6,0xb1
  402cf9:	62 f1 7d 48 70 ff b1 	vpshufd zmm7,zmm7,0xb1
  402d00:	62 51 7d 48 70 c0 b1 	vpshufd zmm8,zmm8,0xb1
  402d07:	62 51 7d 48 70 c9 b1 	vpshufd zmm9,zmm9,0xb1
  402d0e:	62 51 7d 48 70 d2 b1 	vpshufd zmm10,zmm10,0xb1
  402d15:	62 51 7d 48 70 db b1 	vpshufd zmm11,zmm11,0xb1
  402d1c:	62 51 7d 48 70 e4 b1 	vpshufd zmm12,zmm12,0xb1
  402d23:	62 51 7d 48 70 ed b1 	vpshufd zmm13,zmm13,0xb1
  402d2a:	62 51 7d 48 70 f6 b1 	vpshufd zmm14,zmm14,0xb1
  402d31:	62 51 7d 48 70 ff b1 	vpshufd zmm15,zmm15,0xb1
  402d38:	62 f1 7d 48 70 c0 b1 	vpshufd zmm0,zmm0,0xb1
  402d3f:	62 f1 7d 48 70 c9 b1 	vpshufd zmm1,zmm1,0xb1
  402d46:	62 f1 7d 48 70 d2 b1 	vpshufd zmm2,zmm2,0xb1
  402d4d:	62 f1 7d 48 70 db b1 	vpshufd zmm3,zmm3,0xb1
  402d54:	62 f1 7d 48 70 e4 b1 	vpshufd zmm4,zmm4,0xb1
  402d5b:	62 f1 7d 48 70 ed b1 	vpshufd zmm5,zmm5,0xb1
  402d62:	62 f1 7d 48 70 f6 b1 	vpshufd zmm6,zmm6,0xb1
  402d69:	62 f1 7d 48 70 ff b1 	vpshufd zmm7,zmm7,0xb1
  402d70:	62 51 7d 48 70 c0 b1 	vpshufd zmm8,zmm8,0xb1
  402d77:	62 51 7d 48 70 c9 b1 	vpshufd zmm9,zmm9,0xb1
  402d7e:	62 51 7d 48 70 d2 b1 	vpshufd zmm10,zmm10,0xb1
  402d85:	62 51 7d 48 70 db b1 	vpshufd zmm11,zmm11,0xb1
  402d8c:	62 51 7d 48 70 e4 b1 	vpshufd zmm12,zmm12,0xb1
  402d93:	62 51 7d 48 70 ed b1 	vpshufd zmm13,zmm13,0xb1
  402d9a:	62 51 7d 48 70 f6 b1 	vpshufd zmm14,zmm14,0xb1
  402da1:	62 51 7d 48 70 ff b1 	vpshufd zmm15,zmm15,0xb1
  402da8:	ff c8                	dec    eax
  402daa:	0f 85 78 fc ff ff    	jne    402a28 <shuf+0x78>
  402db0:	0f ae e8             	lfence 
  402db3:	0f 31                	rdtsc  
  402db5:	c5 f8 57 c0          	vxorps xmm0,xmm0,xmm0
  402db9:	48 c1 e2 20          	shl    rdx,0x20
  402dbd:	48 09 d0             	or     rax,rdx
  402dc0:	48 29 c8             	sub    rax,rcx
  402dc3:	62 f1 ff 08 7b c0    	vcvtusi2sd xmm0,xmm0,rax
  402dc9:	c5 fb 5e 05 2f 23 00 	vdivsd xmm0,xmm0,QWORD PTR [rip+0x232f]        # 405100 <__dso_handle+0xf8>
  402dd0:	00 
  402dd1:	c5 f8 77             	vzeroupper 
  402dd4:	c3                   	ret    
  402dd5:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
  402ddc:	00 00 00 00 

0000000000402de0 <shift>:
  402de0:	62 f1 7d 48 ef c0    	vpxord zmm0,zmm0,zmm0
  402de6:	62 f1 75 48 ef c9    	vpxord zmm1,zmm1,zmm1
  402dec:	62 f1 6d 48 ef d2    	vpxord zmm2,zmm2,zmm2
  402df2:	62 f1 65 48 ef db    	vpxord zmm3,zmm3,zmm3
  402df8:	62 f1 5d 48 ef e4    	vpxord zmm4,zmm4,zmm4
  402dfe:	62 f1 55 48 ef ed    	vpxord zmm5,zmm5,zmm5
  402e04:	62 f1 4d 48 ef f6    	vpxord zmm6,zmm6,zmm6
  402e0a:	62 f1 45 48 ef ff    	vpxord zmm7,zmm7,zmm7
  402e10:	62 51 3d 48 ef c0    	vpxord zmm8,zmm8,zmm8
  402e16:	62 51 35 48 ef c9    	vpxord zmm9,zmm9,zmm9
  402e1c:	62 51 2d 48 ef d2    	vpxord zmm10,zmm10,zmm10
  402e22:	62 51 25 48 ef db    	vpxord zmm11,zmm11,zmm11
  402e28:	62 51 1d 48 ef e4    	vpxord zmm12,zmm12,zmm12
  402e2e:	62 51 15 48 ef ed    	vpxord zmm13,zmm13,zmm13
  402e34:	62 51 0d 48 ef f6    	vpxord zmm14,zmm14,zmm14
  402e3a:	62 51 05 48 ef ff    	vpxord zmm15,zmm15,zmm15
  402e40:	0f ae e8             	lfence 
  402e43:	0f 31                	rdtsc  
  402e45:	48 89 c1             	mov    rcx,rax
  402e48:	48 c1 e2 20          	shl    rdx,0x20
  402e4c:	48 09 d1             	or     rcx,rdx
  402e4f:	b8 a0 86 01 00       	mov    eax,0x186a0
  402e54:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
  402e58:	62 f1 fd 48 73 d0 20 	vpsrlq zmm0,zmm0,0x20
  402e5f:	62 f1 f5 48 73 d1 20 	vpsrlq zmm1,zmm1,0x20
  402e66:	62 f1 ed 48 73 d2 20 	vpsrlq zmm2,zmm2,0x20
  402e6d:	62 f1 e5 48 73 d3 20 	vpsrlq zmm3,zmm3,0x20
  402e74:	62 f1 dd 48 73 d4 20 	vpsrlq zmm4,zmm4,0x20
  402e7b:	62 f1 d5 48 73 d5 20 	vpsrlq zmm5,zmm5,0x20
  402e82:	62 f1 cd 48 73 d6 20 	vpsrlq zmm6,zmm6,0x20
  402e89:	62 f1 c5 48 73 d7 20 	vpsrlq zmm7,zmm7,0x20
  402e90:	62 d1 bd 48 73 d0 20 	vpsrlq zmm8,zmm8,0x20
  402e97:	62 d1 b5 48 73 d1 20 	vpsrlq zmm9,zmm9,0x20
  402e9e:	62 d1 ad 48 73 d2 20 	vpsrlq zmm10,zmm10,0x20
  402ea5:	62 d1 a5 48 73 d3 20 	vpsrlq zmm11,zmm11,0x20
  402eac:	62 d1 9d 48 73 d4 20 	vpsrlq zmm12,zmm12,0x20
  402eb3:	62 d1 95 48 73 d5 20 	vpsrlq zmm13,zmm13,0x20
  402eba:	62 d1 8d 48 73 d6 20 	vpsrlq zmm14,zmm14,0x20
  402ec1:	62 d1 85 48 73 d7 20 	vpsrlq zmm15,zmm15,0x20
  402ec8:	62 f1 fd 48 73 d0 20 	vpsrlq zmm0,zmm0,0x20
  402ecf:	62 f1 f5 48 73 d1 20 	vpsrlq zmm1,zmm1,0x20
  402ed6:	62 f1 ed 48 73 d2 20 	vpsrlq zmm2,zmm2,0x20
  402edd:	62 f1 e5 48 73 d3 20 	vpsrlq zmm3,zmm3,0x20
  402ee4:	62 f1 dd 48 73 d4 20 	vpsrlq zmm4,zmm4,0x20
  402eeb:	62 f1 d5 48 73 d5 20 	vpsrlq zmm5,zmm5,0x20
  402ef2:	62 f1 cd 48 73 d6 20 	vpsrlq zmm6,zmm6,0x20
  402ef9:	62 f1 c5 48 73 d7 20 	vpsrlq zmm7,zmm7,0x20
  402f00:	62 d1 bd 48 73 d0 20 	vpsrlq zmm8,zmm8,0x20
  402f07:	62 d1 b5 48 73 d1 20 	vpsrlq zmm9,zmm9,0x20
  402f0e:	62 d1 ad 48 73 d2 20 	vpsrlq zmm10,zmm10,0x20
  402f15:	62 d1 a5 48 73 d3 20 	vpsrlq zmm11,zmm11,0x20
  402f1c:	62 d1 9d 48 73 d4 20 	vpsrlq zmm12,zmm12,0x20
  402f23:	62 d1 95 48 73 d5 20 	vpsrlq zmm13,zmm13,0x20
  402f2a:	62 d1 8d 48 73 d6 20 	vpsrlq zmm14,zmm14,0x20
  402f31:	62 d1 85 48 73 d7 20 	vpsrlq zmm15,zmm15,0x20
  402f38:	62 f1 fd 48 73 d0 20 	vpsrlq zmm0,zmm0,0x20
  402f3f:	62 f1 f5 48 73 d1 20 	vpsrlq zmm1,zmm1,0x20
  402f46:	62 f1 ed 48 73 d2 20 	vpsrlq zmm2,zmm2,0x20
  402f4d:	62 f1 e5 48 73 d3 20 	vpsrlq zmm3,zmm3,0x20
  402f54:	62 f1 dd 48 73 d4 20 	vpsrlq zmm4,zmm4,0x20
  402f5b:	62 f1 d5 48 73 d5 20 	vpsrlq zmm5,zmm5,0x20
  402f62:	62 f1 cd 48 73 d6 20 	vpsrlq zmm6,zmm6,0x20
  402f69:	62 f1 c5 48 73 d7 20 	vpsrlq zmm7,zmm7,0x20
  402f70:	62 d1 bd 48 73 d0 20 	vpsrlq zmm8,zmm8,0x20
  402f77:	62 d1 b5 48 73 d1 20 	vpsrlq zmm9,zmm9,0x20
  402f7e:	62 d1 ad 48 73 d2 20 	vpsrlq zmm10,zmm10,0x20
  402f85:	62 d1 a5 48 73 d3 20 	vpsrlq zmm11,zmm11,0x20
  402f8c:	62 d1 9d 48 73 d4 20 	vpsrlq zmm12,zmm12,0x20
  402f93:	62 d1 95 48 73 d5 20 	vpsrlq zmm13,zmm13,0x20
  402f9a:	62 d1 8d 48 73 d6 20 	vpsrlq zmm14,zmm14,0x20
  402fa1:	62 d1 85 48 73 d7 20 	vpsrlq zmm15,zmm15,0x20
  402fa8:	62 f1 fd 48 73 d0 20 	vpsrlq zmm0,zmm0,0x20
  402faf:	62 f1 f5 48 73 d1 20 	vpsrlq zmm1,zmm1,0x20
  402fb6:	62 f1 ed 48 73 d2 20 	vpsrlq zmm2,zmm2,0x20
  402fbd:	62 f1 e5 48 73 d3 20 	vpsrlq zmm3,zmm3,0x20
  402fc4:	62 f1 dd 48 73 d4 20 	vpsrlq zmm4,zmm4,0x20
  402fcb:	62 f1 d5 48 73 d5 20 	vpsrlq zmm5,zmm5,0x20
  402fd2:	62 f1 cd 48 73 d6 20 	vpsrlq zmm6,zmm6,0x20
  402fd9:	62 f1 c5 48 73 d7 20 	vpsrlq zmm7,zmm7,0x20
  402fe0:	62 d1 bd 48 73 d0 20 	vpsrlq zmm8,zmm8,0x20
  402fe7:	62 d1 b5 48 73 d1 20 	vpsrlq zmm9,zmm9,0x20
  402fee:	62 d1 ad 48 73 d2 20 	vpsrlq zmm10,zmm10,0x20
  402ff5:	62 d1 a5 48 73 d3 20 	vpsrlq zmm11,zmm11,0x20
  402ffc:	62 d1 9d 48 73 d4 20 	vpsrlq zmm12,zmm12,0x20
  403003:	62 d1 95 48 73 d5 20 	vpsrlq zmm13,zmm13,0x20
  40300a:	62 d1 8d 48 73 d6 20 	vpsrlq zmm14,zmm14,0x20
  403011:	62 d1 85 48 73 d7 20 	vpsrlq zmm15,zmm15,0x20
  403018:	62 f1 fd 48 73 d0 20 	vpsrlq zmm0,zmm0,0x20
  40301f:	62 f1 f5 48 73 d1 20 	vpsrlq zmm1,zmm1,0x20
  403026:	62 f1 ed 48 73 d2 20 	vpsrlq zmm2,zmm2,0x20
  40302d:	62 f1 e5 48 73 d3 20 	vpsrlq zmm3,zmm3,0x20
  403034:	62 f1 dd 48 73 d4 20 	vpsrlq zmm4,zmm4,0x20
  40303b:	62 f1 d5 48 73 d5 20 	vpsrlq zmm5,zmm5,0x20
  403042:	62 f1 cd 48 73 d6 20 	vpsrlq zmm6,zmm6,0x20
  403049:	62 f1 c5 48 73 d7 20 	vpsrlq zmm7,zmm7,0x20
  403050:	62 d1 bd 48 73 d0 20 	vpsrlq zmm8,zmm8,0x20
  403057:	62 d1 b5 48 73 d1 20 	vpsrlq zmm9,zmm9,0x20
  40305e:	62 d1 ad 48 73 d2 20 	vpsrlq zmm10,zmm10,0x20
  403065:	62 d1 a5 48 73 d3 20 	vpsrlq zmm11,zmm11,0x20
  40306c:	62 d1 9d 48 73 d4 20 	vpsrlq zmm12,zmm12,0x20
  403073:	62 d1 95 48 73 d5 20 	vpsrlq zmm13,zmm13,0x20
  40307a:	62 d1 8d 48 73 d6 20 	vpsrlq zmm14,zmm14,0x20
  403081:	62 d1 85 48 73 d7 20 	vpsrlq zmm15,zmm15,0x20
  403088:	62 f1 fd 48 73 d0 20 	vpsrlq zmm0,zmm0,0x20
  40308f:	62 f1 f5 48 73 d1 20 	vpsrlq zmm1,zmm1,0x20
  403096:	62 f1 ed 48 73 d2 20 	vpsrlq zmm2,zmm2,0x20
  40309d:	62 f1 e5 48 73 d3 20 	vpsrlq zmm3,zmm3,0x20
  4030a4:	62 f1 dd 48 73 d4 20 	vpsrlq zmm4,zmm4,0x20
  4030ab:	62 f1 d5 48 73 d5 20 	vpsrlq zmm5,zmm5,0x20
  4030b2:	62 f1 cd 48 73 d6 20 	vpsrlq zmm6,zmm6,0x20
  4030b9:	62 f1 c5 48 73 d7 20 	vpsrlq zmm7,zmm7,0x20
  4030c0:	62 d1 bd 48 73 d0 20 	vpsrlq zmm8,zmm8,0x20
  4030c7:	62 d1 b5 48 73 d1 20 	vpsrlq zmm9,zmm9,0x20
  4030ce:	62 d1 ad 48 73 d2 20 	vpsrlq zmm10,zmm10,0x20
  4030d5:	62 d1 a5 48 73 d3 20 	vpsrlq zmm11,zmm11,0x20
  4030dc:	62 d1 9d 48 73 d4 20 	vpsrlq zmm12,zmm12,0x20
  4030e3:	62 d1 95 48 73 d5 20 	vpsrlq zmm13,zmm13,0x20
  4030ea:	62 d1 8d 48 73 d6 20 	vpsrlq zmm14,zmm14,0x20
  4030f1:	62 d1 85 48 73 d7 20 	vpsrlq zmm15,zmm15,0x20
  4030f8:	62 f1 fd 48 73 d0 20 	vpsrlq zmm0,zmm0,0x20
  4030ff:	62 f1 f5 48 73 d1 20 	vpsrlq zmm1,zmm1,0x20
  403106:	62 f1 ed 48 73 d2 20 	vpsrlq zmm2,zmm2,0x20
  40310d:	62 f1 e5 48 73 d3 20 	vpsrlq zmm3,zmm3,0x20
  403114:	62 f1 dd 48 73 d4 20 	vpsrlq zmm4,zmm4,0x20
  40311b:	62 f1 d5 48 73 d5 20 	vpsrlq zmm5,zmm5,0x20
  403122:	62 f1 cd 48 73 d6 20 	vpsrlq zmm6,zmm6,0x20
  403129:	62 f1 c5 48 73 d7 20 	vpsrlq zmm7,zmm7,0x20
  403130:	62 d1 bd 48 73 d0 20 	vpsrlq zmm8,zmm8,0x20
  403137:	62 d1 b5 48 73 d1 20 	vpsrlq zmm9,zmm9,0x20
  40313e:	62 d1 ad 48 73 d2 20 	vpsrlq zmm10,zmm10,0x20
  403145:	62 d1 a5 48 73 d3 20 	vpsrlq zmm11,zmm11,0x20
  40314c:	62 d1 9d 48 73 d4 20 	vpsrlq zmm12,zmm12,0x20
  403153:	62 d1 95 48 73 d5 20 	vpsrlq zmm13,zmm13,0x20
  40315a:	62 d1 8d 48 73 d6 20 	vpsrlq zmm14,zmm14,0x20
  403161:	62 d1 85 48 73 d7 20 	vpsrlq zmm15,zmm15,0x20
  403168:	62 f1 fd 48 73 d0 20 	vpsrlq zmm0,zmm0,0x20
  40316f:	62 f1 f5 48 73 d1 20 	vpsrlq zmm1,zmm1,0x20
  403176:	62 f1 ed 48 73 d2 20 	vpsrlq zmm2,zmm2,0x20
  40317d:	62 f1 e5 48 73 d3 20 	vpsrlq zmm3,zmm3,0x20
  403184:	62 f1 dd 48 73 d4 20 	vpsrlq zmm4,zmm4,0x20
  40318b:	62 f1 d5 48 73 d5 20 	vpsrlq zmm5,zmm5,0x20
  403192:	62 f1 cd 48 73 d6 20 	vpsrlq zmm6,zmm6,0x20
  403199:	62 f1 c5 48 73 d7 20 	vpsrlq zmm7,zmm7,0x20
  4031a0:	62 d1 bd 48 73 d0 20 	vpsrlq zmm8,zmm8,0x20
  4031a7:	62 d1 b5 48 73 d1 20 	vpsrlq zmm9,zmm9,0x20
  4031ae:	62 d1 ad 48 73 d2 20 	vpsrlq zmm10,zmm10,0x20
  4031b5:	62 d1 a5 48 73 d3 20 	vpsrlq zmm11,zmm11,0x20
  4031bc:	62 d1 9d 48 73 d4 20 	vpsrlq zmm12,zmm12,0x20
  4031c3:	62 d1 95 48 73 d5 20 	vpsrlq zmm13,zmm13,0x20
  4031ca:	62 d1 8d 48 73 d6 20 	vpsrlq zmm14,zmm14,0x20
  4031d1:	62 d1 85 48 73 d7 20 	vpsrlq zmm15,zmm15,0x20
  4031d8:	ff c8                	dec    eax
  4031da:	0f 85 78 fc ff ff    	jne    402e58 <shift+0x78>
  4031e0:	0f ae e8             	lfence 
  4031e3:	0f 31                	rdtsc  
  4031e5:	c5 f8 57 c0          	vxorps xmm0,xmm0,xmm0
  4031e9:	48 c1 e2 20          	shl    rdx,0x20
  4031ed:	48 09 d0             	or     rax,rdx
  4031f0:	48 29 c8             	sub    rax,rcx
  4031f3:	62 f1 ff 08 7b c0    	vcvtusi2sd xmm0,xmm0,rax
  4031f9:	c5 fb 5e 05 ff 1e 00 	vdivsd xmm0,xmm0,QWORD PTR [rip+0x1eff]        # 405100 <__dso_handle+0xf8>
  403200:	00 
  403201:	c5 f8 77             	vzeroupper 
  403204:	c3                   	ret    
  403205:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
  40320c:	00 00 00 00 

0000000000403210 <add>:
  403210:	62 f1 7d 48 ef c0    	vpxord zmm0,zmm0,zmm0
  403216:	62 f1 75 48 ef c9    	vpxord zmm1,zmm1,zmm1
  40321c:	62 f1 6d 48 ef d2    	vpxord zmm2,zmm2,zmm2
  403222:	62 f1 65 48 ef db    	vpxord zmm3,zmm3,zmm3
  403228:	62 f1 5d 48 ef e4    	vpxord zmm4,zmm4,zmm4
  40322e:	62 f1 55 48 ef ed    	vpxord zmm5,zmm5,zmm5
  403234:	62 f1 4d 48 ef f6    	vpxord zmm6,zmm6,zmm6
  40323a:	62 f1 45 48 ef ff    	vpxord zmm7,zmm7,zmm7
  403240:	62 51 3d 48 ef c0    	vpxord zmm8,zmm8,zmm8
  403246:	62 51 35 48 ef c9    	vpxord zmm9,zmm9,zmm9
  40324c:	62 51 2d 48 ef d2    	vpxord zmm10,zmm10,zmm10
  403252:	62 51 25 48 ef db    	vpxord zmm11,zmm11,zmm11
  403258:	62 51 1d 48 ef e4    	vpxord zmm12,zmm12,zmm12
  40325e:	62 51 15 48 ef ed    	vpxord zmm13,zmm13,zmm13
  403264:	62 51 0d 48 ef f6    	vpxord zmm14,zmm14,zmm14
  40326a:	62 51 05 48 ef ff    	vpxord zmm15,zmm15,zmm15
  403270:	0f ae e8             	lfence 
  403273:	0f 31                	rdtsc  
  403275:	48 89 c1             	mov    rcx,rax
  403278:	48 c1 e2 20          	shl    rdx,0x20
  40327c:	48 09 d1             	or     rcx,rdx
  40327f:	b8 a0 86 01 00       	mov    eax,0x186a0
  403284:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
  403288:	62 f1 fd 48 d4 c0    	vpaddq zmm0,zmm0,zmm0
  40328e:	62 f1 f5 48 d4 c9    	vpaddq zmm1,zmm1,zmm1
  403294:	62 f1 ed 48 d4 d2    	vpaddq zmm2,zmm2,zmm2
  40329a:	62 f1 e5 48 d4 db    	vpaddq zmm3,zmm3,zmm3
  4032a0:	62 f1 dd 48 d4 e4    	vpaddq zmm4,zmm4,zmm4
  4032a6:	62 f1 d5 48 d4 ed    	vpaddq zmm5,zmm5,zmm5
  4032ac:	62 f1 cd 48 d4 f6    	vpaddq zmm6,zmm6,zmm6
  4032b2:	62 f1 c5 48 d4 ff    	vpaddq zmm7,zmm7,zmm7
  4032b8:	62 51 bd 48 d4 c0    	vpaddq zmm8,zmm8,zmm8
  4032be:	62 51 b5 48 d4 c9    	vpaddq zmm9,zmm9,zmm9
  4032c4:	62 51 ad 48 d4 d2    	vpaddq zmm10,zmm10,zmm10
  4032ca:	62 51 a5 48 d4 db    	vpaddq zmm11,zmm11,zmm11
  4032d0:	62 51 9d 48 d4 e4    	vpaddq zmm12,zmm12,zmm12
  4032d6:	62 51 95 48 d4 ed    	vpaddq zmm13,zmm13,zmm13
  4032dc:	62 51 8d 48 d4 f6    	vpaddq zmm14,zmm14,zmm14
  4032e2:	62 51 85 48 d4 ff    	vpaddq zmm15,zmm15,zmm15
  4032e8:	62 f1 fd 48 d4 c0    	vpaddq zmm0,zmm0,zmm0
  4032ee:	62 f1 f5 48 d4 c9    	vpaddq zmm1,zmm1,zmm1
  4032f4:	62 f1 ed 48 d4 d2    	vpaddq zmm2,zmm2,zmm2
  4032fa:	62 f1 e5 48 d4 db    	vpaddq zmm3,zmm3,zmm3
  403300:	62 f1 dd 48 d4 e4    	vpaddq zmm4,zmm4,zmm4
  403306:	62 f1 d5 48 d4 ed    	vpaddq zmm5,zmm5,zmm5
  40330c:	62 f1 cd 48 d4 f6    	vpaddq zmm6,zmm6,zmm6
  403312:	62 f1 c5 48 d4 ff    	vpaddq zmm7,zmm7,zmm7
  403318:	62 51 bd 48 d4 c0    	vpaddq zmm8,zmm8,zmm8
  40331e:	62 51 b5 48 d4 c9    	vpaddq zmm9,zmm9,zmm9
  403324:	62 51 ad 48 d4 d2    	vpaddq zmm10,zmm10,zmm10
  40332a:	62 51 a5 48 d4 db    	vpaddq zmm11,zmm11,zmm11
  403330:	62 51 9d 48 d4 e4    	vpaddq zmm12,zmm12,zmm12
  403336:	62 51 95 48 d4 ed    	vpaddq zmm13,zmm13,zmm13
  40333c:	62 51 8d 48 d4 f6    	vpaddq zmm14,zmm14,zmm14
  403342:	62 51 85 48 d4 ff    	vpaddq zmm15,zmm15,zmm15
  403348:	62 f1 fd 48 d4 c0    	vpaddq zmm0,zmm0,zmm0
  40334e:	62 f1 f5 48 d4 c9    	vpaddq zmm1,zmm1,zmm1
  403354:	62 f1 ed 48 d4 d2    	vpaddq zmm2,zmm2,zmm2
  40335a:	62 f1 e5 48 d4 db    	vpaddq zmm3,zmm3,zmm3
  403360:	62 f1 dd 48 d4 e4    	vpaddq zmm4,zmm4,zmm4
  403366:	62 f1 d5 48 d4 ed    	vpaddq zmm5,zmm5,zmm5
  40336c:	62 f1 cd 48 d4 f6    	vpaddq zmm6,zmm6,zmm6
  403372:	62 f1 c5 48 d4 ff    	vpaddq zmm7,zmm7,zmm7
  403378:	62 51 bd 48 d4 c0    	vpaddq zmm8,zmm8,zmm8
  40337e:	62 51 b5 48 d4 c9    	vpaddq zmm9,zmm9,zmm9
  403384:	62 51 ad 48 d4 d2    	vpaddq zmm10,zmm10,zmm10
  40338a:	62 51 a5 48 d4 db    	vpaddq zmm11,zmm11,zmm11
  403390:	62 51 9d 48 d4 e4    	vpaddq zmm12,zmm12,zmm12
  403396:	62 51 95 48 d4 ed    	vpaddq zmm13,zmm13,zmm13
  40339c:	62 51 8d 48 d4 f6    	vpaddq zmm14,zmm14,zmm14
  4033a2:	62 51 85 48 d4 ff    	vpaddq zmm15,zmm15,zmm15
  4033a8:	62 f1 fd 48 d4 c0    	vpaddq zmm0,zmm0,zmm0
  4033ae:	62 f1 f5 48 d4 c9    	vpaddq zmm1,zmm1,zmm1
  4033b4:	62 f1 ed 48 d4 d2    	vpaddq zmm2,zmm2,zmm2
  4033ba:	62 f1 e5 48 d4 db    	vpaddq zmm3,zmm3,zmm3
  4033c0:	62 f1 dd 48 d4 e4    	vpaddq zmm4,zmm4,zmm4
  4033c6:	62 f1 d5 48 d4 ed    	vpaddq zmm5,zmm5,zmm5
  4033cc:	62 f1 cd 48 d4 f6    	vpaddq zmm6,zmm6,zmm6
  4033d2:	62 f1 c5 48 d4 ff    	vpaddq zmm7,zmm7,zmm7
  4033d8:	62 51 bd 48 d4 c0    	vpaddq zmm8,zmm8,zmm8
  4033de:	62 51 b5 48 d4 c9    	vpaddq zmm9,zmm9,zmm9
  4033e4:	62 51 ad 48 d4 d2    	vpaddq zmm10,zmm10,zmm10
  4033ea:	62 51 a5 48 d4 db    	vpaddq zmm11,zmm11,zmm11
  4033f0:	62 51 9d 48 d4 e4    	vpaddq zmm12,zmm12,zmm12
  4033f6:	62 51 95 48 d4 ed    	vpaddq zmm13,zmm13,zmm13
  4033fc:	62 51 8d 48 d4 f6    	vpaddq zmm14,zmm14,zmm14
  403402:	62 51 85 48 d4 ff    	vpaddq zmm15,zmm15,zmm15
  403408:	62 f1 fd 48 d4 c0    	vpaddq zmm0,zmm0,zmm0
  40340e:	62 f1 f5 48 d4 c9    	vpaddq zmm1,zmm1,zmm1
  403414:	62 f1 ed 48 d4 d2    	vpaddq zmm2,zmm2,zmm2
  40341a:	62 f1 e5 48 d4 db    	vpaddq zmm3,zmm3,zmm3
  403420:	62 f1 dd 48 d4 e4    	vpaddq zmm4,zmm4,zmm4
  403426:	62 f1 d5 48 d4 ed    	vpaddq zmm5,zmm5,zmm5
  40342c:	62 f1 cd 48 d4 f6    	vpaddq zmm6,zmm6,zmm6
  403432:	62 f1 c5 48 d4 ff    	vpaddq zmm7,zmm7,zmm7
  403438:	62 51 bd 48 d4 c0    	vpaddq zmm8,zmm8,zmm8
  40343e:	62 51 b5 48 d4 c9    	vpaddq zmm9,zmm9,zmm9
  403444:	62 51 ad 48 d4 d2    	vpaddq zmm10,zmm10,zmm10
  40344a:	62 51 a5 48 d4 db    	vpaddq zmm11,zmm11,zmm11
  403450:	62 51 9d 48 d4 e4    	vpaddq zmm12,zmm12,zmm12
  403456:	62 51 95 48 d4 ed    	vpaddq zmm13,zmm13,zmm13
  40345c:	62 51 8d 48 d4 f6    	vpaddq zmm14,zmm14,zmm14
  403462:	62 51 85 48 d4 ff    	vpaddq zmm15,zmm15,zmm15
  403468:	62 f1 fd 48 d4 c0    	vpaddq zmm0,zmm0,zmm0
  40346e:	62 f1 f5 48 d4 c9    	vpaddq zmm1,zmm1,zmm1
  403474:	62 f1 ed 48 d4 d2    	vpaddq zmm2,zmm2,zmm2
  40347a:	62 f1 e5 48 d4 db    	vpaddq zmm3,zmm3,zmm3
  403480:	62 f1 dd 48 d4 e4    	vpaddq zmm4,zmm4,zmm4
  403486:	62 f1 d5 48 d4 ed    	vpaddq zmm5,zmm5,zmm5
  40348c:	62 f1 cd 48 d4 f6    	vpaddq zmm6,zmm6,zmm6
  403492:	62 f1 c5 48 d4 ff    	vpaddq zmm7,zmm7,zmm7
  403498:	62 51 bd 48 d4 c0    	vpaddq zmm8,zmm8,zmm8
  40349e:	62 51 b5 48 d4 c9    	vpaddq zmm9,zmm9,zmm9
  4034a4:	62 51 ad 48 d4 d2    	vpaddq zmm10,zmm10,zmm10
  4034aa:	62 51 a5 48 d4 db    	vpaddq zmm11,zmm11,zmm11
  4034b0:	62 51 9d 48 d4 e4    	vpaddq zmm12,zmm12,zmm12
  4034b6:	62 51 95 48 d4 ed    	vpaddq zmm13,zmm13,zmm13
  4034bc:	62 51 8d 48 d4 f6    	vpaddq zmm14,zmm14,zmm14
  4034c2:	62 51 85 48 d4 ff    	vpaddq zmm15,zmm15,zmm15
  4034c8:	62 f1 fd 48 d4 c0    	vpaddq zmm0,zmm0,zmm0
  4034ce:	62 f1 f5 48 d4 c9    	vpaddq zmm1,zmm1,zmm1
  4034d4:	62 f1 ed 48 d4 d2    	vpaddq zmm2,zmm2,zmm2
  4034da:	62 f1 e5 48 d4 db    	vpaddq zmm3,zmm3,zmm3
  4034e0:	62 f1 dd 48 d4 e4    	vpaddq zmm4,zmm4,zmm4
  4034e6:	62 f1 d5 48 d4 ed    	vpaddq zmm5,zmm5,zmm5
  4034ec:	62 f1 cd 48 d4 f6    	vpaddq zmm6,zmm6,zmm6
  4034f2:	62 f1 c5 48 d4 ff    	vpaddq zmm7,zmm7,zmm7
  4034f8:	62 51 bd 48 d4 c0    	vpaddq zmm8,zmm8,zmm8
  4034fe:	62 51 b5 48 d4 c9    	vpaddq zmm9,zmm9,zmm9
  403504:	62 51 ad 48 d4 d2    	vpaddq zmm10,zmm10,zmm10
  40350a:	62 51 a5 48 d4 db    	vpaddq zmm11,zmm11,zmm11
  403510:	62 51 9d 48 d4 e4    	vpaddq zmm12,zmm12,zmm12
  403516:	62 51 95 48 d4 ed    	vpaddq zmm13,zmm13,zmm13
  40351c:	62 51 8d 48 d4 f6    	vpaddq zmm14,zmm14,zmm14
  403522:	62 51 85 48 d4 ff    	vpaddq zmm15,zmm15,zmm15
  403528:	62 f1 fd 48 d4 c0    	vpaddq zmm0,zmm0,zmm0
  40352e:	62 f1 f5 48 d4 c9    	vpaddq zmm1,zmm1,zmm1
  403534:	62 f1 ed 48 d4 d2    	vpaddq zmm2,zmm2,zmm2
  40353a:	62 f1 e5 48 d4 db    	vpaddq zmm3,zmm3,zmm3
  403540:	62 f1 dd 48 d4 e4    	vpaddq zmm4,zmm4,zmm4
  403546:	62 f1 d5 48 d4 ed    	vpaddq zmm5,zmm5,zmm5
  40354c:	62 f1 cd 48 d4 f6    	vpaddq zmm6,zmm6,zmm6
  403552:	62 f1 c5 48 d4 ff    	vpaddq zmm7,zmm7,zmm7
  403558:	62 51 bd 48 d4 c0    	vpaddq zmm8,zmm8,zmm8
  40355e:	62 51 b5 48 d4 c9    	vpaddq zmm9,zmm9,zmm9
  403564:	62 51 ad 48 d4 d2    	vpaddq zmm10,zmm10,zmm10
  40356a:	62 51 a5 48 d4 db    	vpaddq zmm11,zmm11,zmm11
  403570:	62 51 9d 48 d4 e4    	vpaddq zmm12,zmm12,zmm12
  403576:	62 51 95 48 d4 ed    	vpaddq zmm13,zmm13,zmm13
  40357c:	62 51 8d 48 d4 f6    	vpaddq zmm14,zmm14,zmm14
  403582:	62 51 85 48 d4 ff    	vpaddq zmm15,zmm15,zmm15
  403588:	ff c8                	dec    eax
  40358a:	0f 85 f8 fc ff ff    	jne    403288 <add+0x78>
  403590:	0f ae e8             	lfence 
  403593:	0f 31                	rdtsc  
  403595:	c5 f8 57 c0          	vxorps xmm0,xmm0,xmm0
  403599:	48 c1 e2 20          	shl    rdx,0x20
  40359d:	48 09 d0             	or     rax,rdx
  4035a0:	48 29 c8             	sub    rax,rcx
  4035a3:	62 f1 ff 08 7b c0    	vcvtusi2sd xmm0,xmm0,rax
  4035a9:	c5 fb 5e 05 4f 1b 00 	vdivsd xmm0,xmm0,QWORD PTR [rip+0x1b4f]        # 405100 <__dso_handle+0xf8>
  4035b0:	00 
  4035b1:	c5 f8 77             	vzeroupper 
  4035b4:	c3                   	ret    
  4035b5:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
  4035bc:	00 00 00 00 

00000000004035c0 <mix>:
  4035c0:	62 f1 7d 48 ef c0    	vpxord zmm0,zmm0,zmm0
  4035c6:	62 f1 75 48 ef c9    	vpxord zmm1,zmm1,zmm1
  4035cc:	62 f1 6d 48 ef d2    	vpxord zmm2,zmm2,zmm2
  4035d2:	62 f1 65 48 ef db    	vpxord zmm3,zmm3,zmm3
  4035d8:	62 f1 5d 48 ef e4    	vpxord zmm4,zmm4,zmm4
  4035de:	62 f1 55 48 ef ed    	vpxord zmm5,zmm5,zmm5
  4035e4:	62 f1 4d 48 ef f6    	vpxord zmm6,zmm6,zmm6
  4035ea:	62 f1 45 48 ef ff    	vpxord zmm7,zmm7,zmm7
  4035f0:	62 51 3d 48 ef c0    	vpxord zmm8,zmm8,zmm8
  4035f6:	62 51 35 48 ef c9    	vpxord zmm9,zmm9,zmm9
  4035fc:	62 51 2d 48 ef d2    	vpxord zmm10,zmm10,zmm10
  403602:	62 51 25 48 ef db    	vpxord zmm11,zmm11,zmm11
  403608:	62 51 1d 48 ef e4    	vpxord zmm12,zmm12,zmm12
  40360e:	62 51 15 48 ef ed    	vpxord zmm13,zmm13,zmm13
  403614:	62 51 0d 48 ef f6    	vpxord zmm14,zmm14,zmm14
  40361a:	62 51 05 48 ef ff    	vpxord zmm15,zmm15,zmm15
  403620:	0f ae e8             	lfence 
  403623:	0f 31                	rdtsc  
  403625:	48 89 c1             	mov    rcx,rax
  403628:	48 c1 e2 20          	shl    rdx,0x20
  40362c:	48 09 d1             	or     rcx,rdx
  40362f:	b8 a0 86 01 00       	mov    eax,0x186a0
  403634:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
  403638:	62 f3 7d 48 44 c0 10 	vpclmullqhqdq zmm0,zmm0,zmm0
  40363f:	62 f1 fd 48 f4 c0    	vpmuludq zmm0,zmm0,zmm0
  403645:	62 f3 75 48 44 c9 10 	vpclmullqhqdq zmm1,zmm1,zmm1
  40364c:	62 f1 f5 48 f4 c9    	vpmuludq zmm1,zmm1,zmm1
  403652:	62 f3 6d 48 44 d2 10 	vpclmullqhqdq zmm2,zmm2,zmm2
  403659:	62 f1 ed 48 f4 d2    	vpmuludq zmm2,zmm2,zmm2
  40365f:	62 f3 65 48 44 db 10 	vpclmullqhqdq zmm3,zmm3,zmm3
  403666:	62 f1 e5 48 f4 db    	vpmuludq zmm3,zmm3,zmm3
  40366c:	62 f3 5d 48 44 e4 10 	vpclmullqhqdq zmm4,zmm4,zmm4
  403673:	62 f1 dd 48 f4 e4    	vpmuludq zmm4,zmm4,zmm4
  403679:	62 f3 55 48 44 ed 10 	vpclmullqhqdq zmm5,zmm5,zmm5
  403680:	62 f1 d5 48 f4 ed    	vpmuludq zmm5,zmm5,zmm5
  403686:	62 f3 4d 48 44 f6 10 	vpclmullqhqdq zmm6,zmm6,zmm6
  40368d:	62 f1 cd 48 f4 f6    	vpmuludq zmm6,zmm6,zmm6
  403693:	62 f3 45 48 44 ff 10 	vpclmullqhqdq zmm7,zmm7,zmm7
  40369a:	62 f1 c5 48 f4 ff    	vpmuludq zmm7,zmm7,zmm7
  4036a0:	62 53 3d 48 44 c0 10 	vpclmullqhqdq zmm8,zmm8,zmm8
  4036a7:	62 51 bd 48 f4 c0    	vpmuludq zmm8,zmm8,zmm8
  4036ad:	62 53 35 48 44 c9 10 	vpclmullqhqdq zmm9,zmm9,zmm9
  4036b4:	62 51 b5 48 f4 c9    	vpmuludq zmm9,zmm9,zmm9
  4036ba:	62 53 2d 48 44 d2 10 	vpclmullqhqdq zmm10,zmm10,zmm10
  4036c1:	62 51 ad 48 f4 d2    	vpmuludq zmm10,zmm10,zmm10
  4036c7:	62 53 25 48 44 db 10 	vpclmullqhqdq zmm11,zmm11,zmm11
  4036ce:	62 51 a5 48 f4 db    	vpmuludq zmm11,zmm11,zmm11
  4036d4:	62 53 1d 48 44 e4 10 	vpclmullqhqdq zmm12,zmm12,zmm12
  4036db:	62 51 9d 48 f4 e4    	vpmuludq zmm12,zmm12,zmm12
  4036e1:	62 53 15 48 44 ed 10 	vpclmullqhqdq zmm13,zmm13,zmm13
  4036e8:	62 51 95 48 f4 ed    	vpmuludq zmm13,zmm13,zmm13
  4036ee:	62 53 0d 48 44 f6 10 	vpclmullqhqdq zmm14,zmm14,zmm14
  4036f5:	62 51 8d 48 f4 f6    	vpmuludq zmm14,zmm14,zmm14
  4036fb:	62 53 05 48 44 ff 10 	vpclmullqhqdq zmm15,zmm15,zmm15
  403702:	62 51 85 48 f4 ff    	vpmuludq zmm15,zmm15,zmm15
  403708:	62 f3 7d 48 44 c0 10 	vpclmullqhqdq zmm0,zmm0,zmm0
  40370f:	62 f1 fd 48 f4 c0    	vpmuludq zmm0,zmm0,zmm0
  403715:	62 f3 75 48 44 c9 10 	vpclmullqhqdq zmm1,zmm1,zmm1
  40371c:	62 f1 f5 48 f4 c9    	vpmuludq zmm1,zmm1,zmm1
  403722:	62 f3 6d 48 44 d2 10 	vpclmullqhqdq zmm2,zmm2,zmm2
  403729:	62 f1 ed 48 f4 d2    	vpmuludq zmm2,zmm2,zmm2
  40372f:	62 f3 65 48 44 db 10 	vpclmullqhqdq zmm3,zmm3,zmm3
  403736:	62 f1 e5 48 f4 db    	vpmuludq zmm3,zmm3,zmm3
  40373c:	62 f3 5d 48 44 e4 10 	vpclmullqhqdq zmm4,zmm4,zmm4
  403743:	62 f1 dd 48 f4 e4    	vpmuludq zmm4,zmm4,zmm4
  403749:	62 f3 55 48 44 ed 10 	vpclmullqhqdq zmm5,zmm5,zmm5
  403750:	62 f1 d5 48 f4 ed    	vpmuludq zmm5,zmm5,zmm5
  403756:	62 f3 4d 48 44 f6 10 	vpclmullqhqdq zmm6,zmm6,zmm6
  40375d:	62 f1 cd 48 f4 f6    	vpmuludq zmm6,zmm6,zmm6
  403763:	62 f3 45 48 44 ff 10 	vpclmullqhqdq zmm7,zmm7,zmm7
  40376a:	62 f1 c5 48 f4 ff    	vpmuludq zmm7,zmm7,zmm7
  403770:	62 53 3d 48 44 c0 10 	vpclmullqhqdq zmm8,zmm8,zmm8
  403777:	62 51 bd 48 f4 c0    	vpmuludq zmm8,zmm8,zmm8
  40377d:	62 53 35 48 44 c9 10 	vpclmullqhqdq zmm9,zmm9,zmm9
  403784:	62 51 b5 48 f4 c9    	vpmuludq zmm9,zmm9,zmm9
  40378a:	62 53 2d 48 44 d2 10 	vpclmullqhqdq zmm10,zmm10,zmm10
  403791:	62 51 ad 48 f4 d2    	vpmuludq zmm10,zmm10,zmm10
  403797:	62 53 25 48 44 db 10 	vpclmullqhqdq zmm11,zmm11,zmm11
  40379e:	62 51 a5 48 f4 db    	vpmuludq zmm11,zmm11,zmm11
  4037a4:	62 53 1d 48 44 e4 10 	vpclmullqhqdq zmm12,zmm12,zmm12
  4037ab:	62 51 9d 48 f4 e4    	vpmuludq zmm12,zmm12,zmm12
  4037b1:	62 53 15 48 44 ed 10 	vpclmullqhqdq zmm13,zmm13,zmm13
  4037b8:	62 51 95 48 f4 ed    	vpmuludq zmm13,zmm13,zmm13
  4037be:	62 53 0d 48 44 f6 10 	vpclmullqhqdq zmm14,zmm14,zmm14
  4037c5:	62 51 8d 48 f4 f6    	vpmuludq zmm14,zmm14,zmm14
  4037cb:	62 53 05 48 44 ff 10 	vpclmullqhqdq zmm15,zmm15,zmm15
  4037d2:	62 51 85 48 f4 ff    	vpmuludq zmm15,zmm15,zmm15
  4037d8:	62 f3 7d 48 44 c0 10 	vpclmullqhqdq zmm0,zmm0,zmm0
  4037df:	62 f1 fd 48 f4 c0    	vpmuludq zmm0,zmm0,zmm0
  4037e5:	62 f3 75 48 44 c9 10 	vpclmullqhqdq zmm1,zmm1,zmm1
  4037ec:	62 f1 f5 48 f4 c9    	vpmuludq zmm1,zmm1,zmm1
  4037f2:	62 f3 6d 48 44 d2 10 	vpclmullqhqdq zmm2,zmm2,zmm2
  4037f9:	62 f1 ed 48 f4 d2    	vpmuludq zmm2,zmm2,zmm2
  4037ff:	62 f3 65 48 44 db 10 	vpclmullqhqdq zmm3,zmm3,zmm3
  403806:	62 f1 e5 48 f4 db    	vpmuludq zmm3,zmm3,zmm3
  40380c:	62 f3 5d 48 44 e4 10 	vpclmullqhqdq zmm4,zmm4,zmm4
  403813:	62 f1 dd 48 f4 e4    	vpmuludq zmm4,zmm4,zmm4
  403819:	62 f3 55 48 44 ed 10 	vpclmullqhqdq zmm5,zmm5,zmm5
  403820:	62 f1 d5 48 f4 ed    	vpmuludq zmm5,zmm5,zmm5
  403826:	62 f3 4d 48 44 f6 10 	vpclmullqhqdq zmm6,zmm6,zmm6
  40382d:	62 f1 cd 48 f4 f6    	vpmuludq zmm6,zmm6,zmm6
  403833:	62 f3 45 48 44 ff 10 	vpclmullqhqdq zmm7,zmm7,zmm7
  40383a:	62 f1 c5 48 f4 ff    	vpmuludq zmm7,zmm7,zmm7
  403840:	62 53 3d 48 44 c0 10 	vpclmullqhqdq zmm8,zmm8,zmm8
  403847:	62 51 bd 48 f4 c0    	vpmuludq zmm8,zmm8,zmm8
  40384d:	62 53 35 48 44 c9 10 	vpclmullqhqdq zmm9,zmm9,zmm9
  403854:	62 51 b5 48 f4 c9    	vpmuludq zmm9,zmm9,zmm9
  40385a:	62 53 2d 48 44 d2 10 	vpclmullqhqdq zmm10,zmm10,zmm10
  403861:	62 51 ad 48 f4 d2    	vpmuludq zmm10,zmm10,zmm10
  403867:	62 53 25 48 44 db 10 	vpclmullqhqdq zmm11,zmm11,zmm11
  40386e:	62 51 a5 48 f4 db    	vpmuludq zmm11,zmm11,zmm11
  403874:	62 53 1d 48 44 e4 10 	vpclmullqhqdq zmm12,zmm12,zmm12
  40387b:	62 51 9d 48 f4 e4    	vpmuludq zmm12,zmm12,zmm12
  403881:	62 53 15 48 44 ed 10 	vpclmullqhqdq zmm13,zmm13,zmm13
  403888:	62 51 95 48 f4 ed    	vpmuludq zmm13,zmm13,zmm13
  40388e:	62 53 0d 48 44 f6 10 	vpclmullqhqdq zmm14,zmm14,zmm14
  403895:	62 51 8d 48 f4 f6    	vpmuludq zmm14,zmm14,zmm14
  40389b:	62 53 05 48 44 ff 10 	vpclmullqhqdq zmm15,zmm15,zmm15
  4038a2:	62 51 85 48 f4 ff    	vpmuludq zmm15,zmm15,zmm15
  4038a8:	62 f3 7d 48 44 c0 10 	vpclmullqhqdq zmm0,zmm0,zmm0
  4038af:	62 f1 fd 48 f4 c0    	vpmuludq zmm0,zmm0,zmm0
  4038b5:	62 f3 75 48 44 c9 10 	vpclmullqhqdq zmm1,zmm1,zmm1
  4038bc:	62 f1 f5 48 f4 c9    	vpmuludq zmm1,zmm1,zmm1
  4038c2:	62 f3 6d 48 44 d2 10 	vpclmullqhqdq zmm2,zmm2,zmm2
  4038c9:	62 f1 ed 48 f4 d2    	vpmuludq zmm2,zmm2,zmm2
  4038cf:	62 f3 65 48 44 db 10 	vpclmullqhqdq zmm3,zmm3,zmm3
  4038d6:	62 f1 e5 48 f4 db    	vpmuludq zmm3,zmm3,zmm3
  4038dc:	62 f3 5d 48 44 e4 10 	vpclmullqhqdq zmm4,zmm4,zmm4
  4038e3:	62 f1 dd 48 f4 e4    	vpmuludq zmm4,zmm4,zmm4
  4038e9:	62 f3 55 48 44 ed 10 	vpclmullqhqdq zmm5,zmm5,zmm5
  4038f0:	62 f1 d5 48 f4 ed    	vpmuludq zmm5,zmm5,zmm5
  4038f6:	62 f3 4d 48 44 f6 10 	vpclmullqhqdq zmm6,zmm6,zmm6
  4038fd:	62 f1 cd 48 f4 f6    	vpmuludq zmm6,zmm6,zmm6
  403903:	62 f3 45 48 44 ff 10 	vpclmullqhqdq zmm7,zmm7,zmm7
  40390a:	62 f1 c5 48 f4 ff    	vpmuludq zmm7,zmm7,zmm7
  403910:	62 53 3d 48 44 c0 10 	vpclmullqhqdq zmm8,zmm8,zmm8
  403917:	62 51 bd 48 f4 c0    	vpmuludq zmm8,zmm8,zmm8
  40391d:	62 53 35 48 44 c9 10 	vpclmullqhqdq zmm9,zmm9,zmm9
  403924:	62 51 b5 48 f4 c9    	vpmuludq zmm9,zmm9,zmm9
  40392a:	62 53 2d 48 44 d2 10 	vpclmullqhqdq zmm10,zmm10,zmm10
  403931:	62 51 ad 48 f4 d2    	vpmuludq zmm10,zmm10,zmm10
  403937:	62 53 25 48 44 db 10 	vpclmullqhqdq zmm11,zmm11,zmm11
  40393e:	62 51 a5 48 f4 db    	vpmuludq zmm11,zmm11,zmm11
  403944:	62 53 1d 48 44 e4 10 	vpclmullqhqdq zmm12,zmm12,zmm12
  40394b:	62 51 9d 48 f4 e4    	vpmuludq zmm12,zmm12,zmm12
  403951:	62 53 15 48 44 ed 10 	vpclmullqhqdq zmm13,zmm13,zmm13
  403958:	62 51 95 48 f4 ed    	vpmuludq zmm13,zmm13,zmm13
  40395e:	62 53 0d 48 44 f6 10 	vpclmullqhqdq zmm14,zmm14,zmm14
  403965:	62 51 8d 48 f4 f6    	vpmuludq zmm14,zmm14,zmm14
  40396b:	62 53 05 48 44 ff 10 	vpclmullqhqdq zmm15,zmm15,zmm15
  403972:	62 51 85 48 f4 ff    	vpmuludq zmm15,zmm15,zmm15
  403978:	62 f3 7d 48 44 c0 10 	vpclmullqhqdq zmm0,zmm0,zmm0
  40397f:	62 f1 fd 48 f4 c0    	vpmuludq zmm0,zmm0,zmm0
  403985:	62 f3 75 48 44 c9 10 	vpclmullqhqdq zmm1,zmm1,zmm1
  40398c:	62 f1 f5 48 f4 c9    	vpmuludq zmm1,zmm1,zmm1
  403992:	62 f3 6d 48 44 d2 10 	vpclmullqhqdq zmm2,zmm2,zmm2
  403999:	62 f1 ed 48 f4 d2    	vpmuludq zmm2,zmm2,zmm2
  40399f:	62 f3 65 48 44 db 10 	vpclmullqhqdq zmm3,zmm3,zmm3
  4039a6:	62 f1 e5 48 f4 db    	vpmuludq zmm3,zmm3,zmm3
  4039ac:	62 f3 5d 48 44 e4 10 	vpclmullqhqdq zmm4,zmm4,zmm4
  4039b3:	62 f1 dd 48 f4 e4    	vpmuludq zmm4,zmm4,zmm4
  4039b9:	62 f3 55 48 44 ed 10 	vpclmullqhqdq zmm5,zmm5,zmm5
  4039c0:	62 f1 d5 48 f4 ed    	vpmuludq zmm5,zmm5,zmm5
  4039c6:	62 f3 4d 48 44 f6 10 	vpclmullqhqdq zmm6,zmm6,zmm6
  4039cd:	62 f1 cd 48 f4 f6    	vpmuludq zmm6,zmm6,zmm6
  4039d3:	62 f3 45 48 44 ff 10 	vpclmullqhqdq zmm7,zmm7,zmm7
  4039da:	62 f1 c5 48 f4 ff    	vpmuludq zmm7,zmm7,zmm7
  4039e0:	62 53 3d 48 44 c0 10 	vpclmullqhqdq zmm8,zmm8,zmm8
  4039e7:	62 51 bd 48 f4 c0    	vpmuludq zmm8,zmm8,zmm8
  4039ed:	62 53 35 48 44 c9 10 	vpclmullqhqdq zmm9,zmm9,zmm9
  4039f4:	62 51 b5 48 f4 c9    	vpmuludq zmm9,zmm9,zmm9
  4039fa:	62 53 2d 48 44 d2 10 	vpclmullqhqdq zmm10,zmm10,zmm10
  403a01:	62 51 ad 48 f4 d2    	vpmuludq zmm10,zmm10,zmm10
  403a07:	62 53 25 48 44 db 10 	vpclmullqhqdq zmm11,zmm11,zmm11
  403a0e:	62 51 a5 48 f4 db    	vpmuludq zmm11,zmm11,zmm11
  403a14:	62 53 1d 48 44 e4 10 	vpclmullqhqdq zmm12,zmm12,zmm12
  403a1b:	62 51 9d 48 f4 e4    	vpmuludq zmm12,zmm12,zmm12
  403a21:	62 53 15 48 44 ed 10 	vpclmullqhqdq zmm13,zmm13,zmm13
  403a28:	62 51 95 48 f4 ed    	vpmuludq zmm13,zmm13,zmm13
  403a2e:	62 53 0d 48 44 f6 10 	vpclmullqhqdq zmm14,zmm14,zmm14
  403a35:	62 51 8d 48 f4 f6    	vpmuludq zmm14,zmm14,zmm14
  403a3b:	62 53 05 48 44 ff 10 	vpclmullqhqdq zmm15,zmm15,zmm15
  403a42:	62 51 85 48 f4 ff    	vpmuludq zmm15,zmm15,zmm15
  403a48:	62 f3 7d 48 44 c0 10 	vpclmullqhqdq zmm0,zmm0,zmm0
  403a4f:	62 f1 fd 48 f4 c0    	vpmuludq zmm0,zmm0,zmm0
  403a55:	62 f3 75 48 44 c9 10 	vpclmullqhqdq zmm1,zmm1,zmm1
  403a5c:	62 f1 f5 48 f4 c9    	vpmuludq zmm1,zmm1,zmm1
  403a62:	62 f3 6d 48 44 d2 10 	vpclmullqhqdq zmm2,zmm2,zmm2
  403a69:	62 f1 ed 48 f4 d2    	vpmuludq zmm2,zmm2,zmm2
  403a6f:	62 f3 65 48 44 db 10 	vpclmullqhqdq zmm3,zmm3,zmm3
  403a76:	62 f1 e5 48 f4 db    	vpmuludq zmm3,zmm3,zmm3
  403a7c:	62 f3 5d 48 44 e4 10 	vpclmullqhqdq zmm4,zmm4,zmm4
  403a83:	62 f1 dd 48 f4 e4    	vpmuludq zmm4,zmm4,zmm4
  403a89:	62 f3 55 48 44 ed 10 	vpclmullqhqdq zmm5,zmm5,zmm5
  403a90:	62 f1 d5 48 f4 ed    	vpmuludq zmm5,zmm5,zmm5
  403a96:	62 f3 4d 48 44 f6 10 	vpclmullqhqdq zmm6,zmm6,zmm6
  403a9d:	62 f1 cd 48 f4 f6    	vpmuludq zmm6,zmm6,zmm6
  403aa3:	62 f3 45 48 44 ff 10 	vpclmullqhqdq zmm7,zmm7,zmm7
  403aaa:	62 f1 c5 48 f4 ff    	vpmuludq zmm7,zmm7,zmm7
  403ab0:	62 53 3d 48 44 c0 10 	vpclmullqhqdq zmm8,zmm8,zmm8
  403ab7:	62 51 bd 48 f4 c0    	vpmuludq zmm8,zmm8,zmm8
  403abd:	62 53 35 48 44 c9 10 	vpclmullqhqdq zmm9,zmm9,zmm9
  403ac4:	62 51 b5 48 f4 c9    	vpmuludq zmm9,zmm9,zmm9
  403aca:	62 53 2d 48 44 d2 10 	vpclmullqhqdq zmm10,zmm10,zmm10
  403ad1:	62 51 ad 48 f4 d2    	vpmuludq zmm10,zmm10,zmm10
  403ad7:	62 53 25 48 44 db 10 	vpclmullqhqdq zmm11,zmm11,zmm11
  403ade:	62 51 a5 48 f4 db    	vpmuludq zmm11,zmm11,zmm11
  403ae4:	62 53 1d 48 44 e4 10 	vpclmullqhqdq zmm12,zmm12,zmm12
  403aeb:	62 51 9d 48 f4 e4    	vpmuludq zmm12,zmm12,zmm12
  403af1:	62 53 15 48 44 ed 10 	vpclmullqhqdq zmm13,zmm13,zmm13
  403af8:	62 51 95 48 f4 ed    	vpmuludq zmm13,zmm13,zmm13
  403afe:	62 53 0d 48 44 f6 10 	vpclmullqhqdq zmm14,zmm14,zmm14
  403b05:	62 51 8d 48 f4 f6    	vpmuludq zmm14,zmm14,zmm14
  403b0b:	62 53 05 48 44 ff 10 	vpclmullqhqdq zmm15,zmm15,zmm15
  403b12:	62 51 85 48 f4 ff    	vpmuludq zmm15,zmm15,zmm15
  403b18:	62 f3 7d 48 44 c0 10 	vpclmullqhqdq zmm0,zmm0,zmm0
  403b1f:	62 f1 fd 48 f4 c0    	vpmuludq zmm0,zmm0,zmm0
  403b25:	62 f3 75 48 44 c9 10 	vpclmullqhqdq zmm1,zmm1,zmm1
  403b2c:	62 f1 f5 48 f4 c9    	vpmuludq zmm1,zmm1,zmm1
  403b32:	62 f3 6d 48 44 d2 10 	vpclmullqhqdq zmm2,zmm2,zmm2
  403b39:	62 f1 ed 48 f4 d2    	vpmuludq zmm2,zmm2,zmm2
  403b3f:	62 f3 65 48 44 db 10 	vpclmullqhqdq zmm3,zmm3,zmm3
  403b46:	62 f1 e5 48 f4 db    	vpmuludq zmm3,zmm3,zmm3
  403b4c:	62 f3 5d 48 44 e4 10 	vpclmullqhqdq zmm4,zmm4,zmm4
  403b53:	62 f1 dd 48 f4 e4    	vpmuludq zmm4,zmm4,zmm4
  403b59:	62 f3 55 48 44 ed 10 	vpclmullqhqdq zmm5,zmm5,zmm5
  403b60:	62 f1 d5 48 f4 ed    	vpmuludq zmm5,zmm5,zmm5
  403b66:	62 f3 4d 48 44 f6 10 	vpclmullqhqdq zmm6,zmm6,zmm6
  403b6d:	62 f1 cd 48 f4 f6    	vpmuludq zmm6,zmm6,zmm6
  403b73:	62 f3 45 48 44 ff 10 	vpclmullqhqdq zmm7,zmm7,zmm7
  403b7a:	62 f1 c5 48 f4 ff    	vpmuludq zmm7,zmm7,zmm7
  403b80:	62 53 3d 48 44 c0 10 	vpclmullqhqdq zmm8,zmm8,zmm8
  403b87:	62 51 bd 48 f4 c0    	vpmuludq zmm8,zmm8,zmm8
  403b8d:	62 53 35 48 44 c9 10 	vpclmullqhqdq zmm9,zmm9,zmm9
  403b94:	62 51 b5 48 f4 c9    	vpmuludq zmm9,zmm9,zmm9
  403b9a:	62 53 2d 48 44 d2 10 	vpclmullqhqdq zmm10,zmm10,zmm10
  403ba1:	62 51 ad 48 f4 d2    	vpmuludq zmm10,zmm10,zmm10
  403ba7:	62 53 25 48 44 db 10 	vpclmullqhqdq zmm11,zmm11,zmm11
  403bae:	62 51 a5 48 f4 db    	vpmuludq zmm11,zmm11,zmm11
  403bb4:	62 53 1d 48 44 e4 10 	vpclmullqhqdq zmm12,zmm12,zmm12
  403bbb:	62 51 9d 48 f4 e4    	vpmuludq zmm12,zmm12,zmm12
  403bc1:	62 53 15 48 44 ed 10 	vpclmullqhqdq zmm13,zmm13,zmm13
  403bc8:	62 51 95 48 f4 ed    	vpmuludq zmm13,zmm13,zmm13
  403bce:	62 53 0d 48 44 f6 10 	vpclmullqhqdq zmm14,zmm14,zmm14
  403bd5:	62 51 8d 48 f4 f6    	vpmuludq zmm14,zmm14,zmm14
  403bdb:	62 53 05 48 44 ff 10 	vpclmullqhqdq zmm15,zmm15,zmm15
  403be2:	62 51 85 48 f4 ff    	vpmuludq zmm15,zmm15,zmm15
  403be8:	62 f3 7d 48 44 c0 10 	vpclmullqhqdq zmm0,zmm0,zmm0
  403bef:	62 f1 fd 48 f4 c0    	vpmuludq zmm0,zmm0,zmm0
  403bf5:	62 f3 75 48 44 c9 10 	vpclmullqhqdq zmm1,zmm1,zmm1
  403bfc:	62 f1 f5 48 f4 c9    	vpmuludq zmm1,zmm1,zmm1
  403c02:	62 f3 6d 48 44 d2 10 	vpclmullqhqdq zmm2,zmm2,zmm2
  403c09:	62 f1 ed 48 f4 d2    	vpmuludq zmm2,zmm2,zmm2
  403c0f:	62 f3 65 48 44 db 10 	vpclmullqhqdq zmm3,zmm3,zmm3
  403c16:	62 f1 e5 48 f4 db    	vpmuludq zmm3,zmm3,zmm3
  403c1c:	62 f3 5d 48 44 e4 10 	vpclmullqhqdq zmm4,zmm4,zmm4
  403c23:	62 f1 dd 48 f4 e4    	vpmuludq zmm4,zmm4,zmm4
  403c29:	62 f3 55 48 44 ed 10 	vpclmullqhqdq zmm5,zmm5,zmm5
  403c30:	62 f1 d5 48 f4 ed    	vpmuludq zmm5,zmm5,zmm5
  403c36:	62 f3 4d 48 44 f6 10 	vpclmullqhqdq zmm6,zmm6,zmm6
  403c3d:	62 f1 cd 48 f4 f6    	vpmuludq zmm6,zmm6,zmm6
  403c43:	62 f3 45 48 44 ff 10 	vpclmullqhqdq zmm7,zmm7,zmm7
  403c4a:	62 f1 c5 48 f4 ff    	vpmuludq zmm7,zmm7,zmm7
  403c50:	62 53 3d 48 44 c0 10 	vpclmullqhqdq zmm8,zmm8,zmm8
  403c57:	62 51 bd 48 f4 c0    	vpmuludq zmm8,zmm8,zmm8
  403c5d:	62 53 35 48 44 c9 10 	vpclmullqhqdq zmm9,zmm9,zmm9
  403c64:	62 51 b5 48 f4 c9    	vpmuludq zmm9,zmm9,zmm9
  403c6a:	62 53 2d 48 44 d2 10 	vpclmullqhqdq zmm10,zmm10,zmm10
  403c71:	62 51 ad 48 f4 d2    	vpmuludq zmm10,zmm10,zmm10
  403c77:	62 53 25 48 44 db 10 	vpclmullqhqdq zmm11,zmm11,zmm11
  403c7e:	62 51 a5 48 f4 db    	vpmuludq zmm11,zmm11,zmm11
  403c84:	62 53 1d 48 44 e4 10 	vpclmullqhqdq zmm12,zmm12,zmm12
  403c8b:	62 51 9d 48 f4 e4    	vpmuludq zmm12,zmm12,zmm12
  403c91:	62 53 15 48 44 ed 10 	vpclmullqhqdq zmm13,zmm13,zmm13
  403c98:	62 51 95 48 f4 ed    	vpmuludq zmm13,zmm13,zmm13
  403c9e:	62 53 0d 48 44 f6 10 	vpclmullqhqdq zmm14,zmm14,zmm14
  403ca5:	62 51 8d 48 f4 f6    	vpmuludq zmm14,zmm14,zmm14
  403cab:	62 53 05 48 44 ff 10 	vpclmullqhqdq zmm15,zmm15,zmm15
  403cb2:	62 51 85 48 f4 ff    	vpmuludq zmm15,zmm15,zmm15
  403cb8:	ff c8                	dec    eax
  403cba:	0f 85 78 f9 ff ff    	jne    403638 <mix+0x78>
  403cc0:	0f ae e8             	lfence 
  403cc3:	0f 31                	rdtsc  
  403cc5:	c5 f8 57 c0          	vxorps xmm0,xmm0,xmm0
  403cc9:	48 c1 e2 20          	shl    rdx,0x20
  403ccd:	48 09 d0             	or     rax,rdx
  403cd0:	48 29 c8             	sub    rax,rcx
  403cd3:	62 f1 ff 08 7b c0    	vcvtusi2sd xmm0,xmm0,rax
  403cd9:	c5 fb 5e 05 1f 14 00 	vdivsd xmm0,xmm0,QWORD PTR [rip+0x141f]        # 405100 <__dso_handle+0xf8>
  403ce0:	00 
  403ce1:	c5 f8 77             	vzeroupper 
  403ce4:	c3                   	ret    
  403ce5:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
  403cec:	00 00 00 00 

0000000000403cf0 <msh>:
  403cf0:	62 f1 7d 48 ef c0    	vpxord zmm0,zmm0,zmm0
  403cf6:	62 f1 75 48 ef c9    	vpxord zmm1,zmm1,zmm1
  403cfc:	62 f1 6d 48 ef d2    	vpxord zmm2,zmm2,zmm2
  403d02:	62 f1 65 48 ef db    	vpxord zmm3,zmm3,zmm3
  403d08:	62 f1 5d 48 ef e4    	vpxord zmm4,zmm4,zmm4
  403d0e:	62 f1 55 48 ef ed    	vpxord zmm5,zmm5,zmm5
  403d14:	62 f1 4d 48 ef f6    	vpxord zmm6,zmm6,zmm6
  403d1a:	62 f1 45 48 ef ff    	vpxord zmm7,zmm7,zmm7
  403d20:	62 51 3d 48 ef c0    	vpxord zmm8,zmm8,zmm8
  403d26:	62 51 35 48 ef c9    	vpxord zmm9,zmm9,zmm9
  403d2c:	62 51 2d 48 ef d2    	vpxord zmm10,zmm10,zmm10
  403d32:	62 51 25 48 ef db    	vpxord zmm11,zmm11,zmm11
  403d38:	62 51 1d 48 ef e4    	vpxord zmm12,zmm12,zmm12
  403d3e:	62 51 15 48 ef ed    	vpxord zmm13,zmm13,zmm13
  403d44:	62 51 0d 48 ef f6    	vpxord zmm14,zmm14,zmm14
  403d4a:	62 51 05 48 ef ff    	vpxord zmm15,zmm15,zmm15
  403d50:	0f ae e8             	lfence 
  403d53:	0f 31                	rdtsc  
  403d55:	48 89 c1             	mov    rcx,rax
  403d58:	48 c1 e2 20          	shl    rdx,0x20
  403d5c:	48 09 d1             	or     rcx,rdx
  403d5f:	b8 a0 86 01 00       	mov    eax,0x186a0
  403d64:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
  403d68:	62 f1 fd 48 f4 c0    	vpmuludq zmm0,zmm0,zmm0
  403d6e:	62 f1 fd 48 73 d0 20 	vpsrlq zmm0,zmm0,0x20
  403d75:	62 f1 f5 48 f4 c9    	vpmuludq zmm1,zmm1,zmm1
  403d7b:	62 f1 f5 48 73 d1 20 	vpsrlq zmm1,zmm1,0x20
  403d82:	62 f1 ed 48 f4 d2    	vpmuludq zmm2,zmm2,zmm2
  403d88:	62 f1 ed 48 73 d2 20 	vpsrlq zmm2,zmm2,0x20
  403d8f:	62 f1 e5 48 f4 db    	vpmuludq zmm3,zmm3,zmm3
  403d95:	62 f1 e5 48 73 d3 20 	vpsrlq zmm3,zmm3,0x20
  403d9c:	62 f1 dd 48 f4 e4    	vpmuludq zmm4,zmm4,zmm4
  403da2:	62 f1 dd 48 73 d4 20 	vpsrlq zmm4,zmm4,0x20
  403da9:	62 f1 d5 48 f4 ed    	vpmuludq zmm5,zmm5,zmm5
  403daf:	62 f1 d5 48 73 d5 20 	vpsrlq zmm5,zmm5,0x20
  403db6:	62 f1 cd 48 f4 f6    	vpmuludq zmm6,zmm6,zmm6
  403dbc:	62 f1 cd 48 73 d6 20 	vpsrlq zmm6,zmm6,0x20
  403dc3:	62 f1 c5 48 f4 ff    	vpmuludq zmm7,zmm7,zmm7
  403dc9:	62 f1 c5 48 73 d7 20 	vpsrlq zmm7,zmm7,0x20
  403dd0:	62 51 bd 48 f4 c0    	vpmuludq zmm8,zmm8,zmm8
  403dd6:	62 d1 bd 48 73 d0 20 	vpsrlq zmm8,zmm8,0x20
  403ddd:	62 51 b5 48 f4 c9    	vpmuludq zmm9,zmm9,zmm9
  403de3:	62 d1 b5 48 73 d1 20 	vpsrlq zmm9,zmm9,0x20
  403dea:	62 51 ad 48 f4 d2    	vpmuludq zmm10,zmm10,zmm10
  403df0:	62 d1 ad 48 73 d2 20 	vpsrlq zmm10,zmm10,0x20
  403df7:	62 51 a5 48 f4 db    	vpmuludq zmm11,zmm11,zmm11
  403dfd:	62 d1 a5 48 73 d3 20 	vpsrlq zmm11,zmm11,0x20
  403e04:	62 51 9d 48 f4 e4    	vpmuludq zmm12,zmm12,zmm12
  403e0a:	62 d1 9d 48 73 d4 20 	vpsrlq zmm12,zmm12,0x20
  403e11:	62 51 95 48 f4 ed    	vpmuludq zmm13,zmm13,zmm13
  403e17:	62 d1 95 48 73 d5 20 	vpsrlq zmm13,zmm13,0x20
  403e1e:	62 51 8d 48 f4 f6    	vpmuludq zmm14,zmm14,zmm14
  403e24:	62 d1 8d 48 73 d6 20 	vpsrlq zmm14,zmm14,0x20
  403e2b:	62 51 85 48 f4 ff    	vpmuludq zmm15,zmm15,zmm15
  403e31:	62 d1 85 48 73 d7 20 	vpsrlq zmm15,zmm15,0x20
  403e38:	62 f1 fd 48 f4 c0    	vpmuludq zmm0,zmm0,zmm0
  403e3e:	62 f1 fd 48 73 d0 20 	vpsrlq zmm0,zmm0,0x20
  403e45:	62 f1 f5 48 f4 c9    	vpmuludq zmm1,zmm1,zmm1
  403e4b:	62 f1 f5 48 73 d1 20 	vpsrlq zmm1,zmm1,0x20
  403e52:	62 f1 ed 48 f4 d2    	vpmuludq zmm2,zmm2,zmm2
  403e58:	62 f1 ed 48 73 d2 20 	vpsrlq zmm2,zmm2,0x20
  403e5f:	62 f1 e5 48 f4 db    	vpmuludq zmm3,zmm3,zmm3
  403e65:	62 f1 e5 48 73 d3 20 	vpsrlq zmm3,zmm3,0x20
  403e6c:	62 f1 dd 48 f4 e4    	vpmuludq zmm4,zmm4,zmm4
  403e72:	62 f1 dd 48 73 d4 20 	vpsrlq zmm4,zmm4,0x20
  403e79:	62 f1 d5 48 f4 ed    	vpmuludq zmm5,zmm5,zmm5
  403e7f:	62 f1 d5 48 73 d5 20 	vpsrlq zmm5,zmm5,0x20
  403e86:	62 f1 cd 48 f4 f6    	vpmuludq zmm6,zmm6,zmm6
  403e8c:	62 f1 cd 48 73 d6 20 	vpsrlq zmm6,zmm6,0x20
  403e93:	62 f1 c5 48 f4 ff    	vpmuludq zmm7,zmm7,zmm7
  403e99:	62 f1 c5 48 73 d7 20 	vpsrlq zmm7,zmm7,0x20
  403ea0:	62 51 bd 48 f4 c0    	vpmuludq zmm8,zmm8,zmm8
  403ea6:	62 d1 bd 48 73 d0 20 	vpsrlq zmm8,zmm8,0x20
  403ead:	62 51 b5 48 f4 c9    	vpmuludq zmm9,zmm9,zmm9
  403eb3:	62 d1 b5 48 73 d1 20 	vpsrlq zmm9,zmm9,0x20
  403eba:	62 51 ad 48 f4 d2    	vpmuludq zmm10,zmm10,zmm10
  403ec0:	62 d1 ad 48 73 d2 20 	vpsrlq zmm10,zmm10,0x20
  403ec7:	62 51 a5 48 f4 db    	vpmuludq zmm11,zmm11,zmm11
  403ecd:	62 d1 a5 48 73 d3 20 	vpsrlq zmm11,zmm11,0x20
  403ed4:	62 51 9d 48 f4 e4    	vpmuludq zmm12,zmm12,zmm12
  403eda:	62 d1 9d 48 73 d4 20 	vpsrlq zmm12,zmm12,0x20
  403ee1:	62 51 95 48 f4 ed    	vpmuludq zmm13,zmm13,zmm13
  403ee7:	62 d1 95 48 73 d5 20 	vpsrlq zmm13,zmm13,0x20
  403eee:	62 51 8d 48 f4 f6    	vpmuludq zmm14,zmm14,zmm14
  403ef4:	62 d1 8d 48 73 d6 20 	vpsrlq zmm14,zmm14,0x20
  403efb:	62 51 85 48 f4 ff    	vpmuludq zmm15,zmm15,zmm15
  403f01:	62 d1 85 48 73 d7 20 	vpsrlq zmm15,zmm15,0x20
  403f08:	62 f1 fd 48 f4 c0    	vpmuludq zmm0,zmm0,zmm0
  403f0e:	62 f1 fd 48 73 d0 20 	vpsrlq zmm0,zmm0,0x20
  403f15:	62 f1 f5 48 f4 c9    	vpmuludq zmm1,zmm1,zmm1
  403f1b:	62 f1 f5 48 73 d1 20 	vpsrlq zmm1,zmm1,0x20
  403f22:	62 f1 ed 48 f4 d2    	vpmuludq zmm2,zmm2,zmm2
  403f28:	62 f1 ed 48 73 d2 20 	vpsrlq zmm2,zmm2,0x20
  403f2f:	62 f1 e5 48 f4 db    	vpmuludq zmm3,zmm3,zmm3
  403f35:	62 f1 e5 48 73 d3 20 	vpsrlq zmm3,zmm3,0x20
  403f3c:	62 f1 dd 48 f4 e4    	vpmuludq zmm4,zmm4,zmm4
  403f42:	62 f1 dd 48 73 d4 20 	vpsrlq zmm4,zmm4,0x20
  403f49:	62 f1 d5 48 f4 ed    	vpmuludq zmm5,zmm5,zmm5
  403f4f:	62 f1 d5 48 73 d5 20 	vpsrlq zmm5,zmm5,0x20
  403f56:	62 f1 cd 48 f4 f6    	vpmuludq zmm6,zmm6,zmm6
  403f5c:	62 f1 cd 48 73 d6 20 	vpsrlq zmm6,zmm6,0x20
  403f63:	62 f1 c5 48 f4 ff    	vpmuludq zmm7,zmm7,zmm7
  403f69:	62 f1 c5 48 73 d7 20 	vpsrlq zmm7,zmm7,0x20
  403f70:	62 51 bd 48 f4 c0    	vpmuludq zmm8,zmm8,zmm8
  403f76:	62 d1 bd 48 73 d0 20 	vpsrlq zmm8,zmm8,0x20
  403f7d:	62 51 b5 48 f4 c9    	vpmuludq zmm9,zmm9,zmm9
  403f83:	62 d1 b5 48 73 d1 20 	vpsrlq zmm9,zmm9,0x20
  403f8a:	62 51 ad 48 f4 d2    	vpmuludq zmm10,zmm10,zmm10
  403f90:	62 d1 ad 48 73 d2 20 	vpsrlq zmm10,zmm10,0x20
  403f97:	62 51 a5 48 f4 db    	vpmuludq zmm11,zmm11,zmm11
  403f9d:	62 d1 a5 48 73 d3 20 	vpsrlq zmm11,zmm11,0x20
  403fa4:	62 51 9d 48 f4 e4    	vpmuludq zmm12,zmm12,zmm12
  403faa:	62 d1 9d 48 73 d4 20 	vpsrlq zmm12,zmm12,0x20
  403fb1:	62 51 95 48 f4 ed    	vpmuludq zmm13,zmm13,zmm13
  403fb7:	62 d1 95 48 73 d5 20 	vpsrlq zmm13,zmm13,0x20
  403fbe:	62 51 8d 48 f4 f6    	vpmuludq zmm14,zmm14,zmm14
  403fc4:	62 d1 8d 48 73 d6 20 	vpsrlq zmm14,zmm14,0x20
  403fcb:	62 51 85 48 f4 ff    	vpmuludq zmm15,zmm15,zmm15
  403fd1:	62 d1 85 48 73 d7 20 	vpsrlq zmm15,zmm15,0x20
  403fd8:	62 f1 fd 48 f4 c0    	vpmuludq zmm0,zmm0,zmm0
  403fde:	62 f1 fd 48 73 d0 20 	vpsrlq zmm0,zmm0,0x20
  403fe5:	62 f1 f5 48 f4 c9    	vpmuludq zmm1,zmm1,zmm1
  403feb:	62 f1 f5 48 73 d1 20 	vpsrlq zmm1,zmm1,0x20
  403ff2:	62 f1 ed 48 f4 d2    	vpmuludq zmm2,zmm2,zmm2
  403ff8:	62 f1 ed 48 73 d2 20 	vpsrlq zmm2,zmm2,0x20
  403fff:	62 f1 e5 48 f4 db    	vpmuludq zmm3,zmm3,zmm3
  404005:	62 f1 e5 48 73 d3 20 	vpsrlq zmm3,zmm3,0x20
  40400c:	62 f1 dd 48 f4 e4    	vpmuludq zmm4,zmm4,zmm4
  404012:	62 f1 dd 48 73 d4 20 	vpsrlq zmm4,zmm4,0x20
  404019:	62 f1 d5 48 f4 ed    	vpmuludq zmm5,zmm5,zmm5
  40401f:	62 f1 d5 48 73 d5 20 	vpsrlq zmm5,zmm5,0x20
  404026:	62 f1 cd 48 f4 f6    	vpmuludq zmm6,zmm6,zmm6
  40402c:	62 f1 cd 48 73 d6 20 	vpsrlq zmm6,zmm6,0x20
  404033:	62 f1 c5 48 f4 ff    	vpmuludq zmm7,zmm7,zmm7
  404039:	62 f1 c5 48 73 d7 20 	vpsrlq zmm7,zmm7,0x20
  404040:	62 51 bd 48 f4 c0    	vpmuludq zmm8,zmm8,zmm8
  404046:	62 d1 bd 48 73 d0 20 	vpsrlq zmm8,zmm8,0x20
  40404d:	62 51 b5 48 f4 c9    	vpmuludq zmm9,zmm9,zmm9
  404053:	62 d1 b5 48 73 d1 20 	vpsrlq zmm9,zmm9,0x20
  40405a:	62 51 ad 48 f4 d2    	vpmuludq zmm10,zmm10,zmm10
  404060:	62 d1 ad 48 73 d2 20 	vpsrlq zmm10,zmm10,0x20
  404067:	62 51 a5 48 f4 db    	vpmuludq zmm11,zmm11,zmm11
  40406d:	62 d1 a5 48 73 d3 20 	vpsrlq zmm11,zmm11,0x20
  404074:	62 51 9d 48 f4 e4    	vpmuludq zmm12,zmm12,zmm12
  40407a:	62 d1 9d 48 73 d4 20 	vpsrlq zmm12,zmm12,0x20
  404081:	62 51 95 48 f4 ed    	vpmuludq zmm13,zmm13,zmm13
  404087:	62 d1 95 48 73 d5 20 	vpsrlq zmm13,zmm13,0x20
  40408e:	62 51 8d 48 f4 f6    	vpmuludq zmm14,zmm14,zmm14
  404094:	62 d1 8d 48 73 d6 20 	vpsrlq zmm14,zmm14,0x20
  40409b:	62 51 85 48 f4 ff    	vpmuludq zmm15,zmm15,zmm15
  4040a1:	62 d1 85 48 73 d7 20 	vpsrlq zmm15,zmm15,0x20
  4040a8:	62 f1 fd 48 f4 c0    	vpmuludq zmm0,zmm0,zmm0
  4040ae:	62 f1 fd 48 73 d0 20 	vpsrlq zmm0,zmm0,0x20
  4040b5:	62 f1 f5 48 f4 c9    	vpmuludq zmm1,zmm1,zmm1
  4040bb:	62 f1 f5 48 73 d1 20 	vpsrlq zmm1,zmm1,0x20
  4040c2:	62 f1 ed 48 f4 d2    	vpmuludq zmm2,zmm2,zmm2
  4040c8:	62 f1 ed 48 73 d2 20 	vpsrlq zmm2,zmm2,0x20
  4040cf:	62 f1 e5 48 f4 db    	vpmuludq zmm3,zmm3,zmm3
  4040d5:	62 f1 e5 48 73 d3 20 	vpsrlq zmm3,zmm3,0x20
  4040dc:	62 f1 dd 48 f4 e4    	vpmuludq zmm4,zmm4,zmm4
  4040e2:	62 f1 dd 48 73 d4 20 	vpsrlq zmm4,zmm4,0x20
  4040e9:	62 f1 d5 48 f4 ed    	vpmuludq zmm5,zmm5,zmm5
  4040ef:	62 f1 d5 48 73 d5 20 	vpsrlq zmm5,zmm5,0x20
  4040f6:	62 f1 cd 48 f4 f6    	vpmuludq zmm6,zmm6,zmm6
  4040fc:	62 f1 cd 48 73 d6 20 	vpsrlq zmm6,zmm6,0x20
  404103:	62 f1 c5 48 f4 ff    	vpmuludq zmm7,zmm7,zmm7
  404109:	62 f1 c5 48 73 d7 20 	vpsrlq zmm7,zmm7,0x20
  404110:	62 51 bd 48 f4 c0    	vpmuludq zmm8,zmm8,zmm8
  404116:	62 d1 bd 48 73 d0 20 	vpsrlq zmm8,zmm8,0x20
  40411d:	62 51 b5 48 f4 c9    	vpmuludq zmm9,zmm9,zmm9
  404123:	62 d1 b5 48 73 d1 20 	vpsrlq zmm9,zmm9,0x20
  40412a:	62 51 ad 48 f4 d2    	vpmuludq zmm10,zmm10,zmm10
  404130:	62 d1 ad 48 73 d2 20 	vpsrlq zmm10,zmm10,0x20
  404137:	62 51 a5 48 f4 db    	vpmuludq zmm11,zmm11,zmm11
  40413d:	62 d1 a5 48 73 d3 20 	vpsrlq zmm11,zmm11,0x20
  404144:	62 51 9d 48 f4 e4    	vpmuludq zmm12,zmm12,zmm12
  40414a:	62 d1 9d 48 73 d4 20 	vpsrlq zmm12,zmm12,0x20
  404151:	62 51 95 48 f4 ed    	vpmuludq zmm13,zmm13,zmm13
  404157:	62 d1 95 48 73 d5 20 	vpsrlq zmm13,zmm13,0x20
  40415e:	62 51 8d 48 f4 f6    	vpmuludq zmm14,zmm14,zmm14
  404164:	62 d1 8d 48 73 d6 20 	vpsrlq zmm14,zmm14,0x20
  40416b:	62 51 85 48 f4 ff    	vpmuludq zmm15,zmm15,zmm15
  404171:	62 d1 85 48 73 d7 20 	vpsrlq zmm15,zmm15,0x20
  404178:	62 f1 fd 48 f4 c0    	vpmuludq zmm0,zmm0,zmm0
  40417e:	62 f1 fd 48 73 d0 20 	vpsrlq zmm0,zmm0,0x20
  404185:	62 f1 f5 48 f4 c9    	vpmuludq zmm1,zmm1,zmm1
  40418b:	62 f1 f5 48 73 d1 20 	vpsrlq zmm1,zmm1,0x20
  404192:	62 f1 ed 48 f4 d2    	vpmuludq zmm2,zmm2,zmm2
  404198:	62 f1 ed 48 73 d2 20 	vpsrlq zmm2,zmm2,0x20
  40419f:	62 f1 e5 48 f4 db    	vpmuludq zmm3,zmm3,zmm3
  4041a5:	62 f1 e5 48 73 d3 20 	vpsrlq zmm3,zmm3,0x20
  4041ac:	62 f1 dd 48 f4 e4    	vpmuludq zmm4,zmm4,zmm4
  4041b2:	62 f1 dd 48 73 d4 20 	vpsrlq zmm4,zmm4,0x20
  4041b9:	62 f1 d5 48 f4 ed    	vpmuludq zmm5,zmm5,zmm5
  4041bf:	62 f1 d5 48 73 d5 20 	vpsrlq zmm5,zmm5,0x20
  4041c6:	62 f1 cd 48 f4 f6    	vpmuludq zmm6,zmm6,zmm6
  4041cc:	62 f1 cd 48 73 d6 20 	vpsrlq zmm6,zmm6,0x20
  4041d3:	62 f1 c5 48 f4 ff    	vpmuludq zmm7,zmm7,zmm7
  4041d9:	62 f1 c5 48 73 d7 20 	vpsrlq zmm7,zmm7,0x20
  4041e0:	62 51 bd 48 f4 c0    	vpmuludq zmm8,zmm8,zmm8
  4041e6:	62 d1 bd 48 73 d0 20 	vpsrlq zmm8,zmm8,0x20
  4041ed:	62 51 b5 48 f4 c9    	vpmuludq zmm9,zmm9,zmm9
  4041f3:	62 d1 b5 48 73 d1 20 	vpsrlq zmm9,zmm9,0x20
  4041fa:	62 51 ad 48 f4 d2    	vpmuludq zmm10,zmm10,zmm10
  404200:	62 d1 ad 48 73 d2 20 	vpsrlq zmm10,zmm10,0x20
  404207:	62 51 a5 48 f4 db    	vpmuludq zmm11,zmm11,zmm11
  40420d:	62 d1 a5 48 73 d3 20 	vpsrlq zmm11,zmm11,0x20
  404214:	62 51 9d 48 f4 e4    	vpmuludq zmm12,zmm12,zmm12
  40421a:	62 d1 9d 48 73 d4 20 	vpsrlq zmm12,zmm12,0x20
  404221:	62 51 95 48 f4 ed    	vpmuludq zmm13,zmm13,zmm13
  404227:	62 d1 95 48 73 d5 20 	vpsrlq zmm13,zmm13,0x20
  40422e:	62 51 8d 48 f4 f6    	vpmuludq zmm14,zmm14,zmm14
  404234:	62 d1 8d 48 73 d6 20 	vpsrlq zmm14,zmm14,0x20
  40423b:	62 51 85 48 f4 ff    	vpmuludq zmm15,zmm15,zmm15
  404241:	62 d1 85 48 73 d7 20 	vpsrlq zmm15,zmm15,0x20
  404248:	62 f1 fd 48 f4 c0    	vpmuludq zmm0,zmm0,zmm0
  40424e:	62 f1 fd 48 73 d0 20 	vpsrlq zmm0,zmm0,0x20
  404255:	62 f1 f5 48 f4 c9    	vpmuludq zmm1,zmm1,zmm1
  40425b:	62 f1 f5 48 73 d1 20 	vpsrlq zmm1,zmm1,0x20
  404262:	62 f1 ed 48 f4 d2    	vpmuludq zmm2,zmm2,zmm2
  404268:	62 f1 ed 48 73 d2 20 	vpsrlq zmm2,zmm2,0x20
  40426f:	62 f1 e5 48 f4 db    	vpmuludq zmm3,zmm3,zmm3
  404275:	62 f1 e5 48 73 d3 20 	vpsrlq zmm3,zmm3,0x20
  40427c:	62 f1 dd 48 f4 e4    	vpmuludq zmm4,zmm4,zmm4
  404282:	62 f1 dd 48 73 d4 20 	vpsrlq zmm4,zmm4,0x20
  404289:	62 f1 d5 48 f4 ed    	vpmuludq zmm5,zmm5,zmm5
  40428f:	62 f1 d5 48 73 d5 20 	vpsrlq zmm5,zmm5,0x20
  404296:	62 f1 cd 48 f4 f6    	vpmuludq zmm6,zmm6,zmm6
  40429c:	62 f1 cd 48 73 d6 20 	vpsrlq zmm6,zmm6,0x20
  4042a3:	62 f1 c5 48 f4 ff    	vpmuludq zmm7,zmm7,zmm7
  4042a9:	62 f1 c5 48 73 d7 20 	vpsrlq zmm7,zmm7,0x20
  4042b0:	62 51 bd 48 f4 c0    	vpmuludq zmm8,zmm8,zmm8
  4042b6:	62 d1 bd 48 73 d0 20 	vpsrlq zmm8,zmm8,0x20
  4042bd:	62 51 b5 48 f4 c9    	vpmuludq zmm9,zmm9,zmm9
  4042c3:	62 d1 b5 48 73 d1 20 	vpsrlq zmm9,zmm9,0x20
  4042ca:	62 51 ad 48 f4 d2    	vpmuludq zmm10,zmm10,zmm10
  4042d0:	62 d1 ad 48 73 d2 20 	vpsrlq zmm10,zmm10,0x20
  4042d7:	62 51 a5 48 f4 db    	vpmuludq zmm11,zmm11,zmm11
  4042dd:	62 d1 a5 48 73 d3 20 	vpsrlq zmm11,zmm11,0x20
  4042e4:	62 51 9d 48 f4 e4    	vpmuludq zmm12,zmm12,zmm12
  4042ea:	62 d1 9d 48 73 d4 20 	vpsrlq zmm12,zmm12,0x20
  4042f1:	62 51 95 48 f4 ed    	vpmuludq zmm13,zmm13,zmm13
  4042f7:	62 d1 95 48 73 d5 20 	vpsrlq zmm13,zmm13,0x20
  4042fe:	62 51 8d 48 f4 f6    	vpmuludq zmm14,zmm14,zmm14
  404304:	62 d1 8d 48 73 d6 20 	vpsrlq zmm14,zmm14,0x20
  40430b:	62 51 85 48 f4 ff    	vpmuludq zmm15,zmm15,zmm15
  404311:	62 d1 85 48 73 d7 20 	vpsrlq zmm15,zmm15,0x20
  404318:	62 f1 fd 48 f4 c0    	vpmuludq zmm0,zmm0,zmm0
  40431e:	62 f1 fd 48 73 d0 20 	vpsrlq zmm0,zmm0,0x20
  404325:	62 f1 f5 48 f4 c9    	vpmuludq zmm1,zmm1,zmm1
  40432b:	62 f1 f5 48 73 d1 20 	vpsrlq zmm1,zmm1,0x20
  404332:	62 f1 ed 48 f4 d2    	vpmuludq zmm2,zmm2,zmm2
  404338:	62 f1 ed 48 73 d2 20 	vpsrlq zmm2,zmm2,0x20
  40433f:	62 f1 e5 48 f4 db    	vpmuludq zmm3,zmm3,zmm3
  404345:	62 f1 e5 48 73 d3 20 	vpsrlq zmm3,zmm3,0x20
  40434c:	62 f1 dd 48 f4 e4    	vpmuludq zmm4,zmm4,zmm4
  404352:	62 f1 dd 48 73 d4 20 	vpsrlq zmm4,zmm4,0x20
  404359:	62 f1 d5 48 f4 ed    	vpmuludq zmm5,zmm5,zmm5
  40435f:	62 f1 d5 48 73 d5 20 	vpsrlq zmm5,zmm5,0x20
  404366:	62 f1 cd 48 f4 f6    	vpmuludq zmm6,zmm6,zmm6
  40436c:	62 f1 cd 48 73 d6 20 	vpsrlq zmm6,zmm6,0x20
  404373:	62 f1 c5 48 f4 ff    	vpmuludq zmm7,zmm7,zmm7
  404379:	62 f1 c5 48 73 d7 20 	vpsrlq zmm7,zmm7,0x20
  404380:	62 51 bd 48 f4 c0    	vpmuludq zmm8,zmm8,zmm8
  404386:	62 d1 bd 48 73 d0 20 	vpsrlq zmm8,zmm8,0x20
  40438d:	62 51 b5 48 f4 c9    	vpmuludq zmm9,zmm9,zmm9
  404393:	62 d1 b5 48 73 d1 20 	vpsrlq zmm9,zmm9,0x20
  40439a:	62 51 ad 48 f4 d2    	vpmuludq zmm10,zmm10,zmm10
  4043a0:	62 d1 ad 48 73 d2 20 	vpsrlq zmm10,zmm10,0x20
  4043a7:	62 51 a5 48 f4 db    	vpmuludq zmm11,zmm11,zmm11
  4043ad:	62 d1 a5 48 73 d3 20 	vpsrlq zmm11,zmm11,0x20
  4043b4:	62 51 9d 48 f4 e4    	vpmuludq zmm12,zmm12,zmm12
  4043ba:	62 d1 9d 48 73 d4 20 	vpsrlq zmm12,zmm12,0x20
  4043c1:	62 51 95 48 f4 ed    	vpmuludq zmm13,zmm13,zmm13
  4043c7:	62 d1 95 48 73 d5 20 	vpsrlq zmm13,zmm13,0x20
  4043ce:	62 51 8d 48 f4 f6    	vpmuludq zmm14,zmm14,zmm14
  4043d4:	62 d1 8d 48 73 d6 20 	vpsrlq zmm14,zmm14,0x20
  4043db:	62 51 85 48 f4 ff    	vpmuludq zmm15,zmm15,zmm15
  4043e1:	62 d1 85 48 73 d7 20 	vpsrlq zmm15,zmm15,0x20
  4043e8:	ff c8                	dec    eax
  4043ea:	0f 85 78 f9 ff ff    	jne    403d68 <msh+0x78>
  4043f0:	0f ae e8             	lfence 
  4043f3:	0f 31                	rdtsc  
  4043f5:	c5 f8 57 c0          	vxorps xmm0,xmm0,xmm0
  4043f9:	48 c1 e2 20          	shl    rdx,0x20
  4043fd:	48 09 d0             	or     rax,rdx
  404400:	48 29 c8             	sub    rax,rcx
  404403:	62 f1 ff 08 7b c0    	vcvtusi2sd xmm0,xmm0,rax
  404409:	c5 fb 5e 05 ef 0c 00 	vdivsd xmm0,xmm0,QWORD PTR [rip+0xcef]        # 405100 <__dso_handle+0xf8>
  404410:	00 
  404411:	c5 f8 77             	vzeroupper 
  404414:	c3                   	ret    

Disassembly of section .fini:

0000000000404418 <_fini>:
  404418:	f3 0f 1e fa          	endbr64 
  40441c:	48 83 ec 08          	sub    rsp,0x8
  404420:	48 83 c4 08          	add    rsp,0x8
  404424:	c3                   	ret    
