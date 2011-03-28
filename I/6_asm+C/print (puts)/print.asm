format elf

extrn	_puts
public	print	as "_print"
print:
	push	ebp
	mov	ebp, esp
	mov	eax, [ebp+4+4]
	push	eax
	call	_puts
	add	sp, 4
	pop	ebp
	ret