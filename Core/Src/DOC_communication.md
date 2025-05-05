# 📡 communication.c – Interfață I2C pentru STM32

Fișierul `communication.c` gestionează comunicația I2C dintre STM32 și un slave I2C (tipic: placa de control a motoarelor). Conține rutine de trimitere a pachetelor, citire de status și un mecanism de timeout. Toată poezia în C low-level, pentru când HAL-ul e prea pufos pentru tine.

---

## 📜 Funcții

| Funcție | Descriere |
|--------|-----------|
| `I2C_Send_Buffer(...)` | Trimite un buffer brut de date prin I2C. Blochează până la finalizarea transmisiei. |
| `I2C_Send_Packet(...)` | Construitor de pachete smart: masca + date PWM pentru canalele active. Automatizat pentru leneși. |
| `I2C_Read_Status(...)` | Citește un byte de la slave fără timeout. Ideal pentru interogări rapide. |
| `I2C_Read_WithTimeout(...)` | Versiunea serioasă: dacă slave-ul nu răspunde într-un timp definit, returnează 207. |

---

## 📦 Formatul pachetului `I2C_Send_Packet`

```text
| Byte | Descriere                     |
|------|-------------------------------|
| 0    | Mask High Byte (MSB)          |
| 1    | Mask Low Byte (LSB)           |
| 2    | PWM pentru primul canal activ (MSB) |
| 3    | PWM pentru primul canal activ (LSB) |
| ..   | Repetă pentru fiecare canal activ   |
```

> Valorile PWM lipsă sunt completate automat cu `2048`, pentru a simula decizia "meh, merge și așa".

---

## 🛑 Timeout

`I2C_Read_WithTimeout` este prietenul tău atunci când:
- Slave-ul a luat-o razna.
- Cablul e scos.
- Sau te-ai plictisit să aștepți etern în `RXNE`.

Returnează:
- `1` pentru succes.
- `207` pentru timeout (codul nostru personal de „bad vibes”).

---

## 🧠 Exemplu de utilizare

```c
uint16_t speeds[4] = {1500, 1600, 1700, 1800};
I2C_Send_Packet(0x10, 0x0F00, speeds, 4);  // Trimite 4 viteze la canalele 8–11
```

---

## ✅ Observații tehnice

- Masca este procesată pe 16 biți.
- Codificarea este **big-endian**.
- Se folosește `I2C_CR2_AUTOEND` pentru a evita grijile legate de STOP manual.
- Nu se folosește HAL pentru că… controlul e cool.
