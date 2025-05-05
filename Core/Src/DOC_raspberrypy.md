# raspberry_com.c – Interfață Raspberry Pi <-> STM32

Fișierul `raspberry_com.c` conține logica de procesare a comenzilor trimise de Raspberry Pi prin USART către STM32. Da, e un protocol simplu, dar făcut cu grijă (și nervi).

---

## 🎯 Funcționalități principale

### 🔌 `process_rasp_data(...)`
Routerul central al comenzilor. Primește pachetul despachetat și apelează una din funcțiile:

- `control_car()` – deplasare robot (direcție, tick, viteză)
- `control_servo()` – controlează unul din două servo-uri
- `request_data()` – trimite date de la senzori înapoi către Pi
- `save_next_cross_direction()` – setează următoarea direcție de întoarcere
- `set_mode()` – setează modul de funcționare al robotului
- `decode_and_save_directions()` – decriptează și salvează o secvență de direcții codificate
- `adjust_box()` – probabil lovește o cutie. Sau o aliniază. Tu știi.
- `case 7` – trimite o secvență de LED-uri (semnalizare de primire)
- `default` – ignoră ce nu recunoaște

---

### 🧠 `parse_and_process_data()`
Se ocupă de citirea datelor brute din bufferul USART:
- Caută pachetele valide (start: `0xAA`, end: `0xBB`)
- Suportă vector de lungime `1` sau `4`
- Odată extras, trimite mai departe către `process_rasp_data`

---

### 🚗 `control_car()`
Trimite comenzi I2C către motoare. Direcția e codificată (max 20), cu tick și vector de viteză.

---

### 🦾 `control_servo()`
Controlează două servo-uri:
- ID 9 / 181 → Servo 1
- ID 10 / 182 → Servo 2
Rezultat:
- `1` → succes
- `207` → timeout
- altceva → fail

Trimite feedback înapoi și aprinde LED-urile.

---

### 🧠 Alte funcții:
- `request_data()` – trimite valorile din `sensor_data[8]`, dar cu 2, 5 și 7 forțate la zero
- `decode_and_save_directions()` – transformă un payload criptat XOR în direcții (max 10, 3 biți fiecare)
- `save_next_cross_direction()` – setează `headTo`
- `set_mode()` – setează `mode`
- `adjust_box()` – trimite un semnal I2C (0x0800) probabil legat de mecanism de cutie (o cutie magică)

---

## 🧱 Formatul pachetului (USART)

-[ 0xAA | cmd_type | data1 | data2 | vector_len | vector... | 0xBB ]


- vector_len poate fi 1 sau 4
- dacă e 1, se repetă acel element pe toate cele 4 poziții

---

## 💡 Note utile

- `sensor_data[2/5/7]` NU se trimit — sunt valori analogice, nu binare
- `SetSensorLeft/Right()` sunt probabil LED-uri de status
- În cazul pachetelor invalide sau incomplete, `RingBuffer_Clear()` resetează tot
- E pregătit pentru extindere (dacă nu uiți să updatezi switch-ul)

---

## 🤔 TODO
- Adaugă o funcție de retry în caz de timeout la `request_data`
- Probabil ai putea înlocui LED-urile cu ceva mai informativ, dar de ce ai face viața ușoară?

---

> 🧠 Protip: Codul e ok. Protocolul e minimal. Dar dacă îl modifici la 2 noaptea, lasă un comentariu, ca să nu urli dimineață.


