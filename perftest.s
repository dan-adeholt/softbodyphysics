
./build-release/CMakeFiles/PerfTest.dir/game/Game.cpp.o:	file format mach-o arm64

Disassembly of section __TEXT,__text:

0000000000000000 <ltmp0>:
       0: a9be4ff4     	stp	x20, x19, [sp, #-32]!
       4: a9017bfd     	stp	x29, x30, [sp, #16]
       8: 910043fd     	add	x29, sp, #16
       c: aa0003f3     	mov	x19, x0
      10: 5296b800     	mov	w0, #46528
      14: 94000000     	bl	0x14 <ltmp0+0x14>
      18: aa0003f4     	mov	x20, x0
      1c: 94000000     	bl	0x1c <ltmp0+0x1c>
      20: f9000274     	str	x20, [x19]
      24: aa1303e0     	mov	x0, x19
      28: a9417bfd     	ldp	x29, x30, [sp, #16]
      2c: a8c24ff4     	ldp	x20, x19, [sp], #32
      30: d65f03c0     	ret
      34: aa0003f3     	mov	x19, x0
      38: aa1403e0     	mov	x0, x20
      3c: 94000000     	bl	0x3c <ltmp0+0x3c>
      40: aa1303e0     	mov	x0, x19
      44: 94000000     	bl	0x44 <ltmp0+0x44>

0000000000000048 <__ZN4GameC1Ev>:
      48: a9be4ff4     	stp	x20, x19, [sp, #-32]!
      4c: a9017bfd     	stp	x29, x30, [sp, #16]
      50: 910043fd     	add	x29, sp, #16
      54: aa0003f3     	mov	x19, x0
      58: 5296b800     	mov	w0, #46528
      5c: 94000000     	bl	0x5c <__ZN4GameC1Ev+0x14>
      60: aa0003f4     	mov	x20, x0
      64: 94000000     	bl	0x64 <__ZN4GameC1Ev+0x1c>
      68: f9000274     	str	x20, [x19]
      6c: aa1303e0     	mov	x0, x19
      70: a9417bfd     	ldp	x29, x30, [sp, #16]
      74: a8c24ff4     	ldp	x20, x19, [sp], #32
      78: d65f03c0     	ret
      7c: aa0003f3     	mov	x19, x0
      80: aa1403e0     	mov	x0, x20
      84: 94000000     	bl	0x84 <__ZN4GameC1Ev+0x3c>
      88: aa1303e0     	mov	x0, x19
      8c: 94000000     	bl	0x8c <__ZN4GameC1Ev+0x44>

0000000000000090 <__ZN4Game4initEPKc>:
      90: a9bc5ff8     	stp	x24, x23, [sp, #-64]!
      94: a90157f6     	stp	x22, x21, [sp, #16]
      98: a9024ff4     	stp	x20, x19, [sp, #32]
      9c: a9037bfd     	stp	x29, x30, [sp, #48]
      a0: 9100c3fd     	add	x29, sp, #48
      a4: 90000008     	adrp	x8, 0x0 <__ZN4Game4initEPKc+0x14>
      a8: f9400108     	ldr	x8, [x8]
      ac: b9400115     	ldr	w21, [x8]
      b0: 710006bf     	cmp	w21, #1
      b4: 5400020b     	b.lt	0xf4 <__ZN4Game4initEPKc+0x64>
      b8: aa0103f4     	mov	x20, x1
      bc: aa0003f3     	mov	x19, x0
      c0: d2800016     	mov	x22, #0
      c4: 90000008     	adrp	x8, 0x0 <__ZN4Game4initEPKc+0x34>
      c8: f9400108     	ldr	x8, [x8]
      cc: f9400117     	ldr	x23, [x8]
      d0: aa1703f8     	mov	x24, x23
      d4: f9400300     	ldr	x0, [x24]
      d8: aa1403e1     	mov	x1, x20
      dc: 94000000     	bl	0xdc <__ZN4Game4initEPKc+0x4c>
      e0: 34000140     	cbz	w0, 0x108 <__ZN4Game4initEPKc+0x78>
      e4: 910006d6     	add	x22, x22, #1
      e8: 91004318     	add	x24, x24, #16
      ec: eb1602bf     	cmp	x21, x22
      f0: 54ffff21     	b.ne	0xd4 <__ZN4Game4initEPKc+0x44>
      f4: a9437bfd     	ldp	x29, x30, [sp, #48]
      f8: a9424ff4     	ldp	x20, x19, [sp, #32]
      fc: a94157f6     	ldp	x22, x21, [sp, #16]
     100: a8c45ff8     	ldp	x24, x23, [sp], #64
     104: d65f03c0     	ret
     108: 8b3652e8     	add	x8, x23, w22, uxtw #4
     10c: f9400501     	ldr	x1, [x8, #8]
     110: aa1303e0     	mov	x0, x19
     114: a9437bfd     	ldp	x29, x30, [sp, #48]
     118: a9424ff4     	ldp	x20, x19, [sp, #32]
     11c: a94157f6     	ldp	x22, x21, [sp, #16]
     120: a8c45ff8     	ldp	x24, x23, [sp], #64
     124: d61f0020     	br	x1

0000000000000128 <__ZN4GameD2Ev>:
     128: a9be4ff4     	stp	x20, x19, [sp, #-32]!
     12c: a9017bfd     	stp	x29, x30, [sp, #16]
     130: 910043fd     	add	x29, sp, #16
     134: aa0003f3     	mov	x19, x0
     138: f9400000     	ldr	x0, [x0]
     13c: b4000060     	cbz	x0, 0x148 <__ZN4GameD2Ev+0x20>
     140: 94000000     	bl	0x140 <__ZN4GameD2Ev+0x18>
     144: 94000000     	bl	0x144 <__ZN4GameD2Ev+0x1c>
     148: aa1303e0     	mov	x0, x19
     14c: a9417bfd     	ldp	x29, x30, [sp, #16]
     150: a8c24ff4     	ldp	x20, x19, [sp], #32
     154: d65f03c0     	ret

0000000000000158 <__ZN4GameD1Ev>:
     158: a9be4ff4     	stp	x20, x19, [sp, #-32]!
     15c: a9017bfd     	stp	x29, x30, [sp, #16]
     160: 910043fd     	add	x29, sp, #16
     164: aa0003f3     	mov	x19, x0
     168: f9400000     	ldr	x0, [x0]
     16c: b4000060     	cbz	x0, 0x178 <__ZN4GameD1Ev+0x20>
     170: 94000000     	bl	0x170 <__ZN4GameD1Ev+0x18>
     174: 94000000     	bl	0x174 <__ZN4GameD1Ev+0x1c>
     178: aa1303e0     	mov	x0, x19
     17c: a9417bfd     	ldp	x29, x30, [sp, #16]
     180: a8c24ff4     	ldp	x20, x19, [sp], #32
     184: d65f03c0     	ret

0000000000000188 <__Z9springJobPv>:
     188: aa0003e1     	mov	x1, x0
     18c: 91004000     	add	x0, x0, #16
     190: a9450c22     	ldp	x2, x3, [x1, #80]
     194: 14000000     	b	0x194 <__Z9springJobPv+0xc>

0000000000000198 <__ZNK4Game16getDynamicShapesER5RangeI5ShapeE>:
     198: f9400008     	ldr	x8, [x0]
     19c: 52968109     	mov	w9, #46088
     1a0: 8b090108     	add	x8, x8, x9
     1a4: b9400109     	ldr	w9, [x8]
     1a8: f9400508     	ldr	x8, [x8, #8]
     1ac: a9002428     	stp	x8, x9, [x1]
     1b0: d65f03c0     	ret

00000000000001b4 <__ZNK4Game15getStaticShapesER5RangeI5ShapeE>:
     1b4: f9400008     	ldr	x8, [x0]
     1b8: 52968b09     	mov	w9, #46168
     1bc: 8b090108     	add	x8, x8, x9
     1c0: b9400109     	ldr	w9, [x8]
     1c4: f9400508     	ldr	x8, [x8, #8]
     1c8: a9002428     	stp	x8, x9, [x1]
     1cc: d65f03c0     	ret

00000000000001d0 <__ZN4Game10getSpringsER5RangeI6SpringE>:
     1d0: f9400008     	ldr	x8, [x0]
     1d4: 52969509     	mov	w9, #46248
     1d8: 8b090108     	add	x8, x8, x9
     1dc: b9400109     	ldr	w9, [x8]
     1e0: f9400508     	ldr	x8, [x8, #8]
     1e4: a9002428     	stp	x8, x9, [x1]
     1e8: d65f03c0     	ret

00000000000001ec <__ZNK4Game16getDynamicPointsER16PointMassesRange>:
     1ec: f9400008     	ldr	x8, [x0]
     1f0: 52968309     	mov	w9, #46104
     1f4: 8b090108     	add	x8, x8, x9
     1f8: b9400109     	ldr	w9, [x8]
     1fc: f940050a     	ldr	x10, [x8, #8]
     200: f9400d0b     	ldr	x11, [x8, #24]
     204: f940150c     	ldr	x12, [x8, #40]
     208: f9401d08     	ldr	x8, [x8, #56]
     20c: a900242a     	stp	x10, x9, [x1]
     210: a901242b     	stp	x11, x9, [x1, #16]
     214: a902242c     	stp	x12, x9, [x1, #32]
     218: a9032428     	stp	x8, x9, [x1, #48]
     21c: d65f03c0     	ret

0000000000000220 <__ZNK4Game15getStaticPointsER16PointMassesRange>:
     220: f9400008     	ldr	x8, [x0]
     224: 52968d09     	mov	w9, #46184
     228: 8b090108     	add	x8, x8, x9
     22c: b9400109     	ldr	w9, [x8]
     230: f940050a     	ldr	x10, [x8, #8]
     234: f9400d0b     	ldr	x11, [x8, #24]
     238: f940150c     	ldr	x12, [x8, #40]
     23c: f9401d08     	ldr	x8, [x8, #56]
     240: a900242a     	stp	x10, x9, [x1]
     244: a901242b     	stp	x11, x9, [x1, #16]
     248: a902242c     	stp	x12, x9, [x1, #32]
     24c: a9032428     	stp	x8, x9, [x1, #48]
     250: d65f03c0     	ret

0000000000000254 <__Z17sortBoundingBoxesR5ArrayI16ShapeBoundingBoxE>:
     254: d10043ff     	sub	sp, sp, #16
     258: b9400008     	ldr	w8, [x0]
     25c: 7100091f     	cmp	w8, #2
     260: 5400060b     	b.lt	0x320 <__Z17sortBoundingBoxesR5ArrayI16ShapeBoundingBoxE+0xcc>
     264: 52800028     	mov	w8, #1
     268: 52800289     	mov	w9, #20
     26c: 5280028a     	mov	w10, #20
     270: 1400000d     	b	0x2a4 <__Z17sortBoundingBoxesR5ArrayI16ShapeBoundingBoxE+0x50>
     274: 9b29398c     	smaddl	x12, w12, w9, x14
     278: b900018b     	str	w11, [x12]
     27c: bd000580     	str	s0, [x12, #4]
     280: f94003eb     	ldr	x11, [sp]
     284: f900058b     	str	x11, [x12, #8]
     288: b9400beb     	ldr	w11, [sp, #8]
     28c: b900118b     	str	w11, [x12, #16]
     290: 91000508     	add	x8, x8, #1
     294: b980000b     	ldrsw	x11, [x0]
     298: 9100514a     	add	x10, x10, #20
     29c: eb0b011f     	cmp	x8, x11
     2a0: 5400040a     	b.ge	0x320 <__Z17sortBoundingBoxesR5ArrayI16ShapeBoundingBoxE+0xcc>
     2a4: f940040b     	ldr	x11, [x0, #8]
     2a8: 9b092d0c     	madd	x12, x8, x9, x11
     2ac: b940018b     	ldr	w11, [x12]
     2b0: bd400580     	ldr	s0, [x12, #4]
     2b4: f940058d     	ldr	x13, [x12, #8]
     2b8: f90003ed     	str	x13, [sp]
     2bc: b940118c     	ldr	w12, [x12, #16]
     2c0: b9000bec     	str	w12, [sp, #8]
     2c4: aa0a03ed     	mov	x13, x10
     2c8: aa0803ec     	mov	x12, x8
     2cc: d100058f     	sub	x15, x12, #1
     2d0: f940040e     	ldr	x14, [x0, #8]
     2d4: 92407df0     	and	x16, x15, #0xffffffff
     2d8: 9b093a11     	madd	x17, x16, x9, x14
     2dc: bd400621     	ldr	s1, [x17, #4]
     2e0: 1e202020     	fcmp	s1, s0
     2e4: 54fffc8d     	b.le	0x274 <__Z17sortBoundingBoxesR5ArrayI16ShapeBoundingBoxE+0x20>
     2e8: 9b093a0c     	madd	x12, x16, x9, x14
     2ec: 8b0d01ce     	add	x14, x14, x13
     2f0: 3dc00181     	ldr	q1, [x12]
     2f4: b940118c     	ldr	w12, [x12, #16]
     2f8: b90011cc     	str	w12, [x14, #16]
     2fc: 3d8001c1     	str	q1, [x14]
     300: 910005ee     	add	x14, x15, #1
     304: d10051ad     	sub	x13, x13, #20
     308: aa0f03ec     	mov	x12, x15
     30c: f10005df     	cmp	x14, #1
     310: 54fffdec     	b.gt	0x2cc <__Z17sortBoundingBoxesR5ArrayI16ShapeBoundingBoxE+0x78>
     314: d280000c     	mov	x12, #0
     318: f940040e     	ldr	x14, [x0, #8]
     31c: 17ffffd6     	b	0x274 <__Z17sortBoundingBoxesR5ArrayI16ShapeBoundingBoxE+0x20>
     320: 910043ff     	add	sp, sp, #16
     324: d65f03c0     	ret

0000000000000328 <__Z25updateSortedBoundingBoxesR5ArrayI16ShapeBoundingBoxES2_>:
     328: b9400008     	ldr	w8, [x0]
     32c: 7100051f     	cmp	w8, #1
     330: 5400018b     	b.lt	0x360 <__Z25updateSortedBoundingBoxesR5ArrayI16ShapeBoundingBoxES2_+0x38>
     334: f940040a     	ldr	x10, [x0, #8]
     338: f9400429     	ldr	x9, [x1, #8]
     33c: 91001129     	add	x9, x9, #4
     340: 9100114a     	add	x10, x10, #4
     344: 5280028b     	mov	w11, #20
     348: b89fc14c     	ldursw	x12, [x10, #-4]
     34c: 9b2b7d8c     	smull	x12, w12, w11
     350: 3cec6920     	ldr	q0, [x9, x12]
     354: 3c814540     	str	q0, [x10], #20
     358: f1000508     	subs	x8, x8, #1
     35c: 54ffff61     	b.ne	0x348 <__Z25updateSortedBoundingBoxesR5ArrayI16ShapeBoundingBoxES2_+0x20>
     360: d65f03c0     	ret

0000000000000364 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo>:
     364: 6db923e9     	stp	d9, d8, [sp, #-112]!
     368: a9016ffc     	stp	x28, x27, [sp, #16]
     36c: a90267fa     	stp	x26, x25, [sp, #32]
     370: a9035ff8     	stp	x24, x23, [sp, #48]
     374: a90457f6     	stp	x22, x21, [sp, #64]
     378: a9054ff4     	stp	x20, x19, [sp, #80]
     37c: a9067bfd     	stp	x29, x30, [sp, #96]
     380: 910183fd     	add	x29, sp, #96
     384: d107c3ff     	sub	sp, sp, #496
     388: f90013e3     	str	x3, [sp, #32]
     38c: 1e604008     	fmov	d8, d0
     390: aa0003f3     	mov	x19, x0
     394: f90023e0     	str	x0, [sp, #64]
     398: f9400008     	ldr	x8, [x0]
     39c: 52969b09     	mov	w9, #46296
     3a0: 8b090115     	add	x21, x8, x9
     3a4: 52969d09     	mov	w9, #46312
     3a8: 8b090116     	add	x22, x8, x9
     3ac: d10223a0     	sub	x0, x29, #136
     3b0: 94000000     	bl	0x3b0 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x4c>
     3b4: f9400268     	ldr	x8, [x19]
     3b8: 52968109     	mov	w9, #46088
     3bc: 8b090101     	add	x1, x8, x9
     3c0: 52968309     	mov	w9, #46104
     3c4: 8b090102     	add	x2, x8, x9
     3c8: aa1503e0     	mov	x0, x21
     3cc: 94000000     	bl	0x3cc <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x68>
     3d0: f9400268     	ldr	x8, [x19]
     3d4: 52968b09     	mov	w9, #46168
     3d8: 8b090101     	add	x1, x8, x9
     3dc: 52968d09     	mov	w9, #46184
     3e0: 8b090102     	add	x2, x8, x9
     3e4: aa1603e0     	mov	x0, x22
     3e8: 94000000     	bl	0x3e8 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x84>
     3ec: f9400268     	ldr	x8, [x19]
     3f0: 52969f09     	mov	w9, #46328
     3f4: b8696909     	ldr	w9, [x8, x9]
     3f8: 34000209     	cbz	w9, 0x438 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0xd4>
     3fc: b94022a9     	ldr	w9, [x21, #32]
     400: 7100053f     	cmp	w9, #1
     404: 5400088b     	b.lt	0x514 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x1b0>
     408: f94016ab     	ldr	x11, [x21, #40]
     40c: f94006aa     	ldr	x10, [x21, #8]
     410: 9100114a     	add	x10, x10, #4
     414: 9100116b     	add	x11, x11, #4
     418: 5280028c     	mov	w12, #20
     41c: b89fc16d     	ldursw	x13, [x11, #-4]
     420: 9b2c7dad     	smull	x13, w13, w12
     424: 3ced6940     	ldr	q0, [x10, x13]
     428: 3c814560     	str	q0, [x11], #20
     42c: f1000529     	subs	x9, x9, #1
     430: 54ffff61     	b.ne	0x41c <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0xb8>
     434: 14000038     	b	0x514 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x1b0>
     438: b94002a9     	ldr	w9, [x21]
     43c: 7100053f     	cmp	w9, #1
     440: 540006ab     	b.lt	0x514 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x1b0>
     444: d2800014     	mov	x20, #0
     448: 52800298     	mov	w24, #20
     44c: 52800059     	mov	w25, #2
     450: 14000013     	b	0x49c <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x138>
     454: b90022bf     	str	wzr, [x21, #32]
     458: b4000040     	cbz	x0, 0x460 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0xfc>
     45c: 94000000     	bl	0x45c <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0xf8>
     460: f90016b7     	str	x23, [x21, #40]
     464: b90026bc     	str	w28, [x21, #36]
     468: 9b186e88     	madd	x8, x20, x24, x27
     46c: f94016a9     	ldr	x9, [x21, #40]
     470: 1100074a     	add	w10, w26, #1
     474: b90022aa     	str	w10, [x21, #32]
     478: 9b382749     	smaddl	x9, w26, w24, x9
     47c: 3dc00100     	ldr	q0, [x8]
     480: b9401108     	ldr	w8, [x8, #16]
     484: b9001128     	str	w8, [x9, #16]
     488: 3d800120     	str	q0, [x9]
     48c: 91000694     	add	x20, x20, #1
     490: b98002a8     	ldrsw	x8, [x21]
     494: eb08029f     	cmp	x20, x8
     498: 540003aa     	b.ge	0x50c <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x1a8>
     49c: f94006bb     	ldr	x27, [x21, #8]
     4a0: 294422b3     	ldp	w19, w8, [x21, #32]
     4a4: 93407e7a     	sxtw	x26, w19
     4a8: 6b08027f     	cmp	w19, w8
     4ac: 54fffde1     	b.ne	0x468 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x104>
     4b0: 531f7a68     	lsl	w8, w19, #1
     4b4: 7100075f     	cmp	w26, #1
     4b8: 7a48a348     	ccmp	w26, w8, #8, ge
     4bc: 54fffd6a     	b.ge	0x468 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x104>
     4c0: 7100091f     	cmp	w8, #2
     4c4: 1a99c11c     	csel	w28, w8, w25, gt
     4c8: 9bb87f80     	umull	x0, w28, w24
     4cc: 94000000     	bl	0x4cc <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x168>
     4d0: aa0003f7     	mov	x23, x0
     4d4: f94016a0     	ldr	x0, [x21, #40]
     4d8: aa1703e8     	mov	x8, x23
     4dc: aa0003e9     	mov	x9, x0
     4e0: 7100067f     	cmp	w19, #1
     4e4: 54fffb8b     	b.lt	0x454 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0xf0>
     4e8: 3dc00120     	ldr	q0, [x9]
     4ec: b940112a     	ldr	w10, [x9, #16]
     4f0: b900110a     	str	w10, [x8, #16]
     4f4: 3c814500     	str	q0, [x8], #20
     4f8: 91005129     	add	x9, x9, #20
     4fc: f1000673     	subs	x19, x19, #1
     500: 54ffff41     	b.ne	0x4e8 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x184>
     504: b90022bf     	str	wzr, [x21, #32]
     508: 17ffffd5     	b	0x45c <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0xf8>
     50c: f94023e8     	ldr	x8, [sp, #64]
     510: f9400108     	ldr	x8, [x8]
     514: 5296a109     	mov	w9, #46344
     518: b8696908     	ldr	w8, [x8, x9]
     51c: 34000208     	cbz	w8, 0x55c <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x1f8>
     520: b94032a8     	ldr	w8, [x21, #48]
     524: 7100051f     	cmp	w8, #1
     528: 5400084b     	b.lt	0x630 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x2cc>
     52c: f9401eaa     	ldr	x10, [x21, #56]
     530: f9400ea9     	ldr	x9, [x21, #24]
     534: 91001129     	add	x9, x9, #4
     538: 9100114a     	add	x10, x10, #4
     53c: 5280028b     	mov	w11, #20
     540: b89fc14c     	ldursw	x12, [x10, #-4]
     544: 9b2b7d8c     	smull	x12, w12, w11
     548: 3cec6920     	ldr	q0, [x9, x12]
     54c: 3c814540     	str	q0, [x10], #20
     550: f1000508     	subs	x8, x8, #1
     554: 54ffff61     	b.ne	0x540 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x1dc>
     558: 14000036     	b	0x630 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x2cc>
     55c: b94002c8     	ldr	w8, [x22]
     560: 7100051f     	cmp	w8, #1
     564: 5400066b     	b.lt	0x630 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x2cc>
     568: d2800014     	mov	x20, #0
     56c: 52800297     	mov	w23, #20
     570: 52800058     	mov	w24, #2
     574: 14000013     	b	0x5c0 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x25c>
     578: b90032bf     	str	wzr, [x21, #48]
     57c: b4000040     	cbz	x0, 0x584 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x220>
     580: 94000000     	bl	0x580 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x21c>
     584: f9001eb6     	str	x22, [x21, #56]
     588: b90036bb     	str	w27, [x21, #52]
     58c: 9b176a88     	madd	x8, x20, x23, x26
     590: f9401ea9     	ldr	x9, [x21, #56]
     594: 1100072a     	add	w10, w25, #1
     598: b90032aa     	str	w10, [x21, #48]
     59c: 9b372729     	smaddl	x9, w25, w23, x9
     5a0: 3dc00100     	ldr	q0, [x8]
     5a4: b9401108     	ldr	w8, [x8, #16]
     5a8: b9001128     	str	w8, [x9, #16]
     5ac: 3d800120     	str	q0, [x9]
     5b0: 91000694     	add	x20, x20, #1
     5b4: b98012a8     	ldrsw	x8, [x21, #16]
     5b8: eb08029f     	cmp	x20, x8
     5bc: 540003aa     	b.ge	0x630 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x2cc>
     5c0: f9400eba     	ldr	x26, [x21, #24]
     5c4: 294622b3     	ldp	w19, w8, [x21, #48]
     5c8: 93407e79     	sxtw	x25, w19
     5cc: 6b08027f     	cmp	w19, w8
     5d0: 54fffde1     	b.ne	0x58c <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x228>
     5d4: 531f7a68     	lsl	w8, w19, #1
     5d8: 7100073f     	cmp	w25, #1
     5dc: 7a48a328     	ccmp	w25, w8, #8, ge
     5e0: 54fffd6a     	b.ge	0x58c <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x228>
     5e4: 7100091f     	cmp	w8, #2
     5e8: 1a98c11b     	csel	w27, w8, w24, gt
     5ec: 9bb77f60     	umull	x0, w27, w23
     5f0: 94000000     	bl	0x5f0 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x28c>
     5f4: aa0003f6     	mov	x22, x0
     5f8: f9401ea0     	ldr	x0, [x21, #56]
     5fc: aa1603e8     	mov	x8, x22
     600: aa0003e9     	mov	x9, x0
     604: 7100067f     	cmp	w19, #1
     608: 54fffb8b     	b.lt	0x578 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x214>
     60c: 3dc00120     	ldr	q0, [x9]
     610: b940112a     	ldr	w10, [x9, #16]
     614: b900110a     	str	w10, [x8, #16]
     618: 3c814500     	str	q0, [x8], #20
     61c: 91005129     	add	x9, x9, #20
     620: f1000673     	subs	x19, x19, #1
     624: 54ffff41     	b.ne	0x60c <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x2a8>
     628: b90032bf     	str	wzr, [x21, #48]
     62c: 17ffffd5     	b	0x580 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x21c>
     630: b94022a8     	ldr	w8, [x21, #32]
     634: 7100051f     	cmp	w8, #1
     638: f94013f3     	ldr	x19, [sp, #32]
     63c: f94023f9     	ldr	x25, [sp, #64]
     640: 5400060d     	b.le	0x700 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x39c>
     644: 52800028     	mov	w8, #1
     648: 52800289     	mov	w9, #20
     64c: 5280028a     	mov	w10, #20
     650: 1400000d     	b	0x684 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x320>
     654: 9b29398c     	smaddl	x12, w12, w9, x14
     658: b900018b     	str	w11, [x12]
     65c: bd000580     	str	s0, [x12, #4]
     660: f85883ab     	ldur	x11, [x29, #-120]
     664: f900058b     	str	x11, [x12, #8]
     668: b85903ab     	ldur	w11, [x29, #-112]
     66c: b900118b     	str	w11, [x12, #16]
     670: 91000508     	add	x8, x8, #1
     674: b98022ab     	ldrsw	x11, [x21, #32]
     678: 9100514a     	add	x10, x10, #20
     67c: eb0b011f     	cmp	x8, x11
     680: 5400040a     	b.ge	0x700 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x39c>
     684: f94016ab     	ldr	x11, [x21, #40]
     688: 9b092d0c     	madd	x12, x8, x9, x11
     68c: b940018b     	ldr	w11, [x12]
     690: bd400580     	ldr	s0, [x12, #4]
     694: f940058d     	ldr	x13, [x12, #8]
     698: f81883ad     	stur	x13, [x29, #-120]
     69c: b940118c     	ldr	w12, [x12, #16]
     6a0: b81903ac     	stur	w12, [x29, #-112]
     6a4: aa0a03ed     	mov	x13, x10
     6a8: aa0803ec     	mov	x12, x8
     6ac: d100058f     	sub	x15, x12, #1
     6b0: f94016ae     	ldr	x14, [x21, #40]
     6b4: 92407df0     	and	x16, x15, #0xffffffff
     6b8: 9b093a11     	madd	x17, x16, x9, x14
     6bc: bd400621     	ldr	s1, [x17, #4]
     6c0: 1e202020     	fcmp	s1, s0
     6c4: 54fffc8d     	b.le	0x654 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x2f0>
     6c8: 9b093a0c     	madd	x12, x16, x9, x14
     6cc: 8b0d01ce     	add	x14, x14, x13
     6d0: 3dc00181     	ldr	q1, [x12]
     6d4: b940118c     	ldr	w12, [x12, #16]
     6d8: b90011cc     	str	w12, [x14, #16]
     6dc: 3d8001c1     	str	q1, [x14]
     6e0: 910005ee     	add	x14, x15, #1
     6e4: d10051ad     	sub	x13, x13, #20
     6e8: aa0f03ec     	mov	x12, x15
     6ec: f10005df     	cmp	x14, #1
     6f0: 54fffdec     	b.gt	0x6ac <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x348>
     6f4: d280000c     	mov	x12, #0
     6f8: f94016ae     	ldr	x14, [x21, #40]
     6fc: 17ffffd6     	b	0x654 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x2f0>
     700: b94032a8     	ldr	w8, [x21, #48]
     704: 7100051f     	cmp	w8, #1
     708: 5400060d     	b.le	0x7c8 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x464>
     70c: 52800028     	mov	w8, #1
     710: 52800289     	mov	w9, #20
     714: 5280028a     	mov	w10, #20
     718: 1400000d     	b	0x74c <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x3e8>
     71c: 9b29398c     	smaddl	x12, w12, w9, x14
     720: b900018b     	str	w11, [x12]
     724: bd000580     	str	s0, [x12, #4]
     728: f85883ab     	ldur	x11, [x29, #-120]
     72c: f900058b     	str	x11, [x12, #8]
     730: b85903ab     	ldur	w11, [x29, #-112]
     734: b900118b     	str	w11, [x12, #16]
     738: 91000508     	add	x8, x8, #1
     73c: b98032ab     	ldrsw	x11, [x21, #48]
     740: 9100514a     	add	x10, x10, #20
     744: eb0b011f     	cmp	x8, x11
     748: 5400040a     	b.ge	0x7c8 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x464>
     74c: f9401eab     	ldr	x11, [x21, #56]
     750: 9b092d0c     	madd	x12, x8, x9, x11
     754: b940018b     	ldr	w11, [x12]
     758: bd400580     	ldr	s0, [x12, #4]
     75c: f940058d     	ldr	x13, [x12, #8]
     760: f81883ad     	stur	x13, [x29, #-120]
     764: b940118c     	ldr	w12, [x12, #16]
     768: b81903ac     	stur	w12, [x29, #-112]
     76c: aa0a03ed     	mov	x13, x10
     770: aa0803ec     	mov	x12, x8
     774: d100058f     	sub	x15, x12, #1
     778: f9401eae     	ldr	x14, [x21, #56]
     77c: 92407df0     	and	x16, x15, #0xffffffff
     780: 9b093a11     	madd	x17, x16, x9, x14
     784: bd400621     	ldr	s1, [x17, #4]
     788: 1e202020     	fcmp	s1, s0
     78c: 54fffc8d     	b.le	0x71c <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x3b8>
     790: 9b093a0c     	madd	x12, x16, x9, x14
     794: 8b0d01ce     	add	x14, x14, x13
     798: 3dc00181     	ldr	q1, [x12]
     79c: b940118c     	ldr	w12, [x12, #16]
     7a0: b90011cc     	str	w12, [x14, #16]
     7a4: 3d8001c1     	str	q1, [x14]
     7a8: 910005ee     	add	x14, x15, #1
     7ac: d10051ad     	sub	x13, x13, #20
     7b0: aa0f03ec     	mov	x12, x15
     7b4: f10005df     	cmp	x14, #1
     7b8: 54fffdec     	b.gt	0x774 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x410>
     7bc: d280000c     	mov	x12, #0
     7c0: f9401eae     	ldr	x14, [x21, #56]
     7c4: 17ffffd6     	b	0x71c <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x3b8>
     7c8: d10223a0     	sub	x0, x29, #136
     7cc: 94000000     	bl	0x7cc <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x468>
     7d0: fd002260     	str	d0, [x19, #64]
     7d4: b94022a8     	ldr	w8, [x21, #32]
     7d8: 29097e68     	stp	w8, wzr, [x19, #72]
     7dc: 7100051f     	cmp	w8, #1
     7e0: 5400224b     	b.lt	0xc28 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x8c4>
     7e4: f9001bff     	str	xzr, [sp, #48]
     7e8: d280000f     	mov	x15, #0
     7ec: 5280000e     	mov	w14, #0
     7f0: 52800029     	mov	w9, #1
     7f4: b9002fe9     	str	w9, [sp, #44]
     7f8: 5280028d     	mov	w13, #20
     7fc: 5280031a     	mov	w26, #24
     800: 5296841c     	mov	w28, #46112
     804: 1400000e     	b	0x83c <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x4d8>
     808: b94022a8     	ldr	w8, [x21, #32]
     80c: 5280028d     	mov	w13, #20
     810: f9401fee     	ldr	x14, [sp, #56]
     814: f94007ea     	ldr	x10, [sp, #8]
     818: f9401be9     	ldr	x9, [sp, #48]
     81c: 91005129     	add	x9, x9, #20
     820: f9001be9     	str	x9, [sp, #48]
     824: b9402fe9     	ldr	w9, [sp, #44]
     828: 11000529     	add	w9, w9, #1
     82c: b9002fe9     	str	w9, [sp, #44]
     830: aa0a03ef     	mov	x15, x10
     834: eb28c15f     	cmp	x10, w8, sxtw
     838: 54001f8a     	b.ge	0xc28 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x8c4>
     83c: f9001fee     	str	x14, [sp, #56]
     840: f94016ae     	ldr	x14, [x21, #40]
     844: 9b0d39f6     	madd	x22, x15, x13, x14
     848: f9400329     	ldr	x9, [x25]
     84c: 5296820a     	mov	w10, #46096
     850: 8b0a0129     	add	x9, x9, x10
     854: b98002ca     	ldrsw	x10, [x22]
     858: f940012b     	ldr	x11, [x9]
     85c: 9b3a2d5b     	smaddl	x27, w10, w26, x11
     860: f940036a     	ldr	x10, [x27]
     864: d360fd4b     	lsr	x11, x10, #32
     868: f9406129     	ldr	x9, [x9, #192]
     86c: d3607d4c     	lsl	x12, x10, #32
     870: 8b8c7929     	add	x9, x9, x12, asr #30
     874: 4b0a016a     	sub	w10, w11, w10
     878: a938aba9     	stp	x9, x10, [x29, #-120]
     87c: 910005e9     	add	x9, x15, #1
     880: a900bbe9     	stp	x9, x14, [sp, #8]
     884: eb28c13f     	cmp	x9, w8, sxtw
     888: f9000fef     	str	x15, [sp, #24]
     88c: 5400100a     	b.ge	0xa8c <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x728>
     890: 52800289     	mov	w9, #20
     894: 9b0939f4     	madd	x20, x15, x9, x14
     898: b9402ff9     	ldr	w25, [sp, #44]
     89c: f9401bf8     	ldr	x24, [sp, #48]
     8a0: 14000005     	b	0x8b4 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x550>
     8a4: 91005318     	add	x24, x24, #20
     8a8: 11000739     	add	w25, w25, #1
     8ac: 6b19011f     	cmp	w8, w25
     8b0: 54000eed     	b.le	0xa8c <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x728>
     8b4: f94016aa     	ldr	x10, [x21, #40]
     8b8: 8b180149     	add	x9, x10, x24
     8bc: b9404e6b     	ldr	w11, [x19, #76]
     8c0: 1100056b     	add	w11, w11, #1
     8c4: b9004e6b     	str	w11, [x19, #76]
     8c8: bd401920     	ldr	s0, [x9, #24]
     8cc: bd400e81     	ldr	s1, [x20, #12]
     8d0: 1e212000     	fcmp	s0, s1
     8d4: 54000dc5     	b.pl	0xa8c <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x728>
     8d8: bd401d20     	ldr	s0, [x9, #28]
     8dc: bd401281     	ldr	s1, [x20, #16]
     8e0: 1e212000     	fcmp	s0, s1
     8e4: 54fffe08     	b.hi	0x8a4 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x540>
     8e8: 8b18014a     	add	x10, x10, x24
     8ec: bd402540     	ldr	s0, [x10, #36]
     8f0: bd400a81     	ldr	s1, [x20, #8]
     8f4: 1e212000     	fcmp	s0, s1
     8f8: 54fffd6b     	b.lt	0x8a4 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x540>
     8fc: 91005137     	add	x23, x9, #20
     900: 5280030a     	mov	w10, #24
     904: f94023fa     	ldr	x26, [sp, #64]
     908: f9400348     	ldr	x8, [x26]
     90c: 52968209     	mov	w9, #46096
     910: 8b090108     	add	x8, x8, x9
     914: b98002e9     	ldrsw	x9, [x23]
     918: f940011c     	ldr	x28, [x8]
     91c: 9b2a7d33     	smull	x19, w9, w10
     920: f8736b89     	ldr	x9, [x28, x19]
     924: d360fd2a     	lsr	x10, x9, #32
     928: f940610b     	ldr	x11, [x8, #192]
     92c: d3607d2c     	lsl	x12, x9, #32
     930: 8b8c796b     	add	x11, x11, x12, asr #30
     934: 4b090149     	sub	w9, w10, w9
     938: a937a7ab     	stp	x11, x9, [x29, #-136]
     93c: f9400369     	ldr	x9, [x27]
     940: d360fd2a     	lsr	x10, x9, #32
     944: f940090b     	ldr	x11, [x8, #16]
     948: d3607d2c     	lsl	x12, x9, #32
     94c: 937d7d2d     	sbfiz	x13, x9, #3, #32
     950: 8b0d016e     	add	x14, x11, x13
     954: 4b090149     	sub	w9, w10, w9
     958: a933a7ae     	stp	x14, x9, [x29, #-200]
     95c: f940110a     	ldr	x10, [x8, #32]
     960: 8b8c794c     	add	x12, x10, x12, asr #30
     964: a934a7ac     	stp	x12, x9, [x29, #-184]
     968: f940190c     	ldr	x12, [x8, #48]
     96c: 8b0d018e     	add	x14, x12, x13
     970: a935a7ae     	stp	x14, x9, [x29, #-168]
     974: f9402108     	ldr	x8, [x8, #64]
     978: 8b0d010d     	add	x13, x8, x13
     97c: a936a7ad     	stp	x13, x9, [x29, #-152]
     980: f8736b89     	ldr	x9, [x28, x19]
     984: d360fd2d     	lsr	x13, x9, #32
     988: d3607d2e     	lsl	x14, x9, #32
     98c: 937d7d2f     	sbfiz	x15, x9, #3, #32
     990: 8b0f016b     	add	x11, x11, x15
     994: 4b0901a9     	sub	w9, w13, w9
     998: a914a7eb     	stp	x11, x9, [sp, #328]
     99c: 8b8e794a     	add	x10, x10, x14, asr #30
     9a0: a915a7ea     	stp	x10, x9, [sp, #344]
     9a4: 8b0f018a     	add	x10, x12, x15
     9a8: a916a7ea     	stp	x10, x9, [sp, #360]
     9ac: 8b0f0108     	add	x8, x8, x15
     9b0: a917a7e8     	stp	x8, x9, [sp, #376]
     9b4: d10323a0     	sub	x0, x29, #200
     9b8: 910523e1     	add	x1, sp, #328
     9bc: d101e3a4     	sub	x4, x29, #120
     9c0: d10223a5     	sub	x5, x29, #136
     9c4: aa1603e2     	mov	x2, x22
     9c8: aa1703e3     	mov	x3, x23
     9cc: 1e604100     	fmov	d0, d8
     9d0: 94000000     	bl	0x9d0 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x66c>
     9d4: f9400348     	ldr	x8, [x26]
     9d8: 5280031a     	mov	w26, #24
     9dc: 52968409     	mov	w9, #46112
     9e0: 8b090108     	add	x8, x8, x9
     9e4: f8736b89     	ldr	x9, [x28, x19]
     9e8: 5296841c     	mov	w28, #46112
     9ec: f94013f3     	ldr	x19, [sp, #32]
     9f0: d360fd2a     	lsr	x10, x9, #32
     9f4: f940010b     	ldr	x11, [x8]
     9f8: d3607d2c     	lsl	x12, x9, #32
     9fc: 937d7d2d     	sbfiz	x13, x9, #3, #32
     a00: 8b0d016e     	add	x14, x11, x13
     a04: 4b090149     	sub	w9, w10, w9
     a08: a910a7ee     	stp	x14, x9, [sp, #264]
     a0c: f940090a     	ldr	x10, [x8, #16]
     a10: 8b8c794c     	add	x12, x10, x12, asr #30
     a14: a911a7ec     	stp	x12, x9, [sp, #280]
     a18: f940110c     	ldr	x12, [x8, #32]
     a1c: 8b0d018e     	add	x14, x12, x13
     a20: a912a7ee     	stp	x14, x9, [sp, #296]
     a24: f9401908     	ldr	x8, [x8, #48]
     a28: 8b0d010d     	add	x13, x8, x13
     a2c: a913a7ed     	stp	x13, x9, [sp, #312]
     a30: f9400369     	ldr	x9, [x27]
     a34: d360fd2d     	lsr	x13, x9, #32
     a38: d3607d2e     	lsl	x14, x9, #32
     a3c: 937d7d2f     	sbfiz	x15, x9, #3, #32
     a40: 8b0f016b     	add	x11, x11, x15
     a44: 4b0901a9     	sub	w9, w13, w9
     a48: a90ca7eb     	stp	x11, x9, [sp, #200]
     a4c: 8b8e794a     	add	x10, x10, x14, asr #30
     a50: a90da7ea     	stp	x10, x9, [sp, #216]
     a54: 8b0f018a     	add	x10, x12, x15
     a58: a90ea7ea     	stp	x10, x9, [sp, #232]
     a5c: 8b0f0108     	add	x8, x8, x15
     a60: a90fa7e8     	stp	x8, x9, [sp, #248]
     a64: 910423e0     	add	x0, sp, #264
     a68: 910323e1     	add	x1, sp, #200
     a6c: d10223a4     	sub	x4, x29, #136
     a70: d101e3a5     	sub	x5, x29, #120
     a74: aa1703e2     	mov	x2, x23
     a78: aa1603e3     	mov	x3, x22
     a7c: 1e604100     	fmov	d0, d8
     a80: 94000000     	bl	0xa80 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x71c>
     a84: b94022a8     	ldr	w8, [x21, #32]
     a88: 17ffff87     	b	0x8a4 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x540>
     a8c: b94032a9     	ldr	w9, [x21, #48]
     a90: f9401fee     	ldr	x14, [sp, #56]
     a94: 6b0901df     	cmp	w14, w9
     a98: 5400028a     	b.ge	0xae8 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x784>
     a9c: 93407d2a     	sxtw	x10, w9
     aa0: f9401eac     	ldr	x12, [x21, #56]
     aa4: 5280028d     	mov	w13, #20
     aa8: a9413ff0     	ldp	x16, x15, [sp, #16]
     aac: 9b0d41eb     	madd	x11, x15, x13, x16
     ab0: bd400560     	ldr	s0, [x11, #4]
     ab4: 93407dcb     	sxtw	x11, w14
     ab8: 9b2d31cc     	smaddl	x12, w14, w13, x12
     abc: 9100318c     	add	x12, x12, #12
     ac0: f94023f9     	ldr	x25, [sp, #64]
     ac4: bd400181     	ldr	s1, [x12]
     ac8: 1e202020     	fcmp	s1, s0
     acc: 540001a5     	b.pl	0xb00 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x79c>
     ad0: 9100056b     	add	x11, x11, #1
     ad4: 9100518c     	add	x12, x12, #20
     ad8: eb0b015f     	cmp	x10, x11
     adc: 54ffff41     	b.ne	0xac4 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x760>
     ae0: aa0903ee     	mov	x14, x9
     ae4: 17ffff4c     	b	0x814 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x4b0>
     ae8: f94023f9     	ldr	x25, [sp, #64]
     aec: 5280028d     	mov	w13, #20
     af0: a9413ff0     	ldp	x16, x15, [sp, #16]
     af4: 6b0901df     	cmp	w14, w9
     af8: 54ffe8ea     	b.ge	0x814 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x4b0>
     afc: 14000004     	b	0xb0c <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x7a8>
     b00: aa0b03ee     	mov	x14, x11
     b04: 6b0901df     	cmp	w14, w9
     b08: 54ffe86a     	b.ge	0x814 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x4b0>
     b0c: 52800288     	mov	w8, #20
     b10: 9b0841f4     	madd	x20, x15, x8, x16
     b14: aa0e03e8     	mov	x8, x14
     b18: 93407dd7     	sxtw	x23, w14
     b1c: f9001fee     	str	x14, [sp, #56]
     b20: 8b2ecae8     	add	x8, x23, w14, sxtw #2
     b24: d37ef518     	lsl	x24, x8, #2
     b28: f9401ea8     	ldr	x8, [x21, #56]
     b2c: 8b180102     	add	x2, x8, x24
     b30: bd400440     	ldr	s0, [x2, #4]
     b34: bd400e81     	ldr	s1, [x20, #12]
     b38: 1e212000     	fcmp	s0, s1
     b3c: 54ffe665     	b.pl	0x808 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x4a4>
     b40: f9400328     	ldr	x8, [x25]
     b44: 8b1c0108     	add	x8, x8, x28
     b48: b9800049     	ldrsw	x9, [x2]
     b4c: f940210a     	ldr	x10, [x8, #64]
     b50: 9b3a7d29     	smull	x9, w9, w26
     b54: f940036b     	ldr	x11, [x27]
     b58: d360fd6c     	lsr	x12, x11, #32
     b5c: f940590d     	ldr	x13, [x8, #176]
     b60: d3607d6e     	lsl	x14, x11, #32
     b64: 8b8e79ad     	add	x13, x13, x14, asr #30
     b68: 4b0b018b     	sub	w11, w12, w11
     b6c: a937afad     	stp	x13, x11, [x29, #-136]
     b70: f8696949     	ldr	x9, [x10, x9]
     b74: d360fd2a     	lsr	x10, x9, #32
     b78: f940290b     	ldr	x11, [x8, #80]
     b7c: d3607d2c     	lsl	x12, x9, #32
     b80: 937d7d2d     	sbfiz	x13, x9, #3, #32
     b84: 8b0d016b     	add	x11, x11, x13
     b88: 4b090149     	sub	w9, w10, w9
     b8c: a908a7eb     	stp	x11, x9, [sp, #136]
     b90: f940310a     	ldr	x10, [x8, #96]
     b94: 8b8c794a     	add	x10, x10, x12, asr #30
     b98: a909a7ea     	stp	x10, x9, [sp, #152]
     b9c: f940390a     	ldr	x10, [x8, #112]
     ba0: 8b0d014a     	add	x10, x10, x13
     ba4: a90aa7ea     	stp	x10, x9, [sp, #168]
     ba8: f940410a     	ldr	x10, [x8, #128]
     bac: 8b0d014a     	add	x10, x10, x13
     bb0: a90ba7ea     	stp	x10, x9, [sp, #184]
     bb4: f9400369     	ldr	x9, [x27]
     bb8: d360fd2a     	lsr	x10, x9, #32
     bbc: f940010b     	ldr	x11, [x8]
     bc0: d3607d2c     	lsl	x12, x9, #32
     bc4: 937d7d2d     	sbfiz	x13, x9, #3, #32
     bc8: 8b0d016b     	add	x11, x11, x13
     bcc: 4b090149     	sub	w9, w10, w9
     bd0: a904a7eb     	stp	x11, x9, [sp, #72]
     bd4: f940090a     	ldr	x10, [x8, #16]
     bd8: 8b8c794a     	add	x10, x10, x12, asr #30
     bdc: a905a7ea     	stp	x10, x9, [sp, #88]
     be0: f940110a     	ldr	x10, [x8, #32]
     be4: 8b0d014a     	add	x10, x10, x13
     be8: a906a7ea     	stp	x10, x9, [sp, #104]
     bec: f9401908     	ldr	x8, [x8, #48]
     bf0: 8b0d0108     	add	x8, x8, x13
     bf4: a907a7e8     	stp	x8, x9, [sp, #120]
     bf8: 910223e0     	add	x0, sp, #136
     bfc: 910123e1     	add	x1, sp, #72
     c00: d10223a4     	sub	x4, x29, #136
     c04: aa1603e3     	mov	x3, x22
     c08: 1e604100     	fmov	d0, d8
     c0c: 94000000     	bl	0xc0c <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x8a8>
     c10: 910006f7     	add	x23, x23, #1
     c14: b98032a8     	ldrsw	x8, [x21, #48]
     c18: 91005318     	add	x24, x24, #20
     c1c: eb0802ff     	cmp	x23, x8
     c20: 54fff84b     	b.lt	0xb28 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x7c4>
     c24: 17fffef9     	b	0x808 <__ZN4Game16handleCollisionsER16PointMassesRangeRK5RangeIiEdR18ConsoleProfileInfo+0x4a4>
     c28: 9107c3ff     	add	sp, sp, #496
     c2c: a9467bfd     	ldp	x29, x30, [sp, #96]
     c30: a9454ff4     	ldp	x20, x19, [sp, #80]
     c34: a94457f6     	ldp	x22, x21, [sp, #64]
     c38: a9435ff8     	ldp	x24, x23, [sp, #48]
     c3c: a94267fa     	ldp	x26, x25, [sp, #32]
     c40: a9416ffc     	ldp	x28, x27, [sp, #16]
     c44: 6cc723e9     	ldp	d9, d8, [sp], #112
     c48: d65f03c0     	ret

0000000000000c4c <__ZN4Game12loadFromFileEPKc>:
     c4c: d10383ff     	sub	sp, sp, #224
     c50: a9086ffc     	stp	x28, x27, [sp, #128]
     c54: a90967fa     	stp	x26, x25, [sp, #144]
     c58: a90a5ff8     	stp	x24, x23, [sp, #160]
     c5c: a90b57f6     	stp	x22, x21, [sp, #176]
     c60: a90c4ff4     	stp	x20, x19, [sp, #192]
     c64: a90d7bfd     	stp	x29, x30, [sp, #208]
     c68: 910343fd     	add	x29, sp, #208
     c6c: aa0103e8     	mov	x8, x1
     c70: aa0003f3     	mov	x19, x0
     c74: 90000001     	adrp	x1, 0x0 <__ZN4Game12loadFromFileEPKc+0x28>
     c78: 91000021     	add	x1, x1, #0
     c7c: aa0803e0     	mov	x0, x8
     c80: 94000000     	bl	0xc80 <__ZN4Game12loadFromFileEPKc+0x34>
     c84: b4003ac0     	cbz	x0, 0x13dc <__ZN4Game12loadFromFileEPKc+0x790>
     c88: aa0003f6     	mov	x22, x0
     c8c: d10173a8     	sub	x8, x29, #92
     c90: f9000be8     	str	x8, [sp, #16]
     c94: d10163a8     	sub	x8, x29, #88
     c98: d10153a9     	sub	x9, x29, #84
     c9c: a90023e9     	stp	x9, x8, [sp]
     ca0: 90000001     	adrp	x1, 0x0 <__ZN4Game12loadFromFileEPKc+0x54>
     ca4: 91000021     	add	x1, x1, #0
     ca8: 94000000     	bl	0xca8 <__ZN4Game12loadFromFileEPKc+0x5c>
     cac: f9400277     	ldr	x23, [x19]
     cb0: 52968108     	mov	w8, #46088
     cb4: 8b0802f4     	add	x20, x23, x8
     cb8: b900029f     	str	wzr, [x20]
     cbc: b85a83a8     	ldur	w8, [x29, #-88]
     cc0: b9400689     	ldr	w9, [x20, #4]
     cc4: 7100053f     	cmp	w9, #1
     cc8: 7a48a128     	ccmp	w9, w8, #8, ge
     ccc: a903dbf3     	stp	x19, x22, [sp, #56]
     cd0: 5400040a     	b.ge	0xd50 <__ZN4Game12loadFromFileEPKc+0x104>
     cd4: 52800049     	mov	w9, #2
     cd8: 7100091f     	cmp	w8, #2
     cdc: 1a89c116     	csel	w22, w8, w9, gt
     ce0: 52800308     	mov	w8, #24
     ce4: 9ba87ec0     	umull	x0, w22, w8
     ce8: 94000000     	bl	0xce8 <__ZN4Game12loadFromFileEPKc+0x9c>
     cec: aa0003f5     	mov	x21, x0
     cf0: b9400293     	ldr	w19, [x20]
     cf4: f9400680     	ldr	x0, [x20, #8]
     cf8: 7100067f     	cmp	w19, #1
     cfc: 540001ab     	b.lt	0xd30 <__ZN4Game12loadFromFileEPKc+0xe4>
     d00: aa1503e8     	mov	x8, x21
     d04: aa0003e9     	mov	x9, x0
     d08: aa1303ea     	mov	x10, x19
     d0c: 3dc00120     	ldr	q0, [x9]
     d10: f940092b     	ldr	x11, [x9, #16]
     d14: f900090b     	str	x11, [x8, #16]
     d18: 3c818500     	str	q0, [x8], #24
     d1c: 91006129     	add	x9, x9, #24
     d20: f100054a     	subs	x10, x10, #1
     d24: 54ffff41     	b.ne	0xd0c <__ZN4Game12loadFromFileEPKc+0xc0>
     d28: b900029f     	str	wzr, [x20]
     d2c: 14000003     	b	0xd38 <__ZN4Game12loadFromFileEPKc+0xec>
     d30: b900029f     	str	wzr, [x20]
     d34: b4000080     	cbz	x0, 0xd44 <__ZN4Game12loadFromFileEPKc+0xf8>
     d38: 94000000     	bl	0xd38 <__ZN4Game12loadFromFileEPKc+0xec>
     d3c: f9401fe8     	ldr	x8, [sp, #56]
     d40: f9400117     	ldr	x23, [x8]
     d44: f9000695     	str	x21, [x20, #8]
     d48: 29005a93     	stp	w19, w22, [x20]
     d4c: a943dbf3     	ldp	x19, x22, [sp, #56]
     d50: 52968308     	mov	w8, #46104
     d54: 8b0802e0     	add	x0, x23, x8
     d58: b900001f     	str	wzr, [x0]
     d5c: b900101f     	str	wzr, [x0, #16]
     d60: b900201f     	str	wzr, [x0, #32]
     d64: b900301f     	str	wzr, [x0, #48]
     d68: b85ac3a1     	ldur	w1, [x29, #-84]
     d6c: 94000000     	bl	0xd6c <__ZN4Game12loadFromFileEPKc+0x120>
     d70: b85ac3a8     	ldur	w8, [x29, #-84]
     d74: 340020e8     	cbz	w8, 0x1190 <__ZN4Game12loadFromFileEPKc+0x544>
     d78: d2800017     	mov	x23, #0
     d7c: 90000015     	adrp	x21, 0x0 <__ZN4Game12loadFromFileEPKc+0x130>
     d80: 910002b5     	add	x21, x21, #0
     d84: 1400000f     	b	0xdc0 <__ZN4Game12loadFromFileEPKc+0x174>
     d88: b900037f     	str	wzr, [x27]
     d8c: 94000000     	bl	0xd8c <__ZN4Game12loadFromFileEPKc+0x140>
     d90: f9000776     	str	x22, [x27, #8]
     d94: b9000774     	str	w20, [x27, #4]
     d98: f94023f6     	ldr	x22, [sp, #64]
     d9c: f9400768     	ldr	x8, [x27, #8]
     da0: 11000709     	add	w9, w24, #1
     da4: b9000369     	str	w9, [x27]
     da8: 8b190d08     	add	x8, x8, x25, lsl #3
     dac: f900011f     	str	xzr, [x8]
     db0: 910006f7     	add	x23, x23, #1
     db4: b89ac3a8     	ldursw	x8, [x29, #-84]
     db8: eb0802ff     	cmp	x23, x8
     dbc: 54001ea2     	b.hs	0x1190 <__ZN4Game12loadFromFileEPKc+0x544>
     dc0: 9101a3e8     	add	x8, sp, #104
     dc4: f90013e8     	str	x8, [sp, #32]
     dc8: d10193a8     	sub	x8, x29, #100
     dcc: f9000fe8     	str	x8, [sp, #24]
     dd0: d10183a8     	sub	x8, x29, #96
     dd4: f9000be8     	str	x8, [sp, #16]
     dd8: 910123e8     	add	x8, sp, #72
     ddc: f90007e8     	str	x8, [sp, #8]
     de0: 910193e8     	add	x8, sp, #100
     de4: f90003e8     	str	x8, [sp]
     de8: aa1603e0     	mov	x0, x22
     dec: aa1503e1     	mov	x1, x21
     df0: 94000000     	bl	0xdf0 <__ZN4Game12loadFromFileEPKc+0x1a4>
     df4: f9400268     	ldr	x8, [x19]
     df8: 52968309     	mov	w9, #46104
     dfc: 8b090118     	add	x24, x8, x9
     e00: b9404bf9     	ldr	w25, [sp, #72]
     e04: b85a03ba     	ldur	w26, [x29, #-96]
     e08: 2940231b     	ldp	w27, w8, [x24]
     e0c: 93407f7c     	sxtw	x28, w27
     e10: 6b08037f     	cmp	w27, w8
     e14: 54000601     	b.ne	0xed4 <__ZN4Game12loadFromFileEPKc+0x288>
     e18: 531f7b88     	lsl	w8, w28, #1
     e1c: 7100079f     	cmp	w28, #1
     e20: 7a48a388     	ccmp	w28, w8, #8, ge
     e24: 5400058a     	b.ge	0xed4 <__ZN4Game12loadFromFileEPKc+0x288>
     e28: 7100091f     	cmp	w8, #2
     e2c: 52800049     	mov	w9, #2
     e30: 1a89c114     	csel	w20, w8, w9, gt
     e34: d37d7e80     	ubfiz	x0, x20, #3, #32
     e38: 94000000     	bl	0xe38 <__ZN4Game12loadFromFileEPKc+0x1ec>
     e3c: aa0003f6     	mov	x22, x0
     e40: f9400700     	ldr	x0, [x24, #8]
     e44: 7100079f     	cmp	w28, #1
     e48: 540003ab     	b.lt	0xebc <__ZN4Game12loadFromFileEPKc+0x270>
     e4c: d2800008     	mov	x8, #0
     e50: 7100237f     	cmp	w27, #8
     e54: 54000203     	b.lo	0xe94 <__ZN4Game12loadFromFileEPKc+0x248>
     e58: cb0002c9     	sub	x9, x22, x0
     e5c: f101013f     	cmp	x9, #64
     e60: 540001a3     	b.lo	0xe94 <__ZN4Game12loadFromFileEPKc+0x248>
     e64: 927d7368     	and	x8, x27, #0xfffffff8
     e68: 91008009     	add	x9, x0, #32
     e6c: 910082ca     	add	x10, x22, #32
     e70: aa0803eb     	mov	x11, x8
     e74: ad7f0520     	ldp	q0, q1, [x9, #-32]
     e78: acc20d22     	ldp	q2, q3, [x9], #64
     e7c: ad3f0540     	stp	q0, q1, [x10, #-32]
     e80: ac820d42     	stp	q2, q3, [x10], #64
     e84: f100216b     	subs	x11, x11, #8
     e88: 54ffff61     	b.ne	0xe74 <__ZN4Game12loadFromFileEPKc+0x228>
     e8c: eb1b011f     	cmp	x8, x27
     e90: 54000120     	b.eq	0xeb4 <__ZN4Game12loadFromFileEPKc+0x268>
     e94: cb080369     	sub	x9, x27, x8
     e98: d37df10a     	lsl	x10, x8, #3
     e9c: 8b0a0008     	add	x8, x0, x10
     ea0: 8b0a02ca     	add	x10, x22, x10
     ea4: f840850b     	ldr	x11, [x8], #8
     ea8: f800854b     	str	x11, [x10], #8
     eac: f1000529     	subs	x9, x9, #1
     eb0: 54ffffa1     	b.ne	0xea4 <__ZN4Game12loadFromFileEPKc+0x258>
     eb4: b900031f     	str	wzr, [x24]
     eb8: 14000003     	b	0xec4 <__ZN4Game12loadFromFileEPKc+0x278>
     ebc: b900031f     	str	wzr, [x24]
     ec0: b4000040     	cbz	x0, 0xec8 <__ZN4Game12loadFromFileEPKc+0x27c>
     ec4: 94000000     	bl	0xec4 <__ZN4Game12loadFromFileEPKc+0x278>
     ec8: f9000716     	str	x22, [x24, #8]
     ecc: b9000714     	str	w20, [x24, #4]
     ed0: f94023f6     	ldr	x22, [sp, #64]
     ed4: f9400708     	ldr	x8, [x24, #8]
     ed8: 11000769     	add	w9, w27, #1
     edc: b9000309     	str	w9, [x24]
     ee0: 8b1c0d08     	add	x8, x8, x28, lsl #3
     ee4: 29006919     	stp	w25, w26, [x8]
     ee8: f9400268     	ldr	x8, [x19]
     eec: 52968709     	mov	w9, #46136
     ef0: 8b090118     	add	x24, x8, x9
     ef4: b859c3b9     	ldur	w25, [x29, #-100]
     ef8: b9406bfa     	ldr	w26, [sp, #104]
     efc: 2940231b     	ldp	w27, w8, [x24]
     f00: 93407f7c     	sxtw	x28, w27
     f04: 6b08037f     	cmp	w27, w8
     f08: 54000601     	b.ne	0xfc8 <__ZN4Game12loadFromFileEPKc+0x37c>
     f0c: 531f7b88     	lsl	w8, w28, #1
     f10: 7100079f     	cmp	w28, #1
     f14: 7a48a388     	ccmp	w28, w8, #8, ge
     f18: 5400058a     	b.ge	0xfc8 <__ZN4Game12loadFromFileEPKc+0x37c>
     f1c: 7100091f     	cmp	w8, #2
     f20: 52800049     	mov	w9, #2
     f24: 1a89c114     	csel	w20, w8, w9, gt
     f28: d37d7e80     	ubfiz	x0, x20, #3, #32
     f2c: 94000000     	bl	0xf2c <__ZN4Game12loadFromFileEPKc+0x2e0>
     f30: aa0003f6     	mov	x22, x0
     f34: f9400700     	ldr	x0, [x24, #8]
     f38: 7100079f     	cmp	w28, #1
     f3c: 540003ab     	b.lt	0xfb0 <__ZN4Game12loadFromFileEPKc+0x364>
     f40: d2800008     	mov	x8, #0
     f44: 7100237f     	cmp	w27, #8
     f48: 54000203     	b.lo	0xf88 <__ZN4Game12loadFromFileEPKc+0x33c>
     f4c: cb0002c9     	sub	x9, x22, x0
     f50: f101013f     	cmp	x9, #64
     f54: 540001a3     	b.lo	0xf88 <__ZN4Game12loadFromFileEPKc+0x33c>
     f58: 927d7368     	and	x8, x27, #0xfffffff8
     f5c: 91008009     	add	x9, x0, #32
     f60: 910082ca     	add	x10, x22, #32
     f64: aa0803eb     	mov	x11, x8
     f68: ad7f0520     	ldp	q0, q1, [x9, #-32]
     f6c: acc20d22     	ldp	q2, q3, [x9], #64
     f70: ad3f0540     	stp	q0, q1, [x10, #-32]
     f74: ac820d42     	stp	q2, q3, [x10], #64
     f78: f100216b     	subs	x11, x11, #8
     f7c: 54ffff61     	b.ne	0xf68 <__ZN4Game12loadFromFileEPKc+0x31c>
     f80: eb1b011f     	cmp	x8, x27
     f84: 54000120     	b.eq	0xfa8 <__ZN4Game12loadFromFileEPKc+0x35c>
     f88: cb080369     	sub	x9, x27, x8
     f8c: d37df10a     	lsl	x10, x8, #3
     f90: 8b0a0008     	add	x8, x0, x10
     f94: 8b0a02ca     	add	x10, x22, x10
     f98: f840850b     	ldr	x11, [x8], #8
     f9c: f800854b     	str	x11, [x10], #8
     fa0: f1000529     	subs	x9, x9, #1
     fa4: 54ffffa1     	b.ne	0xf98 <__ZN4Game12loadFromFileEPKc+0x34c>
     fa8: b900031f     	str	wzr, [x24]
     fac: 14000003     	b	0xfb8 <__ZN4Game12loadFromFileEPKc+0x36c>
     fb0: b900031f     	str	wzr, [x24]
     fb4: b4000040     	cbz	x0, 0xfbc <__ZN4Game12loadFromFileEPKc+0x370>
     fb8: 94000000     	bl	0xfb8 <__ZN4Game12loadFromFileEPKc+0x36c>
     fbc: f9000716     	str	x22, [x24, #8]
     fc0: b9000714     	str	w20, [x24, #4]
     fc4: f94023f6     	ldr	x22, [sp, #64]
     fc8: f9400708     	ldr	x8, [x24, #8]
     fcc: 11000769     	add	w9, w27, #1
     fd0: b9000309     	str	w9, [x24]
     fd4: 8b1c0d08     	add	x8, x8, x28, lsl #3
     fd8: 29006919     	stp	w25, w26, [x8]
     fdc: f940027b     	ldr	x27, [x19]
     fe0: 52968508     	mov	w8, #46120
     fe4: 8b080378     	add	x24, x27, x8
     fe8: 29402319     	ldp	w25, w8, [x24]
     fec: 93407f3a     	sxtw	x26, w25
     ff0: 6b08033f     	cmp	w25, w8
     ff4: 54000621     	b.ne	0x10b8 <__ZN4Game12loadFromFileEPKc+0x46c>
     ff8: 531f7b48     	lsl	w8, w26, #1
     ffc: 7100075f     	cmp	w26, #1
    1000: 7a48a348     	ccmp	w26, w8, #8, ge
    1004: 540005aa     	b.ge	0x10b8 <__ZN4Game12loadFromFileEPKc+0x46c>
    1008: 7100091f     	cmp	w8, #2
    100c: 52800049     	mov	w9, #2
    1010: 1a89c114     	csel	w20, w8, w9, gt
    1014: d37e7e80     	ubfiz	x0, x20, #2, #32
    1018: 94000000     	bl	0x1018 <__ZN4Game12loadFromFileEPKc+0x3cc>
    101c: aa0003f6     	mov	x22, x0
    1020: f9400700     	ldr	x0, [x24, #8]
    1024: 7100075f     	cmp	w26, #1
    1028: 540003ab     	b.lt	0x109c <__ZN4Game12loadFromFileEPKc+0x450>
    102c: d2800008     	mov	x8, #0
    1030: 7100433f     	cmp	w25, #16
    1034: 54000203     	b.lo	0x1074 <__ZN4Game12loadFromFileEPKc+0x428>
    1038: cb0002c9     	sub	x9, x22, x0
    103c: f101013f     	cmp	x9, #64
    1040: 540001a3     	b.lo	0x1074 <__ZN4Game12loadFromFileEPKc+0x428>
    1044: 927c6f28     	and	x8, x25, #0xfffffff0
    1048: 91008009     	add	x9, x0, #32
    104c: 910082ca     	add	x10, x22, #32
    1050: aa0803eb     	mov	x11, x8
    1054: ad7f0520     	ldp	q0, q1, [x9, #-32]
    1058: acc20d22     	ldp	q2, q3, [x9], #64
    105c: ad3f0540     	stp	q0, q1, [x10, #-32]
    1060: ac820d42     	stp	q2, q3, [x10], #64
    1064: f100416b     	subs	x11, x11, #16
    1068: 54ffff61     	b.ne	0x1054 <__ZN4Game12loadFromFileEPKc+0x408>
    106c: eb19011f     	cmp	x8, x25
    1070: 54000120     	b.eq	0x1094 <__ZN4Game12loadFromFileEPKc+0x448>
    1074: cb080329     	sub	x9, x25, x8
    1078: d37ef50a     	lsl	x10, x8, #2
    107c: 8b0a0008     	add	x8, x0, x10
    1080: 8b0a02ca     	add	x10, x22, x10
    1084: bc404500     	ldr	s0, [x8], #4
    1088: bc004540     	str	s0, [x10], #4
    108c: f1000529     	subs	x9, x9, #1
    1090: 54ffffa1     	b.ne	0x1084 <__ZN4Game12loadFromFileEPKc+0x438>
    1094: b900031f     	str	wzr, [x24]
    1098: 14000003     	b	0x10a4 <__ZN4Game12loadFromFileEPKc+0x458>
    109c: b900031f     	str	wzr, [x24]
    10a0: b4000060     	cbz	x0, 0x10ac <__ZN4Game12loadFromFileEPKc+0x460>
    10a4: 94000000     	bl	0x10a4 <__ZN4Game12loadFromFileEPKc+0x458>
    10a8: f940027b     	ldr	x27, [x19]
    10ac: f9000716     	str	x22, [x24, #8]
    10b0: b9000714     	str	w20, [x24, #4]
    10b4: f94023f6     	ldr	x22, [sp, #64]
    10b8: 52968908     	mov	w8, #46152
    10bc: 8b08037b     	add	x27, x27, x8
    10c0: f9400708     	ldr	x8, [x24, #8]
    10c4: 11000729     	add	w9, w25, #1
    10c8: b9000309     	str	w9, [x24]
    10cc: bd4067e0     	ldr	s0, [sp, #100]
    10d0: bc3a7900     	str	s0, [x8, x26, lsl #2]
    10d4: 29402378     	ldp	w24, w8, [x27]
    10d8: 93407f19     	sxtw	x25, w24
    10dc: 6b08031f     	cmp	w24, w8
    10e0: 54ffe5e1     	b.ne	0xd9c <__ZN4Game12loadFromFileEPKc+0x150>
    10e4: 531f7b28     	lsl	w8, w25, #1
    10e8: 7100073f     	cmp	w25, #1
    10ec: 7a48a328     	ccmp	w25, w8, #8, ge
    10f0: 54ffe56a     	b.ge	0xd9c <__ZN4Game12loadFromFileEPKc+0x150>
    10f4: 7100091f     	cmp	w8, #2
    10f8: 52800049     	mov	w9, #2
    10fc: 1a89c114     	csel	w20, w8, w9, gt
    1100: d37d7e80     	ubfiz	x0, x20, #3, #32
    1104: 94000000     	bl	0x1104 <__ZN4Game12loadFromFileEPKc+0x4b8>
    1108: aa0003f6     	mov	x22, x0
    110c: f9400760     	ldr	x0, [x27, #8]
    1110: 7100073f     	cmp	w25, #1
    1114: 5400038b     	b.lt	0x1184 <__ZN4Game12loadFromFileEPKc+0x538>
    1118: d2800008     	mov	x8, #0
    111c: 7100231f     	cmp	w24, #8
    1120: 54000203     	b.lo	0x1160 <__ZN4Game12loadFromFileEPKc+0x514>
    1124: cb0002c9     	sub	x9, x22, x0
    1128: f101013f     	cmp	x9, #64
    112c: 540001a3     	b.lo	0x1160 <__ZN4Game12loadFromFileEPKc+0x514>
    1130: 927d7308     	and	x8, x24, #0xfffffff8
    1134: 91008009     	add	x9, x0, #32
    1138: 910082ca     	add	x10, x22, #32
    113c: aa0803eb     	mov	x11, x8
    1140: ad7f0520     	ldp	q0, q1, [x9, #-32]
    1144: acc20d22     	ldp	q2, q3, [x9], #64
    1148: ad3f0540     	stp	q0, q1, [x10, #-32]
    114c: ac820d42     	stp	q2, q3, [x10], #64
    1150: f100216b     	subs	x11, x11, #8
    1154: 54ffff61     	b.ne	0x1140 <__ZN4Game12loadFromFileEPKc+0x4f4>
    1158: eb18011f     	cmp	x8, x24
    115c: 54ffe160     	b.eq	0xd88 <__ZN4Game12loadFromFileEPKc+0x13c>
    1160: cb080309     	sub	x9, x24, x8
    1164: d37df10a     	lsl	x10, x8, #3
    1168: 8b0a0008     	add	x8, x0, x10
    116c: 8b0a02ca     	add	x10, x22, x10
    1170: f840850b     	ldr	x11, [x8], #8
    1174: f800854b     	str	x11, [x10], #8
    1178: f1000529     	subs	x9, x9, #1
    117c: 54ffffa1     	b.ne	0x1170 <__ZN4Game12loadFromFileEPKc+0x524>
    1180: 17ffff02     	b	0xd88 <__ZN4Game12loadFromFileEPKc+0x13c>
    1184: b900037f     	str	wzr, [x27]
    1188: b5ffe020     	cbnz	x0, 0xd8c <__ZN4Game12loadFromFileEPKc+0x140>
    118c: 17ffff01     	b	0xd90 <__ZN4Game12loadFromFileEPKc+0x144>
    1190: b85a83a8     	ldur	w8, [x29, #-88]
    1194: 340007c8     	cbz	w8, 0x128c <__ZN4Game12loadFromFileEPKc+0x640>
    1198: d2800014     	mov	x20, #0
    119c: 910123e8     	add	x8, sp, #72
    11a0: b27e0118     	orr	x24, x8, #0x4
    11a4: 90000015     	adrp	x21, 0x1000 <__ZN4Game12loadFromFileEPKc+0x558>
    11a8: 910002b5     	add	x21, x21, #0
    11ac: 52968119     	mov	w25, #46088
    11b0: 5280031b     	mov	w27, #24
    11b4: 14000013     	b	0x1200 <__ZN4Game12loadFromFileEPKc+0x5b4>
    11b8: b900039f     	str	wzr, [x28]
    11bc: b4000040     	cbz	x0, 0x11c4 <__ZN4Game12loadFromFileEPKc+0x578>
    11c0: 94000000     	bl	0x11c0 <__ZN4Game12loadFromFileEPKc+0x574>
    11c4: f9000796     	str	x22, [x28, #8]
    11c8: b9000793     	str	w19, [x28, #4]
    11cc: a943dbf3     	ldp	x19, x22, [sp, #56]
    11d0: f9400788     	ldr	x8, [x28, #8]
    11d4: 11000749     	add	w9, w26, #1
    11d8: b9000389     	str	w9, [x28]
    11dc: 9b3b2348     	smaddl	x8, w26, w27, x8
    11e0: 3cc483e0     	ldur	q0, [sp, #72]
    11e4: f9402fe9     	ldr	x9, [sp, #88]
    11e8: f9000909     	str	x9, [x8, #16]
    11ec: 3d800100     	str	q0, [x8]
    11f0: 91000694     	add	x20, x20, #1
    11f4: b89a83a8     	ldursw	x8, [x29, #-88]
    11f8: eb08029f     	cmp	x20, x8
    11fc: 54000482     	b.hs	0x128c <__ZN4Game12loadFromFileEPKc+0x640>
    1200: 910123e8     	add	x8, sp, #72
    1204: a90063e8     	stp	x8, x24, [sp]
    1208: aa1603e0     	mov	x0, x22
    120c: aa1503e1     	mov	x1, x21
    1210: 94000000     	bl	0x1210 <__ZN4Game12loadFromFileEPKc+0x5c4>
    1214: f9400268     	ldr	x8, [x19]
    1218: 8b19011c     	add	x28, x8, x25
    121c: 29402397     	ldp	w23, w8, [x28]
    1220: 93407efa     	sxtw	x26, w23
    1224: 6b0802ff     	cmp	w23, w8
    1228: 54fffd41     	b.ne	0x11d0 <__ZN4Game12loadFromFileEPKc+0x584>
    122c: 531f7ae8     	lsl	w8, w23, #1
    1230: 7100075f     	cmp	w26, #1
    1234: 7a48a348     	ccmp	w26, w8, #8, ge
    1238: 54fffcca     	b.ge	0x11d0 <__ZN4Game12loadFromFileEPKc+0x584>
    123c: 7100091f     	cmp	w8, #2
    1240: 52800049     	mov	w9, #2
    1244: 1a89c113     	csel	w19, w8, w9, gt
    1248: 9bbb7e60     	umull	x0, w19, w27
    124c: 94000000     	bl	0x124c <__ZN4Game12loadFromFileEPKc+0x600>
    1250: aa0003f6     	mov	x22, x0
    1254: f9400780     	ldr	x0, [x28, #8]
    1258: aa1603e8     	mov	x8, x22
    125c: aa0003e9     	mov	x9, x0
    1260: 710006ff     	cmp	w23, #1
    1264: 54fffaab     	b.lt	0x11b8 <__ZN4Game12loadFromFileEPKc+0x56c>
    1268: 3dc00120     	ldr	q0, [x9]
    126c: f940092a     	ldr	x10, [x9, #16]
    1270: f900090a     	str	x10, [x8, #16]
    1274: 3c818500     	str	q0, [x8], #24
    1278: 91006129     	add	x9, x9, #24
    127c: f10006f7     	subs	x23, x23, #1
    1280: 54ffff41     	b.ne	0x1268 <__ZN4Game12loadFromFileEPKc+0x61c>
    1284: b900039f     	str	wzr, [x28]
    1288: 17ffffce     	b	0x11c0 <__ZN4Game12loadFromFileEPKc+0x574>
    128c: b85a43a8     	ldur	w8, [x29, #-92]
    1290: 34000928     	cbz	w8, 0x13b4 <__ZN4Game12loadFromFileEPKc+0x768>
    1294: d2800017     	mov	x23, #0
    1298: 910123f9     	add	x25, sp, #72
    129c: b27e0328     	orr	x8, x25, #0x4
    12a0: f9001be8     	str	x8, [sp, #48]
    12a4: 9100233a     	add	x26, x25, #8
    12a8: 9100333b     	add	x27, x25, #12
    12ac: 9100433c     	add	x28, x25, #16
    12b0: 91005334     	add	x20, x25, #20
    12b4: 14000016     	b	0x130c <__ZN4Game12loadFromFileEPKc+0x6c0>
    12b8: b900031f     	str	wzr, [x24]
    12bc: b4000040     	cbz	x0, 0x12c4 <__ZN4Game12loadFromFileEPKc+0x678>
    12c0: 94000000     	bl	0x12c0 <__ZN4Game12loadFromFileEPKc+0x674>
    12c4: f9000716     	str	x22, [x24, #8]
    12c8: b9000719     	str	w25, [x24, #4]
    12cc: f94023f6     	ldr	x22, [sp, #64]
    12d0: 910123f9     	add	x25, sp, #72
    12d4: f9400708     	ldr	x8, [x24, #8]
    12d8: 110006a9     	add	w9, w21, #1
    12dc: b9000309     	str	w9, [x24]
    12e0: 52800309     	mov	w9, #24
    12e4: 9b2922a8     	smaddl	x8, w21, w9, x8
    12e8: 3cc483e0     	ldur	q0, [sp, #72]
    12ec: f9402fe9     	ldr	x9, [sp, #88]
    12f0: f9000909     	str	x9, [x8, #16]
    12f4: 3d800100     	str	q0, [x8]
    12f8: 910006f7     	add	x23, x23, #1
    12fc: b89a43a8     	ldursw	x8, [x29, #-92]
    1300: eb0802ff     	cmp	x23, x8
    1304: f9401ff3     	ldr	x19, [sp, #56]
    1308: 54000562     	b.hs	0x13b4 <__ZN4Game12loadFromFileEPKc+0x768>
    130c: a904ffff     	stp	xzr, xzr, [sp, #72]
    1310: f9002fff     	str	xzr, [sp, #88]
    1314: a90253fc     	stp	x28, x20, [sp, #32]
    1318: a9016ffa     	stp	x26, x27, [sp, #16]
    131c: f9401be8     	ldr	x8, [sp, #48]
    1320: a90023f9     	stp	x25, x8, [sp]
    1324: aa1603e0     	mov	x0, x22
    1328: 90000001     	adrp	x1, 0x1000 <__ZN4Game12loadFromFileEPKc+0x6dc>
    132c: 91000021     	add	x1, x1, #0
    1330: 94000000     	bl	0x1330 <__ZN4Game12loadFromFileEPKc+0x6e4>
    1334: f9400268     	ldr	x8, [x19]
    1338: 52969509     	mov	w9, #46248
    133c: 8b090118     	add	x24, x8, x9
    1340: 29402313     	ldp	w19, w8, [x24]
    1344: 93407e75     	sxtw	x21, w19
    1348: 6b08027f     	cmp	w19, w8
    134c: 54fffc41     	b.ne	0x12d4 <__ZN4Game12loadFromFileEPKc+0x688>
    1350: 531f7a68     	lsl	w8, w19, #1
    1354: 710006bf     	cmp	w21, #1
    1358: 7a48a2a8     	ccmp	w21, w8, #8, ge
    135c: 54fffbca     	b.ge	0x12d4 <__ZN4Game12loadFromFileEPKc+0x688>
    1360: 7100091f     	cmp	w8, #2
    1364: 52800049     	mov	w9, #2
    1368: 1a89c119     	csel	w25, w8, w9, gt
    136c: 52800308     	mov	w8, #24
    1370: 9ba87f20     	umull	x0, w25, w8
    1374: 94000000     	bl	0x1374 <__ZN4Game12loadFromFileEPKc+0x728>
    1378: aa0003f6     	mov	x22, x0
    137c: f9400700     	ldr	x0, [x24, #8]
    1380: aa1603e8     	mov	x8, x22
    1384: aa0003e9     	mov	x9, x0
    1388: 7100067f     	cmp	w19, #1
    138c: 54fff96b     	b.lt	0x12b8 <__ZN4Game12loadFromFileEPKc+0x66c>
    1390: 3dc00120     	ldr	q0, [x9]
    1394: f940092a     	ldr	x10, [x9, #16]
    1398: f900090a     	str	x10, [x8, #16]
    139c: 3c818500     	str	q0, [x8], #24
    13a0: 91006129     	add	x9, x9, #24
    13a4: f1000673     	subs	x19, x19, #1
    13a8: 54ffff41     	b.ne	0x1390 <__ZN4Game12loadFromFileEPKc+0x744>
    13ac: b900031f     	str	wzr, [x24]
    13b0: 17ffffc4     	b	0x12c0 <__ZN4Game12loadFromFileEPKc+0x674>
    13b4: aa1603e0     	mov	x0, x22
    13b8: 94000000     	bl	0x13b8 <__ZN4Game12loadFromFileEPKc+0x76c>
    13bc: a94d7bfd     	ldp	x29, x30, [sp, #208]
    13c0: a94c4ff4     	ldp	x20, x19, [sp, #192]
    13c4: a94b57f6     	ldp	x22, x21, [sp, #176]
    13c8: a94a5ff8     	ldp	x24, x23, [sp, #160]
    13cc: a94967fa     	ldp	x26, x25, [sp, #144]
    13d0: a9486ffc     	ldp	x28, x27, [sp, #128]
    13d4: 910383ff     	add	sp, sp, #224
    13d8: d65f03c0     	ret
    13dc: 90000000     	adrp	x0, 0x1000 <__ZN4Game12loadFromFileEPKc+0x790>
    13e0: 91000000     	add	x0, x0, #0
    13e4: a94d7bfd     	ldp	x29, x30, [sp, #208]
    13e8: a94c4ff4     	ldp	x20, x19, [sp, #192]
    13ec: a94b57f6     	ldp	x22, x21, [sp, #176]
    13f0: a94a5ff8     	ldp	x24, x23, [sp, #160]
    13f4: a94967fa     	ldp	x26, x25, [sp, #144]
    13f8: a9486ffc     	ldp	x28, x27, [sp, #128]
    13fc: 910383ff     	add	sp, sp, #224
    1400: 14000000     	b	0x1400 <__ZN4Game12loadFromFileEPKc+0x7b4>

0000000000001404 <__ZN11PointMasses7reserveEi>:
    1404: a9bc5ff8     	stp	x24, x23, [sp, #-64]!
    1408: a90157f6     	stp	x22, x21, [sp, #16]
    140c: a9024ff4     	stp	x20, x19, [sp, #32]
    1410: a9037bfd     	stp	x29, x30, [sp, #48]
    1414: 9100c3fd     	add	x29, sp, #48
    1418: aa0103f4     	mov	x20, x1
    141c: aa0003f3     	mov	x19, x0
    1420: b9400408     	ldr	w8, [x0, #4]
    1424: 7100051f     	cmp	w8, #1
    1428: 7a41a108     	ccmp	w8, w1, #8, ge
    142c: 5400024b     	b.lt	0x1474 <__ZN11PointMasses7reserveEi+0x70>
    1430: b9401668     	ldr	w8, [x19, #20]
    1434: 7100051f     	cmp	w8, #1
    1438: 7a54a108     	ccmp	w8, w20, #8, ge
    143c: 540007ab     	b.lt	0x1530 <__ZN11PointMasses7reserveEi+0x12c>
    1440: b9402668     	ldr	w8, [x19, #36]
    1444: 7100051f     	cmp	w8, #1
    1448: 7a54a108     	ccmp	w8, w20, #8, ge
    144c: 54000d0b     	b.lt	0x15ec <__ZN11PointMasses7reserveEi+0x1e8>
    1450: b9403668     	ldr	w8, [x19, #52]
    1454: 7100051f     	cmp	w8, #1
    1458: 7a54a108     	ccmp	w8, w20, #8, ge
    145c: 5400126b     	b.lt	0x16a8 <__ZN11PointMasses7reserveEi+0x2a4>
    1460: a9437bfd     	ldp	x29, x30, [sp, #48]
    1464: a9424ff4     	ldp	x20, x19, [sp, #32]
    1468: a94157f6     	ldp	x22, x21, [sp, #16]
    146c: a8c45ff8     	ldp	x24, x23, [sp], #64
    1470: d65f03c0     	ret
    1474: 52800048     	mov	w8, #2
    1478: 71000a9f     	cmp	w20, #2
    147c: 1a88c296     	csel	w22, w20, w8, gt
    1480: d37d7ec0     	ubfiz	x0, x22, #3, #32
    1484: 94000000     	bl	0x1484 <__ZN11PointMasses7reserveEi+0x80>
    1488: aa0003f5     	mov	x21, x0
    148c: b9400277     	ldr	w23, [x19]
    1490: f9400660     	ldr	x0, [x19, #8]
    1494: 710006ff     	cmp	w23, #1
    1498: 540003ab     	b.lt	0x150c <__ZN11PointMasses7reserveEi+0x108>
    149c: d2800008     	mov	x8, #0
    14a0: 710022ff     	cmp	w23, #8
    14a4: 54000203     	b.lo	0x14e4 <__ZN11PointMasses7reserveEi+0xe0>
    14a8: cb0002a9     	sub	x9, x21, x0
    14ac: f101013f     	cmp	x9, #64
    14b0: 540001a3     	b.lo	0x14e4 <__ZN11PointMasses7reserveEi+0xe0>
    14b4: 927d72e8     	and	x8, x23, #0xfffffff8
    14b8: 91008009     	add	x9, x0, #32
    14bc: 910082aa     	add	x10, x21, #32
    14c0: aa0803eb     	mov	x11, x8
    14c4: ad7f0520     	ldp	q0, q1, [x9, #-32]
    14c8: acc20d22     	ldp	q2, q3, [x9], #64
    14cc: ad3f0540     	stp	q0, q1, [x10, #-32]
    14d0: ac820d42     	stp	q2, q3, [x10], #64
    14d4: f100216b     	subs	x11, x11, #8
    14d8: 54ffff61     	b.ne	0x14c4 <__ZN11PointMasses7reserveEi+0xc0>
    14dc: eb17011f     	cmp	x8, x23
    14e0: 54000120     	b.eq	0x1504 <__ZN11PointMasses7reserveEi+0x100>
    14e4: cb0802e9     	sub	x9, x23, x8
    14e8: d37df10a     	lsl	x10, x8, #3
    14ec: 8b0a0008     	add	x8, x0, x10
    14f0: 8b0a02aa     	add	x10, x21, x10
    14f4: f840850b     	ldr	x11, [x8], #8
    14f8: f800854b     	str	x11, [x10], #8
    14fc: f1000529     	subs	x9, x9, #1
    1500: 54ffffa1     	b.ne	0x14f4 <__ZN11PointMasses7reserveEi+0xf0>
    1504: b900027f     	str	wzr, [x19]
    1508: 14000003     	b	0x1514 <__ZN11PointMasses7reserveEi+0x110>
    150c: b900027f     	str	wzr, [x19]
    1510: b4000040     	cbz	x0, 0x1518 <__ZN11PointMasses7reserveEi+0x114>
    1514: 94000000     	bl	0x1514 <__ZN11PointMasses7reserveEi+0x110>
    1518: f9000675     	str	x21, [x19, #8]
    151c: 29005a77     	stp	w23, w22, [x19]
    1520: b9401668     	ldr	w8, [x19, #20]
    1524: 7100051f     	cmp	w8, #1
    1528: 7a54a108     	ccmp	w8, w20, #8, ge
    152c: 54fff8aa     	b.ge	0x1440 <__ZN11PointMasses7reserveEi+0x3c>
    1530: 52800048     	mov	w8, #2
    1534: 71000a9f     	cmp	w20, #2
    1538: 1a88c296     	csel	w22, w20, w8, gt
    153c: d37e7ec0     	ubfiz	x0, x22, #2, #32
    1540: 94000000     	bl	0x1540 <__ZN11PointMasses7reserveEi+0x13c>
    1544: aa0003f5     	mov	x21, x0
    1548: b9401277     	ldr	w23, [x19, #16]
    154c: f9400e60     	ldr	x0, [x19, #24]
    1550: 710006ff     	cmp	w23, #1
    1554: 540003ab     	b.lt	0x15c8 <__ZN11PointMasses7reserveEi+0x1c4>
    1558: d2800008     	mov	x8, #0
    155c: 710042ff     	cmp	w23, #16
    1560: 54000203     	b.lo	0x15a0 <__ZN11PointMasses7reserveEi+0x19c>
    1564: cb0002a9     	sub	x9, x21, x0
    1568: f101013f     	cmp	x9, #64
    156c: 540001a3     	b.lo	0x15a0 <__ZN11PointMasses7reserveEi+0x19c>
    1570: 927c6ee8     	and	x8, x23, #0xfffffff0
    1574: 91008009     	add	x9, x0, #32
    1578: 910082aa     	add	x10, x21, #32
    157c: aa0803eb     	mov	x11, x8
    1580: ad7f0520     	ldp	q0, q1, [x9, #-32]
    1584: acc20d22     	ldp	q2, q3, [x9], #64
    1588: ad3f0540     	stp	q0, q1, [x10, #-32]
    158c: ac820d42     	stp	q2, q3, [x10], #64
    1590: f100416b     	subs	x11, x11, #16
    1594: 54ffff61     	b.ne	0x1580 <__ZN11PointMasses7reserveEi+0x17c>
    1598: eb17011f     	cmp	x8, x23
    159c: 54000120     	b.eq	0x15c0 <__ZN11PointMasses7reserveEi+0x1bc>
    15a0: cb0802e9     	sub	x9, x23, x8
    15a4: d37ef50a     	lsl	x10, x8, #2
    15a8: 8b0a0008     	add	x8, x0, x10
    15ac: 8b0a02aa     	add	x10, x21, x10
    15b0: bc404500     	ldr	s0, [x8], #4
    15b4: bc004540     	str	s0, [x10], #4
    15b8: f1000529     	subs	x9, x9, #1
    15bc: 54ffffa1     	b.ne	0x15b0 <__ZN11PointMasses7reserveEi+0x1ac>
    15c0: b900127f     	str	wzr, [x19, #16]
    15c4: 14000003     	b	0x15d0 <__ZN11PointMasses7reserveEi+0x1cc>
    15c8: b900127f     	str	wzr, [x19, #16]
    15cc: b4000040     	cbz	x0, 0x15d4 <__ZN11PointMasses7reserveEi+0x1d0>
    15d0: 94000000     	bl	0x15d0 <__ZN11PointMasses7reserveEi+0x1cc>
    15d4: f9000e75     	str	x21, [x19, #24]
    15d8: 29025a77     	stp	w23, w22, [x19, #16]
    15dc: b9402668     	ldr	w8, [x19, #36]
    15e0: 7100051f     	cmp	w8, #1
    15e4: 7a54a108     	ccmp	w8, w20, #8, ge
    15e8: 54fff34a     	b.ge	0x1450 <__ZN11PointMasses7reserveEi+0x4c>
    15ec: 52800048     	mov	w8, #2
    15f0: 71000a9f     	cmp	w20, #2
    15f4: 1a88c296     	csel	w22, w20, w8, gt
    15f8: d37d7ec0     	ubfiz	x0, x22, #3, #32
    15fc: 94000000     	bl	0x15fc <__ZN11PointMasses7reserveEi+0x1f8>
    1600: aa0003f5     	mov	x21, x0
    1604: b9402277     	ldr	w23, [x19, #32]
    1608: f9401660     	ldr	x0, [x19, #40]
    160c: 710006ff     	cmp	w23, #1
    1610: 540003ab     	b.lt	0x1684 <__ZN11PointMasses7reserveEi+0x280>
    1614: d2800008     	mov	x8, #0
    1618: 710022ff     	cmp	w23, #8
    161c: 54000203     	b.lo	0x165c <__ZN11PointMasses7reserveEi+0x258>
    1620: cb0002a9     	sub	x9, x21, x0
    1624: f101013f     	cmp	x9, #64
    1628: 540001a3     	b.lo	0x165c <__ZN11PointMasses7reserveEi+0x258>
    162c: 927d72e8     	and	x8, x23, #0xfffffff8
    1630: 91008009     	add	x9, x0, #32
    1634: 910082aa     	add	x10, x21, #32
    1638: aa0803eb     	mov	x11, x8
    163c: ad7f0520     	ldp	q0, q1, [x9, #-32]
    1640: acc20d22     	ldp	q2, q3, [x9], #64
    1644: ad3f0540     	stp	q0, q1, [x10, #-32]
    1648: ac820d42     	stp	q2, q3, [x10], #64
    164c: f100216b     	subs	x11, x11, #8
    1650: 54ffff61     	b.ne	0x163c <__ZN11PointMasses7reserveEi+0x238>
    1654: eb17011f     	cmp	x8, x23
    1658: 54000120     	b.eq	0x167c <__ZN11PointMasses7reserveEi+0x278>
    165c: cb0802e9     	sub	x9, x23, x8
    1660: d37df10a     	lsl	x10, x8, #3
    1664: 8b0a0008     	add	x8, x0, x10
    1668: 8b0a02aa     	add	x10, x21, x10
    166c: f840850b     	ldr	x11, [x8], #8
    1670: f800854b     	str	x11, [x10], #8
    1674: f1000529     	subs	x9, x9, #1
    1678: 54ffffa1     	b.ne	0x166c <__ZN11PointMasses7reserveEi+0x268>
    167c: b900227f     	str	wzr, [x19, #32]
    1680: 14000003     	b	0x168c <__ZN11PointMasses7reserveEi+0x288>
    1684: b900227f     	str	wzr, [x19, #32]
    1688: b4000040     	cbz	x0, 0x1690 <__ZN11PointMasses7reserveEi+0x28c>
    168c: 94000000     	bl	0x168c <__ZN11PointMasses7reserveEi+0x288>
    1690: f9001675     	str	x21, [x19, #40]
    1694: 29045a77     	stp	w23, w22, [x19, #32]
    1698: b9403668     	ldr	w8, [x19, #52]
    169c: 7100051f     	cmp	w8, #1
    16a0: 7a54a108     	ccmp	w8, w20, #8, ge
    16a4: 54ffedea     	b.ge	0x1460 <__ZN11PointMasses7reserveEi+0x5c>
    16a8: 52800048     	mov	w8, #2
    16ac: 71000a9f     	cmp	w20, #2
    16b0: 1a88c295     	csel	w21, w20, w8, gt
    16b4: d37d7ea0     	ubfiz	x0, x21, #3, #32
    16b8: 94000000     	bl	0x16b8 <__ZN11PointMasses7reserveEi+0x2b4>
    16bc: aa0003f4     	mov	x20, x0
    16c0: b9403276     	ldr	w22, [x19, #48]
    16c4: f9401e60     	ldr	x0, [x19, #56]
    16c8: 710006df     	cmp	w22, #1
    16cc: 540003ab     	b.lt	0x1740 <__ZN11PointMasses7reserveEi+0x33c>
    16d0: d2800008     	mov	x8, #0
    16d4: 710022df     	cmp	w22, #8
    16d8: 54000203     	b.lo	0x1718 <__ZN11PointMasses7reserveEi+0x314>
    16dc: cb000289     	sub	x9, x20, x0
    16e0: f101013f     	cmp	x9, #64
    16e4: 540001a3     	b.lo	0x1718 <__ZN11PointMasses7reserveEi+0x314>
    16e8: 927d72c8     	and	x8, x22, #0xfffffff8
    16ec: 91008009     	add	x9, x0, #32
    16f0: 9100828a     	add	x10, x20, #32
    16f4: aa0803eb     	mov	x11, x8
    16f8: ad7f0520     	ldp	q0, q1, [x9, #-32]
    16fc: acc20d22     	ldp	q2, q3, [x9], #64
    1700: ad3f0540     	stp	q0, q1, [x10, #-32]
    1704: ac820d42     	stp	q2, q3, [x10], #64
    1708: f100216b     	subs	x11, x11, #8
    170c: 54ffff61     	b.ne	0x16f8 <__ZN11PointMasses7reserveEi+0x2f4>
    1710: eb16011f     	cmp	x8, x22
    1714: 54000120     	b.eq	0x1738 <__ZN11PointMasses7reserveEi+0x334>
    1718: cb0802c9     	sub	x9, x22, x8
    171c: d37df10a     	lsl	x10, x8, #3
    1720: 8b0a0008     	add	x8, x0, x10
    1724: 8b0a028a     	add	x10, x20, x10
    1728: f840850b     	ldr	x11, [x8], #8
    172c: f800854b     	str	x11, [x10], #8
    1730: f1000529     	subs	x9, x9, #1
    1734: 54ffffa1     	b.ne	0x1728 <__ZN11PointMasses7reserveEi+0x324>
    1738: b900327f     	str	wzr, [x19, #48]
    173c: 14000003     	b	0x1748 <__ZN11PointMasses7reserveEi+0x344>
    1740: b900327f     	str	wzr, [x19, #48]
    1744: b4000040     	cbz	x0, 0x174c <__ZN11PointMasses7reserveEi+0x348>
    1748: 94000000     	bl	0x1748 <__ZN11PointMasses7reserveEi+0x344>
    174c: f9001e74     	str	x20, [x19, #56]
    1750: 29065676     	stp	w22, w21, [x19, #48]
    1754: a9437bfd     	ldp	x29, x30, [sp, #48]
    1758: a9424ff4     	ldp	x20, x19, [sp, #32]
    175c: a94157f6     	ldp	x22, x21, [sp, #16]
    1760: a8c45ff8     	ldp	x24, x23, [sp], #64
    1764: d65f03c0     	ret

0000000000001768 <__ZN4Game10dumpToFileEPKc>:
    1768: d10243ff     	sub	sp, sp, #144
    176c: a9036ffc     	stp	x28, x27, [sp, #48]
    1770: a90467fa     	stp	x26, x25, [sp, #64]
    1774: a9055ff8     	stp	x24, x23, [sp, #80]
    1778: a90657f6     	stp	x22, x21, [sp, #96]
    177c: a9074ff4     	stp	x20, x19, [sp, #112]
    1780: a9087bfd     	stp	x29, x30, [sp, #128]
    1784: 910203fd     	add	x29, sp, #128
    1788: aa0103e8     	mov	x8, x1
    178c: aa0003f3     	mov	x19, x0
    1790: 90000001     	adrp	x1, 0x1000 <__ZN4Game10dumpToFileEPKc+0x28>
    1794: 91000021     	add	x1, x1, #0
    1798: aa0803e0     	mov	x0, x8
    179c: 94000000     	bl	0x179c <__ZN4Game10dumpToFileEPKc+0x34>
    17a0: b4000f60     	cbz	x0, 0x198c <__ZN4Game10dumpToFileEPKc+0x224>
    17a4: aa0003f4     	mov	x20, x0
    17a8: f9400268     	ldr	x8, [x19]
    17ac: 52968109     	mov	w9, #46088
    17b0: 8b090108     	add	x8, x8, x9
    17b4: b9401109     	ldr	w9, [x8, #16]
    17b8: b940010a     	ldr	w10, [x8]
    17bc: b940a108     	ldr	w8, [x8, #160]
    17c0: a900a3ea     	stp	x10, x8, [sp, #8]
    17c4: f90003e9     	str	x9, [sp]
    17c8: 90000001     	adrp	x1, 0x1000 <__ZN4Game10dumpToFileEPKc+0x60>
    17cc: 91000021     	add	x1, x1, #0
    17d0: 94000000     	bl	0x17d0 <__ZN4Game10dumpToFileEPKc+0x68>
    17d4: f9400268     	ldr	x8, [x19]
    17d8: 52968309     	mov	w9, #46104
    17dc: b8696909     	ldr	w9, [x8, x9]
    17e0: 340004c9     	cbz	w9, 0x1878 <__ZN4Game10dumpToFileEPKc+0x110>
    17e4: d2800016     	mov	x22, #0
    17e8: d2800017     	mov	x23, #0
    17ec: 52968418     	mov	w24, #46112
    17f0: 52968319     	mov	w25, #46104
    17f4: d2c0003a     	mov	x26, #4294967296
    17f8: 90000015     	adrp	x21, 0x1000 <__ZN4Game10dumpToFileEPKc+0x90>
    17fc: 910002b5     	add	x21, x21, #0
    1800: 8b180108     	add	x8, x8, x24
    1804: f9400109     	ldr	x9, [x8]
    1808: 935dfeca     	asr	x10, x22, #29
    180c: 8b0a0129     	add	x9, x9, x10
    1810: 2d400520     	ldp	s0, s1, [x9]
    1814: f9401109     	ldr	x9, [x8, #32]
    1818: 8b0a0129     	add	x9, x9, x10
    181c: f9400908     	ldr	x8, [x8, #16]
    1820: 935efeca     	asr	x10, x22, #30
    1824: bc6a6902     	ldr	s2, [x8, x10]
    1828: 2d401123     	ldp	s3, s4, [x9]
    182c: 1e22c042     	fcvt	d2, s2
    1830: 1e22c000     	fcvt	d0, s0
    1834: 1e22c021     	fcvt	d1, s1
    1838: 1e22c063     	fcvt	d3, s3
    183c: 1e22c084     	fcvt	d4, s4
    1840: fd0013e4     	str	d4, [sp, #32]
    1844: fd000fe3     	str	d3, [sp, #24]
    1848: fd000be1     	str	d1, [sp, #16]
    184c: fd0007e0     	str	d0, [sp, #8]
    1850: fd0003e2     	str	d2, [sp]
    1854: aa1403e0     	mov	x0, x20
    1858: aa1503e1     	mov	x1, x21
    185c: 94000000     	bl	0x185c <__ZN4Game10dumpToFileEPKc+0xf4>
    1860: 910006f7     	add	x23, x23, #1
    1864: f9400268     	ldr	x8, [x19]
    1868: b8b96909     	ldrsw	x9, [x8, x25]
    186c: 8b1a02d6     	add	x22, x22, x26
    1870: eb0902ff     	cmp	x23, x9
    1874: 54fffc63     	b.lo	0x1800 <__ZN4Game10dumpToFileEPKc+0x98>
    1878: 52968109     	mov	w9, #46088
    187c: b8696909     	ldr	w9, [x8, x9]
    1880: 340002e9     	cbz	w9, 0x18dc <__ZN4Game10dumpToFileEPKc+0x174>
    1884: d2800016     	mov	x22, #0
    1888: d2800017     	mov	x23, #0
    188c: 52968218     	mov	w24, #46096
    1890: 52800319     	mov	w25, #24
    1894: 5296811a     	mov	w26, #46088
    1898: d2c0003b     	mov	x27, #4294967296
    189c: 90000015     	adrp	x21, 0x1000 <__ZN4Game10dumpToFileEPKc+0x134>
    18a0: 910002b5     	add	x21, x21, #0
    18a4: f8786908     	ldr	x8, [x8, x24]
    18a8: 9360fec9     	asr	x9, x22, #32
    18ac: 9b392128     	smaddl	x8, w9, w25, x8
    18b0: 29402109     	ldp	w9, w8, [x8]
    18b4: a90023e9     	stp	x9, x8, [sp]
    18b8: aa1403e0     	mov	x0, x20
    18bc: aa1503e1     	mov	x1, x21
    18c0: 94000000     	bl	0x18c0 <__ZN4Game10dumpToFileEPKc+0x158>
    18c4: 910006f7     	add	x23, x23, #1
    18c8: f9400268     	ldr	x8, [x19]
    18cc: b8ba6909     	ldrsw	x9, [x8, x26]
    18d0: 8b1b02d6     	add	x22, x22, x27
    18d4: eb0902ff     	cmp	x23, x9
    18d8: 54fffe63     	b.lo	0x18a4 <__ZN4Game10dumpToFileEPKc+0x13c>
    18dc: 52969509     	mov	w9, #46248
    18e0: b8696909     	ldr	w9, [x8, x9]
    18e4: 34000429     	cbz	w9, 0x1968 <__ZN4Game10dumpToFileEPKc+0x200>
    18e8: d2800016     	mov	x22, #0
    18ec: d2800017     	mov	x23, #0
    18f0: 52969618     	mov	w24, #46256
    18f4: 52800319     	mov	w25, #24
    18f8: 5296951a     	mov	w26, #46248
    18fc: d2c0003b     	mov	x27, #4294967296
    1900: 90000015     	adrp	x21, 0x1000 <__ZN4Game10dumpToFileEPKc+0x198>
    1904: 910002b5     	add	x21, x21, #0
    1908: f8786908     	ldr	x8, [x8, x24]
    190c: 9360fec9     	asr	x9, x22, #32
    1910: 9b392128     	smaddl	x8, w9, w25, x8
    1914: 2d410500     	ldp	s0, s1, [x8, #8]
    1918: 1e22c000     	fcvt	d0, s0
    191c: 1e22c021     	fcvt	d1, s1
    1920: bd401102     	ldr	s2, [x8, #16]
    1924: 1e22c042     	fcvt	d2, s2
    1928: 29402909     	ldp	w9, w10, [x8]
    192c: b9401508     	ldr	w8, [x8, #20]
    1930: f90017e8     	str	x8, [sp, #40]
    1934: fd0013e2     	str	d2, [sp, #32]
    1938: fd000fe1     	str	d1, [sp, #24]
    193c: fd000be0     	str	d0, [sp, #16]
    1940: a9002be9     	stp	x9, x10, [sp]
    1944: aa1403e0     	mov	x0, x20
    1948: aa1503e1     	mov	x1, x21
    194c: 94000000     	bl	0x194c <__ZN4Game10dumpToFileEPKc+0x1e4>
    1950: 910006f7     	add	x23, x23, #1
    1954: f9400268     	ldr	x8, [x19]
    1958: b8ba6909     	ldrsw	x9, [x8, x26]
    195c: 8b1b02d6     	add	x22, x22, x27
    1960: eb0902ff     	cmp	x23, x9
    1964: 54fffd23     	b.lo	0x1908 <__ZN4Game10dumpToFileEPKc+0x1a0>
    1968: aa1403e0     	mov	x0, x20
    196c: a9487bfd     	ldp	x29, x30, [sp, #128]
    1970: a9474ff4     	ldp	x20, x19, [sp, #112]
    1974: a94657f6     	ldp	x22, x21, [sp, #96]
    1978: a9455ff8     	ldp	x24, x23, [sp, #80]
    197c: a94467fa     	ldp	x26, x25, [sp, #64]
    1980: a9436ffc     	ldp	x28, x27, [sp, #48]
    1984: 910243ff     	add	sp, sp, #144
    1988: 14000000     	b	0x1988 <__ZN4Game10dumpToFileEPKc+0x220>
    198c: 90000000     	adrp	x0, 0x1000 <__ZN4Game10dumpToFileEPKc+0x224>
    1990: 91000000     	add	x0, x0, #0
    1994: a9487bfd     	ldp	x29, x30, [sp, #128]
    1998: a9474ff4     	ldp	x20, x19, [sp, #112]
    199c: a94657f6     	ldp	x22, x21, [sp, #96]
    19a0: a9455ff8     	ldp	x24, x23, [sp, #80]
    19a4: a94467fa     	ldp	x26, x25, [sp, #64]
    19a8: a9436ffc     	ldp	x28, x27, [sp, #48]
    19ac: 910243ff     	add	sp, sp, #144
    19b0: 14000000     	b	0x19b0 <__ZN4Game10dumpToFileEPKc+0x248>

00000000000019b4 <__ZN4Game26updateAfterRewindOrForwardEv>:
    19b4: a9bc5ff8     	stp	x24, x23, [sp, #-64]!
    19b8: a90157f6     	stp	x22, x21, [sp, #16]
    19bc: a9024ff4     	stp	x20, x19, [sp, #32]
    19c0: a9037bfd     	stp	x29, x30, [sp, #48]
    19c4: 9100c3fd     	add	x29, sp, #48
    19c8: aa0003f3     	mov	x19, x0
    19cc: f9400008     	ldr	x8, [x0]
    19d0: 52968089     	mov	w9, #46084
    19d4: 8b090115     	add	x21, x8, x9
    19d8: b90006bf     	str	wzr, [x21, #4]
    19dc: b90016bf     	str	wzr, [x21, #20]
    19e0: b90026bf     	str	wzr, [x21, #36]
    19e4: b90036bf     	str	wzr, [x21, #52]
    19e8: b90046bf     	str	wzr, [x21, #68]
    19ec: b900a6bf     	str	wzr, [x21, #164]
    19f0: b900b6bf     	str	wzr, [x21, #180]
    19f4: b90066bf     	str	wzr, [x21, #100]
    19f8: b90076bf     	str	wzr, [x21, #116]
    19fc: b90086bf     	str	wzr, [x21, #132]
    1a00: b90096bf     	str	wzr, [x21, #148]
    1a04: b90056bf     	str	wzr, [x21, #84]
    1a08: b98002a9     	ldrsw	x9, [x21]
    1a0c: 5280180a     	mov	w10, #192
    1a10: 9b2a2136     	smaddl	x22, w9, w10, x8
    1a14: b94002c8     	ldr	w8, [x22]
    1a18: b9400aa9     	ldr	w9, [x21, #8]
    1a1c: 7100053f     	cmp	w9, #1
    1a20: 7a48a128     	ccmp	w9, w8, #8, ge
    1a24: 540001ea     	b.ge	0x1a60 <__ZN4Game26updateAfterRewindOrForwardEv+0xac>
    1a28: 52800049     	mov	w9, #2
    1a2c: 7100091f     	cmp	w8, #2
    1a30: 1a89c117     	csel	w23, w8, w9, gt
    1a34: 52800308     	mov	w8, #24
    1a38: 9ba87ee0     	umull	x0, w23, w8
    1a3c: 94000000     	bl	0x1a3c <__ZN4Game26updateAfterRewindOrForwardEv+0x88>
    1a40: aa0003f4     	mov	x20, x0
    1a44: f840c2a0     	ldur	x0, [x21, #12]
    1a48: b90006bf     	str	wzr, [x21, #4]
    1a4c: b4000040     	cbz	x0, 0x1a54 <__ZN4Game26updateAfterRewindOrForwardEv+0xa0>
    1a50: 94000000     	bl	0x1a50 <__ZN4Game26updateAfterRewindOrForwardEv+0x9c>
    1a54: f800c2b4     	stur	x20, [x21, #12]
    1a58: 2900debf     	stp	wzr, w23, [x21, #4]
    1a5c: b94002c8     	ldr	w8, [x22]
    1a60: 7100051f     	cmp	w8, #1
    1a64: 5400028b     	b.lt	0x1ab4 <__ZN4Game26updateAfterRewindOrForwardEv+0x100>
    1a68: d2800008     	mov	x8, #0
    1a6c: d2800009     	mov	x9, #0
    1a70: 5280030a     	mov	w10, #24
    1a74: f840c2ab     	ldur	x11, [x21, #12]
    1a78: b98006ac     	ldrsw	x12, [x21, #4]
    1a7c: 1100058d     	add	w13, w12, #1
    1a80: b90006ad     	str	w13, [x21, #4]
    1a84: 9b2a2d8b     	smaddl	x11, w12, w10, x11
    1a88: f94006cc     	ldr	x12, [x22, #8]
    1a8c: 8b08018c     	add	x12, x12, x8
    1a90: 3dc00180     	ldr	q0, [x12]
    1a94: f940098c     	ldr	x12, [x12, #16]
    1a98: f900096c     	str	x12, [x11, #16]
    1a9c: 3d800160     	str	q0, [x11]
    1aa0: 91000529     	add	x9, x9, #1
    1aa4: b98002cb     	ldrsw	x11, [x22]
    1aa8: 91006108     	add	x8, x8, #24
    1aac: eb0b013f     	cmp	x9, x11
    1ab0: 54fffe2b     	b.lt	0x1a74 <__ZN4Game26updateAfterRewindOrForwardEv+0xc0>
    1ab4: f9400268     	ldr	x8, [x19]
    1ab8: 52968089     	mov	w9, #46084
    1abc: 8b090115     	add	x21, x8, x9
    1ac0: b98002a9     	ldrsw	x9, [x21]
    1ac4: 5280180a     	mov	w10, #192
    1ac8: 9b2a2136     	smaddl	x22, w9, w10, x8
    1acc: b940b2c8     	ldr	w8, [x22, #176]
    1ad0: 294aaab7     	ldp	w23, w10, [x21, #84]
    1ad4: 0b170109     	add	w9, w8, w23
    1ad8: 7100055f     	cmp	w10, #1
    1adc: 7a49a148     	ccmp	w10, w9, #8, ge
    1ae0: 540003aa     	b.ge	0x1b54 <__ZN4Game26updateAfterRewindOrForwardEv+0x1a0>
    1ae4: 52800048     	mov	w8, #2
    1ae8: 7100093f     	cmp	w9, #2
    1aec: 1a88c138     	csel	w24, w9, w8, gt
    1af0: 52800308     	mov	w8, #24
    1af4: 9ba87f00     	umull	x0, w24, w8
    1af8: 94000000     	bl	0x1af8 <__ZN4Game26updateAfterRewindOrForwardEv+0x144>
    1afc: aa0003f4     	mov	x20, x0
    1b00: f845c2a0     	ldur	x0, [x21, #92]
    1b04: 710006ff     	cmp	w23, #1
    1b08: 540001ab     	b.lt	0x1b3c <__ZN4Game26updateAfterRewindOrForwardEv+0x188>
    1b0c: aa1403e8     	mov	x8, x20
    1b10: aa0003e9     	mov	x9, x0
    1b14: aa1703ea     	mov	x10, x23
    1b18: 3dc00120     	ldr	q0, [x9]
    1b1c: f940092b     	ldr	x11, [x9, #16]
    1b20: f900090b     	str	x11, [x8, #16]
    1b24: 3c818500     	str	q0, [x8], #24
    1b28: 91006129     	add	x9, x9, #24
    1b2c: f100054a     	subs	x10, x10, #1
    1b30: 54ffff41     	b.ne	0x1b18 <__ZN4Game26updateAfterRewindOrForwardEv+0x164>
    1b34: b90056bf     	str	wzr, [x21, #84]
    1b38: 14000003     	b	0x1b44 <__ZN4Game26updateAfterRewindOrForwardEv+0x190>
    1b3c: b90056bf     	str	wzr, [x21, #84]
    1b40: b4000040     	cbz	x0, 0x1b48 <__ZN4Game26updateAfterRewindOrForwardEv+0x194>
    1b44: 94000000     	bl	0x1b44 <__ZN4Game26updateAfterRewindOrForwardEv+0x190>
    1b48: f805c2b4     	stur	x20, [x21, #92]
    1b4c: 290ae2b7     	stp	w23, w24, [x21, #84]
    1b50: b940b2c8     	ldr	w8, [x22, #176]
    1b54: 7100051f     	cmp	w8, #1
    1b58: 5400028b     	b.lt	0x1ba8 <__ZN4Game26updateAfterRewindOrForwardEv+0x1f4>
    1b5c: d2800008     	mov	x8, #0
    1b60: d2800009     	mov	x9, #0
    1b64: 5280030a     	mov	w10, #24
    1b68: f845c2ab     	ldur	x11, [x21, #92]
    1b6c: b98056ac     	ldrsw	x12, [x21, #84]
    1b70: 1100058d     	add	w13, w12, #1
    1b74: b90056ad     	str	w13, [x21, #84]
    1b78: 9b2a2d8b     	smaddl	x11, w12, w10, x11
    1b7c: f9405ecc     	ldr	x12, [x22, #184]
    1b80: 8b08018c     	add	x12, x12, x8
    1b84: 3dc00180     	ldr	q0, [x12]
    1b88: f940098c     	ldr	x12, [x12, #16]
    1b8c: f900096c     	str	x12, [x11, #16]
    1b90: 3d800160     	str	q0, [x11]
    1b94: 91000529     	add	x9, x9, #1
    1b98: b980b2cb     	ldrsw	x11, [x22, #176]
    1b9c: 91006108     	add	x8, x8, #24
    1ba0: eb0b013f     	cmp	x9, x11
    1ba4: 54fffe2b     	b.lt	0x1b68 <__ZN4Game26updateAfterRewindOrForwardEv+0x1b4>
    1ba8: f9400268     	ldr	x8, [x19]
    1bac: 52968309     	mov	w9, #46104
    1bb0: 8b090100     	add	x0, x8, x9
    1bb4: 52968094     	mov	w20, #46084
    1bb8: b8b46909     	ldrsw	x9, [x8, x20]
    1bbc: 52801816     	mov	w22, #192
    1bc0: 9b362128     	smaddl	x8, w9, w22, x8
    1bc4: 91014101     	add	x1, x8, #80
    1bc8: 94000000     	bl	0x1bc8 <__ZN4Game26updateAfterRewindOrForwardEv+0x214>
    1bcc: f9400268     	ldr	x8, [x19]
    1bd0: 52968d09     	mov	w9, #46184
    1bd4: 8b090100     	add	x0, x8, x9
    1bd8: b8b46909     	ldrsw	x9, [x8, x20]
    1bdc: 9b362128     	smaddl	x8, w9, w22, x8
    1be0: 91004101     	add	x1, x8, #16
    1be4: 94000000     	bl	0x1be4 <__ZN4Game26updateAfterRewindOrForwardEv+0x230>
    1be8: f9400268     	ldr	x8, [x19]
    1bec: 8b140115     	add	x21, x8, x20
    1bf0: b98002a9     	ldrsw	x9, [x21]
    1bf4: 9b362136     	smaddl	x22, w9, w22, x8
    1bf8: b94092c8     	ldr	w8, [x22, #144]
    1bfc: 2954aab7     	ldp	w23, w10, [x21, #164]
    1c00: 0b170109     	add	w9, w8, w23
    1c04: 7100055f     	cmp	w10, #1
    1c08: 7a49a148     	ccmp	w10, w9, #8, ge
    1c0c: 540003aa     	b.ge	0x1c80 <__ZN4Game26updateAfterRewindOrForwardEv+0x2cc>
    1c10: 52800048     	mov	w8, #2
    1c14: 7100093f     	cmp	w9, #2
    1c18: 1a88c138     	csel	w24, w9, w8, gt
    1c1c: 52800308     	mov	w8, #24
    1c20: 9ba87f00     	umull	x0, w24, w8
    1c24: 94000000     	bl	0x1c24 <__ZN4Game26updateAfterRewindOrForwardEv+0x270>
    1c28: aa0003f4     	mov	x20, x0
    1c2c: f84ac2a0     	ldur	x0, [x21, #172]
    1c30: 710006ff     	cmp	w23, #1
    1c34: 540001ab     	b.lt	0x1c68 <__ZN4Game26updateAfterRewindOrForwardEv+0x2b4>
    1c38: aa1403e8     	mov	x8, x20
    1c3c: aa0003e9     	mov	x9, x0
    1c40: aa1703ea     	mov	x10, x23
    1c44: 3dc00120     	ldr	q0, [x9]
    1c48: f940092b     	ldr	x11, [x9, #16]
    1c4c: f900090b     	str	x11, [x8, #16]
    1c50: 3c818500     	str	q0, [x8], #24
    1c54: 91006129     	add	x9, x9, #24
    1c58: f100054a     	subs	x10, x10, #1
    1c5c: 54ffff41     	b.ne	0x1c44 <__ZN4Game26updateAfterRewindOrForwardEv+0x290>
    1c60: b900a6bf     	str	wzr, [x21, #164]
    1c64: 14000003     	b	0x1c70 <__ZN4Game26updateAfterRewindOrForwardEv+0x2bc>
    1c68: b900a6bf     	str	wzr, [x21, #164]
    1c6c: b4000040     	cbz	x0, 0x1c74 <__ZN4Game26updateAfterRewindOrForwardEv+0x2c0>
    1c70: 94000000     	bl	0x1c70 <__ZN4Game26updateAfterRewindOrForwardEv+0x2bc>
    1c74: f80ac2b4     	stur	x20, [x21, #172]
    1c78: 2914e2b7     	stp	w23, w24, [x21, #164]
    1c7c: b94092c8     	ldr	w8, [x22, #144]
    1c80: 7100051f     	cmp	w8, #1
    1c84: 5400028b     	b.lt	0x1cd4 <__ZN4Game26updateAfterRewindOrForwardEv+0x320>
    1c88: d2800008     	mov	x8, #0
    1c8c: d2800009     	mov	x9, #0
    1c90: 5280030a     	mov	w10, #24
    1c94: f84ac2ab     	ldur	x11, [x21, #172]
    1c98: b980a6ac     	ldrsw	x12, [x21, #164]
    1c9c: 1100058d     	add	w13, w12, #1
    1ca0: b900a6ad     	str	w13, [x21, #164]
    1ca4: 9b2a2d8b     	smaddl	x11, w12, w10, x11
    1ca8: f9404ecc     	ldr	x12, [x22, #152]
    1cac: 8b08018c     	add	x12, x12, x8
    1cb0: 3dc00180     	ldr	q0, [x12]
    1cb4: f940098c     	ldr	x12, [x12, #16]
    1cb8: f900096c     	str	x12, [x11, #16]
    1cbc: 3d800160     	str	q0, [x11]
    1cc0: 91000529     	add	x9, x9, #1
    1cc4: b98092cb     	ldrsw	x11, [x22, #144]
    1cc8: 91006108     	add	x8, x8, #24
    1ccc: eb0b013f     	cmp	x9, x11
    1cd0: 54fffe2b     	b.lt	0x1c94 <__ZN4Game26updateAfterRewindOrForwardEv+0x2e0>
    1cd4: f9400268     	ldr	x8, [x19]
    1cd8: 52968089     	mov	w9, #46084
    1cdc: 8b090114     	add	x20, x8, x9
    1ce0: b9800289     	ldrsw	x9, [x20]
    1ce4: 5280180a     	mov	w10, #192
    1ce8: 9b2a2135     	smaddl	x21, w9, w10, x8
    1cec: b940a2a8     	ldr	w8, [x21, #160]
    1cf0: 2956aa96     	ldp	w22, w10, [x20, #180]
    1cf4: 0b160109     	add	w9, w8, w22
    1cf8: 7100055f     	cmp	w10, #1
    1cfc: 7a49a148     	ccmp	w10, w9, #8, ge
    1d00: 540003aa     	b.ge	0x1d74 <__ZN4Game26updateAfterRewindOrForwardEv+0x3c0>
    1d04: 52800048     	mov	w8, #2
    1d08: 7100093f     	cmp	w9, #2
    1d0c: 1a88c137     	csel	w23, w9, w8, gt
    1d10: 52800188     	mov	w8, #12
    1d14: 9ba87ee0     	umull	x0, w23, w8
    1d18: 94000000     	bl	0x1d18 <__ZN4Game26updateAfterRewindOrForwardEv+0x364>
    1d1c: aa0003f3     	mov	x19, x0
    1d20: f84bc280     	ldur	x0, [x20, #188]
    1d24: 710006df     	cmp	w22, #1
    1d28: 540001ab     	b.lt	0x1d5c <__ZN4Game26updateAfterRewindOrForwardEv+0x3a8>
    1d2c: aa1303e8     	mov	x8, x19
    1d30: aa0003e9     	mov	x9, x0
    1d34: aa1603ea     	mov	x10, x22
    1d38: f940012b     	ldr	x11, [x9]
    1d3c: b940092c     	ldr	w12, [x9, #8]
    1d40: b900090c     	str	w12, [x8, #8]
    1d44: f800c50b     	str	x11, [x8], #12
    1d48: 91003129     	add	x9, x9, #12
    1d4c: f100054a     	subs	x10, x10, #1
    1d50: 54ffff41     	b.ne	0x1d38 <__ZN4Game26updateAfterRewindOrForwardEv+0x384>
    1d54: b900b69f     	str	wzr, [x20, #180]
    1d58: 14000003     	b	0x1d64 <__ZN4Game26updateAfterRewindOrForwardEv+0x3b0>
    1d5c: b900b69f     	str	wzr, [x20, #180]
    1d60: b4000040     	cbz	x0, 0x1d68 <__ZN4Game26updateAfterRewindOrForwardEv+0x3b4>
    1d64: 94000000     	bl	0x1d64 <__ZN4Game26updateAfterRewindOrForwardEv+0x3b0>
    1d68: f80bc293     	stur	x19, [x20, #188]
    1d6c: 2916de96     	stp	w22, w23, [x20, #180]
    1d70: b940a2a8     	ldr	w8, [x21, #160]
    1d74: 7100051f     	cmp	w8, #1
    1d78: 5400028b     	b.lt	0x1dc8 <__ZN4Game26updateAfterRewindOrForwardEv+0x414>
    1d7c: d2800008     	mov	x8, #0
    1d80: d2800009     	mov	x9, #0
    1d84: 5280018a     	mov	w10, #12
    1d88: f84bc28b     	ldur	x11, [x20, #188]
    1d8c: b980b68c     	ldrsw	x12, [x20, #180]
    1d90: 1100058d     	add	w13, w12, #1
    1d94: b900b68d     	str	w13, [x20, #180]
    1d98: 9b2a2d8b     	smaddl	x11, w12, w10, x11
    1d9c: f94056ac     	ldr	x12, [x21, #168]
    1da0: 8b08018c     	add	x12, x12, x8
    1da4: f940018d     	ldr	x13, [x12]
    1da8: b940098c     	ldr	w12, [x12, #8]
    1dac: b900096c     	str	w12, [x11, #8]
    1db0: f900016d     	str	x13, [x11]
    1db4: 91000529     	add	x9, x9, #1
    1db8: b980a2ab     	ldrsw	x11, [x21, #160]
    1dbc: 91003108     	add	x8, x8, #12
    1dc0: eb0b013f     	cmp	x9, x11
    1dc4: 54fffe2b     	b.lt	0x1d88 <__ZN4Game26updateAfterRewindOrForwardEv+0x3d4>
    1dc8: a9437bfd     	ldp	x29, x30, [sp, #48]
    1dcc: a9424ff4     	ldp	x20, x19, [sp, #32]
    1dd0: a94157f6     	ldp	x22, x21, [sp, #16]
    1dd4: a8c45ff8     	ldp	x24, x23, [sp], #64
    1dd8: d65f03c0     	ret

0000000000001ddc <__ZN11PointMasses6appendERKS_>:
    1ddc: a9bc5ff8     	stp	x24, x23, [sp, #-64]!
    1de0: a90157f6     	stp	x22, x21, [sp, #16]
    1de4: a9024ff4     	stp	x20, x19, [sp, #32]
    1de8: a9037bfd     	stp	x29, x30, [sp, #48]
    1dec: 9100c3fd     	add	x29, sp, #48
    1df0: aa0103f3     	mov	x19, x1
    1df4: aa0003f4     	mov	x20, x0
    1df8: b9400028     	ldr	w8, [x1]
    1dfc: 29402816     	ldp	w22, w10, [x0]
    1e00: 0b160109     	add	w9, w8, w22
    1e04: 7100055f     	cmp	w10, #1
    1e08: 7a49a148     	ccmp	w10, w9, #8, ge
    1e0c: 5400058a     	b.ge	0x1ebc <__ZN11PointMasses6appendERKS_+0xe0>
    1e10: 52800048     	mov	w8, #2
    1e14: 7100093f     	cmp	w9, #2
    1e18: 1a88c137     	csel	w23, w9, w8, gt
    1e1c: d37d7ee0     	ubfiz	x0, x23, #3, #32
    1e20: 94000000     	bl	0x1e20 <__ZN11PointMasses6appendERKS_+0x44>
    1e24: aa0003f5     	mov	x21, x0
    1e28: f9400680     	ldr	x0, [x20, #8]
    1e2c: 710006df     	cmp	w22, #1
    1e30: 540003ab     	b.lt	0x1ea4 <__ZN11PointMasses6appendERKS_+0xc8>
    1e34: d2800008     	mov	x8, #0
    1e38: 710022df     	cmp	w22, #8
    1e3c: 54000203     	b.lo	0x1e7c <__ZN11PointMasses6appendERKS_+0xa0>
    1e40: cb0002a9     	sub	x9, x21, x0
    1e44: f101013f     	cmp	x9, #64
    1e48: 540001a3     	b.lo	0x1e7c <__ZN11PointMasses6appendERKS_+0xa0>
    1e4c: 927d72c8     	and	x8, x22, #0xfffffff8
    1e50: 91008009     	add	x9, x0, #32
    1e54: 910082aa     	add	x10, x21, #32
    1e58: aa0803eb     	mov	x11, x8
    1e5c: ad7f0520     	ldp	q0, q1, [x9, #-32]
    1e60: acc20d22     	ldp	q2, q3, [x9], #64
    1e64: ad3f0540     	stp	q0, q1, [x10, #-32]
    1e68: ac820d42     	stp	q2, q3, [x10], #64
    1e6c: f100216b     	subs	x11, x11, #8
    1e70: 54ffff61     	b.ne	0x1e5c <__ZN11PointMasses6appendERKS_+0x80>
    1e74: eb16011f     	cmp	x8, x22
    1e78: 54000120     	b.eq	0x1e9c <__ZN11PointMasses6appendERKS_+0xc0>
    1e7c: cb0802c9     	sub	x9, x22, x8
    1e80: d37df10a     	lsl	x10, x8, #3
    1e84: 8b0a0008     	add	x8, x0, x10
    1e88: 8b0a02aa     	add	x10, x21, x10
    1e8c: f840850b     	ldr	x11, [x8], #8
    1e90: f800854b     	str	x11, [x10], #8
    1e94: f1000529     	subs	x9, x9, #1
    1e98: 54ffffa1     	b.ne	0x1e8c <__ZN11PointMasses6appendERKS_+0xb0>
    1e9c: b900029f     	str	wzr, [x20]
    1ea0: 14000003     	b	0x1eac <__ZN11PointMasses6appendERKS_+0xd0>
    1ea4: b900029f     	str	wzr, [x20]
    1ea8: b4000040     	cbz	x0, 0x1eb0 <__ZN11PointMasses6appendERKS_+0xd4>
    1eac: 94000000     	bl	0x1eac <__ZN11PointMasses6appendERKS_+0xd0>
    1eb0: f9000695     	str	x21, [x20, #8]
    1eb4: 29005e96     	stp	w22, w23, [x20]
    1eb8: b9400268     	ldr	w8, [x19]
    1ebc: 7100051f     	cmp	w8, #1
    1ec0: 540001ab     	b.lt	0x1ef4 <__ZN11PointMasses6appendERKS_+0x118>
    1ec4: d2800008     	mov	x8, #0
    1ec8: b9800289     	ldrsw	x9, [x20]
    1ecc: 1100052a     	add	w10, w9, #1
    1ed0: b900028a     	str	w10, [x20]
    1ed4: f940066a     	ldr	x10, [x19, #8]
    1ed8: f868794a     	ldr	x10, [x10, x8, lsl #3]
    1edc: f940068b     	ldr	x11, [x20, #8]
    1ee0: f829796a     	str	x10, [x11, x9, lsl #3]
    1ee4: 91000508     	add	x8, x8, #1
    1ee8: b9800269     	ldrsw	x9, [x19]
    1eec: eb09011f     	cmp	x8, x9
    1ef0: 54fffecb     	b.lt	0x1ec8 <__ZN11PointMasses6appendERKS_+0xec>
    1ef4: b9401268     	ldr	w8, [x19, #16]
    1ef8: 29422a96     	ldp	w22, w10, [x20, #16]
    1efc: 0b160109     	add	w9, w8, w22
    1f00: 7100055f     	cmp	w10, #1
    1f04: 7a49a148     	ccmp	w10, w9, #8, ge
    1f08: 5400058a     	b.ge	0x1fb8 <__ZN11PointMasses6appendERKS_+0x1dc>
    1f0c: 52800048     	mov	w8, #2
    1f10: 7100093f     	cmp	w9, #2
    1f14: 1a88c137     	csel	w23, w9, w8, gt
    1f18: d37e7ee0     	ubfiz	x0, x23, #2, #32
    1f1c: 94000000     	bl	0x1f1c <__ZN11PointMasses6appendERKS_+0x140>
    1f20: aa0003f5     	mov	x21, x0
    1f24: f9400e80     	ldr	x0, [x20, #24]
    1f28: 710006df     	cmp	w22, #1
    1f2c: 540003ab     	b.lt	0x1fa0 <__ZN11PointMasses6appendERKS_+0x1c4>
    1f30: d2800008     	mov	x8, #0
    1f34: 710042df     	cmp	w22, #16
    1f38: 54000203     	b.lo	0x1f78 <__ZN11PointMasses6appendERKS_+0x19c>
    1f3c: cb0002a9     	sub	x9, x21, x0
    1f40: f101013f     	cmp	x9, #64
    1f44: 540001a3     	b.lo	0x1f78 <__ZN11PointMasses6appendERKS_+0x19c>
    1f48: 927c6ec8     	and	x8, x22, #0xfffffff0
    1f4c: 91008009     	add	x9, x0, #32
    1f50: 910082aa     	add	x10, x21, #32
    1f54: aa0803eb     	mov	x11, x8
    1f58: ad7f0520     	ldp	q0, q1, [x9, #-32]
    1f5c: acc20d22     	ldp	q2, q3, [x9], #64
    1f60: ad3f0540     	stp	q0, q1, [x10, #-32]
    1f64: ac820d42     	stp	q2, q3, [x10], #64
    1f68: f100416b     	subs	x11, x11, #16
    1f6c: 54ffff61     	b.ne	0x1f58 <__ZN11PointMasses6appendERKS_+0x17c>
    1f70: eb16011f     	cmp	x8, x22
    1f74: 54000120     	b.eq	0x1f98 <__ZN11PointMasses6appendERKS_+0x1bc>
    1f78: cb0802c9     	sub	x9, x22, x8
    1f7c: d37ef50a     	lsl	x10, x8, #2
    1f80: 8b0a0008     	add	x8, x0, x10
    1f84: 8b0a02aa     	add	x10, x21, x10
    1f88: bc404500     	ldr	s0, [x8], #4
    1f8c: bc004540     	str	s0, [x10], #4
    1f90: f1000529     	subs	x9, x9, #1
    1f94: 54ffffa1     	b.ne	0x1f88 <__ZN11PointMasses6appendERKS_+0x1ac>
    1f98: b900129f     	str	wzr, [x20, #16]
    1f9c: 14000003     	b	0x1fa8 <__ZN11PointMasses6appendERKS_+0x1cc>
    1fa0: b900129f     	str	wzr, [x20, #16]
    1fa4: b4000040     	cbz	x0, 0x1fac <__ZN11PointMasses6appendERKS_+0x1d0>
    1fa8: 94000000     	bl	0x1fa8 <__ZN11PointMasses6appendERKS_+0x1cc>
    1fac: f9000e95     	str	x21, [x20, #24]
    1fb0: 29025e96     	stp	w22, w23, [x20, #16]
    1fb4: b9401268     	ldr	w8, [x19, #16]
    1fb8: 7100051f     	cmp	w8, #1
    1fbc: 5400020b     	b.lt	0x1ffc <__ZN11PointMasses6appendERKS_+0x220>
    1fc0: d2800008     	mov	x8, #0
    1fc4: 93407eca     	sxtw	x10, w22
    1fc8: f9400e8b     	ldr	x11, [x20, #24]
    1fcc: f9400e69     	ldr	x9, [x19, #24]
    1fd0: 8b0a096a     	add	x10, x11, x10, lsl #2
    1fd4: 110006cb     	add	w11, w22, #1
    1fd8: 0b08016c     	add	w12, w11, w8
    1fdc: d37ef50d     	lsl	x13, x8, #2
    1fe0: bc6d6920     	ldr	s0, [x9, x13]
    1fe4: b900128c     	str	w12, [x20, #16]
    1fe8: bc2d6940     	str	s0, [x10, x13]
    1fec: 91000508     	add	x8, x8, #1
    1ff0: b980126c     	ldrsw	x12, [x19, #16]
    1ff4: eb0c011f     	cmp	x8, x12
    1ff8: 54ffff0b     	b.lt	0x1fd8 <__ZN11PointMasses6appendERKS_+0x1fc>
    1ffc: b9402268     	ldr	w8, [x19, #32]
    2000: 29442a96     	ldp	w22, w10, [x20, #32]
    2004: 0b160109     	add	w9, w8, w22
    2008: 7100055f     	cmp	w10, #1
    200c: 7a49a148     	ccmp	w10, w9, #8, ge
    2010: 5400058a     	b.ge	0x20c0 <__ZN11PointMasses6appendERKS_+0x2e4>
    2014: 52800048     	mov	w8, #2
    2018: 7100093f     	cmp	w9, #2
    201c: 1a88c137     	csel	w23, w9, w8, gt
    2020: d37d7ee0     	ubfiz	x0, x23, #3, #32
    2024: 94000000     	bl	0x2024 <__ZN11PointMasses6appendERKS_+0x248>
    2028: aa0003f5     	mov	x21, x0
    202c: f9401680     	ldr	x0, [x20, #40]
    2030: 710006df     	cmp	w22, #1
    2034: 540003ab     	b.lt	0x20a8 <__ZN11PointMasses6appendERKS_+0x2cc>
    2038: d2800008     	mov	x8, #0
    203c: 710022df     	cmp	w22, #8
    2040: 54000203     	b.lo	0x2080 <__ZN11PointMasses6appendERKS_+0x2a4>
    2044: cb0002a9     	sub	x9, x21, x0
    2048: f101013f     	cmp	x9, #64
    204c: 540001a3     	b.lo	0x2080 <__ZN11PointMasses6appendERKS_+0x2a4>
    2050: 927d72c8     	and	x8, x22, #0xfffffff8
    2054: 91008009     	add	x9, x0, #32
    2058: 910082aa     	add	x10, x21, #32
    205c: aa0803eb     	mov	x11, x8
    2060: ad7f0520     	ldp	q0, q1, [x9, #-32]
    2064: acc20d22     	ldp	q2, q3, [x9], #64
    2068: ad3f0540     	stp	q0, q1, [x10, #-32]
    206c: ac820d42     	stp	q2, q3, [x10], #64
    2070: f100216b     	subs	x11, x11, #8
    2074: 54ffff61     	b.ne	0x2060 <__ZN11PointMasses6appendERKS_+0x284>
    2078: eb16011f     	cmp	x8, x22
    207c: 54000120     	b.eq	0x20a0 <__ZN11PointMasses6appendERKS_+0x2c4>
    2080: cb0802c9     	sub	x9, x22, x8
    2084: d37df10a     	lsl	x10, x8, #3
    2088: 8b0a0008     	add	x8, x0, x10
    208c: 8b0a02aa     	add	x10, x21, x10
    2090: f840850b     	ldr	x11, [x8], #8
    2094: f800854b     	str	x11, [x10], #8
    2098: f1000529     	subs	x9, x9, #1
    209c: 54ffffa1     	b.ne	0x2090 <__ZN11PointMasses6appendERKS_+0x2b4>
    20a0: b900229f     	str	wzr, [x20, #32]
    20a4: 14000003     	b	0x20b0 <__ZN11PointMasses6appendERKS_+0x2d4>
    20a8: b900229f     	str	wzr, [x20, #32]
    20ac: b4000040     	cbz	x0, 0x20b4 <__ZN11PointMasses6appendERKS_+0x2d8>
    20b0: 94000000     	bl	0x20b0 <__ZN11PointMasses6appendERKS_+0x2d4>
    20b4: f9001695     	str	x21, [x20, #40]
    20b8: 29045e96     	stp	w22, w23, [x20, #32]
    20bc: b9402268     	ldr	w8, [x19, #32]
    20c0: 7100051f     	cmp	w8, #1
    20c4: 540001ab     	b.lt	0x20f8 <__ZN11PointMasses6appendERKS_+0x31c>
    20c8: d2800008     	mov	x8, #0
    20cc: b9802289     	ldrsw	x9, [x20, #32]
    20d0: 1100052a     	add	w10, w9, #1
    20d4: b900228a     	str	w10, [x20, #32]
    20d8: f940166a     	ldr	x10, [x19, #40]
    20dc: f868794a     	ldr	x10, [x10, x8, lsl #3]
    20e0: f940168b     	ldr	x11, [x20, #40]
    20e4: f829796a     	str	x10, [x11, x9, lsl #3]
    20e8: 91000508     	add	x8, x8, #1
    20ec: b9802269     	ldrsw	x9, [x19, #32]
    20f0: eb09011f     	cmp	x8, x9
    20f4: 54fffecb     	b.lt	0x20cc <__ZN11PointMasses6appendERKS_+0x2f0>
    20f8: b9403268     	ldr	w8, [x19, #48]
    20fc: 29462a96     	ldp	w22, w10, [x20, #48]
    2100: 0b160109     	add	w9, w8, w22
    2104: 7100055f     	cmp	w10, #1
    2108: 7a49a148     	ccmp	w10, w9, #8, ge
    210c: 5400058a     	b.ge	0x21bc <__ZN11PointMasses6appendERKS_+0x3e0>
    2110: 52800048     	mov	w8, #2
    2114: 7100093f     	cmp	w9, #2
    2118: 1a88c137     	csel	w23, w9, w8, gt
    211c: d37d7ee0     	ubfiz	x0, x23, #3, #32
    2120: 94000000     	bl	0x2120 <__ZN11PointMasses6appendERKS_+0x344>
    2124: aa0003f5     	mov	x21, x0
    2128: f9401e80     	ldr	x0, [x20, #56]
    212c: 710006df     	cmp	w22, #1
    2130: 540003ab     	b.lt	0x21a4 <__ZN11PointMasses6appendERKS_+0x3c8>
    2134: d2800008     	mov	x8, #0
    2138: 710022df     	cmp	w22, #8
    213c: 54000203     	b.lo	0x217c <__ZN11PointMasses6appendERKS_+0x3a0>
    2140: cb0002a9     	sub	x9, x21, x0
    2144: f101013f     	cmp	x9, #64
    2148: 540001a3     	b.lo	0x217c <__ZN11PointMasses6appendERKS_+0x3a0>
    214c: 927d72c8     	and	x8, x22, #0xfffffff8
    2150: 91008009     	add	x9, x0, #32
    2154: 910082aa     	add	x10, x21, #32
    2158: aa0803eb     	mov	x11, x8
    215c: ad7f0520     	ldp	q0, q1, [x9, #-32]
    2160: acc20d22     	ldp	q2, q3, [x9], #64
    2164: ad3f0540     	stp	q0, q1, [x10, #-32]
    2168: ac820d42     	stp	q2, q3, [x10], #64
    216c: f100216b     	subs	x11, x11, #8
    2170: 54ffff61     	b.ne	0x215c <__ZN11PointMasses6appendERKS_+0x380>
    2174: eb16011f     	cmp	x8, x22
    2178: 54000120     	b.eq	0x219c <__ZN11PointMasses6appendERKS_+0x3c0>
    217c: cb0802c9     	sub	x9, x22, x8
    2180: d37df10a     	lsl	x10, x8, #3
    2184: 8b0a0008     	add	x8, x0, x10
    2188: 8b0a02aa     	add	x10, x21, x10
    218c: f840850b     	ldr	x11, [x8], #8
    2190: f800854b     	str	x11, [x10], #8
    2194: f1000529     	subs	x9, x9, #1
    2198: 54ffffa1     	b.ne	0x218c <__ZN11PointMasses6appendERKS_+0x3b0>
    219c: b900329f     	str	wzr, [x20, #48]
    21a0: 14000003     	b	0x21ac <__ZN11PointMasses6appendERKS_+0x3d0>
    21a4: b900329f     	str	wzr, [x20, #48]
    21a8: b4000040     	cbz	x0, 0x21b0 <__ZN11PointMasses6appendERKS_+0x3d4>
    21ac: 94000000     	bl	0x21ac <__ZN11PointMasses6appendERKS_+0x3d0>
    21b0: f9001e95     	str	x21, [x20, #56]
    21b4: 29065e96     	stp	w22, w23, [x20, #48]
    21b8: b9403268     	ldr	w8, [x19, #48]
    21bc: 7100051f     	cmp	w8, #1
    21c0: 540001ab     	b.lt	0x21f4 <__ZN11PointMasses6appendERKS_+0x418>
    21c4: d2800008     	mov	x8, #0
    21c8: b9803289     	ldrsw	x9, [x20, #48]
    21cc: 1100052a     	add	w10, w9, #1
    21d0: b900328a     	str	w10, [x20, #48]
    21d4: f9401e6a     	ldr	x10, [x19, #56]
    21d8: f868794a     	ldr	x10, [x10, x8, lsl #3]
    21dc: f9401e8b     	ldr	x11, [x20, #56]
    21e0: f829796a     	str	x10, [x11, x9, lsl #3]
    21e4: 91000508     	add	x8, x8, #1
    21e8: b9803269     	ldrsw	x9, [x19, #48]
    21ec: eb09011f     	cmp	x8, x9
    21f0: 54fffecb     	b.lt	0x21c8 <__ZN11PointMasses6appendERKS_+0x3ec>
    21f4: a9437bfd     	ldp	x29, x30, [sp, #48]
    21f8: a9424ff4     	ldp	x20, x19, [sp, #32]
    21fc: a94157f6     	ldp	x22, x21, [sp, #16]
    2200: a8c45ff8     	ldp	x24, x23, [sp], #64
    2204: d65f03c0     	ret

0000000000002208 <__ZN4Game13rewindHistoryEv>:
    2208: f9400008     	ldr	x8, [x0]
    220c: 52968089     	mov	w9, #46084
    2210: b869690a     	ldr	w10, [x8, x9]
    2214: 7100054a     	subs	w10, w10, #1
    2218: 52801deb     	mov	w11, #239
    221c: 1a8ab16a     	csel	w10, w11, w10, lt
    2220: b829690a     	str	w10, [x8, x9]
    2224: 14000000     	b	0x2224 <__ZN4Game13rewindHistoryEv+0x1c>

0000000000002228 <__ZN4Game14forwardHistoryEv>:
    2228: f9400008     	ldr	x8, [x0]
    222c: 52968009     	mov	w9, #46080
    2230: 8b090108     	add	x8, x8, x9
    2234: 2940250a     	ldp	w10, w9, [x8]
    2238: 5100054a     	sub	w10, w10, #1
    223c: 6b0a013f     	cmp	w9, w10
    2240: 54000041     	b.ne	0x2248 <__ZN4Game14forwardHistoryEv+0x20>
    2244: d65f03c0     	ret
    2248: 11000529     	add	w9, w9, #1
    224c: 5291112a     	mov	w10, #34953
    2250: 72b1110a     	movk	w10, #34952, lsl #16
    2254: 9b2a7d2a     	smull	x10, w9, w10
    2258: d360fd4a     	lsr	x10, x10, #32
    225c: 0b09014a     	add	w10, w10, w9
    2260: 13077d4b     	asr	w11, w10, #7
    2264: 0b4a7d6a     	add	w10, w11, w10, lsr #31
    2268: 52801e0b     	mov	w11, #240
    226c: 1b0ba549     	msub	w9, w10, w11, w9
    2270: b9000509     	str	w9, [x8, #4]
    2274: 14000000     	b	0x2274 <__ZN4Game14forwardHistoryEv+0x4c>

0000000000002278 <__ZN4Game17setGravityEnabledEb>:
    2278: f9400008     	ldr	x8, [x0]
    227c: 5296b689     	mov	w9, #46516
    2280: 38296901     	strb	w1, [x8, x9]
    2284: d65f03c0     	ret

0000000000002288 <__ZN4Game20setCollisionsEnabledEb>:
    2288: f9400008     	ldr	x8, [x0]
    228c: 5296b6a9     	mov	w9, #46517
    2290: 38296901     	strb	w1, [x8, x9]
    2294: d65f03c0     	ret

0000000000002298 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo>:
    2298: 6db733ed     	stp	d13, d12, [sp, #-144]!
    229c: 6d012beb     	stp	d11, d10, [sp, #16]
    22a0: 6d0223e9     	stp	d9, d8, [sp, #32]
    22a4: a9036ffc     	stp	x28, x27, [sp, #48]
    22a8: a90467fa     	stp	x26, x25, [sp, #64]
    22ac: a9055ff8     	stp	x24, x23, [sp, #80]
    22b0: a90657f6     	stp	x22, x21, [sp, #96]
    22b4: a9074ff4     	stp	x20, x19, [sp, #112]
    22b8: a9087bfd     	stp	x29, x30, [sp, #128]
    22bc: 910203fd     	add	x29, sp, #128
    22c0: aa0403f3     	mov	x19, x4
    22c4: aa0303f4     	mov	x20, x3
    22c8: 1e604008     	fmov	d8, d0
    22cc: aa0103f5     	mov	x21, x1
    22d0: aa0003f6     	mov	x22, x0
    22d4: f9400008     	ldr	x8, [x0]
    22d8: 5296ab09     	mov	w9, #46424
    22dc: 8b090118     	add	x24, x8, x9
    22e0: b9400308     	ldr	w8, [x24]
    22e4: b9400839     	ldr	w25, [x1, #8]
    22e8: 6b19011f     	cmp	w8, w25
    22ec: 54000f20     	b.eq	0x24d0 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x238>
    22f0: b9400708     	ldr	w8, [x24, #4]
    22f4: 7100051f     	cmp	w8, #1
    22f8: 7a59a108     	ccmp	w8, w25, #8, ge
    22fc: 5400058a     	b.ge	0x23ac <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x114>
    2300: 52800048     	mov	w8, #2
    2304: 71000b3f     	cmp	w25, #2
    2308: 1a88c33a     	csel	w26, w25, w8, gt
    230c: d37d7f40     	ubfiz	x0, x26, #3, #32
    2310: 94000000     	bl	0x2310 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x78>
    2314: aa0003f7     	mov	x23, x0
    2318: b9400308     	ldr	w8, [x24]
    231c: f9400700     	ldr	x0, [x24, #8]
    2320: 7100051f     	cmp	w8, #1
    2324: 540003ab     	b.lt	0x2398 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x100>
    2328: d2800009     	mov	x9, #0
    232c: 7100211f     	cmp	w8, #8
    2330: 54000203     	b.lo	0x2370 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0xd8>
    2334: cb0002ea     	sub	x10, x23, x0
    2338: f101015f     	cmp	x10, #64
    233c: 540001a3     	b.lo	0x2370 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0xd8>
    2340: 927d7109     	and	x9, x8, #0xfffffff8
    2344: 9100800a     	add	x10, x0, #32
    2348: 910082eb     	add	x11, x23, #32
    234c: aa0903ec     	mov	x12, x9
    2350: ad7f0540     	ldp	q0, q1, [x10, #-32]
    2354: acc20d42     	ldp	q2, q3, [x10], #64
    2358: ad3f0560     	stp	q0, q1, [x11, #-32]
    235c: ac820d62     	stp	q2, q3, [x11], #64
    2360: f100218c     	subs	x12, x12, #8
    2364: 54ffff61     	b.ne	0x2350 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0xb8>
    2368: eb08013f     	cmp	x9, x8
    236c: 54000120     	b.eq	0x2390 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0xf8>
    2370: cb090108     	sub	x8, x8, x9
    2374: d37df12a     	lsl	x10, x9, #3
    2378: 8b0a0009     	add	x9, x0, x10
    237c: 8b0a02ea     	add	x10, x23, x10
    2380: f840852b     	ldr	x11, [x9], #8
    2384: f800854b     	str	x11, [x10], #8
    2388: f1000508     	subs	x8, x8, #1
    238c: 54ffffa1     	b.ne	0x2380 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0xe8>
    2390: b900031f     	str	wzr, [x24]
    2394: 14000003     	b	0x23a0 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x108>
    2398: b900031f     	str	wzr, [x24]
    239c: b4000040     	cbz	x0, 0x23a4 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x10c>
    23a0: 94000000     	bl	0x23a0 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x108>
    23a4: f9000717     	str	x23, [x24, #8]
    23a8: b900071a     	str	w26, [x24, #4]
    23ac: b9000319     	str	w25, [x24]
    23b0: 7100073f     	cmp	w25, #1
    23b4: 5400012b     	b.lt	0x23d8 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x140>
    23b8: d2800008     	mov	x8, #0
    23bc: f9400709     	ldr	x9, [x24, #8]
    23c0: 8b080d29     	add	x9, x9, x8, lsl #3
    23c4: f900013f     	str	xzr, [x9]
    23c8: 91000508     	add	x8, x8, #1
    23cc: b9800309     	ldrsw	x9, [x24]
    23d0: eb09011f     	cmp	x8, x9
    23d4: 54ffff4b     	b.lt	0x23bc <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x124>
    23d8: f94002c8     	ldr	x8, [x22]
    23dc: 5296af09     	mov	w9, #46456
    23e0: 8b090118     	add	x24, x8, x9
    23e4: b9400ab9     	ldr	w25, [x21, #8]
    23e8: b9400708     	ldr	w8, [x24, #4]
    23ec: 7100051f     	cmp	w8, #1
    23f0: 7a59a108     	ccmp	w8, w25, #8, ge
    23f4: 5400058a     	b.ge	0x24a4 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x20c>
    23f8: 52800048     	mov	w8, #2
    23fc: 71000b3f     	cmp	w25, #2
    2400: 1a88c33a     	csel	w26, w25, w8, gt
    2404: d37d7f40     	ubfiz	x0, x26, #3, #32
    2408: 94000000     	bl	0x2408 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x170>
    240c: aa0003f7     	mov	x23, x0
    2410: b9400308     	ldr	w8, [x24]
    2414: f9400700     	ldr	x0, [x24, #8]
    2418: 7100051f     	cmp	w8, #1
    241c: 540003ab     	b.lt	0x2490 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x1f8>
    2420: d2800009     	mov	x9, #0
    2424: 7100211f     	cmp	w8, #8
    2428: 54000203     	b.lo	0x2468 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x1d0>
    242c: cb0002ea     	sub	x10, x23, x0
    2430: f101015f     	cmp	x10, #64
    2434: 540001a3     	b.lo	0x2468 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x1d0>
    2438: 927d7109     	and	x9, x8, #0xfffffff8
    243c: 9100800a     	add	x10, x0, #32
    2440: 910082eb     	add	x11, x23, #32
    2444: aa0903ec     	mov	x12, x9
    2448: ad7f0540     	ldp	q0, q1, [x10, #-32]
    244c: acc20d42     	ldp	q2, q3, [x10], #64
    2450: ad3f0560     	stp	q0, q1, [x11, #-32]
    2454: ac820d62     	stp	q2, q3, [x11], #64
    2458: f100218c     	subs	x12, x12, #8
    245c: 54ffff61     	b.ne	0x2448 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x1b0>
    2460: eb08013f     	cmp	x9, x8
    2464: 54000120     	b.eq	0x2488 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x1f0>
    2468: cb090108     	sub	x8, x8, x9
    246c: d37df12a     	lsl	x10, x9, #3
    2470: 8b0a0009     	add	x9, x0, x10
    2474: 8b0a02ea     	add	x10, x23, x10
    2478: f840852b     	ldr	x11, [x9], #8
    247c: f800854b     	str	x11, [x10], #8
    2480: f1000508     	subs	x8, x8, #1
    2484: 54ffffa1     	b.ne	0x2478 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x1e0>
    2488: b900031f     	str	wzr, [x24]
    248c: 14000003     	b	0x2498 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x200>
    2490: b900031f     	str	wzr, [x24]
    2494: b4000040     	cbz	x0, 0x249c <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x204>
    2498: 94000000     	bl	0x2498 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x200>
    249c: f9000717     	str	x23, [x24, #8]
    24a0: b900071a     	str	w26, [x24, #4]
    24a4: b9000319     	str	w25, [x24]
    24a8: 7100073f     	cmp	w25, #1
    24ac: 5400012b     	b.lt	0x24d0 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x238>
    24b0: d2800008     	mov	x8, #0
    24b4: f9400709     	ldr	x9, [x24, #8]
    24b8: 8b080d29     	add	x9, x9, x8, lsl #3
    24bc: f900013f     	str	xzr, [x9]
    24c0: 91000508     	add	x8, x8, #1
    24c4: b9800309     	ldrsw	x9, [x24]
    24c8: eb09011f     	cmp	x8, x9
    24cc: 54ffff4b     	b.lt	0x24b4 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x21c>
    24d0: f94002c8     	ldr	x8, [x22]
    24d4: 5296ad09     	mov	w9, #46440
    24d8: 8b090118     	add	x24, x8, x9
    24dc: b9401abb     	ldr	w27, [x21, #24]
    24e0: b9400708     	ldr	w8, [x24, #4]
    24e4: 7100051f     	cmp	w8, #1
    24e8: 7a5ba108     	ccmp	w8, w27, #8, ge
    24ec: 540005aa     	b.ge	0x25a0 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x308>
    24f0: 52800048     	mov	w8, #2
    24f4: 71000b7f     	cmp	w27, #2
    24f8: 1a88c379     	csel	w25, w27, w8, gt
    24fc: d37e7f20     	ubfiz	x0, x25, #2, #32
    2500: 94000000     	bl	0x2500 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x268>
    2504: aa0003f7     	mov	x23, x0
    2508: b940031a     	ldr	w26, [x24]
    250c: f9400700     	ldr	x0, [x24, #8]
    2510: 7100075f     	cmp	w26, #1
    2514: 540003ab     	b.lt	0x2588 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x2f0>
    2518: d2800008     	mov	x8, #0
    251c: 7100435f     	cmp	w26, #16
    2520: 54000203     	b.lo	0x2560 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x2c8>
    2524: cb0002e9     	sub	x9, x23, x0
    2528: f101013f     	cmp	x9, #64
    252c: 540001a3     	b.lo	0x2560 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x2c8>
    2530: 927c6f48     	and	x8, x26, #0xfffffff0
    2534: 91008009     	add	x9, x0, #32
    2538: 910082ea     	add	x10, x23, #32
    253c: aa0803eb     	mov	x11, x8
    2540: ad7f0520     	ldp	q0, q1, [x9, #-32]
    2544: acc20d22     	ldp	q2, q3, [x9], #64
    2548: ad3f0540     	stp	q0, q1, [x10, #-32]
    254c: ac820d42     	stp	q2, q3, [x10], #64
    2550: f100416b     	subs	x11, x11, #16
    2554: 54ffff61     	b.ne	0x2540 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x2a8>
    2558: eb1a011f     	cmp	x8, x26
    255c: 54000120     	b.eq	0x2580 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x2e8>
    2560: cb080349     	sub	x9, x26, x8
    2564: d37ef50a     	lsl	x10, x8, #2
    2568: 8b0a0008     	add	x8, x0, x10
    256c: 8b0a02ea     	add	x10, x23, x10
    2570: bc404500     	ldr	s0, [x8], #4
    2574: bc004540     	str	s0, [x10], #4
    2578: f1000529     	subs	x9, x9, #1
    257c: 54ffffa1     	b.ne	0x2570 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x2d8>
    2580: b900031f     	str	wzr, [x24]
    2584: 14000003     	b	0x2590 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x2f8>
    2588: b900031f     	str	wzr, [x24]
    258c: b4000060     	cbz	x0, 0x2598 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x300>
    2590: 94000000     	bl	0x2590 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x2f8>
    2594: b9401abb     	ldr	w27, [x21, #24]
    2598: f9000717     	str	x23, [x24, #8]
    259c: 2900671a     	stp	w26, w25, [x24]
    25a0: 7100077f     	cmp	w27, #1
    25a4: 540003cb     	b.lt	0x261c <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x384>
    25a8: d280000a     	mov	x10, #0
    25ac: f9400708     	ldr	x8, [x24, #8]
    25b0: f9400aa9     	ldr	x9, [x21, #16]
    25b4: 2a1b03eb     	mov	w11, w27
    25b8: 7100437f     	cmp	w27, #16
    25bc: 54000203     	b.lo	0x25fc <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x364>
    25c0: cb09010c     	sub	x12, x8, x9
    25c4: f101019f     	cmp	x12, #64
    25c8: 540001a3     	b.lo	0x25fc <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x364>
    25cc: 927c6d6a     	and	x10, x11, #0xfffffff0
    25d0: 9100812c     	add	x12, x9, #32
    25d4: 9100810d     	add	x13, x8, #32
    25d8: aa0a03ee     	mov	x14, x10
    25dc: ad7f0580     	ldp	q0, q1, [x12, #-32]
    25e0: acc20d82     	ldp	q2, q3, [x12], #64
    25e4: ad3f05a0     	stp	q0, q1, [x13, #-32]
    25e8: ac820da2     	stp	q2, q3, [x13], #64
    25ec: f10041ce     	subs	x14, x14, #16
    25f0: 54ffff61     	b.ne	0x25dc <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x344>
    25f4: eb0b015f     	cmp	x10, x11
    25f8: 54000120     	b.eq	0x261c <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x384>
    25fc: cb0a016b     	sub	x11, x11, x10
    2600: d37ef54a     	lsl	x10, x10, #2
    2604: 8b0a0129     	add	x9, x9, x10
    2608: 8b0a0108     	add	x8, x8, x10
    260c: bc404520     	ldr	s0, [x9], #4
    2610: bc004500     	str	s0, [x8], #4
    2614: f100056b     	subs	x11, x11, #1
    2618: 54ffffa1     	b.ne	0x260c <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x374>
    261c: f94002c8     	ldr	x8, [x22]
    2620: 5296b109     	mov	w9, #46472
    2624: 8b090118     	add	x24, x8, x9
    2628: b9400ab9     	ldr	w25, [x21, #8]
    262c: b9400708     	ldr	w8, [x24, #4]
    2630: 7100051f     	cmp	w8, #1
    2634: 7a59a108     	ccmp	w8, w25, #8, ge
    2638: 5400058a     	b.ge	0x26e8 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x450>
    263c: 52800048     	mov	w8, #2
    2640: 71000b3f     	cmp	w25, #2
    2644: 1a88c33a     	csel	w26, w25, w8, gt
    2648: d37d7f40     	ubfiz	x0, x26, #3, #32
    264c: 94000000     	bl	0x264c <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x3b4>
    2650: aa0003f7     	mov	x23, x0
    2654: b9400308     	ldr	w8, [x24]
    2658: f9400700     	ldr	x0, [x24, #8]
    265c: 7100051f     	cmp	w8, #1
    2660: 540003ab     	b.lt	0x26d4 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x43c>
    2664: d2800009     	mov	x9, #0
    2668: 7100211f     	cmp	w8, #8
    266c: 54000203     	b.lo	0x26ac <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x414>
    2670: cb0002ea     	sub	x10, x23, x0
    2674: f101015f     	cmp	x10, #64
    2678: 540001a3     	b.lo	0x26ac <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x414>
    267c: 927d7109     	and	x9, x8, #0xfffffff8
    2680: 9100800a     	add	x10, x0, #32
    2684: 910082eb     	add	x11, x23, #32
    2688: aa0903ec     	mov	x12, x9
    268c: ad7f0540     	ldp	q0, q1, [x10, #-32]
    2690: acc20d42     	ldp	q2, q3, [x10], #64
    2694: ad3f0560     	stp	q0, q1, [x11, #-32]
    2698: ac820d62     	stp	q2, q3, [x11], #64
    269c: f100218c     	subs	x12, x12, #8
    26a0: 54ffff61     	b.ne	0x268c <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x3f4>
    26a4: eb08013f     	cmp	x9, x8
    26a8: 54000120     	b.eq	0x26cc <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x434>
    26ac: cb090108     	sub	x8, x8, x9
    26b0: d37df12a     	lsl	x10, x9, #3
    26b4: 8b0a0009     	add	x9, x0, x10
    26b8: 8b0a02ea     	add	x10, x23, x10
    26bc: f840852b     	ldr	x11, [x9], #8
    26c0: f800854b     	str	x11, [x10], #8
    26c4: f1000508     	subs	x8, x8, #1
    26c8: 54ffffa1     	b.ne	0x26bc <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x424>
    26cc: b900031f     	str	wzr, [x24]
    26d0: 14000003     	b	0x26dc <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x444>
    26d4: b900031f     	str	wzr, [x24]
    26d8: b4000040     	cbz	x0, 0x26e0 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x448>
    26dc: 94000000     	bl	0x26dc <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x444>
    26e0: f9000717     	str	x23, [x24, #8]
    26e4: b900071a     	str	w26, [x24, #4]
    26e8: b9000319     	str	w25, [x24]
    26ec: 7100073f     	cmp	w25, #1
    26f0: 5400012b     	b.lt	0x2714 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x47c>
    26f4: d2800008     	mov	x8, #0
    26f8: f9400709     	ldr	x9, [x24, #8]
    26fc: 8b080d29     	add	x9, x9, x8, lsl #3
    2700: f900013f     	str	xzr, [x9]
    2704: 91000508     	add	x8, x8, #1
    2708: b9800309     	ldrsw	x9, [x24]
    270c: eb09011f     	cmp	x8, x9
    2710: 54ffff4b     	b.lt	0x26f8 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x460>
    2714: 94000000     	bl	0x2714 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x47c>
    2718: 1e204009     	fmov	s9, s0
    271c: 1e20402a     	fmov	s10, s1
    2720: 94000000     	bl	0x2720 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x488>
    2724: f94002c8     	ldr	x8, [x22]
    2728: 5296b689     	mov	w9, #46516
    272c: 38696908     	ldrb	w8, [x8, x9]
    2730: 7100011f     	cmp	w8, #0
    2734: 2f00e402     	movi	d2, #0000000000000000
    2738: 1e220c0b     	fcsel	s11, s0, s2, eq
    273c: 52892a48     	mov	w8, #18770
    2740: 72a723a8     	movk	w8, #14621, lsl #16
    2744: 1e270100     	fmov	s0, w8
    2748: 1e200c2c     	fcsel	s12, s1, s0, eq
    274c: b9400ab8     	ldr	w24, [x21, #8]
    2750: b9400668     	ldr	w8, [x19, #4]
    2754: 7100051f     	cmp	w8, #1
    2758: 7a58a108     	ccmp	w8, w24, #8, ge
    275c: 5400030a     	b.ge	0x27bc <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x524>
    2760: 52800048     	mov	w8, #2
    2764: 71000b1f     	cmp	w24, #2
    2768: 1a88c319     	csel	w25, w24, w8, gt
    276c: d37c7f20     	ubfiz	x0, x25, #4, #32
    2770: 94000000     	bl	0x2770 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x4d8>
    2774: aa0003f7     	mov	x23, x0
    2778: b9400268     	ldr	w8, [x19]
    277c: f9400660     	ldr	x0, [x19, #8]
    2780: 7100051f     	cmp	w8, #1
    2784: 5400012b     	b.lt	0x27a8 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x510>
    2788: aa1703e9     	mov	x9, x23
    278c: aa0003ea     	mov	x10, x0
    2790: 3cc10540     	ldr	q0, [x10], #16
    2794: 3c810520     	str	q0, [x9], #16
    2798: f1000508     	subs	x8, x8, #1
    279c: 54ffffa1     	b.ne	0x2790 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x4f8>
    27a0: b900027f     	str	wzr, [x19]
    27a4: 14000003     	b	0x27b0 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x518>
    27a8: b900027f     	str	wzr, [x19]
    27ac: b4000040     	cbz	x0, 0x27b4 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x51c>
    27b0: 94000000     	bl	0x27b0 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x518>
    27b4: f9000677     	str	x23, [x19, #8]
    27b8: b9000679     	str	w25, [x19, #4]
    27bc: b9000278     	str	w24, [x19]
    27c0: 7100071f     	cmp	w24, #1
    27c4: 5400018b     	b.lt	0x27f4 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x55c>
    27c8: d2800008     	mov	x8, #0
    27cc: d2800009     	mov	x9, #0
    27d0: f940066a     	ldr	x10, [x19, #8]
    27d4: 8b08014a     	add	x10, x10, x8
    27d8: 2d002949     	stp	s9, s10, [x10]
    27dc: 2d01314b     	stp	s11, s12, [x10, #8]
    27e0: 91000529     	add	x9, x9, #1
    27e4: b980026a     	ldrsw	x10, [x19]
    27e8: 91004108     	add	x8, x8, #16
    27ec: eb0a013f     	cmp	x9, x10
    27f0: 54ffff0b     	b.lt	0x27d0 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x538>
    27f4: b9800aa8     	ldrsw	x8, [x21, #8]
    27f8: 34001148     	cbz	w8, 0x2a20 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x788>
    27fc: f94002c9     	ldr	x9, [x22]
    2800: 5296ac0a     	mov	w10, #46432
    2804: 8b0a012c     	add	x12, x9, x10
    2808: f9400189     	ldr	x9, [x12]
    280c: 1e624100     	fcvt	s0, d8
    2810: f94012aa     	ldr	x10, [x21, #32]
    2814: f94002ab     	ldr	x11, [x21]
    2818: f940068d     	ldr	x13, [x20, #8]
    281c: f9400661     	ldr	x1, [x19, #8]
    2820: f9401190     	ldr	x16, [x12, #32]
    2824: 92fc000c     	mov	x12, #2305843009213693951
    2828: 8b0c010e     	add	x14, x8, x12
    282c: 9240f1cc     	and	x12, x14, #0x1fffffffffffffff
    2830: f1003d9f     	cmp	x12, #15
    2834: 54000ca3     	b.lo	0x29c8 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x730>
    2838: d37cedcf     	lsl	x15, x14, #4
    283c: 8b0f0031     	add	x17, x1, x15
    2840: 91002234     	add	x20, x17, #8
    2844: d37df1ce     	lsl	x14, x14, #3
    2848: 8b0e0131     	add	x17, x9, x14
    284c: 91002235     	add	x21, x17, #8
    2850: 8b0e0211     	add	x17, x16, x14
    2854: 91002233     	add	x19, x17, #8
    2858: 8b0f01af     	add	x15, x13, x15
    285c: 910041e6     	add	x6, x15, #16
    2860: 8b0e014f     	add	x15, x10, x14
    2864: 910021e7     	add	x7, x15, #8
    2868: 8b0e016e     	add	x14, x11, x14
    286c: 910021d6     	add	x22, x14, #8
    2870: eb13003f     	cmp	x1, x19
    2874: fa543202     	ccmp	x16, x20, #2, lo
    2878: 1a9f27ee     	cset	w14, lo
    287c: eb06003f     	cmp	x1, x6
    2880: fa5431a2     	ccmp	x13, x20, #2, lo
    2884: 1a9f27ef     	cset	w15, lo
    2888: eb07003f     	cmp	x1, x7
    288c: fa543142     	ccmp	x10, x20, #2, lo
    2890: 1a9f27f1     	cset	w17, lo
    2894: eb16003f     	cmp	x1, x22
    2898: fa543162     	ccmp	x11, x20, #2, lo
    289c: 1a9f27e0     	cset	w0, lo
    28a0: eb13013f     	cmp	x9, x19
    28a4: fa553202     	ccmp	x16, x21, #2, lo
    28a8: 1a9f27e2     	cset	w2, lo
    28ac: eb06013f     	cmp	x9, x6
    28b0: fa5531a2     	ccmp	x13, x21, #2, lo
    28b4: 1a9f27e3     	cset	w3, lo
    28b8: eb07013f     	cmp	x9, x7
    28bc: fa553142     	ccmp	x10, x21, #2, lo
    28c0: 1a9f27e4     	cset	w4, lo
    28c4: eb16013f     	cmp	x9, x22
    28c8: fa553162     	ccmp	x11, x21, #2, lo
    28cc: 1a9f27e5     	cset	w5, lo
    28d0: eb06021f     	cmp	x16, x6
    28d4: fa5331a2     	ccmp	x13, x19, #2, lo
    28d8: 1a9f27e6     	cset	w6, lo
    28dc: eb07021f     	cmp	x16, x7
    28e0: fa533142     	ccmp	x10, x19, #2, lo
    28e4: 1a9f27e7     	cset	w7, lo
    28e8: eb16021f     	cmp	x16, x22
    28ec: fa533162     	ccmp	x11, x19, #2, lo
    28f0: 1a9f27f3     	cset	w19, lo
    28f4: eb14013f     	cmp	x9, x20
    28f8: fa553022     	ccmp	x1, x21, #2, lo
    28fc: 54000663     	b.lo	0x29c8 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x730>
    2900: 3700064e     	tbnz	w14, #0, 0x29c8 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x730>
    2904: 3700062f     	tbnz	w15, #0, 0x29c8 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x730>
    2908: 37000611     	tbnz	w17, #0, 0x29c8 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x730>
    290c: 370005e0     	tbnz	w0, #0, 0x29c8 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x730>
    2910: 370005c2     	tbnz	w2, #0, 0x29c8 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x730>
    2914: 370005a3     	tbnz	w3, #0, 0x29c8 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x730>
    2918: 37000584     	tbnz	w4, #0, 0x29c8 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x730>
    291c: 37000565     	tbnz	w5, #0, 0x29c8 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x730>
    2920: 37000546     	tbnz	w6, #0, 0x29c8 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x730>
    2924: 37000527     	tbnz	w7, #0, 0x29c8 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x730>
    2928: 37000513     	tbnz	w19, #0, 0x29c8 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x730>
    292c: 91000583     	add	x3, x12, #1
    2930: 927eec64     	and	x4, x3, #0x3ffffffffffffffc
    2934: d37df082     	lsl	x2, x4, #3
    2938: 8b02012c     	add	x12, x9, x2
    293c: 8b02020e     	add	x14, x16, x2
    2940: d37cec91     	lsl	x17, x4, #4
    2944: 8b11002f     	add	x15, x1, x17
    2948: 8b1101b1     	add	x17, x13, x17
    294c: 8b020160     	add	x0, x11, x2
    2950: 8b020142     	add	x2, x10, x2
    2954: 91008021     	add	x1, x1, #32
    2958: aa0903e5     	mov	x5, x9
    295c: aa0403e6     	mov	x6, x4
    2960: 4cdf09a1     	ld4.4s	{ v1, v2, v3, v4 }, [x13], #64
    2964: acc11546     	ldp	q6, q5, [x10], #32
    2968: 4cdf8970     	ld2.4s	{ v16, v17 }, [x11], #32
    296c: d1004027     	sub	x7, x1, #16
    2970: 91004033     	add	x19, x1, #16
    2974: fc1e0026     	stur	d6, [x1, #-32]
    2978: 4d0084e6     	st1.d	{ v6 }[1], [x7]
    297c: fc040425     	str	d5, [x1], #64
    2980: 4d008665     	st1.d	{ v5 }[1], [x19]
    2984: 4f809027     	fmul.4s	v7, v1, v0[0]
    2988: 4f809052     	fmul.4s	v18, v2, v0[0]
    298c: 4e30d4f3     	fadd.4s	v19, v7, v16
    2990: 4e31d654     	fadd.4s	v20, v18, v17
    2994: 4c9f88b3     	st2.4s	{ v19, v20 }, [x5], #32
    2998: 4e8518c7     	uzp1.4s	v7, v6, v5
    299c: 4e8558c5     	uzp2.4s	v5, v6, v5
    29a0: 4f809066     	fmul.4s	v6, v3, v0[0]
    29a4: 4f809081     	fmul.4s	v1, v4, v0[0]
    29a8: 4e27d4c2     	fadd.4s	v2, v6, v7
    29ac: 4e25d423     	fadd.4s	v3, v1, v5
    29b0: 4c9f8a02     	st2.4s	{ v2, v3 }, [x16], #32
    29b4: f10010c6     	subs	x6, x6, #4
    29b8: 54fffd41     	b.ne	0x2960 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x6c8>
    29bc: eb04007f     	cmp	x3, x4
    29c0: 54000101     	b.ne	0x29e0 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x748>
    29c4: 14000017     	b	0x2a20 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x788>
    29c8: aa0903ec     	mov	x12, x9
    29cc: aa1003ee     	mov	x14, x16
    29d0: aa0103ef     	mov	x15, x1
    29d4: aa0d03f1     	mov	x17, x13
    29d8: aa0b03e0     	mov	x0, x11
    29dc: aa0a03e2     	mov	x2, x10
    29e0: 8b080d28     	add	x8, x9, x8, lsl #3
    29e4: aa0203e9     	mov	x9, x2
    29e8: fc408521     	ldr	d1, [x9], #8
    29ec: f940004a     	ldr	x10, [x2]
    29f0: fc408402     	ldr	d2, [x0], #8
    29f4: 6cc11223     	ldp	d3, d4, [x17], #16
    29f8: f80105ea     	str	x10, [x15], #16
    29fc: 0f809063     	fmul.2s	v3, v3, v0[0]
    2a00: 0e22d462     	fadd.2s	v2, v3, v2
    2a04: fc008582     	str	d2, [x12], #8
    2a08: 0f809082     	fmul.2s	v2, v4, v0[0]
    2a0c: 0e21d441     	fadd.2s	v1, v2, v1
    2a10: fc0085c1     	str	d1, [x14], #8
    2a14: aa0903e2     	mov	x2, x9
    2a18: eb08019f     	cmp	x12, x8
    2a1c: 54fffe61     	b.ne	0x29e8 <__ZN4Game14prepareRK4StepER16PointMassesRangeR5RangeI6SpringEdR5ArrayI15PointDerivativeES9_R18ConsoleProfileInfo+0x750>
    2a20: a9487bfd     	ldp	x29, x30, [sp, #128]
    2a24: a9474ff4     	ldp	x20, x19, [sp, #112]
    2a28: a94657f6     	ldp	x22, x21, [sp, #96]
    2a2c: a9455ff8     	ldp	x24, x23, [sp, #80]
    2a30: a94467fa     	ldp	x26, x25, [sp, #64]
    2a34: a9436ffc     	ldp	x28, x27, [sp, #48]
    2a38: 6d4223e9     	ldp	d9, d8, [sp, #32]
    2a3c: 6d412beb     	ldp	d11, d10, [sp, #16]
    2a40: 6cc933ed     	ldp	d13, d12, [sp], #144
    2a44: d65f03c0     	ret

0000000000002a48 <__ZN4Game16updateRK4SpringsER5RangeI6SpringER5ArrayI15PointDerivativeER18ConsoleProfileInfo>:
    2a48: d10143ff     	sub	sp, sp, #80
    2a4c: a9047bfd     	stp	x29, x30, [sp, #64]
    2a50: 910103fd     	add	x29, sp, #64
    2a54: aa0303e5     	mov	x5, x3
    2a58: aa0103e8     	mov	x8, x1
    2a5c: f9400009     	ldr	x9, [x0]
    2a60: 5296ab0a     	mov	w10, #46424
    2a64: 8b0a0129     	add	x9, x9, x10
    2a68: b940012a     	ldr	w10, [x9]
    2a6c: f940052b     	ldr	x11, [x9, #8]
    2a70: a9002beb     	stp	x11, x10, [sp]
    2a74: f9400d2b     	ldr	x11, [x9, #24]
    2a78: a9012beb     	stp	x11, x10, [sp, #16]
    2a7c: f940152b     	ldr	x11, [x9, #40]
    2a80: a9022beb     	stp	x11, x10, [sp, #32]
    2a84: f9401d29     	ldr	x9, [x9, #56]
    2a88: a9032be9     	stp	x9, x10, [sp, #48]
    2a8c: b9400044     	ldr	w4, [x2]
    2a90: f9400443     	ldr	x3, [x2, #8]
    2a94: 910003e1     	mov	x1, sp
    2a98: aa0803e2     	mov	x2, x8
    2a9c: 94000000     	bl	0x2a9c <__ZN4Game16updateRK4SpringsER5RangeI6SpringER5ArrayI15PointDerivativeER18ConsoleProfileInfo+0x54>
    2aa0: a9447bfd     	ldp	x29, x30, [sp, #64]
    2aa4: 910143ff     	add	sp, sp, #80
    2aa8: d65f03c0     	ret

0000000000002aac <__ZN4Game32performThreadedSpringDerivativesER16PointMassesRangeR5RangeI6SpringES2_I15PointDerivativeER18ConsoleProfileInfo>:
    2aac: a9bc5ff8     	stp	x24, x23, [sp, #-64]!
    2ab0: a90157f6     	stp	x22, x21, [sp, #16]
    2ab4: a9024ff4     	stp	x20, x19, [sp, #32]
    2ab8: a9037bfd     	stp	x29, x30, [sp, #48]
    2abc: 9100c3fd     	add	x29, sp, #48
    2ac0: d10043ff     	sub	sp, sp, #16
    2ac4: aa0503f3     	mov	x19, x5
    2ac8: aa0403f4     	mov	x20, x4
    2acc: aa0303f5     	mov	x21, x3
    2ad0: aa0203f7     	mov	x23, x2
    2ad4: aa0103f6     	mov	x22, x1
    2ad8: 90000008     	adrp	x8, 0x2000 <__ZN4Game32performThreadedSpringDerivativesER16PointMassesRangeR5RangeI6SpringES2_I15PointDerivativeER18ConsoleProfileInfo+0x2c>
    2adc: f9400108     	ldr	x8, [x8]
    2ae0: f9400108     	ldr	x8, [x8]
    2ae4: f81c83a8     	stur	x8, [x29, #-56]
    2ae8: d10103a0     	sub	x0, x29, #64
    2aec: 94000000     	bl	0x2aec <__ZN4Game32performThreadedSpringDerivativesER16PointMassesRangeR5RangeI6SpringES2_I15PointDerivativeER18ConsoleProfileInfo+0x40>
    2af0: 910003f8     	mov	x24, sp
    2af4: 9000000d     	adrp	x13, 0x2000 <__ZN4Game32performThreadedSpringDerivativesER16PointMassesRangeR5RangeI6SpringES2_I15PointDerivativeER18ConsoleProfileInfo+0x48>
    2af8: f94001ad     	ldr	x13, [x13]
    2afc: b94001a8     	ldr	w8, [x13]
    2b00: 8b080508     	add	x8, x8, x8, lsl #1
    2b04: d37be908     	lsl	x8, x8, #5
    2b08: aa0803e9     	mov	x9, x8
    2b0c: 90000010     	adrp	x16, 0x2000 <__ZN4Game32performThreadedSpringDerivativesER16PointMassesRangeR5RangeI6SpringES2_I15PointDerivativeER18ConsoleProfileInfo+0x60>
    2b10: f9400210     	ldr	x16, [x16]
    2b14: d63f0200     	blr	x16
    2b18: 910003e9     	mov	x9, sp
    2b1c: cb08012c     	sub	x12, x9, x8
    2b20: 9100019f     	mov	sp, x12
    2b24: b94001a2     	ldr	w2, [x13]
    2b28: d37cec48     	lsl	x8, x2, #4
    2b2c: aa0803e9     	mov	x9, x8
    2b30: 90000010     	adrp	x16, 0x2000 <__ZN4Game32performThreadedSpringDerivativesER16PointMassesRangeR5RangeI6SpringES2_I15PointDerivativeER18ConsoleProfileInfo+0x84>
    2b34: f9400210     	ldr	x16, [x16]
    2b38: d63f0200     	blr	x16
    2b3c: 910003e9     	mov	x9, sp
    2b40: cb080121     	sub	x1, x9, x8
    2b44: 9100003f     	mov	sp, x1
    2b48: d2800008     	mov	x8, #0
    2b4c: 52800003     	mov	w3, #0
    2b50: b9400ae9     	ldr	w9, [x23, #8]
    2b54: f94002ea     	ldr	x10, [x23]
    2b58: 93407d2b     	sxtw	x11, w9
    2b5c: 7100005f     	cmp	w2, #0
    2b60: 1a9fc04d     	csel	w13, w2, wzr, gt
    2b64: 9100514e     	add	x14, x10, #20
    2b68: 5280030f     	mov	w15, #24
    2b6c: 52800c10     	mov	w16, #96
    2b70: 90000011     	adrp	x17, 0x2000 <__ZN4Game32performThreadedSpringDerivativesER16PointMassesRangeR5RangeI6SpringES2_I15PointDerivativeER18ConsoleProfileInfo+0xc4>
    2b74: 91000231     	add	x17, x17, #0
    2b78: 1ac20d20     	sdiv	w0, w9, w2
    2b7c: 14000011     	b	0x2bc0 <__ZN4Game32performThreadedSpringDerivativesER16PointMassesRangeR5RangeI6SpringES2_I15PointDerivativeER18ConsoleProfileInfo+0x114>
    2b80: aa0503e4     	mov	x4, x5
    2b84: 9b2f2865     	smaddl	x5, w3, w15, x10
    2b88: 4b030083     	sub	w3, w4, w3
    2b8c: 9b103106     	madd	x6, x8, x16, x12
    2b90: a9000cc5     	stp	x5, x3, [x6]
    2b94: ad4006c0     	ldp	q0, q1, [x22]
    2b98: ad0084c0     	stp	q0, q1, [x6, #16]
    2b9c: ad4106c0     	ldp	q0, q1, [x22, #32]
    2ba0: ad0184c0     	stp	q0, q1, [x6, #48]
    2ba4: a90550d5     	stp	x21, x20, [x6, #80]
    2ba8: 8b081025     	add	x5, x1, x8, lsl #4
    2bac: 91000508     	add	x8, x8, #1
    2bb0: aa0403e3     	mov	x3, x4
    2bb4: a90018b1     	stp	x17, x6, [x5]
    2bb8: 6b09009f     	cmp	w4, w9
    2bbc: 540002e0     	b.eq	0x2c18 <__ZN4Game32performThreadedSpringDerivativesER16PointMassesRangeR5RangeI6SpringES2_I15PointDerivativeER18ConsoleProfileInfo+0x16c>
    2bc0: eb0d011f     	cmp	x8, x13
    2bc4: 540002c0     	b.eq	0x2c1c <__ZN4Game32performThreadedSpringDerivativesER16PointMassesRangeR5RangeI6SpringES2_I15PointDerivativeER18ConsoleProfileInfo+0x170>
    2bc8: 0b000065     	add	w5, w3, w0
    2bcc: 6b0900bf     	cmp	w5, w9
    2bd0: 1a89b0a4     	csel	w4, w5, w9, lt
    2bd4: 6b05013f     	cmp	w9, w5
    2bd8: 54fffd6d     	b.le	0x2b84 <__ZN4Game32performThreadedSpringDerivativesER16PointMassesRangeR5RangeI6SpringES2_I15PointDerivativeER18ConsoleProfileInfo+0xd8>
    2bdc: 93407c85     	sxtw	x5, w4
    2be0: 8b24c4a4     	add	x4, x5, w4, sxtw #1
    2be4: d37df086     	lsl	x6, x4, #3
    2be8: 8b060144     	add	x4, x10, x6
    2bec: b85fc084     	ldur	w4, [x4, #-4]
    2bf0: 8b0601c6     	add	x6, x14, x6
    2bf4: b94000c7     	ldr	w7, [x6]
    2bf8: 6b0400ff     	cmp	w7, w4
    2bfc: 54fffc21     	b.ne	0x2b80 <__ZN4Game32performThreadedSpringDerivativesER16PointMassesRangeR5RangeI6SpringES2_I15PointDerivativeER18ConsoleProfileInfo+0xd4>
    2c00: 910004a5     	add	x5, x5, #1
    2c04: 910060c6     	add	x6, x6, #24
    2c08: eb0b00bf     	cmp	x5, x11
    2c0c: 54ffff4b     	b.lt	0x2bf4 <__ZN4Game32performThreadedSpringDerivativesER16PointMassesRangeR5RangeI6SpringES2_I15PointDerivativeER18ConsoleProfileInfo+0x148>
    2c10: aa0903e4     	mov	x4, x9
    2c14: 17ffffdc     	b	0x2b84 <__ZN4Game32performThreadedSpringDerivativesER16PointMassesRangeR5RangeI6SpringES2_I15PointDerivativeER18ConsoleProfileInfo+0xd8>
    2c18: aa0803e2     	mov	x2, x8
    2c1c: 90000008     	adrp	x8, 0x2000 <__ZN4Game32performThreadedSpringDerivativesER16PointMassesRangeR5RangeI6SpringES2_I15PointDerivativeER18ConsoleProfileInfo+0x170>
    2c20: f9400108     	ldr	x8, [x8]
    2c24: f9400100     	ldr	x0, [x8]
    2c28: 94000000     	bl	0x2c28 <__ZN4Game32performThreadedSpringDerivativesER16PointMassesRangeR5RangeI6SpringES2_I15PointDerivativeER18ConsoleProfileInfo+0x17c>
    2c2c: d10103a0     	sub	x0, x29, #64
    2c30: 94000000     	bl	0x2c30 <__ZN4Game32performThreadedSpringDerivativesER16PointMassesRangeR5RangeI6SpringES2_I15PointDerivativeER18ConsoleProfileInfo+0x184>
    2c34: fd401a61     	ldr	d1, [x19, #48]
    2c38: 1e612800     	fadd	d0, d0, d1
    2c3c: fd001a60     	str	d0, [x19, #48]
    2c40: 9100031f     	mov	sp, x24
    2c44: f85c83a8     	ldur	x8, [x29, #-56]
    2c48: 90000009     	adrp	x9, 0x2000 <__ZN4Game32performThreadedSpringDerivativesER16PointMassesRangeR5RangeI6SpringES2_I15PointDerivativeER18ConsoleProfileInfo+0x19c>
    2c4c: f9400129     	ldr	x9, [x9]
    2c50: f9400129     	ldr	x9, [x9]
    2c54: eb08013f     	cmp	x9, x8
    2c58: 540000e1     	b.ne	0x2c74 <__ZN4Game32performThreadedSpringDerivativesER16PointMassesRangeR5RangeI6SpringES2_I15PointDerivativeER18ConsoleProfileInfo+0x1c8>
    2c5c: d100c3bf     	sub	sp, x29, #48
    2c60: a9437bfd     	ldp	x29, x30, [sp, #48]
    2c64: a9424ff4     	ldp	x20, x19, [sp, #32]
    2c68: a94157f6     	ldp	x22, x21, [sp, #16]
    2c6c: a8c45ff8     	ldp	x24, x23, [sp], #64
    2c70: d65f03c0     	ret
    2c74: 94000000     	bl	0x2c74 <__ZN4Game32performThreadedSpringDerivativesER16PointMassesRangeR5RangeI6SpringES2_I15PointDerivativeER18ConsoleProfileInfo+0x1c8>

0000000000002c78 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo>:
    2c78: d10443ff     	sub	sp, sp, #272
    2c7c: 6d092beb     	stp	d11, d10, [sp, #144]
    2c80: 6d0a23e9     	stp	d9, d8, [sp, #160]
    2c84: a90b6ffc     	stp	x28, x27, [sp, #176]
    2c88: a90c67fa     	stp	x26, x25, [sp, #192]
    2c8c: a90d5ff8     	stp	x24, x23, [sp, #208]
    2c90: a90e57f6     	stp	x22, x21, [sp, #224]
    2c94: a90f4ff4     	stp	x20, x19, [sp, #240]
    2c98: a9107bfd     	stp	x29, x30, [sp, #256]
    2c9c: 910403fd     	add	x29, sp, #256
    2ca0: aa0503f6     	mov	x22, x5
    2ca4: aa0403fc     	mov	x28, x4
    2ca8: aa0203f7     	mov	x23, x2
    2cac: aa0103f3     	mov	x19, x1
    2cb0: aa0003f5     	mov	x21, x0
    2cb4: f9400008     	ldr	x8, [x0]
    2cb8: 5296b309     	mov	w9, #46488
    2cbc: 8b090119     	add	x25, x8, x9
    2cc0: b9400328     	ldr	w8, [x25]
    2cc4: b9400829     	ldr	w9, [x1, #8]
    2cc8: 6b09011f     	cmp	w8, w9
    2ccc: 540006a0     	b.eq	0x2da0 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x128>
    2cd0: 94000000     	bl	0x2cd0 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x58>
    2cd4: 1e204008     	fmov	s8, s0
    2cd8: 1e204029     	fmov	s9, s1
    2cdc: 94000000     	bl	0x2cdc <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x64>
    2ce0: 1e20400a     	fmov	s10, s0
    2ce4: 1e20402b     	fmov	s11, s1
    2ce8: f94002a8     	ldr	x8, [x21]
    2cec: 52968309     	mov	w9, #46104
    2cf0: b869691a     	ldr	w26, [x8, x9]
    2cf4: b9400728     	ldr	w8, [x25, #4]
    2cf8: 7100051f     	cmp	w8, #1
    2cfc: 7a5aa108     	ccmp	w8, w26, #8, ge
    2d00: 5400030a     	b.ge	0x2d60 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0xe8>
    2d04: 52800048     	mov	w8, #2
    2d08: 71000b5f     	cmp	w26, #2
    2d0c: 1a88c35b     	csel	w27, w26, w8, gt
    2d10: d37c7f60     	ubfiz	x0, x27, #4, #32
    2d14: 94000000     	bl	0x2d14 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x9c>
    2d18: aa0003f8     	mov	x24, x0
    2d1c: b9400328     	ldr	w8, [x25]
    2d20: f9400720     	ldr	x0, [x25, #8]
    2d24: 7100051f     	cmp	w8, #1
    2d28: 5400012b     	b.lt	0x2d4c <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0xd4>
    2d2c: aa1803e9     	mov	x9, x24
    2d30: aa0003ea     	mov	x10, x0
    2d34: 3cc10540     	ldr	q0, [x10], #16
    2d38: 3c810520     	str	q0, [x9], #16
    2d3c: f1000508     	subs	x8, x8, #1
    2d40: 54ffffa1     	b.ne	0x2d34 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0xbc>
    2d44: b900033f     	str	wzr, [x25]
    2d48: 14000003     	b	0x2d54 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0xdc>
    2d4c: b900033f     	str	wzr, [x25]
    2d50: b4000040     	cbz	x0, 0x2d58 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0xe0>
    2d54: 94000000     	bl	0x2d54 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0xdc>
    2d58: f9000738     	str	x24, [x25, #8]
    2d5c: b900073b     	str	w27, [x25, #4]
    2d60: b900033a     	str	w26, [x25]
    2d64: 7100075f     	cmp	w26, #1
    2d68: 540001cb     	b.lt	0x2da0 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x128>
    2d6c: d2800008     	mov	x8, #0
    2d70: d2800009     	mov	x9, #0
    2d74: f940072a     	ldr	x10, [x25, #8]
    2d78: 8b08014a     	add	x10, x10, x8
    2d7c: bd000148     	str	s8, [x10]
    2d80: bd000549     	str	s9, [x10, #4]
    2d84: bd00094a     	str	s10, [x10, #8]
    2d88: bd000d4b     	str	s11, [x10, #12]
    2d8c: 91000529     	add	x9, x9, #1
    2d90: b980032a     	ldrsw	x10, [x25]
    2d94: 91004108     	add	x8, x8, #16
    2d98: eb0a013f     	cmp	x9, x10
    2d9c: 54fffecb     	b.lt	0x2d74 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0xfc>
    2da0: f94002a8     	ldr	x8, [x21]
    2da4: 5296b309     	mov	w9, #46488
    2da8: 8b090103     	add	x3, x8, x9
    2dac: 5296a314     	mov	w20, #46360
    2db0: 8b140104     	add	x4, x8, x20
    2db4: 2f00e400     	movi	d0, #0000000000000000
    2db8: aa1503e0     	mov	x0, x21
    2dbc: aa1303e1     	mov	x1, x19
    2dc0: 94000000     	bl	0x2dc0 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x148>
    2dc4: f94002a8     	ldr	x8, [x21]
    2dc8: 8b140108     	add	x8, x8, x20
    2dcc: b9404109     	ldr	w9, [x8, #64]
    2dd0: f940250a     	ldr	x10, [x8, #72]
    2dd4: a904a7ea     	stp	x10, x9, [sp, #72]
    2dd8: f9402d0a     	ldr	x10, [x8, #88]
    2ddc: a905a7ea     	stp	x10, x9, [sp, #88]
    2de0: f940350a     	ldr	x10, [x8, #104]
    2de4: a906a7ea     	stp	x10, x9, [sp, #104]
    2de8: f9403d0a     	ldr	x10, [x8, #120]
    2dec: a907a7ea     	stp	x10, x9, [sp, #120]
    2df0: b9400104     	ldr	w4, [x8]
    2df4: f9400503     	ldr	x3, [x8, #8]
    2df8: 910123e1     	add	x1, sp, #72
    2dfc: aa1703e2     	mov	x2, x23
    2e00: aa1603e5     	mov	x5, x22
    2e04: 94000000     	bl	0x2e04 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x18c>
    2e08: f94002a8     	ldr	x8, [x21]
    2e0c: 8b140103     	add	x3, x8, x20
    2e10: 5296a514     	mov	w20, #46376
    2e14: 8b140104     	add	x4, x8, x20
    2e18: 1e6c1000     	fmov	d0, #0.50000000
    2e1c: aa1503e0     	mov	x0, x21
    2e20: aa1303e1     	mov	x1, x19
    2e24: 94000000     	bl	0x2e24 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x1ac>
    2e28: f94002a8     	ldr	x8, [x21]
    2e2c: 8b140108     	add	x8, x8, x20
    2e30: b9403109     	ldr	w9, [x8, #48]
    2e34: f9401d0a     	ldr	x10, [x8, #56]
    2e38: a904a7ea     	stp	x10, x9, [sp, #72]
    2e3c: f940250a     	ldr	x10, [x8, #72]
    2e40: a905a7ea     	stp	x10, x9, [sp, #88]
    2e44: f9402d0a     	ldr	x10, [x8, #88]
    2e48: a906a7ea     	stp	x10, x9, [sp, #104]
    2e4c: f940350a     	ldr	x10, [x8, #104]
    2e50: a907a7ea     	stp	x10, x9, [sp, #120]
    2e54: b9400104     	ldr	w4, [x8]
    2e58: f9400503     	ldr	x3, [x8, #8]
    2e5c: 910123e1     	add	x1, sp, #72
    2e60: aa1703e2     	mov	x2, x23
    2e64: aa1603e5     	mov	x5, x22
    2e68: 94000000     	bl	0x2e68 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x1f0>
    2e6c: f94002a8     	ldr	x8, [x21]
    2e70: 8b140103     	add	x3, x8, x20
    2e74: 5296a714     	mov	w20, #46392
    2e78: 8b140104     	add	x4, x8, x20
    2e7c: 1e6c1000     	fmov	d0, #0.50000000
    2e80: aa1503e0     	mov	x0, x21
    2e84: aa1303e1     	mov	x1, x19
    2e88: 94000000     	bl	0x2e88 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x210>
    2e8c: f94002a8     	ldr	x8, [x21]
    2e90: 8b140108     	add	x8, x8, x20
    2e94: b9402109     	ldr	w9, [x8, #32]
    2e98: f940150a     	ldr	x10, [x8, #40]
    2e9c: a904a7ea     	stp	x10, x9, [sp, #72]
    2ea0: f9401d0a     	ldr	x10, [x8, #56]
    2ea4: a905a7ea     	stp	x10, x9, [sp, #88]
    2ea8: f940250a     	ldr	x10, [x8, #72]
    2eac: a906a7ea     	stp	x10, x9, [sp, #104]
    2eb0: f9402d0a     	ldr	x10, [x8, #88]
    2eb4: a907a7ea     	stp	x10, x9, [sp, #120]
    2eb8: b9400104     	ldr	w4, [x8]
    2ebc: f9400503     	ldr	x3, [x8, #8]
    2ec0: 910123e1     	add	x1, sp, #72
    2ec4: aa1703e2     	mov	x2, x23
    2ec8: aa1603e5     	mov	x5, x22
    2ecc: 94000000     	bl	0x2ecc <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x254>
    2ed0: f94002a8     	ldr	x8, [x21]
    2ed4: 8b140103     	add	x3, x8, x20
    2ed8: 5296a914     	mov	w20, #46408
    2edc: 8b140104     	add	x4, x8, x20
    2ee0: 1e6e1000     	fmov	d0, #1.00000000
    2ee4: aa1503e0     	mov	x0, x21
    2ee8: aa1303e1     	mov	x1, x19
    2eec: 94000000     	bl	0x2eec <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x274>
    2ef0: f94002a8     	ldr	x8, [x21]
    2ef4: 8b140108     	add	x8, x8, x20
    2ef8: b9401109     	ldr	w9, [x8, #16]
    2efc: f9400d0a     	ldr	x10, [x8, #24]
    2f00: a904a7ea     	stp	x10, x9, [sp, #72]
    2f04: f940150a     	ldr	x10, [x8, #40]
    2f08: a905a7ea     	stp	x10, x9, [sp, #88]
    2f0c: f9401d0a     	ldr	x10, [x8, #56]
    2f10: a906a7ea     	stp	x10, x9, [sp, #104]
    2f14: f940250a     	ldr	x10, [x8, #72]
    2f18: a907a7ea     	stp	x10, x9, [sp, #120]
    2f1c: b9400104     	ldr	w4, [x8]
    2f20: f9400503     	ldr	x3, [x8, #8]
    2f24: 910123e1     	add	x1, sp, #72
    2f28: aa1703e2     	mov	x2, x23
    2f2c: aa1603e5     	mov	x5, x22
    2f30: 94000000     	bl	0x2f30 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x2b8>
    2f34: f94002a8     	ldr	x8, [x21]
    2f38: 52968309     	mov	w9, #46104
    2f3c: 8b090109     	add	x9, x8, x9
    2f40: b940012c     	ldr	w12, [x9]
    2f44: 7100059f     	cmp	w12, #1
    2f48: 54001bab     	b.lt	0x32bc <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x644>
    2f4c: f940853b     	ldr	x27, [x9, #264]
    2f50: f9408d2d     	ldr	x13, [x9, #280]
    2f54: f940952a     	ldr	x10, [x9, #296]
    2f58: f9409d2b     	ldr	x11, [x9, #312]
    2f5c: f940026e     	ldr	x14, [x19]
    2f60: f9401270     	ldr	x16, [x19, #32]
    2f64: 7100219f     	cmp	w12, #8
    2f68: 54000062     	b.hs	0x2f74 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x2fc>
    2f6c: d2800011     	mov	x17, #0
    2f70: 140000ab     	b	0x321c <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x5a4>
    2f74: b90047fc     	str	w28, [sp, #68]
    2f78: a9025bff     	stp	xzr, x22, [sp, #32]
    2f7c: d37df181     	lsl	x1, x12, #3
    2f80: 8b0101d4     	add	x20, x14, x1
    2f84: d1001280     	sub	x0, x20, #4
    2f88: 910011d6     	add	x22, x14, #4
    2f8c: 8b010201     	add	x1, x16, x1
    2f90: d1001022     	sub	x2, x1, #4
    2f94: d37ced83     	lsl	x3, x12, #4
    2f98: aa0d03fc     	mov	x28, x13
    2f9c: 8b0301be     	add	x30, x13, x3
    2fa0: d10013c5     	sub	x5, x30, #4
    2fa4: 8b030144     	add	x4, x10, x3
    2fa8: d1001086     	sub	x6, x4, #4
    2fac: 8b03036d     	add	x13, x27, x3
    2fb0: d10011a7     	sub	x7, x13, #4
    2fb4: 8b030163     	add	x3, x11, x3
    2fb8: d1001077     	sub	x23, x3, #4
    2fbc: 9100120f     	add	x15, x16, #4
    2fc0: 91001398     	add	x24, x28, #4
    2fc4: 91001159     	add	x25, x10, #4
    2fc8: 9100137a     	add	x26, x27, #4
    2fcc: eb0501df     	cmp	x14, x5
    2fd0: fa403382     	ccmp	x28, x0, #2, lo
    2fd4: 1a9f27f1     	cset	w17, lo
    2fd8: b9001ff1     	str	w17, [sp, #28]
    2fdc: eb0601df     	cmp	x14, x6
    2fe0: fa403142     	ccmp	x10, x0, #2, lo
    2fe4: 1a9f27f1     	cset	w17, lo
    2fe8: b9001bf1     	str	w17, [sp, #24]
    2fec: eb0701df     	cmp	x14, x7
    2ff0: fa403362     	ccmp	x27, x0, #2, lo
    2ff4: 1a9f27f1     	cset	w17, lo
    2ff8: b90017f1     	str	w17, [sp, #20]
    2ffc: eb1701df     	cmp	x14, x23
    3000: fa403162     	ccmp	x11, x0, #2, lo
    3004: 1a9f27f1     	cset	w17, lo
    3008: b90013f1     	str	w17, [sp, #16]
    300c: eb05021f     	cmp	x16, x5
    3010: a9036ffc     	stp	x28, x27, [sp, #48]
    3014: fa423382     	ccmp	x28, x2, #2, lo
    3018: 1a9f27f1     	cset	w17, lo
    301c: eb06021f     	cmp	x16, x6
    3020: fa423142     	ccmp	x10, x2, #2, lo
    3024: 1a9f27e6     	cset	w6, lo
    3028: eb07021f     	cmp	x16, x7
    302c: fa423362     	ccmp	x27, x2, #2, lo
    3030: 1a9f27e5     	cset	w5, lo
    3034: 29011be5     	stp	w5, w6, [sp, #8]
    3038: eb17021f     	cmp	x16, x23
    303c: fa423162     	ccmp	x11, x2, #2, lo
    3040: 1a9f27e5     	cset	w5, lo
    3044: b90007e5     	str	w5, [sp, #4]
    3048: eb0102df     	cmp	x22, x1
    304c: fa5431e2     	ccmp	x15, x20, #2, lo
    3050: 1a9f27e5     	cset	w5, lo
    3054: eb1e02df     	cmp	x22, x30
    3058: fa543302     	ccmp	x24, x20, #2, lo
    305c: 1a9f27e6     	cset	w6, lo
    3060: eb0402df     	cmp	x22, x4
    3064: fa543322     	ccmp	x25, x20, #2, lo
    3068: 1a9f27e7     	cset	w7, lo
    306c: eb0d02df     	cmp	x22, x13
    3070: fa543342     	ccmp	x26, x20, #2, lo
    3074: 1a9f27fb     	cset	w27, lo
    3078: eb0302df     	cmp	x22, x3
    307c: 91001177     	add	x23, x11, #4
    3080: fa5432e2     	ccmp	x23, x20, #2, lo
    3084: 1a9f27fc     	cset	w28, lo
    3088: eb1e01ff     	cmp	x15, x30
    308c: fa413302     	ccmp	x24, x1, #2, lo
    3090: 1a9f27fe     	cset	w30, lo
    3094: eb0401ff     	cmp	x15, x4
    3098: fa413322     	ccmp	x25, x1, #2, lo
    309c: 1a9f27f6     	cset	w22, lo
    30a0: eb0d01ff     	cmp	x15, x13
    30a4: fa413342     	ccmp	x26, x1, #2, lo
    30a8: 1a9f27f4     	cset	w20, lo
    30ac: eb0301ff     	cmp	x15, x3
    30b0: fa4132e2     	ccmp	x23, x1, #2, lo
    30b4: 1a9f27e1     	cset	w1, lo
    30b8: eb00021f     	cmp	x16, x0
    30bc: fa4231c2     	ccmp	x14, x2, #2, lo
    30c0: 540009e3     	b.lo	0x31fc <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x584>
    30c4: b9401fed     	ldr	w13, [sp, #28]
    30c8: 370009ad     	tbnz	w13, #0, 0x31fc <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x584>
    30cc: b9401bed     	ldr	w13, [sp, #24]
    30d0: 3700096d     	tbnz	w13, #0, 0x31fc <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x584>
    30d4: b94017ed     	ldr	w13, [sp, #20]
    30d8: 3700092d     	tbnz	w13, #0, 0x31fc <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x584>
    30dc: b94013ed     	ldr	w13, [sp, #16]
    30e0: 370008ed     	tbnz	w13, #0, 0x31fc <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x584>
    30e4: 370008d1     	tbnz	w17, #0, 0x31fc <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x584>
    30e8: b9400fed     	ldr	w13, [sp, #12]
    30ec: 3700088d     	tbnz	w13, #0, 0x31fc <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x584>
    30f0: b9400bed     	ldr	w13, [sp, #8]
    30f4: 3700084d     	tbnz	w13, #0, 0x31fc <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x584>
    30f8: b94007ed     	ldr	w13, [sp, #4]
    30fc: 3700080d     	tbnz	w13, #0, 0x31fc <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x584>
    3100: 370007e5     	tbnz	w5, #0, 0x31fc <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x584>
    3104: 370007c6     	tbnz	w6, #0, 0x31fc <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x584>
    3108: 370007a7     	tbnz	w7, #0, 0x31fc <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x584>
    310c: 3700079b     	tbnz	w27, #0, 0x31fc <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x584>
    3110: 3700077c     	tbnz	w28, #0, 0x31fc <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x584>
    3114: f9401ffb     	ldr	x27, [sp, #56]
    3118: 370007be     	tbnz	w30, #0, 0x320c <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x594>
    311c: b94047fc     	ldr	w28, [sp, #68]
    3120: 370006b6     	tbnz	w22, #0, 0x31f4 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x57c>
    3124: f9401bed     	ldr	x13, [sp, #48]
    3128: 37000634     	tbnz	w20, #0, 0x31ec <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x574>
    312c: a9425bf1     	ldp	x17, x22, [sp, #32]
    3130: 37000761     	tbnz	w1, #0, 0x321c <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x5a4>
    3134: 927e7591     	and	x17, x12, #0xfffffffc
    3138: 4f00f700     	fmov.4s	v0, #6.00000000
    313c: aa1103e0     	mov	x0, x17
    3140: aa1003e1     	mov	x1, x16
    3144: aa0e03e2     	mov	x2, x14
    3148: aa0b03e3     	mov	x3, x11
    314c: aa0a03e4     	mov	x4, x10
    3150: aa0d03e5     	mov	x5, x13
    3154: aa1b03e6     	mov	x6, x27
    3158: 4cdf08a1     	ld4.4s	{ v1, v2, v3, v4 }, [x5], #64
    315c: 4cdf0890     	ld4.4s	{ v16, v17, v18, v19 }, [x4], #64
    3160: 4e30d425     	fadd.4s	v5, v1, v16
    3164: 4cdf08d4     	ld4.4s	{ v20, v21, v22, v23 }, [x6], #64
    3168: 4cdf0878     	ld4.4s	{ v24, v25, v26, v27 }, [x3], #64
    316c: 4e31d446     	fadd.4s	v6, v2, v17
    3170: 4e25d4a5     	fadd.4s	v5, v5, v5
    3174: 4e26d4c6     	fadd.4s	v6, v6, v6
    3178: 4e34d4a5     	fadd.4s	v5, v5, v20
    317c: 4e26d6a6     	fadd.4s	v6, v21, v6
    3180: 4e38d4a5     	fadd.4s	v5, v5, v24
    3184: 4e39d4c6     	fadd.4s	v6, v6, v25
    3188: 6e20fca5     	fdiv.4s	v5, v5, v0
    318c: 4e32d467     	fadd.4s	v7, v3, v18
    3190: 4e33d481     	fadd.4s	v1, v4, v19
    3194: 4e27d4e2     	fadd.4s	v2, v7, v7
    3198: 6e20fcc3     	fdiv.4s	v3, v6, v0
    319c: 4e21d421     	fadd.4s	v1, v1, v1
    31a0: 4c408846     	ld2.4s	{ v6, v7 }, [x2]
    31a4: 4e36d442     	fadd.4s	v2, v2, v22
    31a8: 4e21d6e1     	fadd.4s	v1, v23, v1
    31ac: 4e3ad442     	fadd.4s	v2, v2, v26
    31b0: 4e3bd421     	fadd.4s	v1, v1, v27
    31b4: 4e26d4a4     	fadd.4s	v4, v5, v6
    31b8: 4e27d465     	fadd.4s	v5, v3, v7
    31bc: 6e20fc42     	fdiv.4s	v2, v2, v0
    31c0: 4c9f8844     	st2.4s	{ v4, v5 }, [x2], #32
    31c4: 6e20fc21     	fdiv.4s	v1, v1, v0
    31c8: 4c408823     	ld2.4s	{ v3, v4 }, [x1]
    31cc: 4e23d445     	fadd.4s	v5, v2, v3
    31d0: 4e24d426     	fadd.4s	v6, v1, v4
    31d4: 4c9f8825     	st2.4s	{ v5, v6 }, [x1], #32
    31d8: f1001000     	subs	x0, x0, #4
    31dc: 54fffbe1     	b.ne	0x3158 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x4e0>
    31e0: eb0c023f     	cmp	x17, x12
    31e4: 540001c1     	b.ne	0x321c <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x5a4>
    31e8: 14000035     	b	0x32bc <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x644>
    31ec: a9425bf1     	ldp	x17, x22, [sp, #32]
    31f0: 1400000b     	b	0x321c <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x5a4>
    31f4: f94017f6     	ldr	x22, [sp, #40]
    31f8: 14000007     	b	0x3214 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x59c>
    31fc: f94017f6     	ldr	x22, [sp, #40]
    3200: b94047fc     	ldr	w28, [sp, #68]
    3204: f9401ffb     	ldr	x27, [sp, #56]
    3208: 14000003     	b	0x3214 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x59c>
    320c: f94017f6     	ldr	x22, [sp, #40]
    3210: b94047fc     	ldr	w28, [sp, #68]
    3214: f9401bed     	ldr	x13, [sp, #48]
    3218: f94013f1     	ldr	x17, [sp, #32]
    321c: cb11018c     	sub	x12, x12, x17
    3220: d37cee20     	lsl	x0, x17, #4
    3224: 910021af     	add	x15, x13, #8
    3228: 9100236d     	add	x13, x27, #8
    322c: d37df231     	lsl	x17, x17, #3
    3230: 8b110210     	add	x16, x16, x17
    3234: 8b1101ce     	add	x14, x14, x17
    3238: 0f00f700     	fmov.2s	v0, #6.00000000
    323c: 8b0001b1     	add	x17, x13, x0
    3240: 8b0001e1     	add	x1, x15, x0
    3244: 8b000142     	add	x2, x10, x0
    3248: 8b000163     	add	x3, x11, x0
    324c: fc5f8021     	ldur	d1, [x1, #-8]
    3250: 6d400c42     	ldp	d2, d3, [x2]
    3254: 0e22d421     	fadd.2s	v1, v1, v2
    3258: 0e21d421     	fadd.2s	v1, v1, v1
    325c: fc5f8222     	ldur	d2, [x17, #-8]
    3260: 0e22d421     	fadd.2s	v1, v1, v2
    3264: 6d401062     	ldp	d2, d4, [x3]
    3268: 0e22d421     	fadd.2s	v1, v1, v2
    326c: 2e20fc21     	fdiv.2s	v1, v1, v0
    3270: fd4001c2     	ldr	d2, [x14]
    3274: 0e22d421     	fadd.2s	v1, v1, v2
    3278: fd400022     	ldr	d2, [x1]
    327c: 0e23d442     	fadd.2s	v2, v2, v3
    3280: 0e22d442     	fadd.2s	v2, v2, v2
    3284: fd400223     	ldr	d3, [x17]
    3288: 0e23d442     	fadd.2s	v2, v2, v3
    328c: 0e24d442     	fadd.2s	v2, v2, v4
    3290: 2e20fc42     	fdiv.2s	v2, v2, v0
    3294: fc0085c1     	str	d1, [x14], #8
    3298: fd400201     	ldr	d1, [x16]
    329c: 0e21d441     	fadd.2s	v1, v2, v1
    32a0: fc008601     	str	d1, [x16], #8
    32a4: 9100414a     	add	x10, x10, #16
    32a8: 910041ef     	add	x15, x15, #16
    32ac: 910041ad     	add	x13, x13, #16
    32b0: 9100416b     	add	x11, x11, #16
    32b4: f100058c     	subs	x12, x12, #1
    32b8: 54fffc21     	b.ne	0x323c <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x5c4>
    32bc: b940a129     	ldr	w9, [x9, #160]
    32c0: 7100053f     	cmp	w9, #1
    32c4: 540002ab     	b.lt	0x3318 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x6a0>
    32c8: d2800009     	mov	x9, #0
    32cc: d280000a     	mov	x10, #0
    32d0: 5296980b     	mov	w11, #46272
    32d4: 5296970c     	mov	w12, #46264
    32d8: f86b6908     	ldr	x8, [x8, x11]
    32dc: 8b090108     	add	x8, x8, x9
    32e0: b980010d     	ldrsw	x13, [x8]
    32e4: f940026e     	ldr	x14, [x19]
    32e8: f840410f     	ldur	x15, [x8, #4]
    32ec: f82d79cf     	str	x15, [x14, x13, lsl #3]
    32f0: b9800108     	ldrsw	x8, [x8]
    32f4: f940126d     	ldr	x13, [x19, #32]
    32f8: 8b080da8     	add	x8, x13, x8, lsl #3
    32fc: f900011f     	str	xzr, [x8]
    3300: 9100054a     	add	x10, x10, #1
    3304: f94002a8     	ldr	x8, [x21]
    3308: b8ac690d     	ldrsw	x13, [x8, x12]
    330c: 91003129     	add	x9, x9, #12
    3310: eb0d015f     	cmp	x10, x13
    3314: 54fffe2b     	b.lt	0x32d8 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x660>
    3318: 5296b509     	mov	w9, #46504
    331c: 8b090109     	add	x9, x8, x9
    3320: b980012a     	ldrsw	x10, [x9]
    3324: 3100055f     	cmn	w10, #1
    3328: 540001e0     	b.eq	0x3364 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x6ec>
    332c: f9400268     	ldr	x8, [x19]
    3330: f8404129     	ldur	x9, [x9, #4]
    3334: f82a7909     	str	x9, [x8, x10, lsl #3]
    3338: f94002a8     	ldr	x8, [x21]
    333c: 5296b509     	mov	w9, #46504
    3340: b8a96908     	ldrsw	x8, [x8, x9]
    3344: f940126a     	ldr	x10, [x19, #32]
    3348: 6f00e400     	movi.2d	v0, #0000000000000000
    334c: fc287940     	str	d0, [x10, x8, lsl #3]
    3350: f94002a8     	ldr	x8, [x21]
    3354: b8a96908     	ldrsw	x8, [x8, x9]
    3358: f9401a69     	ldr	x9, [x19, #48]
    335c: fc287920     	str	d0, [x9, x8, lsl #3]
    3360: f94002a8     	ldr	x8, [x21]
    3364: 5296b6a9     	mov	w9, #46517
    3368: 38696908     	ldrb	w8, [x8, x9]
    336c: 7100011f     	cmp	w8, #0
    3370: 7a401b84     	ccmp	w28, #0, #4, ne
    3374: 54000c20     	b.eq	0x34f8 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x880>
    3378: b9400a68     	ldr	w8, [x19, #8]
    337c: 7100051f     	cmp	w8, #1
    3380: 5400012b     	b.lt	0x33a4 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x72c>
    3384: d2800008     	mov	x8, #0
    3388: f9401a69     	ldr	x9, [x19, #48]
    338c: 8b080d29     	add	x9, x9, x8, lsl #3
    3390: f900013f     	str	xzr, [x9]
    3394: 91000508     	add	x8, x8, #1
    3398: b9800a69     	ldrsw	x9, [x19, #8]
    339c: eb09011f     	cmp	x8, x9
    33a0: 54ffff4b     	b.lt	0x3388 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x710>
    33a4: 910123e0     	add	x0, sp, #72
    33a8: 94000000     	bl	0x33a8 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x730>
    33ac: 1e6e1000     	fmov	d0, #1.00000000
    33b0: aa1503e0     	mov	x0, x21
    33b4: aa1603e3     	mov	x3, x22
    33b8: 94000000     	bl	0x33b8 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x740>
    33bc: 910123e0     	add	x0, sp, #72
    33c0: 94000000     	bl	0x33c0 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x748>
    33c4: fd001ec0     	str	d0, [x22, #56]
    33c8: b9400a68     	ldr	w8, [x19, #8]
    33cc: 7100051f     	cmp	w8, #1
    33d0: 5400094b     	b.lt	0x34f8 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x880>
    33d4: f9401a69     	ldr	x9, [x19, #48]
    33d8: f940126a     	ldr	x10, [x19, #32]
    33dc: f940026b     	ldr	x11, [x19]
    33e0: 7100311f     	cmp	w8, #12
    33e4: 54000062     	b.hs	0x33f0 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x778>
    33e8: d280000c     	mov	x12, #0
    33ec: 14000035     	b	0x34c0 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x848>
    33f0: d280000c     	mov	x12, #0
    33f4: d37df10d     	lsl	x13, x8, #3
    33f8: 8b0d0150     	add	x16, x10, x13
    33fc: d1001200     	sub	x0, x16, #4
    3400: 91001151     	add	x17, x10, #4
    3404: 8b0d0161     	add	x1, x11, x13
    3408: d1001022     	sub	x2, x1, #4
    340c: 8b0d0123     	add	x3, x9, x13
    3410: d100106e     	sub	x14, x3, #4
    3414: 91001164     	add	x4, x11, #4
    3418: eb0e015f     	cmp	x10, x14
    341c: fa403122     	ccmp	x9, x0, #2, lo
    3420: 1a9f27ed     	cset	w13, lo
    3424: eb0e017f     	cmp	x11, x14
    3428: fa423122     	ccmp	x9, x2, #2, lo
    342c: 1a9f27ee     	cset	w14, lo
    3430: eb01023f     	cmp	x17, x1
    3434: fa503082     	ccmp	x4, x16, #2, lo
    3438: 1a9f27ef     	cset	w15, lo
    343c: eb03023f     	cmp	x17, x3
    3440: 91001131     	add	x17, x9, #4
    3444: fa503222     	ccmp	x17, x16, #2, lo
    3448: 1a9f27f0     	cset	w16, lo
    344c: eb03009f     	cmp	x4, x3
    3450: fa413222     	ccmp	x17, x1, #2, lo
    3454: 1a9f27f1     	cset	w17, lo
    3458: eb00017f     	cmp	x11, x0
    345c: fa423142     	ccmp	x10, x2, #2, lo
    3460: 54000303     	b.lo	0x34c0 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x848>
    3464: 370002ed     	tbnz	w13, #0, 0x34c0 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x848>
    3468: 370002ce     	tbnz	w14, #0, 0x34c0 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x848>
    346c: 370002af     	tbnz	w15, #0, 0x34c0 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x848>
    3470: 37000290     	tbnz	w16, #0, 0x34c0 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x848>
    3474: 37000271     	tbnz	w17, #0, 0x34c0 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x848>
    3478: 927e750c     	and	x12, x8, #0xfffffffc
    347c: aa0c03ed     	mov	x13, x12
    3480: aa0b03ee     	mov	x14, x11
    3484: aa0a03ef     	mov	x15, x10
    3488: aa0903f0     	mov	x16, x9
    348c: acc10600     	ldp	q0, q1, [x16], #32
    3490: ad400de2     	ldp	q2, q3, [x15]
    3494: 4e23d421     	fadd.4s	v1, v1, v3
    3498: 4e22d400     	fadd.4s	v0, v0, v2
    349c: ac8105e0     	stp	q0, q1, [x15], #32
    34a0: ad400dc2     	ldp	q2, q3, [x14]
    34a4: 4e23d421     	fadd.4s	v1, v1, v3
    34a8: 4e22d400     	fadd.4s	v0, v0, v2
    34ac: ac8105c0     	stp	q0, q1, [x14], #32
    34b0: f10011ad     	subs	x13, x13, #4
    34b4: 54fffec1     	b.ne	0x348c <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x814>
    34b8: eb08019f     	cmp	x12, x8
    34bc: 540001e0     	b.eq	0x34f8 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x880>
    34c0: d37df18d     	lsl	x13, x12, #3
    34c4: 8b0d016b     	add	x11, x11, x13
    34c8: 8b0d014a     	add	x10, x10, x13
    34cc: 8b0d0129     	add	x9, x9, x13
    34d0: cb0c0108     	sub	x8, x8, x12
    34d4: fc408520     	ldr	d0, [x9], #8
    34d8: fd400141     	ldr	d1, [x10]
    34dc: 0e21d400     	fadd.2s	v0, v0, v1
    34e0: fc008540     	str	d0, [x10], #8
    34e4: fd400161     	ldr	d1, [x11]
    34e8: 0e21d400     	fadd.2s	v0, v0, v1
    34ec: fc008560     	str	d0, [x11], #8
    34f0: f1000508     	subs	x8, x8, #1
    34f4: 54ffff01     	b.ne	0x34d4 <__ZN4Game21performRK4IntegrationER16PointMassesRangeR5RangeI6SpringERS2_IiEbR18ConsoleProfileInfo+0x85c>
    34f8: a9507bfd     	ldp	x29, x30, [sp, #256]
    34fc: a94f4ff4     	ldp	x20, x19, [sp, #240]
    3500: a94e57f6     	ldp	x22, x21, [sp, #224]
    3504: a94d5ff8     	ldp	x24, x23, [sp, #208]
    3508: a94c67fa     	ldp	x26, x25, [sp, #192]
    350c: a94b6ffc     	ldp	x28, x27, [sp, #176]
    3510: 6d4a23e9     	ldp	d9, d8, [sp, #160]
    3514: 6d492beb     	ldp	d11, d10, [sp, #144]
    3518: 910443ff     	add	sp, sp, #272
    351c: d65f03c0     	ret

0000000000003520 <__ZN4Game6updateEdR18ConsoleProfileInfo>:
    3520: d10383ff     	sub	sp, sp, #224
    3524: 6d062beb     	stp	d11, d10, [sp, #96]
    3528: 6d0723e9     	stp	d9, d8, [sp, #112]
    352c: a9086ffc     	stp	x28, x27, [sp, #128]
    3530: a90967fa     	stp	x26, x25, [sp, #144]
    3534: a90a5ff8     	stp	x24, x23, [sp, #160]
    3538: a90b57f6     	stp	x22, x21, [sp, #176]
    353c: a90c4ff4     	stp	x20, x19, [sp, #192]
    3540: a90d7bfd     	stp	x29, x30, [sp, #208]
    3544: 910343fd     	add	x29, sp, #208
    3548: aa0103f4     	mov	x20, x1
    354c: 1e604008     	fmov	d8, d0
    3550: aa0003f3     	mov	x19, x0
    3554: f9400008     	ldr	x8, [x0]
    3558: 5296b709     	mov	w9, #46520
    355c: fc696900     	ldr	d0, [x8, x9]
    3560: 1e682800     	fadd	d0, d0, d8
    3564: fc296900     	str	d0, [x8, x9]
    3568: fd000428     	str	d8, [x1, #8]
    356c: 1e6e1009     	fmov	d9, #1.00000000
    3570: 1e692000     	fcmp	d0, d9
    3574: 5400004b     	b.lt	0x357c <__ZN4Game6updateEdR18ConsoleProfileInfo+0x5c>
    3578: 94000000     	bl	0x3578 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x58>
    357c: 910163e0     	add	x0, sp, #88
    3580: 94000000     	bl	0x3580 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x60>
    3584: f9400268     	ldr	x8, [x19]
    3588: 5296b709     	mov	w9, #46520
    358c: fc696900     	ldr	d0, [x8, x9]
    3590: 52800016     	mov	w22, #0
    3594: 1e692000     	fcmp	d0, d9
    3598: 54000cad     	b.le	0x372c <__ZN4Game6updateEdR18ConsoleProfileInfo+0x20c>
    359c: 52968317     	mov	w23, #46104
    35a0: 52800058     	mov	w24, #2
    35a4: 5296b719     	mov	w25, #46520
    35a8: 1e7e1009     	fmov	d9, #-1.00000000
    35ac: 1e6e100a     	fmov	d10, #1.00000000
    35b0: 8b17011a     	add	x26, x8, x23
    35b4: b940035b     	ldr	w27, [x26]
    35b8: f9400748     	ldr	x8, [x26, #8]
    35bc: a901efe8     	stp	x8, x27, [sp, #24]
    35c0: f9400f48     	ldr	x8, [x26, #24]
    35c4: a902efe8     	stp	x8, x27, [sp, #40]
    35c8: f9401748     	ldr	x8, [x26, #40]
    35cc: a903efe8     	stp	x8, x27, [sp, #56]
    35d0: f9401f48     	ldr	x8, [x26, #56]
    35d4: a904efe8     	stp	x8, x27, [sp, #72]
    35d8: b9409348     	ldr	w8, [x26, #144]
    35dc: f9404f49     	ldr	x9, [x26, #152]
    35e0: a900a3e9     	stp	x9, x8, [sp, #8]
    35e4: b940b748     	ldr	w8, [x26, #180]
    35e8: 7100051f     	cmp	w8, #1
    35ec: 5400006b     	b.lt	0x35f8 <__ZN4Game6updateEdR18ConsoleProfileInfo+0xd8>
    35f0: 6b1b011f     	cmp	w8, w27
    35f4: 5400056a     	b.ge	0x36a0 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x180>
    35f8: 71000b7f     	cmp	w27, #2
    35fc: 1a98c37c     	csel	w28, w27, w24, gt
    3600: d37e7f80     	ubfiz	x0, x28, #2, #32
    3604: 94000000     	bl	0x3604 <__ZN4Game6updateEdR18ConsoleProfileInfo+0xe4>
    3608: aa0003f5     	mov	x21, x0
    360c: b940b348     	ldr	w8, [x26, #176]
    3610: f9405f40     	ldr	x0, [x26, #184]
    3614: 7100051f     	cmp	w8, #1
    3618: 540003ab     	b.lt	0x368c <__ZN4Game6updateEdR18ConsoleProfileInfo+0x16c>
    361c: d2800009     	mov	x9, #0
    3620: 7100411f     	cmp	w8, #16
    3624: 54000203     	b.lo	0x3664 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x144>
    3628: cb0002aa     	sub	x10, x21, x0
    362c: f101015f     	cmp	x10, #64
    3630: 540001a3     	b.lo	0x3664 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x144>
    3634: 927c6d09     	and	x9, x8, #0xfffffff0
    3638: 9100800a     	add	x10, x0, #32
    363c: 910082ab     	add	x11, x21, #32
    3640: aa0903ec     	mov	x12, x9
    3644: ad7f0540     	ldp	q0, q1, [x10, #-32]
    3648: acc20d42     	ldp	q2, q3, [x10], #64
    364c: ad3f0560     	stp	q0, q1, [x11, #-32]
    3650: ac820d62     	stp	q2, q3, [x11], #64
    3654: f100418c     	subs	x12, x12, #16
    3658: 54ffff61     	b.ne	0x3644 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x124>
    365c: eb08013f     	cmp	x9, x8
    3660: 54000120     	b.eq	0x3684 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x164>
    3664: cb090108     	sub	x8, x8, x9
    3668: d37ef52a     	lsl	x10, x9, #2
    366c: 8b0a0009     	add	x9, x0, x10
    3670: 8b0a02aa     	add	x10, x21, x10
    3674: b840452b     	ldr	w11, [x9], #4
    3678: b800454b     	str	w11, [x10], #4
    367c: f1000508     	subs	x8, x8, #1
    3680: 54ffffa1     	b.ne	0x3674 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x154>
    3684: b900b35f     	str	wzr, [x26, #176]
    3688: 14000003     	b	0x3694 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x174>
    368c: b900b35f     	str	wzr, [x26, #176]
    3690: b4000040     	cbz	x0, 0x3698 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x178>
    3694: 94000000     	bl	0x3694 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x174>
    3698: f9005f55     	str	x21, [x26, #184]
    369c: b900b75c     	str	w28, [x26, #180]
    36a0: b900b35b     	str	w27, [x26, #176]
    36a4: 7100077f     	cmp	w27, #1
    36a8: 5400010b     	b.lt	0x36c8 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x1a8>
    36ac: d2800008     	mov	x8, #0
    36b0: f9405f49     	ldr	x9, [x26, #184]
    36b4: b828793f     	str	wzr, [x9, x8, lsl #2]
    36b8: 91000508     	add	x8, x8, #1
    36bc: b980b34a     	ldrsw	x10, [x26, #176]
    36c0: eb0a011f     	cmp	x8, x10
    36c4: 54ffff8b     	b.lt	0x36b4 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x194>
    36c8: 72000adf     	tst	w22, #0x7
    36cc: 1a9f17e4     	cset	w4, eq
    36d0: 910063e1     	add	x1, sp, #24
    36d4: 910023e2     	add	x2, sp, #8
    36d8: aa1303e0     	mov	x0, x19
    36dc: aa1403e5     	mov	x5, x20
    36e0: 94000000     	bl	0x36e0 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x1c0>
    36e4: f9400268     	ldr	x8, [x19]
    36e8: fc796900     	ldr	d0, [x8, x25]
    36ec: 1e692800     	fadd	d0, d0, d9
    36f0: fc396900     	str	d0, [x8, x25]
    36f4: 110006d6     	add	w22, w22, #1
    36f8: 910163e0     	add	x0, sp, #88
    36fc: 94000000     	bl	0x36fc <__ZN4Game6updateEdR18ConsoleProfileInfo+0x1dc>
    3700: 1e682000     	fcmp	d0, d8
    3704: 540000cc     	b.gt	0x371c <__ZN4Game6updateEdR18ConsoleProfileInfo+0x1fc>
    3708: f9400268     	ldr	x8, [x19]
    370c: fc796900     	ldr	d0, [x8, x25]
    3710: 1e6a2000     	fcmp	d0, d10
    3714: 54fff4ec     	b.gt	0x35b0 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x90>
    3718: 14000005     	b	0x372c <__ZN4Game6updateEdR18ConsoleProfileInfo+0x20c>
    371c: 90000000     	adrp	x0, 0x3000 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x1fc>
    3720: 91000000     	add	x0, x0, #0
    3724: 94000000     	bl	0x3724 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x204>
    3728: f9400268     	ldr	x8, [x19]
    372c: b9001296     	str	w22, [x20, #16]
    3730: 52969509     	mov	w9, #46248
    3734: b8696908     	ldr	w8, [x8, x9]
    3738: b9005288     	str	w8, [x20, #80]
    373c: 910163e0     	add	x0, sp, #88
    3740: 94000000     	bl	0x3740 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x220>
    3744: 1e6202c1     	scvtf	d1, w22
    3748: 1e611800     	fdiv	d0, d0, d1
    374c: fd000280     	str	d0, [x20]
    3750: fd401a80     	ldr	d0, [x20, #48]
    3754: 1e611800     	fdiv	d0, d0, d1
    3758: fd001a80     	str	d0, [x20, #48]
    375c: f9400268     	ldr	x8, [x19]
    3760: 52968009     	mov	w9, #46080
    3764: 8b090115     	add	x21, x8, x9
    3768: b98002a9     	ldrsw	x9, [x21]
    376c: 5280180a     	mov	w10, #192
    3770: 9b2a2136     	smaddl	x22, w9, w10, x8
    3774: b900b2df     	str	wzr, [x22, #176]
    3778: b90002df     	str	wzr, [x22]
    377c: b90052df     	str	wzr, [x22, #80]
    3780: b90062df     	str	wzr, [x22, #96]
    3784: b90072df     	str	wzr, [x22, #112]
    3788: b90082df     	str	wzr, [x22, #128]
    378c: b90092df     	str	wzr, [x22, #144]
    3790: b900a2df     	str	wzr, [x22, #160]
    3794: b9400aa8     	ldr	w8, [x21, #8]
    3798: b94006c9     	ldr	w9, [x22, #4]
    379c: 7100053f     	cmp	w9, #1
    37a0: 7a48a128     	ccmp	w9, w8, #8, ge
    37a4: 540001ea     	b.ge	0x37e0 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x2c0>
    37a8: 52800049     	mov	w9, #2
    37ac: 7100091f     	cmp	w8, #2
    37b0: 1a89c117     	csel	w23, w8, w9, gt
    37b4: 52800308     	mov	w8, #24
    37b8: 9ba87ee0     	umull	x0, w23, w8
    37bc: 94000000     	bl	0x37bc <__ZN4Game6updateEdR18ConsoleProfileInfo+0x29c>
    37c0: aa0003f4     	mov	x20, x0
    37c4: f94006c0     	ldr	x0, [x22, #8]
    37c8: b90002df     	str	wzr, [x22]
    37cc: b4000040     	cbz	x0, 0x37d4 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x2b4>
    37d0: 94000000     	bl	0x37d0 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x2b0>
    37d4: f90006d4     	str	x20, [x22, #8]
    37d8: 29005edf     	stp	wzr, w23, [x22]
    37dc: b9400aa8     	ldr	w8, [x21, #8]
    37e0: 7100051f     	cmp	w8, #1
    37e4: 5400028b     	b.lt	0x3834 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x314>
    37e8: d2800008     	mov	x8, #0
    37ec: d2800009     	mov	x9, #0
    37f0: 5280030a     	mov	w10, #24
    37f4: f94006cb     	ldr	x11, [x22, #8]
    37f8: b98002cc     	ldrsw	x12, [x22]
    37fc: 1100058d     	add	w13, w12, #1
    3800: b90002cd     	str	w13, [x22]
    3804: 9b2a2d8b     	smaddl	x11, w12, w10, x11
    3808: f9400aac     	ldr	x12, [x21, #16]
    380c: 8b08018c     	add	x12, x12, x8
    3810: 3dc00180     	ldr	q0, [x12]
    3814: f940098c     	ldr	x12, [x12, #16]
    3818: f900096c     	str	x12, [x11, #16]
    381c: 3d800160     	str	q0, [x11]
    3820: 91000529     	add	x9, x9, #1
    3824: b9800aab     	ldrsw	x11, [x21, #8]
    3828: 91006108     	add	x8, x8, #24
    382c: eb0b013f     	cmp	x9, x11
    3830: 54fffe2b     	b.lt	0x37f4 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x2d4>
    3834: f9400268     	ldr	x8, [x19]
    3838: 52968009     	mov	w9, #46080
    383c: 8b090115     	add	x21, x8, x9
    3840: b98002a9     	ldrsw	x9, [x21]
    3844: 5280180a     	mov	w10, #192
    3848: 9b2a2136     	smaddl	x22, w9, w10, x8
    384c: b9405aa8     	ldr	w8, [x21, #88]
    3850: 29562ad7     	ldp	w23, w10, [x22, #176]
    3854: 0b170109     	add	w9, w8, w23
    3858: 7100055f     	cmp	w10, #1
    385c: 7a49a148     	ccmp	w10, w9, #8, ge
    3860: 540003aa     	b.ge	0x38d4 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x3b4>
    3864: 52800048     	mov	w8, #2
    3868: 7100093f     	cmp	w9, #2
    386c: 1a88c138     	csel	w24, w9, w8, gt
    3870: 52800308     	mov	w8, #24
    3874: 9ba87f00     	umull	x0, w24, w8
    3878: 94000000     	bl	0x3878 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x358>
    387c: aa0003f4     	mov	x20, x0
    3880: f9405ec0     	ldr	x0, [x22, #184]
    3884: 710006ff     	cmp	w23, #1
    3888: 540001ab     	b.lt	0x38bc <__ZN4Game6updateEdR18ConsoleProfileInfo+0x39c>
    388c: aa1403e8     	mov	x8, x20
    3890: aa0003e9     	mov	x9, x0
    3894: aa1703ea     	mov	x10, x23
    3898: 3dc00120     	ldr	q0, [x9]
    389c: f940092b     	ldr	x11, [x9, #16]
    38a0: f900090b     	str	x11, [x8, #16]
    38a4: 3c818500     	str	q0, [x8], #24
    38a8: 91006129     	add	x9, x9, #24
    38ac: f100054a     	subs	x10, x10, #1
    38b0: 54ffff41     	b.ne	0x3898 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x378>
    38b4: b900b2df     	str	wzr, [x22, #176]
    38b8: 14000003     	b	0x38c4 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x3a4>
    38bc: b900b2df     	str	wzr, [x22, #176]
    38c0: b4000040     	cbz	x0, 0x38c8 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x3a8>
    38c4: 94000000     	bl	0x38c4 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x3a4>
    38c8: f9005ed4     	str	x20, [x22, #184]
    38cc: 291662d7     	stp	w23, w24, [x22, #176]
    38d0: b9405aa8     	ldr	w8, [x21, #88]
    38d4: 7100051f     	cmp	w8, #1
    38d8: 5400028b     	b.lt	0x3928 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x408>
    38dc: d2800008     	mov	x8, #0
    38e0: d2800009     	mov	x9, #0
    38e4: 5280030a     	mov	w10, #24
    38e8: f9405ecb     	ldr	x11, [x22, #184]
    38ec: b980b2cc     	ldrsw	x12, [x22, #176]
    38f0: 1100058d     	add	w13, w12, #1
    38f4: b900b2cd     	str	w13, [x22, #176]
    38f8: 9b2a2d8b     	smaddl	x11, w12, w10, x11
    38fc: f94032ac     	ldr	x12, [x21, #96]
    3900: 8b08018c     	add	x12, x12, x8
    3904: 3dc00180     	ldr	q0, [x12]
    3908: f940098c     	ldr	x12, [x12, #16]
    390c: f900096c     	str	x12, [x11, #16]
    3910: 3d800160     	str	q0, [x11]
    3914: 91000529     	add	x9, x9, #1
    3918: b9805aab     	ldrsw	x11, [x21, #88]
    391c: 91006108     	add	x8, x8, #24
    3920: eb0b013f     	cmp	x9, x11
    3924: 54fffe2b     	b.lt	0x38e8 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x3c8>
    3928: f9400268     	ldr	x8, [x19]
    392c: 52968014     	mov	w20, #46080
    3930: b8b46909     	ldrsw	x9, [x8, x20]
    3934: 52801816     	mov	w22, #192
    3938: 9b362129     	smaddl	x9, w9, w22, x8
    393c: 91014120     	add	x0, x9, #80
    3940: 52968309     	mov	w9, #46104
    3944: 8b090101     	add	x1, x8, x9
    3948: 94000000     	bl	0x3948 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x428>
    394c: f9400268     	ldr	x8, [x19]
    3950: 8b140115     	add	x21, x8, x20
    3954: b98002a9     	ldrsw	x9, [x21]
    3958: 9b362136     	smaddl	x22, w9, w22, x8
    395c: b940aaa8     	ldr	w8, [x21, #168]
    3960: 29522ad7     	ldp	w23, w10, [x22, #144]
    3964: 0b170109     	add	w9, w8, w23
    3968: 7100055f     	cmp	w10, #1
    396c: 7a49a148     	ccmp	w10, w9, #8, ge
    3970: 540003aa     	b.ge	0x39e4 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x4c4>
    3974: 52800048     	mov	w8, #2
    3978: 7100093f     	cmp	w9, #2
    397c: 1a88c138     	csel	w24, w9, w8, gt
    3980: 52800308     	mov	w8, #24
    3984: 9ba87f00     	umull	x0, w24, w8
    3988: 94000000     	bl	0x3988 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x468>
    398c: aa0003f4     	mov	x20, x0
    3990: f9404ec0     	ldr	x0, [x22, #152]
    3994: 710006ff     	cmp	w23, #1
    3998: 540001ab     	b.lt	0x39cc <__ZN4Game6updateEdR18ConsoleProfileInfo+0x4ac>
    399c: aa1403e8     	mov	x8, x20
    39a0: aa0003e9     	mov	x9, x0
    39a4: aa1703ea     	mov	x10, x23
    39a8: 3dc00120     	ldr	q0, [x9]
    39ac: f940092b     	ldr	x11, [x9, #16]
    39b0: f900090b     	str	x11, [x8, #16]
    39b4: 3c818500     	str	q0, [x8], #24
    39b8: 91006129     	add	x9, x9, #24
    39bc: f100054a     	subs	x10, x10, #1
    39c0: 54ffff41     	b.ne	0x39a8 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x488>
    39c4: b90092df     	str	wzr, [x22, #144]
    39c8: 14000003     	b	0x39d4 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x4b4>
    39cc: b90092df     	str	wzr, [x22, #144]
    39d0: b4000040     	cbz	x0, 0x39d8 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x4b8>
    39d4: 94000000     	bl	0x39d4 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x4b4>
    39d8: f9004ed4     	str	x20, [x22, #152]
    39dc: 291262d7     	stp	w23, w24, [x22, #144]
    39e0: b940aaa8     	ldr	w8, [x21, #168]
    39e4: 7100051f     	cmp	w8, #1
    39e8: 5400028b     	b.lt	0x3a38 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x518>
    39ec: d2800008     	mov	x8, #0
    39f0: d2800009     	mov	x9, #0
    39f4: 5280030a     	mov	w10, #24
    39f8: f9404ecb     	ldr	x11, [x22, #152]
    39fc: b98092cc     	ldrsw	x12, [x22, #144]
    3a00: 1100058d     	add	w13, w12, #1
    3a04: b90092cd     	str	w13, [x22, #144]
    3a08: 9b2a2d8b     	smaddl	x11, w12, w10, x11
    3a0c: f9405aac     	ldr	x12, [x21, #176]
    3a10: 8b08018c     	add	x12, x12, x8
    3a14: 3dc00180     	ldr	q0, [x12]
    3a18: f940098c     	ldr	x12, [x12, #16]
    3a1c: f900096c     	str	x12, [x11, #16]
    3a20: 3d800160     	str	q0, [x11]
    3a24: 91000529     	add	x9, x9, #1
    3a28: b980aaab     	ldrsw	x11, [x21, #168]
    3a2c: 91006108     	add	x8, x8, #24
    3a30: eb0b013f     	cmp	x9, x11
    3a34: 54fffe2b     	b.lt	0x39f8 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x4d8>
    3a38: f9400268     	ldr	x8, [x19]
    3a3c: 52968009     	mov	w9, #46080
    3a40: 8b090115     	add	x21, x8, x9
    3a44: b98002a9     	ldrsw	x9, [x21]
    3a48: 5280180a     	mov	w10, #192
    3a4c: 9b2a2136     	smaddl	x22, w9, w10, x8
    3a50: b940baa8     	ldr	w8, [x21, #184]
    3a54: 29542ad7     	ldp	w23, w10, [x22, #160]
    3a58: 0b170109     	add	w9, w8, w23
    3a5c: 7100055f     	cmp	w10, #1
    3a60: 7a49a148     	ccmp	w10, w9, #8, ge
    3a64: 540003aa     	b.ge	0x3ad8 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x5b8>
    3a68: 52800048     	mov	w8, #2
    3a6c: 7100093f     	cmp	w9, #2
    3a70: 1a88c138     	csel	w24, w9, w8, gt
    3a74: 52800188     	mov	w8, #12
    3a78: 9ba87f00     	umull	x0, w24, w8
    3a7c: 94000000     	bl	0x3a7c <__ZN4Game6updateEdR18ConsoleProfileInfo+0x55c>
    3a80: aa0003f4     	mov	x20, x0
    3a84: f94056c0     	ldr	x0, [x22, #168]
    3a88: 710006ff     	cmp	w23, #1
    3a8c: 540001ab     	b.lt	0x3ac0 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x5a0>
    3a90: aa1403e8     	mov	x8, x20
    3a94: aa0003e9     	mov	x9, x0
    3a98: aa1703ea     	mov	x10, x23
    3a9c: f940012b     	ldr	x11, [x9]
    3aa0: b940092c     	ldr	w12, [x9, #8]
    3aa4: b900090c     	str	w12, [x8, #8]
    3aa8: f800c50b     	str	x11, [x8], #12
    3aac: 91003129     	add	x9, x9, #12
    3ab0: f100054a     	subs	x10, x10, #1
    3ab4: 54ffff41     	b.ne	0x3a9c <__ZN4Game6updateEdR18ConsoleProfileInfo+0x57c>
    3ab8: b900a2df     	str	wzr, [x22, #160]
    3abc: 14000003     	b	0x3ac8 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x5a8>
    3ac0: b900a2df     	str	wzr, [x22, #160]
    3ac4: b4000040     	cbz	x0, 0x3acc <__ZN4Game6updateEdR18ConsoleProfileInfo+0x5ac>
    3ac8: 94000000     	bl	0x3ac8 <__ZN4Game6updateEdR18ConsoleProfileInfo+0x5a8>
    3acc: f90056d4     	str	x20, [x22, #168]
    3ad0: 291462d7     	stp	w23, w24, [x22, #160]
    3ad4: b940baa8     	ldr	w8, [x21, #184]
    3ad8: 7100051f     	cmp	w8, #1
    3adc: 5400028b     	b.lt	0x3b2c <__ZN4Game6updateEdR18ConsoleProfileInfo+0x60c>
    3ae0: d2800008     	mov	x8, #0
    3ae4: d2800009     	mov	x9, #0
    3ae8: 5280018a     	mov	w10, #12
    3aec: f94056cb     	ldr	x11, [x22, #168]
    3af0: b980a2cc     	ldrsw	x12, [x22, #160]
    3af4: 1100058d     	add	w13, w12, #1
    3af8: b900a2cd     	str	w13, [x22, #160]
    3afc: 9b2a2d8b     	smaddl	x11, w12, w10, x11
    3b00: f94062ac     	ldr	x12, [x21, #192]
    3b04: 8b08018c     	add	x12, x12, x8
    3b08: f940018d     	ldr	x13, [x12]
    3b0c: b940098c     	ldr	w12, [x12, #8]
    3b10: b900096c     	str	w12, [x11, #8]
    3b14: f900016d     	str	x13, [x11]
    3b18: 91000529     	add	x9, x9, #1
    3b1c: b980baab     	ldrsw	x11, [x21, #184]
    3b20: 91003108     	add	x8, x8, #12
    3b24: eb0b013f     	cmp	x9, x11
    3b28: 54fffe2b     	b.lt	0x3aec <__ZN4Game6updateEdR18ConsoleProfileInfo+0x5cc>
    3b2c: f9400268     	ldr	x8, [x19]
    3b30: 52968009     	mov	w9, #46080
    3b34: 8b090108     	add	x8, x8, x9
    3b38: b9400109     	ldr	w9, [x8]
    3b3c: 1100052a     	add	w10, w9, #1
    3b40: 5291112b     	mov	w11, #34953
    3b44: 72b1110b     	movk	w11, #34952, lsl #16
    3b48: 9b2b7d4b     	smull	x11, w10, w11
    3b4c: d360fd6b     	lsr	x11, x11, #32
    3b50: 0b0a016b     	add	w11, w11, w10
    3b54: 13077d6c     	asr	w12, w11, #7
    3b58: 0b4b7d8b     	add	w11, w12, w11, lsr #31
    3b5c: 52801e0c     	mov	w12, #240
    3b60: 1b0ca96a     	msub	w10, w11, w12, w10
    3b64: 2900250a     	stp	w10, w9, [x8]
    3b68: a94d7bfd     	ldp	x29, x30, [sp, #208]
    3b6c: a94c4ff4     	ldp	x20, x19, [sp, #192]
    3b70: a94b57f6     	ldp	x22, x21, [sp, #176]
    3b74: a94a5ff8     	ldp	x24, x23, [sp, #160]
    3b78: a94967fa     	ldp	x26, x25, [sp, #144]
    3b7c: a9486ffc     	ldp	x28, x27, [sp, #128]
    3b80: 6d4723e9     	ldp	d9, d8, [sp, #112]
    3b84: 6d462beb     	ldp	d11, d10, [sp, #96]
    3b88: 910383ff     	add	sp, sp, #224
    3b8c: d65f03c0     	ret

0000000000003b90 <__ZN4Game15mouseButtonDownEii>:
    3b90: d10103ff     	sub	sp, sp, #64
    3b94: a90157f6     	stp	x22, x21, [sp, #16]
    3b98: a9024ff4     	stp	x20, x19, [sp, #32]
    3b9c: a9037bfd     	stp	x29, x30, [sp, #48]
    3ba0: 9100c3fd     	add	x29, sp, #48
    3ba4: aa0203f3     	mov	x19, x2
    3ba8: aa0103f4     	mov	x20, x1
    3bac: aa0003f5     	mov	x21, x0
    3bb0: a9000be1     	stp	x1, x2, [sp]
    3bb4: 90000000     	adrp	x0, 0x3000 <__ZN4Game15mouseButtonDownEii+0x24>
    3bb8: 91000000     	add	x0, x0, #0
    3bbc: 94000000     	bl	0x3bbc <__ZN4Game15mouseButtonDownEii+0x2c>
    3bc0: f94002aa     	ldr	x10, [x21]
    3bc4: 52968308     	mov	w8, #46104
    3bc8: 8b08014b     	add	x11, x10, x8
    3bcc: b9400168     	ldr	w8, [x11]
    3bd0: 7100051f     	cmp	w8, #1
    3bd4: 540002ab     	b.lt	0x3c28 <__ZN4Game15mouseButtonDownEii+0x98>
    3bd8: d2800009     	mov	x9, #0
    3bdc: 5296b50c     	mov	w12, #46504
    3be0: 8b0c014a     	add	x10, x10, x12
    3be4: f940056b     	ldr	x11, [x11, #8]
    3be8: 1e220280     	scvtf	s0, w20
    3bec: 1e220261     	scvtf	s1, w19
    3bf0: 9100116b     	add	x11, x11, #4
    3bf4: 1e269002     	fmov	s2, #20.00000000
    3bf8: 2d7f9163     	ldp	s3, s4, [x11, #-4]
    3bfc: 1e233803     	fsub	s3, s0, s3
    3c00: 1e243824     	fsub	s4, s1, s4
    3c04: 1e240884     	fmul	s4, s4, s4
    3c08: 1f031063     	fmadd	s3, s3, s3, s4
    3c0c: 1e21c063     	fsqrt	s3, s3
    3c10: 1e222060     	fcmp	s3, s2
    3c14: 54000144     	b.mi	0x3c3c <__ZN4Game15mouseButtonDownEii+0xac>
    3c18: 91000529     	add	x9, x9, #1
    3c1c: 9100216b     	add	x11, x11, #8
    3c20: eb09011f     	cmp	x8, x9
    3c24: 54fffea1     	b.ne	0x3bf8 <__ZN4Game15mouseButtonDownEii+0x68>
    3c28: a9437bfd     	ldp	x29, x30, [sp, #48]
    3c2c: a9424ff4     	ldp	x20, x19, [sp, #32]
    3c30: a94157f6     	ldp	x22, x21, [sp, #16]
    3c34: 910103ff     	add	sp, sp, #64
    3c38: d65f03c0     	ret
    3c3c: b9000149     	str	w9, [x10]
    3c40: 2d008540     	stp	s0, s1, [x10, #4]
    3c44: a9437bfd     	ldp	x29, x30, [sp, #48]
    3c48: a9424ff4     	ldp	x20, x19, [sp, #32]
    3c4c: a94157f6     	ldp	x22, x21, [sp, #16]
    3c50: 910103ff     	add	sp, sp, #64
    3c54: d65f03c0     	ret

0000000000003c58 <__ZN4Game13mouseButtonUpEii>:
    3c58: f9400008     	ldr	x8, [x0]
    3c5c: 5296b509     	mov	w9, #46504
    3c60: 1280000a     	mov	w10, #-1
    3c64: b829690a     	str	w10, [x8, x9]
    3c68: d65f03c0     	ret

0000000000003c6c <__ZN4Game9mouseMoveEii>:
    3c6c: f9400009     	ldr	x9, [x0]
    3c70: 5296b508     	mov	w8, #46504
    3c74: b8a86928     	ldrsw	x8, [x9, x8]
    3c78: 3100051f     	cmn	w8, #1
    3c7c: 54000300     	b.eq	0x3cdc <__ZN4Game9mouseMoveEii+0x70>
    3c80: 5296840a     	mov	w10, #46112
    3c84: 8b0a0129     	add	x9, x9, x10
    3c88: 1e220020     	scvtf	s0, w1
    3c8c: 1e220041     	scvtf	s1, w2
    3c90: f9400129     	ldr	x9, [x9]
    3c94: d37df108     	lsl	x8, x8, #3
    3c98: 8b080129     	add	x9, x9, x8
    3c9c: bd000120     	str	s0, [x9]
    3ca0: bd000521     	str	s1, [x9, #4]
    3ca4: f9400009     	ldr	x9, [x0]
    3ca8: 5296880a     	mov	w10, #46144
    3cac: f86a6929     	ldr	x9, [x9, x10]
    3cb0: 6f00e402     	movi.2d	v2, #0000000000000000
    3cb4: fc286922     	str	d2, [x9, x8]
    3cb8: f9400009     	ldr	x9, [x0]
    3cbc: 52968a0a     	mov	w10, #46160
    3cc0: f86a6929     	ldr	x9, [x9, x10]
    3cc4: fc286922     	str	d2, [x9, x8]
    3cc8: f9400008     	ldr	x8, [x0]
    3ccc: 5296b589     	mov	w9, #46508
    3cd0: 8b090108     	add	x8, x8, x9
    3cd4: bd000100     	str	s0, [x8]
    3cd8: bd000501     	str	s1, [x8, #4]
    3cdc: d65f03c0     	ret

0000000000003ce0 <__ZN4Game6shapesEv>:
    3ce0: f9400008     	ldr	x8, [x0]
    3ce4: 52968109     	mov	w9, #46088
    3ce8: 8b090100     	add	x0, x8, x9
    3cec: d65f03c0     	ret

0000000000003cf0 <__ZN4Game14nextShapeIndexEv>:
    3cf0: f9400008     	ldr	x8, [x0]
    3cf4: 52968109     	mov	w9, #46088
    3cf8: b8696900     	ldr	w0, [x8, x9]
    3cfc: d65f03c0     	ret

0000000000003d00 <__ZN4Game20nextStaticShapeIndexEv>:
    3d00: f9400008     	ldr	x8, [x0]
    3d04: 52968b09     	mov	w9, #46168
    3d08: b8696900     	ldr	w0, [x8, x9]
    3d0c: d65f03c0     	ret

0000000000003d10 <__ZN4Game12staticShapesEv>:
    3d10: f9400008     	ldr	x8, [x0]
    3d14: 52968b09     	mov	w9, #46168
    3d18: 8b090100     	add	x0, x8, x9
    3d1c: d65f03c0     	ret

0000000000003d20 <__ZN4Game6pointsEv>:
    3d20: f9400008     	ldr	x8, [x0]
    3d24: 52968309     	mov	w9, #46104
    3d28: 8b090100     	add	x0, x8, x9
    3d2c: d65f03c0     	ret

0000000000003d30 <__ZN4Game12staticPointsEv>:
    3d30: f9400008     	ldr	x8, [x0]
    3d34: 52968d09     	mov	w9, #46184
    3d38: 8b090100     	add	x0, x8, x9
    3d3c: d65f03c0     	ret

0000000000003d40 <__ZN4Game7springsEv>:
    3d40: f9400008     	ldr	x8, [x0]
    3d44: 52969509     	mov	w9, #46248
    3d48: 8b090100     	add	x0, x8, x9
    3d4c: d65f03c0     	ret

0000000000003d50 <__ZN4Game12staticJointsEv>:
    3d50: f9400008     	ldr	x8, [x0]
    3d54: 52969709     	mov	w9, #46264
    3d58: 8b090100     	add	x0, x8, x9
    3d5c: d65f03c0     	ret

0000000000003d60 <__ZN4Game21testSpringPerformanceEi>:
    3d60: d10483ff     	sub	sp, sp, #288
    3d64: 6d0b2beb     	stp	d11, d10, [sp, #176]
    3d68: 6d0c23e9     	stp	d9, d8, [sp, #192]
    3d6c: a90d6ffc     	stp	x28, x27, [sp, #208]
    3d70: a90e5ff8     	stp	x24, x23, [sp, #224]
    3d74: a90f57f6     	stp	x22, x21, [sp, #240]
    3d78: a9104ff4     	stp	x20, x19, [sp, #256]
    3d7c: a9117bfd     	stp	x29, x30, [sp, #272]
    3d80: 910443fd     	add	x29, sp, #272
    3d84: aa0103f3     	mov	x19, x1
    3d88: aa0003f4     	mov	x20, x0
    3d8c: f9400008     	ldr	x8, [x0]
    3d90: 5296b309     	mov	w9, #46488
    3d94: 8b090116     	add	x22, x8, x9
    3d98: 94000000     	bl	0x3d98 <__ZN4Game21testSpringPerformanceEi+0x38>
    3d9c: 1e204008     	fmov	s8, s0
    3da0: 1e204029     	fmov	s9, s1
    3da4: 94000000     	bl	0x3da4 <__ZN4Game21testSpringPerformanceEi+0x44>
    3da8: 1e20400a     	fmov	s10, s0
    3dac: 1e20402b     	fmov	s11, s1
    3db0: f9400288     	ldr	x8, [x20]
    3db4: 52968309     	mov	w9, #46104
    3db8: b8696917     	ldr	w23, [x8, x9]
    3dbc: b94006c8     	ldr	w8, [x22, #4]
    3dc0: 7100051f     	cmp	w8, #1
    3dc4: 7a57a108     	ccmp	w8, w23, #8, ge
    3dc8: 5400030a     	b.ge	0x3e28 <__ZN4Game21testSpringPerformanceEi+0xc8>
    3dcc: 52800048     	mov	w8, #2
    3dd0: 71000aff     	cmp	w23, #2
    3dd4: 1a88c2f8     	csel	w24, w23, w8, gt
    3dd8: d37c7f00     	ubfiz	x0, x24, #4, #32
    3ddc: 94000000     	bl	0x3ddc <__ZN4Game21testSpringPerformanceEi+0x7c>
    3de0: aa0003f5     	mov	x21, x0
    3de4: b94002c8     	ldr	w8, [x22]
    3de8: f94006c0     	ldr	x0, [x22, #8]
    3dec: 7100051f     	cmp	w8, #1
    3df0: 5400012b     	b.lt	0x3e14 <__ZN4Game21testSpringPerformanceEi+0xb4>
    3df4: aa1503e9     	mov	x9, x21
    3df8: aa0003ea     	mov	x10, x0
    3dfc: 3cc10540     	ldr	q0, [x10], #16
    3e00: 3c810520     	str	q0, [x9], #16
    3e04: f1000508     	subs	x8, x8, #1
    3e08: 54ffffa1     	b.ne	0x3dfc <__ZN4Game21testSpringPerformanceEi+0x9c>
    3e0c: b90002df     	str	wzr, [x22]
    3e10: 14000003     	b	0x3e1c <__ZN4Game21testSpringPerformanceEi+0xbc>
    3e14: b90002df     	str	wzr, [x22]
    3e18: b4000040     	cbz	x0, 0x3e20 <__ZN4Game21testSpringPerformanceEi+0xc0>
    3e1c: 94000000     	bl	0x3e1c <__ZN4Game21testSpringPerformanceEi+0xbc>
    3e20: f90006d5     	str	x21, [x22, #8]
    3e24: b90006d8     	str	w24, [x22, #4]
    3e28: b90002d7     	str	w23, [x22]
    3e2c: 710006ff     	cmp	w23, #1
    3e30: 540001cb     	b.lt	0x3e68 <__ZN4Game21testSpringPerformanceEi+0x108>
    3e34: d2800008     	mov	x8, #0
    3e38: d2800009     	mov	x9, #0
    3e3c: f94006ca     	ldr	x10, [x22, #8]
    3e40: 8b08014a     	add	x10, x10, x8
    3e44: bd000148     	str	s8, [x10]
    3e48: bd000549     	str	s9, [x10, #4]
    3e4c: bd00094a     	str	s10, [x10, #8]
    3e50: bd000d4b     	str	s11, [x10, #12]
    3e54: 91000529     	add	x9, x9, #1
    3e58: b98002ca     	ldrsw	x10, [x22]
    3e5c: 91004108     	add	x8, x8, #16
    3e60: eb0a013f     	cmp	x9, x10
    3e64: 54fffecb     	b.lt	0x3e3c <__ZN4Game21testSpringPerformanceEi+0xdc>
    3e68: f9400288     	ldr	x8, [x20]
    3e6c: 52968309     	mov	w9, #46104
    3e70: 8b090109     	add	x9, x8, x9
    3e74: b940012a     	ldr	w10, [x9]
    3e78: f940052b     	ldr	x11, [x9, #8]
    3e7c: a901abeb     	stp	x11, x10, [sp, #24]
    3e80: f9400d2b     	ldr	x11, [x9, #24]
    3e84: a902abeb     	stp	x11, x10, [sp, #40]
    3e88: f940152b     	ldr	x11, [x9, #40]
    3e8c: a903abeb     	stp	x11, x10, [sp, #56]
    3e90: f9401d2b     	ldr	x11, [x9, #56]
    3e94: a904abeb     	stp	x11, x10, [sp, #72]
    3e98: b940912a     	ldr	w10, [x9, #144]
    3e9c: f9404d29     	ldr	x9, [x9, #152]
    3ea0: a900abe9     	stp	x9, x10, [sp, #8]
    3ea4: 5296b309     	mov	w9, #46488
    3ea8: 8b090103     	add	x3, x8, x9
    3eac: 5296a315     	mov	w21, #46360
    3eb0: 8b150104     	add	x4, x8, x21
    3eb4: 910063e1     	add	x1, sp, #24
    3eb8: 2f00e400     	movi	d0, #0000000000000000
    3ebc: aa1403e0     	mov	x0, x20
    3ec0: 94000000     	bl	0x3ec0 <__ZN4Game21testSpringPerformanceEi+0x160>
    3ec4: 7100067f     	cmp	w19, #1
    3ec8: 540001ab     	b.lt	0x3efc <__ZN4Game21testSpringPerformanceEi+0x19c>
    3ecc: f9400288     	ldr	x8, [x20]
    3ed0: 8b150108     	add	x8, x8, x21
    3ed4: b9400114     	ldr	w20, [x8]
    3ed8: f9400515     	ldr	x21, [x8, #8]
    3edc: 910063e1     	add	x1, sp, #24
    3ee0: 910023e2     	add	x2, sp, #8
    3ee4: 910163e5     	add	x5, sp, #88
    3ee8: aa1503e3     	mov	x3, x21
    3eec: aa1403e4     	mov	x4, x20
    3ef0: 94000000     	bl	0x3ef0 <__ZN4Game21testSpringPerformanceEi+0x190>
    3ef4: 71000673     	subs	w19, w19, #1
    3ef8: 54ffff21     	b.ne	0x3edc <__ZN4Game21testSpringPerformanceEi+0x17c>
    3efc: a9517bfd     	ldp	x29, x30, [sp, #272]
    3f00: a9504ff4     	ldp	x20, x19, [sp, #256]
    3f04: a94f57f6     	ldp	x22, x21, [sp, #240]
    3f08: a94e5ff8     	ldp	x24, x23, [sp, #224]
    3f0c: a94d6ffc     	ldp	x28, x27, [sp, #208]
    3f10: 6d4c23e9     	ldp	d9, d8, [sp, #192]
    3f14: 6d4b2beb     	ldp	d11, d10, [sp, #176]
    3f18: 910483ff     	add	sp, sp, #288
    3f1c: d65f03c0     	ret

0000000000003f20 <__ZN4Game18testRK4PerformanceEi>:
    3f20: d102c3ff     	sub	sp, sp, #176
    3f24: 6d052beb     	stp	d11, d10, [sp, #80]
    3f28: 6d0623e9     	stp	d9, d8, [sp, #96]
    3f2c: a9075ff8     	stp	x24, x23, [sp, #112]
    3f30: a90857f6     	stp	x22, x21, [sp, #128]
    3f34: a9094ff4     	stp	x20, x19, [sp, #144]
    3f38: a90a7bfd     	stp	x29, x30, [sp, #160]
    3f3c: 910283fd     	add	x29, sp, #160
    3f40: aa0103f3     	mov	x19, x1
    3f44: aa0003f4     	mov	x20, x0
    3f48: f9400008     	ldr	x8, [x0]
    3f4c: 5296b309     	mov	w9, #46488
    3f50: 8b090116     	add	x22, x8, x9
    3f54: 94000000     	bl	0x3f54 <__ZN4Game18testRK4PerformanceEi+0x34>
    3f58: 1e204008     	fmov	s8, s0
    3f5c: 1e204029     	fmov	s9, s1
    3f60: 94000000     	bl	0x3f60 <__ZN4Game18testRK4PerformanceEi+0x40>
    3f64: 1e20400a     	fmov	s10, s0
    3f68: 1e20402b     	fmov	s11, s1
    3f6c: f9400288     	ldr	x8, [x20]
    3f70: 52968309     	mov	w9, #46104
    3f74: b8696917     	ldr	w23, [x8, x9]
    3f78: b94006c8     	ldr	w8, [x22, #4]
    3f7c: 7100051f     	cmp	w8, #1
    3f80: 7a57a108     	ccmp	w8, w23, #8, ge
    3f84: 5400030a     	b.ge	0x3fe4 <__ZN4Game18testRK4PerformanceEi+0xc4>
    3f88: 52800048     	mov	w8, #2
    3f8c: 71000aff     	cmp	w23, #2
    3f90: 1a88c2f8     	csel	w24, w23, w8, gt
    3f94: d37c7f00     	ubfiz	x0, x24, #4, #32
    3f98: 94000000     	bl	0x3f98 <__ZN4Game18testRK4PerformanceEi+0x78>
    3f9c: aa0003f5     	mov	x21, x0
    3fa0: b94002c8     	ldr	w8, [x22]
    3fa4: f94006c0     	ldr	x0, [x22, #8]
    3fa8: 7100051f     	cmp	w8, #1
    3fac: 5400012b     	b.lt	0x3fd0 <__ZN4Game18testRK4PerformanceEi+0xb0>
    3fb0: aa1503e9     	mov	x9, x21
    3fb4: aa0003ea     	mov	x10, x0
    3fb8: 3cc10540     	ldr	q0, [x10], #16
    3fbc: 3c810520     	str	q0, [x9], #16
    3fc0: f1000508     	subs	x8, x8, #1
    3fc4: 54ffffa1     	b.ne	0x3fb8 <__ZN4Game18testRK4PerformanceEi+0x98>
    3fc8: b90002df     	str	wzr, [x22]
    3fcc: 14000003     	b	0x3fd8 <__ZN4Game18testRK4PerformanceEi+0xb8>
    3fd0: b90002df     	str	wzr, [x22]
    3fd4: b4000040     	cbz	x0, 0x3fdc <__ZN4Game18testRK4PerformanceEi+0xbc>
    3fd8: 94000000     	bl	0x3fd8 <__ZN4Game18testRK4PerformanceEi+0xb8>
    3fdc: f90006d5     	str	x21, [x22, #8]
    3fe0: b90006d8     	str	w24, [x22, #4]
    3fe4: b90002d7     	str	w23, [x22]
    3fe8: 710006ff     	cmp	w23, #1
    3fec: 540001cb     	b.lt	0x4024 <__ZN4Game18testRK4PerformanceEi+0x104>
    3ff0: d2800008     	mov	x8, #0
    3ff4: d2800009     	mov	x9, #0
    3ff8: f94006ca     	ldr	x10, [x22, #8]
    3ffc: 8b08014a     	add	x10, x10, x8
    4000: bd000148     	str	s8, [x10]
    4004: bd000549     	str	s9, [x10, #4]
    4008: bd00094a     	str	s10, [x10, #8]
    400c: bd000d4b     	str	s11, [x10, #12]
    4010: 91000529     	add	x9, x9, #1
    4014: b98002ca     	ldrsw	x10, [x22]
    4018: 91004108     	add	x8, x8, #16
    401c: eb0a013f     	cmp	x9, x10
    4020: 54fffecb     	b.lt	0x3ff8 <__ZN4Game18testRK4PerformanceEi+0xd8>
    4024: f9400288     	ldr	x8, [x20]
    4028: 52968309     	mov	w9, #46104
    402c: 8b090108     	add	x8, x8, x9
    4030: f9400509     	ldr	x9, [x8, #8]
    4034: b940010a     	ldr	w10, [x8]
    4038: a9012be9     	stp	x9, x10, [sp, #16]
    403c: f9400d09     	ldr	x9, [x8, #24]
    4040: a9022be9     	stp	x9, x10, [sp, #32]
    4044: f9401509     	ldr	x9, [x8, #40]
    4048: a9032be9     	stp	x9, x10, [sp, #48]
    404c: f9401d08     	ldr	x8, [x8, #56]
    4050: a9042be8     	stp	x8, x10, [sp, #64]
    4054: f90003ea     	str	x10, [sp]
    4058: 90000000     	adrp	x0, 0x4000 <__ZN4Game18testRK4PerformanceEi+0x138>
    405c: 91000000     	add	x0, x0, #0
    4060: 94000000     	bl	0x4060 <__ZN4Game18testRK4PerformanceEi+0x140>
    4064: 7100067f     	cmp	w19, #1
    4068: 5400018b     	b.lt	0x4098 <__ZN4Game18testRK4PerformanceEi+0x178>
    406c: 5296b315     	mov	w21, #46488
    4070: 5296a316     	mov	w22, #46360
    4074: f9400288     	ldr	x8, [x20]
    4078: 8b150103     	add	x3, x8, x21
    407c: 8b160104     	add	x4, x8, x22
    4080: 910043e1     	add	x1, sp, #16
    4084: 2f00e400     	movi	d0, #0000000000000000
    4088: aa1403e0     	mov	x0, x20
    408c: 94000000     	bl	0x408c <__ZN4Game18testRK4PerformanceEi+0x16c>
    4090: 71000673     	subs	w19, w19, #1
    4094: 54ffff01     	b.ne	0x4074 <__ZN4Game18testRK4PerformanceEi+0x154>
    4098: a94a7bfd     	ldp	x29, x30, [sp, #160]
    409c: a9494ff4     	ldp	x20, x19, [sp, #144]
    40a0: a94857f6     	ldp	x22, x21, [sp, #128]
    40a4: a9475ff8     	ldp	x24, x23, [sp, #112]
    40a8: 6d4623e9     	ldp	d9, d8, [sp, #96]
    40ac: 6d452beb     	ldp	d11, d10, [sp, #80]
    40b0: 9102c3ff     	add	sp, sp, #176
    40b4: d65f03c0     	ret

00000000000040b8 <__ZN4Game5clearEv>:
    40b8: f9400008     	ldr	x8, [x0]
    40bc: 52968109     	mov	w9, #46088
    40c0: 8b090109     	add	x9, x8, x9
    40c4: b900013f     	str	wzr, [x9]
    40c8: 5296b50a     	mov	w10, #46504
    40cc: 8b0a0108     	add	x8, x8, x10
    40d0: b900113f     	str	wzr, [x9, #16]
    40d4: b900213f     	str	wzr, [x9, #32]
    40d8: b900313f     	str	wzr, [x9, #48]
    40dc: b900413f     	str	wzr, [x9, #64]
    40e0: b900513f     	str	wzr, [x9, #80]
    40e4: b900613f     	str	wzr, [x9, #96]
    40e8: b900713f     	str	wzr, [x9, #112]
    40ec: b900813f     	str	wzr, [x9, #128]
    40f0: b900913f     	str	wzr, [x9, #144]
    40f4: b900a13f     	str	wzr, [x9, #160]
    40f8: b900b13f     	str	wzr, [x9, #176]
    40fc: b900c13f     	str	wzr, [x9, #192]
    4100: b900d13f     	str	wzr, [x9, #208]
    4104: b900e13f     	str	wzr, [x9, #224]
    4108: b900f13f     	str	wzr, [x9, #240]
    410c: b901013f     	str	wzr, [x9, #256]
    4110: 12800009     	mov	w9, #-1
    4114: b9000109     	str	w9, [x8]
    4118: 52800029     	mov	w9, #1
    411c: 39003109     	strb	w9, [x8, #12]
    4120: d65f03c0     	ret

0000000000004124 <__ZN4Game4ImplC2Ev>:
    4124: a9bb67fa     	stp	x26, x25, [sp, #-80]!
    4128: a9015ff8     	stp	x24, x23, [sp, #16]
    412c: a90257f6     	stp	x22, x21, [sp, #32]
    4130: a9034ff4     	stp	x20, x19, [sp, #48]
    4134: a9047bfd     	stp	x29, x30, [sp, #64]
    4138: 910103fd     	add	x29, sp, #64
    413c: aa0003f3     	mov	x19, x0
    4140: 5296b508     	mov	w8, #46504
    4144: 8b080017     	add	x23, x0, x8
    4148: 52968108     	mov	w8, #46088
    414c: 8b080018     	add	x24, x0, x8
    4150: 52968308     	mov	w8, #46104
    4154: 8b080014     	add	x20, x0, x8
    4158: 52968d08     	mov	w8, #46184
    415c: 8b080015     	add	x21, x0, x8
    4160: 2900feff     	stp	wzr, wzr, [x23, #4]
    4164: 5296b501     	mov	w1, #46504
    4168: 94000000     	bl	0x4168 <__ZN4Game4ImplC2Ev+0x44>
    416c: 52802028     	mov	w8, #257
    4170: 79001ae8     	strh	w8, [x23, #12]
    4174: f9000aff     	str	xzr, [x23, #16]
    4178: 52807800     	mov	w0, #960
    417c: 94000000     	bl	0x417c <__ZN4Game4ImplC2Ev+0x58>
    4180: f9000700     	str	x0, [x24, #8]
    4184: 90000008     	adrp	x8, 0x4000 <__ZN4Game4ImplC2Ev+0x60>
    4188: fd400100     	ldr	d0, [x8]
    418c: fd000300     	str	d0, [x24]
    4190: aa1403e0     	mov	x0, x20
    4194: 52808001     	mov	w1, #1024
    4198: 94000000     	bl	0x4198 <__ZN4Game4ImplC2Ev+0x74>
    419c: b9405708     	ldr	w8, [x24, #84]
    41a0: 71009d1f     	cmp	w8, #39
    41a4: 5400034c     	b.gt	0x420c <__ZN4Game4ImplC2Ev+0xe8>
    41a8: 52807800     	mov	w0, #960
    41ac: 94000000     	bl	0x41ac <__ZN4Game4ImplC2Ev+0x88>
    41b0: aa0003f6     	mov	x22, x0
    41b4: b9405319     	ldr	w25, [x24, #80]
    41b8: f9402f00     	ldr	x0, [x24, #88]
    41bc: 7100073f     	cmp	w25, #1
    41c0: 540001ab     	b.lt	0x41f4 <__ZN4Game4ImplC2Ev+0xd0>
    41c4: aa1603e8     	mov	x8, x22
    41c8: aa0003e9     	mov	x9, x0
    41cc: aa1903ea     	mov	x10, x25
    41d0: 3dc00120     	ldr	q0, [x9]
    41d4: f940092b     	ldr	x11, [x9, #16]
    41d8: f900090b     	str	x11, [x8, #16]
    41dc: 3c818500     	str	q0, [x8], #24
    41e0: 91006129     	add	x9, x9, #24
    41e4: f100054a     	subs	x10, x10, #1
    41e8: 54ffff41     	b.ne	0x41d0 <__ZN4Game4ImplC2Ev+0xac>
    41ec: b900531f     	str	wzr, [x24, #80]
    41f0: 14000003     	b	0x41fc <__ZN4Game4ImplC2Ev+0xd8>
    41f4: b900531f     	str	wzr, [x24, #80]
    41f8: b4000040     	cbz	x0, 0x4200 <__ZN4Game4ImplC2Ev+0xdc>
    41fc: 94000000     	bl	0x41fc <__ZN4Game4ImplC2Ev+0xd8>
    4200: f9002f16     	str	x22, [x24, #88]
    4204: 52800508     	mov	w8, #40
    4208: 290a2319     	stp	w25, w8, [x24, #80]
    420c: aa1503e0     	mov	x0, x21
    4210: 52808001     	mov	w1, #1024
    4214: 94000000     	bl	0x4214 <__ZN4Game4ImplC2Ev+0xf0>
    4218: b940c708     	ldr	w8, [x24, #196]
    421c: 710ffd1f     	cmp	w8, #1023
    4220: 5400054c     	b.gt	0x42c8 <__ZN4Game4ImplC2Ev+0x1a4>
    4224: 52820000     	mov	w0, #4096
    4228: 94000000     	bl	0x4228 <__ZN4Game4ImplC2Ev+0x104>
    422c: aa0003f6     	mov	x22, x0
    4230: b940c319     	ldr	w25, [x24, #192]
    4234: f9406700     	ldr	x0, [x24, #200]
    4238: 7100073f     	cmp	w25, #1
    423c: 540003ab     	b.lt	0x42b0 <__ZN4Game4ImplC2Ev+0x18c>
    4240: d2800008     	mov	x8, #0
    4244: 7100433f     	cmp	w25, #16
    4248: 54000203     	b.lo	0x4288 <__ZN4Game4ImplC2Ev+0x164>
    424c: cb0002c9     	sub	x9, x22, x0
    4250: f101013f     	cmp	x9, #64
    4254: 540001a3     	b.lo	0x4288 <__ZN4Game4ImplC2Ev+0x164>
    4258: 927c6f28     	and	x8, x25, #0xfffffff0
    425c: 91008009     	add	x9, x0, #32
    4260: 910082ca     	add	x10, x22, #32
    4264: aa0803eb     	mov	x11, x8
    4268: ad7f0520     	ldp	q0, q1, [x9, #-32]
    426c: acc20d22     	ldp	q2, q3, [x9], #64
    4270: ad3f0540     	stp	q0, q1, [x10, #-32]
    4274: ac820d42     	stp	q2, q3, [x10], #64
    4278: f100416b     	subs	x11, x11, #16
    427c: 54ffff61     	b.ne	0x4268 <__ZN4Game4ImplC2Ev+0x144>
    4280: eb19011f     	cmp	x8, x25
    4284: 54000120     	b.eq	0x42a8 <__ZN4Game4ImplC2Ev+0x184>
    4288: cb080329     	sub	x9, x25, x8
    428c: d37ef50a     	lsl	x10, x8, #2
    4290: 8b0a0008     	add	x8, x0, x10
    4294: 8b0a02ca     	add	x10, x22, x10
    4298: b840450b     	ldr	w11, [x8], #4
    429c: b800454b     	str	w11, [x10], #4
    42a0: f1000529     	subs	x9, x9, #1
    42a4: 54ffffa1     	b.ne	0x4298 <__ZN4Game4ImplC2Ev+0x174>
    42a8: b900c31f     	str	wzr, [x24, #192]
    42ac: 14000003     	b	0x42b8 <__ZN4Game4ImplC2Ev+0x194>
    42b0: b900c31f     	str	wzr, [x24, #192]
    42b4: b4000040     	cbz	x0, 0x42bc <__ZN4Game4ImplC2Ev+0x198>
    42b8: 94000000     	bl	0x42b8 <__ZN4Game4ImplC2Ev+0x194>
    42bc: f9006716     	str	x22, [x24, #200]
    42c0: 52808008     	mov	w8, #1024
    42c4: 29182319     	stp	w25, w8, [x24, #192]
    42c8: b940d708     	ldr	w8, [x24, #212]
    42cc: 71009d1f     	cmp	w8, #39
    42d0: 5400034c     	b.gt	0x4338 <__ZN4Game4ImplC2Ev+0x214>
    42d4: 52806400     	mov	w0, #800
    42d8: 94000000     	bl	0x42d8 <__ZN4Game4ImplC2Ev+0x1b4>
    42dc: aa0003f6     	mov	x22, x0
    42e0: b940d319     	ldr	w25, [x24, #208]
    42e4: f9406f00     	ldr	x0, [x24, #216]
    42e8: 7100073f     	cmp	w25, #1
    42ec: 540001ab     	b.lt	0x4320 <__ZN4Game4ImplC2Ev+0x1fc>
    42f0: aa1603e8     	mov	x8, x22
    42f4: aa0003e9     	mov	x9, x0
    42f8: aa1903ea     	mov	x10, x25
    42fc: 3dc00120     	ldr	q0, [x9]
    4300: b940112b     	ldr	w11, [x9, #16]
    4304: b900110b     	str	w11, [x8, #16]
    4308: 3c814500     	str	q0, [x8], #20
    430c: 91005129     	add	x9, x9, #20
    4310: f100054a     	subs	x10, x10, #1
    4314: 54ffff41     	b.ne	0x42fc <__ZN4Game4ImplC2Ev+0x1d8>
    4318: b900d31f     	str	wzr, [x24, #208]
    431c: 14000003     	b	0x4328 <__ZN4Game4ImplC2Ev+0x204>
    4320: b900d31f     	str	wzr, [x24, #208]
    4324: b4000040     	cbz	x0, 0x432c <__ZN4Game4ImplC2Ev+0x208>
    4328: 94000000     	bl	0x4328 <__ZN4Game4ImplC2Ev+0x204>
    432c: f9006f16     	str	x22, [x24, #216]
    4330: 52800508     	mov	w8, #40
    4334: 291a2319     	stp	w25, w8, [x24, #208]
    4338: b940e708     	ldr	w8, [x24, #228]
    433c: 71009d1f     	cmp	w8, #39
    4340: 5400034c     	b.gt	0x43a8 <__ZN4Game4ImplC2Ev+0x284>
    4344: 52806400     	mov	w0, #800
    4348: 94000000     	bl	0x4348 <__ZN4Game4ImplC2Ev+0x224>
    434c: aa0003f6     	mov	x22, x0
    4350: b940e319     	ldr	w25, [x24, #224]
    4354: f9407700     	ldr	x0, [x24, #232]
    4358: 7100073f     	cmp	w25, #1
    435c: 540001ab     	b.lt	0x4390 <__ZN4Game4ImplC2Ev+0x26c>
    4360: aa1603e8     	mov	x8, x22
    4364: aa0003e9     	mov	x9, x0
    4368: aa1903ea     	mov	x10, x25
    436c: 3dc00120     	ldr	q0, [x9]
    4370: b940112b     	ldr	w11, [x9, #16]
    4374: b900110b     	str	w11, [x8, #16]
    4378: 3c814500     	str	q0, [x8], #20
    437c: 91005129     	add	x9, x9, #20
    4380: f100054a     	subs	x10, x10, #1
    4384: 54ffff41     	b.ne	0x436c <__ZN4Game4ImplC2Ev+0x248>
    4388: b900e31f     	str	wzr, [x24, #224]
    438c: 14000003     	b	0x4398 <__ZN4Game4ImplC2Ev+0x274>
    4390: b900e31f     	str	wzr, [x24, #224]
    4394: b4000040     	cbz	x0, 0x439c <__ZN4Game4ImplC2Ev+0x278>
    4398: 94000000     	bl	0x4398 <__ZN4Game4ImplC2Ev+0x274>
    439c: f9007716     	str	x22, [x24, #232]
    43a0: 52800508     	mov	w8, #40
    43a4: 291c2319     	stp	w25, w8, [x24, #224]
    43a8: b940f708     	ldr	w8, [x24, #244]
    43ac: 71009d1f     	cmp	w8, #39
    43b0: 5400034c     	b.gt	0x4418 <__ZN4Game4ImplC2Ev+0x2f4>
    43b4: 52806400     	mov	w0, #800
    43b8: 94000000     	bl	0x43b8 <__ZN4Game4ImplC2Ev+0x294>
    43bc: aa0003f6     	mov	x22, x0
    43c0: b940f319     	ldr	w25, [x24, #240]
    43c4: f9407f00     	ldr	x0, [x24, #248]
    43c8: 7100073f     	cmp	w25, #1
    43cc: 540001ab     	b.lt	0x4400 <__ZN4Game4ImplC2Ev+0x2dc>
    43d0: aa1603e8     	mov	x8, x22
    43d4: aa0003e9     	mov	x9, x0
    43d8: aa1903ea     	mov	x10, x25
    43dc: 3dc00120     	ldr	q0, [x9]
    43e0: b940112b     	ldr	w11, [x9, #16]
    43e4: b900110b     	str	w11, [x8, #16]
    43e8: 3c814500     	str	q0, [x8], #20
    43ec: 91005129     	add	x9, x9, #20
    43f0: f100054a     	subs	x10, x10, #1
    43f4: 54ffff41     	b.ne	0x43dc <__ZN4Game4ImplC2Ev+0x2b8>
    43f8: b900f31f     	str	wzr, [x24, #240]
    43fc: 14000003     	b	0x4408 <__ZN4Game4ImplC2Ev+0x2e4>
    4400: b900f31f     	str	wzr, [x24, #240]
    4404: b4000040     	cbz	x0, 0x440c <__ZN4Game4ImplC2Ev+0x2e8>
    4408: 94000000     	bl	0x4408 <__ZN4Game4ImplC2Ev+0x2e4>
    440c: f9007f16     	str	x22, [x24, #248]
    4410: 52800508     	mov	w8, #40
    4414: 291e2319     	stp	w25, w8, [x24, #240]
    4418: b9410708     	ldr	w8, [x24, #260]
    441c: 71009d1f     	cmp	w8, #39
    4420: 5400036c     	b.gt	0x448c <__ZN4Game4ImplC2Ev+0x368>
    4424: 52806400     	mov	w0, #800
    4428: 94000000     	bl	0x4428 <__ZN4Game4ImplC2Ev+0x304>
    442c: aa0003f6     	mov	x22, x0
    4430: b9410319     	ldr	w25, [x24, #256]
    4434: f9408700     	ldr	x0, [x24, #264]
    4438: 7100073f     	cmp	w25, #1
    443c: 540001ab     	b.lt	0x4470 <__ZN4Game4ImplC2Ev+0x34c>
    4440: aa1603e8     	mov	x8, x22
    4444: aa0003e9     	mov	x9, x0
    4448: aa1903ea     	mov	x10, x25
    444c: 3dc00120     	ldr	q0, [x9]
    4450: b940112b     	ldr	w11, [x9, #16]
    4454: b900110b     	str	w11, [x8, #16]
    4458: 3c814500     	str	q0, [x8], #20
    445c: 91005129     	add	x9, x9, #20
    4460: f100054a     	subs	x10, x10, #1
    4464: 54ffff41     	b.ne	0x444c <__ZN4Game4ImplC2Ev+0x328>
    4468: b901031f     	str	wzr, [x24, #256]
    446c: 14000003     	b	0x4478 <__ZN4Game4ImplC2Ev+0x354>
    4470: b901031f     	str	wzr, [x24, #256]
    4474: b4000040     	cbz	x0, 0x447c <__ZN4Game4ImplC2Ev+0x358>
    4478: 94000000     	bl	0x4478 <__ZN4Game4ImplC2Ev+0x354>
    447c: f9008716     	str	x22, [x24, #264]
    4480: 52800508     	mov	w8, #40
    4484: b9010708     	str	w8, [x24, #260]
    4488: b9010319     	str	w25, [x24, #256]
    448c: b940b708     	ldr	w8, [x24, #180]
    4490: 7107fd1f     	cmp	w8, #511
    4494: 5400034c     	b.gt	0x44fc <__ZN4Game4ImplC2Ev+0x3d8>
    4498: 52830000     	mov	w0, #6144
    449c: 94000000     	bl	0x449c <__ZN4Game4ImplC2Ev+0x378>
    44a0: aa0003f6     	mov	x22, x0
    44a4: b940b319     	ldr	w25, [x24, #176]
    44a8: f9405f00     	ldr	x0, [x24, #184]
    44ac: 7100073f     	cmp	w25, #1
    44b0: 540001ab     	b.lt	0x44e4 <__ZN4Game4ImplC2Ev+0x3c0>
    44b4: aa1603e8     	mov	x8, x22
    44b8: aa0003e9     	mov	x9, x0
    44bc: aa1903ea     	mov	x10, x25
    44c0: f940012b     	ldr	x11, [x9]
    44c4: b940092c     	ldr	w12, [x9, #8]
    44c8: b900090c     	str	w12, [x8, #8]
    44cc: f800c50b     	str	x11, [x8], #12
    44d0: 91003129     	add	x9, x9, #12
    44d4: f100054a     	subs	x10, x10, #1
    44d8: 54ffff41     	b.ne	0x44c0 <__ZN4Game4ImplC2Ev+0x39c>
    44dc: b900b31f     	str	wzr, [x24, #176]
    44e0: 14000003     	b	0x44ec <__ZN4Game4ImplC2Ev+0x3c8>
    44e4: b900b31f     	str	wzr, [x24, #176]
    44e8: b4000040     	cbz	x0, 0x44f0 <__ZN4Game4ImplC2Ev+0x3cc>
    44ec: 94000000     	bl	0x44ec <__ZN4Game4ImplC2Ev+0x3c8>
    44f0: f9005f16     	str	x22, [x24, #184]
    44f4: 52804008     	mov	w8, #512
    44f8: 29162319     	stp	w25, w8, [x24, #176]
    44fc: b940a708     	ldr	w8, [x24, #164]
    4500: 7107fd1f     	cmp	w8, #511
    4504: 5400034c     	b.gt	0x456c <__ZN4Game4ImplC2Ev+0x448>
    4508: 52860000     	mov	w0, #12288
    450c: 94000000     	bl	0x450c <__ZN4Game4ImplC2Ev+0x3e8>
    4510: aa0003f6     	mov	x22, x0
    4514: b940a319     	ldr	w25, [x24, #160]
    4518: f9405700     	ldr	x0, [x24, #168]
    451c: 7100073f     	cmp	w25, #1
    4520: 540001ab     	b.lt	0x4554 <__ZN4Game4ImplC2Ev+0x430>
    4524: aa1603e8     	mov	x8, x22
    4528: aa0003e9     	mov	x9, x0
    452c: aa1903ea     	mov	x10, x25
    4530: 3dc00120     	ldr	q0, [x9]
    4534: f940092b     	ldr	x11, [x9, #16]
    4538: f900090b     	str	x11, [x8, #16]
    453c: 3c818500     	str	q0, [x8], #24
    4540: 91006129     	add	x9, x9, #24
    4544: f100054a     	subs	x10, x10, #1
    4548: 54ffff41     	b.ne	0x4530 <__ZN4Game4ImplC2Ev+0x40c>
    454c: b900a31f     	str	wzr, [x24, #160]
    4550: 14000003     	b	0x455c <__ZN4Game4ImplC2Ev+0x438>
    4554: b900a31f     	str	wzr, [x24, #160]
    4558: b4000040     	cbz	x0, 0x4560 <__ZN4Game4ImplC2Ev+0x43c>
    455c: 94000000     	bl	0x455c <__ZN4Game4ImplC2Ev+0x438>
    4560: f9005716     	str	x22, [x24, #168]
    4564: 52804008     	mov	w8, #512
    4568: 29142319     	stp	w25, w8, [x24, #160]
    456c: 12800008     	mov	w8, #-1
    4570: b90002e8     	str	w8, [x23]
    4574: 94000000     	bl	0x4574 <__ZN4Game4ImplC2Ev+0x450>
    4578: 2d0086e0     	stp	s0, s1, [x23, #4]
    457c: aa1303e0     	mov	x0, x19
    4580: a9447bfd     	ldp	x29, x30, [sp, #64]
    4584: a9434ff4     	ldp	x20, x19, [sp, #48]
    4588: a94257f6     	ldp	x22, x21, [sp, #32]
    458c: a9415ff8     	ldp	x24, x23, [sp, #16]
    4590: a8c567fa     	ldp	x26, x25, [sp], #80
    4594: d65f03c0     	ret
    4598: 14000001     	b	0x459c <__ZN4Game4ImplC2Ev+0x478>
    459c: aa0003f6     	mov	x22, x0
    45a0: 5296ab08     	mov	w8, #46424
    45a4: 8b080277     	add	x23, x19, x8
    45a8: aa1803e8     	mov	x8, x24
    45ac: b901931f     	str	wzr, [x24, #400]
    45b0: f940cf00     	ldr	x0, [x24, #408]
    45b4: b4000040     	cbz	x0, 0x45bc <__ZN4Game4ImplC2Ev+0x498>
    45b8: 94000000     	bl	0x45b8 <__ZN4Game4ImplC2Ev+0x494>
    45bc: aa1803f9     	mov	x25, x24
    45c0: b901971f     	str	wzr, [x24, #404]
    45c4: aa1703e0     	mov	x0, x23
    45c8: 94000000     	bl	0x45c8 <__ZN4Game4ImplC2Ev+0x4a4>
    45cc: b901431f     	str	wzr, [x24, #320]
    45d0: f940a700     	ldr	x0, [x24, #328]
    45d4: b50009c0     	cbnz	x0, 0x470c <__ZN4Game4ImplC2Ev+0x5e8>
    45d8: aa1803e8     	mov	x8, x24
    45dc: b901471f     	str	wzr, [x24, #324]
    45e0: b901331f     	str	wzr, [x24, #304]
    45e4: f9409f00     	ldr	x0, [x24, #312]
    45e8: b50009e0     	cbnz	x0, 0x4724 <__ZN4Game4ImplC2Ev+0x600>
    45ec: aa1803e8     	mov	x8, x24
    45f0: b901371f     	str	wzr, [x24, #308]
    45f4: b901231f     	str	wzr, [x24, #288]
    45f8: f9409700     	ldr	x0, [x24, #296]
    45fc: b5000a00     	cbnz	x0, 0x473c <__ZN4Game4ImplC2Ev+0x618>
    4600: aa1803e8     	mov	x8, x24
    4604: b901271f     	str	wzr, [x24, #292]
    4608: b901131f     	str	wzr, [x24, #272]
    460c: f9408f00     	ldr	x0, [x24, #280]
    4610: b5000a20     	cbnz	x0, 0x4754 <__ZN4Game4ImplC2Ev+0x630>
    4614: aa1803e8     	mov	x8, x24
    4618: b901171f     	str	wzr, [x24, #276]
    461c: b901031f     	str	wzr, [x24, #256]
    4620: f9408700     	ldr	x0, [x24, #264]
    4624: b5000a40     	cbnz	x0, 0x476c <__ZN4Game4ImplC2Ev+0x648>
    4628: aa1803e8     	mov	x8, x24
    462c: b901071f     	str	wzr, [x24, #260]
    4630: b900f31f     	str	wzr, [x24, #240]
    4634: f9407f00     	ldr	x0, [x24, #248]
    4638: b5000a60     	cbnz	x0, 0x4784 <__ZN4Game4ImplC2Ev+0x660>
    463c: aa1803e8     	mov	x8, x24
    4640: b900f71f     	str	wzr, [x24, #244]
    4644: b900e31f     	str	wzr, [x24, #224]
    4648: f9407700     	ldr	x0, [x24, #232]
    464c: b5000a80     	cbnz	x0, 0x479c <__ZN4Game4ImplC2Ev+0x678>
    4650: aa1803e8     	mov	x8, x24
    4654: b900e71f     	str	wzr, [x24, #228]
    4658: b900d31f     	str	wzr, [x24, #208]
    465c: f9406f00     	ldr	x0, [x24, #216]
    4660: b5000aa0     	cbnz	x0, 0x47b4 <__ZN4Game4ImplC2Ev+0x690>
    4664: aa1803e8     	mov	x8, x24
    4668: b900d71f     	str	wzr, [x24, #212]
    466c: b900c31f     	str	wzr, [x24, #192]
    4670: f9406700     	ldr	x0, [x24, #200]
    4674: b5000ac0     	cbnz	x0, 0x47cc <__ZN4Game4ImplC2Ev+0x6a8>
    4678: aa1803e8     	mov	x8, x24
    467c: b900c71f     	str	wzr, [x24, #196]
    4680: b900b31f     	str	wzr, [x24, #176]
    4684: f9405f00     	ldr	x0, [x24, #184]
    4688: b5000ae0     	cbnz	x0, 0x47e4 <__ZN4Game4ImplC2Ev+0x6c0>
    468c: aa1803e8     	mov	x8, x24
    4690: b900b71f     	str	wzr, [x24, #180]
    4694: b900a31f     	str	wzr, [x24, #160]
    4698: f9405700     	ldr	x0, [x24, #168]
    469c: b4000040     	cbz	x0, 0x46a4 <__ZN4Game4ImplC2Ev+0x580>
    46a0: 94000000     	bl	0x46a0 <__ZN4Game4ImplC2Ev+0x57c>
    46a4: aa1803f7     	mov	x23, x24
    46a8: b900a71f     	str	wzr, [x24, #164]
    46ac: aa1503e0     	mov	x0, x21
    46b0: 94000000     	bl	0x46b0 <__ZN4Game4ImplC2Ev+0x58c>
    46b4: b900531f     	str	wzr, [x24, #80]
    46b8: f9402f00     	ldr	x0, [x24, #88]
    46bc: b4000040     	cbz	x0, 0x46c4 <__ZN4Game4ImplC2Ev+0x5a0>
    46c0: 94000000     	bl	0x46c0 <__ZN4Game4ImplC2Ev+0x59c>
    46c4: aa1803f5     	mov	x21, x24
    46c8: b900571f     	str	wzr, [x24, #84]
    46cc: aa1403e0     	mov	x0, x20
    46d0: 94000000     	bl	0x46d0 <__ZN4Game4ImplC2Ev+0x5ac>
    46d4: b900031f     	str	wzr, [x24]
    46d8: f9400700     	ldr	x0, [x24, #8]
    46dc: b4000040     	cbz	x0, 0x46e4 <__ZN4Game4ImplC2Ev+0x5c0>
    46e0: 94000000     	bl	0x46e0 <__ZN4Game4ImplC2Ev+0x5bc>
    46e4: b900071f     	str	wzr, [x24, #4]
    46e8: 52966808     	mov	w8, #45888
    46ec: 8b080260     	add	x0, x19, x8
    46f0: 92967ff3     	mov	x19, #-46080
    46f4: 94000000     	bl	0x46f4 <__ZN4Game4ImplC2Ev+0x5d0>
    46f8: d1030000     	sub	x0, x0, #192
    46fc: b1030273     	adds	x19, x19, #192
    4700: 54ffffa1     	b.ne	0x46f4 <__ZN4Game4ImplC2Ev+0x5d0>
    4704: aa1603e0     	mov	x0, x22
    4708: 94000000     	bl	0x4708 <__ZN4Game4ImplC2Ev+0x5e4>
    470c: 94000000     	bl	0x470c <__ZN4Game4ImplC2Ev+0x5e8>
    4710: aa1803e8     	mov	x8, x24
    4714: b901471f     	str	wzr, [x24, #324]
    4718: b901331f     	str	wzr, [x24, #304]
    471c: f9409f00     	ldr	x0, [x24, #312]
    4720: b4fff660     	cbz	x0, 0x45ec <__ZN4Game4ImplC2Ev+0x4c8>
    4724: 94000000     	bl	0x4724 <__ZN4Game4ImplC2Ev+0x600>
    4728: aa1803e8     	mov	x8, x24
    472c: b901371f     	str	wzr, [x24, #308]
    4730: b901231f     	str	wzr, [x24, #288]
    4734: f9409700     	ldr	x0, [x24, #296]
    4738: b4fff640     	cbz	x0, 0x4600 <__ZN4Game4ImplC2Ev+0x4dc>
    473c: 94000000     	bl	0x473c <__ZN4Game4ImplC2Ev+0x618>
    4740: aa1803e8     	mov	x8, x24
    4744: b901271f     	str	wzr, [x24, #292]
    4748: b901131f     	str	wzr, [x24, #272]
    474c: f9408f00     	ldr	x0, [x24, #280]
    4750: b4fff620     	cbz	x0, 0x4614 <__ZN4Game4ImplC2Ev+0x4f0>
    4754: 94000000     	bl	0x4754 <__ZN4Game4ImplC2Ev+0x630>
    4758: aa1803e8     	mov	x8, x24
    475c: b901171f     	str	wzr, [x24, #276]
    4760: b901031f     	str	wzr, [x24, #256]
    4764: f9408700     	ldr	x0, [x24, #264]
    4768: b4fff600     	cbz	x0, 0x4628 <__ZN4Game4ImplC2Ev+0x504>
    476c: 94000000     	bl	0x476c <__ZN4Game4ImplC2Ev+0x648>
    4770: aa1803e8     	mov	x8, x24
    4774: b901071f     	str	wzr, [x24, #260]
    4778: b900f31f     	str	wzr, [x24, #240]
    477c: f9407f00     	ldr	x0, [x24, #248]
    4780: b4fff5e0     	cbz	x0, 0x463c <__ZN4Game4ImplC2Ev+0x518>
    4784: 94000000     	bl	0x4784 <__ZN4Game4ImplC2Ev+0x660>
    4788: aa1803e8     	mov	x8, x24
    478c: b900f71f     	str	wzr, [x24, #244]
    4790: b900e31f     	str	wzr, [x24, #224]
    4794: f9407700     	ldr	x0, [x24, #232]
    4798: b4fff5c0     	cbz	x0, 0x4650 <__ZN4Game4ImplC2Ev+0x52c>
    479c: 94000000     	bl	0x479c <__ZN4Game4ImplC2Ev+0x678>
    47a0: aa1803e8     	mov	x8, x24
    47a4: b900e71f     	str	wzr, [x24, #228]
    47a8: b900d31f     	str	wzr, [x24, #208]
    47ac: f9406f00     	ldr	x0, [x24, #216]
    47b0: b4fff5a0     	cbz	x0, 0x4664 <__ZN4Game4ImplC2Ev+0x540>
    47b4: 94000000     	bl	0x47b4 <__ZN4Game4ImplC2Ev+0x690>
    47b8: aa1803e8     	mov	x8, x24
    47bc: b900d71f     	str	wzr, [x24, #212]
    47c0: b900c31f     	str	wzr, [x24, #192]
    47c4: f9406700     	ldr	x0, [x24, #200]
    47c8: b4fff580     	cbz	x0, 0x4678 <__ZN4Game4ImplC2Ev+0x554>
    47cc: 94000000     	bl	0x47cc <__ZN4Game4ImplC2Ev+0x6a8>
    47d0: aa1803e8     	mov	x8, x24
    47d4: b900c71f     	str	wzr, [x24, #196]
    47d8: b900b31f     	str	wzr, [x24, #176]
    47dc: f9405f00     	ldr	x0, [x24, #184]
    47e0: b4fff560     	cbz	x0, 0x468c <__ZN4Game4ImplC2Ev+0x568>
    47e4: 94000000     	bl	0x47e4 <__ZN4Game4ImplC2Ev+0x6c0>
    47e8: aa1803e8     	mov	x8, x24
    47ec: b900b71f     	str	wzr, [x24, #180]
    47f0: b900a31f     	str	wzr, [x24, #160]
    47f4: f9405700     	ldr	x0, [x24, #168]
    47f8: b5fff540     	cbnz	x0, 0x46a0 <__ZN4Game4ImplC2Ev+0x57c>
    47fc: 17ffffaa     	b	0x46a4 <__ZN4Game4ImplC2Ev+0x580>

0000000000004800 <__ZN11PointMassesD1Ev>:
    4800: a9be4ff4     	stp	x20, x19, [sp, #-32]!
    4804: a9017bfd     	stp	x29, x30, [sp, #16]
    4808: 910043fd     	add	x29, sp, #16
    480c: aa0003f3     	mov	x19, x0
    4810: b900301f     	str	wzr, [x0, #48]
    4814: f9401c00     	ldr	x0, [x0, #56]
    4818: b4000040     	cbz	x0, 0x4820 <__ZN11PointMassesD1Ev+0x20>
    481c: 94000000     	bl	0x481c <__ZN11PointMassesD1Ev+0x1c>
    4820: b900367f     	str	wzr, [x19, #52]
    4824: b900227f     	str	wzr, [x19, #32]
    4828: f9401660     	ldr	x0, [x19, #40]
    482c: b4000040     	cbz	x0, 0x4834 <__ZN11PointMassesD1Ev+0x34>
    4830: 94000000     	bl	0x4830 <__ZN11PointMassesD1Ev+0x30>
    4834: b900267f     	str	wzr, [x19, #36]
    4838: b900127f     	str	wzr, [x19, #16]
    483c: f9400e60     	ldr	x0, [x19, #24]
    4840: b4000040     	cbz	x0, 0x4848 <__ZN11PointMassesD1Ev+0x48>
    4844: 94000000     	bl	0x4844 <__ZN11PointMassesD1Ev+0x44>
    4848: b900167f     	str	wzr, [x19, #20]
    484c: b900027f     	str	wzr, [x19]
    4850: f9400660     	ldr	x0, [x19, #8]
    4854: b4000040     	cbz	x0, 0x485c <__ZN11PointMassesD1Ev+0x5c>
    4858: 94000000     	bl	0x4858 <__ZN11PointMassesD1Ev+0x58>
    485c: b900067f     	str	wzr, [x19, #4]
    4860: aa1303e0     	mov	x0, x19
    4864: a9417bfd     	ldp	x29, x30, [sp, #16]
    4868: a8c24ff4     	ldp	x20, x19, [sp], #32
    486c: d65f03c0     	ret

0000000000004870 <__ZN15HistoricalStateD2Ev>:
    4870: a9be4ff4     	stp	x20, x19, [sp, #-32]!
    4874: a9017bfd     	stp	x29, x30, [sp, #16]
    4878: 910043fd     	add	x29, sp, #16
    487c: aa0003f3     	mov	x19, x0
    4880: b900b01f     	str	wzr, [x0, #176]
    4884: f9405c00     	ldr	x0, [x0, #184]
    4888: b4000040     	cbz	x0, 0x4890 <__ZN15HistoricalStateD2Ev+0x20>
    488c: 94000000     	bl	0x488c <__ZN15HistoricalStateD2Ev+0x1c>
    4890: b900b67f     	str	wzr, [x19, #180]
    4894: b900a27f     	str	wzr, [x19, #160]
    4898: f9405660     	ldr	x0, [x19, #168]
    489c: b4000040     	cbz	x0, 0x48a4 <__ZN15HistoricalStateD2Ev+0x34>
    48a0: 94000000     	bl	0x48a0 <__ZN15HistoricalStateD2Ev+0x30>
    48a4: b900a67f     	str	wzr, [x19, #164]
    48a8: b900927f     	str	wzr, [x19, #144]
    48ac: f9404e60     	ldr	x0, [x19, #152]
    48b0: b4000040     	cbz	x0, 0x48b8 <__ZN15HistoricalStateD2Ev+0x48>
    48b4: 94000000     	bl	0x48b4 <__ZN15HistoricalStateD2Ev+0x44>
    48b8: b900967f     	str	wzr, [x19, #148]
    48bc: b900827f     	str	wzr, [x19, #128]
    48c0: f9404660     	ldr	x0, [x19, #136]
    48c4: b4000040     	cbz	x0, 0x48cc <__ZN15HistoricalStateD2Ev+0x5c>
    48c8: 94000000     	bl	0x48c8 <__ZN15HistoricalStateD2Ev+0x58>
    48cc: b900867f     	str	wzr, [x19, #132]
    48d0: b900727f     	str	wzr, [x19, #112]
    48d4: f9403e60     	ldr	x0, [x19, #120]
    48d8: b4000040     	cbz	x0, 0x48e0 <__ZN15HistoricalStateD2Ev+0x70>
    48dc: 94000000     	bl	0x48dc <__ZN15HistoricalStateD2Ev+0x6c>
    48e0: b900767f     	str	wzr, [x19, #116]
    48e4: b900627f     	str	wzr, [x19, #96]
    48e8: f9403660     	ldr	x0, [x19, #104]
    48ec: b4000040     	cbz	x0, 0x48f4 <__ZN15HistoricalStateD2Ev+0x84>
    48f0: 94000000     	bl	0x48f0 <__ZN15HistoricalStateD2Ev+0x80>
    48f4: b900667f     	str	wzr, [x19, #100]
    48f8: b900527f     	str	wzr, [x19, #80]
    48fc: f9402e60     	ldr	x0, [x19, #88]
    4900: b4000040     	cbz	x0, 0x4908 <__ZN15HistoricalStateD2Ev+0x98>
    4904: 94000000     	bl	0x4904 <__ZN15HistoricalStateD2Ev+0x94>
    4908: b900567f     	str	wzr, [x19, #84]
    490c: b900427f     	str	wzr, [x19, #64]
    4910: f9402660     	ldr	x0, [x19, #72]
    4914: b4000040     	cbz	x0, 0x491c <__ZN15HistoricalStateD2Ev+0xac>
    4918: 94000000     	bl	0x4918 <__ZN15HistoricalStateD2Ev+0xa8>
    491c: b900467f     	str	wzr, [x19, #68]
    4920: b900327f     	str	wzr, [x19, #48]
    4924: f9401e60     	ldr	x0, [x19, #56]
    4928: b4000040     	cbz	x0, 0x4930 <__ZN15HistoricalStateD2Ev+0xc0>
    492c: 94000000     	bl	0x492c <__ZN15HistoricalStateD2Ev+0xbc>
    4930: b900367f     	str	wzr, [x19, #52]
    4934: b900227f     	str	wzr, [x19, #32]
    4938: f9401660     	ldr	x0, [x19, #40]
    493c: b4000040     	cbz	x0, 0x4944 <__ZN15HistoricalStateD2Ev+0xd4>
    4940: 94000000     	bl	0x4940 <__ZN15HistoricalStateD2Ev+0xd0>
    4944: b900267f     	str	wzr, [x19, #36]
    4948: b900127f     	str	wzr, [x19, #16]
    494c: f9400e60     	ldr	x0, [x19, #24]
    4950: b4000040     	cbz	x0, 0x4958 <__ZN15HistoricalStateD2Ev+0xe8>
    4954: 94000000     	bl	0x4954 <__ZN15HistoricalStateD2Ev+0xe4>
    4958: b900167f     	str	wzr, [x19, #20]
    495c: b900027f     	str	wzr, [x19]
    4960: f9400660     	ldr	x0, [x19, #8]
    4964: b4000040     	cbz	x0, 0x496c <__ZN15HistoricalStateD2Ev+0xfc>
    4968: 94000000     	bl	0x4968 <__ZN15HistoricalStateD2Ev+0xf8>
    496c: b900067f     	str	wzr, [x19, #4]
    4970: aa1303e0     	mov	x0, x19
    4974: a9417bfd     	ldp	x29, x30, [sp, #16]
    4978: a8c24ff4     	ldp	x20, x19, [sp], #32
    497c: d65f03c0     	ret

0000000000004980 <__ZN4Game4ImplD2Ev>:
    4980: a9bd57f6     	stp	x22, x21, [sp, #-48]!
    4984: a9014ff4     	stp	x20, x19, [sp, #16]
    4988: a9027bfd     	stp	x29, x30, [sp, #32]
    498c: 910083fd     	add	x29, sp, #32
    4990: aa0003f3     	mov	x19, x0
    4994: 5296ab08     	mov	w8, #46424
    4998: 8b080015     	add	x21, x0, x8
    499c: b90042bf     	str	wzr, [x21, #64]
    49a0: f94026a0     	ldr	x0, [x21, #72]
    49a4: b4000040     	cbz	x0, 0x49ac <__ZN4Game4ImplD2Ev+0x2c>
    49a8: 94000000     	bl	0x49a8 <__ZN4Game4ImplD2Ev+0x28>
    49ac: 52968114     	mov	w20, #46088
    49b0: b90046bf     	str	wzr, [x21, #68]
    49b4: b90032bf     	str	wzr, [x21, #48]
    49b8: f9401ea0     	ldr	x0, [x21, #56]
    49bc: b4000040     	cbz	x0, 0x49c4 <__ZN4Game4ImplD2Ev+0x44>
    49c0: 94000000     	bl	0x49c0 <__ZN4Game4ImplD2Ev+0x40>
    49c4: b90036bf     	str	wzr, [x21, #52]
    49c8: b90022bf     	str	wzr, [x21, #32]
    49cc: f94016a0     	ldr	x0, [x21, #40]
    49d0: b4000040     	cbz	x0, 0x49d8 <__ZN4Game4ImplD2Ev+0x58>
    49d4: 94000000     	bl	0x49d4 <__ZN4Game4ImplD2Ev+0x54>
    49d8: 8b140274     	add	x20, x19, x20
    49dc: b90026bf     	str	wzr, [x21, #36]
    49e0: b90012bf     	str	wzr, [x21, #16]
    49e4: f9400ea0     	ldr	x0, [x21, #24]
    49e8: b4000040     	cbz	x0, 0x49f0 <__ZN4Game4ImplD2Ev+0x70>
    49ec: 94000000     	bl	0x49ec <__ZN4Game4ImplD2Ev+0x6c>
    49f0: b90016bf     	str	wzr, [x21, #20]
    49f4: b90002bf     	str	wzr, [x21]
    49f8: f94006a0     	ldr	x0, [x21, #8]
    49fc: b4000040     	cbz	x0, 0x4a04 <__ZN4Game4ImplD2Ev+0x84>
    4a00: 94000000     	bl	0x4a00 <__ZN4Game4ImplD2Ev+0x80>
    4a04: b90006bf     	str	wzr, [x21, #4]
    4a08: b901429f     	str	wzr, [x20, #320]
    4a0c: f940a680     	ldr	x0, [x20, #328]
    4a10: b4000040     	cbz	x0, 0x4a18 <__ZN4Game4ImplD2Ev+0x98>
    4a14: 94000000     	bl	0x4a14 <__ZN4Game4ImplD2Ev+0x94>
    4a18: b901469f     	str	wzr, [x20, #324]
    4a1c: b901329f     	str	wzr, [x20, #304]
    4a20: f9409e80     	ldr	x0, [x20, #312]
    4a24: b4000040     	cbz	x0, 0x4a2c <__ZN4Game4ImplD2Ev+0xac>
    4a28: 94000000     	bl	0x4a28 <__ZN4Game4ImplD2Ev+0xa8>
    4a2c: b901369f     	str	wzr, [x20, #308]
    4a30: b901229f     	str	wzr, [x20, #288]
    4a34: f9409680     	ldr	x0, [x20, #296]
    4a38: b4000040     	cbz	x0, 0x4a40 <__ZN4Game4ImplD2Ev+0xc0>
    4a3c: 94000000     	bl	0x4a3c <__ZN4Game4ImplD2Ev+0xbc>
    4a40: b901269f     	str	wzr, [x20, #292]
    4a44: b901129f     	str	wzr, [x20, #272]
    4a48: f9408e80     	ldr	x0, [x20, #280]
    4a4c: b4000040     	cbz	x0, 0x4a54 <__ZN4Game4ImplD2Ev+0xd4>
    4a50: 94000000     	bl	0x4a50 <__ZN4Game4ImplD2Ev+0xd0>
    4a54: b901169f     	str	wzr, [x20, #276]
    4a58: b901029f     	str	wzr, [x20, #256]
    4a5c: f9408680     	ldr	x0, [x20, #264]
    4a60: b4000040     	cbz	x0, 0x4a68 <__ZN4Game4ImplD2Ev+0xe8>
    4a64: 94000000     	bl	0x4a64 <__ZN4Game4ImplD2Ev+0xe4>
    4a68: b901069f     	str	wzr, [x20, #260]
    4a6c: b900f29f     	str	wzr, [x20, #240]
    4a70: f9407e80     	ldr	x0, [x20, #248]
    4a74: b4000040     	cbz	x0, 0x4a7c <__ZN4Game4ImplD2Ev+0xfc>
    4a78: 94000000     	bl	0x4a78 <__ZN4Game4ImplD2Ev+0xf8>
    4a7c: b900f69f     	str	wzr, [x20, #244]
    4a80: b900e29f     	str	wzr, [x20, #224]
    4a84: f9407680     	ldr	x0, [x20, #232]
    4a88: b4000040     	cbz	x0, 0x4a90 <__ZN4Game4ImplD2Ev+0x110>
    4a8c: 94000000     	bl	0x4a8c <__ZN4Game4ImplD2Ev+0x10c>
    4a90: b900e69f     	str	wzr, [x20, #228]
    4a94: b900d29f     	str	wzr, [x20, #208]
    4a98: f9406e80     	ldr	x0, [x20, #216]
    4a9c: b4000040     	cbz	x0, 0x4aa4 <__ZN4Game4ImplD2Ev+0x124>
    4aa0: 94000000     	bl	0x4aa0 <__ZN4Game4ImplD2Ev+0x120>
    4aa4: b900d69f     	str	wzr, [x20, #212]
    4aa8: b900c29f     	str	wzr, [x20, #192]
    4aac: f9406680     	ldr	x0, [x20, #200]
    4ab0: b4000040     	cbz	x0, 0x4ab8 <__ZN4Game4ImplD2Ev+0x138>
    4ab4: 94000000     	bl	0x4ab4 <__ZN4Game4ImplD2Ev+0x134>
    4ab8: b900c69f     	str	wzr, [x20, #196]
    4abc: b900b29f     	str	wzr, [x20, #176]
    4ac0: f9405e80     	ldr	x0, [x20, #184]
    4ac4: b4000040     	cbz	x0, 0x4acc <__ZN4Game4ImplD2Ev+0x14c>
    4ac8: 94000000     	bl	0x4ac8 <__ZN4Game4ImplD2Ev+0x148>
    4acc: b900b69f     	str	wzr, [x20, #180]
    4ad0: b900a29f     	str	wzr, [x20, #160]
    4ad4: f9405680     	ldr	x0, [x20, #168]
    4ad8: b4000040     	cbz	x0, 0x4ae0 <__ZN4Game4ImplD2Ev+0x160>
    4adc: 94000000     	bl	0x4adc <__ZN4Game4ImplD2Ev+0x15c>
    4ae0: b900a69f     	str	wzr, [x20, #164]
    4ae4: b900929f     	str	wzr, [x20, #144]
    4ae8: f9404e80     	ldr	x0, [x20, #152]
    4aec: b4000040     	cbz	x0, 0x4af4 <__ZN4Game4ImplD2Ev+0x174>
    4af0: 94000000     	bl	0x4af0 <__ZN4Game4ImplD2Ev+0x170>
    4af4: b900969f     	str	wzr, [x20, #148]
    4af8: b900829f     	str	wzr, [x20, #128]
    4afc: f9404680     	ldr	x0, [x20, #136]
    4b00: b4000040     	cbz	x0, 0x4b08 <__ZN4Game4ImplD2Ev+0x188>
    4b04: 94000000     	bl	0x4b04 <__ZN4Game4ImplD2Ev+0x184>
    4b08: b900869f     	str	wzr, [x20, #132]
    4b0c: b900729f     	str	wzr, [x20, #112]
    4b10: f9403e80     	ldr	x0, [x20, #120]
    4b14: b4000040     	cbz	x0, 0x4b1c <__ZN4Game4ImplD2Ev+0x19c>
    4b18: 94000000     	bl	0x4b18 <__ZN4Game4ImplD2Ev+0x198>
    4b1c: b900769f     	str	wzr, [x20, #116]
    4b20: b900629f     	str	wzr, [x20, #96]
    4b24: f9403680     	ldr	x0, [x20, #104]
    4b28: b4000040     	cbz	x0, 0x4b30 <__ZN4Game4ImplD2Ev+0x1b0>
    4b2c: 94000000     	bl	0x4b2c <__ZN4Game4ImplD2Ev+0x1ac>
    4b30: b900669f     	str	wzr, [x20, #100]
    4b34: b900529f     	str	wzr, [x20, #80]
    4b38: f9402e80     	ldr	x0, [x20, #88]
    4b3c: b4000040     	cbz	x0, 0x4b44 <__ZN4Game4ImplD2Ev+0x1c4>
    4b40: 94000000     	bl	0x4b40 <__ZN4Game4ImplD2Ev+0x1c0>
    4b44: b900569f     	str	wzr, [x20, #84]
    4b48: b900429f     	str	wzr, [x20, #64]
    4b4c: f9402680     	ldr	x0, [x20, #72]
    4b50: b4000040     	cbz	x0, 0x4b58 <__ZN4Game4ImplD2Ev+0x1d8>
    4b54: 94000000     	bl	0x4b54 <__ZN4Game4ImplD2Ev+0x1d4>
    4b58: b900469f     	str	wzr, [x20, #68]
    4b5c: b900329f     	str	wzr, [x20, #48]
    4b60: f9401e80     	ldr	x0, [x20, #56]
    4b64: b4000040     	cbz	x0, 0x4b6c <__ZN4Game4ImplD2Ev+0x1ec>
    4b68: 94000000     	bl	0x4b68 <__ZN4Game4ImplD2Ev+0x1e8>
    4b6c: b900369f     	str	wzr, [x20, #52]
    4b70: b900229f     	str	wzr, [x20, #32]
    4b74: f9401680     	ldr	x0, [x20, #40]
    4b78: b4000040     	cbz	x0, 0x4b80 <__ZN4Game4ImplD2Ev+0x200>
    4b7c: 94000000     	bl	0x4b7c <__ZN4Game4ImplD2Ev+0x1fc>
    4b80: b900269f     	str	wzr, [x20, #36]
    4b84: b900129f     	str	wzr, [x20, #16]
    4b88: f9400e80     	ldr	x0, [x20, #24]
    4b8c: b4000040     	cbz	x0, 0x4b94 <__ZN4Game4ImplD2Ev+0x214>
    4b90: 94000000     	bl	0x4b90 <__ZN4Game4ImplD2Ev+0x210>
    4b94: b900169f     	str	wzr, [x20, #20]
    4b98: b900029f     	str	wzr, [x20]
    4b9c: f9400680     	ldr	x0, [x20, #8]
    4ba0: b4000040     	cbz	x0, 0x4ba8 <__ZN4Game4ImplD2Ev+0x228>
    4ba4: 94000000     	bl	0x4ba4 <__ZN4Game4ImplD2Ev+0x224>
    4ba8: d2800015     	mov	x21, #0
    4bac: b900069f     	str	wzr, [x20, #4]
    4bb0: 52966808     	mov	w8, #45888
    4bb4: 8b080274     	add	x20, x19, x8
    4bb8: 92967ff6     	mov	x22, #-46080
    4bbc: 8b150280     	add	x0, x20, x21
    4bc0: 94000000     	bl	0x4bc0 <__ZN4Game4ImplD2Ev+0x240>
    4bc4: d10302b5     	sub	x21, x21, #192
    4bc8: eb1602bf     	cmp	x21, x22
    4bcc: 54ffff81     	b.ne	0x4bbc <__ZN4Game4ImplD2Ev+0x23c>
    4bd0: aa1303e0     	mov	x0, x19
    4bd4: a9427bfd     	ldp	x29, x30, [sp, #32]
    4bd8: a9414ff4     	ldp	x20, x19, [sp, #16]
    4bdc: a8c357f6     	ldp	x22, x21, [sp], #48
    4be0: d65f03c0     	ret
