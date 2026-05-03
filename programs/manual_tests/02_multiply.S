# Test 02 — Multiplication par addition itérative
#
# Calcule 5 × 7 = 35 en additionnant 7 cinq fois.
# RV32I de base n'a pas d'instruction `mul` (c'est l'extension M, hors scope).
#
# Exerce : addi, beq, add, j, ecall — boucle de base.

.section .text
.global _start

_start:
    li   t0, 5        # t0 = compteur (combien d'additions)
    li   t1, 7        # t1 = valeur à ajouter à chaque tour
    li   a0, 0        # a0 = accumulateur (résultat)

loop:
    beq  t0, zero, end  # si t0 == 0, sortir
    add  a0, a0, t1     # a0 += t1
    addi t0, t0, -1     # t0--
    j    loop           # retour

end:
    ecall
