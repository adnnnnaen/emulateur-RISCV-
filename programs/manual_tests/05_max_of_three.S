# Test 05 — Maximum de trois nombres
#
# Calcule max(15, 7, 22) = 22.
#
# Pattern : max = a ; if (b > max) max = b ; if (c > max) max = c.
#
# Exerce : bge (signed >=), mv, structure if-then-else sans else.

.section .text
.global _start

_start:
    li   t0, 15       # a
    li   t1, 7        # b
    li   t2, 22       # c

    mv   a0, t0       # max = a

    bge  a0, t1, skip1   # si max >= b, on saute (rien à faire)
    mv   a0, t1          # sinon max = b

skip1:
    bge  a0, t2, skip2   # si max >= c, on saute
    mv   a0, t2          # sinon max = c

skip2:
    ecall
