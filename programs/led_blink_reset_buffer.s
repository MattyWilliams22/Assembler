main:
    ldr w0, RequestOnAddress
    movk w5, #0x20
    str w5, [w0, #0x0]
    movk w5, #0x0
    str w5, [w0, #0x4]
    movk w5, #0x3, lsl #16
    movk w5, #0x8041
    str w5, [w0, #0x8]
    movk w5, #0x8
    str w5, [w0, #0xc]
    movk w5, #0x0
    str w5, [w0, #0x10]
    movk w5, #0x82
    str w5, [w0, #0x14]
    movk w5, #0x1
    str w5, [w0, #0x18]
    movk w5, #0x0
    str w5, [w0, #0x1c]
    add w0, w0, #0x8

    ldr x12, MailboxAddress
    write1:
        ldr x13, [x12, #0x38]
        tst x13, #0x80000000
        b.ne write1

    str x0, [x12, #0x20]

    ldr x12, MailboxAddress
    movk x20, #0x8
    read1:
        ldr x13, [x12, #0x38]
        tst x13, #0x40000000
        b.ne read1

        ldr x16, [x12]
        and x17, x16, #0b1111
        
        eor x19, x17, x20
        b.ne read1
    mov x20, x16

    movz x3, #0xffff, lsl #48
    wait1:
        sub x3, x3, #0x1
        cmp x3, #0x0
        b.ne wait1

    ldr w0, RequestOffAddress
    movk w5, #0x20
    str w5, [w0, #0x0]
    movk w5, #0x0
    str w5, [w0, #0x4]
    movk w5, #0x3, lsl #16
    movk w5, #0x8041
    str w5, [w0, #0x8]
    movk w5, #0x8
    str w5, [w0, #0xc]
    movk w5, #0x0
    str w5, [w0, #0x10]
    movk w5, #0x82
    str w5, [w0, #0x14]
    movk w5, #0x0
    str w5, [w0, #0x18]
    movk w5, #0x0
    str w5, [w0, #0x1c]
    add w0, w0, #0x8
    
    ldr x12, MailboxAddress
    write2:
        ldr x13, [x12, #0x38]
        tst x13, #0x80000000
        b.ne write2

    str x0, [x12, #0x20]

    movk x20, #0x8
    read2:
        ldr x13, [x12, #0x38]
        tst x13, #0x40000000
        b.ne read2

        ldr x16, [x12]
        and x17, x16, #0b1111
        
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
    .int 0x80150 
    .int 0

RequestOffAddress:
    .int 0x80170
    .int 0

nop
nop
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