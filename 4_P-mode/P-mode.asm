org	7C00h

; - - - - - - - - - - sector 1 - - - - - - - - - -
	cli
	xor	sp, sp
	mov	ss, sp
	sti

	mov	ds, sp
	mov	es, sp
	jmp	0:@f
@@:
	mov	ax, 0204h
	mov	cx, 0002h
	mov	dh, ch
	mov	bx, buffer
	int	13h
	jc	_error

	cli
	lgdt	[GDTpointer]
	lidt	[IDTpointer]
	mov	eax, cr0
	or	al, 1
	mov	cr0, eax

	mov	ax, 8*2
	mov	ss, ax
	jmp	8:@f
@@:	sti

;        int     13

;        mov     ax, 8
;        mov     ds, ax

;        jmp      0x1000:0

	mov	cx, ok_str_len
	mov	dx, ok_str
	call	print_string
	cli
	hlt

ok_str	db	'OK'
ok_str_len = $ - ok_str

error	db	'Error'
error_len = $ - error
_error:
	mov	ax, 1301h
	mov	bh, 0
	mov	bl, 04
	mov	bp, error
	mov	cx, error_len
	xor	dx, dx
	int	10h
	cli
	hlt


string	db	'Hello, INT!'
len = $ - string
int_all:
	push	cx
	push	dx
	mov	cx, len
	mov	dx, string
	call	print_string
	pop	dx
	pop	cx
	iret

str_13	db	'Hello, INT13!'
len_13 = $ - str_13
int_13:
	push	cx
	push	dx
	mov	cx, len_13
	mov	dx, str_13
	call	print_string
	pop	dx

;        push     bp
;        mov      bp, sp
;        add      bp, 2+4+2
;        mov      cx, [bp]
;        inc      cx
;        inc      cx
;        mov      [bp], cx
;        pop      bp

	pop	cx
;        add     sp, 4
	iret
;        cli
;        hlt

print_string:
	push	ax
	push	si
	push	di
	push	ds
	push	es
	mov	ax, 8*2
	mov	ds, ax
	mov	si, dx
	mov	ax, 8*3
	mov	es, ax
	xor	di, di
	mov	al, 02h
_str:
	movsb
	stosb
	loop	 _str
	pop	es
	pop	ds
	pop	di
	pop	si
	pop	ax
	ret


GDTpointer:
	dw	GDTlen - 1
	dd	0:GDT
IDTpointer:
	dw	256*8
	dd	0:buffer


GDT:
	db	8 dup(0)
	db	0xFF,0xFF,  0x00,0x00,0x00,  10011000b, 00000000b, 0x00
	db	0xFF,0xFF,  0x00,0x00,0x00,  10010010b, 00001111b, 0x00
	db	0x00,0x10,  0x00,0x80,0x0B,  10010010b, 00000000b, 0x00
GDTlen = $ - GDT

buffer:
	db	510 - ($ - $$) dup (0)
	db	0x55, 0xAA

; - - - - - - - - - - sectors 2, 3, 4 - - - - - - - - - -
IDT:
	repeat 13
	       dd 8:int_all
	       db 0, 10000110b
	       dw 0
	end repeat

	dd 8:int_13
	db 0, 10000110b
	dw 0

	repeat 256 - 14
		dd 8:int_all
		db 0, 10000110b
		dw 0
	end repeat