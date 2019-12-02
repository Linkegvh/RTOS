   1              		.syntax unified
   2              		.cpu cortex-m3
   3              		.fpu softvfp
   4              		.eabi_attribute 20, 1
   5              		.eabi_attribute 21, 1
   6              		.eabi_attribute 23, 3
   7              		.eabi_attribute 24, 1
   8              		.eabi_attribute 25, 1
   9              		.eabi_attribute 26, 1
  10              		.eabi_attribute 30, 6
  11              		.eabi_attribute 34, 1
  12              		.eabi_attribute 18, 4
  13              		.thumb
  14              		.file	"main.c"
  15              		.text
  16              	.Ltext0:
  17              		.cfi_sections	.debug_frame
  18              		.section	.rodata
  19              		.align	2
  20              	.LC1:
  21 0000 76617269 		.ascii	"variance: %d \000"
  21      616E6365 
  21      3A202564 
  21      2000
  22 000e 0000     		.align	2
  23              	.LC2:
  24 0010 62756666 		.ascii	"buffer: \000"
  24      65723A20 
  24      00
  25 0019 000000   		.align	2
  26              	.LC3:
  27 001c 25642000 		.ascii	"%d \000"
  28              		.align	2
  29              	.LC4:
  30 0020 0A00     		.ascii	"\012\000"
  31 0022 0000     		.align	2
  32              	.LC0:
  33 0024 02000000 		.word	2
  34 0028 04000000 		.word	4
  35 002c 02000000 		.word	2
  36 0030 04000000 		.word	4
  37 0034 02000000 		.word	2
  38 0038 04000000 		.word	4
  39 003c 0A000000 		.word	10
  40 0040 0A000000 		.word	10
  41 0044 0A000000 		.word	10
  42 0048 0A000000 		.word	10
  43 004c 0A000000 		.word	10
  44 0050 02000000 		.word	2
  45              		.section	.text.main,"ax",%progbits
  46              		.align	2
  47              		.global	main
  48              		.thumb
  49              		.thumb_func
  51              	main:
  52              	.LFB0:
  53              		.file 1 "../src/main.c"
   1:../src/main.c **** #include "stdio.h"
   2:../src/main.c **** 
   3:../src/main.c **** extern int asm_streaming(int x, int mean, int reset, int* buf);
   4:../src/main.c **** 
   5:../src/main.c **** // [T]EE2028 Assignment 1, Sem 1, AY2019/20
   6:../src/main.c **** // (c) CK Tham, ECE NUS
   7:../src/main.c **** 
   8:../src/main.c **** int main(void)
   9:../src/main.c **** {
  54              		.loc 1 9 0
  55              		.cfi_startproc
  56              		@ args = 0, pretend = 0, frame = 80
  57              		@ frame_needed = 1, uses_anonymous_args = 0
  58 0000 B0B5     		push	{r4, r5, r7, lr}
  59              	.LCFI0:
  60              		.cfi_def_cfa_offset 16
  61              		.cfi_offset 14, -4
  62              		.cfi_offset 7, -8
  63              		.cfi_offset 5, -12
  64              		.cfi_offset 4, -16
  65 0002 94B0     		sub	sp, sp, #80
  66              	.LCFI1:
  67              		.cfi_def_cfa_offset 96
  68 0004 00AF     		add	r7, sp, #0
  69              	.LCFI2:
  70              		.cfi_def_cfa_register 7
  10:../src/main.c **** 	int i,j;
  11:../src/main.c **** 
  12:../src/main.c **** 	// Variable definitions
  13:../src/main.c **** 	int x[12]={2,4,2,4,2,4,10,10,10,10,10,2};
  71              		.loc 1 13 0
  72 0006 40F20003 		movw	r3, #:lower16:.LC0
  73 000a C0F20003 		movt	r3, #:upper16:.LC0
  74 000e 07F11804 		add	r4, r7, #24
  75 0012 1D46     		mov	r5, r3
  76 0014 0FCD     		ldmia	r5!, {r0, r1, r2, r3}
  77 0016 0FC4     		stmia	r4!, {r0, r1, r2, r3}
  78 0018 0FCD     		ldmia	r5!, {r0, r1, r2, r3}
  79 001a 0FC4     		stmia	r4!, {r0, r1, r2, r3}
  80 001c 95E80F00 		ldmia	r5, {r0, r1, r2, r3}
  81 0020 84E80F00 		stmia	r4, {r0, r1, r2, r3}
  14:../src/main.c **** 	int buffer[5]={0,0,0,0,0};
  82              		.loc 1 14 0
  83 0024 4FF00003 		mov	r3, #0
  84 0028 7B60     		str	r3, [r7, #4]
  85 002a 4FF00003 		mov	r3, #0
  86 002e BB60     		str	r3, [r7, #8]
  87 0030 4FF00003 		mov	r3, #0
  88 0034 FB60     		str	r3, [r7, #12]
  89 0036 4FF00003 		mov	r3, #0
  90 003a 3B61     		str	r3, [r7, #16]
  91 003c 4FF00003 		mov	r3, #0
  92 0040 7B61     		str	r3, [r7, #20]
  15:../src/main.c **** 
  16:../src/main.c **** 	// Call asm function with mean 3 ***** this is weird, why are we only calling it with mean 3?
  17:../src/main.c **** 	for (i=0;i<12;i++)
  93              		.loc 1 17 0
  94 0042 4FF00003 		mov	r3, #0
  95 0046 FB64     		str	r3, [r7, #76]
  96 0048 67E0     		b	.L2
  97              	.L8:
  18:../src/main.c **** 	{
  19:../src/main.c **** 		if (i ==0 || i == 11)
  98              		.loc 1 19 0
  99 004a FB6C     		ldr	r3, [r7, #76]
 100 004c 002B     		cmp	r3, #0
 101 004e 02D0     		beq	.L3
 102              		.loc 1 19 0 is_stmt 0 discriminator 1
 103 0050 FB6C     		ldr	r3, [r7, #76]
 104 0052 0B2B     		cmp	r3, #11
 105 0054 1AD1     		bne	.L4
 106              	.L3:
  20:../src/main.c **** 			printf("variance: %d ", asm_streaming(x[i],3,1,buffer));
 107              		.loc 1 20 0 is_stmt 1
 108 0056 40F20004 		movw	r4, #:lower16:.LC1
 109 005a C0F20004 		movt	r4, #:upper16:.LC1
 110 005e FB6C     		ldr	r3, [r7, #76]
 111 0060 4FEA8303 		lsl	r3, r3, #2
 112 0064 07F15001 		add	r1, r7, #80
 113 0068 CB18     		adds	r3, r1, r3
 114 006a 53F8382C 		ldr	r2, [r3, #-56]
 115 006e 07F10403 		add	r3, r7, #4
 116 0072 1046     		mov	r0, r2
 117 0074 4FF00301 		mov	r1, #3
 118 0078 4FF00102 		mov	r2, #1
 119 007c FFF7FEFF 		bl	asm_streaming
 120 0080 0346     		mov	r3, r0
 121 0082 2046     		mov	r0, r4
 122 0084 1946     		mov	r1, r3
 123 0086 FFF7FEFF 		bl	printf
 124 008a 19E0     		b	.L5
 125              	.L4:
  21:../src/main.c **** 		else
  22:../src/main.c **** 			printf("variance: %d ", asm_streaming(x[i],3,0,buffer));
 126              		.loc 1 22 0
 127 008c 40F20004 		movw	r4, #:lower16:.LC1
 128 0090 C0F20004 		movt	r4, #:upper16:.LC1
 129 0094 FB6C     		ldr	r3, [r7, #76]
 130 0096 4FEA8303 		lsl	r3, r3, #2
 131 009a 07F15002 		add	r2, r7, #80
 132 009e D318     		adds	r3, r2, r3
 133 00a0 53F8382C 		ldr	r2, [r3, #-56]
 134 00a4 07F10403 		add	r3, r7, #4
 135 00a8 1046     		mov	r0, r2
 136 00aa 4FF00301 		mov	r1, #3
 137 00ae 4FF00002 		mov	r2, #0
 138 00b2 FFF7FEFF 		bl	asm_streaming
 139 00b6 0346     		mov	r3, r0
 140 00b8 2046     		mov	r0, r4
 141 00ba 1946     		mov	r1, r3
 142 00bc FFF7FEFF 		bl	printf
 143              	.L5:
  23:../src/main.c **** 		printf("buffer: ");
 144              		.loc 1 23 0
 145 00c0 40F20003 		movw	r3, #:lower16:.LC2
 146 00c4 C0F20003 		movt	r3, #:upper16:.LC2
 147 00c8 1846     		mov	r0, r3
 148 00ca FFF7FEFF 		bl	printf
  24:../src/main.c **** 		for (j=0;j<5;j++)
 149              		.loc 1 24 0
 150 00ce 4FF00003 		mov	r3, #0
 151 00d2 BB64     		str	r3, [r7, #72]
 152 00d4 13E0     		b	.L6
 153              	.L7:
  25:../src/main.c **** 			printf("%d ", buffer[j]);
 154              		.loc 1 25 0 discriminator 2
 155 00d6 40F20003 		movw	r3, #:lower16:.LC3
 156 00da C0F20003 		movt	r3, #:upper16:.LC3
 157 00de BA6C     		ldr	r2, [r7, #72]
 158 00e0 4FEA8202 		lsl	r2, r2, #2
 159 00e4 07F15001 		add	r1, r7, #80
 160 00e8 8A18     		adds	r2, r1, r2
 161 00ea 52F84C2C 		ldr	r2, [r2, #-76]
 162 00ee 1846     		mov	r0, r3
 163 00f0 1146     		mov	r1, r2
 164 00f2 FFF7FEFF 		bl	printf
  24:../src/main.c **** 		for (j=0;j<5;j++)
 165              		.loc 1 24 0 discriminator 2
 166 00f6 BB6C     		ldr	r3, [r7, #72]
 167 00f8 03F10103 		add	r3, r3, #1
 168 00fc BB64     		str	r3, [r7, #72]
 169              	.L6:
  24:../src/main.c **** 		for (j=0;j<5;j++)
 170              		.loc 1 24 0 is_stmt 0 discriminator 1
 171 00fe BB6C     		ldr	r3, [r7, #72]
 172 0100 042B     		cmp	r3, #4
 173 0102 E8DD     		ble	.L7
  26:../src/main.c **** 		printf("\n");
 174              		.loc 1 26 0 is_stmt 1
 175 0104 40F20003 		movw	r3, #:lower16:.LC4
 176 0108 C0F20003 		movt	r3, #:upper16:.LC4
 177 010c 1846     		mov	r0, r3
 178 010e FFF7FEFF 		bl	printf
  17:../src/main.c **** 	for (i=0;i<12;i++)
 179              		.loc 1 17 0
 180 0112 FB6C     		ldr	r3, [r7, #76]
 181 0114 03F10103 		add	r3, r3, #1
 182 0118 FB64     		str	r3, [r7, #76]
 183              	.L2:
  17:../src/main.c **** 	for (i=0;i<12;i++)
 184              		.loc 1 17 0 is_stmt 0 discriminator 1
 185 011a FB6C     		ldr	r3, [r7, #76]
 186 011c 0B2B     		cmp	r3, #11
 187 011e 94DD     		ble	.L8
 188              	.L9:
  27:../src/main.c **** 	}
  28:../src/main.c **** 
  29:../src/main.c **** // Enter an infinite loop, just incrementing a counter
  30:../src/main.c **** 	volatile static int loop = 0;
  31:../src/main.c **** 	while (1) {
  32:../src/main.c **** 		loop++;
 189              		.loc 1 32 0 is_stmt 1 discriminator 1
 190 0120 40F20003 		movw	r3, #:lower16:loop.3824
 191 0124 C0F20003 		movt	r3, #:upper16:loop.3824
 192 0128 1B68     		ldr	r3, [r3, #0]
 193 012a 03F10102 		add	r2, r3, #1
 194 012e 40F20003 		movw	r3, #:lower16:loop.3824
 195 0132 C0F20003 		movt	r3, #:upper16:loop.3824
 196 0136 1A60     		str	r2, [r3, #0]
  33:../src/main.c **** 	}
 197              		.loc 1 33 0 discriminator 1
 198 0138 F2E7     		b	.L9
 199              		.cfi_endproc
 200              	.LFE0:
 202 013a 00BF     		.bss
 203              		.align	2
 204              	loop.3824:
 205 0000 00000000 		.space	4
 206              		.text
 207              	.Letext0:
DEFINED SYMBOLS
                            *ABS*:00000000 main.c
C:\Users\Lingke\AppData\Local\Temp\cc6JAslm.s:19     .rodata:00000000 $d
C:\Users\Lingke\AppData\Local\Temp\cc6JAslm.s:20     .rodata:00000000 .LC1
C:\Users\Lingke\AppData\Local\Temp\cc6JAslm.s:23     .rodata:00000010 .LC2
C:\Users\Lingke\AppData\Local\Temp\cc6JAslm.s:26     .rodata:0000001c .LC3
C:\Users\Lingke\AppData\Local\Temp\cc6JAslm.s:29     .rodata:00000020 .LC4
C:\Users\Lingke\AppData\Local\Temp\cc6JAslm.s:32     .rodata:00000024 .LC0
C:\Users\Lingke\AppData\Local\Temp\cc6JAslm.s:46     .text.main:00000000 $t
C:\Users\Lingke\AppData\Local\Temp\cc6JAslm.s:51     .text.main:00000000 main
C:\Users\Lingke\AppData\Local\Temp\cc6JAslm.s:204    .bss:00000000 loop.3824
C:\Users\Lingke\AppData\Local\Temp\cc6JAslm.s:203    .bss:00000000 $d
                     .debug_frame:00000010 $d

UNDEFINED SYMBOLS
asm_streaming
printf
