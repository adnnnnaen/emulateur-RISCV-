# Test 01 — Le plus simple
#
# Pose la valeur 42 dans a0 et arrête l'émulateur.
# But : valider que `li`, `ecall` et le boot à 0x80000000 marchent.

.section .text
.global _start

_start:
    li   a0, 42       # a0 = 42 (charge constante immédiate)
    ecall             # arrête l'émulateur
