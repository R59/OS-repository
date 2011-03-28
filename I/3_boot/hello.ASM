org	7C00h

; - - - - - - - - - - sector 1 - - - - - - - - - -

	cli
	xor	sp, sp
	mov	ss, sp
	sti

	mov	es, sp
	cld
	jmp	0:$+5


	mov	ax, 0201h
	mov	cx, 0002h
	mov	dh, ch
	mov	bx, buffer
	int	13h
	jc	_error

	mov	al, 0
	mov	cx, 512
	mov	di, buffer
	repnz	scasb

	neg	cx
	add	cx, 511

	mov	bl, 02
	mov	bp, buffer
	jmp	_write
_error:
	mov	bl, 04
	mov	bp, error
	mov	cx, buffer - error
_write:
	mov	ax, 1301h
	mov	bh, 0
	xor	dx, dx
	int	10h

	cli
	hlt

error	db	'Error'
buffer:
	db	510 - ($-$$) dup(0)
	db	0x55, 0xAA

; - - - - - - - - - - sector 2 - - - - - - - - - -

	db	'Hello from sector 2!', 0
	db	512*2 - ($-$$) dup(0)