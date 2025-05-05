# 🧠 main.c – Controlul central al aplicației STM32

Acest fișier reprezintă logica principală a firmware-ului. Aici se inițializează toate componentele hardware, se gestionează logica de funcționare prin stări (`mode`) și se comunică direct cu placa de control și Raspberry Pi.

---

## 🔄 Arhitectură principală

| Componentă     | Descriere                                 |
|----------------|--------------------------------------------|
| Init Hardware  | GPIO, I2C, ADC, UART, Timere               |
| Timer SysTick  | Folosit pentru delays și timeout-uri       |
| Mode handler   | Selectează comportamentul în funcție de `mode` |
| Communication  | Procesare date UART și răspunsuri I2C      |
| Line tracking  | Control complet prin `line_process()`      |
| Event-driven   | Răspunsuri automate la CROSS/intersecții   |

---

## ⚙️ Moduri funcționale (`mode`)

| `mode` | Comportament                             |
|--------|-------------------------------------------|
| 0      | Idle / standby                            |
| 1      | Follow line cu protecție anti-blocaj      |
| 3      | Fail-safe: trimite `0xDE 0xAD` pe UART     |
| 5      | Parcă robotul și trimite `0xFA`           |
| 6      | Confirmare poziție (parcare) – trimite `0xC8` |
| 7      | Blink debug pentru vectorul de direcții   |

---

## 🚧 Protecție anti-blocaj (`mode = 1`)

Dacă `DriveCommand` rămâne identic pentru mai multe cicluri:
- După 50 cicluri → trimite pachet cu mask `0x0009`
- După 100 cicluri → trimite `0x0006` și resetează contorul

Aceasta previne scenariile în care robotul rămâne „agățat” și nu mai progresează.

---

## 📡 Comunicare cu Raspberry Pi

### Recepție:
- `newDataFlag` este setat când s-a primit un pachet valid
- Pachetul este procesat prin `parse_and_process_data()`

### Trimiteri:
- Se folosesc `USART_Send_Byte()` pentru statusuri sau confirmări.

---

## 🛠️ Inițializări hardware

```c
MX_GPIO_Init();
MX_ADC_Init();
MX_I2C1_Init();
MX_TIM14_Init();
MX_TIM16_Init();
MX_USART1_UART_Init();
```

> Sfat: nu rula în `mode = 1` în producție. Acesta e un mod de urmarire linie, e setat de rasp

---

## 🧨 Error Handling

Funcția `Error_Handler()` blochează execuția dacă apare o eroare critică și dezactivează întreruperile. Practic: „Ai încurcat-o, stăm aici până rezolvi tu.”

---

## 📌 Alte detalii

- Structura `DriveCommand` este esențială pentru controlul motorului.
- În caz de intersecție (`CROSS`), se apelează `follow_next_direction()`.
- Debounce/anti-spam se face cu `avoid_spam` în unele moduri.

---

> Pentru extindere, adaugă noi moduri în `switch(mode)` și mapează-le la comenzi de la Raspberry Pi.
