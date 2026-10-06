# 🚶 Zigbee Smart Crosswalk — Wireless Pedestrian Signaling Network

A 3-node Zigbee (XBee S2C) wireless network coordinating traffic lights and pedestrian crossing requests, with an LED matrix "walking person" animation.

> 📎 Based on the project report *"Passage Piéton Intelligent — Synchronisé sans fil (XBee 802.15.4)"* — Master ISOC, Faculté des Sciences de Meknès.

## Overview

**Topology:** Star network — 1 coordinator + 2 end devices, PAN ID `2026`, XBee S2C modules in **Transparent (AT) mode** (not API mode) to avoid binary packet-decoding overhead on memory-constrained microcontrollers.

```
                 [Nœud 2: Arduino Uno]
                  Bouton + Feu + Matrice LED
                        |  Zigbee (XBee S2C)
                        |
   [Nœud 1: ESP32-S3 — Coordinateur]  ──Zigbee── [Nœud 3: Arduino Uno]
   Machine à états (busy lock)                     Bouton + Feu
```

<img width="863" height="471" alt="image" src="https://github.com/user-attachments/assets/4597a998-bbe4-408f-ae7c-84c31bf2eb6d" />



## Hardware & pin mapping

| Node | Role | Board | Key pins |
|---|---|---|---|
| **Nœud 1** | Coordinateur Zigbee, machine à états | ESP32-S3 | UART1: RX=GPIO18, TX=GPIO17 (9600 baud) |
| **Nœud 2** | Intersection principale (bouton + feu + matrice piéton) | Arduino Uno | XBee (SoftwareSerial) D2/D3 · Bouton D4 · LEDs rouge/jaune/vert D5/D6/D7 · Matrice MAX7219 (DIN/CLK/CS) D10/D11/D12 |
| **Nœud 3** | Second point de passage (bouton + feu) | Arduino Uno | XBee (SoftwareSerial) D2/D3 · Bouton D4 · LEDs D5/D6/D7 |

## XCTU radio configuration

| Paramètre | Nœud 1 (Coordinateur) | Nœud 2/3 (Routeurs) |
|---|---|---|
| CE (Coordinator Enable) | 1 [Enabled] | 0 [Disabled] |
| AP (API Enable) | 0 [Transparent] | 0 [Transparent] |
| ID (PAN ID) | 2026 | 2026 |
| BD (Baud Rate) | 3 [9600] | 3 [9600] |
| SM (Sleep Mode) | 0 [No Sleep] | 0 [No Sleep] |

<img width="763" height="373" alt="image" src="https://github.com/user-attachments/assets/bb3a2689-d1e0-4d1a-a965-b592e03c4e10" />


## State machine (coordinator-driven, `busy` lock)

A software lock (`busy = true/false` in `noeud1.ino`) blocks any new pedestrian request while a crossing cycle is in progress:

1. **Repos** — feux verts, matrice STOP rouge. Attente d'un appui bouton (`'X'` du Nœud 2 ou `'Z'` du Nœud 3).
2. **Transition (2s)** — coordinateur envoie `'Y'` → feux jaunes.
3. **Traversée (7s)** — coordinateur envoie `'R'` → feux rouges, animation "bonhomme qui marche" sur la matrice du Nœud 2 (2 frames alternées toutes les 200ms, non-bloquant).
4. **Restauration (3s de sécurité)** — coordinateur envoie `'G'` → feux verts, matrice éteinte, `busy = false`.

**Cycle total : 12 secondes**, avec anti-rebond logiciel (300ms) sur chaque bouton poussoir.

## Repository structure

```
zigbee-smart-crosswalk/
├── noeud1/
│   └── noeud1.ino      # Coordinateur ESP32-S3 — machine à états + verrou busy
├── noeud2/
│   └── noeud2.ino      # Arduino Uno — bouton + feu + matrice LED MAX7219 (LedControl lib)
├── noeud3/
│   └── noeud3.ino      # Arduino Uno — bouton + feu (sans matrice)
├── images/              # Add your report screenshots here
└── README.md
```

## Validated results

- Latence réseau Zigbee mesurée : **< 50 ms** en mode Transparent
- Synchronisation fiable entre affichage matriciel et feux de signalisation sur les 3 nœuds

## What I'd improve next

- Add Zigbee payload authentication/encryption (currently plaintext single-character commands — a natural next step given my embedded security specialization)
- Add a request timeout/retry mechanism in case a button-press frame is lost
- Extend the star topology to a mesh for multiple intersections on one PAN

## Author

Aymane Ait Arab & Kawthar Derouich — M2 Intelligence et Sécurité des Objets Connectés, Faculté des Sciences de Meknès
