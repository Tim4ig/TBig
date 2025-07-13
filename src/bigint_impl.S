.intel_syntax noprefix
.section .text


//
//  uint64_t tbig_add(const uint64_t *a, const uint64_t *b, uint64_t *out, size_t limbs);
//
//  Performs addition of two multi-precision integers a and b with carry propagation
//
//  Precondition (caller must ensure):
//      limbs > 0
//      limbs % 4 == 0
//
//  Arguments:
//      rdi -> a                        in*
//      rsi -> b                        in*
//      rdx -> out                      out*
//      rcx -> limbs                    in
//
//  Returns:
//      rax -> carry (0 or 1)
//
.global tbig_add
.type tbig_add, @function
.p2align 4, 0x90
tbig_add:
    shr     rcx, 2                  //  limbs /= 4

.L_add_loop:
    mov     rax, [rdi]              //  rax = a[0]
    adc     rax, [rsi]              //  rax += b[0] + carry
    mov     [rdx], rax              //  out[0] = rax

    mov     rax, [rdi + 8]          //  rax = a[1]
    adc     rax, [rsi + 8]          //  rax += b[1] + carry
    mov     [rdx + 8], rax          //  out[1] = rax

    mov     rax, [rdi + 16]         //  rax = a[2]
    adc     rax, [rsi + 16]         //  rax += b[2] + carry
    mov     [rdx + 16], rax         //  out[2] = rax

    mov     rax, [rdi + 24]         //  rax = a[3]
    adc     rax, [rsi + 24]         //  rax += b[3] + carry
    mov     [rdx + 24], rax         //  out[3] = rax

    lea     rdi, [rdi + 32]         //  a += 4
    lea     rsi, [rsi + 32]         //  b += 4
    lea     rdx, [rdx + 32]         //  out += 4

    dec     rcx                     //  rcx -= 1
    jnz     .L_add_loop             //  rcx != 0 -> .L_add_loop

.L_add_end:
    mov     rax, 0                  //  rax = 0
    setc    al                      //  rax = carry
    ret                             //  return rax
.size   bigint_add, .-bigint_add


//
//  uint64_t tbig_sub(const uint64_t *a, const uint64_t *b, uint64_t *out, size_t limbs);
//
//  Performs substraction of two multi-precision integers a and b with carry propagation
//
//  Precondition (caller must ensure):
//      limbs > 0
//      limbs % 4 == 0
//
//  Arguments:
//      rdi -> a                        in*
//      rsi -> b                        in*
//      rdx -> out                      out*
//      rcx -> limbs                    in
//
//  Returns:
//      rax -> borrow (0 or 1)
//
.global tbig_sub
.type   tbig_sub, @function
.p2align 4, 0x90
tbig_sub:
    shr     rcx, 2                  //  limb /= 4

.L_sub_loop:
    mov     rax, [rdi]              //  rax = a[0]
    sbb     rax, [rsi]              //  rax -= b[0] - borrow
    mov     [rdx], rax              //  out[0] = rax

    mov     rax, [rdi + 8]          //  rax = a[0]
    sbb     rax, [rsi + 8]          //  rax -= b[0] - borrow
    mov     [rdx + 8], rax          //  out[0] = rax

    mov     rax, [rdi + 16]         //  rax = a[0]
    sbb     rax, [rsi + 16]         //  rax -= b[0] - borrow
    mov     [rdx + 16], rax         //  out[0] = rax

    mov     rax, [rdi + 24]         //  rax = a[0]
    sbb     rax, [rsi + 24]         //  rax -= b[0] - borrow
    mov     [rdx + 24], rax         //  out[0] = rax

    lea     rdi, [rdi + 32]         //  a += 4
    lea     rsi, [rsi + 32]         //  b += 4
    lea     rdx, [rdx + 32]         //  out += 4

    dec     rcx                     //  rcx -= 1
    jne     .L_sub_loop             //  rcx != 0 -> .L_sub_loop

.L_sub_end:
    mov     rax, 0                  // rax = 0
    setc    al                      // rax = borrow
    ret                             // return rax
.size   bigint_sub, .-bigint_sub


