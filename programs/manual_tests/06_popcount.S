# Test 06 — Population count (compte des bits à 1)
#
# Calcule popcount(0xFF00FF00) = 16.
# 0xFF00FF00 = 1111_1111_0000_0000_1111_1111_0000_0000 → 8 + 8 = 16 bits à 1.
#
# Algorithme : décaler à droite et incrémenter si LSB == 1.
#
# Exerce : li avec constante 32 bits (lui+addi), andi (mask LSB), srli, boucle.

.section .text
.global _start

_start:
    li   t0, 0xFF00FF00   # valeur à analyser
    li   a0, 0            # compteur de bits
    li   t1, 32           # bits restants à examiner

loop:
    beq  t1, zero, end
    andi t2, t0, 1        # t2 = LSB (0 ou 1)
    add  a0, a0, t2       # compteur += LSB
    srli t0, t0, 1        # décale à droite (logique, zéro injecté)
    addi t1, t1, -1
    j    loop

end:
    ecall
