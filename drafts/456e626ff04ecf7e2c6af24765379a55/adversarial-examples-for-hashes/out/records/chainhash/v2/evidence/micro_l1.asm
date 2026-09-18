
experiments/micro_l1:     file format elf64-x86-64


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

0000000000401030 <free@plt>:
  401030:	ff 25 e2 5f 00 00    	jmp    QWORD PTR [rip+0x5fe2]        # 407018 <free@GLIBC_2.2.5>
  401036:	68 00 00 00 00       	push   0x0
  40103b:	e9 e0 ff ff ff       	jmp    401020 <.plt>

0000000000401040 <puts@plt>:
  401040:	ff 25 da 5f 00 00    	jmp    QWORD PTR [rip+0x5fda]        # 407020 <puts@GLIBC_2.2.5>
  401046:	68 01 00 00 00       	push   0x1
  40104b:	e9 d0 ff ff ff       	jmp    401020 <.plt>

0000000000401050 <printf@plt>:
  401050:	ff 25 d2 5f 00 00    	jmp    QWORD PTR [rip+0x5fd2]        # 407028 <printf@GLIBC_2.2.5>
  401056:	68 02 00 00 00       	push   0x2
  40105b:	e9 c0 ff ff ff       	jmp    401020 <.plt>

0000000000401060 <fprintf@plt>:
  401060:	ff 25 ca 5f 00 00    	jmp    QWORD PTR [rip+0x5fca]        # 407030 <fprintf@GLIBC_2.2.5>
  401066:	68 03 00 00 00       	push   0x3
  40106b:	e9 b0 ff ff ff       	jmp    401020 <.plt>

0000000000401070 <memcpy@plt>:
  401070:	ff 25 c2 5f 00 00    	jmp    QWORD PTR [rip+0x5fc2]        # 407038 <memcpy@GLIBC_2.14>
  401076:	68 04 00 00 00       	push   0x4
  40107b:	e9 a0 ff ff ff       	jmp    401020 <.plt>

0000000000401080 <fflush@plt>:
  401080:	ff 25 ba 5f 00 00    	jmp    QWORD PTR [rip+0x5fba]        # 407040 <fflush@GLIBC_2.2.5>
  401086:	68 05 00 00 00       	push   0x5
  40108b:	e9 90 ff ff ff       	jmp    401020 <.plt>

0000000000401090 <aligned_alloc@plt>:
  401090:	ff 25 b2 5f 00 00    	jmp    QWORD PTR [rip+0x5fb2]        # 407048 <aligned_alloc@GLIBC_2.16>
  401096:	68 06 00 00 00       	push   0x6
  40109b:	e9 80 ff ff ff       	jmp    401020 <.plt>

Disassembly of section .text:

00000000004010a0 <main>:
  4010a0:	41 57                	push   r15
  4010a2:	be 40 00 04 00       	mov    esi,0x40040
  4010a7:	bf 40 00 00 00       	mov    edi,0x40
  4010ac:	41 56                	push   r14
  4010ae:	41 55                	push   r13
  4010b0:	41 54                	push   r12
  4010b2:	55                   	push   rbp
  4010b3:	53                   	push   rbx
  4010b4:	48 81 ec 78 01 00 00 	sub    rsp,0x178
  4010bb:	e8 d0 ff ff ff       	call   401090 <aligned_alloc@plt>
  4010c0:	be 00 10 00 00       	mov    esi,0x1000
  4010c5:	bf 40 00 00 00       	mov    edi,0x40
  4010ca:	49 89 c4             	mov    r12,rax
  4010cd:	e8 be ff ff ff       	call   401090 <aligned_alloc@plt>
  4010d2:	48 89 c5             	mov    rbp,rax
  4010d5:	4c 89 e1             	mov    rcx,r12
  4010d8:	49 8d b4 24 40 00 04 	lea    rsi,[r12+0x40040]
  4010df:	00 
  4010e0:	b8 11 00 00 00       	mov    eax,0x11
  4010e5:	0f 1f 00             	nop    DWORD PTR [rax]
  4010e8:	48 89 c2             	mov    rdx,rax
  4010eb:	48 c1 e2 0d          	shl    rdx,0xd
  4010ef:	48 31 d0             	xor    rax,rdx
  4010f2:	48 89 c2             	mov    rdx,rax
  4010f5:	48 c1 ea 07          	shr    rdx,0x7
  4010f9:	48 31 c2             	xor    rdx,rax
  4010fc:	48 89 d0             	mov    rax,rdx
  4010ff:	48 c1 e0 11          	shl    rax,0x11
  401103:	48 31 d0             	xor    rax,rdx
  401106:	88 01                	mov    BYTE PTR [rcx],al
  401108:	48 ff c1             	inc    rcx
  40110b:	48 39 f1             	cmp    rcx,rsi
  40110e:	75 d8                	jne    4010e8 <main+0x48>
  401110:	48 89 e9             	mov    rcx,rbp
  401113:	48 8d b5 00 10 00 00 	lea    rsi,[rbp+0x1000]
  40111a:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
  401120:	48 89 c2             	mov    rdx,rax
  401123:	48 c1 e2 0d          	shl    rdx,0xd
  401127:	48 31 d0             	xor    rax,rdx
  40112a:	48 89 c2             	mov    rdx,rax
  40112d:	48 c1 ea 07          	shr    rdx,0x7
  401131:	48 31 c2             	xor    rdx,rax
  401134:	48 89 d0             	mov    rax,rdx
  401137:	48 c1 e0 11          	shl    rax,0x11
  40113b:	48 31 d0             	xor    rax,rdx
  40113e:	88 01                	mov    BYTE PTR [rcx],al
  401140:	48 ff c1             	inc    rcx
  401143:	48 39 ce             	cmp    rsi,rcx
  401146:	75 d8                	jne    401120 <main+0x80>
  401148:	ba 20 01 00 00       	mov    edx,0x120
  40114d:	be 80 51 40 00       	mov    esi,0x405180
  401152:	48 8d 7c 24 50       	lea    rdi,[rsp+0x50]
  401157:	e8 14 ff ff ff       	call   401070 <memcpy@plt>
  40115c:	48 8d 44 24 50       	lea    rax,[rsp+0x50]
  401161:	48 89 44 24 08       	mov    QWORD PTR [rsp+0x8],rax
  401166:	48 8b 44 24 08       	mov    rax,QWORD PTR [rsp+0x8]
  40116b:	bb e8 03 00 00       	mov    ebx,0x3e8
  401170:	4c 8b 68 08          	mov    r13,QWORD PTR [rax+0x8]
  401174:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
  401178:	ba 00 00 04 00       	mov    edx,0x40000
  40117d:	48 89 ee             	mov    rsi,rbp
  401180:	4c 89 e7             	mov    rdi,r12
  401183:	41 ff d5             	call   r13
  401186:	49 89 c0             	mov    r8,rax
  401189:	48 8b 05 00 5f 00 00 	mov    rax,QWORD PTR [rip+0x5f00]        # 407090 <sink>
  401190:	4c 31 c0             	xor    rax,r8
  401193:	48 89 05 f6 5e 00 00 	mov    QWORD PTR [rip+0x5ef6],rax        # 407090 <sink>
  40119a:	ff cb                	dec    ebx
  40119c:	75 da                	jne    401178 <main+0xd8>
  40119e:	4c 8d 74 24 10       	lea    r14,[rsp+0x10]
  4011a3:	0f ae e8             	lfence 
  4011a6:	0f 31                	rdtsc  
  4011a8:	bb e8 03 00 00       	mov    ebx,0x3e8
  4011ad:	49 89 c7             	mov    r15,rax
  4011b0:	48 c1 e2 20          	shl    rdx,0x20
  4011b4:	49 09 d7             	or     r15,rdx
  4011b7:	66 0f 1f 84 00 00 00 	nop    WORD PTR [rax+rax*1+0x0]
  4011be:	00 00 
  4011c0:	ba 00 00 04 00       	mov    edx,0x40000
  4011c5:	48 89 ee             	mov    rsi,rbp
  4011c8:	4c 89 e7             	mov    rdi,r12
  4011cb:	41 ff d5             	call   r13
  4011ce:	49 89 c0             	mov    r8,rax
  4011d1:	48 8b 05 b8 5e 00 00 	mov    rax,QWORD PTR [rip+0x5eb8]        # 407090 <sink>
  4011d8:	4c 31 c0             	xor    rax,r8
  4011db:	48 89 05 ae 5e 00 00 	mov    QWORD PTR [rip+0x5eae],rax        # 407090 <sink>
  4011e2:	ff cb                	dec    ebx
  4011e4:	75 da                	jne    4011c0 <main+0x120>
  4011e6:	0f ae e8             	lfence 
  4011e9:	0f 31                	rdtsc  
  4011eb:	c5 e1 57 db          	vxorpd xmm3,xmm3,xmm3
  4011ef:	48 8b 4c 24 08       	mov    rcx,QWORD PTR [rsp+0x8]
  4011f4:	48 c1 e2 20          	shl    rdx,0x20
  4011f8:	c5 fb 10 51 10       	vmovsd xmm2,QWORD PTR [rcx+0x10]
  4011fd:	48 09 d0             	or     rax,rdx
  401200:	4c 29 f8             	sub    rax,r15
  401203:	c5 eb 59 05 45 41 00 	vmulsd xmm0,xmm2,QWORD PTR [rip+0x4145]        # 405350 <__dso_handle+0x348>
  40120a:	00 
  40120b:	62 f1 e7 08 7b c8    	vcvtusi2sd xmm1,xmm3,rax
  401211:	49 83 c6 08          	add    r14,0x8
  401215:	48 8d 44 24 48       	lea    rax,[rsp+0x48]
  40121a:	c5 fb 5e c1          	vdivsd xmm0,xmm0,xmm1
  40121e:	c4 c1 7b 11 46 f8    	vmovsd QWORD PTR [r14-0x8],xmm0
  401224:	4c 39 f0             	cmp    rax,r14
  401227:	0f 85 76 ff ff ff    	jne    4011a3 <main+0x103>
  40122d:	48 8b 31             	mov    rsi,QWORD PTR [rcx]
  401230:	c5 eb 10 c2          	vmovsd xmm0,xmm2,xmm2
  401234:	bf 10 50 40 00       	mov    edi,0x405010
  401239:	b8 01 00 00 00       	mov    eax,0x1
  40123e:	e8 0d fe ff ff       	call   401050 <printf@plt>
  401243:	31 db                	xor    ebx,ebx
  401245:	eb 1d                	jmp    401264 <main+0x1c4>
  401247:	be 89 50 40 00       	mov    esi,0x405089
  40124c:	bf 8b 50 40 00       	mov    edi,0x40508b
  401251:	b8 01 00 00 00       	mov    eax,0x1
  401256:	e8 f5 fd ff ff       	call   401050 <printf@plt>
  40125b:	48 83 fb 06          	cmp    rbx,0x6
  40125f:	74 24                	je     401285 <main+0x1e5>
  401261:	48 ff c3             	inc    rbx
  401264:	c5 fb 10 44 dc 10    	vmovsd xmm0,QWORD PTR [rsp+rbx*8+0x10]
  40126a:	48 85 db             	test   rbx,rbx
  40126d:	75 d8                	jne    401247 <main+0x1a7>
  40126f:	be a3 50 40 00       	mov    esi,0x4050a3
  401274:	bf 8b 50 40 00       	mov    edi,0x40508b
  401279:	b8 01 00 00 00       	mov    eax,0x1
  40127e:	e8 cd fd ff ff       	call   401050 <printf@plt>
  401283:	eb dc                	jmp    401261 <main+0x1c1>
  401285:	bf 92 50 40 00       	mov    edi,0x405092
  40128a:	e8 b1 fd ff ff       	call   401040 <puts@plt>
  40128f:	48 8b 3d ca 5d 00 00 	mov    rdi,QWORD PTR [rip+0x5dca]        # 407060 <stdout@@GLIBC_2.2.5>
  401296:	e8 e5 fd ff ff       	call   401080 <fflush@plt>
  40129b:	48 83 44 24 08 18    	add    QWORD PTR [rsp+0x8],0x18
  4012a1:	48 8d 8c 24 70 01 00 	lea    rcx,[rsp+0x170]
  4012a8:	00 
  4012a9:	48 8b 44 24 08       	mov    rax,QWORD PTR [rsp+0x8]
  4012ae:	48 39 c1             	cmp    rcx,rax
  4012b1:	0f 85 af fe ff ff    	jne    401166 <main+0xc6>
  4012b7:	48 8b 15 d2 5d 00 00 	mov    rdx,QWORD PTR [rip+0x5dd2]        # 407090 <sink>
  4012be:	48 8b 3d bb 5d 00 00 	mov    rdi,QWORD PTR [rip+0x5dbb]        # 407080 <stderr@@GLIBC_2.2.5>
  4012c5:	be 95 50 40 00       	mov    esi,0x405095
  4012ca:	31 c0                	xor    eax,eax
  4012cc:	e8 8f fd ff ff       	call   401060 <fprintf@plt>
  4012d1:	4c 89 e7             	mov    rdi,r12
  4012d4:	e8 57 fd ff ff       	call   401030 <free@plt>
  4012d9:	48 89 ef             	mov    rdi,rbp
  4012dc:	e8 4f fd ff ff       	call   401030 <free@plt>
  4012e1:	48 81 c4 78 01 00 00 	add    rsp,0x178
  4012e8:	5b                   	pop    rbx
  4012e9:	5d                   	pop    rbp
  4012ea:	41 5c                	pop    r12
  4012ec:	41 5d                	pop    r13
  4012ee:	41 5e                	pop    r14
  4012f0:	31 c0                	xor    eax,eax
  4012f2:	41 5f                	pop    r15
  4012f4:	c3                   	ret    
  4012f5:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
  4012fc:	00 00 00 
  4012ff:	90                   	nop

0000000000401300 <_start>:
  401300:	f3 0f 1e fa          	endbr64 
  401304:	31 ed                	xor    ebp,ebp
  401306:	49 89 d1             	mov    r9,rdx
  401309:	5e                   	pop    rsi
  40130a:	48 89 e2             	mov    rdx,rsp
  40130d:	48 83 e4 f0          	and    rsp,0xfffffffffffffff0
  401311:	50                   	push   rax
  401312:	54                   	push   rsp
  401313:	45 31 c0             	xor    r8d,r8d
  401316:	31 c9                	xor    ecx,ecx
  401318:	48 c7 c7 a0 10 40 00 	mov    rdi,0x4010a0
  40131f:	ff 15 bb 5c 00 00    	call   QWORD PTR [rip+0x5cbb]        # 406fe0 <__libc_start_main@GLIBC_2.34>
  401325:	f4                   	hlt    
  401326:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
  40132d:	00 00 00 

0000000000401330 <_dl_relocate_static_pie>:
  401330:	f3 0f 1e fa          	endbr64 
  401334:	c3                   	ret    
  401335:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
  40133c:	00 00 00 
  40133f:	90                   	nop

0000000000401340 <deregister_tm_clones>:
  401340:	48 8d 3d 11 5d 00 00 	lea    rdi,[rip+0x5d11]        # 407058 <__TMC_END__>
  401347:	48 8d 05 0a 5d 00 00 	lea    rax,[rip+0x5d0a]        # 407058 <__TMC_END__>
  40134e:	48 39 f8             	cmp    rax,rdi
  401351:	74 15                	je     401368 <deregister_tm_clones+0x28>
  401353:	48 8b 05 8e 5c 00 00 	mov    rax,QWORD PTR [rip+0x5c8e]        # 406fe8 <_ITM_deregisterTMCloneTable>
  40135a:	48 85 c0             	test   rax,rax
  40135d:	74 09                	je     401368 <deregister_tm_clones+0x28>
  40135f:	ff e0                	jmp    rax
  401361:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]
  401368:	c3                   	ret    
  401369:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]

0000000000401370 <register_tm_clones>:
  401370:	48 8d 3d e1 5c 00 00 	lea    rdi,[rip+0x5ce1]        # 407058 <__TMC_END__>
  401377:	48 8d 35 da 5c 00 00 	lea    rsi,[rip+0x5cda]        # 407058 <__TMC_END__>
  40137e:	48 29 fe             	sub    rsi,rdi
  401381:	48 89 f0             	mov    rax,rsi
  401384:	48 c1 ee 3f          	shr    rsi,0x3f
  401388:	48 c1 f8 03          	sar    rax,0x3
  40138c:	48 01 c6             	add    rsi,rax
  40138f:	48 d1 fe             	sar    rsi,1
  401392:	74 14                	je     4013a8 <register_tm_clones+0x38>
  401394:	48 8b 05 5d 5c 00 00 	mov    rax,QWORD PTR [rip+0x5c5d]        # 406ff8 <_ITM_registerTMCloneTable>
  40139b:	48 85 c0             	test   rax,rax
  40139e:	74 08                	je     4013a8 <register_tm_clones+0x38>
  4013a0:	ff e0                	jmp    rax
  4013a2:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
  4013a8:	c3                   	ret    
  4013a9:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]

00000000004013b0 <__do_global_dtors_aux>:
  4013b0:	f3 0f 1e fa          	endbr64 
  4013b4:	80 3d cd 5c 00 00 00 	cmp    BYTE PTR [rip+0x5ccd],0x0        # 407088 <completed.0>
  4013bb:	75 13                	jne    4013d0 <__do_global_dtors_aux+0x20>
  4013bd:	55                   	push   rbp
  4013be:	48 89 e5             	mov    rbp,rsp
  4013c1:	e8 7a ff ff ff       	call   401340 <deregister_tm_clones>
  4013c6:	c6 05 bb 5c 00 00 01 	mov    BYTE PTR [rip+0x5cbb],0x1        # 407088 <completed.0>
  4013cd:	5d                   	pop    rbp
  4013ce:	c3                   	ret    
  4013cf:	90                   	nop
  4013d0:	c3                   	ret    
  4013d1:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
  4013d8:	00 00 00 00 
  4013dc:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]

00000000004013e0 <frame_dummy>:
  4013e0:	f3 0f 1e fa          	endbr64 
  4013e4:	eb 8a                	jmp    401370 <register_tm_clones>
  4013e6:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
  4013ed:	00 00 00 

00000000004013f0 <nh64_scalar>:
  4013f0:	48 85 d2             	test   rdx,rdx
  4013f3:	0f 84 1c 01 00 00    	je     401515 <nh64_scalar+0x125>
  4013f9:	41 57                	push   r15
  4013fb:	48 89 f0             	mov    rax,rsi
  4013fe:	45 31 ff             	xor    r15d,r15d
  401401:	41 56                	push   r14
  401403:	48 89 d6             	mov    rsi,rdx
  401406:	45 31 f6             	xor    r14d,r14d
  401409:	41 55                	push   r13
  40140b:	31 c9                	xor    ecx,ecx
  40140d:	45 31 d2             	xor    r10d,r10d
  401410:	41 54                	push   r12
  401412:	45 31 db             	xor    r11d,r11d
  401415:	45 31 e4             	xor    r12d,r12d
  401418:	55                   	push   rbp
  401419:	45 31 ed             	xor    r13d,r13d
  40141c:	53                   	push   rbx
  40141d:	31 db                	xor    ebx,ebx
  40141f:	48 c7 44 24 f0 00 00 	mov    QWORD PTR [rsp-0x10],0x0
  401426:	00 00 
  401428:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
  40142f:	00 
  401430:	48 8b 54 24 f0       	mov    rdx,QWORD PTR [rsp-0x10]
  401435:	4c 8b 47 08          	mov    r8,QWORD PTR [rdi+0x8]
  401439:	81 e2 ff 03 00 00    	and    edx,0x3ff
  40143f:	4c 8b 0f             	mov    r9,QWORD PTR [rdi]
  401442:	4c 03 44 10 08       	add    r8,QWORD PTR [rax+rdx*1+0x8]
  401447:	4c 03 0c 10          	add    r9,QWORD PTR [rax+rdx*1]
  40144b:	4c 89 c2             	mov    rdx,r8
  40144e:	c4 42 bb f6 c9       	mulx   r9,r8,r9
  401453:	48 8b 6c 24 f0       	mov    rbp,QWORD PTR [rsp-0x10]
  401458:	4d 01 c4             	add    r12,r8
  40145b:	4d 11 cd             	adc    r13,r9
  40145e:	4c 8b 47 18          	mov    r8,QWORD PTR [rdi+0x18]
  401462:	4c 8d 4d 10          	lea    r9,[rbp+0x10]
  401466:	41 81 e1 ff 03 00 00 	and    r9d,0x3ff
  40146d:	4e 03 44 08 08       	add    r8,QWORD PTR [rax+r9*1+0x8]
  401472:	4e 8b 0c 08          	mov    r9,QWORD PTR [rax+r9*1]
  401476:	4c 03 4f 10          	add    r9,QWORD PTR [rdi+0x10]
  40147a:	4c 89 ca             	mov    rdx,r9
  40147d:	c4 42 bb f6 c8       	mulx   r9,r8,r8
  401482:	4d 01 c2             	add    r10,r8
  401485:	4d 11 cb             	adc    r11,r9
  401488:	4c 8b 47 28          	mov    r8,QWORD PTR [rdi+0x28]
  40148c:	4c 8d 4d 20          	lea    r9,[rbp+0x20]
  401490:	41 81 e1 ff 03 00 00 	and    r9d,0x3ff
  401497:	4e 03 44 08 08       	add    r8,QWORD PTR [rax+r9*1+0x8]
  40149c:	4e 8b 0c 08          	mov    r9,QWORD PTR [rax+r9*1]
  4014a0:	4c 03 4f 20          	add    r9,QWORD PTR [rdi+0x20]
  4014a4:	4c 89 ca             	mov    rdx,r9
  4014a7:	c4 42 bb f6 c8       	mulx   r9,r8,r8
  4014ac:	4c 01 c1             	add    rcx,r8
  4014af:	4c 11 cb             	adc    rbx,r9
  4014b2:	4c 8b 47 38          	mov    r8,QWORD PTR [rdi+0x38]
  4014b6:	4c 8d 4d 30          	lea    r9,[rbp+0x30]
  4014ba:	41 81 e1 ff 03 00 00 	and    r9d,0x3ff
  4014c1:	4e 03 44 08 08       	add    r8,QWORD PTR [rax+r9*1+0x8]
  4014c6:	4e 8b 0c 08          	mov    r9,QWORD PTR [rax+r9*1]
  4014ca:	4c 03 4f 30          	add    r9,QWORD PTR [rdi+0x30]
  4014ce:	4c 89 ca             	mov    rdx,r9
  4014d1:	c4 42 bb f6 c8       	mulx   r9,r8,r8
  4014d6:	4d 01 c6             	add    r14,r8
  4014d9:	4d 11 cf             	adc    r15,r9
  4014dc:	48 83 c5 40          	add    rbp,0x40
  4014e0:	48 89 6c 24 f0       	mov    QWORD PTR [rsp-0x10],rbp
  4014e5:	48 83 c7 40          	add    rdi,0x40
  4014e9:	48 39 ee             	cmp    rsi,rbp
  4014ec:	0f 87 3e ff ff ff    	ja     401430 <nh64_scalar+0x40>
  4014f2:	49 01 ca             	add    r10,rcx
  4014f5:	49 11 db             	adc    r11,rbx
  4014f8:	5b                   	pop    rbx
  4014f9:	5d                   	pop    rbp
  4014fa:	4d 01 e2             	add    r10,r12
  4014fd:	4d 11 eb             	adc    r11,r13
  401500:	41 5c                	pop    r12
  401502:	4d 01 f2             	add    r10,r14
  401505:	4d 11 fb             	adc    r11,r15
  401508:	41 5d                	pop    r13
  40150a:	4c 89 d8             	mov    rax,r11
  40150d:	41 5e                	pop    r14
  40150f:	4c 31 d0             	xor    rax,r10
  401512:	41 5f                	pop    r15
  401514:	c3                   	ret    
  401515:	31 c0                	xor    eax,eax
  401517:	c3                   	ret    
  401518:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
  40151f:	00 

0000000000401520 <nh32_one>:
  401520:	55                   	push   rbp
  401521:	c5 f9 ef c0          	vpxor  xmm0,xmm0,xmm0
  401525:	48 89 e5             	mov    rbp,rsp
  401528:	48 83 e4 c0          	and    rsp,0xffffffffffffffc0
  40152c:	48 81 ec 88 01 00 00 	sub    rsp,0x188
  401533:	c5 f9 7f 84 24 88 00 	vmovdqa XMMWORD PTR [rsp+0x88],xmm0
  40153a:	00 00 
  40153c:	c5 f9 7f 84 24 98 00 	vmovdqa XMMWORD PTR [rsp+0x98],xmm0
  401543:	00 00 
  401545:	c5 f9 7f 84 24 a8 00 	vmovdqa XMMWORD PTR [rsp+0xa8],xmm0
  40154c:	00 00 
  40154e:	c5 f9 7f 84 24 b8 00 	vmovdqa XMMWORD PTR [rsp+0xb8],xmm0
  401555:	00 00 
  401557:	c5 f9 7f 84 24 c8 00 	vmovdqa XMMWORD PTR [rsp+0xc8],xmm0
  40155e:	00 00 
  401560:	c5 f9 7f 84 24 d8 00 	vmovdqa XMMWORD PTR [rsp+0xd8],xmm0
  401567:	00 00 
  401569:	c5 f9 7f 84 24 e8 00 	vmovdqa XMMWORD PTR [rsp+0xe8],xmm0
  401570:	00 00 
  401572:	c5 f9 7f 84 24 f8 00 	vmovdqa XMMWORD PTR [rsp+0xf8],xmm0
  401579:	00 00 
  40157b:	c5 f9 7f 84 24 08 01 	vmovdqa XMMWORD PTR [rsp+0x108],xmm0
  401582:	00 00 
  401584:	c5 f9 7f 84 24 18 01 	vmovdqa XMMWORD PTR [rsp+0x118],xmm0
  40158b:	00 00 
  40158d:	c5 f9 7f 84 24 28 01 	vmovdqa XMMWORD PTR [rsp+0x128],xmm0
  401594:	00 00 
  401596:	c5 f9 7f 84 24 38 01 	vmovdqa XMMWORD PTR [rsp+0x138],xmm0
  40159d:	00 00 
  40159f:	c5 f9 7f 84 24 48 01 	vmovdqa XMMWORD PTR [rsp+0x148],xmm0
  4015a6:	00 00 
  4015a8:	c5 f9 7f 84 24 58 01 	vmovdqa XMMWORD PTR [rsp+0x158],xmm0
  4015af:	00 00 
  4015b1:	c5 f9 7f 84 24 68 01 	vmovdqa XMMWORD PTR [rsp+0x168],xmm0
  4015b8:	00 00 
  4015ba:	c5 f9 7f 84 24 78 01 	vmovdqa XMMWORD PTR [rsp+0x178],xmm0
  4015c1:	00 00 
  4015c3:	c5 f9 7f 44 24 88    	vmovdqa XMMWORD PTR [rsp-0x78],xmm0
  4015c9:	c5 f9 7f 44 24 98    	vmovdqa XMMWORD PTR [rsp-0x68],xmm0
  4015cf:	c5 f9 7f 44 24 a8    	vmovdqa XMMWORD PTR [rsp-0x58],xmm0
  4015d5:	c5 f9 7f 44 24 b8    	vmovdqa XMMWORD PTR [rsp-0x48],xmm0
  4015db:	c5 f9 7f 44 24 c8    	vmovdqa XMMWORD PTR [rsp-0x38],xmm0
  4015e1:	c5 f9 7f 44 24 d8    	vmovdqa XMMWORD PTR [rsp-0x28],xmm0
  4015e7:	c5 f9 7f 44 24 e8    	vmovdqa XMMWORD PTR [rsp-0x18],xmm0
  4015ed:	c5 f9 7f 44 24 f8    	vmovdqa XMMWORD PTR [rsp-0x8],xmm0
  4015f3:	c5 f9 7f 44 24 08    	vmovdqa XMMWORD PTR [rsp+0x8],xmm0
  4015f9:	c5 f9 7f 44 24 18    	vmovdqa XMMWORD PTR [rsp+0x18],xmm0
  4015ff:	c5 f9 7f 44 24 28    	vmovdqa XMMWORD PTR [rsp+0x28],xmm0
  401605:	c5 f9 7f 44 24 38    	vmovdqa XMMWORD PTR [rsp+0x38],xmm0
  40160b:	c5 f9 7f 44 24 48    	vmovdqa XMMWORD PTR [rsp+0x48],xmm0
  401611:	c5 f9 7f 44 24 58    	vmovdqa XMMWORD PTR [rsp+0x58],xmm0
  401617:	c5 f9 7f 44 24 68    	vmovdqa XMMWORD PTR [rsp+0x68],xmm0
  40161d:	c5 f9 7f 44 24 78    	vmovdqa XMMWORD PTR [rsp+0x78],xmm0
  401623:	48 81 fa ff 00 00 00 	cmp    rdx,0xff
  40162a:	0f 86 58 01 00 00    	jbe    401788 <nh32_one+0x268>
  401630:	c5 e9 ef d2          	vpxor  xmm2,xmm2,xmm2
  401634:	48 89 f9             	mov    rcx,rdi
  401637:	31 c0                	xor    eax,eax
  401639:	62 f1 fd 48 6f da    	vmovdqa64 zmm3,zmm2
  40163f:	62 f1 fd 48 6f e2    	vmovdqa64 zmm4,zmm2
  401645:	62 f1 fd 48 6f ca    	vmovdqa64 zmm1,zmm2
  40164b:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
  401650:	48 89 c7             	mov    rdi,rax
  401653:	81 e7 ff 03 00 00    	and    edi,0x3ff
  401659:	62 f1 7e 48 6f 34 3e 	vmovdqu32 zmm6,ZMMWORD PTR [rsi+rdi*1]
  401660:	48 8d 78 40          	lea    rdi,[rax+0x40]
  401664:	62 f1 4d 48 fe 04 01 	vpaddd zmm0,zmm6,ZMMWORD PTR [rcx+rax*1]
  40166b:	81 e7 ff 03 00 00    	and    edi,0x3ff
  401671:	62 f1 d5 48 73 d0 20 	vpsrlq zmm5,zmm0,0x20
  401678:	62 f1 fd 48 f4 c5    	vpmuludq zmm0,zmm0,zmm5
  40167e:	62 f1 7e 48 6f 3c 3e 	vmovdqu32 zmm7,ZMMWORD PTR [rsi+rdi*1]
  401685:	48 8d b8 80 00 00 00 	lea    rdi,[rax+0x80]
  40168c:	81 e7 ff 03 00 00    	and    edi,0x3ff
  401692:	62 f1 7e 48 6f 34 3e 	vmovdqu32 zmm6,ZMMWORD PTR [rsi+rdi*1]
  401699:	62 f1 f5 48 d4 c8    	vpaddq zmm1,zmm1,zmm0
  40169f:	62 f1 45 48 fe 44 01 	vpaddd zmm0,zmm7,ZMMWORD PTR [rcx+rax*1+0x40]
  4016a6:	01 
  4016a7:	48 8d b8 c0 00 00 00 	lea    rdi,[rax+0xc0]
  4016ae:	62 f1 d5 48 73 d0 20 	vpsrlq zmm5,zmm0,0x20
  4016b5:	62 f1 fd 48 f4 c5    	vpmuludq zmm0,zmm0,zmm5
  4016bb:	81 e7 ff 03 00 00    	and    edi,0x3ff
  4016c1:	62 f1 7e 48 6f 3c 3e 	vmovdqu32 zmm7,ZMMWORD PTR [rsi+rdi*1]
  4016c8:	48 89 c7             	mov    rdi,rax
  4016cb:	48 81 c7 00 02 00 00 	add    rdi,0x200
  4016d2:	62 f1 dd 48 d4 e0    	vpaddq zmm4,zmm4,zmm0
  4016d8:	62 f1 4d 48 fe 44 01 	vpaddd zmm0,zmm6,ZMMWORD PTR [rcx+rax*1+0x80]
  4016df:	02 
  4016e0:	62 f1 d5 48 73 d0 20 	vpsrlq zmm5,zmm0,0x20
  4016e7:	62 f1 fd 48 f4 c5    	vpmuludq zmm0,zmm0,zmm5
  4016ed:	62 f1 e5 48 d4 d8    	vpaddq zmm3,zmm3,zmm0
  4016f3:	62 f1 45 48 fe 44 01 	vpaddd zmm0,zmm7,ZMMWORD PTR [rcx+rax*1+0xc0]
  4016fa:	03 
  4016fb:	48 05 00 01 00 00    	add    rax,0x100
  401701:	62 f1 d5 48 73 d0 20 	vpsrlq zmm5,zmm0,0x20
  401708:	62 f1 fd 48 f4 c5    	vpmuludq zmm0,zmm0,zmm5
  40170e:	62 f1 ed 48 d4 d0    	vpaddq zmm2,zmm2,zmm0
  401714:	48 39 d7             	cmp    rdi,rdx
  401717:	0f 86 33 ff ff ff    	jbe    401650 <nh32_one+0x130>
  40171d:	62 f1 fd 48 7f a4 24 	vmovdqa64 ZMMWORD PTR [rsp-0x38],zmm4
  401724:	c8 ff ff ff 
  401728:	62 f1 fd 48 7f 9c 24 	vmovdqa64 ZMMWORD PTR [rsp+0x8],zmm3
  40172f:	08 00 00 00 
  401733:	62 f1 fd 48 7f 94 24 	vmovdqa64 ZMMWORD PTR [rsp+0x48],zmm2
  40173a:	48 00 00 00 
  40173e:	62 f1 f5 48 d4 84 24 	vpaddq zmm0,zmm1,ZMMWORD PTR [rsp-0x38]
  401745:	c8 ff ff ff 
  401749:	62 f1 fd 48 d4 84 24 	vpaddq zmm0,zmm0,ZMMWORD PTR [rsp+0x8]
  401750:	08 00 00 00 
  401754:	62 f1 fd 48 d4 84 24 	vpaddq zmm0,zmm0,ZMMWORD PTR [rsp+0x48]
  40175b:	48 00 00 00 
  40175f:	62 f3 fd 48 3b c1 01 	vextracti64x4 ymm1,zmm0,0x1
  401766:	c5 f5 d4 c8          	vpaddq ymm1,ymm1,ymm0
  40176a:	62 f3 fd 28 39 c8 01 	vextracti64x2 xmm0,ymm1,0x1
  401771:	c5 f9 d4 c1          	vpaddq xmm0,xmm0,xmm1
  401775:	c4 e1 f9 7e c0       	vmovq  rax,xmm0
  40177a:	c4 e3 f9 16 c2 01    	vpextrq rdx,xmm0,0x1
  401780:	48 01 d0             	add    rax,rdx
  401783:	c5 f8 77             	vzeroupper 
  401786:	c9                   	leave  
  401787:	c3                   	ret    
  401788:	c5 f1 ef c9          	vpxor  xmm1,xmm1,xmm1
  40178c:	eb b0                	jmp    40173e <nh32_one+0x21e>
  40178e:	66 90                	xchg   ax,ax

0000000000401790 <nh32_two_shift>:
  401790:	55                   	push   rbp
  401791:	c5 f9 ef c0          	vpxor  xmm0,xmm0,xmm0
  401795:	48 89 e5             	mov    rbp,rsp
  401798:	48 83 e4 c0          	and    rsp,0xffffffffffffffc0
  40179c:	48 81 ec 88 01 00 00 	sub    rsp,0x188
  4017a3:	c5 f9 7f 84 24 88 00 	vmovdqa XMMWORD PTR [rsp+0x88],xmm0
  4017aa:	00 00 
  4017ac:	c5 f9 7f 84 24 98 00 	vmovdqa XMMWORD PTR [rsp+0x98],xmm0
  4017b3:	00 00 
  4017b5:	c5 f9 7f 84 24 a8 00 	vmovdqa XMMWORD PTR [rsp+0xa8],xmm0
  4017bc:	00 00 
  4017be:	c5 f9 7f 84 24 b8 00 	vmovdqa XMMWORD PTR [rsp+0xb8],xmm0
  4017c5:	00 00 
  4017c7:	c5 f9 7f 84 24 c8 00 	vmovdqa XMMWORD PTR [rsp+0xc8],xmm0
  4017ce:	00 00 
  4017d0:	c5 f9 7f 84 24 d8 00 	vmovdqa XMMWORD PTR [rsp+0xd8],xmm0
  4017d7:	00 00 
  4017d9:	c5 f9 7f 84 24 e8 00 	vmovdqa XMMWORD PTR [rsp+0xe8],xmm0
  4017e0:	00 00 
  4017e2:	c5 f9 7f 84 24 f8 00 	vmovdqa XMMWORD PTR [rsp+0xf8],xmm0
  4017e9:	00 00 
  4017eb:	c5 f9 7f 84 24 08 01 	vmovdqa XMMWORD PTR [rsp+0x108],xmm0
  4017f2:	00 00 
  4017f4:	c5 f9 7f 84 24 18 01 	vmovdqa XMMWORD PTR [rsp+0x118],xmm0
  4017fb:	00 00 
  4017fd:	c5 f9 7f 84 24 28 01 	vmovdqa XMMWORD PTR [rsp+0x128],xmm0
  401804:	00 00 
  401806:	c5 f9 7f 84 24 38 01 	vmovdqa XMMWORD PTR [rsp+0x138],xmm0
  40180d:	00 00 
  40180f:	c5 f9 7f 84 24 48 01 	vmovdqa XMMWORD PTR [rsp+0x148],xmm0
  401816:	00 00 
  401818:	c5 f9 7f 84 24 58 01 	vmovdqa XMMWORD PTR [rsp+0x158],xmm0
  40181f:	00 00 
  401821:	c5 f9 7f 84 24 68 01 	vmovdqa XMMWORD PTR [rsp+0x168],xmm0
  401828:	00 00 
  40182a:	c5 f9 7f 84 24 78 01 	vmovdqa XMMWORD PTR [rsp+0x178],xmm0
  401831:	00 00 
  401833:	c5 f9 7f 44 24 88    	vmovdqa XMMWORD PTR [rsp-0x78],xmm0
  401839:	c5 f9 7f 44 24 98    	vmovdqa XMMWORD PTR [rsp-0x68],xmm0
  40183f:	c5 f9 7f 44 24 a8    	vmovdqa XMMWORD PTR [rsp-0x58],xmm0
  401845:	c5 f9 7f 44 24 b8    	vmovdqa XMMWORD PTR [rsp-0x48],xmm0
  40184b:	c5 f9 7f 44 24 c8    	vmovdqa XMMWORD PTR [rsp-0x38],xmm0
  401851:	c5 f9 7f 44 24 d8    	vmovdqa XMMWORD PTR [rsp-0x28],xmm0
  401857:	c5 f9 7f 44 24 e8    	vmovdqa XMMWORD PTR [rsp-0x18],xmm0
  40185d:	c5 f9 7f 44 24 f8    	vmovdqa XMMWORD PTR [rsp-0x8],xmm0
  401863:	c5 f9 7f 44 24 08    	vmovdqa XMMWORD PTR [rsp+0x8],xmm0
  401869:	c5 f9 7f 44 24 18    	vmovdqa XMMWORD PTR [rsp+0x18],xmm0
  40186f:	c5 f9 7f 44 24 28    	vmovdqa XMMWORD PTR [rsp+0x28],xmm0
  401875:	c5 f9 7f 44 24 38    	vmovdqa XMMWORD PTR [rsp+0x38],xmm0
  40187b:	c5 f9 7f 44 24 48    	vmovdqa XMMWORD PTR [rsp+0x48],xmm0
  401881:	c5 f9 7f 44 24 58    	vmovdqa XMMWORD PTR [rsp+0x58],xmm0
  401887:	c5 f9 7f 44 24 68    	vmovdqa XMMWORD PTR [rsp+0x68],xmm0
  40188d:	c5 f9 7f 44 24 78    	vmovdqa XMMWORD PTR [rsp+0x78],xmm0
  401893:	48 81 fa ff 00 00 00 	cmp    rdx,0xff
  40189a:	0f 86 45 02 00 00    	jbe    401ae5 <nh32_two_shift+0x355>
  4018a0:	c5 d9 ef e4          	vpxor  xmm4,xmm4,xmm4
  4018a4:	48 89 f1             	mov    rcx,rsi
  4018a7:	31 c0                	xor    eax,eax
  4018a9:	62 f1 fd 48 6f ec    	vmovdqa64 zmm5,zmm4
  4018af:	62 f1 fd 48 6f f4    	vmovdqa64 zmm6,zmm4
  4018b5:	62 f1 fd 48 6f fc    	vmovdqa64 zmm7,zmm4
  4018bb:	62 71 fd 48 6f c4    	vmovdqa64 zmm8,zmm4
  4018c1:	62 71 fd 48 6f cc    	vmovdqa64 zmm9,zmm4
  4018c7:	62 f1 fd 48 6f d4    	vmovdqa64 zmm2,zmm4
  4018cd:	62 f1 fd 48 6f dc    	vmovdqa64 zmm3,zmm4
  4018d3:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
  4018d8:	62 f1 fe 48 6f 04 07 	vmovdqu64 zmm0,ZMMWORD PTR [rdi+rax*1]
  4018df:	48 89 c6             	mov    rsi,rax
  4018e2:	81 e6 ff 03 00 00    	and    esi,0x3ff
  4018e8:	62 f1 7d 48 fe 0c 31 	vpaddd zmm1,zmm0,ZMMWORD PTR [rcx+rsi*1]
  4018ef:	62 f1 7d 48 fe 44 31 	vpaddd zmm0,zmm0,ZMMWORD PTR [rcx+rsi*1+0x400]
  4018f6:	10 
  4018f7:	62 f1 ad 48 73 d1 20 	vpsrlq zmm10,zmm1,0x20
  4018fe:	62 d1 f5 48 f4 ca    	vpmuludq zmm1,zmm1,zmm10
  401904:	48 8d 70 40          	lea    rsi,[rax+0x40]
  401908:	81 e6 ff 03 00 00    	and    esi,0x3ff
  40190e:	62 f1 e5 48 d4 d9    	vpaddq zmm3,zmm3,zmm1
  401914:	62 f1 f5 48 73 d0 20 	vpsrlq zmm1,zmm0,0x20
  40191b:	62 f1 fd 48 f4 c1    	vpmuludq zmm0,zmm0,zmm1
  401921:	62 f1 ed 48 d4 d0    	vpaddq zmm2,zmm2,zmm0
  401927:	62 f1 fe 48 6f 44 07 	vmovdqu64 zmm0,ZMMWORD PTR [rdi+rax*1+0x40]
  40192e:	01 
  40192f:	62 f1 7d 48 fe 0c 31 	vpaddd zmm1,zmm0,ZMMWORD PTR [rcx+rsi*1]
  401936:	62 f1 7d 48 fe 44 31 	vpaddd zmm0,zmm0,ZMMWORD PTR [rcx+rsi*1+0x400]
  40193d:	10 
  40193e:	62 f1 ad 48 73 d1 20 	vpsrlq zmm10,zmm1,0x20
  401945:	62 d1 f5 48 f4 ca    	vpmuludq zmm1,zmm1,zmm10
  40194b:	48 8d b0 80 00 00 00 	lea    rsi,[rax+0x80]
  401952:	81 e6 ff 03 00 00    	and    esi,0x3ff
  401958:	62 71 b5 48 d4 c9    	vpaddq zmm9,zmm9,zmm1
  40195e:	62 f1 f5 48 73 d0 20 	vpsrlq zmm1,zmm0,0x20
  401965:	62 f1 fd 48 f4 c1    	vpmuludq zmm0,zmm0,zmm1
  40196b:	62 71 bd 48 d4 c0    	vpaddq zmm8,zmm8,zmm0
  401971:	62 f1 fe 48 6f 44 07 	vmovdqu64 zmm0,ZMMWORD PTR [rdi+rax*1+0x80]
  401978:	02 
  401979:	62 f1 7d 48 fe 0c 31 	vpaddd zmm1,zmm0,ZMMWORD PTR [rcx+rsi*1]
  401980:	62 f1 7d 48 fe 44 31 	vpaddd zmm0,zmm0,ZMMWORD PTR [rcx+rsi*1+0x400]
  401987:	10 
  401988:	62 f1 ad 48 73 d1 20 	vpsrlq zmm10,zmm1,0x20
  40198f:	62 d1 f5 48 f4 ca    	vpmuludq zmm1,zmm1,zmm10
  401995:	48 8d b0 c0 00 00 00 	lea    rsi,[rax+0xc0]
  40199c:	81 e6 ff 03 00 00    	and    esi,0x3ff
  4019a2:	62 f1 c5 48 d4 f9    	vpaddq zmm7,zmm7,zmm1
  4019a8:	62 f1 f5 48 73 d0 20 	vpsrlq zmm1,zmm0,0x20
  4019af:	62 f1 fd 48 f4 c1    	vpmuludq zmm0,zmm0,zmm1
  4019b5:	62 f1 cd 48 d4 f0    	vpaddq zmm6,zmm6,zmm0
  4019bb:	62 f1 fe 48 6f 44 07 	vmovdqu64 zmm0,ZMMWORD PTR [rdi+rax*1+0xc0]
  4019c2:	03 
  4019c3:	62 f1 7d 48 fe 0c 31 	vpaddd zmm1,zmm0,ZMMWORD PTR [rcx+rsi*1]
  4019ca:	62 f1 7d 48 fe 44 31 	vpaddd zmm0,zmm0,ZMMWORD PTR [rcx+rsi*1+0x400]
  4019d1:	10 
  4019d2:	62 f1 ad 48 73 d1 20 	vpsrlq zmm10,zmm1,0x20
  4019d9:	62 d1 f5 48 f4 ca    	vpmuludq zmm1,zmm1,zmm10
  4019df:	48 89 c6             	mov    rsi,rax
  4019e2:	48 81 c6 00 02 00 00 	add    rsi,0x200
  4019e9:	48 05 00 01 00 00    	add    rax,0x100
  4019ef:	62 f1 d5 48 d4 e9    	vpaddq zmm5,zmm5,zmm1
  4019f5:	62 f1 f5 48 73 d0 20 	vpsrlq zmm1,zmm0,0x20
  4019fc:	62 f1 fd 48 f4 c1    	vpmuludq zmm0,zmm0,zmm1
  401a02:	62 f1 dd 48 d4 e0    	vpaddq zmm4,zmm4,zmm0
  401a08:	48 39 f2             	cmp    rdx,rsi
  401a0b:	0f 83 c7 fe ff ff    	jae    4018d8 <nh32_two_shift+0x148>
  401a11:	62 71 fd 48 7f 8c 24 	vmovdqa64 ZMMWORD PTR [rsp-0x38],zmm9
  401a18:	c8 ff ff ff 
  401a1c:	62 71 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0xc8],zmm8
  401a23:	c8 00 00 00 
  401a27:	62 f1 fd 48 7f bc 24 	vmovdqa64 ZMMWORD PTR [rsp+0x8],zmm7
  401a2e:	08 00 00 00 
  401a32:	62 f1 fd 48 7f b4 24 	vmovdqa64 ZMMWORD PTR [rsp+0x108],zmm6
  401a39:	08 01 00 00 
  401a3d:	62 f1 fd 48 7f ac 24 	vmovdqa64 ZMMWORD PTR [rsp+0x48],zmm5
  401a44:	48 00 00 00 
  401a48:	62 f1 fd 48 7f a4 24 	vmovdqa64 ZMMWORD PTR [rsp+0x148],zmm4
  401a4f:	48 01 00 00 
  401a53:	62 f1 e5 48 d4 8c 24 	vpaddq zmm1,zmm3,ZMMWORD PTR [rsp-0x38]
  401a5a:	c8 ff ff ff 
  401a5e:	62 f1 ed 48 d4 84 24 	vpaddq zmm0,zmm2,ZMMWORD PTR [rsp+0xc8]
  401a65:	c8 00 00 00 
  401a69:	62 f1 f5 48 d4 8c 24 	vpaddq zmm1,zmm1,ZMMWORD PTR [rsp+0x8]
  401a70:	08 00 00 00 
  401a74:	62 f1 fd 48 d4 84 24 	vpaddq zmm0,zmm0,ZMMWORD PTR [rsp+0x108]
  401a7b:	08 01 00 00 
  401a7f:	62 f1 f5 48 d4 8c 24 	vpaddq zmm1,zmm1,ZMMWORD PTR [rsp+0x48]
  401a86:	48 00 00 00 
  401a8a:	62 f1 fd 48 d4 84 24 	vpaddq zmm0,zmm0,ZMMWORD PTR [rsp+0x148]
  401a91:	48 01 00 00 
  401a95:	62 f3 fd 48 3b ca 01 	vextracti64x4 ymm2,zmm1,0x1
  401a9c:	c5 ed d4 d1          	vpaddq ymm2,ymm2,ymm1
  401aa0:	62 f3 fd 28 39 d1 01 	vextracti64x2 xmm1,ymm2,0x1
  401aa7:	c5 f1 d4 ca          	vpaddq xmm1,xmm1,xmm2
  401aab:	62 f3 fd 48 3b c2 01 	vextracti64x4 ymm2,zmm0,0x1
  401ab2:	c5 ed d4 d0          	vpaddq ymm2,ymm2,ymm0
  401ab6:	62 f3 fd 28 39 d0 01 	vextracti64x2 xmm0,ymm2,0x1
  401abd:	c5 f9 d4 c2          	vpaddq xmm0,xmm0,xmm2
  401ac1:	c4 e1 f9 7e c2       	vmovq  rdx,xmm0
  401ac6:	c4 e3 f9 16 c0 01    	vpextrq rax,xmm0,0x1
  401acc:	48 01 d0             	add    rax,rdx
  401acf:	c4 e1 f9 7e c9       	vmovq  rcx,xmm1
  401ad4:	c4 e3 f9 16 ca 01    	vpextrq rdx,xmm1,0x1
  401ada:	48 01 ca             	add    rdx,rcx
  401add:	48 31 d0             	xor    rax,rdx
  401ae0:	c5 f8 77             	vzeroupper 
  401ae3:	c9                   	leave  
  401ae4:	c3                   	ret    
  401ae5:	c5 e9 ef d2          	vpxor  xmm2,xmm2,xmm2
  401ae9:	62 f1 fd 48 6f da    	vmovdqa64 zmm3,zmm2
  401aef:	e9 5f ff ff ff       	jmp    401a53 <nh32_two_shift+0x2c3>
  401af4:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
  401afb:	00 00 00 00 
  401aff:	90                   	nop

0000000000401b00 <nh32_two_shuffle>:
  401b00:	55                   	push   rbp
  401b01:	c5 f9 ef c0          	vpxor  xmm0,xmm0,xmm0
  401b05:	48 89 e5             	mov    rbp,rsp
  401b08:	48 83 e4 c0          	and    rsp,0xffffffffffffffc0
  401b0c:	48 81 ec 88 01 00 00 	sub    rsp,0x188
  401b13:	c5 f9 7f 84 24 88 00 	vmovdqa XMMWORD PTR [rsp+0x88],xmm0
  401b1a:	00 00 
  401b1c:	c5 f9 7f 84 24 98 00 	vmovdqa XMMWORD PTR [rsp+0x98],xmm0
  401b23:	00 00 
  401b25:	c5 f9 7f 84 24 a8 00 	vmovdqa XMMWORD PTR [rsp+0xa8],xmm0
  401b2c:	00 00 
  401b2e:	c5 f9 7f 84 24 b8 00 	vmovdqa XMMWORD PTR [rsp+0xb8],xmm0
  401b35:	00 00 
  401b37:	c5 f9 7f 84 24 c8 00 	vmovdqa XMMWORD PTR [rsp+0xc8],xmm0
  401b3e:	00 00 
  401b40:	c5 f9 7f 84 24 d8 00 	vmovdqa XMMWORD PTR [rsp+0xd8],xmm0
  401b47:	00 00 
  401b49:	c5 f9 7f 84 24 e8 00 	vmovdqa XMMWORD PTR [rsp+0xe8],xmm0
  401b50:	00 00 
  401b52:	c5 f9 7f 84 24 f8 00 	vmovdqa XMMWORD PTR [rsp+0xf8],xmm0
  401b59:	00 00 
  401b5b:	c5 f9 7f 84 24 08 01 	vmovdqa XMMWORD PTR [rsp+0x108],xmm0
  401b62:	00 00 
  401b64:	c5 f9 7f 84 24 18 01 	vmovdqa XMMWORD PTR [rsp+0x118],xmm0
  401b6b:	00 00 
  401b6d:	c5 f9 7f 84 24 28 01 	vmovdqa XMMWORD PTR [rsp+0x128],xmm0
  401b74:	00 00 
  401b76:	c5 f9 7f 84 24 38 01 	vmovdqa XMMWORD PTR [rsp+0x138],xmm0
  401b7d:	00 00 
  401b7f:	c5 f9 7f 84 24 48 01 	vmovdqa XMMWORD PTR [rsp+0x148],xmm0
  401b86:	00 00 
  401b88:	c5 f9 7f 84 24 58 01 	vmovdqa XMMWORD PTR [rsp+0x158],xmm0
  401b8f:	00 00 
  401b91:	c5 f9 7f 84 24 68 01 	vmovdqa XMMWORD PTR [rsp+0x168],xmm0
  401b98:	00 00 
  401b9a:	c5 f9 7f 84 24 78 01 	vmovdqa XMMWORD PTR [rsp+0x178],xmm0
  401ba1:	00 00 
  401ba3:	c5 f9 7f 44 24 88    	vmovdqa XMMWORD PTR [rsp-0x78],xmm0
  401ba9:	c5 f9 7f 44 24 98    	vmovdqa XMMWORD PTR [rsp-0x68],xmm0
  401baf:	c5 f9 7f 44 24 a8    	vmovdqa XMMWORD PTR [rsp-0x58],xmm0
  401bb5:	c5 f9 7f 44 24 b8    	vmovdqa XMMWORD PTR [rsp-0x48],xmm0
  401bbb:	c5 f9 7f 44 24 c8    	vmovdqa XMMWORD PTR [rsp-0x38],xmm0
  401bc1:	c5 f9 7f 44 24 d8    	vmovdqa XMMWORD PTR [rsp-0x28],xmm0
  401bc7:	c5 f9 7f 44 24 e8    	vmovdqa XMMWORD PTR [rsp-0x18],xmm0
  401bcd:	c5 f9 7f 44 24 f8    	vmovdqa XMMWORD PTR [rsp-0x8],xmm0
  401bd3:	c5 f9 7f 44 24 08    	vmovdqa XMMWORD PTR [rsp+0x8],xmm0
  401bd9:	c5 f9 7f 44 24 18    	vmovdqa XMMWORD PTR [rsp+0x18],xmm0
  401bdf:	c5 f9 7f 44 24 28    	vmovdqa XMMWORD PTR [rsp+0x28],xmm0
  401be5:	c5 f9 7f 44 24 38    	vmovdqa XMMWORD PTR [rsp+0x38],xmm0
  401beb:	c5 f9 7f 44 24 48    	vmovdqa XMMWORD PTR [rsp+0x48],xmm0
  401bf1:	c5 f9 7f 44 24 58    	vmovdqa XMMWORD PTR [rsp+0x58],xmm0
  401bf7:	c5 f9 7f 44 24 68    	vmovdqa XMMWORD PTR [rsp+0x68],xmm0
  401bfd:	c5 f9 7f 44 24 78    	vmovdqa XMMWORD PTR [rsp+0x78],xmm0
  401c03:	48 81 fa ff 00 00 00 	cmp    rdx,0xff
  401c0a:	0f 86 45 02 00 00    	jbe    401e55 <nh32_two_shuffle+0x355>
  401c10:	c5 d9 ef e4          	vpxor  xmm4,xmm4,xmm4
  401c14:	48 89 f1             	mov    rcx,rsi
  401c17:	31 c0                	xor    eax,eax
  401c19:	62 f1 fd 48 6f ec    	vmovdqa64 zmm5,zmm4
  401c1f:	62 f1 fd 48 6f f4    	vmovdqa64 zmm6,zmm4
  401c25:	62 f1 fd 48 6f fc    	vmovdqa64 zmm7,zmm4
  401c2b:	62 71 fd 48 6f c4    	vmovdqa64 zmm8,zmm4
  401c31:	62 71 fd 48 6f cc    	vmovdqa64 zmm9,zmm4
  401c37:	62 f1 fd 48 6f d4    	vmovdqa64 zmm2,zmm4
  401c3d:	62 f1 fd 48 6f dc    	vmovdqa64 zmm3,zmm4
  401c43:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
  401c48:	62 f1 fe 48 6f 04 07 	vmovdqu64 zmm0,ZMMWORD PTR [rdi+rax*1]
  401c4f:	48 89 c6             	mov    rsi,rax
  401c52:	81 e6 ff 03 00 00    	and    esi,0x3ff
  401c58:	62 f1 7d 48 fe 0c 31 	vpaddd zmm1,zmm0,ZMMWORD PTR [rcx+rsi*1]
  401c5f:	62 f1 7d 48 fe 44 31 	vpaddd zmm0,zmm0,ZMMWORD PTR [rcx+rsi*1+0x400]
  401c66:	10 
  401c67:	62 71 7d 48 70 d1 b1 	vpshufd zmm10,zmm1,0xb1
  401c6e:	62 d1 f5 48 f4 ca    	vpmuludq zmm1,zmm1,zmm10
  401c74:	48 8d 70 40          	lea    rsi,[rax+0x40]
  401c78:	81 e6 ff 03 00 00    	and    esi,0x3ff
  401c7e:	62 f1 e5 48 d4 d9    	vpaddq zmm3,zmm3,zmm1
  401c84:	62 f1 7d 48 70 c8 b1 	vpshufd zmm1,zmm0,0xb1
  401c8b:	62 f1 fd 48 f4 c1    	vpmuludq zmm0,zmm0,zmm1
  401c91:	62 f1 ed 48 d4 d0    	vpaddq zmm2,zmm2,zmm0
  401c97:	62 f1 fe 48 6f 44 07 	vmovdqu64 zmm0,ZMMWORD PTR [rdi+rax*1+0x40]
  401c9e:	01 
  401c9f:	62 f1 7d 48 fe 0c 31 	vpaddd zmm1,zmm0,ZMMWORD PTR [rcx+rsi*1]
  401ca6:	62 f1 7d 48 fe 44 31 	vpaddd zmm0,zmm0,ZMMWORD PTR [rcx+rsi*1+0x400]
  401cad:	10 
  401cae:	62 71 7d 48 70 d1 b1 	vpshufd zmm10,zmm1,0xb1
  401cb5:	62 d1 f5 48 f4 ca    	vpmuludq zmm1,zmm1,zmm10
  401cbb:	48 8d b0 80 00 00 00 	lea    rsi,[rax+0x80]
  401cc2:	81 e6 ff 03 00 00    	and    esi,0x3ff
  401cc8:	62 71 b5 48 d4 c9    	vpaddq zmm9,zmm9,zmm1
  401cce:	62 f1 7d 48 70 c8 b1 	vpshufd zmm1,zmm0,0xb1
  401cd5:	62 f1 fd 48 f4 c1    	vpmuludq zmm0,zmm0,zmm1
  401cdb:	62 71 bd 48 d4 c0    	vpaddq zmm8,zmm8,zmm0
  401ce1:	62 f1 fe 48 6f 44 07 	vmovdqu64 zmm0,ZMMWORD PTR [rdi+rax*1+0x80]
  401ce8:	02 
  401ce9:	62 f1 7d 48 fe 0c 31 	vpaddd zmm1,zmm0,ZMMWORD PTR [rcx+rsi*1]
  401cf0:	62 f1 7d 48 fe 44 31 	vpaddd zmm0,zmm0,ZMMWORD PTR [rcx+rsi*1+0x400]
  401cf7:	10 
  401cf8:	62 71 7d 48 70 d1 b1 	vpshufd zmm10,zmm1,0xb1
  401cff:	62 d1 f5 48 f4 ca    	vpmuludq zmm1,zmm1,zmm10
  401d05:	48 8d b0 c0 00 00 00 	lea    rsi,[rax+0xc0]
  401d0c:	81 e6 ff 03 00 00    	and    esi,0x3ff
  401d12:	62 f1 c5 48 d4 f9    	vpaddq zmm7,zmm7,zmm1
  401d18:	62 f1 7d 48 70 c8 b1 	vpshufd zmm1,zmm0,0xb1
  401d1f:	62 f1 fd 48 f4 c1    	vpmuludq zmm0,zmm0,zmm1
  401d25:	62 f1 cd 48 d4 f0    	vpaddq zmm6,zmm6,zmm0
  401d2b:	62 f1 fe 48 6f 44 07 	vmovdqu64 zmm0,ZMMWORD PTR [rdi+rax*1+0xc0]
  401d32:	03 
  401d33:	62 f1 7d 48 fe 0c 31 	vpaddd zmm1,zmm0,ZMMWORD PTR [rcx+rsi*1]
  401d3a:	62 f1 7d 48 fe 44 31 	vpaddd zmm0,zmm0,ZMMWORD PTR [rcx+rsi*1+0x400]
  401d41:	10 
  401d42:	62 71 7d 48 70 d1 b1 	vpshufd zmm10,zmm1,0xb1
  401d49:	62 d1 f5 48 f4 ca    	vpmuludq zmm1,zmm1,zmm10
  401d4f:	48 89 c6             	mov    rsi,rax
  401d52:	48 81 c6 00 02 00 00 	add    rsi,0x200
  401d59:	48 05 00 01 00 00    	add    rax,0x100
  401d5f:	62 f1 d5 48 d4 e9    	vpaddq zmm5,zmm5,zmm1
  401d65:	62 f1 7d 48 70 c8 b1 	vpshufd zmm1,zmm0,0xb1
  401d6c:	62 f1 fd 48 f4 c1    	vpmuludq zmm0,zmm0,zmm1
  401d72:	62 f1 dd 48 d4 e0    	vpaddq zmm4,zmm4,zmm0
  401d78:	48 39 f2             	cmp    rdx,rsi
  401d7b:	0f 83 c7 fe ff ff    	jae    401c48 <nh32_two_shuffle+0x148>
  401d81:	62 71 fd 48 7f 8c 24 	vmovdqa64 ZMMWORD PTR [rsp-0x38],zmm9
  401d88:	c8 ff ff ff 
  401d8c:	62 71 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0xc8],zmm8
  401d93:	c8 00 00 00 
  401d97:	62 f1 fd 48 7f bc 24 	vmovdqa64 ZMMWORD PTR [rsp+0x8],zmm7
  401d9e:	08 00 00 00 
  401da2:	62 f1 fd 48 7f b4 24 	vmovdqa64 ZMMWORD PTR [rsp+0x108],zmm6
  401da9:	08 01 00 00 
  401dad:	62 f1 fd 48 7f ac 24 	vmovdqa64 ZMMWORD PTR [rsp+0x48],zmm5
  401db4:	48 00 00 00 
  401db8:	62 f1 fd 48 7f a4 24 	vmovdqa64 ZMMWORD PTR [rsp+0x148],zmm4
  401dbf:	48 01 00 00 
  401dc3:	62 f1 e5 48 d4 8c 24 	vpaddq zmm1,zmm3,ZMMWORD PTR [rsp-0x38]
  401dca:	c8 ff ff ff 
  401dce:	62 f1 ed 48 d4 84 24 	vpaddq zmm0,zmm2,ZMMWORD PTR [rsp+0xc8]
  401dd5:	c8 00 00 00 
  401dd9:	62 f1 f5 48 d4 8c 24 	vpaddq zmm1,zmm1,ZMMWORD PTR [rsp+0x8]
  401de0:	08 00 00 00 
  401de4:	62 f1 fd 48 d4 84 24 	vpaddq zmm0,zmm0,ZMMWORD PTR [rsp+0x108]
  401deb:	08 01 00 00 
  401def:	62 f1 f5 48 d4 8c 24 	vpaddq zmm1,zmm1,ZMMWORD PTR [rsp+0x48]
  401df6:	48 00 00 00 
  401dfa:	62 f1 fd 48 d4 84 24 	vpaddq zmm0,zmm0,ZMMWORD PTR [rsp+0x148]
  401e01:	48 01 00 00 
  401e05:	62 f3 fd 48 3b ca 01 	vextracti64x4 ymm2,zmm1,0x1
  401e0c:	c5 ed d4 d1          	vpaddq ymm2,ymm2,ymm1
  401e10:	62 f3 fd 28 39 d1 01 	vextracti64x2 xmm1,ymm2,0x1
  401e17:	c5 f1 d4 ca          	vpaddq xmm1,xmm1,xmm2
  401e1b:	62 f3 fd 48 3b c2 01 	vextracti64x4 ymm2,zmm0,0x1
  401e22:	c5 ed d4 d0          	vpaddq ymm2,ymm2,ymm0
  401e26:	62 f3 fd 28 39 d0 01 	vextracti64x2 xmm0,ymm2,0x1
  401e2d:	c5 f9 d4 c2          	vpaddq xmm0,xmm0,xmm2
  401e31:	c4 e1 f9 7e c2       	vmovq  rdx,xmm0
  401e36:	c4 e3 f9 16 c0 01    	vpextrq rax,xmm0,0x1
  401e3c:	48 01 d0             	add    rax,rdx
  401e3f:	c4 e1 f9 7e c9       	vmovq  rcx,xmm1
  401e44:	c4 e3 f9 16 ca 01    	vpextrq rdx,xmm1,0x1
  401e4a:	48 01 ca             	add    rdx,rcx
  401e4d:	48 31 d0             	xor    rax,rdx
  401e50:	c5 f8 77             	vzeroupper 
  401e53:	c9                   	leave  
  401e54:	c3                   	ret    
  401e55:	c5 e9 ef d2          	vpxor  xmm2,xmm2,xmm2
  401e59:	62 f1 fd 48 6f da    	vmovdqa64 zmm3,zmm2
  401e5f:	e9 5f ff ff ff       	jmp    401dc3 <nh32_two_shuffle+0x2c3>
  401e64:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
  401e6b:	00 00 00 00 
  401e6f:	90                   	nop

0000000000401e70 <ifma52_one_packed>:
  401e70:	48 89 f1             	mov    rcx,rsi
  401e73:	48 89 d0             	mov    rax,rdx
  401e76:	48 be c5 4e ec c4 4e 	movabs rsi,0x4ec4ec4ec4ec4ec5
  401e7d:	ec c4 4e 
  401e80:	55                   	push   rbp
  401e81:	48 f7 e6             	mul    rsi
  401e84:	c5 f9 ef c0          	vpxor  xmm0,xmm0,xmm0
  401e88:	48 89 e5             	mov    rbp,rsp
  401e8b:	48 c1 ea 07          	shr    rdx,0x7
  401e8f:	48 83 e4 c0          	and    rsp,0xffffffffffffffc0
  401e93:	48 81 ec 88 03 00 00 	sub    rsp,0x388
  401e9a:	48 c1 e2 02          	shl    rdx,0x2
  401e9e:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x188],zmm0
  401ea5:	88 01 00 00 
  401ea9:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp-0x78],zmm0
  401eb0:	88 ff ff ff 
  401eb4:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x1c8],zmm0
  401ebb:	c8 01 00 00 
  401ebf:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp-0x38],zmm0
  401ec6:	c8 ff ff ff 
  401eca:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x208],zmm0
  401ed1:	08 02 00 00 
  401ed5:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x8],zmm0
  401edc:	08 00 00 00 
  401ee0:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x248],zmm0
  401ee7:	48 02 00 00 
  401eeb:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x48],zmm0
  401ef2:	48 00 00 00 
  401ef6:	0f 84 e2 04 00 00    	je     4023de <ifma52_one_packed+0x56e>
  401efc:	49 89 f9             	mov    r9,rdi
  401eff:	48 83 fa 08          	cmp    rdx,0x8
  401f03:	0f 86 67 06 00 00    	jbe    402570 <ifma52_one_packed+0x700>
  401f09:	48 8d 42 f7          	lea    rax,[rdx-0x9]
  401f0d:	48 c1 e8 03          	shr    rax,0x3
  401f11:	62 f1 fd 48 6f e0    	vmovdqa64 zmm4,zmm0
  401f17:	62 71 fd 48 6f c8    	vmovdqa64 zmm9,zmm0
  401f1d:	62 71 fd 48 6f d0    	vmovdqa64 zmm10,zmm0
  401f23:	62 71 fd 48 6f d8    	vmovdqa64 zmm11,zmm0
  401f29:	62 f1 fd 48 6f e8    	vmovdqa64 zmm5,zmm0
  401f2f:	62 71 fd 48 6f c0    	vmovdqa64 zmm8,zmm0
  401f35:	62 f1 fd 48 6f f0    	vmovdqa64 zmm6,zmm0
  401f3b:	62 f1 fd 48 6f f8    	vmovdqa64 zmm7,zmm0
  401f41:	62 f1 fd 48 6f 0d b5 	vmovdqa64 zmm1,ZMMWORD PTR [rip+0x33b5]        # 405300 <__dso_handle+0x2f8>
  401f48:	33 00 00 
  401f4b:	62 f1 fd 48 6f 05 6b 	vmovdqa64 zmm0,ZMMWORD PTR [rip+0x336b]        # 4052c0 <__dso_handle+0x2b8>
  401f52:	33 00 00 
  401f55:	4c 8d 14 c5 08 00 00 	lea    r10,[rax*8+0x8]
  401f5c:	00 
  401f5d:	31 f6                	xor    esi,esi
  401f5f:	62 f2 7d 48 8d 17    	vpermb zmm2,zmm0,ZMMWORD PTR [rdi]
  401f65:	62 f2 7d 48 8d 9f 34 	vpermb zmm3,zmm0,ZMMWORD PTR [rdi+0x34]
  401f6c:	00 00 00 
  401f6f:	48 89 f0             	mov    rax,rsi
  401f72:	62 f2 ed 48 45 d1    	vpsrlvq zmm2,zmm2,zmm1
  401f78:	62 f2 e5 48 45 d9    	vpsrlvq zmm3,zmm3,zmm1
  401f7e:	83 e0 07             	and    eax,0x7
  401f81:	48 c1 e0 07          	shl    rax,0x7
  401f85:	62 f1 ed 48 d4 14 01 	vpaddq zmm2,zmm2,ZMMWORD PTR [rcx+rax*1]
  401f8c:	62 f1 e5 48 d4 5c 01 	vpaddq zmm3,zmm3,ZMMWORD PTR [rcx+rax*1+0x40]
  401f93:	01 
  401f94:	48 8d 46 04          	lea    rax,[rsi+0x4]
  401f98:	62 f2 ed 48 b4 fb    	vpmadd52luq zmm7,zmm2,zmm3
  401f9e:	62 f2 ed 48 b5 f3    	vpmadd52huq zmm6,zmm2,zmm3
  401fa4:	62 f2 7d 48 8d 97 a0 	vpermb zmm2,zmm0,ZMMWORD PTR [rdi+0x1a0]
  401fab:	01 00 00 
  401fae:	62 f2 7d 48 8d 9f d4 	vpermb zmm3,zmm0,ZMMWORD PTR [rdi+0x1d4]
  401fb5:	01 00 00 
  401fb8:	62 f2 ed 48 45 d1    	vpsrlvq zmm2,zmm2,zmm1
  401fbe:	62 f2 e5 48 45 d9    	vpsrlvq zmm3,zmm3,zmm1
  401fc4:	83 e0 07             	and    eax,0x7
  401fc7:	48 c1 e0 07          	shl    rax,0x7
  401fcb:	62 f1 ed 48 d4 14 01 	vpaddq zmm2,zmm2,ZMMWORD PTR [rcx+rax*1]
  401fd2:	62 f1 e5 48 d4 5c 01 	vpaddq zmm3,zmm3,ZMMWORD PTR [rcx+rax*1+0x40]
  401fd9:	01 
  401fda:	48 8d 46 01          	lea    rax,[rsi+0x1]
  401fde:	62 f2 ed 48 b4 fb    	vpmadd52luq zmm7,zmm2,zmm3
  401fe4:	62 f2 ed 48 b5 f3    	vpmadd52huq zmm6,zmm2,zmm3
  401fea:	62 f2 7d 48 8d 97 68 	vpermb zmm2,zmm0,ZMMWORD PTR [rdi+0x68]
  401ff1:	00 00 00 
  401ff4:	62 f2 7d 48 8d 9f 9c 	vpermb zmm3,zmm0,ZMMWORD PTR [rdi+0x9c]
  401ffb:	00 00 00 
  401ffe:	62 f2 ed 48 45 d1    	vpsrlvq zmm2,zmm2,zmm1
  402004:	62 f2 e5 48 45 d9    	vpsrlvq zmm3,zmm3,zmm1
  40200a:	83 e0 07             	and    eax,0x7
  40200d:	48 c1 e0 07          	shl    rax,0x7
  402011:	62 f1 ed 48 d4 14 01 	vpaddq zmm2,zmm2,ZMMWORD PTR [rcx+rax*1]
  402018:	62 f1 e5 48 d4 5c 01 	vpaddq zmm3,zmm3,ZMMWORD PTR [rcx+rax*1+0x40]
  40201f:	01 
  402020:	48 8d 46 05          	lea    rax,[rsi+0x5]
  402024:	62 72 ed 48 b4 c3    	vpmadd52luq zmm8,zmm2,zmm3
  40202a:	62 f2 ed 48 b5 eb    	vpmadd52huq zmm5,zmm2,zmm3
  402030:	62 f2 7d 48 8d 97 08 	vpermb zmm2,zmm0,ZMMWORD PTR [rdi+0x208]
  402037:	02 00 00 
  40203a:	62 f2 7d 48 8d 9f 3c 	vpermb zmm3,zmm0,ZMMWORD PTR [rdi+0x23c]
  402041:	02 00 00 
  402044:	62 f2 ed 48 45 d1    	vpsrlvq zmm2,zmm2,zmm1
  40204a:	62 f2 e5 48 45 d9    	vpsrlvq zmm3,zmm3,zmm1
  402050:	83 e0 07             	and    eax,0x7
  402053:	48 c1 e0 07          	shl    rax,0x7
  402057:	62 f1 ed 48 d4 14 01 	vpaddq zmm2,zmm2,ZMMWORD PTR [rcx+rax*1]
  40205e:	62 f1 e5 48 d4 5c 01 	vpaddq zmm3,zmm3,ZMMWORD PTR [rcx+rax*1+0x40]
  402065:	01 
  402066:	48 8d 46 02          	lea    rax,[rsi+0x2]
  40206a:	62 72 ed 48 b4 c3    	vpmadd52luq zmm8,zmm2,zmm3
  402070:	62 f2 ed 48 b5 eb    	vpmadd52huq zmm5,zmm2,zmm3
  402076:	62 f2 7d 48 8d 97 d0 	vpermb zmm2,zmm0,ZMMWORD PTR [rdi+0xd0]
  40207d:	00 00 00 
  402080:	62 f2 7d 48 8d 9f 04 	vpermb zmm3,zmm0,ZMMWORD PTR [rdi+0x104]
  402087:	01 00 00 
  40208a:	62 f2 ed 48 45 d1    	vpsrlvq zmm2,zmm2,zmm1
  402090:	62 f2 e5 48 45 d9    	vpsrlvq zmm3,zmm3,zmm1
  402096:	83 e0 07             	and    eax,0x7
  402099:	48 c1 e0 07          	shl    rax,0x7
  40209d:	62 f1 ed 48 d4 14 01 	vpaddq zmm2,zmm2,ZMMWORD PTR [rcx+rax*1]
  4020a4:	62 f1 e5 48 d4 5c 01 	vpaddq zmm3,zmm3,ZMMWORD PTR [rcx+rax*1+0x40]
  4020ab:	01 
  4020ac:	48 8d 46 06          	lea    rax,[rsi+0x6]
  4020b0:	62 72 ed 48 b4 db    	vpmadd52luq zmm11,zmm2,zmm3
  4020b6:	62 72 ed 48 b5 d3    	vpmadd52huq zmm10,zmm2,zmm3
  4020bc:	62 f2 7d 48 8d 97 70 	vpermb zmm2,zmm0,ZMMWORD PTR [rdi+0x270]
  4020c3:	02 00 00 
  4020c6:	62 f2 7d 48 8d 9f a4 	vpermb zmm3,zmm0,ZMMWORD PTR [rdi+0x2a4]
  4020cd:	02 00 00 
  4020d0:	62 f2 ed 48 45 d1    	vpsrlvq zmm2,zmm2,zmm1
  4020d6:	62 f2 e5 48 45 d9    	vpsrlvq zmm3,zmm3,zmm1
  4020dc:	83 e0 07             	and    eax,0x7
  4020df:	48 c1 e0 07          	shl    rax,0x7
  4020e3:	62 f1 ed 48 d4 14 01 	vpaddq zmm2,zmm2,ZMMWORD PTR [rcx+rax*1]
  4020ea:	62 f1 e5 48 d4 5c 01 	vpaddq zmm3,zmm3,ZMMWORD PTR [rcx+rax*1+0x40]
  4020f1:	01 
  4020f2:	48 8d 46 03          	lea    rax,[rsi+0x3]
  4020f6:	62 72 ed 48 b4 db    	vpmadd52luq zmm11,zmm2,zmm3
  4020fc:	62 72 ed 48 b5 d3    	vpmadd52huq zmm10,zmm2,zmm3
  402102:	62 f2 7d 48 8d 97 38 	vpermb zmm2,zmm0,ZMMWORD PTR [rdi+0x138]
  402109:	01 00 00 
  40210c:	62 f2 7d 48 8d 9f 6c 	vpermb zmm3,zmm0,ZMMWORD PTR [rdi+0x16c]
  402113:	01 00 00 
  402116:	62 f2 ed 48 45 d1    	vpsrlvq zmm2,zmm2,zmm1
  40211c:	62 f2 e5 48 45 d9    	vpsrlvq zmm3,zmm3,zmm1
  402122:	83 e0 07             	and    eax,0x7
  402125:	48 c1 e0 07          	shl    rax,0x7
  402129:	62 f1 ed 48 d4 14 01 	vpaddq zmm2,zmm2,ZMMWORD PTR [rcx+rax*1]
  402130:	62 f1 e5 48 d4 5c 01 	vpaddq zmm3,zmm3,ZMMWORD PTR [rcx+rax*1+0x40]
  402137:	01 
  402138:	4c 8d 46 07          	lea    r8,[rsi+0x7]
  40213c:	62 72 ed 48 b4 cb    	vpmadd52luq zmm9,zmm2,zmm3
  402142:	62 f2 ed 48 b5 e3    	vpmadd52huq zmm4,zmm2,zmm3
  402148:	62 f2 7d 48 8d 97 d8 	vpermb zmm2,zmm0,ZMMWORD PTR [rdi+0x2d8]
  40214f:	02 00 00 
  402152:	62 f2 7d 48 8d 9f 0c 	vpermb zmm3,zmm0,ZMMWORD PTR [rdi+0x30c]
  402159:	03 00 00 
  40215c:	62 f2 ed 48 45 d1    	vpsrlvq zmm2,zmm2,zmm1
  402162:	62 f2 e5 48 45 d9    	vpsrlvq zmm3,zmm3,zmm1
  402168:	41 83 e0 07          	and    r8d,0x7
  40216c:	49 c1 e0 07          	shl    r8,0x7
  402170:	62 b1 ed 48 d4 14 01 	vpaddq zmm2,zmm2,ZMMWORD PTR [rcx+r8*1]
  402177:	62 b1 e5 48 d4 5c 01 	vpaddq zmm3,zmm3,ZMMWORD PTR [rcx+r8*1+0x40]
  40217e:	01 
  40217f:	48 83 c6 08          	add    rsi,0x8
  402183:	62 72 ed 48 b4 cb    	vpmadd52luq zmm9,zmm2,zmm3
  402189:	62 f2 ed 48 b5 e3    	vpmadd52huq zmm4,zmm2,zmm3
  40218f:	48 81 c7 40 03 00 00 	add    rdi,0x340
  402196:	4c 39 d6             	cmp    rsi,r10
  402199:	0f 85 c0 fd ff ff    	jne    401f5f <ifma52_one_packed+0xef>
  40219f:	62 f1 fd 48 7f bc 24 	vmovdqa64 ZMMWORD PTR [rsp-0x78],zmm7
  4021a6:	88 ff ff ff 
  4021aa:	62 f1 fd 48 7f b4 24 	vmovdqa64 ZMMWORD PTR [rsp+0x188],zmm6
  4021b1:	88 01 00 00 
  4021b5:	62 71 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp-0x38],zmm8
  4021bc:	c8 ff ff ff 
  4021c0:	62 f1 fd 48 7f ac 24 	vmovdqa64 ZMMWORD PTR [rsp+0x1c8],zmm5
  4021c7:	c8 01 00 00 
  4021cb:	62 71 fd 48 7f 9c 24 	vmovdqa64 ZMMWORD PTR [rsp+0x8],zmm11
  4021d2:	08 00 00 00 
  4021d6:	62 71 fd 48 7f 94 24 	vmovdqa64 ZMMWORD PTR [rsp+0x208],zmm10
  4021dd:	08 02 00 00 
  4021e1:	62 71 fd 48 7f 8c 24 	vmovdqa64 ZMMWORD PTR [rsp+0x48],zmm9
  4021e8:	48 00 00 00 
  4021ec:	62 f1 fd 48 7f a4 24 	vmovdqa64 ZMMWORD PTR [rsp+0x248],zmm4
  4021f3:	48 02 00 00 
  4021f7:	48 6b c6 68          	imul   rax,rsi,0x68
  4021fb:	62 71 fd 48 6f 9c 24 	vmovdqa64 zmm11,ZMMWORD PTR [rsp-0x78]
  402202:	88 ff ff ff 
  402206:	62 71 fd 48 6f 94 24 	vmovdqa64 zmm10,ZMMWORD PTR [rsp+0x188]
  40220d:	88 01 00 00 
  402211:	62 71 fd 48 6f 8c 24 	vmovdqa64 zmm9,ZMMWORD PTR [rsp-0x38]
  402218:	c8 ff ff ff 
  40221c:	62 71 fd 48 6f 84 24 	vmovdqa64 zmm8,ZMMWORD PTR [rsp+0x1c8]
  402223:	c8 01 00 00 
  402227:	62 f1 fd 48 6f bc 24 	vmovdqa64 zmm7,ZMMWORD PTR [rsp+0x8]
  40222e:	08 00 00 00 
  402232:	62 f1 fd 48 6f b4 24 	vmovdqa64 zmm6,ZMMWORD PTR [rsp+0x208]
  402239:	08 02 00 00 
  40223d:	62 f1 fd 48 6f ac 24 	vmovdqa64 zmm5,ZMMWORD PTR [rsp+0x48]
  402244:	48 00 00 00 
  402248:	62 f1 fd 48 6f a4 24 	vmovdqa64 zmm4,ZMMWORD PTR [rsp+0x248]
  40224f:	48 02 00 00 
  402253:	4c 01 c8             	add    rax,r9
  402256:	66 2e 0f 1f 84 00 00 	nop    WORD PTR cs:[rax+rax*1+0x0]
  40225d:	00 00 00 
  402260:	62 f2 7d 48 8d 10    	vpermb zmm2,zmm0,ZMMWORD PTR [rax]
  402266:	62 f2 7d 48 8d 98 34 	vpermb zmm3,zmm0,ZMMWORD PTR [rax+0x34]
  40226d:	00 00 00 
  402270:	48 89 f7             	mov    rdi,rsi
  402273:	62 f2 ed 48 45 d1    	vpsrlvq zmm2,zmm2,zmm1
  402279:	62 f2 e5 48 45 d9    	vpsrlvq zmm3,zmm3,zmm1
  40227f:	83 e7 07             	and    edi,0x7
  402282:	48 c1 e7 07          	shl    rdi,0x7
  402286:	62 f1 ed 48 d4 14 39 	vpaddq zmm2,zmm2,ZMMWORD PTR [rcx+rdi*1]
  40228d:	62 f1 e5 48 d4 5c 39 	vpaddq zmm3,zmm3,ZMMWORD PTR [rcx+rdi*1+0x40]
  402294:	01 
  402295:	48 8d 7e 01          	lea    rdi,[rsi+0x1]
  402299:	62 72 ed 48 b4 db    	vpmadd52luq zmm11,zmm2,zmm3
  40229f:	62 72 ed 48 b5 d3    	vpmadd52huq zmm10,zmm2,zmm3
  4022a5:	62 f2 7d 48 8d 90 68 	vpermb zmm2,zmm0,ZMMWORD PTR [rax+0x68]
  4022ac:	00 00 00 
  4022af:	62 f2 7d 48 8d 98 9c 	vpermb zmm3,zmm0,ZMMWORD PTR [rax+0x9c]
  4022b6:	00 00 00 
  4022b9:	62 f2 ed 48 45 d1    	vpsrlvq zmm2,zmm2,zmm1
  4022bf:	62 f2 e5 48 45 d9    	vpsrlvq zmm3,zmm3,zmm1
  4022c5:	83 e7 07             	and    edi,0x7
  4022c8:	48 c1 e7 07          	shl    rdi,0x7
  4022cc:	62 f1 ed 48 d4 14 39 	vpaddq zmm2,zmm2,ZMMWORD PTR [rcx+rdi*1]
  4022d3:	62 f1 e5 48 d4 5c 39 	vpaddq zmm3,zmm3,ZMMWORD PTR [rcx+rdi*1+0x40]
  4022da:	01 
  4022db:	48 8d 7e 02          	lea    rdi,[rsi+0x2]
  4022df:	62 72 ed 48 b4 cb    	vpmadd52luq zmm9,zmm2,zmm3
  4022e5:	62 72 ed 48 b5 c3    	vpmadd52huq zmm8,zmm2,zmm3
  4022eb:	62 f2 7d 48 8d 90 d0 	vpermb zmm2,zmm0,ZMMWORD PTR [rax+0xd0]
  4022f2:	00 00 00 
  4022f5:	62 f2 7d 48 8d 98 04 	vpermb zmm3,zmm0,ZMMWORD PTR [rax+0x104]
  4022fc:	01 00 00 
  4022ff:	62 f2 ed 48 45 d1    	vpsrlvq zmm2,zmm2,zmm1
  402305:	62 f2 e5 48 45 d9    	vpsrlvq zmm3,zmm3,zmm1
  40230b:	83 e7 07             	and    edi,0x7
  40230e:	48 c1 e7 07          	shl    rdi,0x7
  402312:	62 f1 ed 48 d4 14 39 	vpaddq zmm2,zmm2,ZMMWORD PTR [rcx+rdi*1]
  402319:	62 f1 e5 48 d4 5c 39 	vpaddq zmm3,zmm3,ZMMWORD PTR [rcx+rdi*1+0x40]
  402320:	01 
  402321:	48 8d 7e 03          	lea    rdi,[rsi+0x3]
  402325:	62 f2 ed 48 b4 fb    	vpmadd52luq zmm7,zmm2,zmm3
  40232b:	62 f2 ed 48 b5 f3    	vpmadd52huq zmm6,zmm2,zmm3
  402331:	62 f2 7d 48 8d 90 38 	vpermb zmm2,zmm0,ZMMWORD PTR [rax+0x138]
  402338:	01 00 00 
  40233b:	62 f2 7d 48 8d 98 6c 	vpermb zmm3,zmm0,ZMMWORD PTR [rax+0x16c]
  402342:	01 00 00 
  402345:	62 f2 ed 48 45 d1    	vpsrlvq zmm2,zmm2,zmm1
  40234b:	62 f2 e5 48 45 d9    	vpsrlvq zmm3,zmm3,zmm1
  402351:	83 e7 07             	and    edi,0x7
  402354:	48 c1 e7 07          	shl    rdi,0x7
  402358:	62 f1 ed 48 d4 14 39 	vpaddq zmm2,zmm2,ZMMWORD PTR [rcx+rdi*1]
  40235f:	62 f1 e5 48 d4 5c 39 	vpaddq zmm3,zmm3,ZMMWORD PTR [rcx+rdi*1+0x40]
  402366:	01 
  402367:	48 83 c6 04          	add    rsi,0x4
  40236b:	62 f2 ed 48 b4 eb    	vpmadd52luq zmm5,zmm2,zmm3
  402371:	62 f2 ed 48 b5 e3    	vpmadd52huq zmm4,zmm2,zmm3
  402377:	48 05 a0 01 00 00    	add    rax,0x1a0
  40237d:	48 39 f2             	cmp    rdx,rsi
  402380:	0f 87 da fe ff ff    	ja     402260 <ifma52_one_packed+0x3f0>
  402386:	62 71 fd 48 7f 9c 24 	vmovdqa64 ZMMWORD PTR [rsp-0x78],zmm11
  40238d:	88 ff ff ff 
  402391:	62 71 fd 48 7f 94 24 	vmovdqa64 ZMMWORD PTR [rsp+0x188],zmm10
  402398:	88 01 00 00 
  40239c:	62 71 fd 48 7f 8c 24 	vmovdqa64 ZMMWORD PTR [rsp-0x38],zmm9
  4023a3:	c8 ff ff ff 
  4023a7:	62 71 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x1c8],zmm8
  4023ae:	c8 01 00 00 
  4023b2:	62 f1 fd 48 7f bc 24 	vmovdqa64 ZMMWORD PTR [rsp+0x8],zmm7
  4023b9:	08 00 00 00 
  4023bd:	62 f1 fd 48 7f b4 24 	vmovdqa64 ZMMWORD PTR [rsp+0x208],zmm6
  4023c4:	08 02 00 00 
  4023c8:	62 f1 fd 48 7f ac 24 	vmovdqa64 ZMMWORD PTR [rsp+0x48],zmm5
  4023cf:	48 00 00 00 
  4023d3:	62 f1 fd 48 7f a4 24 	vmovdqa64 ZMMWORD PTR [rsp+0x248],zmm4
  4023da:	48 02 00 00 
  4023de:	62 f1 fd 48 6f 8c 24 	vmovdqa64 zmm1,ZMMWORD PTR [rsp-0x78]
  4023e5:	88 ff ff ff 
  4023e9:	62 f3 fd 48 3b c8 01 	vextracti64x4 ymm0,zmm1,0x1
  4023f0:	c5 fd d4 c1          	vpaddq ymm0,ymm0,ymm1
  4023f4:	62 f1 fd 48 6f 8c 24 	vmovdqa64 zmm1,ZMMWORD PTR [rsp+0x188]
  4023fb:	88 01 00 00 
  4023ff:	62 f3 fd 28 39 c6 01 	vextracti64x2 xmm6,ymm0,0x1
  402406:	c5 c9 d4 f0          	vpaddq xmm6,xmm6,xmm0
  40240a:	62 f3 fd 48 3b c8 01 	vextracti64x4 ymm0,zmm1,0x1
  402411:	c5 fd d4 c1          	vpaddq ymm0,ymm0,ymm1
  402415:	62 f1 fd 48 6f 8c 24 	vmovdqa64 zmm1,ZMMWORD PTR [rsp-0x38]
  40241c:	c8 ff ff ff 
  402420:	62 d3 fd 28 39 c0 01 	vextracti64x2 xmm8,ymm0,0x1
  402427:	c5 39 d4 c0          	vpaddq xmm8,xmm8,xmm0
  40242b:	62 f3 fd 48 3b c8 01 	vextracti64x4 ymm0,zmm1,0x1
  402432:	c5 fd d4 c1          	vpaddq ymm0,ymm0,ymm1
  402436:	62 f1 fd 48 6f 8c 24 	vmovdqa64 zmm1,ZMMWORD PTR [rsp+0x1c8]
  40243d:	c8 01 00 00 
  402441:	62 f3 fd 28 39 c5 01 	vextracti64x2 xmm5,ymm0,0x1
  402448:	c5 d1 d4 e8          	vpaddq xmm5,xmm5,xmm0
  40244c:	62 f3 fd 48 3b c8 01 	vextracti64x4 ymm0,zmm1,0x1
  402453:	c5 fd d4 c1          	vpaddq ymm0,ymm0,ymm1
  402457:	62 f1 fd 48 6f 8c 24 	vmovdqa64 zmm1,ZMMWORD PTR [rsp+0x8]
  40245e:	08 00 00 00 
  402462:	62 f3 fd 28 39 c4 01 	vextracti64x2 xmm4,ymm0,0x1
  402469:	c5 d9 d4 e0          	vpaddq xmm4,xmm4,xmm0
  40246d:	62 f3 fd 48 3b c8 01 	vextracti64x4 ymm0,zmm1,0x1
  402474:	c5 fd d4 c1          	vpaddq ymm0,ymm0,ymm1
  402478:	62 f1 fd 48 6f 8c 24 	vmovdqa64 zmm1,ZMMWORD PTR [rsp+0x208]
  40247f:	08 02 00 00 
  402483:	c4 e3 f9 16 f2 01    	vpextrq rdx,xmm6,0x1
  402489:	62 f3 fd 28 39 c3 01 	vextracti64x2 xmm3,ymm0,0x1
  402490:	c4 e1 f9 7e f0       	vmovq  rax,xmm6
  402495:	c5 e1 d4 d8          	vpaddq xmm3,xmm3,xmm0
  402499:	48 01 d0             	add    rax,rdx
  40249c:	c4 61 f9 7e c1       	vmovq  rcx,xmm8
  4024a1:	62 f3 fd 48 3b c8 01 	vextracti64x4 ymm0,zmm1,0x1
  4024a8:	c4 63 f9 16 c2 01    	vpextrq rdx,xmm8,0x1
  4024ae:	c5 fd d4 c1          	vpaddq ymm0,ymm0,ymm1
  4024b2:	48 01 ca             	add    rdx,rcx
  4024b5:	62 f1 fd 48 6f 8c 24 	vmovdqa64 zmm1,ZMMWORD PTR [rsp+0x48]
  4024bc:	48 00 00 00 
  4024c0:	48 31 d0             	xor    rax,rdx
  4024c3:	c4 e1 f9 7e e9       	vmovq  rcx,xmm5
  4024c8:	62 f3 fd 28 39 c2 01 	vextracti64x2 xmm2,ymm0,0x1
  4024cf:	c4 e3 f9 16 ea 01    	vpextrq rdx,xmm5,0x1
  4024d5:	c5 e9 d4 d0          	vpaddq xmm2,xmm2,xmm0
  4024d9:	48 01 ca             	add    rdx,rcx
  4024dc:	62 f3 fd 48 3b c8 01 	vextracti64x4 ymm0,zmm1,0x1
  4024e3:	c5 fd d4 c1          	vpaddq ymm0,ymm0,ymm1
  4024e7:	48 31 d0             	xor    rax,rdx
  4024ea:	c4 e1 f9 7e e1       	vmovq  rcx,xmm4
  4024ef:	c4 e3 f9 16 e2 01    	vpextrq rdx,xmm4,0x1
  4024f5:	48 01 ca             	add    rdx,rcx
  4024f8:	62 f3 fd 28 39 c1 01 	vextracti64x2 xmm1,ymm0,0x1
  4024ff:	c5 f1 d4 c8          	vpaddq xmm1,xmm1,xmm0
  402503:	48 31 d0             	xor    rax,rdx
  402506:	62 f1 fd 48 6f 84 24 	vmovdqa64 zmm0,ZMMWORD PTR [rsp+0x248]
  40250d:	48 02 00 00 
  402511:	c4 e1 f9 7e d9       	vmovq  rcx,xmm3
  402516:	c4 e3 f9 16 da 01    	vpextrq rdx,xmm3,0x1
  40251c:	48 01 ca             	add    rdx,rcx
  40251f:	48 31 d0             	xor    rax,rdx
  402522:	c4 e1 f9 7e d1       	vmovq  rcx,xmm2
  402527:	62 f3 fd 48 3b c7 01 	vextracti64x4 ymm7,zmm0,0x1
  40252e:	c4 e3 f9 16 d2 01    	vpextrq rdx,xmm2,0x1
  402534:	48 01 ca             	add    rdx,rcx
  402537:	c5 c5 d4 f8          	vpaddq ymm7,ymm7,ymm0
  40253b:	48 31 d0             	xor    rax,rdx
  40253e:	c4 e1 f9 7e c9       	vmovq  rcx,xmm1
  402543:	62 f3 fd 28 39 f8 01 	vextracti64x2 xmm0,ymm7,0x1
  40254a:	c4 e3 f9 16 ca 01    	vpextrq rdx,xmm1,0x1
  402550:	48 01 ca             	add    rdx,rcx
  402553:	c5 f9 d4 c7          	vpaddq xmm0,xmm0,xmm7
  402557:	48 31 d0             	xor    rax,rdx
  40255a:	c4 e1 f9 7e c1       	vmovq  rcx,xmm0
  40255f:	c4 e3 f9 16 c2 01    	vpextrq rdx,xmm0,0x1
  402565:	48 01 ca             	add    rdx,rcx
  402568:	48 31 d0             	xor    rax,rdx
  40256b:	c5 f8 77             	vzeroupper 
  40256e:	c9                   	leave  
  40256f:	c3                   	ret    
  402570:	62 f1 fd 48 6f 05 46 	vmovdqa64 zmm0,ZMMWORD PTR [rip+0x2d46]        # 4052c0 <__dso_handle+0x2b8>
  402577:	2d 00 00 
  40257a:	62 f1 fd 48 6f 0d 7c 	vmovdqa64 zmm1,ZMMWORD PTR [rip+0x2d7c]        # 405300 <__dso_handle+0x2f8>
  402581:	2d 00 00 
  402584:	31 f6                	xor    esi,esi
  402586:	e9 6c fc ff ff       	jmp    4021f7 <ifma52_one_packed+0x387>
  40258b:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]

0000000000402590 <ifma52_two_packed>:
  402590:	49 89 d0             	mov    r8,rdx
  402593:	4c 89 c0             	mov    rax,r8
  402596:	48 ba c5 4e ec c4 4e 	movabs rdx,0x4ec4ec4ec4ec4ec5
  40259d:	ec c4 4e 
  4025a0:	48 f7 e2             	mul    rdx
  4025a3:	55                   	push   rbp
  4025a4:	c5 f9 ef c0          	vpxor  xmm0,xmm0,xmm0
  4025a8:	49 89 d0             	mov    r8,rdx
  4025ab:	48 89 e5             	mov    rbp,rsp
  4025ae:	49 c1 e8 07          	shr    r8,0x7
  4025b2:	48 83 e4 c0          	and    rsp,0xffffffffffffffc0
  4025b6:	48 81 ec 88 03 00 00 	sub    rsp,0x388
  4025bd:	49 c1 e0 02          	shl    r8,0x2
  4025c1:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x188],zmm0
  4025c8:	88 01 00 00 
  4025cc:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp-0x78],zmm0
  4025d3:	88 ff ff ff 
  4025d7:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x1c8],zmm0
  4025de:	c8 01 00 00 
  4025e2:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp-0x38],zmm0
  4025e9:	c8 ff ff ff 
  4025ed:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x208],zmm0
  4025f4:	08 02 00 00 
  4025f8:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x8],zmm0
  4025ff:	08 00 00 00 
  402603:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x248],zmm0
  40260a:	48 02 00 00 
  40260e:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x48],zmm0
  402615:	48 00 00 00 
  402619:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x288],zmm0
  402620:	88 02 00 00 
  402624:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x88],zmm0
  40262b:	88 00 00 00 
  40262f:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x2c8],zmm0
  402636:	c8 02 00 00 
  40263a:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0xc8],zmm0
  402641:	c8 00 00 00 
  402645:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x308],zmm0
  40264c:	08 03 00 00 
  402650:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x108],zmm0
  402657:	08 01 00 00 
  40265b:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x348],zmm0
  402662:	48 03 00 00 
  402666:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x148],zmm0
  40266d:	48 01 00 00 
  402671:	0f 84 c7 02 00 00    	je     40293e <ifma52_two_packed+0x3ae>
  402677:	62 f1 fd 48 6f e0    	vmovdqa64 zmm4,zmm0
  40267d:	62 f1 fd 48 6f e8    	vmovdqa64 zmm5,zmm0
  402683:	62 f1 fd 48 6f f0    	vmovdqa64 zmm6,zmm0
  402689:	62 f1 fd 48 6f f8    	vmovdqa64 zmm7,zmm0
  40268f:	62 71 fd 48 6f c0    	vmovdqa64 zmm8,zmm0
  402695:	62 71 fd 48 6f c8    	vmovdqa64 zmm9,zmm0
  40269b:	62 71 fd 48 6f d0    	vmovdqa64 zmm10,zmm0
  4026a1:	62 71 fd 48 6f d8    	vmovdqa64 zmm11,zmm0
  4026a7:	62 71 fd 48 6f e0    	vmovdqa64 zmm12,zmm0
  4026ad:	62 71 fd 48 6f e8    	vmovdqa64 zmm13,zmm0
  4026b3:	62 71 fd 48 6f f0    	vmovdqa64 zmm14,zmm0
  4026b9:	62 71 fd 48 6f f8    	vmovdqa64 zmm15,zmm0
  4026bf:	62 e1 fd 48 6f c0    	vmovdqa64 zmm16,zmm0
  4026c5:	62 e1 fd 48 6f c8    	vmovdqa64 zmm17,zmm0
  4026cb:	62 e1 fd 48 6f d0    	vmovdqa64 zmm18,zmm0
  4026d1:	62 e1 fd 48 6f d8    	vmovdqa64 zmm19,zmm0
  4026d7:	62 f1 fd 48 6f 0d df 	vmovdqa64 zmm1,ZMMWORD PTR [rip+0x2bdf]        # 4052c0 <__dso_handle+0x2b8>
  4026de:	2b 00 00 
  4026e1:	62 f1 fd 48 6f 05 15 	vmovdqa64 zmm0,ZMMWORD PTR [rip+0x2c15]        # 405300 <__dso_handle+0x2f8>
  4026e8:	2c 00 00 
  4026eb:	48 89 f1             	mov    rcx,rsi
  4026ee:	48 89 f8             	mov    rax,rdi
  4026f1:	31 f6                	xor    esi,esi
  4026f3:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
  4026f8:	62 f2 75 48 8d 10    	vpermb zmm2,zmm1,ZMMWORD PTR [rax]
  4026fe:	62 f2 75 48 8d 98 34 	vpermb zmm3,zmm1,ZMMWORD PTR [rax+0x34]
  402705:	00 00 00 
  402708:	48 89 f2             	mov    rdx,rsi
  40270b:	62 f2 ed 48 45 d0    	vpsrlvq zmm2,zmm2,zmm0
  402711:	62 f2 e5 48 45 d8    	vpsrlvq zmm3,zmm3,zmm0
  402717:	83 e2 07             	and    edx,0x7
  40271a:	48 c1 e2 07          	shl    rdx,0x7
  40271e:	62 e1 ed 48 d4 24 11 	vpaddq zmm20,zmm2,ZMMWORD PTR [rcx+rdx*1]
  402725:	62 e1 e5 48 d4 6c 11 	vpaddq zmm21,zmm3,ZMMWORD PTR [rcx+rdx*1+0x40]
  40272c:	01 
  40272d:	62 f1 ed 48 d4 54 11 	vpaddq zmm2,zmm2,ZMMWORD PTR [rcx+rdx*1+0x400]
  402734:	10 
  402735:	62 f1 e5 48 d4 5c 11 	vpaddq zmm3,zmm3,ZMMWORD PTR [rcx+rdx*1+0x440]
  40273c:	11 
  40273d:	48 8d 56 01          	lea    rdx,[rsi+0x1]
  402741:	62 e2 ed 48 b4 cb    	vpmadd52luq zmm17,zmm2,zmm3
  402747:	62 e2 ed 48 b5 c3    	vpmadd52huq zmm16,zmm2,zmm3
  40274d:	62 f2 75 48 8d 90 68 	vpermb zmm2,zmm1,ZMMWORD PTR [rax+0x68]
  402754:	00 00 00 
  402757:	62 f2 75 48 8d 98 9c 	vpermb zmm3,zmm1,ZMMWORD PTR [rax+0x9c]
  40275e:	00 00 00 
  402761:	62 f2 ed 48 45 d0    	vpsrlvq zmm2,zmm2,zmm0
  402767:	62 f2 e5 48 45 d8    	vpsrlvq zmm3,zmm3,zmm0
  40276d:	83 e2 07             	and    edx,0x7
  402770:	48 c1 e2 07          	shl    rdx,0x7
  402774:	62 a2 dd 40 b4 dd    	vpmadd52luq zmm19,zmm20,zmm21
  40277a:	62 a2 dd 40 b5 d5    	vpmadd52huq zmm18,zmm20,zmm21
  402780:	62 e1 ed 48 d4 24 11 	vpaddq zmm20,zmm2,ZMMWORD PTR [rcx+rdx*1]
  402787:	62 e1 e5 48 d4 6c 11 	vpaddq zmm21,zmm3,ZMMWORD PTR [rcx+rdx*1+0x40]
  40278e:	01 
  40278f:	62 f1 ed 48 d4 54 11 	vpaddq zmm2,zmm2,ZMMWORD PTR [rcx+rdx*1+0x400]
  402796:	10 
  402797:	62 f1 e5 48 d4 5c 11 	vpaddq zmm3,zmm3,ZMMWORD PTR [rcx+rdx*1+0x440]
  40279e:	11 
  40279f:	48 8d 56 02          	lea    rdx,[rsi+0x2]
  4027a3:	62 72 ed 48 b4 eb    	vpmadd52luq zmm13,zmm2,zmm3
  4027a9:	62 72 ed 48 b5 e3    	vpmadd52huq zmm12,zmm2,zmm3
  4027af:	62 f2 75 48 8d 90 d0 	vpermb zmm2,zmm1,ZMMWORD PTR [rax+0xd0]
  4027b6:	00 00 00 
  4027b9:	62 f2 75 48 8d 98 04 	vpermb zmm3,zmm1,ZMMWORD PTR [rax+0x104]
  4027c0:	01 00 00 
  4027c3:	62 f2 ed 48 45 d0    	vpsrlvq zmm2,zmm2,zmm0
  4027c9:	62 f2 e5 48 45 d8    	vpsrlvq zmm3,zmm3,zmm0
  4027cf:	83 e2 07             	and    edx,0x7
  4027d2:	48 c1 e2 07          	shl    rdx,0x7
  4027d6:	62 32 dd 40 b4 fd    	vpmadd52luq zmm15,zmm20,zmm21
  4027dc:	62 32 dd 40 b5 f5    	vpmadd52huq zmm14,zmm20,zmm21
  4027e2:	62 e1 ed 48 d4 24 11 	vpaddq zmm20,zmm2,ZMMWORD PTR [rcx+rdx*1]
  4027e9:	62 e1 e5 48 d4 6c 11 	vpaddq zmm21,zmm3,ZMMWORD PTR [rcx+rdx*1+0x40]
  4027f0:	01 
  4027f1:	62 f1 ed 48 d4 54 11 	vpaddq zmm2,zmm2,ZMMWORD PTR [rcx+rdx*1+0x400]
  4027f8:	10 
  4027f9:	62 f1 e5 48 d4 5c 11 	vpaddq zmm3,zmm3,ZMMWORD PTR [rcx+rdx*1+0x440]
  402800:	11 
  402801:	48 8d 56 03          	lea    rdx,[rsi+0x3]
  402805:	62 72 ed 48 b4 cb    	vpmadd52luq zmm9,zmm2,zmm3
  40280b:	62 72 ed 48 b5 c3    	vpmadd52huq zmm8,zmm2,zmm3
  402811:	62 f2 75 48 8d 90 38 	vpermb zmm2,zmm1,ZMMWORD PTR [rax+0x138]
  402818:	01 00 00 
  40281b:	62 f2 75 48 8d 98 6c 	vpermb zmm3,zmm1,ZMMWORD PTR [rax+0x16c]
  402822:	01 00 00 
  402825:	62 f2 ed 48 45 d0    	vpsrlvq zmm2,zmm2,zmm0
  40282b:	62 f2 e5 48 45 d8    	vpsrlvq zmm3,zmm3,zmm0
  402831:	83 e2 07             	and    edx,0x7
  402834:	48 c1 e2 07          	shl    rdx,0x7
  402838:	62 32 dd 40 b4 dd    	vpmadd52luq zmm11,zmm20,zmm21
  40283e:	62 32 dd 40 b5 d5    	vpmadd52huq zmm10,zmm20,zmm21
  402844:	62 e1 ed 48 d4 24 11 	vpaddq zmm20,zmm2,ZMMWORD PTR [rcx+rdx*1]
  40284b:	62 e1 e5 48 d4 6c 11 	vpaddq zmm21,zmm3,ZMMWORD PTR [rcx+rdx*1+0x40]
  402852:	01 
  402853:	62 f1 ed 48 d4 54 11 	vpaddq zmm2,zmm2,ZMMWORD PTR [rcx+rdx*1+0x400]
  40285a:	10 
  40285b:	62 f1 e5 48 d4 5c 11 	vpaddq zmm3,zmm3,ZMMWORD PTR [rcx+rdx*1+0x440]
  402862:	11 
  402863:	48 83 c6 04          	add    rsi,0x4
  402867:	62 b2 dd 40 b4 fd    	vpmadd52luq zmm7,zmm20,zmm21
  40286d:	62 b2 dd 40 b5 f5    	vpmadd52huq zmm6,zmm20,zmm21
  402873:	62 f2 ed 48 b4 eb    	vpmadd52luq zmm5,zmm2,zmm3
  402879:	62 f2 ed 48 b5 e3    	vpmadd52huq zmm4,zmm2,zmm3
  40287f:	48 05 a0 01 00 00    	add    rax,0x1a0
  402885:	49 39 f0             	cmp    r8,rsi
  402888:	0f 87 6a fe ff ff    	ja     4026f8 <ifma52_two_packed+0x168>
  40288e:	62 e1 fd 48 7f 9c 24 	vmovdqa64 ZMMWORD PTR [rsp-0x78],zmm19
  402895:	88 ff ff ff 
  402899:	62 e1 fd 48 7f 94 24 	vmovdqa64 ZMMWORD PTR [rsp+0x188],zmm18
  4028a0:	88 01 00 00 
  4028a4:	62 e1 fd 48 7f 8c 24 	vmovdqa64 ZMMWORD PTR [rsp+0x88],zmm17
  4028ab:	88 00 00 00 
  4028af:	62 e1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x288],zmm16
  4028b6:	88 02 00 00 
  4028ba:	62 71 fd 48 7f bc 24 	vmovdqa64 ZMMWORD PTR [rsp-0x38],zmm15
  4028c1:	c8 ff ff ff 
  4028c5:	62 71 fd 48 7f b4 24 	vmovdqa64 ZMMWORD PTR [rsp+0x1c8],zmm14
  4028cc:	c8 01 00 00 
  4028d0:	62 71 fd 48 7f ac 24 	vmovdqa64 ZMMWORD PTR [rsp+0xc8],zmm13
  4028d7:	c8 00 00 00 
  4028db:	62 71 fd 48 7f a4 24 	vmovdqa64 ZMMWORD PTR [rsp+0x2c8],zmm12
  4028e2:	c8 02 00 00 
  4028e6:	62 71 fd 48 7f 9c 24 	vmovdqa64 ZMMWORD PTR [rsp+0x8],zmm11
  4028ed:	08 00 00 00 
  4028f1:	62 71 fd 48 7f 94 24 	vmovdqa64 ZMMWORD PTR [rsp+0x208],zmm10
  4028f8:	08 02 00 00 
  4028fc:	62 71 fd 48 7f 8c 24 	vmovdqa64 ZMMWORD PTR [rsp+0x108],zmm9
  402903:	08 01 00 00 
  402907:	62 71 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x308],zmm8
  40290e:	08 03 00 00 
  402912:	62 f1 fd 48 7f bc 24 	vmovdqa64 ZMMWORD PTR [rsp+0x48],zmm7
  402919:	48 00 00 00 
  40291d:	62 f1 fd 48 7f b4 24 	vmovdqa64 ZMMWORD PTR [rsp+0x248],zmm6
  402924:	48 02 00 00 
  402928:	62 f1 fd 48 7f ac 24 	vmovdqa64 ZMMWORD PTR [rsp+0x148],zmm5
  40292f:	48 01 00 00 
  402933:	62 f1 fd 48 7f a4 24 	vmovdqa64 ZMMWORD PTR [rsp+0x348],zmm4
  40293a:	48 03 00 00 
  40293e:	62 f1 fd 48 6f 84 24 	vmovdqa64 zmm0,ZMMWORD PTR [rsp-0x78]
  402945:	88 ff ff ff 
  402949:	62 71 fd 48 6f 8c 24 	vmovdqa64 zmm9,ZMMWORD PTR [rsp+0x88]
  402950:	88 00 00 00 
  402954:	62 f3 fd 48 3b c1 01 	vextracti64x4 ymm1,zmm0,0x1
  40295b:	c5 fd d4 c1          	vpaddq ymm0,ymm0,ymm1
  40295f:	62 f3 fd 28 39 c1 01 	vextracti64x2 xmm1,ymm0,0x1
  402966:	c5 f9 d4 f1          	vpaddq xmm6,xmm0,xmm1
  40296a:	62 f1 fd 48 6f 84 24 	vmovdqa64 zmm0,ZMMWORD PTR [rsp+0x188]
  402971:	88 01 00 00 
  402975:	c4 e3 f9 16 f6 01    	vpextrq rsi,xmm6,0x1
  40297b:	62 f3 fd 48 3b c1 01 	vextracti64x4 ymm1,zmm0,0x1
  402982:	c5 fd d4 c1          	vpaddq ymm0,ymm0,ymm1
  402986:	62 f3 fd 28 39 c1 01 	vextracti64x2 xmm1,ymm0,0x1
  40298d:	c5 f9 d4 f9          	vpaddq xmm7,xmm0,xmm1
  402991:	62 f1 fd 48 6f 84 24 	vmovdqa64 zmm0,ZMMWORD PTR [rsp-0x38]
  402998:	c8 ff ff ff 
  40299c:	62 f3 fd 48 3b c1 01 	vextracti64x4 ymm1,zmm0,0x1
  4029a3:	c5 fd d4 c1          	vpaddq ymm0,ymm0,ymm1
  4029a7:	62 f3 fd 28 39 c1 01 	vextracti64x2 xmm1,ymm0,0x1
  4029ae:	c5 f9 d4 e9          	vpaddq xmm5,xmm0,xmm1
  4029b2:	62 f1 fd 48 6f 84 24 	vmovdqa64 zmm0,ZMMWORD PTR [rsp+0x1c8]
  4029b9:	c8 01 00 00 
  4029bd:	62 f3 fd 48 3b c1 01 	vextracti64x4 ymm1,zmm0,0x1
  4029c4:	c5 fd d4 c1          	vpaddq ymm0,ymm0,ymm1
  4029c8:	62 f3 fd 28 39 c1 01 	vextracti64x2 xmm1,ymm0,0x1
  4029cf:	c5 f9 d4 e1          	vpaddq xmm4,xmm0,xmm1
  4029d3:	62 f1 fd 48 6f 84 24 	vmovdqa64 zmm0,ZMMWORD PTR [rsp+0x8]
  4029da:	08 00 00 00 
  4029de:	62 f3 fd 48 3b c1 01 	vextracti64x4 ymm1,zmm0,0x1
  4029e5:	c5 fd d4 c1          	vpaddq ymm0,ymm0,ymm1
  4029e9:	62 f3 fd 28 39 c1 01 	vextracti64x2 xmm1,ymm0,0x1
  4029f0:	c5 f9 d4 d9          	vpaddq xmm3,xmm0,xmm1
  4029f4:	62 f1 fd 48 6f 84 24 	vmovdqa64 zmm0,ZMMWORD PTR [rsp+0x208]
  4029fb:	08 02 00 00 
  4029ff:	62 f3 fd 48 3b c1 01 	vextracti64x4 ymm1,zmm0,0x1
  402a06:	c5 fd d4 c1          	vpaddq ymm0,ymm0,ymm1
  402a0a:	62 f3 fd 28 39 c1 01 	vextracti64x2 xmm1,ymm0,0x1
  402a11:	c5 f9 d4 d1          	vpaddq xmm2,xmm0,xmm1
  402a15:	62 f1 fd 48 6f 84 24 	vmovdqa64 zmm0,ZMMWORD PTR [rsp+0x48]
  402a1c:	48 00 00 00 
  402a20:	62 f3 fd 48 3b c1 01 	vextracti64x4 ymm1,zmm0,0x1
  402a27:	c5 fd d4 c1          	vpaddq ymm0,ymm0,ymm1
  402a2b:	62 f3 fd 28 39 c1 01 	vextracti64x2 xmm1,ymm0,0x1
  402a32:	c5 f9 d4 c9          	vpaddq xmm1,xmm0,xmm1
  402a36:	62 f1 fd 48 6f 84 24 	vmovdqa64 zmm0,ZMMWORD PTR [rsp+0x248]
  402a3d:	48 02 00 00 
  402a41:	62 d3 fd 48 3b c0 01 	vextracti64x4 ymm8,zmm0,0x1
  402a48:	c4 c1 7d d4 c0       	vpaddq ymm0,ymm0,ymm8
  402a4d:	62 d3 fd 28 39 c0 01 	vextracti64x2 xmm8,ymm0,0x1
  402a54:	c4 c1 79 d4 c0       	vpaddq xmm0,xmm0,xmm8
  402a59:	62 53 fd 48 3b c8 01 	vextracti64x4 ymm8,zmm9,0x1
  402a60:	c4 41 3d d4 c1       	vpaddq ymm8,ymm8,ymm9
  402a65:	62 71 fd 48 6f 8c 24 	vmovdqa64 zmm9,ZMMWORD PTR [rsp+0x288]
  402a6c:	88 02 00 00 
  402a70:	62 53 fd 28 39 c6 01 	vextracti64x2 xmm14,ymm8,0x1
  402a77:	c4 41 09 d4 f0       	vpaddq xmm14,xmm14,xmm8
  402a7c:	62 53 fd 48 3b c8 01 	vextracti64x4 ymm8,zmm9,0x1
  402a83:	c4 41 3d d4 c1       	vpaddq ymm8,ymm8,ymm9
  402a88:	62 71 fd 48 6f 8c 24 	vmovdqa64 zmm9,ZMMWORD PTR [rsp+0xc8]
  402a8f:	c8 00 00 00 
  402a93:	62 53 fd 28 39 c5 01 	vextracti64x2 xmm13,ymm8,0x1
  402a9a:	c4 41 11 d4 e8       	vpaddq xmm13,xmm13,xmm8
  402a9f:	62 53 fd 48 3b c8 01 	vextracti64x4 ymm8,zmm9,0x1
  402aa6:	c4 41 3d d4 c1       	vpaddq ymm8,ymm8,ymm9
  402aab:	62 71 fd 48 6f 8c 24 	vmovdqa64 zmm9,ZMMWORD PTR [rsp+0x2c8]
  402ab2:	c8 02 00 00 
  402ab6:	62 53 fd 28 39 c4 01 	vextracti64x2 xmm12,ymm8,0x1
  402abd:	c4 41 19 d4 e0       	vpaddq xmm12,xmm12,xmm8
  402ac2:	62 53 fd 48 3b c8 01 	vextracti64x4 ymm8,zmm9,0x1
  402ac9:	c4 41 3d d4 c1       	vpaddq ymm8,ymm8,ymm9
  402ace:	62 71 fd 48 6f 8c 24 	vmovdqa64 zmm9,ZMMWORD PTR [rsp+0x108]
  402ad5:	08 01 00 00 
  402ad9:	62 33 fd 28 39 c0 01 	vextracti64x2 xmm16,ymm8,0x1
  402ae0:	62 c1 fd 00 d4 c0    	vpaddq xmm16,xmm16,xmm8
  402ae6:	62 53 fd 48 3b c8 01 	vextracti64x4 ymm8,zmm9,0x1
  402aed:	c4 41 3d d4 c1       	vpaddq ymm8,ymm8,ymm9
  402af2:	62 71 fd 48 6f 8c 24 	vmovdqa64 zmm9,ZMMWORD PTR [rsp+0x308]
  402af9:	08 03 00 00 
  402afd:	c4 63 f9 16 f2 01    	vpextrq rdx,xmm14,0x1
  402b03:	62 53 fd 28 39 c3 01 	vextracti64x2 xmm11,ymm8,0x1
  402b0a:	c4 61 f9 7e f0       	vmovq  rax,xmm14
  402b0f:	c4 41 21 d4 d8       	vpaddq xmm11,xmm11,xmm8
  402b14:	48 01 d0             	add    rax,rdx
  402b17:	c4 61 f9 7e e9       	vmovq  rcx,xmm13
  402b1c:	62 53 fd 48 3b c8 01 	vextracti64x4 ymm8,zmm9,0x1
  402b23:	c4 63 f9 16 ea 01    	vpextrq rdx,xmm13,0x1
  402b29:	c4 41 3d d4 c1       	vpaddq ymm8,ymm8,ymm9
  402b2e:	48 01 ca             	add    rdx,rcx
  402b31:	62 71 fd 48 6f 8c 24 	vmovdqa64 zmm9,ZMMWORD PTR [rsp+0x148]
  402b38:	48 01 00 00 
  402b3c:	48 31 d0             	xor    rax,rdx
  402b3f:	c4 61 f9 7e e1       	vmovq  rcx,xmm12
  402b44:	62 53 fd 28 39 c2 01 	vextracti64x2 xmm10,ymm8,0x1
  402b4b:	c4 63 f9 16 e2 01    	vpextrq rdx,xmm12,0x1
  402b51:	c4 41 29 d4 d0       	vpaddq xmm10,xmm10,xmm8
  402b56:	48 01 ca             	add    rdx,rcx
  402b59:	62 53 fd 48 3b c8 01 	vextracti64x4 ymm8,zmm9,0x1
  402b60:	c4 41 3d d4 c1       	vpaddq ymm8,ymm8,ymm9
  402b65:	48 31 d0             	xor    rax,rdx
  402b68:	62 e1 fd 08 7e c1    	vmovq  rcx,xmm16
  402b6e:	62 e3 fd 08 16 c2 01 	vpextrq rdx,xmm16,0x1
  402b75:	48 01 ca             	add    rdx,rcx
  402b78:	62 53 fd 28 39 c1 01 	vextracti64x2 xmm9,ymm8,0x1
  402b7f:	c4 41 31 d4 c8       	vpaddq xmm9,xmm9,xmm8
  402b84:	48 31 d0             	xor    rax,rdx
  402b87:	62 71 fd 48 6f 84 24 	vmovdqa64 zmm8,ZMMWORD PTR [rsp+0x348]
  402b8e:	48 03 00 00 
  402b92:	c4 61 f9 7e d9       	vmovq  rcx,xmm11
  402b97:	c4 63 f9 16 da 01    	vpextrq rdx,xmm11,0x1
  402b9d:	48 01 ca             	add    rdx,rcx
  402ba0:	48 31 d0             	xor    rax,rdx
  402ba3:	c4 61 f9 7e d1       	vmovq  rcx,xmm10
  402ba8:	62 53 fd 48 3b c7 01 	vextracti64x4 ymm15,zmm8,0x1
  402baf:	c4 63 f9 16 d2 01    	vpextrq rdx,xmm10,0x1
  402bb5:	48 01 ca             	add    rdx,rcx
  402bb8:	c4 41 05 d4 f8       	vpaddq ymm15,ymm15,ymm8
  402bbd:	48 31 d0             	xor    rax,rdx
  402bc0:	c4 61 f9 7e c9       	vmovq  rcx,xmm9
  402bc5:	62 53 fd 28 39 f8 01 	vextracti64x2 xmm8,ymm15,0x1
  402bcc:	c4 63 f9 16 ca 01    	vpextrq rdx,xmm9,0x1
  402bd2:	48 01 ca             	add    rdx,rcx
  402bd5:	c4 41 39 d4 c7       	vpaddq xmm8,xmm8,xmm15
  402bda:	48 31 d0             	xor    rax,rdx
  402bdd:	c4 61 f9 7e c1       	vmovq  rcx,xmm8
  402be2:	c4 63 f9 16 c2 01    	vpextrq rdx,xmm8,0x1
  402be8:	48 01 ca             	add    rdx,rcx
  402beb:	48 31 d0             	xor    rax,rdx
  402bee:	c4 e1 f9 7e f9       	vmovq  rcx,xmm7
  402bf3:	c4 e3 f9 16 fa 01    	vpextrq rdx,xmm7,0x1
  402bf9:	48 01 ca             	add    rdx,rcx
  402bfc:	c4 e1 f9 7e f1       	vmovq  rcx,xmm6
  402c01:	48 01 f1             	add    rcx,rsi
  402c04:	48 31 ca             	xor    rdx,rcx
  402c07:	c4 e3 f9 16 ee 01    	vpextrq rsi,xmm5,0x1
  402c0d:	c4 e1 f9 7e e9       	vmovq  rcx,xmm5
  402c12:	48 01 f1             	add    rcx,rsi
  402c15:	48 31 ca             	xor    rdx,rcx
  402c18:	c4 e3 f9 16 e6 01    	vpextrq rsi,xmm4,0x1
  402c1e:	c4 e1 f9 7e e1       	vmovq  rcx,xmm4
  402c23:	48 01 f1             	add    rcx,rsi
  402c26:	48 31 ca             	xor    rdx,rcx
  402c29:	c4 e3 f9 16 de 01    	vpextrq rsi,xmm3,0x1
  402c2f:	c4 e1 f9 7e d9       	vmovq  rcx,xmm3
  402c34:	48 01 f1             	add    rcx,rsi
  402c37:	48 31 ca             	xor    rdx,rcx
  402c3a:	c4 e3 f9 16 d6 01    	vpextrq rsi,xmm2,0x1
  402c40:	c4 e1 f9 7e d1       	vmovq  rcx,xmm2
  402c45:	48 01 f1             	add    rcx,rsi
  402c48:	48 31 ca             	xor    rdx,rcx
  402c4b:	c4 e3 f9 16 ce 01    	vpextrq rsi,xmm1,0x1
  402c51:	c4 e1 f9 7e c9       	vmovq  rcx,xmm1
  402c56:	48 01 f1             	add    rcx,rsi
  402c59:	48 31 ca             	xor    rdx,rcx
  402c5c:	c4 e3 f9 16 c6 01    	vpextrq rsi,xmm0,0x1
  402c62:	c4 e1 f9 7e c1       	vmovq  rcx,xmm0
  402c67:	48 01 f1             	add    rcx,rsi
  402c6a:	48 31 ca             	xor    rdx,rcx
  402c6d:	48 31 d0             	xor    rax,rdx
  402c70:	c5 f8 77             	vzeroupper 
  402c73:	c9                   	leave  
  402c74:	c3                   	ret    
  402c75:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
  402c7c:	00 00 00 00 

0000000000402c80 <ifma52_two_preexpanded>:
  402c80:	55                   	push   rbp
  402c81:	48 c1 ea 09          	shr    rdx,0x9
  402c85:	49 89 d0             	mov    r8,rdx
  402c88:	48 89 e5             	mov    rbp,rsp
  402c8b:	48 83 e4 c0          	and    rsp,0xffffffffffffffc0
  402c8f:	48 81 ec 88 03 00 00 	sub    rsp,0x388
  402c96:	c5 f9 ef c0          	vpxor  xmm0,xmm0,xmm0
  402c9a:	49 c1 e0 02          	shl    r8,0x2
  402c9e:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x188],zmm0
  402ca5:	88 01 00 00 
  402ca9:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp-0x78],zmm0
  402cb0:	88 ff ff ff 
  402cb4:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x1c8],zmm0
  402cbb:	c8 01 00 00 
  402cbf:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp-0x38],zmm0
  402cc6:	c8 ff ff ff 
  402cca:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x208],zmm0
  402cd1:	08 02 00 00 
  402cd5:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x8],zmm0
  402cdc:	08 00 00 00 
  402ce0:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x248],zmm0
  402ce7:	48 02 00 00 
  402ceb:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x48],zmm0
  402cf2:	48 00 00 00 
  402cf6:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x288],zmm0
  402cfd:	88 02 00 00 
  402d01:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x88],zmm0
  402d08:	88 00 00 00 
  402d0c:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x2c8],zmm0
  402d13:	c8 02 00 00 
  402d17:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0xc8],zmm0
  402d1e:	c8 00 00 00 
  402d22:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x308],zmm0
  402d29:	08 03 00 00 
  402d2d:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x108],zmm0
  402d34:	08 01 00 00 
  402d38:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x348],zmm0
  402d3f:	48 03 00 00 
  402d43:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x148],zmm0
  402d4a:	48 01 00 00 
  402d4e:	0f 84 6e 02 00 00    	je     402fc2 <ifma52_two_preexpanded+0x342>
  402d54:	48 89 f0             	mov    rax,rsi
  402d57:	48 89 fa             	mov    rdx,rdi
  402d5a:	62 f1 fd 48 6f d0    	vmovdqa64 zmm2,zmm0
  402d60:	62 f1 fd 48 6f d8    	vmovdqa64 zmm3,zmm0
  402d66:	62 f1 fd 48 6f e0    	vmovdqa64 zmm4,zmm0
  402d6c:	62 f1 fd 48 6f e8    	vmovdqa64 zmm5,zmm0
  402d72:	62 f1 fd 48 6f f0    	vmovdqa64 zmm6,zmm0
  402d78:	62 f1 fd 48 6f f8    	vmovdqa64 zmm7,zmm0
  402d7e:	62 71 fd 48 6f c0    	vmovdqa64 zmm8,zmm0
  402d84:	62 71 fd 48 6f c8    	vmovdqa64 zmm9,zmm0
  402d8a:	62 71 fd 48 6f d0    	vmovdqa64 zmm10,zmm0
  402d90:	62 71 fd 48 6f d8    	vmovdqa64 zmm11,zmm0
  402d96:	62 71 fd 48 6f e0    	vmovdqa64 zmm12,zmm0
  402d9c:	62 71 fd 48 6f e8    	vmovdqa64 zmm13,zmm0
  402da2:	62 71 fd 48 6f f0    	vmovdqa64 zmm14,zmm0
  402da8:	62 71 fd 48 6f f8    	vmovdqa64 zmm15,zmm0
  402dae:	62 e1 fd 48 6f c0    	vmovdqa64 zmm16,zmm0
  402db4:	62 e1 fd 48 6f c8    	vmovdqa64 zmm17,zmm0
  402dba:	31 f6                	xor    esi,esi
  402dbc:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
  402dc0:	48 89 f1             	mov    rcx,rsi
  402dc3:	62 f1 fe 48 6f 02    	vmovdqu64 zmm0,ZMMWORD PTR [rdx]
  402dc9:	62 f1 fe 48 6f 4a 01 	vmovdqu64 zmm1,ZMMWORD PTR [rdx+0x40]
  402dd0:	83 e1 07             	and    ecx,0x7
  402dd3:	48 c1 e1 07          	shl    rcx,0x7
  402dd7:	62 e1 fd 48 d4 14 08 	vpaddq zmm18,zmm0,ZMMWORD PTR [rax+rcx*1]
  402dde:	62 e1 f5 48 d4 5c 08 	vpaddq zmm19,zmm1,ZMMWORD PTR [rax+rcx*1+0x40]
  402de5:	01 
  402de6:	62 f1 fd 48 d4 44 08 	vpaddq zmm0,zmm0,ZMMWORD PTR [rax+rcx*1+0x400]
  402ded:	10 
  402dee:	62 f1 f5 48 d4 4c 08 	vpaddq zmm1,zmm1,ZMMWORD PTR [rax+rcx*1+0x440]
  402df5:	11 
  402df6:	48 8d 4e 01          	lea    rcx,[rsi+0x1]
  402dfa:	62 72 fd 48 b4 f9    	vpmadd52luq zmm15,zmm0,zmm1
  402e00:	62 72 fd 48 b5 f1    	vpmadd52huq zmm14,zmm0,zmm1
  402e06:	83 e1 07             	and    ecx,0x7
  402e09:	62 f1 fe 48 6f 42 02 	vmovdqu64 zmm0,ZMMWORD PTR [rdx+0x80]
  402e10:	62 f1 fe 48 6f 4a 03 	vmovdqu64 zmm1,ZMMWORD PTR [rdx+0xc0]
  402e17:	48 c1 e1 07          	shl    rcx,0x7
  402e1b:	62 a2 ed 40 b4 cb    	vpmadd52luq zmm17,zmm18,zmm19
  402e21:	62 a2 ed 40 b5 c3    	vpmadd52huq zmm16,zmm18,zmm19
  402e27:	62 e1 fd 48 d4 14 08 	vpaddq zmm18,zmm0,ZMMWORD PTR [rax+rcx*1]
  402e2e:	62 e1 f5 48 d4 5c 08 	vpaddq zmm19,zmm1,ZMMWORD PTR [rax+rcx*1+0x40]
  402e35:	01 
  402e36:	62 f1 fd 48 d4 44 08 	vpaddq zmm0,zmm0,ZMMWORD PTR [rax+rcx*1+0x400]
  402e3d:	10 
  402e3e:	62 f1 f5 48 d4 4c 08 	vpaddq zmm1,zmm1,ZMMWORD PTR [rax+rcx*1+0x440]
  402e45:	11 
  402e46:	48 8d 4e 02          	lea    rcx,[rsi+0x2]
  402e4a:	62 72 fd 48 b4 d9    	vpmadd52luq zmm11,zmm0,zmm1
  402e50:	62 72 fd 48 b5 d1    	vpmadd52huq zmm10,zmm0,zmm1
  402e56:	83 e1 07             	and    ecx,0x7
  402e59:	62 f1 fe 48 6f 42 04 	vmovdqu64 zmm0,ZMMWORD PTR [rdx+0x100]
  402e60:	62 f1 fe 48 6f 4a 05 	vmovdqu64 zmm1,ZMMWORD PTR [rdx+0x140]
  402e67:	48 c1 e1 07          	shl    rcx,0x7
  402e6b:	62 32 ed 40 b4 eb    	vpmadd52luq zmm13,zmm18,zmm19
  402e71:	62 32 ed 40 b5 e3    	vpmadd52huq zmm12,zmm18,zmm19
  402e77:	62 e1 fd 48 d4 14 08 	vpaddq zmm18,zmm0,ZMMWORD PTR [rax+rcx*1]
  402e7e:	62 e1 f5 48 d4 5c 08 	vpaddq zmm19,zmm1,ZMMWORD PTR [rax+rcx*1+0x40]
  402e85:	01 
  402e86:	62 f1 fd 48 d4 44 08 	vpaddq zmm0,zmm0,ZMMWORD PTR [rax+rcx*1+0x400]
  402e8d:	10 
  402e8e:	62 f1 f5 48 d4 4c 08 	vpaddq zmm1,zmm1,ZMMWORD PTR [rax+rcx*1+0x440]
  402e95:	11 
  402e96:	48 8d 4e 03          	lea    rcx,[rsi+0x3]
  402e9a:	62 f2 fd 48 b4 f9    	vpmadd52luq zmm7,zmm0,zmm1
  402ea0:	62 f2 fd 48 b5 f1    	vpmadd52huq zmm6,zmm0,zmm1
  402ea6:	83 e1 07             	and    ecx,0x7
  402ea9:	62 f1 fe 48 6f 42 06 	vmovdqu64 zmm0,ZMMWORD PTR [rdx+0x180]
  402eb0:	62 f1 fe 48 6f 4a 07 	vmovdqu64 zmm1,ZMMWORD PTR [rdx+0x1c0]
  402eb7:	48 c1 e1 07          	shl    rcx,0x7
  402ebb:	62 32 ed 40 b4 cb    	vpmadd52luq zmm9,zmm18,zmm19
  402ec1:	62 32 ed 40 b5 c3    	vpmadd52huq zmm8,zmm18,zmm19
  402ec7:	62 e1 fd 48 d4 14 08 	vpaddq zmm18,zmm0,ZMMWORD PTR [rax+rcx*1]
  402ece:	62 e1 f5 48 d4 5c 08 	vpaddq zmm19,zmm1,ZMMWORD PTR [rax+rcx*1+0x40]
  402ed5:	01 
  402ed6:	62 f1 fd 48 d4 44 08 	vpaddq zmm0,zmm0,ZMMWORD PTR [rax+rcx*1+0x400]
  402edd:	10 
  402ede:	62 f1 f5 48 d4 4c 08 	vpaddq zmm1,zmm1,ZMMWORD PTR [rax+rcx*1+0x440]
  402ee5:	11 
  402ee6:	48 83 c6 04          	add    rsi,0x4
  402eea:	62 b2 ed 40 b4 eb    	vpmadd52luq zmm5,zmm18,zmm19
  402ef0:	62 b2 ed 40 b5 e3    	vpmadd52huq zmm4,zmm18,zmm19
  402ef6:	62 f2 fd 48 b4 d9    	vpmadd52luq zmm3,zmm0,zmm1
  402efc:	62 f2 fd 48 b5 d1    	vpmadd52huq zmm2,zmm0,zmm1
  402f02:	48 81 c2 00 02 00 00 	add    rdx,0x200
  402f09:	49 39 f0             	cmp    r8,rsi
  402f0c:	0f 87 ae fe ff ff    	ja     402dc0 <ifma52_two_preexpanded+0x140>
  402f12:	62 e1 fd 48 7f 8c 24 	vmovdqa64 ZMMWORD PTR [rsp-0x78],zmm17
  402f19:	88 ff ff ff 
  402f1d:	62 e1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x188],zmm16
  402f24:	88 01 00 00 
  402f28:	62 71 fd 48 7f bc 24 	vmovdqa64 ZMMWORD PTR [rsp+0x88],zmm15
  402f2f:	88 00 00 00 
  402f33:	62 71 fd 48 7f b4 24 	vmovdqa64 ZMMWORD PTR [rsp+0x288],zmm14
  402f3a:	88 02 00 00 
  402f3e:	62 71 fd 48 7f ac 24 	vmovdqa64 ZMMWORD PTR [rsp-0x38],zmm13
  402f45:	c8 ff ff ff 
  402f49:	62 71 fd 48 7f a4 24 	vmovdqa64 ZMMWORD PTR [rsp+0x1c8],zmm12
  402f50:	c8 01 00 00 
  402f54:	62 71 fd 48 7f 9c 24 	vmovdqa64 ZMMWORD PTR [rsp+0xc8],zmm11
  402f5b:	c8 00 00 00 
  402f5f:	62 71 fd 48 7f 94 24 	vmovdqa64 ZMMWORD PTR [rsp+0x2c8],zmm10
  402f66:	c8 02 00 00 
  402f6a:	62 71 fd 48 7f 8c 24 	vmovdqa64 ZMMWORD PTR [rsp+0x8],zmm9
  402f71:	08 00 00 00 
  402f75:	62 71 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x208],zmm8
  402f7c:	08 02 00 00 
  402f80:	62 f1 fd 48 7f bc 24 	vmovdqa64 ZMMWORD PTR [rsp+0x108],zmm7
  402f87:	08 01 00 00 
  402f8b:	62 f1 fd 48 7f b4 24 	vmovdqa64 ZMMWORD PTR [rsp+0x308],zmm6
  402f92:	08 03 00 00 
  402f96:	62 f1 fd 48 7f ac 24 	vmovdqa64 ZMMWORD PTR [rsp+0x48],zmm5
  402f9d:	48 00 00 00 
  402fa1:	62 f1 fd 48 7f a4 24 	vmovdqa64 ZMMWORD PTR [rsp+0x248],zmm4
  402fa8:	48 02 00 00 
  402fac:	62 f1 fd 48 7f 9c 24 	vmovdqa64 ZMMWORD PTR [rsp+0x148],zmm3
  402fb3:	48 01 00 00 
  402fb7:	62 f1 fd 48 7f 94 24 	vmovdqa64 ZMMWORD PTR [rsp+0x348],zmm2
  402fbe:	48 03 00 00 
  402fc2:	62 f1 fd 48 6f 84 24 	vmovdqa64 zmm0,ZMMWORD PTR [rsp-0x78]
  402fc9:	88 ff ff ff 
  402fcd:	62 71 fd 48 6f 8c 24 	vmovdqa64 zmm9,ZMMWORD PTR [rsp+0x88]
  402fd4:	88 00 00 00 
  402fd8:	62 f3 fd 48 3b c1 01 	vextracti64x4 ymm1,zmm0,0x1
  402fdf:	c5 fd d4 c1          	vpaddq ymm0,ymm0,ymm1
  402fe3:	62 f3 fd 28 39 c1 01 	vextracti64x2 xmm1,ymm0,0x1
  402fea:	c5 f9 d4 f1          	vpaddq xmm6,xmm0,xmm1
  402fee:	62 f1 fd 48 6f 84 24 	vmovdqa64 zmm0,ZMMWORD PTR [rsp+0x188]
  402ff5:	88 01 00 00 
  402ff9:	c4 e3 f9 16 f6 01    	vpextrq rsi,xmm6,0x1
  402fff:	62 f3 fd 48 3b c1 01 	vextracti64x4 ymm1,zmm0,0x1
  403006:	c5 fd d4 c1          	vpaddq ymm0,ymm0,ymm1
  40300a:	62 f3 fd 28 39 c1 01 	vextracti64x2 xmm1,ymm0,0x1
  403011:	c5 f9 d4 f9          	vpaddq xmm7,xmm0,xmm1
  403015:	62 f1 fd 48 6f 84 24 	vmovdqa64 zmm0,ZMMWORD PTR [rsp-0x38]
  40301c:	c8 ff ff ff 
  403020:	62 f3 fd 48 3b c1 01 	vextracti64x4 ymm1,zmm0,0x1
  403027:	c5 fd d4 c1          	vpaddq ymm0,ymm0,ymm1
  40302b:	62 f3 fd 28 39 c1 01 	vextracti64x2 xmm1,ymm0,0x1
  403032:	c5 f9 d4 e9          	vpaddq xmm5,xmm0,xmm1
  403036:	62 f1 fd 48 6f 84 24 	vmovdqa64 zmm0,ZMMWORD PTR [rsp+0x1c8]
  40303d:	c8 01 00 00 
  403041:	62 f3 fd 48 3b c1 01 	vextracti64x4 ymm1,zmm0,0x1
  403048:	c5 fd d4 c1          	vpaddq ymm0,ymm0,ymm1
  40304c:	62 f3 fd 28 39 c1 01 	vextracti64x2 xmm1,ymm0,0x1
  403053:	c5 f9 d4 e1          	vpaddq xmm4,xmm0,xmm1
  403057:	62 f1 fd 48 6f 84 24 	vmovdqa64 zmm0,ZMMWORD PTR [rsp+0x8]
  40305e:	08 00 00 00 
  403062:	62 f3 fd 48 3b c1 01 	vextracti64x4 ymm1,zmm0,0x1
  403069:	c5 fd d4 c1          	vpaddq ymm0,ymm0,ymm1
  40306d:	62 f3 fd 28 39 c1 01 	vextracti64x2 xmm1,ymm0,0x1
  403074:	c5 f9 d4 d9          	vpaddq xmm3,xmm0,xmm1
  403078:	62 f1 fd 48 6f 84 24 	vmovdqa64 zmm0,ZMMWORD PTR [rsp+0x208]
  40307f:	08 02 00 00 
  403083:	62 f3 fd 48 3b c1 01 	vextracti64x4 ymm1,zmm0,0x1
  40308a:	c5 fd d4 c1          	vpaddq ymm0,ymm0,ymm1
  40308e:	62 f3 fd 28 39 c1 01 	vextracti64x2 xmm1,ymm0,0x1
  403095:	c5 f9 d4 d1          	vpaddq xmm2,xmm0,xmm1
  403099:	62 f1 fd 48 6f 84 24 	vmovdqa64 zmm0,ZMMWORD PTR [rsp+0x48]
  4030a0:	48 00 00 00 
  4030a4:	62 f3 fd 48 3b c1 01 	vextracti64x4 ymm1,zmm0,0x1
  4030ab:	c5 fd d4 c1          	vpaddq ymm0,ymm0,ymm1
  4030af:	62 f3 fd 28 39 c1 01 	vextracti64x2 xmm1,ymm0,0x1
  4030b6:	c5 f9 d4 c9          	vpaddq xmm1,xmm0,xmm1
  4030ba:	62 f1 fd 48 6f 84 24 	vmovdqa64 zmm0,ZMMWORD PTR [rsp+0x248]
  4030c1:	48 02 00 00 
  4030c5:	62 d3 fd 48 3b c0 01 	vextracti64x4 ymm8,zmm0,0x1
  4030cc:	c4 c1 7d d4 c0       	vpaddq ymm0,ymm0,ymm8
  4030d1:	62 d3 fd 28 39 c0 01 	vextracti64x2 xmm8,ymm0,0x1
  4030d8:	c4 c1 79 d4 c0       	vpaddq xmm0,xmm0,xmm8
  4030dd:	62 53 fd 48 3b c8 01 	vextracti64x4 ymm8,zmm9,0x1
  4030e4:	c4 41 3d d4 c1       	vpaddq ymm8,ymm8,ymm9
  4030e9:	62 71 fd 48 6f 8c 24 	vmovdqa64 zmm9,ZMMWORD PTR [rsp+0x288]
  4030f0:	88 02 00 00 
  4030f4:	62 53 fd 28 39 c6 01 	vextracti64x2 xmm14,ymm8,0x1
  4030fb:	c4 41 09 d4 f0       	vpaddq xmm14,xmm14,xmm8
  403100:	62 53 fd 48 3b c8 01 	vextracti64x4 ymm8,zmm9,0x1
  403107:	c4 41 3d d4 c1       	vpaddq ymm8,ymm8,ymm9
  40310c:	62 71 fd 48 6f 8c 24 	vmovdqa64 zmm9,ZMMWORD PTR [rsp+0xc8]
  403113:	c8 00 00 00 
  403117:	62 53 fd 28 39 c5 01 	vextracti64x2 xmm13,ymm8,0x1
  40311e:	c4 41 11 d4 e8       	vpaddq xmm13,xmm13,xmm8
  403123:	62 53 fd 48 3b c8 01 	vextracti64x4 ymm8,zmm9,0x1
  40312a:	c4 41 3d d4 c1       	vpaddq ymm8,ymm8,ymm9
  40312f:	62 71 fd 48 6f 8c 24 	vmovdqa64 zmm9,ZMMWORD PTR [rsp+0x2c8]
  403136:	c8 02 00 00 
  40313a:	62 53 fd 28 39 c4 01 	vextracti64x2 xmm12,ymm8,0x1
  403141:	c4 41 19 d4 e0       	vpaddq xmm12,xmm12,xmm8
  403146:	62 53 fd 48 3b c8 01 	vextracti64x4 ymm8,zmm9,0x1
  40314d:	c4 41 3d d4 c1       	vpaddq ymm8,ymm8,ymm9
  403152:	62 71 fd 48 6f 8c 24 	vmovdqa64 zmm9,ZMMWORD PTR [rsp+0x108]
  403159:	08 01 00 00 
  40315d:	62 33 fd 28 39 c0 01 	vextracti64x2 xmm16,ymm8,0x1
  403164:	62 c1 fd 00 d4 c0    	vpaddq xmm16,xmm16,xmm8
  40316a:	62 53 fd 48 3b c8 01 	vextracti64x4 ymm8,zmm9,0x1
  403171:	c4 41 3d d4 c1       	vpaddq ymm8,ymm8,ymm9
  403176:	62 71 fd 48 6f 8c 24 	vmovdqa64 zmm9,ZMMWORD PTR [rsp+0x308]
  40317d:	08 03 00 00 
  403181:	c4 63 f9 16 f2 01    	vpextrq rdx,xmm14,0x1
  403187:	62 53 fd 28 39 c3 01 	vextracti64x2 xmm11,ymm8,0x1
  40318e:	c4 61 f9 7e f0       	vmovq  rax,xmm14
  403193:	c4 41 21 d4 d8       	vpaddq xmm11,xmm11,xmm8
  403198:	48 01 d0             	add    rax,rdx
  40319b:	c4 61 f9 7e e9       	vmovq  rcx,xmm13
  4031a0:	62 53 fd 48 3b c8 01 	vextracti64x4 ymm8,zmm9,0x1
  4031a7:	c4 63 f9 16 ea 01    	vpextrq rdx,xmm13,0x1
  4031ad:	c4 41 3d d4 c1       	vpaddq ymm8,ymm8,ymm9
  4031b2:	48 01 ca             	add    rdx,rcx
  4031b5:	62 71 fd 48 6f 8c 24 	vmovdqa64 zmm9,ZMMWORD PTR [rsp+0x148]
  4031bc:	48 01 00 00 
  4031c0:	48 31 d0             	xor    rax,rdx
  4031c3:	c4 61 f9 7e e1       	vmovq  rcx,xmm12
  4031c8:	62 53 fd 28 39 c2 01 	vextracti64x2 xmm10,ymm8,0x1
  4031cf:	c4 63 f9 16 e2 01    	vpextrq rdx,xmm12,0x1
  4031d5:	c4 41 29 d4 d0       	vpaddq xmm10,xmm10,xmm8
  4031da:	48 01 ca             	add    rdx,rcx
  4031dd:	62 53 fd 48 3b c8 01 	vextracti64x4 ymm8,zmm9,0x1
  4031e4:	c4 41 3d d4 c1       	vpaddq ymm8,ymm8,ymm9
  4031e9:	48 31 d0             	xor    rax,rdx
  4031ec:	62 e1 fd 08 7e c1    	vmovq  rcx,xmm16
  4031f2:	62 e3 fd 08 16 c2 01 	vpextrq rdx,xmm16,0x1
  4031f9:	48 01 ca             	add    rdx,rcx
  4031fc:	62 53 fd 28 39 c1 01 	vextracti64x2 xmm9,ymm8,0x1
  403203:	c4 41 31 d4 c8       	vpaddq xmm9,xmm9,xmm8
  403208:	48 31 d0             	xor    rax,rdx
  40320b:	62 71 fd 48 6f 84 24 	vmovdqa64 zmm8,ZMMWORD PTR [rsp+0x348]
  403212:	48 03 00 00 
  403216:	c4 61 f9 7e d9       	vmovq  rcx,xmm11
  40321b:	c4 63 f9 16 da 01    	vpextrq rdx,xmm11,0x1
  403221:	48 01 ca             	add    rdx,rcx
  403224:	48 31 d0             	xor    rax,rdx
  403227:	c4 61 f9 7e d1       	vmovq  rcx,xmm10
  40322c:	62 53 fd 48 3b c7 01 	vextracti64x4 ymm15,zmm8,0x1
  403233:	c4 63 f9 16 d2 01    	vpextrq rdx,xmm10,0x1
  403239:	48 01 ca             	add    rdx,rcx
  40323c:	c4 41 05 d4 f8       	vpaddq ymm15,ymm15,ymm8
  403241:	48 31 d0             	xor    rax,rdx
  403244:	c4 61 f9 7e c9       	vmovq  rcx,xmm9
  403249:	62 53 fd 28 39 f8 01 	vextracti64x2 xmm8,ymm15,0x1
  403250:	c4 63 f9 16 ca 01    	vpextrq rdx,xmm9,0x1
  403256:	48 01 ca             	add    rdx,rcx
  403259:	c4 41 39 d4 c7       	vpaddq xmm8,xmm8,xmm15
  40325e:	48 31 d0             	xor    rax,rdx
  403261:	c4 61 f9 7e c1       	vmovq  rcx,xmm8
  403266:	c4 63 f9 16 c2 01    	vpextrq rdx,xmm8,0x1
  40326c:	48 01 ca             	add    rdx,rcx
  40326f:	48 31 d0             	xor    rax,rdx
  403272:	c4 e1 f9 7e f9       	vmovq  rcx,xmm7
  403277:	c4 e3 f9 16 fa 01    	vpextrq rdx,xmm7,0x1
  40327d:	48 01 ca             	add    rdx,rcx
  403280:	c4 e1 f9 7e f1       	vmovq  rcx,xmm6
  403285:	48 01 f1             	add    rcx,rsi
  403288:	48 31 ca             	xor    rdx,rcx
  40328b:	c4 e3 f9 16 ee 01    	vpextrq rsi,xmm5,0x1
  403291:	c4 e1 f9 7e e9       	vmovq  rcx,xmm5
  403296:	48 01 f1             	add    rcx,rsi
  403299:	48 31 ca             	xor    rdx,rcx
  40329c:	c4 e3 f9 16 e6 01    	vpextrq rsi,xmm4,0x1
  4032a2:	c4 e1 f9 7e e1       	vmovq  rcx,xmm4
  4032a7:	48 01 f1             	add    rcx,rsi
  4032aa:	48 31 ca             	xor    rdx,rcx
  4032ad:	c4 e3 f9 16 de 01    	vpextrq rsi,xmm3,0x1
  4032b3:	c4 e1 f9 7e d9       	vmovq  rcx,xmm3
  4032b8:	48 01 f1             	add    rcx,rsi
  4032bb:	48 31 ca             	xor    rdx,rcx
  4032be:	c4 e3 f9 16 d6 01    	vpextrq rsi,xmm2,0x1
  4032c4:	c4 e1 f9 7e d1       	vmovq  rcx,xmm2
  4032c9:	48 01 f1             	add    rcx,rsi
  4032cc:	48 31 ca             	xor    rdx,rcx
  4032cf:	c4 e3 f9 16 ce 01    	vpextrq rsi,xmm1,0x1
  4032d5:	c4 e1 f9 7e c9       	vmovq  rcx,xmm1
  4032da:	48 01 f1             	add    rcx,rsi
  4032dd:	48 31 ca             	xor    rdx,rcx
  4032e0:	c4 e3 f9 16 c6 01    	vpextrq rsi,xmm0,0x1
  4032e6:	c4 e1 f9 7e c1       	vmovq  rcx,xmm0
  4032eb:	48 01 f1             	add    rcx,rsi
  4032ee:	48 31 ca             	xor    rdx,rcx
  4032f1:	48 31 d0             	xor    rax,rdx
  4032f4:	c5 f8 77             	vzeroupper 
  4032f7:	c9                   	leave  
  4032f8:	c3                   	ret    
  4032f9:	0f 1f 80 00 00 00 00 	nop    DWORD PTR [rax+0x0]

0000000000403300 <ph128>:
  403300:	48 85 d2             	test   rdx,rdx
  403303:	0f 84 b8 00 00 00    	je     4033c1 <ph128+0xc1>
  403309:	c5 e9 ef d2          	vpxor  xmm2,xmm2,xmm2
  40330d:	c5 f9 6f e2          	vmovdqa xmm4,xmm2
  403311:	c5 f9 6f ca          	vmovdqa xmm1,xmm2
  403315:	c5 f9 6f da          	vmovdqa xmm3,xmm2
  403319:	31 c0                	xor    eax,eax
  40331b:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
  403320:	48 89 c1             	mov    rcx,rax
  403323:	81 e1 ff 03 00 00    	and    ecx,0x3ff
  403329:	c5 fa 6f 3c 0e       	vmovdqu xmm7,XMMWORD PTR [rsi+rcx*1]
  40332e:	48 8d 48 10          	lea    rcx,[rax+0x10]
  403332:	c5 c1 ef 04 07       	vpxor  xmm0,xmm7,XMMWORD PTR [rdi+rax*1]
  403337:	81 e1 ff 03 00 00    	and    ecx,0x3ff
  40333d:	c5 fa 6f 34 0e       	vmovdqu xmm6,XMMWORD PTR [rsi+rcx*1]
  403342:	c4 e3 79 44 c0 10    	vpclmullqhqdq xmm0,xmm0,xmm0
  403348:	c5 e1 ef d8          	vpxor  xmm3,xmm3,xmm0
  40334c:	48 8d 48 20          	lea    rcx,[rax+0x20]
  403350:	c5 c9 ef 44 07 10    	vpxor  xmm0,xmm6,XMMWORD PTR [rdi+rax*1+0x10]
  403356:	81 e1 ff 03 00 00    	and    ecx,0x3ff
  40335c:	c5 fa 6f 3c 0e       	vmovdqu xmm7,XMMWORD PTR [rsi+rcx*1]
  403361:	c4 e3 79 44 c0 10    	vpclmullqhqdq xmm0,xmm0,xmm0
  403367:	c5 f1 ef c0          	vpxor  xmm0,xmm1,xmm0
  40336b:	48 8d 48 30          	lea    rcx,[rax+0x30]
  40336f:	c5 e1 ef e8          	vpxor  xmm5,xmm3,xmm0
  403373:	c5 f9 6f c8          	vmovdqa xmm1,xmm0
  403377:	81 e1 ff 03 00 00    	and    ecx,0x3ff
  40337d:	c5 c1 ef 44 07 20    	vpxor  xmm0,xmm7,XMMWORD PTR [rdi+rax*1+0x20]
  403383:	c5 fa 6f 34 0e       	vmovdqu xmm6,XMMWORD PTR [rsi+rcx*1]
  403388:	c4 e3 79 44 c0 10    	vpclmullqhqdq xmm0,xmm0,xmm0
  40338e:	c5 d9 ef e0          	vpxor  xmm4,xmm4,xmm0
  403392:	c5 c9 ef 44 07 30    	vpxor  xmm0,xmm6,XMMWORD PTR [rdi+rax*1+0x30]
  403398:	48 83 c0 40          	add    rax,0x40
  40339c:	c4 e3 79 44 c0 10    	vpclmullqhqdq xmm0,xmm0,xmm0
  4033a2:	c5 e9 ef c0          	vpxor  xmm0,xmm2,xmm0
  4033a6:	c5 d9 ef f0          	vpxor  xmm6,xmm4,xmm0
  4033aa:	c5 f9 6f d0          	vmovdqa xmm2,xmm0
  4033ae:	48 39 c2             	cmp    rdx,rax
  4033b1:	0f 87 69 ff ff ff    	ja     403320 <ph128+0x20>
  4033b7:	c5 d1 ef ee          	vpxor  xmm5,xmm5,xmm6
  4033bb:	c4 e1 f9 7e e8       	vmovq  rax,xmm5
  4033c0:	c3                   	ret    
  4033c1:	31 c0                	xor    eax,eax
  4033c3:	c3                   	ret    
  4033c4:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
  4033cb:	00 00 00 00 
  4033cf:	90                   	nop

00000000004033d0 <dual_scalar>:
  4033d0:	48 85 d2             	test   rdx,rdx
  4033d3:	0f 84 9c 01 00 00    	je     403575 <dual_scalar+0x1a5>
  4033d9:	41 57                	push   r15
  4033db:	c5 e9 ef d2          	vpxor  xmm2,xmm2,xmm2
  4033df:	48 89 f0             	mov    rax,rsi
  4033e2:	41 56                	push   r14
  4033e4:	48 89 d6             	mov    rsi,rdx
  4033e7:	45 31 f6             	xor    r14d,r14d
  4033ea:	41 55                	push   r13
  4033ec:	45 31 ff             	xor    r15d,r15d
  4033ef:	31 c9                	xor    ecx,ecx
  4033f1:	41 54                	push   r12
  4033f3:	45 31 d2             	xor    r10d,r10d
  4033f6:	45 31 db             	xor    r11d,r11d
  4033f9:	55                   	push   rbp
  4033fa:	45 31 e4             	xor    r12d,r12d
  4033fd:	45 31 ed             	xor    r13d,r13d
  403400:	53                   	push   rbx
  403401:	c5 f9 6f e2          	vmovdqa xmm4,xmm2
  403405:	31 db                	xor    ebx,ebx
  403407:	48 c7 44 24 f0 00 00 	mov    QWORD PTR [rsp-0x10],0x0
  40340e:	00 00 
  403410:	c5 f9 6f ca          	vmovdqa xmm1,xmm2
  403414:	c5 f9 6f da          	vmovdqa xmm3,xmm2
  403418:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
  40341f:	00 
  403420:	48 8b 54 24 f0       	mov    rdx,QWORD PTR [rsp-0x10]
  403425:	4c 8b 47 10          	mov    r8,QWORD PTR [rdi+0x10]
  403429:	81 e2 ff 03 00 00    	and    edx,0x3ff
  40342f:	4c 8b 4f 18          	mov    r9,QWORD PTR [rdi+0x18]
  403433:	4c 03 44 10 10       	add    r8,QWORD PTR [rax+rdx*1+0x10]
  403438:	c5 fa 6f 3c 10       	vmovdqu xmm7,XMMWORD PTR [rax+rdx*1]
  40343d:	4c 03 4c 10 18       	add    r9,QWORD PTR [rax+rdx*1+0x18]
  403442:	4c 89 c2             	mov    rdx,r8
  403445:	c4 42 bb f6 c9       	mulx   r9,r8,r9
  40344a:	48 8b 6c 24 f0       	mov    rbp,QWORD PTR [rsp-0x10]
  40344f:	c5 c1 ef 07          	vpxor  xmm0,xmm7,XMMWORD PTR [rdi]
  403453:	4d 01 c4             	add    r12,r8
  403456:	4d 11 cd             	adc    r13,r9
  403459:	4c 8b 47 30          	mov    r8,QWORD PTR [rdi+0x30]
  40345d:	4c 8d 4d 20          	lea    r9,[rbp+0x20]
  403461:	41 81 e1 ff 03 00 00 	and    r9d,0x3ff
  403468:	c4 a1 7a 6f 34 08    	vmovdqu xmm6,XMMWORD PTR [rax+r9*1]
  40346e:	4e 03 44 08 10       	add    r8,QWORD PTR [rax+r9*1+0x10]
  403473:	4e 8b 4c 08 18       	mov    r9,QWORD PTR [rax+r9*1+0x18]
  403478:	c4 e3 79 44 c0 10    	vpclmullqhqdq xmm0,xmm0,xmm0
  40347e:	4c 03 4f 38          	add    r9,QWORD PTR [rdi+0x38]
  403482:	4c 89 ca             	mov    rdx,r9
  403485:	c4 42 bb f6 c8       	mulx   r9,r8,r8
  40348a:	c5 e1 ef d8          	vpxor  xmm3,xmm3,xmm0
  40348e:	c5 c9 ef 47 20       	vpxor  xmm0,xmm6,XMMWORD PTR [rdi+0x20]
  403493:	4d 01 c2             	add    r10,r8
  403496:	4d 11 cb             	adc    r11,r9
  403499:	4c 8b 47 50          	mov    r8,QWORD PTR [rdi+0x50]
  40349d:	4c 8d 4d 40          	lea    r9,[rbp+0x40]
  4034a1:	41 81 e1 ff 03 00 00 	and    r9d,0x3ff
  4034a8:	c4 a1 7a 6f 2c 08    	vmovdqu xmm5,XMMWORD PTR [rax+r9*1]
  4034ae:	4e 03 44 08 10       	add    r8,QWORD PTR [rax+r9*1+0x10]
  4034b3:	4e 8b 4c 08 18       	mov    r9,QWORD PTR [rax+r9*1+0x18]
  4034b8:	c4 e3 79 44 c0 10    	vpclmullqhqdq xmm0,xmm0,xmm0
  4034be:	4c 03 4f 58          	add    r9,QWORD PTR [rdi+0x58]
  4034c2:	4c 89 ca             	mov    rdx,r9
  4034c5:	c4 42 bb f6 c8       	mulx   r9,r8,r8
  4034ca:	c5 f1 ef c0          	vpxor  xmm0,xmm1,xmm0
  4034ce:	c5 e1 ef f0          	vpxor  xmm6,xmm3,xmm0
  4034d2:	c5 f9 6f c8          	vmovdqa xmm1,xmm0
  4034d6:	4c 01 c1             	add    rcx,r8
  4034d9:	4c 11 cb             	adc    rbx,r9
  4034dc:	4c 8b 47 70          	mov    r8,QWORD PTR [rdi+0x70]
  4034e0:	4c 8d 4d 60          	lea    r9,[rbp+0x60]
  4034e4:	41 81 e1 ff 03 00 00 	and    r9d,0x3ff
  4034eb:	4e 03 44 08 10       	add    r8,QWORD PTR [rax+r9*1+0x10]
  4034f0:	c4 a1 7a 6f 3c 08    	vmovdqu xmm7,XMMWORD PTR [rax+r9*1]
  4034f6:	4e 8b 4c 08 18       	mov    r9,QWORD PTR [rax+r9*1+0x18]
  4034fb:	c5 d1 ef 47 40       	vpxor  xmm0,xmm5,XMMWORD PTR [rdi+0x40]
  403500:	4c 03 4f 78          	add    r9,QWORD PTR [rdi+0x78]
  403504:	4c 89 ca             	mov    rdx,r9
  403507:	c4 42 bb f6 c8       	mulx   r9,r8,r8
  40350c:	c4 e3 79 44 c0 10    	vpclmullqhqdq xmm0,xmm0,xmm0
  403512:	c5 d9 ef e0          	vpxor  xmm4,xmm4,xmm0
  403516:	c5 c1 ef 47 60       	vpxor  xmm0,xmm7,XMMWORD PTR [rdi+0x60]
  40351b:	4d 01 c6             	add    r14,r8
  40351e:	c4 e3 79 44 c0 10    	vpclmullqhqdq xmm0,xmm0,xmm0
  403524:	c5 e9 ef c0          	vpxor  xmm0,xmm2,xmm0
  403528:	4d 11 cf             	adc    r15,r9
  40352b:	48 83 ed 80          	sub    rbp,0xffffffffffffff80
  40352f:	48 89 6c 24 f0       	mov    QWORD PTR [rsp-0x10],rbp
  403534:	c5 d9 ef e8          	vpxor  xmm5,xmm4,xmm0
  403538:	c5 f9 6f d0          	vmovdqa xmm2,xmm0
  40353c:	48 83 ef 80          	sub    rdi,0xffffffffffffff80
  403540:	48 39 ee             	cmp    rsi,rbp
  403543:	0f 87 d7 fe ff ff    	ja     403420 <dual_scalar+0x50>
  403549:	49 01 ca             	add    r10,rcx
  40354c:	49 11 db             	adc    r11,rbx
  40354f:	5b                   	pop    rbx
  403550:	5d                   	pop    rbp
  403551:	4d 01 e2             	add    r10,r12
  403554:	c5 d1 ef ee          	vpxor  xmm5,xmm5,xmm6
  403558:	41 5c                	pop    r12
  40355a:	4d 11 eb             	adc    r11,r13
  40355d:	c4 e1 f9 7e e8       	vmovq  rax,xmm5
  403562:	4d 01 f2             	add    r10,r14
  403565:	41 5d                	pop    r13
  403567:	4d 11 fb             	adc    r11,r15
  40356a:	41 5e                	pop    r14
  40356c:	4c 31 d0             	xor    rax,r10
  40356f:	4c 31 d8             	xor    rax,r11
  403572:	41 5f                	pop    r15
  403574:	c3                   	ret    
  403575:	31 c0                	xor    eax,eax
  403577:	c3                   	ret    
  403578:	0f 1f 84 00 00 00 00 	nop    DWORD PTR [rax+rax*1+0x0]
  40357f:	00 

0000000000403580 <ph512>:
  403580:	48 89 f9             	mov    rcx,rdi
  403583:	48 85 d2             	test   rdx,rdx
  403586:	0f 84 19 01 00 00    	je     4036a5 <ph512+0x125>
  40358c:	c5 d9 ef e4          	vpxor  xmm4,xmm4,xmm4
  403590:	62 f1 fd 48 6f f4    	vmovdqa64 zmm6,zmm4
  403596:	62 f1 fd 48 6f dc    	vmovdqa64 zmm3,zmm4
  40359c:	62 f1 fd 48 6f ec    	vmovdqa64 zmm5,zmm4
  4035a2:	31 c0                	xor    eax,eax
  4035a4:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]
  4035a8:	48 89 c7             	mov    rdi,rax
  4035ab:	81 e7 ff 03 00 00    	and    edi,0x3ff
  4035b1:	62 f1 7e 48 6f 3c 3e 	vmovdqu32 zmm7,ZMMWORD PTR [rsi+rdi*1]
  4035b8:	48 8d 78 40          	lea    rdi,[rax+0x40]
  4035bc:	81 e7 ff 03 00 00    	and    edi,0x3ff
  4035c2:	62 f1 45 48 ef 04 01 	vpxord zmm0,zmm7,ZMMWORD PTR [rcx+rax*1]
  4035c9:	62 f1 7e 48 6f 3c 3e 	vmovdqu32 zmm7,ZMMWORD PTR [rsi+rdi*1]
  4035d0:	62 f3 7d 48 44 c0 10 	vpclmullqhqdq zmm0,zmm0,zmm0
  4035d7:	62 f1 45 48 ef 4c 01 	vpxord zmm1,zmm7,ZMMWORD PTR [rcx+rax*1+0x40]
  4035de:	01 
  4035df:	48 8d b8 80 00 00 00 	lea    rdi,[rax+0x80]
  4035e6:	62 f3 75 48 44 c9 10 	vpclmullqhqdq zmm1,zmm1,zmm1
  4035ed:	81 e7 ff 03 00 00    	and    edi,0x3ff
  4035f3:	62 f1 7e 48 6f 3c 3e 	vmovdqu32 zmm7,ZMMWORD PTR [rsi+rdi*1]
  4035fa:	48 8d b8 c0 00 00 00 	lea    rdi,[rax+0xc0]
  403601:	62 f1 55 48 ef c0    	vpxord zmm0,zmm5,zmm0
  403607:	81 e7 ff 03 00 00    	and    edi,0x3ff
  40360d:	62 f1 65 48 ef c9    	vpxord zmm1,zmm3,zmm1
  403613:	62 f1 fd 48 6f e8    	vmovdqa64 zmm5,zmm0
  403619:	62 f1 fd 48 6f d9    	vmovdqa64 zmm3,zmm1
  40361f:	62 f1 7d 48 ef c1    	vpxord zmm0,zmm0,zmm1
  403625:	62 f1 45 48 ef 4c 01 	vpxord zmm1,zmm7,ZMMWORD PTR [rcx+rax*1+0x80]
  40362c:	02 
  40362d:	62 f1 7e 48 6f 3c 3e 	vmovdqu32 zmm7,ZMMWORD PTR [rsi+rdi*1]
  403634:	62 f3 75 48 44 c9 10 	vpclmullqhqdq zmm1,zmm1,zmm1
  40363b:	62 f1 45 48 ef 54 01 	vpxord zmm2,zmm7,ZMMWORD PTR [rcx+rax*1+0xc0]
  403642:	03 
  403643:	48 05 00 01 00 00    	add    rax,0x100
  403649:	62 f3 6d 48 44 d2 10 	vpclmullqhqdq zmm2,zmm2,zmm2
  403650:	62 f1 4d 48 ef c9    	vpxord zmm1,zmm6,zmm1
  403656:	62 f1 fd 48 6f f1    	vmovdqa64 zmm6,zmm1
  40365c:	62 f1 5d 48 ef d2    	vpxord zmm2,zmm4,zmm2
  403662:	62 f1 75 48 ef ca    	vpxord zmm1,zmm1,zmm2
  403668:	62 f1 fd 48 6f e2    	vmovdqa64 zmm4,zmm2
  40366e:	48 39 c2             	cmp    rdx,rax
  403671:	0f 87 31 ff ff ff    	ja     4035a8 <ph512+0x28>
  403677:	62 f1 7d 48 ef c1    	vpxord zmm0,zmm0,zmm1
  40367d:	62 f3 fd 48 3b c1 01 	vextracti64x4 ymm1,zmm0,0x1
  403684:	c5 f5 d4 c8          	vpaddq ymm1,ymm1,ymm0
  403688:	62 f3 fd 28 39 c8 01 	vextracti64x2 xmm0,ymm1,0x1
  40368f:	c5 f9 d4 c1          	vpaddq xmm0,xmm0,xmm1
  403693:	c4 e1 f9 7e c0       	vmovq  rax,xmm0
  403698:	c4 e3 f9 16 c2 01    	vpextrq rdx,xmm0,0x1
  40369e:	48 01 d0             	add    rax,rdx
  4036a1:	c5 f8 77             	vzeroupper 
  4036a4:	c3                   	ret    
  4036a5:	c5 f9 ef c0          	vpxor  xmm0,xmm0,xmm0
  4036a9:	eb d2                	jmp    40367d <ph512+0xfd>
  4036ab:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]

00000000004036b0 <dual_vector>:
  4036b0:	55                   	push   rbp
  4036b1:	c5 f9 ef c0          	vpxor  xmm0,xmm0,xmm0
  4036b5:	48 89 e5             	mov    rbp,rsp
  4036b8:	48 83 e4 c0          	and    rsp,0xffffffffffffffc0
  4036bc:	48 81 ec 88 02 00 00 	sub    rsp,0x288
  4036c3:	c5 f9 7f 84 24 88 01 	vmovdqa XMMWORD PTR [rsp+0x188],xmm0
  4036ca:	00 00 
  4036cc:	c5 f9 7f 84 24 98 01 	vmovdqa XMMWORD PTR [rsp+0x198],xmm0
  4036d3:	00 00 
  4036d5:	c5 f9 7f 84 24 a8 01 	vmovdqa XMMWORD PTR [rsp+0x1a8],xmm0
  4036dc:	00 00 
  4036de:	c5 f9 7f 84 24 b8 01 	vmovdqa XMMWORD PTR [rsp+0x1b8],xmm0
  4036e5:	00 00 
  4036e7:	c5 f9 7f 84 24 c8 01 	vmovdqa XMMWORD PTR [rsp+0x1c8],xmm0
  4036ee:	00 00 
  4036f0:	c5 f9 7f 84 24 d8 01 	vmovdqa XMMWORD PTR [rsp+0x1d8],xmm0
  4036f7:	00 00 
  4036f9:	c5 f9 7f 84 24 e8 01 	vmovdqa XMMWORD PTR [rsp+0x1e8],xmm0
  403700:	00 00 
  403702:	c5 f9 7f 84 24 f8 01 	vmovdqa XMMWORD PTR [rsp+0x1f8],xmm0
  403709:	00 00 
  40370b:	c5 f9 7f 84 24 08 02 	vmovdqa XMMWORD PTR [rsp+0x208],xmm0
  403712:	00 00 
  403714:	c5 f9 7f 84 24 18 02 	vmovdqa XMMWORD PTR [rsp+0x218],xmm0
  40371b:	00 00 
  40371d:	c5 f9 7f 84 24 28 02 	vmovdqa XMMWORD PTR [rsp+0x228],xmm0
  403724:	00 00 
  403726:	c5 f9 7f 84 24 38 02 	vmovdqa XMMWORD PTR [rsp+0x238],xmm0
  40372d:	00 00 
  40372f:	c5 f9 7f 84 24 48 02 	vmovdqa XMMWORD PTR [rsp+0x248],xmm0
  403736:	00 00 
  403738:	c5 f9 7f 84 24 58 02 	vmovdqa XMMWORD PTR [rsp+0x258],xmm0
  40373f:	00 00 
  403741:	c5 f9 7f 84 24 68 02 	vmovdqa XMMWORD PTR [rsp+0x268],xmm0
  403748:	00 00 
  40374a:	c5 f9 7f 84 24 78 02 	vmovdqa XMMWORD PTR [rsp+0x278],xmm0
  403751:	00 00 
  403753:	c5 f9 7f 84 24 88 00 	vmovdqa XMMWORD PTR [rsp+0x88],xmm0
  40375a:	00 00 
  40375c:	c5 f9 7f 84 24 98 00 	vmovdqa XMMWORD PTR [rsp+0x98],xmm0
  403763:	00 00 
  403765:	c5 f9 7f 84 24 a8 00 	vmovdqa XMMWORD PTR [rsp+0xa8],xmm0
  40376c:	00 00 
  40376e:	c5 f9 7f 84 24 b8 00 	vmovdqa XMMWORD PTR [rsp+0xb8],xmm0
  403775:	00 00 
  403777:	c5 f9 7f 84 24 c8 00 	vmovdqa XMMWORD PTR [rsp+0xc8],xmm0
  40377e:	00 00 
  403780:	c5 f9 7f 84 24 d8 00 	vmovdqa XMMWORD PTR [rsp+0xd8],xmm0
  403787:	00 00 
  403789:	c5 f9 7f 84 24 e8 00 	vmovdqa XMMWORD PTR [rsp+0xe8],xmm0
  403790:	00 00 
  403792:	c5 f9 7f 84 24 f8 00 	vmovdqa XMMWORD PTR [rsp+0xf8],xmm0
  403799:	00 00 
  40379b:	c5 f9 7f 84 24 08 01 	vmovdqa XMMWORD PTR [rsp+0x108],xmm0
  4037a2:	00 00 
  4037a4:	c5 f9 7f 84 24 18 01 	vmovdqa XMMWORD PTR [rsp+0x118],xmm0
  4037ab:	00 00 
  4037ad:	c5 f9 7f 84 24 28 01 	vmovdqa XMMWORD PTR [rsp+0x128],xmm0
  4037b4:	00 00 
  4037b6:	c5 f9 7f 84 24 38 01 	vmovdqa XMMWORD PTR [rsp+0x138],xmm0
  4037bd:	00 00 
  4037bf:	c5 f9 7f 84 24 48 01 	vmovdqa XMMWORD PTR [rsp+0x148],xmm0
  4037c6:	00 00 
  4037c8:	c5 f9 7f 84 24 58 01 	vmovdqa XMMWORD PTR [rsp+0x158],xmm0
  4037cf:	00 00 
  4037d1:	c5 f9 7f 84 24 68 01 	vmovdqa XMMWORD PTR [rsp+0x168],xmm0
  4037d8:	00 00 
  4037da:	c5 f9 7f 84 24 78 01 	vmovdqa XMMWORD PTR [rsp+0x178],xmm0
  4037e1:	00 00 
  4037e3:	c5 f9 7f 44 24 88    	vmovdqa XMMWORD PTR [rsp-0x78],xmm0
  4037e9:	c5 f9 7f 44 24 98    	vmovdqa XMMWORD PTR [rsp-0x68],xmm0
  4037ef:	c5 f9 7f 44 24 a8    	vmovdqa XMMWORD PTR [rsp-0x58],xmm0
  4037f5:	c5 f9 7f 44 24 b8    	vmovdqa XMMWORD PTR [rsp-0x48],xmm0
  4037fb:	c5 f9 7f 44 24 c8    	vmovdqa XMMWORD PTR [rsp-0x38],xmm0
  403801:	c5 f9 7f 44 24 d8    	vmovdqa XMMWORD PTR [rsp-0x28],xmm0
  403807:	c5 f9 7f 44 24 e8    	vmovdqa XMMWORD PTR [rsp-0x18],xmm0
  40380d:	c5 f9 7f 44 24 f8    	vmovdqa XMMWORD PTR [rsp-0x8],xmm0
  403813:	c5 f9 7f 44 24 08    	vmovdqa XMMWORD PTR [rsp+0x8],xmm0
  403819:	c5 f9 7f 44 24 18    	vmovdqa XMMWORD PTR [rsp+0x18],xmm0
  40381f:	c5 f9 7f 44 24 28    	vmovdqa XMMWORD PTR [rsp+0x28],xmm0
  403825:	c5 f9 7f 44 24 38    	vmovdqa XMMWORD PTR [rsp+0x38],xmm0
  40382b:	c5 f9 7f 44 24 48    	vmovdqa XMMWORD PTR [rsp+0x48],xmm0
  403831:	c5 f9 7f 44 24 58    	vmovdqa XMMWORD PTR [rsp+0x58],xmm0
  403837:	c5 f9 7f 44 24 68    	vmovdqa XMMWORD PTR [rsp+0x68],xmm0
  40383d:	c5 f9 7f 44 24 78    	vmovdqa XMMWORD PTR [rsp+0x78],xmm0
  403843:	48 85 d2             	test   rdx,rdx
  403846:	0f 84 c3 02 00 00    	je     403b0f <dual_vector+0x45f>
  40384c:	c5 d9 ef e4          	vpxor  xmm4,xmm4,xmm4
  403850:	48 89 f0             	mov    rax,rsi
  403853:	62 f1 fd 48 6f ec    	vmovdqa64 zmm5,zmm4
  403859:	48 89 d6             	mov    rsi,rdx
  40385c:	62 71 fd 48 6f fc    	vmovdqa64 zmm15,zmm4
  403862:	62 f1 fd 48 6f f4    	vmovdqa64 zmm6,zmm4
  403868:	62 f1 fd 48 6f fc    	vmovdqa64 zmm7,zmm4
  40386e:	62 71 fd 48 6f f4    	vmovdqa64 zmm14,zmm4
  403874:	62 71 fd 48 6f c4    	vmovdqa64 zmm8,zmm4
  40387a:	62 71 fd 48 6f cc    	vmovdqa64 zmm9,zmm4
  403880:	62 71 fd 48 6f ec    	vmovdqa64 zmm13,zmm4
  403886:	62 71 fd 48 6f d4    	vmovdqa64 zmm10,zmm4
  40388c:	62 71 fd 48 6f dc    	vmovdqa64 zmm11,zmm4
  403892:	62 71 fd 48 6f e4    	vmovdqa64 zmm12,zmm4
  403898:	31 d2                	xor    edx,edx
  40389a:	66 0f 1f 44 00 00    	nop    WORD PTR [rax+rax*1+0x0]
  4038a0:	48 89 d1             	mov    rcx,rdx
  4038a3:	81 e1 ff 03 00 00    	and    ecx,0x3ff
  4038a9:	62 f1 fe 48 6f 47 01 	vmovdqu64 zmm0,ZMMWORD PTR [rdi+0x40]
  4038b0:	62 f1 7e 48 6f 1c 08 	vmovdqu32 zmm3,ZMMWORD PTR [rax+rcx*1]
  4038b7:	48 8d 4a 40          	lea    rcx,[rdx+0x40]
  4038bb:	81 e1 ff 03 00 00    	and    ecx,0x3ff
  4038c1:	62 f1 7d 48 fe 0c 08 	vpaddd zmm1,zmm0,ZMMWORD PTR [rax+rcx*1]
  4038c8:	62 f1 7d 48 fe 44 08 	vpaddd zmm0,zmm0,ZMMWORD PTR [rax+rcx*1+0x400]
  4038cf:	10 
  4038d0:	62 f1 ed 48 73 d1 20 	vpsrlq zmm2,zmm1,0x20
  4038d7:	62 f1 f5 48 f4 ca    	vpmuludq zmm1,zmm1,zmm2
  4038dd:	48 8d 8a 80 00 00 00 	lea    rcx,[rdx+0x80]
  4038e4:	81 e1 ff 03 00 00    	and    ecx,0x3ff
  4038ea:	62 f1 65 48 ef 1f    	vpxord zmm3,zmm3,ZMMWORD PTR [rdi]
  4038f0:	48 81 c7 00 02 00 00 	add    rdi,0x200
  4038f7:	62 71 a5 48 d4 d9    	vpaddq zmm11,zmm11,zmm1
  4038fd:	62 f1 f5 48 73 d0 20 	vpsrlq zmm1,zmm0,0x20
  403904:	62 f1 fd 48 f4 c1    	vpmuludq zmm0,zmm0,zmm1
  40390a:	62 f1 7e 48 6f 0c 08 	vmovdqu32 zmm1,ZMMWORD PTR [rax+rcx*1]
  403911:	48 8d 8a c0 00 00 00 	lea    rcx,[rdx+0xc0]
  403918:	81 e1 ff 03 00 00    	and    ecx,0x3ff
  40391e:	62 f3 65 48 44 db 10 	vpclmullqhqdq zmm3,zmm3,zmm3
  403925:	62 71 ad 48 d4 d0    	vpaddq zmm10,zmm10,zmm0
  40392b:	62 f1 75 48 ef 47 fa 	vpxord zmm0,zmm1,ZMMWORD PTR [rdi-0x180]
  403932:	62 f3 7d 48 44 c0 10 	vpclmullqhqdq zmm0,zmm0,zmm0
  403939:	62 f1 1d 48 ef db    	vpxord zmm3,zmm12,zmm3
  40393f:	62 71 fd 48 6f e3    	vmovdqa64 zmm12,zmm3
  403945:	62 e1 15 48 ef c0    	vpxord zmm16,zmm13,zmm0
  40394b:	62 f1 fe 48 6f 47 fb 	vmovdqu64 zmm0,ZMMWORD PTR [rdi-0x140]
  403952:	62 31 fd 48 6f e8    	vmovdqa64 zmm13,zmm16
  403958:	62 f1 7d 48 fe 0c 08 	vpaddd zmm1,zmm0,ZMMWORD PTR [rax+rcx*1]
  40395f:	62 f1 7d 48 fe 44 08 	vpaddd zmm0,zmm0,ZMMWORD PTR [rax+rcx*1+0x400]
  403966:	10 
  403967:	62 f1 ed 48 73 d1 20 	vpsrlq zmm2,zmm1,0x20
  40396e:	62 f1 f5 48 f4 ca    	vpmuludq zmm1,zmm1,zmm2
  403974:	48 8d 8a 00 01 00 00 	lea    rcx,[rdx+0x100]
  40397b:	81 e1 ff 03 00 00    	and    ecx,0x3ff
  403981:	62 71 b5 48 d4 c9    	vpaddq zmm9,zmm9,zmm1
  403987:	62 f1 f5 48 73 d0 20 	vpsrlq zmm1,zmm0,0x20
  40398e:	62 f1 fd 48 f4 c1    	vpmuludq zmm0,zmm0,zmm1
  403994:	62 f1 7e 48 6f 0c 08 	vmovdqu32 zmm1,ZMMWORD PTR [rax+rcx*1]
  40399b:	48 8d 8a 40 01 00 00 	lea    rcx,[rdx+0x140]
  4039a2:	81 e1 ff 03 00 00    	and    ecx,0x3ff
  4039a8:	62 71 bd 48 d4 c0    	vpaddq zmm8,zmm8,zmm0
  4039ae:	62 f1 75 48 ef 47 fc 	vpxord zmm0,zmm1,ZMMWORD PTR [rdi-0x100]
  4039b5:	62 f3 7d 48 44 c0 10 	vpclmullqhqdq zmm0,zmm0,zmm0
  4039bc:	62 f1 0d 48 ef c8    	vpxord zmm1,zmm14,zmm0
  4039c2:	62 f1 fe 48 6f 47 fd 	vmovdqu64 zmm0,ZMMWORD PTR [rdi-0xc0]
  4039c9:	62 71 fd 48 6f f1    	vmovdqa64 zmm14,zmm1
  4039cf:	62 f1 7d 48 fe 14 08 	vpaddd zmm2,zmm0,ZMMWORD PTR [rax+rcx*1]
  4039d6:	62 f1 7d 48 fe 44 08 	vpaddd zmm0,zmm0,ZMMWORD PTR [rax+rcx*1+0x400]
  4039dd:	10 
  4039de:	62 f1 f5 40 73 d2 20 	vpsrlq zmm17,zmm2,0x20
  4039e5:	62 b1 ed 48 f4 d1    	vpmuludq zmm2,zmm2,zmm17
  4039eb:	48 8d 8a 80 01 00 00 	lea    rcx,[rdx+0x180]
  4039f2:	81 e1 ff 03 00 00    	and    ecx,0x3ff
  4039f8:	62 f1 c5 48 d4 fa    	vpaddq zmm7,zmm7,zmm2
  4039fe:	62 f1 ed 48 73 d0 20 	vpsrlq zmm2,zmm0,0x20
  403a05:	62 f1 fd 48 f4 c2    	vpmuludq zmm0,zmm0,zmm2
  403a0b:	62 f1 7e 48 6f 14 08 	vmovdqu32 zmm2,ZMMWORD PTR [rax+rcx*1]
  403a12:	48 8d 8a c0 01 00 00 	lea    rcx,[rdx+0x1c0]
  403a19:	81 e1 ff 03 00 00    	and    ecx,0x3ff
  403a1f:	48 81 c2 00 02 00 00 	add    rdx,0x200
  403a26:	62 f1 cd 48 d4 f0    	vpaddq zmm6,zmm6,zmm0
  403a2c:	62 f1 6d 48 ef 47 fe 	vpxord zmm0,zmm2,ZMMWORD PTR [rdi-0x80]
  403a33:	62 f1 fe 48 6f 57 ff 	vmovdqu64 zmm2,ZMMWORD PTR [rdi-0x40]
  403a3a:	62 f3 7d 48 44 c0 10 	vpclmullqhqdq zmm0,zmm0,zmm0
  403a41:	62 e1 6d 48 fe 0c 08 	vpaddd zmm17,zmm2,ZMMWORD PTR [rax+rcx*1]
  403a48:	62 f1 6d 48 fe 54 08 	vpaddd zmm2,zmm2,ZMMWORD PTR [rax+rcx*1+0x400]
  403a4f:	10 
  403a50:	62 b1 ed 40 73 d1 20 	vpsrlq zmm18,zmm17,0x20
  403a57:	62 a1 f5 40 f4 ca    	vpmuludq zmm17,zmm17,zmm18
  403a5d:	62 f1 05 48 ef c0    	vpxord zmm0,zmm15,zmm0
  403a63:	62 71 fd 48 6f f8    	vmovdqa64 zmm15,zmm0
  403a69:	62 b1 d5 48 d4 e9    	vpaddq zmm5,zmm5,zmm17
  403a6f:	62 f1 f5 40 73 d2 20 	vpsrlq zmm17,zmm2,0x20
  403a76:	62 b1 ed 48 f4 d1    	vpmuludq zmm2,zmm2,zmm17
  403a7c:	62 f1 dd 48 d4 e2    	vpaddq zmm4,zmm4,zmm2
  403a82:	48 39 d6             	cmp    rsi,rdx
  403a85:	0f 87 15 fe ff ff    	ja     4038a0 <dual_vector+0x1f0>
  403a8b:	62 f1 fd 48 7f 9c 24 	vmovdqa64 ZMMWORD PTR [rsp-0x78],zmm3
  403a92:	88 ff ff ff 
  403a96:	62 71 fd 48 7f 9c 24 	vmovdqa64 ZMMWORD PTR [rsp+0x88],zmm11
  403a9d:	88 00 00 00 
  403aa1:	62 71 fd 48 7f 94 24 	vmovdqa64 ZMMWORD PTR [rsp+0x188],zmm10
  403aa8:	88 01 00 00 
  403aac:	62 e1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp-0x38],zmm16
  403ab3:	c8 ff ff ff 
  403ab7:	62 71 fd 48 7f 8c 24 	vmovdqa64 ZMMWORD PTR [rsp+0xc8],zmm9
  403abe:	c8 00 00 00 
  403ac2:	62 71 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x1c8],zmm8
  403ac9:	c8 01 00 00 
  403acd:	62 f1 fd 48 7f 8c 24 	vmovdqa64 ZMMWORD PTR [rsp+0x8],zmm1
  403ad4:	08 00 00 00 
  403ad8:	62 f1 fd 48 7f bc 24 	vmovdqa64 ZMMWORD PTR [rsp+0x108],zmm7
  403adf:	08 01 00 00 
  403ae3:	62 f1 fd 48 7f b4 24 	vmovdqa64 ZMMWORD PTR [rsp+0x208],zmm6
  403aea:	08 02 00 00 
  403aee:	62 f1 fd 48 7f 84 24 	vmovdqa64 ZMMWORD PTR [rsp+0x48],zmm0
  403af5:	48 00 00 00 
  403af9:	62 f1 fd 48 7f ac 24 	vmovdqa64 ZMMWORD PTR [rsp+0x148],zmm5
  403b00:	48 01 00 00 
  403b04:	62 f1 fd 48 7f a4 24 	vmovdqa64 ZMMWORD PTR [rsp+0x248],zmm4
  403b0b:	48 02 00 00 
  403b0f:	62 f1 fd 48 6f 84 24 	vmovdqa64 zmm0,ZMMWORD PTR [rsp-0x78]
  403b16:	88 ff ff ff 
  403b1a:	62 f1 fd 48 6f a4 24 	vmovdqa64 zmm4,ZMMWORD PTR [rsp+0x88]
  403b21:	88 00 00 00 
  403b25:	62 d3 fd 48 3b c0 01 	vextracti64x4 ymm8,zmm0,0x1
  403b2c:	c5 3d d4 c0          	vpaddq ymm8,ymm8,ymm0
  403b30:	62 73 fd 28 39 c0 01 	vextracti64x2 xmm0,ymm8,0x1
  403b37:	c5 39 d4 c0          	vpaddq xmm8,xmm8,xmm0
  403b3b:	62 f3 fd 48 3b e0 01 	vextracti64x4 ymm0,zmm4,0x1
  403b42:	62 f1 fd 48 6f 9c 24 	vmovdqa64 zmm3,ZMMWORD PTR [rsp+0x188]
  403b49:	88 01 00 00 
  403b4d:	c5 dd d4 e0          	vpaddq ymm4,ymm4,ymm0
  403b51:	62 f3 fd 28 39 e0 01 	vextracti64x2 xmm0,ymm4,0x1
  403b58:	c5 d9 d4 e0          	vpaddq xmm4,xmm4,xmm0
  403b5c:	62 f3 fd 48 3b d8 01 	vextracti64x4 ymm0,zmm3,0x1
  403b63:	62 f1 fd 48 6f b4 24 	vmovdqa64 zmm6,ZMMWORD PTR [rsp-0x38]
  403b6a:	c8 ff ff ff 
  403b6e:	c5 e5 d4 d8          	vpaddq ymm3,ymm3,ymm0
  403b72:	62 f3 fd 28 39 d8 01 	vextracti64x2 xmm0,ymm3,0x1
  403b79:	c5 e1 d4 d8          	vpaddq xmm3,xmm3,xmm0
  403b7d:	62 f3 fd 48 3b f0 01 	vextracti64x4 ymm0,zmm6,0x1
  403b84:	62 f1 fd 48 6f bc 24 	vmovdqa64 zmm7,ZMMWORD PTR [rsp+0xc8]
  403b8b:	c8 00 00 00 
  403b8f:	c5 cd d4 f0          	vpaddq ymm6,ymm6,ymm0
  403b93:	62 f3 fd 28 39 f0 01 	vextracti64x2 xmm0,ymm6,0x1
  403b9a:	c5 c9 d4 f0          	vpaddq xmm6,xmm6,xmm0
  403b9e:	62 f3 fd 48 3b f8 01 	vextracti64x4 ymm0,zmm7,0x1
  403ba5:	62 f1 fd 48 6f ac 24 	vmovdqa64 zmm5,ZMMWORD PTR [rsp+0x1c8]
  403bac:	c8 01 00 00 
  403bb0:	c5 c5 d4 f8          	vpaddq ymm7,ymm7,ymm0
  403bb4:	62 f3 fd 28 39 f8 01 	vextracti64x2 xmm0,ymm7,0x1
  403bbb:	c5 c1 d4 f8          	vpaddq xmm7,xmm7,xmm0
  403bbf:	62 f3 fd 48 3b e8 01 	vextracti64x4 ymm0,zmm5,0x1
  403bc6:	62 f1 fd 48 6f 8c 24 	vmovdqa64 zmm1,ZMMWORD PTR [rsp+0x8]
  403bcd:	08 00 00 00 
  403bd1:	c5 d5 d4 e8          	vpaddq ymm5,ymm5,ymm0
  403bd5:	62 f3 fd 28 39 e8 01 	vextracti64x2 xmm0,ymm5,0x1
  403bdc:	c5 d1 d4 e8          	vpaddq xmm5,xmm5,xmm0
  403be0:	62 f3 fd 48 3b c8 01 	vextracti64x4 ymm0,zmm1,0x1
  403be7:	62 f1 fd 48 6f 94 24 	vmovdqa64 zmm2,ZMMWORD PTR [rsp+0x108]
  403bee:	08 01 00 00 
  403bf2:	c5 f5 d4 c8          	vpaddq ymm1,ymm1,ymm0
  403bf6:	62 f3 fd 28 39 c8 01 	vextracti64x2 xmm0,ymm1,0x1
  403bfd:	c5 f1 d4 c8          	vpaddq xmm1,xmm1,xmm0
  403c01:	c4 e1 f9 7e fa       	vmovq  rdx,xmm7
  403c06:	62 f3 fd 48 3b d0 01 	vextracti64x4 ymm0,zmm2,0x1
  403c0d:	c4 e3 f9 16 f8 01    	vpextrq rax,xmm7,0x1
  403c13:	48 01 d0             	add    rax,rdx
  403c16:	c4 e3 f9 16 f1 01    	vpextrq rcx,xmm6,0x1
  403c1c:	c5 ed d4 d0          	vpaddq ymm2,ymm2,ymm0
  403c20:	c4 e1 f9 7e f2       	vmovq  rdx,xmm6
  403c25:	48 01 ca             	add    rdx,rcx
  403c28:	62 f3 fd 28 39 d0 01 	vextracti64x2 xmm0,ymm2,0x1
  403c2f:	c5 e9 d4 d0          	vpaddq xmm2,xmm2,xmm0
  403c33:	48 31 d0             	xor    rax,rdx
  403c36:	62 f1 fd 48 6f 84 24 	vmovdqa64 zmm0,ZMMWORD PTR [rsp+0x208]
  403c3d:	08 02 00 00 
  403c41:	c4 e3 f9 16 e9 01    	vpextrq rcx,xmm5,0x1
  403c47:	c4 e1 f9 7e ea       	vmovq  rdx,xmm5
  403c4c:	48 01 ca             	add    rdx,rcx
  403c4f:	48 31 d0             	xor    rax,rdx
  403c52:	c4 e1 f9 7e e1       	vmovq  rcx,xmm4
  403c57:	62 d3 fd 48 3b c1 01 	vextracti64x4 ymm9,zmm0,0x1
  403c5e:	c4 e3 f9 16 e2 01    	vpextrq rdx,xmm4,0x1
  403c64:	62 71 fd 48 6f 94 24 	vmovdqa64 zmm10,ZMMWORD PTR [rsp+0x48]
  403c6b:	48 00 00 00 
  403c6f:	48 01 ca             	add    rdx,rcx
  403c72:	c4 63 f9 16 c6 01    	vpextrq rsi,xmm8,0x1
  403c78:	c4 c1 7d d4 c1       	vpaddq ymm0,ymm0,ymm9
  403c7d:	c4 61 f9 7e c1       	vmovq  rcx,xmm8
  403c82:	48 01 f1             	add    rcx,rsi
  403c85:	62 d3 fd 28 39 c1 01 	vextracti64x2 xmm9,ymm0,0x1
  403c8c:	c4 c1 79 d4 c1       	vpaddq xmm0,xmm0,xmm9
  403c91:	48 31 ca             	xor    rdx,rcx
  403c94:	c4 e3 f9 16 de 01    	vpextrq rsi,xmm3,0x1
  403c9a:	62 53 fd 48 3b d1 01 	vextracti64x4 ymm9,zmm10,0x1
  403ca1:	c4 e1 f9 7e d9       	vmovq  rcx,xmm3
  403ca6:	62 71 fd 48 6f 9c 24 	vmovdqa64 zmm11,ZMMWORD PTR [rsp+0x148]
  403cad:	48 01 00 00 
  403cb1:	48 01 f1             	add    rcx,rsi
  403cb4:	c4 41 35 d4 ca       	vpaddq ymm9,ymm9,ymm10
  403cb9:	48 31 ca             	xor    rdx,rcx
  403cbc:	62 53 fd 28 39 ca 01 	vextracti64x2 xmm10,ymm9,0x1
  403cc3:	c4 41 29 d4 d1       	vpaddq xmm10,xmm10,xmm9
  403cc8:	48 31 d0             	xor    rax,rdx
  403ccb:	c4 e1 f9 7e d1       	vmovq  rcx,xmm2
  403cd0:	62 53 fd 48 3b d9 01 	vextracti64x4 ymm9,zmm11,0x1
  403cd7:	c4 e3 f9 16 d2 01    	vpextrq rdx,xmm2,0x1
  403cdd:	48 01 ca             	add    rdx,rcx
  403ce0:	c4 41 35 d4 cb       	vpaddq ymm9,ymm9,ymm11
  403ce5:	c4 e3 f9 16 ce 01    	vpextrq rsi,xmm1,0x1
  403ceb:	c4 e1 f9 7e c9       	vmovq  rcx,xmm1
  403cf0:	48 01 f1             	add    rcx,rsi
  403cf3:	62 53 fd 28 39 cb 01 	vextracti64x2 xmm11,ymm9,0x1
  403cfa:	c4 41 21 d4 d9       	vpaddq xmm11,xmm11,xmm9
  403cff:	48 31 d1             	xor    rcx,rdx
  403d02:	62 71 fd 48 6f 8c 24 	vmovdqa64 zmm9,ZMMWORD PTR [rsp+0x248]
  403d09:	48 02 00 00 
  403d0d:	c4 e3 f9 16 c6 01    	vpextrq rsi,xmm0,0x1
  403d13:	c4 e1 f9 7e c2       	vmovq  rdx,xmm0
  403d18:	48 01 f2             	add    rdx,rsi
  403d1b:	48 31 ca             	xor    rdx,rcx
  403d1e:	62 53 fd 48 3b cc 01 	vextracti64x4 ymm12,zmm9,0x1
  403d25:	c4 41 1d d4 e1       	vpaddq ymm12,ymm12,ymm9
  403d2a:	48 31 d0             	xor    rax,rdx
  403d2d:	c4 63 f9 16 d9 01    	vpextrq rcx,xmm11,0x1
  403d33:	c4 61 f9 7e da       	vmovq  rdx,xmm11
  403d38:	48 01 ca             	add    rdx,rcx
  403d3b:	c4 61 f9 7e d6       	vmovq  rsi,xmm10
  403d40:	62 53 fd 28 39 e1 01 	vextracti64x2 xmm9,ymm12,0x1
  403d47:	c4 63 f9 16 d1 01    	vpextrq rcx,xmm10,0x1
  403d4d:	48 01 f1             	add    rcx,rsi
  403d50:	c4 41 31 d4 cc       	vpaddq xmm9,xmm9,xmm12
  403d55:	48 31 ca             	xor    rdx,rcx
  403d58:	c4 61 f9 7e ce       	vmovq  rsi,xmm9
  403d5d:	c4 63 f9 16 c9 01    	vpextrq rcx,xmm9,0x1
  403d63:	48 01 f1             	add    rcx,rsi
  403d66:	48 31 ca             	xor    rdx,rcx
  403d69:	48 31 d0             	xor    rax,rdx
  403d6c:	c5 f8 77             	vzeroupper 
  403d6f:	c9                   	leave  
  403d70:	c3                   	ret    
  403d71:	66 66 2e 0f 1f 84 00 	data16 nop WORD PTR cs:[rax+rax*1+0x0]
  403d78:	00 00 00 00 
  403d7c:	0f 1f 40 00          	nop    DWORD PTR [rax+0x0]

0000000000403d80 <dual_vector_reduced>:
  403d80:	55                   	push   rbp
  403d81:	48 89 e5             	mov    rbp,rsp
  403d84:	48 83 e4 c0          	and    rsp,0xffffffffffffffc0
  403d88:	48 81 ec 88 03 00 00 	sub    rsp,0x388
  403d8f:	c5 fa 6f a6 00 08 00 	vmovdqu xmm4,XMMWORD PTR [rsi+0x800]
  403d96:	00 
  403d97:	c5 f9 7f a4 24 78 03 	vmovdqa XMMWORD PTR [rsp+0x378],xmm4
  403d9e:	00 00 
  403da0:	c5 fa 6f a6 10 08 00 	vmovdqu xmm4,XMMWORD PTR [rsi+0x810]
  403da7:	00 
  403da8:	c5 f9 7f a4 24 68 03 	vmovdqa XMMWORD PTR [rsp+0x368],xmm4
  403daf:	00 00 
  403db1:	48 85 d2             	test   rdx,rdx
  403db4:	0f 84 00 05 00 00    	je     4042ba <dual_vector_reduced+0x53a>
  403dba:	62 f1 7e 48 6f 66 11 	vmovdqu32 zmm4,ZMMWORD PTR [rsi+0x440]
  403dc1:	62 61 7e 48 6f 36    	vmovdqu32 zmm30,ZMMWORD PTR [rsi]
  403dc7:	62 f1 7d 48 7f a4 24 	vmovdqa32 ZMMWORD PTR [rsp+0x308],zmm4
  403dce:	08 03 00 00 
  403dd2:	62 f1 7e 48 6f 66 03 	vmovdqu32 zmm4,ZMMWORD PTR [rsi+0xc0]
  403dd9:	62 61 7e 48 6f 6e 01 	vmovdqu32 zmm29,ZMMWORD PTR [rsi+0x40]
  403de0:	62 f1 7d 48 7f a4 24 	vmovdqa32 ZMMWORD PTR [rsp+0x2c8],zmm4
  403de7:	c8 02 00 00 
  403deb:	62 f1 7e 48 6f 66 13 	vmovdqu32 zmm4,ZMMWORD PTR [rsi+0x4c0]
  403df2:	62 61 7e 48 6f 66 02 	vmovdqu32 zmm28,ZMMWORD PTR [rsi+0x80]
  403df9:	62 f1 7d 48 7f a4 24 	vmovdqa32 ZMMWORD PTR [rsp+0x288],zmm4
  403e00:	88 02 00 00 
  403e04:	62 f1 7e 48 6f 66 05 	vmovdqu32 zmm4,ZMMWORD PTR [rsi+0x140]
  403e0b:	62 61 7e 48 6f 5e 04 	vmovdqu32 zmm27,ZMMWORD PTR [rsi+0x100]
  403e12:	62 f1 7d 48 7f a4 24 	vmovdqa32 ZMMWORD PTR [rsp+0x248],zmm4
  403e19:	48 02 00 00 
  403e1d:	62 f1 7e 48 6f 66 15 	vmovdqu32 zmm4,ZMMWORD PTR [rsi+0x540]
  403e24:	62 61 7e 48 6f 56 06 	vmovdqu32 zmm26,ZMMWORD PTR [rsi+0x180]
  403e2b:	62 f1 7d 48 7f a4 24 	vmovdqa32 ZMMWORD PTR [rsp+0x208],zmm4
  403e32:	08 02 00 00 
  403e36:	62 f1 7e 48 6f 66 07 	vmovdqu32 zmm4,ZMMWORD PTR [rsi+0x1c0]
  403e3d:	62 61 7e 48 6f 4e 08 	vmovdqu32 zmm25,ZMMWORD PTR [rsi+0x200]
  403e44:	62 f1 7d 48 7f a4 24 	vmovdqa32 ZMMWORD PTR [rsp+0x1c8],zmm4
  403e4b:	c8 01 00 00 
  403e4f:	62 f1 7e 48 6f 66 17 	vmovdqu32 zmm4,ZMMWORD PTR [rsi+0x5c0]
  403e56:	62 61 7e 48 6f 46 0a 	vmovdqu32 zmm24,ZMMWORD PTR [rsi+0x280]
  403e5d:	62 f1 7d 48 7f a4 24 	vmovdqa32 ZMMWORD PTR [rsp+0x188],zmm4
  403e64:	88 01 00 00 
  403e68:	62 f1 7e 48 6f 66 09 	vmovdqu32 zmm4,ZMMWORD PTR [rsi+0x240]
  403e6f:	62 e1 7e 48 6f 7e 0c 	vmovdqu32 zmm23,ZMMWORD PTR [rsi+0x300]
  403e76:	62 f1 7d 48 7f a4 24 	vmovdqa32 ZMMWORD PTR [rsp+0x148],zmm4
  403e7d:	48 01 00 00 
  403e81:	62 f1 7e 48 6f 66 19 	vmovdqu32 zmm4,ZMMWORD PTR [rsi+0x640]
  403e88:	45 31 c0             	xor    r8d,r8d
  403e8b:	62 f1 7d 48 7f a4 24 	vmovdqa32 ZMMWORD PTR [rsp+0x108],zmm4
  403e92:	08 01 00 00 
  403e96:	62 f1 7e 48 6f 66 0b 	vmovdqu32 zmm4,ZMMWORD PTR [rsi+0x2c0]
  403e9d:	62 f1 7d 48 7f a4 24 	vmovdqa32 ZMMWORD PTR [rsp+0xc8],zmm4
  403ea4:	c8 00 00 00 
  403ea8:	62 f1 7e 48 6f 66 1b 	vmovdqu32 zmm4,ZMMWORD PTR [rsi+0x6c0]
  403eaf:	62 f1 7d 48 7f a4 24 	vmovdqa32 ZMMWORD PTR [rsp+0x88],zmm4
  403eb6:	88 00 00 00 
  403eba:	62 f1 7e 48 6f 66 0d 	vmovdqu32 zmm4,ZMMWORD PTR [rsi+0x340]
  403ec1:	62 f1 7d 48 7f a4 24 	vmovdqa32 ZMMWORD PTR [rsp+0x48],zmm4
  403ec8:	48 00 00 00 
  403ecc:	62 f1 7e 48 6f 66 1d 	vmovdqu32 zmm4,ZMMWORD PTR [rsi+0x740]
  403ed3:	c5 79 6f 3d 65 14 00 	vmovdqa xmm15,XMMWORD PTR [rip+0x1465]        # 405340 <__dso_handle+0x338>
  403eda:	00 
  403edb:	62 f1 7d 48 7f a4 24 	vmovdqa32 ZMMWORD PTR [rsp+0x8],zmm4
  403ee2:	08 00 00 00 
  403ee6:	62 f1 7e 48 6f 66 0f 	vmovdqu32 zmm4,ZMMWORD PTR [rsi+0x3c0]
  403eed:	62 e1 7e 48 6f 76 0e 	vmovdqu32 zmm22,ZMMWORD PTR [rsi+0x380]
  403ef4:	62 f1 7d 48 7f a4 24 	vmovdqa32 ZMMWORD PTR [rsp-0x38],zmm4
  403efb:	c8 ff ff ff 
  403eff:	62 f1 7e 48 6f 66 1f 	vmovdqu32 zmm4,ZMMWORD PTR [rsi+0x7c0]
  403f06:	31 f6                	xor    esi,esi
  403f08:	62 f1 7d 48 7f a4 24 	vmovdqa32 ZMMWORD PTR [rsp-0x78],zmm4
  403f0f:	88 ff ff ff 
  403f13:	0f 1f 44 00 00       	nop    DWORD PTR [rax+rax*1+0x0]
  403f18:	62 f1 fe 48 6f 77 01 	vmovdqu64 zmm6,ZMMWORD PTR [rdi+0x40]
  403f1f:	62 71 fe 48 6f 47 03 	vmovdqu64 zmm8,ZMMWORD PTR [rdi+0xc0]
  403f26:	62 f1 15 40 fe e6    	vpaddd zmm4,zmm29,zmm6
  403f2c:	62 f1 4d 48 fe b4 24 	vpaddd zmm6,zmm6,ZMMWORD PTR [rsp+0x308]
  403f33:	08 03 00 00 
  403f37:	62 e1 3d 48 fe 94 24 	vpaddd zmm18,zmm8,ZMMWORD PTR [rsp+0x2c8]
  403f3e:	c8 02 00 00 
  403f42:	62 f1 fe 48 6f 5f 05 	vmovdqu64 zmm3,ZMMWORD PTR [rdi+0x140]
  403f49:	62 f1 fd 48 73 d4 20 	vpsrlq zmm0,zmm4,0x20
  403f50:	62 71 3d 48 fe 84 24 	vpaddd zmm8,zmm8,ZMMWORD PTR [rsp+0x288]
  403f57:	88 02 00 00 
  403f5b:	62 f1 dd 48 f4 e0    	vpmuludq zmm4,zmm4,zmm0
  403f61:	62 f1 fd 48 73 d6 20 	vpsrlq zmm0,zmm6,0x20
  403f68:	62 71 65 48 fe 9c 24 	vpaddd zmm11,zmm3,ZMMWORD PTR [rsp+0x248]
  403f6f:	48 02 00 00 
  403f73:	62 71 fe 48 6f 4f 07 	vmovdqu64 zmm9,ZMMWORD PTR [rdi+0x1c0]
  403f7a:	62 f1 cd 48 f4 f0    	vpmuludq zmm6,zmm6,zmm0
  403f80:	62 b1 fd 48 73 d2 20 	vpsrlq zmm0,zmm18,0x20
  403f87:	62 f1 65 48 fe 9c 24 	vpaddd zmm3,zmm3,ZMMWORD PTR [rsp+0x208]
  403f8e:	08 02 00 00 
  403f92:	62 e1 ed 40 f4 d0    	vpmuludq zmm18,zmm18,zmm0
  403f98:	62 d1 fd 48 73 d0 20 	vpsrlq zmm0,zmm8,0x20
  403f9f:	62 e1 35 48 fe 8c 24 	vpaddd zmm17,zmm9,ZMMWORD PTR [rsp+0x1c8]
  403fa6:	c8 01 00 00 
  403faa:	62 71 bd 48 f4 c0    	vpmuludq zmm8,zmm8,zmm0
  403fb0:	62 d1 fd 48 73 d3 20 	vpsrlq zmm0,zmm11,0x20
  403fb7:	62 71 35 48 fe 8c 24 	vpaddd zmm9,zmm9,ZMMWORD PTR [rsp+0x188]
  403fbe:	88 01 00 00 
  403fc2:	62 71 a5 48 f4 d8    	vpmuludq zmm11,zmm11,zmm0
  403fc8:	62 f1 fd 48 73 d3 20 	vpsrlq zmm0,zmm3,0x20
  403fcf:	62 f1 e5 48 f4 d8    	vpmuludq zmm3,zmm3,zmm0
  403fd5:	62 b1 fd 48 73 d1 20 	vpsrlq zmm0,zmm17,0x20
  403fdc:	62 e1 f5 40 f4 c8    	vpmuludq zmm17,zmm17,zmm0
  403fe2:	62 d1 fd 48 73 d1 20 	vpsrlq zmm0,zmm9,0x20
  403fe9:	62 71 b5 48 f4 c8    	vpmuludq zmm9,zmm9,zmm0
  403fef:	62 f1 fe 48 6f 47 09 	vmovdqu64 zmm0,ZMMWORD PTR [rdi+0x240]
  403ff6:	62 f1 fe 48 6f 7f 0b 	vmovdqu64 zmm7,ZMMWORD PTR [rdi+0x2c0]
  403ffd:	62 71 7d 48 fe 94 24 	vpaddd zmm10,zmm0,ZMMWORD PTR [rsp+0x148]
  404004:	48 01 00 00 
  404008:	62 f1 7d 48 fe 84 24 	vpaddd zmm0,zmm0,ZMMWORD PTR [rsp+0x108]
  40400f:	08 01 00 00 
  404013:	62 e1 45 48 fe 84 24 	vpaddd zmm16,zmm7,ZMMWORD PTR [rsp+0xc8]
  40401a:	c8 00 00 00 
  40401e:	62 d1 f5 48 73 d2 20 	vpsrlq zmm1,zmm10,0x20
  404025:	62 f1 45 48 fe bc 24 	vpaddd zmm7,zmm7,ZMMWORD PTR [rsp+0x88]
  40402c:	88 00 00 00 
  404030:	62 71 ad 48 f4 d1    	vpmuludq zmm10,zmm10,zmm1
  404036:	62 f1 f5 48 73 d0 20 	vpsrlq zmm1,zmm0,0x20
  40403d:	62 f1 fe 48 6f 6f 0d 	vmovdqu64 zmm5,ZMMWORD PTR [rdi+0x340]
  404044:	62 f1 fd 48 f4 c1    	vpmuludq zmm0,zmm0,zmm1
  40404a:	62 b1 f5 48 73 d0 20 	vpsrlq zmm1,zmm16,0x20
  404051:	62 e1 fd 40 f4 c1    	vpmuludq zmm16,zmm16,zmm1
  404057:	62 f1 f5 48 73 d7 20 	vpsrlq zmm1,zmm7,0x20
  40405e:	62 f1 c5 48 f4 f9    	vpmuludq zmm7,zmm7,zmm1
  404064:	62 f1 55 48 fe 8c 24 	vpaddd zmm1,zmm5,ZMMWORD PTR [rsp+0x48]
  40406b:	48 00 00 00 
  40406f:	62 f1 55 48 fe ac 24 	vpaddd zmm5,zmm5,ZMMWORD PTR [rsp+0x8]
  404076:	08 00 00 00 
  40407a:	62 f1 0d 40 ef 17    	vpxord zmm2,zmm30,ZMMWORD PTR [rdi]
  404080:	62 e1 1d 40 ef 6f 02 	vpxord zmm21,zmm28,ZMMWORD PTR [rdi+0x80]
  404087:	62 71 25 40 ef 77 04 	vpxord zmm14,zmm27,ZMMWORD PTR [rdi+0x100]
  40408e:	62 e1 2d 40 ef 67 06 	vpxord zmm20,zmm26,ZMMWORD PTR [rdi+0x180]
  404095:	62 f1 85 40 73 d1 20 	vpsrlq zmm31,zmm1,0x20
  40409c:	62 71 35 40 ef 6f 08 	vpxord zmm13,zmm25,ZMMWORD PTR [rdi+0x200]
  4040a3:	62 e1 3d 40 ef 5f 0a 	vpxord zmm19,zmm24,ZMMWORD PTR [rdi+0x280]
  4040aa:	62 91 f5 48 f4 cf    	vpmuludq zmm1,zmm1,zmm31
  4040b0:	62 f1 85 40 73 d5 20 	vpsrlq zmm31,zmm5,0x20
  4040b7:	62 71 45 40 ef 67 0c 	vpxord zmm12,zmm23,ZMMWORD PTR [rdi+0x300]
  4040be:	62 91 d5 48 f4 ef    	vpmuludq zmm5,zmm5,zmm31
  4040c4:	62 f3 6d 48 44 d2 10 	vpclmullqhqdq zmm2,zmm2,zmm2
  4040cb:	62 61 4d 40 ef 7f 0e 	vpxord zmm31,zmm22,ZMMWORD PTR [rdi+0x380]
  4040d2:	62 a3 55 40 44 ed 10 	vpclmullqhqdq zmm21,zmm21,zmm21
  4040d9:	62 53 0d 48 44 f6 10 	vpclmullqhqdq zmm14,zmm14,zmm14
  4040e0:	62 a3 5d 40 44 e4 10 	vpclmullqhqdq zmm20,zmm20,zmm20
  4040e7:	62 53 15 48 44 ed 10 	vpclmullqhqdq zmm13,zmm13,zmm13
  4040ee:	62 a3 65 40 44 db 10 	vpclmullqhqdq zmm19,zmm19,zmm19
  4040f5:	62 53 1d 48 44 e4 10 	vpclmullqhqdq zmm12,zmm12,zmm12
  4040fc:	62 03 05 40 44 ff 10 	vpclmullqhqdq zmm31,zmm31,zmm31
  404103:	62 b1 6d 48 ef d5    	vpxord zmm2,zmm2,zmm21
  404109:	62 31 0d 48 ef f4    	vpxord zmm14,zmm14,zmm20
  40410f:	62 d1 6d 48 ef d6    	vpxord zmm2,zmm2,zmm14
  404115:	62 31 15 48 ef eb    	vpxord zmm13,zmm13,zmm19
  40411b:	62 d1 6d 48 ef d5    	vpxord zmm2,zmm2,zmm13
  404121:	62 11 1d 48 ef e7    	vpxord zmm12,zmm12,zmm31
  404127:	62 d1 6d 48 ef d4    	vpxord zmm2,zmm2,zmm12
  40412d:	62 71 fe 48 6f 67 0f 	vmovdqu64 zmm12,ZMMWORD PTR [rdi+0x3c0]
  404134:	62 b1 dd 48 d4 e2    	vpaddq zmm4,zmm4,zmm18
  40413a:	62 71 1d 48 fe ac 24 	vpaddd zmm13,zmm12,ZMMWORD PTR [rsp-0x38]
  404141:	c8 ff ff ff 
  404145:	62 31 a5 48 d4 d9    	vpaddq zmm11,zmm11,zmm17
  40414b:	62 d1 8d 48 73 d5 20 	vpsrlq zmm14,zmm13,0x20
  404152:	62 51 95 48 f4 ee    	vpmuludq zmm13,zmm13,zmm14
  404158:	62 71 1d 48 fe a4 24 	vpaddd zmm12,zmm12,ZMMWORD PTR [rsp-0x78]
  40415f:	88 ff ff ff 
  404163:	62 d1 dd 48 d4 e3    	vpaddq zmm4,zmm4,zmm11
  404169:	62 31 ad 48 d4 d0    	vpaddq zmm10,zmm10,zmm16
  40416f:	62 d1 dd 48 d4 e2    	vpaddq zmm4,zmm4,zmm10
  404175:	62 d1 f5 48 d4 cd    	vpaddq zmm1,zmm1,zmm13
  40417b:	62 f1 dd 48 d4 c9    	vpaddq zmm1,zmm4,zmm1
  404181:	62 d1 dd 48 73 d4 20 	vpsrlq zmm4,zmm12,0x20
  404188:	62 71 9d 48 f4 e4    	vpmuludq zmm12,zmm12,zmm4
  40418e:	62 d1 cd 48 d4 f0    	vpaddq zmm6,zmm6,zmm8
  404194:	62 d1 e5 48 d4 d9    	vpaddq zmm3,zmm3,zmm9
  40419a:	62 f1 e5 48 d4 de    	vpaddq zmm3,zmm3,zmm6
  4041a0:	62 f1 fd 48 d4 c7    	vpaddq zmm0,zmm0,zmm7
  4041a6:	62 f1 e5 48 d4 c0    	vpaddq zmm0,zmm3,zmm0
  4041ac:	62 d1 d5 48 d4 ec    	vpaddq zmm5,zmm5,zmm12
  4041b2:	62 f1 fd 48 d4 c5    	vpaddq zmm0,zmm0,zmm5
  4041b8:	62 f3 fd 48 3b cd 01 	vextracti64x4 ymm5,zmm1,0x1
  4041bf:	c5 d5 d4 c9          	vpaddq ymm1,ymm5,ymm1
  4041c3:	62 f3 fd 28 39 cd 01 	vextracti64x2 xmm5,ymm1,0x1
  4041ca:	62 f3 fd 48 3b c6 01 	vextracti64x4 ymm6,zmm0,0x1
  4041d1:	c5 d1 d4 e9          	vpaddq xmm5,xmm5,xmm1
  4041d5:	c5 cd d4 c0          	vpaddq ymm0,ymm6,ymm0
  4041d9:	62 f3 fd 28 39 c6 01 	vextracti64x2 xmm6,ymm0,0x1
  4041e0:	c4 e3 f9 16 e9 01    	vpextrq rcx,xmm5,0x1
  4041e6:	c4 e1 f9 7e e8       	vmovq  rax,xmm5
  4041eb:	c5 c9 d4 c8          	vpaddq xmm1,xmm6,xmm0
  4041ef:	48 01 c8             	add    rax,rcx
  4041f2:	c4 e1 f9 6e c0       	vmovq  xmm0,rax
  4041f7:	c4 e1 f9 7e c9       	vmovq  rcx,xmm1
  4041fc:	c4 e3 f9 16 c8 01    	vpextrq rax,xmm1,0x1
  404202:	c5 f9 6f bc 24 78 03 	vmovdqa xmm7,XMMWORD PTR [rsp+0x378]
  404209:	00 00 
  40420b:	62 f3 fd 48 3b d3 01 	vextracti64x4 ymm3,zmm2,0x1
  404212:	48 01 c8             	add    rax,rcx
  404215:	c5 e5 ef d2          	vpxor  ymm2,ymm3,ymm2
  404219:	c4 e3 f9 22 c0 01    	vpinsrq xmm0,xmm0,rax,0x1
  40421f:	c4 e3 79 44 cf 11    	vpclmulhqhqdq xmm1,xmm0,xmm7
  404225:	62 f3 fd 28 39 d3 01 	vextracti64x2 xmm3,ymm2,0x1
  40422c:	c4 e3 79 44 c7 00    	vpclmullqlqdq xmm0,xmm0,xmm7
  404232:	c5 f9 ef c1          	vpxor  xmm0,xmm0,xmm1
  404236:	c5 e1 ef da          	vpxor  xmm3,xmm3,xmm2
  40423a:	c4 c3 79 44 cf 01    	vpclmulhqlqdq xmm1,xmm0,xmm15
  404240:	c4 c3 61 44 d7 01    	vpclmulhqlqdq xmm2,xmm3,xmm15
  404246:	c4 c3 69 44 e7 01    	vpclmulhqlqdq xmm4,xmm2,xmm15
  40424c:	c4 c3 71 44 ef 01    	vpclmulhqlqdq xmm5,xmm1,xmm15
  404252:	c5 e9 ef d4          	vpxor  xmm2,xmm2,xmm4
  404256:	c5 f1 ef cd          	vpxor  xmm1,xmm1,xmm5
  40425a:	c5 f9 6f b4 24 68 03 	vmovdqa xmm6,XMMWORD PTR [rsp+0x368]
  404261:	00 00 
  404263:	c5 e9 ef d3          	vpxor  xmm2,xmm2,xmm3
  404267:	c5 f1 ef c8          	vpxor  xmm1,xmm1,xmm0
  40426b:	c5 e9 6c c1          	vpunpcklqdq xmm0,xmm2,xmm1
  40426f:	c4 e3 79 44 ce 11    	vpclmulhqhqdq xmm1,xmm0,xmm6
  404275:	c4 e3 79 44 c6 00    	vpclmullqlqdq xmm0,xmm0,xmm6
  40427b:	c5 f9 ef c1          	vpxor  xmm0,xmm0,xmm1
  40427f:	c4 c3 79 44 cf 01    	vpclmulhqlqdq xmm1,xmm0,xmm15
  404285:	c4 c3 71 44 d7 01    	vpclmulhqlqdq xmm2,xmm1,xmm15
  40428b:	c5 f1 ef ca          	vpxor  xmm1,xmm1,xmm2
  40428f:	c5 f1 ef c8          	vpxor  xmm1,xmm1,xmm0
  404293:	c4 e1 f9 7e c8       	vmovq  rax,xmm1
  404298:	48 81 c6 00 04 00 00 	add    rsi,0x400
  40429f:	49 31 c0             	xor    r8,rax
  4042a2:	48 81 c7 00 04 00 00 	add    rdi,0x400
  4042a9:	48 39 f2             	cmp    rdx,rsi
  4042ac:	0f 87 66 fc ff ff    	ja     403f18 <dual_vector_reduced+0x198>
  4042b2:	c5 f8 77             	vzeroupper 
  4042b5:	4c 89 c0             	mov    rax,r8
  4042b8:	c9                   	leave  
  4042b9:	c3                   	ret    
  4042ba:	45 31 c0             	xor    r8d,r8d
  4042bd:	4c 89 c0             	mov    rax,r8
  4042c0:	c9                   	leave  
  4042c1:	c3                   	ret    

Disassembly of section .fini:

00000000004042c4 <_fini>:
  4042c4:	f3 0f 1e fa          	endbr64 
  4042c8:	48 83 ec 08          	sub    rsp,0x8
  4042cc:	48 83 c4 08          	add    rsp,0x8
  4042d0:	c3                   	ret    
