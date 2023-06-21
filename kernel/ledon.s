main:
    ldr x12, MailboxAddress
    ldr x13, [x12, #0x38]
    tst x13, #0x80000000
    b.ne main
    ldr x0, RequestBufferAddress
    add x0, x0, #0x8
    str x0, [x12, #0x20]
    hang:
        b hang
MailboxAddress:
    .int 0x3f00b880
    .int 0
RequestBufferAddress:
    .int 0x80030
    .int 0
RequestBuffer:
    .int 32
    .int 0
    .int 0x00038041
    .int 8
    .int 0
    .int 130
    .int 1
    .int 0