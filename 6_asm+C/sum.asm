format	elf

public	sum as '_sum'
sum:
	push	ebp
	mov	ebp, esp
	mov	eax, [ebp+4+4]
	add	eax, [ebp+4+4+4]
	pop	ebp
	ret