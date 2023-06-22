main:
    ldr x12, MailboxAddress
    write1:
        ldr x13, [x12, #0x38]
        tst x13, #0x80000000
        b.ne write1

    ldr x0, RequestOnAddress
    add x0, x0, #0x8
    str x0, [x12, #0x20]

    ldr x12, MailboxAddress
    movk x20, #0x8
    read1:
        ldr x13, [x12, #0x38]
        tst x13, #0x40000000
        b.ne read1

        ldr x16, [x12]
        and x17, x16, #0xf
        
        eor x19, x17, x20
        b.ne read1
    mov x20, x16

    movz x3, #0xffff, lsl #48
    wait1:
        sub x3, x3, #0x1
        cmp x3, #0x0
        b.ne wait1

    ldr x12, MailboxAddress
    write2:
        ldr x13, [x12, #0x38]
        tst x13, #0x80000000
        b.ne write2

    ldr x0, RequestOffAddress
    add x0, x0, #0x8
    str x0, [x12, #0x20]

    movk x20, #0x8
    read2:
        ldr x13, [x12, #0x38]
        tst x13, #0x40000000
        b.ne read2

        ldr x16, [x12]
        and x17, x16, #0xf
        
        eor x19, x17, x20
        b.ne read2
    mov x20, x16

    movz x3, #0xffff, lsl #48
    wait2:
        sub x3, x3, #0x1
        cmp x3, #0x0
        b.ne wait2

    b main

MailboxAddress:
    .int 0x3f00b880 
    .int 0

RequestOnAddress:
    .int 0x800c0 
    .int 0

RequestOffAddress:
    .int 0x800e0
    .int 0

RequestOn:
    .int 32
    .int 0
    .int 0x00038041
    .int 8
    .int 0
    .int 130
    .int 1
    .int 0

RequestOff:
    .int 32
    .int 0
    .int 0x00038041
    .int 8
    .int 0
    .int 130
    .int 0
    .int 0