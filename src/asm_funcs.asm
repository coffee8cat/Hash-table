global list_search_asm
global list_search_asm_preload
global list_search_asm_opt
section .text

;=======================================================================================
; Entry: rdi: list_t* lst (pointer to a list for search)
;        rsi: size_t key (hash to compare)
; Exit:  rbx: pointer to a list element with the hash == key or
;             NULL if not such element met
; Dstr:  rax, rbx, rcx, rdx, rdi, r8,  xmm0, xmm1, xmm2, xmm3
;=======================================================================================

;struct hashtable_elem_t {
;    char   buffer[16];
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
        push rbx

        ; rdi is list_t* list
        mov     r8, qword [rdi+16]          ; rcx = & (list -> prev)
        movsx   rdx, dword [r8]             ; rdx = list -> prev[0]

        test    edx, edx                    ; if (prev(0) == 0) { end } - means empty list
        je      .return_null

        mov     rdi, qword [rdi]
        mov     rbx, rdx                    ; rbx = list + rdx * sizeof(list_t) <=> rbx = list.data[rdx]
        shl     rbx, 5                      ; sizeof(list_t) = 32
        add     rbx, rdi
        vmovdqu xmm0, [rbx]                 ; xmm0 = list.data[curr].buffer
        vmovdqu xmm1, [rsi]                 ; xmm1 = string
        jmp     .check_cond

.while_body:
        ; rdx = curr - index of current elem in list.data
        movsx   rdx, dword [r8 + rdx*4]      ; rdx = prev(rdx)
        test    edx, edx
        je      .return_null                ; if (prev(curr) == 0) ==> it was last element to check => end
        mov     rbx, rdx                    ; rbx = list.data[curr]
        shl     rbx, 5
        add     rbx, rdi
        vmovdqu xmm0, [rbx]                 ; xmm0 = list.data[curr].buffer

.check_cond:
        vpcmpeqb    xmm3, xmm0, xmm1        ; xmm0 = _mm_cmpeq_epi8(xmm0, xmm1)
        vpmovmskb   ecx, xmm3               ; ecx  = _mm_movemask_epi8(xmm0)
        cmp         ecx, 65535
        jne         .while_body

        mov     rax, rbx
        pop rbx
        ret                                 ; return list_data[curr]

.return_null:
        xor     eax, eax                    ; return false
        pop rbx
        ret


list_search_asm_opt:

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

        vmovdqu   xmm0, [rax]              ; curr_elem_ptr -> buffer
        vpcmpeqb  xmm2, xmm0, xmm1
        vpmovmskb edx, xmm2

        cmp     edx, 0xFFFF
        je      .return

        add     rax, 32                  ; ++curr_elem_ptr
        jmp     .search_loop

.return_null:
        xor     eax, eax

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

        vmovdqu   xmm0, [rax]              ; curr_elem_ptr -> buffer
        vpcmpeqb  xmm2, xmm0, xmm1
        vpmovmskb edx, xmm2

        cmp     edx, 0xFFFF
        je      .return

        add     rax, 32                  ; ++curr_elem_ptr
        jmp     .search_loop

.return_null:
        xor     eax, eax

.return:
        ret


section .note.GNU-stack noalloc noexec nowrite progbits
