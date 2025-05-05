# sensors.c – Senzori de linie și procesare urmărire

Acest fișier se ocupă de citirea senzorilor cu multiplexare, convertirea valorilor analogice și calcularea comenzilor de mișcare în baza acestor date. Esențial pentru ca robotul să nu meargă ca o râmă beată.

---

## 🧠 Ce conține

### 🔁 `read_sensors()`
Citește 8 combinații de pini prin multiplexor. Pe scurt:

- Coduri 0,1,3,4,6 → sunt senzori digitali (0/1), se salvează în `sensor_data[]`
- Coduri 2,5,7 → sunt valori analogice, se salvează în `sensor_data16[]`
- Citirea se face cu ADC pe `ADC1->DR`
- Se face un delay mic după fiecare citire pentru stabilitate

Senzorii sunt multiplexerizați și activați pe GPIOA pinii A/B/C.

---

### 🧠 `line_process()`
Aceasta e inima "autonomiei" robotului. E funcția care:

- Citește datele de la senzori
- Decide încotro s-o ia robotul (PID-ish, dar în pași discreți)
- Generează o structură `DriveCommand` cu:
  - `cmd.mask` – direcția fiecărei roți (FWD/BACK)
  - `cmd.speeds[4]` – vitezele pentru fiecare roată

---

## 🔍 Algoritmi & Strategii

### 📏 Detecție de linie:
- Linia e detectată dacă senzorul respectiv returnează `0`
- Senzorii de pe margine (far_left, far_right) sunt incluși

### ⚠️ Cazuri speciale:
1. **Toți senzorii activi** → intersecție (CROSS), robotul se oprește
2. **Doar extremitățile active** → linia pierdută → STOP
3. **Nimic activ** → începe o căutare pe baza `last_active` (memorează unde era ultima linie)

### 🧠 Corecție pe direcție:
Se face pe baza unei pseudo-erori calculate:
- Ponderi de la -2 (stânga extrem) la +2 (dreapta extrem)
- Se adaptează viteza și sensul roților în funcție de distribuția senzorilor activi

### 💡 Masca finală:
Se generează folosind `directie_roata[]` și valorile FWD/BACK pentru fiecare roată.

---

## 💡 SetControlPins(uint8_t code)
Setează pinii A/B/C în funcție de codul dorit (0-7), pentru multiplexarea senzorilor. Un fel de selector de canal, ca-n televizoarele din 2001.

---

## 🚨 LED-uri de status
- `SetSensorRight(uint8_t)` și `SetSensorLeft(uint8_t)` controlează LED-uri care semnalează diverși pași sau erori.
- Sunt utile pentru debugging vizual (sau doar ca să pari că știi ce faci).

---

## 🛠️ Defines folosite (vezi `globals.h`)
- `BASE_SPEED`, `mid_adjust`, `small_adjust`
- `FWD`, `BACK`, `directie_roata[]`
- `sensor_data[8]`, `sensor_data16[3]`
- `CROSS` – flag pentru intersecție

---

## ⚠️ Observații importante
- Sistemul nu folosește un PID continuu, ci lucrează pe pași discreți de corecție.
- Are fallback logic dacă linia e complet pierdută (robotul nu rămâne etern pierdut ca tine în viață).
- `DriveCommand` e structura cheie pentru mișcare – o poți trimite mai departe prin I2C sau alt sistem de control.

---

> 📎 Dacă robotul o ia în direcția greșită, fie linia e mizerabilă, fie ai uitat să inversezi pinii, fie universul conspiră împotriva ta. Mai probabil, e codul tău.
