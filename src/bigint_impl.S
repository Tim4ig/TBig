.intel_syntax noprefix
.section .text


//
//  uint64_t tbig_add(const uint64_t *a, const uint64_t *b, uint64_t *out, size_t limbs);
//
//  Performs addition of two multi-precision integers
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
.size   tbig_add, .-tbig_add


//
//  uint64_t tbig_sub(const uint64_t *a, const uint64_t *b, uint64_t *out, size_t limbs);
//
//  Performs substraction of two multi-precision integers
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
.size   tbig_sub, .-tbig_sub


//
//  void tbig_mul(const uint64_t *a, const uint64_t *b, uint64_t *out, size_t limbs);
//
//  Multi-precision schoolbook multiplicatio
//  Produces a 2×limbs-wide result into out
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
.global tbig_mul
.type   tbig_mul, @function
.p2align 4, 0x90
tbig_mul:
    push    rbx
    push    r12
    push    r13
    push    r14
    push    r15

    mov     r12, rdi                // r12 = a
    mov     r13, rsi                // r13 = b
    mov     r14, rdx                // r14 = out
    mov     r15, rcx                // r15 = limbs

    mov     rdi, rdx                // rdi = out
    shl     rcx, 1                  // rcx = limbs * 2
    xor     rax, rax                // rax = 0
    rep stosq                       // memset(out, 0, limbs*2*8)

    mov     rdi, r12
    mov     rsi, r13
    mov     rdx, r14
    mov     r8, r15

    xor     r9, r9

.L_mul_loop_i:
    cmp     r9, r8
    jae     .L_mul_end

    xor     r10, r10

.L_mul_loop_j:
    cmp     r10, r8
    jae     .L_mul_next_i

    mov     rax, [rdi + r9*8]
    mul     qword ptr [rsi + r10*8]

    mov     rcx, r9
    add     rcx, r10
    add     [rdx + rcx*8], rax

    inc     rcx
    adc     [rdx + rcx*8], rdx

    inc     r10
    jmp     .L_mul_loop_j

.L_mul_next_i:
    inc     r9
    jmp     .L_mul_loop_i

.L_mul_end:
    pop     r15
    pop     r14
    pop     r13
    pop     r12
    pop     rbx
    ret
.size tbig_mul, .-tbig_mul