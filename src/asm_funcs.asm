global list_search_asm
global list_search_asm_preload
global list_search_asm_opt
extern strncmp

section .text

;=======================================================================================
; Entry: rdi: list_t* lst (pointer to a list for search)
;        rsi: size_t key (hash to compare)
; Exit:  rbx: pointer to a list element with the hash == key or
;             NULL if not such element met
; Dstr:  rax, rbx, rcx, rdx, rdi, r8,  xmm0, xmm1, xmm2, xmm3
;=======================================================================================

;struct hashtable_elem_t {
;    char   buffer[32];
;    size_t hash;
;    size_t counter;
;};

;typedef hashtable_elem_t lst_data_t;
;typedef int32_t lst_index_t;

;struct list_t
;{
;    lst_data_t*  data;
;    lst_index_t* next;
;    lst_index_t* prev;
;
;    lst_index_t free;
;    lst_index_t capacity;
;};

list_search_asm:
;----------------------------------------------------------------------------------------------------------
        ; rdi = list_t* lst
        ; rsi = char key[STRING_SIZE] (STRING_SIZE=16)
        push    r12                      ; save callee-saved regs
        push    rbx

        mov     r12, rsi                 ; r12 = key

        mov     rcx, [rdi + 32]          ; rcx = list.free
        mov     rbx, [rdi]               ; rbx = list.data
        shl     rcx, 5                   ; rcx = rcx * sizeof(list.data[0])
        add     rcx, rbx                 ; rcx = list.data[list.free]

.search_loop:
        cmp     rbx, rcx
        jae     .return_null             ; if (curr >= end) return NULL

        ; prepare args for strncmp
        mov     rdi, rbx                 ; current elem
        mov     rsi, r12                 ; key
        mov     edx, 16                  ; comparison size
        call    strncmp

        test    eax, eax                 ; check match
        je      .return_found

        add     rbx, 64                  ; ++curr_elem_ptr
        jmp     .search_loop

.return_found:
        mov     rax, rbx                 ; return pointer with matching string
        jmp     .cleanup

.return_null:
        xor     eax, eax                 ; return NULL

.cleanup:
        pop     rbx                      ; restore callee-saved regs
        pop     r12
        ret


list_search_asm_opt:
        vpxor ymm0, ymm0, ymm0
        vpxor ymm1, ymm1, ymm1
        vpxor ymm2, ymm2, ymm2

        ; rdi = list_t* lst
        ; rsi = char key[STRING_SIZE]

        vmovdqu   ymm1, [rsi]             ; key for search

        mov     rcx, [rdi + 32]           ; rcx = list.free
        mov     rax, [rdi]                ; rax = list.data
        shl     rcx, 5                    ; rcx = rcx * sizeof(list.data[0])
        add     rcx, rax                  ; rcx = list.data[list.free]

.search_loop:
        ; rax - curr_elem_ptr
        ; rcx - last_elem_ptr
        cmp     rax, rcx
        ja     .return_null                ; if (curr_elem_ptr >= last_elem_ptr) { return NULL }

        vmovdqa   ymm0, [rax]              ; curr_elem_ptr -> buffer
        vpcmpeqb  ymm2, ymm0, ymm1
        vpmovmskb edx, ymm2

        cmp     edx, 0xFFFFFFFF
        je      .return

        add     rax, 64                    ; ++curr_elem_ptr
        jmp     .search_loop

.return_null:
        xor     eax, eax
        vzeroupper

.return:
        ret

list_search_asm_preload:

        ; rdi = list_t* lst
        ; rsi = char key[STRING_SIZE]

        vmovdqu   xmm1, [rsi]             ; key for search

        mov     rcx, [rdi + 32]           ; rcx = list.free
        mov     rax, [rdi]                ; rax = list.data
        shl     rcx, 5                    ; rcx = rcx * sizeof(list.data[0])
        add     rcx, rax                  ; rcx = list.data[list.free]

.search_loop:
        ; rax - curr_elem_ptr
        ; rcx - last_elem_ptr
        cmp     rax, rcx
        ja     .return_null              ; if (curr_elem_ptr >= last_elem_ptr) { return NULL }

        vmovdqa   ymm0, [rax]              ; str32cmp
        vpcmpeqb  ymm2, ymm0, ymm1         ;
        vpmovmskb edx, ymm2

        cmp     edx, 0xFFFF
        je      .return

        add     rax, 48                  ; ++curr_elem_ptr
        jmp     .search_loop

.return_null:
        xor     eax, eax

.return:
        ret


section .note.GNU-stack noalloc noexec nowrite progbits
