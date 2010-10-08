
	.section .header, "a"
	.arm

	.extern load_address
	.extern start

@--------------------------------------

	.word load_address
	.word start

@--------------------------------------

  .end

