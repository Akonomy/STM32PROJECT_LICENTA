# 📦 STM32 Motion Logic – Intersecții, Direcții și Parcări

> Cod sursă pentru gestionarea direcțiilor, parcărilor și deplasării robotului controlat de STM32.  
> Integrare cu Raspberry Pi prin USART și codificare de direcții pe 3 biți.

---

## 📁 Fișier: `motion.c`

### 🔄 `follow_next_direction()`
- Decide următoarea direcție a robotului:
  - Dacă `headTo` e activ (`0-7`): verifică dacă e sincronizat cu `global_directions[0]`.
    - Dacă da → face shift și execută.
    - Dacă nu → golește vectorul și execută oricum `headTo`.
  - Dacă `headTo` e inactiv (255), folosește prima valoare validă din `global_directions`, dacă există.
  - Dacă nu există direcție validă → direcția implicită este `4` (BACK).

---

### 🔁 `makeTurn(uint8_t direction_x)`
Execută comportamentul în funcție de direcția primită:

| Cod direcție | Descriere                          | Detalii                                                              |
|--------------|-------------------------------------|----------------------------------------------------------------------|
| `0`          | Stop complet                       | trimite STOPIE                                                       |
| `1`          | Înainte                             | `move_car(1, 4, viteza)`                                             |
| `2`          | Dreapta 45° + găsire linie          | `move_car(8, 5, viteza)` apoi caută linia cu `move_car(4,1,...)`     |
| `3`          | Stânga 45° + găsire linie           | `move_car(7, 5, viteza)` apoi caută linia cu `move_car(3,1,...)`     |
| `4`          | Întoarcere completă (90° dreapta)   | `move_car(8, 8, vitezaFR)`, caută linia, apoi un pas înainte         |
| `5`          | Parcare completă cu întoarcere      | Ca `4`, dar cu viteza mică + trecere în `mode = 5`                   |
| `6`          | Parcare „speranță” (doar STOP)      | Doar frânare și `mode = 6`                                           |
| `default`    | Eroare                              | Trimite STOP și `mode = 3` (semnal de „ceva a mers prost”)           |

---

### 🏎️ `move_car(uint8_t direction, uint8_t tick, uint16_t speed[4])`
- Trimite comenzi I2C către motor driver.
- Delay: 155ms pentru `tick == 1`, 100ms pentru restul.
- Dacă `direction == 0` sau `tick == 0` → STOP imediat.
- Validează parametrii (direcție max 20, tick max 30).
- Aprinde LED-urile de semnal (SetSensorLeft/Right) în funcție de stare.

---

## 📊 Alte detalii:

### ⚙️ Config:
- `global_directions[MAX_DIRECTIONS]` → setat din USART.
- `MAX_DIRECTIONS` este un `#define` configurabil (ex: `#define MAX_DIRECTIONS 11`).
- `headTo = 255` semnalează că nu este setată o destinație explicită.

---

## 📥 Integrare Raspberry Pi

- Raspberry trimite comenzi codificate.
- Comenzile de parcare vin cu direcții speciale (5 și 6).
- Răspunsurile se trimit prin USART, uneori cu semnalizări:
  - `0xFA` → succes
  - `0xDE, 0xAD` → ceva a mers prost

---

## 🧠 TODO / de reamintit

- Mode 5 și Mode 6 sunt ambele pentru parcare, se poate consolida logica.
- Verificare dacă toate `move_car()` au delay și `STOP` la final.
- Eventuală funcție `is_direction_queue_empty()` pentru debug elegant.
- Citirea FA de către Raspberry (verificat ok).

---

## 👨‍🔧 Autor: andre
Feb 2025 – STM32 embedded madness
