# Test 04 — Factorielle
#
# Calcule 5! = 5 × 4 × 3 × 2 × 1 = 120.
# Sans instruction `mul`, on multiplie via une boucle d'addition imbriquée.
#
# Algorithme :
#   result = 1
#   pour i de n descendant à 2 :
#       result = result × i  (= ajouter result, i fois, à un acc)
#
# Exerce : boucles imbriquées, mv, addi négatif, blt.

.section .text
.global _start

_start:
    li   a0, 1        # result = 1
    li   t0, 5        # i = n = 5

outer_loop:
    li   t1, 2
    blt  t0, t1, end  # si i < 2, fin (factorielle complète)

    # === Multiply step : result = result × i ===
    # On accumule dans a0 = (current result) × i
    # via addition de 'old result' (sauvé dans t2), i fois.
    mv   t2, a0       # t2 = ancien result (constante pour cette multiplication)
    li   a0, 0        # a0 = nouvel accumulateur
    mv   t3, t0       # t3 = compteur d'additions = i

inner_loop:
    beq  t3, zero, inner_done
    add  a0, a0, t2   # acc += old_result
    addi t3, t3, -1
    j    inner_loop

inner_done:
    addi t0, t0, -1   # i--
    j    outer_loop

end:
    ecall
