; - - - - - - - - - - sector 1 - - - - - - - - - -

	cli
	xor	sp, sp
	mov	ax, 07C0h
	mov	ss, ax
	sti

	mov	ds, ax
	mov	es, ax
	cld
	jmp	07C0h:@f
@@:
	mov	ax, 0204h
	mov	cx, 0002h
	mov	dh, ch
	mov	bx, buffer
	int	13h
	jc	error

	cli
	lgdt	[GDTpointer]
	lidt	[IDTpointer]
	mov	eax, cr0
	or	al, 1
	mov	cr0, eax

	xor	ax, ax
	mov	ds, ax
	mov	es, ax
	mov	ax, 8*2
	mov	ss, ax
	jmp	8:@f
@@:	sti

;	int	13		; (1)

;	mov	ax, 8		; (2)
;	mov	ds, ax		;

;	jmp	0x1000:0	; (3)

	mov	cx, str_ok_len
	mov	si, str_ok
	call	print_string
	cli
	hlt

str_ok	db	'OK'
str_ok_len = $ - str_ok

str_err	db	'Error'
str_err_len = $ - str_err
error:
	mov	ax, 1301h
	mov	bh, 0
	mov	bl, 04
	mov	bp, str_err
	mov	cx, str_err_len
	xor	dx, dx
	int	10h
	cli
	hlt


str_all	db	'Hello, INT!'
str_all_len = $ - str_all
int_all:
	push	cx
	push	si
	mov	cx, str_all_len
	mov	si, str_all
	call	print_string
	pop	si
	pop	cx
	iret

str_13	db	'Hello, INT13!'
str_13_len = $ - str_13
int_13:
	push	cx
	push	si
	mov	cx, str_13_len
	mov	si, str_13
	call	print_string
	pop	si

;	push	bp		; (2) (3)
;	mov	bp, sp		; (2) (3)
;	add	bp, 2+2+2	; (2) (3)
;	mov	cx, [bp]	; (2) (3)
;	add	cx, 2		; (2)
;	add	cx, 5		;     (3)
;	mov	[bp], cx	; (2) (3)
;	pop	bp		; (2) (3)

	pop	cx
;	add	sp, 2		; (2) (3)
	iret
;	cli
;	hlt

pos	dw	0
print_string:
	push	ax
	push	cx
	push	si
	push	di
	push	ds
	push	es
	mov	ax, 8*3
	mov	ds, ax
	mov	ax, 8*4
	mov	es, ax
	mov	di, [cs:pos]
	add	[cs:pos], 80*2
	mov	al, 02h
@@:
	movsb
	stosb
	loop	@b
	pop	es
	pop	ds
	pop	di
	pop	si
	pop	cx
	pop	ax
	ret


GDTpointer:
	dw	GDTlen - 1
	dd	7C00h + GDT
IDTpointer:
	dw	256*8
	dd	7C00h + buffer

GDT:
	db	8 dup(0x00)
	db	0x00,0x02,  0x00,0x7C,0x00,  10011000b, 00000000b, 0x00		; code
	db	0x00,0x0A,  0x00,0x7C,0x00,  10010110b, 00000000b, 0x00		; stack
	db	0x00,0x02,  0x00,0x7C,0x00,  10010010b, 00000000b, 0x00		; data
	db	0x00,0x10,  0x00,0x80,0x0B,  10010010b, 00000000b, 0x00		; video
GDTlen = $ - GDT

buffer:
	db	510 - ($ - $$) dup(0)
	db	0x55, 0xAA

; - - - - - - - - - - sectors 2, 3, 4, 5 - - - - - - - - - -

IDT:
	repeat 13
		dw	int_all, 8
		db	0, 10000110b
		dw	0
	end repeat

	dw	int_13, 8
	db	0, 10000110b
	dw	0

	repeat 256 - (13+1)
		dw	int_all, 8
		db	0, 10000110b
		dw	0
	end repeat